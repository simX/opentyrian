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

#include "error.h"
#include "joystick.h"
#include "keyboard.h"
#include "loudness.h"
#include "params.h"
#include "sndmast.h"
#include "vga256d.h"

#include "nortsong.h"

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

	JE_word x;
	FILE *fi;

	JE_resetFile(&fi, "music.mus");

	if (!loadedMusicData)
	{
		/* SYN: We're loading offsets into MUSIC.MUS for each song here. */
		loadedMusicData = true;
		efread(&x, sizeof(x), 1, fi);
		for (int i = 0; i <= MUSIC_NUM; i++)
		{
			vfread(songPos[i], Sint32, fi); /* SYN: reads long int (i.e. 4) * MUSICNUM */
		}
		fseek(fi, 0, SEEK_END);
		songPos[MUSIC_NUM] = ftell(fi); /* Store file size */
	}

	/* SYN: Now move to the start of the song we want, and load the number of bytes given by the
	   difference in offsets between it and the next song. */
	fseek(fi, songPos[songnum - 1], SEEK_SET);
	efread(&musicData, 1, songPos[songnum] - songPos[songnum - 1], fi);

	/* currentSong = songnum; */

	fclose(fi);
}

void JE_loadSndFile( void )
{
	FILE *fi;
	int z;
	unsigned long templ;
	unsigned long sndPos[2][SOUND_NUM + 1]; /* Reindexed by -1, dammit Jason */
	JE_word sndNum;

	/* SYN: Loading offsets into TYRIAN.SND */
	JE_resetFile(&fi, "tyrian.snd");
	efread(&sndNum, sizeof(sndNum), 1, fi);

	for (int x = 0; x < sndNum; x++)
	{
		vfread(sndPos[0][x], Sint32, fi);
	}
	fseek(fi, 0, SEEK_END);
	sndPos[1][sndNum] = ftell(fi); /* Store file size */

	for (int z = 0; z < sndNum; z++)
	{
		fseek(fi, sndPos[0][z], SEEK_SET);
		fxSize[z] = (JE_word)(sndPos[0][z+1] - sndPos[0][z]); /* Store sample sizes */
		free(digiFx[z]);
		digiFx[z] = (Uint8 *)malloc(fxSize[z]);
		efread(digiFx[z], 1, fxSize[z], fi); /* JE: Load sample to buffer */
	}

	fclose(fi);

	/* SYN: Loading offsets into VOICES.SND */
	if (CVars::ch_xmas)
	{
		JE_resetFile(&fi, "voicesc.snd");
	} else {
		JE_resetFile(&fi, "voices.snd");
	}
	efread(&sndNum, sizeof(sndNum), 1, fi);

	for (int x = 0; x < sndNum; x++)
	{
		vfread(sndPos[1][x], Sint32, fi);
	}
	fseek(fi, 0, SEEK_END);
	sndPos[1][sndNum] = ftell(fi); /* Store file size */

	z = SOUND_NUM;

	for (int y = 0; y < sndNum; y++)
	{
		fseek(fi, sndPos[1][y], SEEK_SET);

		templ = (sndPos[1][y+1] - sndPos[1][y]) - 100; /* SYN: I'm not entirely sure what's going on here. */
		if (templ < 1) templ = 1;
		fxSize[z + y] = (JE_word)templ; /* Store sample sizes */
		digiFx[z + y] = (Uint8 *)malloc(fxSize[z + y]);
		efread(digiFx[z + y], 1, fxSize[z + y], fi); /* JE: Load sample to buffer */
	}

	fclose(fi);

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
