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
#ifndef PALETTE_H
#define PALETTE_H

#include "opentyr.h"

#include "SDL.h"

static const unsigned int MAX_PAL = 23;

typedef SDL_Color Palette[256];
typedef Palette JE_PalType[MAX_PAL];

extern JE_PalType palettes;
extern unsigned int palNum;

extern Palette palette;
extern Uint32 rgb_palette[256];
extern Uint32 yuv_palette[256];

extern Palette black, colors, colors2;

void JE_loadPals( );
void load_palette( unsigned int pal_num, bool set_colors );
void JE_zPal( unsigned int pal );

void JE_updateColorsFast( const Palette colorBuffer );
void JE_fadeColors( Palette fromColors, Palette toColors, unsigned int startColor, unsigned int numColors, int numSteps );
void JE_fadeBlack( unsigned int numSteps );
void JE_fadeColor( unsigned int numSteps );
void JE_fadeWhite( unsigned int numSteps );

void JE_setPalette( Uint8 col, Uint8 r, Uint8 g, Uint8 b );

Uint32 rgb_to_yuv( SDL_Color col );

#endif // PALETTE_H
