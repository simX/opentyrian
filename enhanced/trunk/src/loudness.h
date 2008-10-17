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
#ifndef LOUDNESS_H
#define LOUDNESS_H

#include "opentyr.h"
#include "fmopl.h"

#include "console/CVar.h"

#include "SDL.h"


static const int SFX_CHANNELS = 8;

static const int OUTPUT_QUALITY = 4;

static const int SAMPLE_SCALING = OUTPUT_QUALITY;
typedef OPLSAMPLE SAMPLE_TYPE;
#define BYTES_PER_SAMPLE (OPL_SAMPLE_BITS / 8)

typedef unsigned char JE_MusicType[20000];

extern JE_MusicType musicData;
extern bool repeated;
extern bool playing;

extern float music_vol_multiplier;

namespace CVars
{
	extern CVarBool snd_enabled;
	extern CVarBool snd_mute;
	extern CVarFloat snd_music_vol;
	extern CVarFloat snd_fx_vol;
}

bool init_sound( );
bool deinit_sound( );

/* SYN: selectSong is called with 0 to disable the current song. Calling it with 1 will start the current song if not playing,
   or restart it if it is. */
void JE_selectSong( JE_word value );

void JE_multiSamplePlay(unsigned char *buffer, JE_word size, int chan, float vol);

#endif /* LOUDNESS_H */
