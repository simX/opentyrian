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
#include "nortsong.h"

#include "error.h"
#include "joystick.h"
#include "keyboard.h"
#include "loudness.h"
#include "params.h"
#include "sndmast.h"
#include "vga256d.h"
#include "Filesystem.h"
#include "BinaryStream.h"

#include "SDL.h"

const char musicTitle[MUSIC_NUM][48] =
{
	"Asteroid Dance Part 2",
	"Asteroid Dance Part 1",
	"Buy/Sell Music",
	"CAMANIS",
	"CAMANISE",
	"Deli Shop Quartet",
	"Deli Shop Quartet No. 2",
	"Ending Number 1",
	"Ending Number 2",
	"End of Level",
	"Game Over Solo",
	"Gryphons of the West",
	"Somebody pick up the Gryphone",
	"Gyges, Will You Please Help Me?",
	"I speak Gygese",
	"Halloween Ramble",
	"Tunneling Trolls",
	"Tyrian, The Level",
	"The MusicMan",
	"The Navigator",
	"Come Back to Me, Savara",
	"Come Back again to Savara",
	"Space Journey 1",
	"Space Journey 2",
	"The final edge",
	"START5",
	"Parlance",
	"Torm - The Gathering",
	"TRANSON",
	"Tyrian: The Song",
	"ZANAC3",
	"ZANACS",
	"Return me to Savara",
	"High Score Table",
	"One Mustn""t Fall",
	"Sarah""s Song",
	"A Field for Mag",
	"Rock Garden",
	"Quest for Peace",
	"Composition in Q",
	"BEER"
};

Uint32 target, target2;
bool mixEnable = false;
bool loadedSoundData = false;
bool loadedMusicData = false;

JE_SongPosType songPos;

JE_word frameCount, frameCount2, frameCountMax;

int currentSong = 0;

Uint8 *digiFx[SOUND_NUM + 9]; /* [1..soundnum + 9] */
JE_word fxSize[SOUND_NUM + 9]; /* [1..soundnum + 9] */

float tempVolume;

JE_word speed; /* JE: holds timer speed for 70Hz */
float jasondelay = 1000.0f / (1193180.0f / 0x4300);

void setdelay( int delay )
{
	target = (delay << 4)+SDL_GetTicks(); /* delay << 4 == delay * 16 */
}

void setjasondelay( int delay )
{
	target = (Uint32)(SDL_GetTicks() + delay * jasondelay);
}

void setjasondelay2( int delay )
{
	target2 = (Uint32)(SDL_GetTicks() + delay * jasondelay);
}

int delaycount( void )
{
	return (SDL_GetTicks() < target ? target - SDL_GetTicks() : 0);
}

int delaycount2( void )
{
	return (SDL_GetTicks() < target2 ? target2 - SDL_GetTicks() : 0);
}

void wait_delay( void )
{
	while (SDL_GetTicks() < target)
	{
		SDL_Delay(SDL_GetTicks() - target > SDL_POLL_INTERVAL ? SDL_POLL_INTERVAL : SDL_GetTicks() - target);
		service_SDL_events(false);
	}
}

void wait_delayorinput( bool keyboard, bool mouse, bool joystick )
{
	newkey = newmouse = false;
	service_SDL_events(false);
	while (SDL_GetTicks() < target && !(keydown || !keyboard) && !(mousedown || !mouse) && !(button[0] || !joystick))
	{
		SDL_Delay(SDL_GetTicks() - target > SDL_POLL_INTERVAL ? SDL_POLL_INTERVAL : SDL_GetTicks() - target);
		if (joystick)
		{
			JE_joystick2();
		}
		service_SDL_events(false);
	}
}

void JE_loadSong( JE_word songnum )
{
	if (!CVars::snd_enabled)
		return;

	std::fstream fi;
	IBinaryStream bs(fi);

	Filesystem::get().openDatafileFail(fi, "music.mus");

	if (!loadedMusicData)
	{
		/* SYN: We're loading offsets into MUSIC.MUS for each song here. */
		loadedMusicData = true;
		bs.get16();
		for (int i = 0; i <= MUSIC_NUM; i++)
		{
			// SYN: reads long int (i.e. 4) * MUSICNUM
			songPos[i] = bs.getS32();
		}
		fi.seekg(0, std::ios::end);
		songPos[MUSIC_NUM] = fi.tellg(); /* Store file size */
	}

	/* SYN: Now move to the start of the song we want, and load the number of bytes given by the
	   difference in offsets between it and the next song. */
	fi.seekg(songPos[songnum-1]);
	// unsigned char musicData[20000];
	bs.getIter(musicData, musicData + (songPos[songnum]-songPos[songnum-1]));

	/* currentSong = songnum; */

	fi.close();
}

