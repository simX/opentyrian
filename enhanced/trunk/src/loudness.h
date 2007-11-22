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
#ifndef LOUDNESS_H
#define LOUDNESS_H

#include "opentyr.h"

#include "SDL.h"


#define SFX_CHANNELS 8
#define OUTPUT_QUALITY 4
#define BYTES_PER_SAMPLE 2
#define SAMPLE_SCALING OUTPUT_QUALITY
#define SAMPLE_TYPE Sint16

typedef unsigned char JE_MusicType [20000];

extern JE_MusicType musicData;
extern bool repeated;
extern bool playing;

extern float sample_volume;
extern float music_volume;

void JE_initialize( void );
void JE_deinitialize( void );

/* SYN: selectSong is called with 0 to disable the current song. Calling it with 1 will start the current song if not playing,
   or restart it if it is. */
void JE_selectSong( JE_word value );

void JE_multiSamplePlay(unsigned char *buffer, JE_word size, int chan, int vol);

void JE_setVol(JE_word volume, JE_word sample); /* Call with 0x1-0x100 for music volume, and 0x10 to 0xf0 for sample volume. */

#endif /* LOUDNESS_H */
