/* vim: set noet:
 *
 * OpenTyrian Enhanced: A modern cross-platform port of Tyrian
 * Copyright (C) 2007  The OpenTyrian Development Team
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

#include "fm_synth.h"
#include "lds_play.h"
#include "console/Console.h"
#include "console/CVar.h"
#include "sndmast.h"
#include "nortsong.h"

#include "loudness.h"


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

int freq = 11025 * OUTPUT_QUALITY;

bool music_playing = false;

static bool sound_initialized = false;

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

namespace CVars
{
	CVarBool snd_enabled("snd_enabled", CVar::CONFIG, "Enables sound subsystem.", true, snd_enabled_callback);
	CVarBool snd_mute("snd_mute", CVar::CONFIG, "Mutes all sound.", false);
	CVarFloat snd_music_vol("snd_music_vol", CVar::CONFIG, "Music volume.", 1.f, rangeBind(0.f, 1.5f));
	CVarFloat snd_fx_vol("snd_fx_vol", CVar::CONFIG, "Sound effects volume.", 1.f, rangeBind(0.f, 1.5f));
}

void audio_cb(void *userdata, unsigned char *sdl_buffer, int howmuch)
{
	SAMPLE_TYPE *feedme = (SAMPLE_TYPE *)sdl_buffer;

	float s_music_vol = CVars::snd_mute ? 0.f : CVars::snd_music_vol.get();

	if (music_playing && s_music_vol > 0.f)
	{
		s_music_vol *= music_vol_multiplier;

		/* SYN: Simulate the fm synth chip */
		SAMPLE_TYPE *music_pos = feedme;
		long remaining = howmuch / BYTES_PER_SAMPLE;
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
			music_pos += i;
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
		int qu = ((unsigned int)howmuch > channel_len[ch] ? channel_len[ch] : howmuch) / BYTES_PER_SAMPLE;
		for (int smp = 0; smp < qu; smp++)
		{
#if (BYTES_PER_SAMPLE == 2)
			Sint32 clip = (Sint32)feedme[smp] + (Sint32)(channel_pos[ch][smp] * volume);
			feedme[smp] = (clip > 0x7fff) ? 0x7fff : (clip <= -0x8000) ? -0x8000 : (Sint16)clip;
#elif (BYTES_PER_SAMPLE == 1)
			Sint16 clip = (Sint16)feedme[smp] + (Sint16)(channel_pos[ch][smp] * volume);
			feedme[smp] = (clip > 0x7f) ? 0x7f : (clip <= -0x80) ? -0x80 : (Sint8)clip;
#endif
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

	SDL_AudioSpec plz, got;
	plz.freq = freq;
#if (BYTES_PER_SAMPLE == 2)
	plz.format = AUDIO_S16SYS;
#elif (BYTES_PER_SAMPLE == 1)
	plz.format = AUDIO_S8;
#endif
	plz.channels = 1;
	plz.samples = 512;
	plz.callback = audio_cb;

	Console::get() << "Requested SDL frequency: " << plz.freq << "; SDL buffer size: " << plz.samples << std::endl;

	if ( SDL_OpenAudio(&plz, &got) == -1 )
	{
		Console::get() << "\a7Error:\ax Failed to initialize SDL audio." << std::endl;
		CVars::snd_enabled = false;
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return false;
	}

	Console::get() << "Obtained SDL frequency: " << got.freq << "; SDL buffer size: " << got.samples << std::endl;

	opl_init();

	SDL_PauseAudio(0);

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

	SDL_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	sound_initialized = false;

	return true;
}

/* SYN: selectSong is called with 0 to disable the current song. Calling it with 1 will start the current song if not playing, or restart it if it is. */
void JE_selectSong( JE_word value )
{
	/* Making sure that we don't mess with sound buffers when someone else is using them! */
	SDL_LockAudio();

	switch (value)
	{
		case 0:
			music_playing = false;
			break;
		case 1:
		case 2:
			if (CVars::snd_enabled)
			{
				lds_load(musicData); /* Load song */
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