void JE_loadSndFile( void )
{
	unsigned long sndPos[2][SOUND_NUM + 1]; /* Reindexed by -1, dammit Jason */
	JE_word sndNum;

	// SYN: Loading offsets into TYRIAN.SND
	std::fstream f;
	Filesystem::get().openDatafileFail(f, "tyrian.snd");
	IBinaryStream bs(f);

	sndNum = bs.get16();

	for (int i = 0; i < sndNum; ++i)
	{
		sndPos[0][i] = bs.getS32();
	}

	// Store file size
	f.seekg(0, std::ios::end);
	sndPos[1][sndNum] = f.tellg();

	for (int i = 0; i < sndNum; ++i)
	{
		f.seekg(sndPos[0][i]);
		// Store sample sizes
		fxSize[i] = (JE_word)(sndPos[0][i+1] - sndPos[0][i]);

		delete[] digiFx[i];
		digiFx[i] = new Uint8[fxSize[i]];

		if (f.fail()){
			Console::get() << "foobar";
		}

		unsigned int pos = f.tellg();

		// Load sample to buffer
		bs.getIter(digiFx[i], digiFx[i]+fxSize[i]);
	}

	f.close();

	// SYN: Loading offsets into VOICES.SND
	Filesystem::get().openDatafileFail(f, CVars::ch_xmas ? "voicesc.snd" : "voices.snd");
	sndNum = bs.get16();

	for (int i = 0; i < sndNum; ++i)
	{
		sndPos[1][i] = bs.getS32();
	}

	// Store file size
	f.seekg(0, std::ios::end);
	sndPos[1][sndNum] = f.tellg();

	for (int i = 0; i < sndNum; ++i)
	{
		f.seekg(sndPos[1][i]);

		// SYN: I'm not entirely sure what's going on here.
		unsigned long templ = (sndPos[1][i+1] - sndPos[1][i]) - 100;
		if (templ < 1)
			templ = 1;

		// Store sample sizes
		fxSize[SOUND_NUM+i] = (JE_word)templ;
		digiFx[SOUND_NUM+i] = new Uint8[fxSize[SOUND_NUM+i]];
		// Load sample to buffer
		bs.getIter(digiFx[SOUND_NUM+i], digiFx[SOUND_NUM+i]+fxSize[SOUND_NUM+i]);
	}

	f.close();

	loadedSoundData = true;

}

void JE_playSong( JE_word songnum )
{
	DEBUG_MSG("Playing song number " << songnum);

	if (songnum == 0) /* SYN: Trying to play song 0 was doing strange things D: */
	{
		JE_stopSong();
		currentSong = 0;
		return;
	}
	else if (currentSong != songnum)
	{
		JE_stopSong();
		currentSong = songnum;
		JE_loadSong(songnum);
		repeated = false;
		playing = true;
		JE_selectSong(1);
	}
}

void JE_stopSong( void )
{
	JE_selectSong(0);
}

void JE_restartSong( void )
{
	JE_selectSong(2);
}

void JE_playSampleNum( int samplenum )
{
	if (CVars::snd_enabled)
	{
		/* SYN: Reindexing by -1 because of Jason's arrays starting at 1. Dammit. */
		JE_multiSamplePlay( digiFx[samplenum-1], fxSize[samplenum-1], 0, 1.f );
	}
}

void JE_setTimerInt( void )
{
	jasondelay = 1000.0f / (1193180.0f / speed);
}

void JE_resetTimerInt( void )
{
	jasondelay = 1000.0f / (1193180.0f / 0x4300);
}

void JE_timerInt( void )
{
	STUB();
}

void JE_waitFrameCount( void )
{
	/* TODO: I'm not sure how long this function should wait. Leaving it blank for now, it doesn't seem to hurt much... */
}
