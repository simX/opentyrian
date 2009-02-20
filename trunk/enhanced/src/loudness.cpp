/* vim: set noet:
 *
 * OpenTyrian Enhanced: A modern cross-platform port of Tyrian
 * Copyright (C) 2007-2009  The OpenTyrian Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "opentyr.h"
#include "loudness.h"

#include "fm_synth.h"
#include "lds_play.h"
#include "console/Console.h"
#include "console/cvar/CVar.h"
#include "console/CCmd.h"
#include "sndmast.h"
#include "nortsong.h"
#include "Filesystem.h"

#include "SDL_mixer.h"
#include <map>

/* SYN: These are externally accessible variables: */
JE_MusicType musicData;
bool repeated;
bool playing;

float music_vol_multiplier = 1.f;

/* SYN: These shouldn't be used outside this file. Hands off! */
SAMPLE_TYPE *channel_buffer[SFX_CHANNELS];
SAMPLE_TYPE *channel_pos[SFX_CHANNELS];
Uint32 channel_len[SFX_CHANNELS];
float channel_vol[SFX_CHANNELS];

const int freq = 11025 * OUTPUT_QUALITY;

bool music_playing = false;

static bool sound_initialized = false;

std::map<unsigned int, std::string> customMusicMappings;
Mix_Music *currentMusic = 0;
unsigned int currentMusicNum = 0;
bool oggPlaying = false;

