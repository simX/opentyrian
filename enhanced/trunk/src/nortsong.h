/* vim: set noet:
 *
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
 * Copyright (C) 2007  The OpenTyrian Team
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
#ifndef NORTSONG_H
#define NORTSONG_H

#include "opentyr.h"

#include "musmast.h"
#include "sndmast.h"

#include "SDL.h"


typedef unsigned long JE_SongPosType [MUSIC_NUM + 1]; /* [1..Musicnum + 1] */

extern Uint32 target, target2;

extern bool mixEnable;

extern bool notYetLoadedSound, notYetLoadedMusic;
extern JE_SongPosType songPos;

extern int soundEffects;

extern JE_word frameCount, frameCount2, frameCountMax;

extern int currentSong;
extern int soundActive, musicActive;

extern Uint8 *digiFx[SOUND_NUM + 9];
extern JE_word fxSize[SOUND_NUM + 9];

extern JE_word fxVolume, fxPlayVol;
extern JE_word tempVolume;
extern JE_word tyrMusicVolume;

extern float jasondelay;

void JE_timerInt( void );

void setdelay( int delay );
void setjasondelay( int delay );
void setjasondelay2( int delay );
int delaycount( void );
int delaycount2( void );
void wait_delay( void );
void wait_delayorinput( bool keyboard, bool mouse, bool joystick );

void JE_resetTimerInt( void );
void JE_setTimerInt( void );
void JE_playSong ( JE_word songnum );
void JE_loadSong( JE_word songnum );
void JE_endMusic ( int soundeffects);
void JE_stopSong( void );
void JE_restartSong( void );
void JE_reinit ( bool redo );
void JE_aweStuff( void );
void JE_setTimerInt( void );
void JE_calcFXVol( void );
void JE_changeVolume( JE_word *temp, int change, JE_word *fxvol, int fxchange );
void JE_frameDelay( int delay );

void JE_loadSmpFile ( const char *name, int samplenum);
void JE_loadSndFile( void );
void JE_playSampleNum ( int samplenum );

void JE_fxDestruct ( int samplenum );

void JE_setvol (int musicvolume, int sample );

void JE_waitFrameCount( void );

/* SYN: This stuff is probably unneeded, as it deals with sound hardware issues abstracted
   away by SDL. Pascal code is left here as reference, just in case we want this stuff
   later.

midiportlist : ARRAY [1..10] OF
  WORD =
  ($0000, $0000, $1000, $0666, $0330, $0320, $0332, $0334, $0336, $0300);

fxportlist : ARRAY [1..4] OF
  WORD =
  ($FFFF, $0000, $0666, $1000);

dmalist : ARRAY [1..3] OF
  WORD =
  ($0000, $0003, $0001);

*/

#endif /* NORTSONG_H */