static bool snd_enabled_callback( const bool& init )
{
	if (init)
	{
		if (!loadedSoundData)
		{
			JE_loadSndFile();
		}
		if (init_sound())
		{
			if (music_playing)
			{
				JE_loadSong(currentSong);
				JE_selectSong(1);
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return !deinit_sound();
	}
}

static float snd_music_vol_callback( const float& val )
{
	float vol = val;
	if (vol < 0.f) vol = 0.f;
	if (vol > 1.5f) vol = 1.5f;
	if (oggPlaying) Mix_VolumeMusic(vol * 128);
	return vol;
}

namespace CVars
{
	CVarBool snd_enabled("snd_enabled", CVar::CONFIG, "Enables sound subsystem.", true, snd_enabled_callback);
	CVarBool snd_mute("snd_mute", CVar::CONFIG | CVar::CONFIG_AUTO, "Mutes all sound.", false);
	CVarFloat snd_music_vol("snd_music_vol", CVar::CONFIG | CVar::CONFIG_AUTO, "Music volume.", 1.f, snd_music_vol_callback);
	CVarFloat snd_fx_vol("snd_fx_vol", CVar::CONFIG | CVar::CONFIG_AUTO, "Sound effects volume.", 1.f, rangeBind(0.f, 1.5f));
	CVarInt snd_buffer("snd_buffer", CVar::CONFIG, "Size of audio buffer.", 1024, rangeBind(0, 8192));
}

namespace CCmds
{
	namespace Func
	{
		static void snd_add_custom(const std::vector<std::string>& params)
		{
			unsigned int song_id = CCmd::convertParam<unsigned int>(params, 0);
			std::string music_file = CCmd::convertParam<std::string>(params, 1);

			if (music_file.empty())
			{
				customMusicMappings.erase(song_id);
			}
			else
			{
				std::string music_file_path;
				try
				{
					music_file_path = Filesystem::get().findDatafile(music_file);
				}
				catch (Filesystem::FileOpenErrorException& e)
				{
					throw CCmd::RuntimeCCmdError(std::string("Error opening file: ") + e.what());
				}

				customMusicMappings[song_id] = music_file_path;
			}
		}
	}

	CCmd snd_add_custom("snd_add_custom", CCmd::NONE, "Adds a custom music file. Usage: snd_add_custom [song_id] [music_file]", Func::snd_add_custom);
}

void audio_cb(void *userdata, unsigned char *sdl_buffer, int howmuch)
{
	SAMPLE_TYPE *feedme = (SAMPLE_TYPE *)sdl_buffer;

	float s_music_vol = CVars::snd_mute ? 0.f : CVars::snd_music_vol.get();

	if (music_playing && !oggPlaying && s_music_vol > 0.f)
	{
		s_music_vol *= music_vol_multiplier;

		/* SYN: Simulate the fm synth chip */
		SAMPLE_TYPE *music_pos = feedme;
		long remaining = howmuch / BYTES_PER_SAMPLE / 2;
		while (remaining > 0)
		{
			static long ct = 0;

			while(ct < 0)
			{
				ct += freq;
				lds_update(); /* SYN: Do I need to use the return value for anything here? */
			}
			/* SYN: Okay, about the calculations below. I still don't 100% get what's going on, but...
			- freq is samples/time as output by SDL.
			- REFRESH is how often the play proc would have been called in Tyrian. Standard speed is
			70Hz, which is the default value of 70.0f
			- ct represents the margin between play time (representing # of samples) and tick speed of
			the songs (70Hz by default). It keeps track of which one is ahead, because they don't
			synch perfectly. */
	
			/* set i to smaller of data requested by SDL and a value calculated from the refresh rate */
			long i = (long)((ct / REFRESH) + 4) & ~3;
			i = (i > remaining) ? remaining : i; /* i should now equal the number of samples we get */
			opl_update(music_pos, i);
			music_pos += i*2;
			remaining -= i;
			ct -= (long)(REFRESH * i);
		}

		/* Reduce the music volume. */
		int qu = howmuch / BYTES_PER_SAMPLE;
		for (int smp = 0; smp < qu; smp++)
		{
			feedme[smp] = (Sint16)((float)feedme[smp] * s_music_vol);
		}
	}

	float sample_volume = CVars::snd_mute ? 0.f : CVars::snd_fx_vol.get();

	/* SYN: Mix sound channels and shove into audio buffer */
	for (int ch = 0; ch < SFX_CHANNELS; ch++)
	{
		float volume = sample_volume * channel_vol[ch];

		/* SYN: Don't copy more data than is in the channel! */
		int qu = ((unsigned int)howmuch/2 > channel_len[ch] ? channel_len[ch] : howmuch/2) / BYTES_PER_SAMPLE;
		int smp2 = 0;
		for (int smp = 0; smp < qu; smp++, smp2 += 2)
		{
#if (BYTES_PER_SAMPLE == 2)
			Sint32 clip = (Sint32)feedme[smp2] + (Sint32)(channel_pos[ch][smp] * volume);
			feedme[smp2] = (clip > 0x7fff) ? 0x7fff : (clip <= -0x8000) ? -0x8000 : (Sint16)clip;
#elif (BYTES_PER_SAMPLE == 1)
			Sint16 clip = (Sint16)feedme[smp2] + (Sint16)(channel_pos[ch][smp] * volume);
			feedme[smp2] = (clip > 0x7f) ? 0x7f : (clip <= -0x80) ? -0x80 : (Sint8)clip;
#endif
			feedme[smp2+1] = feedme[smp2];
		}

		channel_pos[ch] += qu;
		channel_len[ch] -= qu * BYTES_PER_SAMPLE;

		/* SYN: If we've emptied a channel buffer, let's free the memory and clear the channel. */
		if (channel_len[ch] == 0)
		{
			free(channel_buffer[ch]);
			channel_buffer[ch] = channel_pos[ch] = NULL;
		}
	}
}

bool init_sound( )
{
	if (sound_initialized)
		return true;

	Console::get() << "Initializing SDL audio..." << std::endl;

	if (SDL_InitSubSystem(SDL_INIT_AUDIO))
	{
		Console::get() << "\a7Error:\ax Failed to initialize audio system: " << SDL_GetError() << std::endl;
		CVars::snd_enabled = false;
		return false;
	}

#if (BYTES_PER_SAMPLE == 2)
	Uint16 format = AUDIO_S16SYS;
#elif (BYTES_PER_SAMPLE == 1)
	Uint16 format = AUDIO_S8;
#endif

	Console::get() << "Requested audio frequency: " << freq << "; buffer size: " << CVars::snd_buffer << std::endl;

	if (Mix_OpenAudio(freq, format, 2, CVars::snd_buffer) == -1)
	{
		Console::get() << "\a7Error:\ax Failed to initialize SDL_mixer audio." << std::endl;
		CVars::snd_enabled = false;
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return false;
	}

	int got_freq = 0, foo2;
	Uint16 foo;
	Mix_QuerySpec(&got_freq, &foo, &foo2);

	Console::get() << "Obtained audio frequency: " << got_freq << std::endl;

	opl_init();

	Mix_SetPostMix(audio_cb, 0);

	sound_initialized = true;

	return true;
}

bool deinit_sound( )
{
	if (!sound_initialized)
		return true;

	/* SYN: TODO: Clean up any other audio stuff, if necessary. This should only be called when we're quitting. */
	for (int i = 0; i < SFX_CHANNELS; ++i)
	{
		delete[] channel_buffer[i];
		channel_buffer[i] = channel_pos[i] = 0;
		channel_len[i] = 0;
	}

	opl_deinit();

	Mix_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	sound_initialized = false;

	return true;
}

void loadOggMusic(unsigned int song)
{
	Mix_HaltMusic();

	if (currentMusic != 0)
		Mix_FreeMusic(currentMusic);

	currentMusic = Mix_LoadMUS(customMusicMappings[song].c_str());

	if (currentMusic == 0)
		Console::get() << "\a7Error:\ax Failed to load music: " << Mix_GetError() << std::endl;
	else
		Mix_PlayMusic(currentMusic, -1);

	oggPlaying = true;
	Mix_VolumeMusic(CVars::snd_music_vol * 128);
}

/* SYN: selectSong is called with 0 to disable the current song. Calling it with 1 will start the current song if not playing, or restart it if it is. */
void JE_selectSong( JE_word value )
{
	/* Making sure that we don't mess with sound buffers when someone else is using them! */
	SDL_LockAudio();

	switch (value)
	{
		case 0:
			if (oggPlaying)
			{
				Mix_HaltMusic();
				oggPlaying = false;
			}
			music_playing = false;
			break;
		case 1:
		case 2:
			if (CVars::snd_enabled)
			{
				if (oggPlaying && currentSong == currentMusicNum)
				{
					Mix_RewindMusic();
				}
				else
				{
					if (customMusicMappings.find(currentSong) != customMusicMappings.end())
						loadOggMusic(currentSong);
					else
					{
						oggPlaying = false;
						lds_load(musicData);
					}
				}
			}
			music_playing = true;
			break;
		default:
			Console::get() << "JE_selectSong: fading TODO!" << std::endl;
			/* TODO: Finish this FADING function! */
			break;
	}

	SDL_UnlockAudio();
}

void JE_multiSamplePlay(unsigned char *buffer, JE_word size, int chan, float vol)
{
	if (!CVars::snd_enabled)
		return;
	
	/* Making sure that we don't mess with sound buffers when someone else is using them! */
	SDL_LockAudio();

	delete[] channel_buffer[chan];

	channel_len[chan] = size * BYTES_PER_SAMPLE * SAMPLE_SCALING;
	channel_buffer[chan] = new SAMPLE_TYPE[channel_len[chan]];
	channel_pos[chan] = channel_buffer[chan];
	channel_vol[chan] = vol;

	for (int i = 0; i < size; i++)
	{
		for (int ex = 0; ex < SAMPLE_SCALING; ex++)
		{
#if (BYTES_PER_SAMPLE == 2)
			channel_buffer[chan][(i * SAMPLE_SCALING) + ex] = (Sint8)buffer[i] << 8;
#elif (BYTES_PER_SAMPLE == 1)
			channel_buffer[chan][(i * SAMPLE_SCALING) + ex] = (Sint8)buffer[i];
#endif
		}
	}

	SDL_UnlockAudio();
}

