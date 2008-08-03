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
#include "palette.h"

#include "error.h"
#include "nortsong.h"
#include "nortvars.h"
#include "video.h"
#include "BinaryStream.h"
#include <fstream>

JE_PalType palettes;
unsigned int palNum;

Palette palette;
Uint32 rgb_palette[256];
Uint32 yuv_palette[256];

Palette black;
Palette colors, colors2;

void JE_loadPals( )
{
	palNum = 0;

	std::ifstream stream;
	open_datafile_fail(stream, "palette.dat");
	IBinaryStream f(stream);

	while (palNum < MAX_PAL && !stream.eof())
	{
		for (int i = 0; i < 256; ++i)
		{
			palettes[palNum][i].r = f.get8() << 2;
			palettes[palNum][i].g = f.get8() << 2;
			palettes[palNum][i].b = f.get8() << 2;
		}
		++palNum;
	}
}

void JE_zPal( unsigned int pal )
{
	JE_updateColorsFast(palettes[pal-1]);
}

void JE_updateColorsFast( const Palette colorBuffer )
{
	for (int i = 0; i < 256; ++i)
	{
		palette[i] = colorBuffer[i];
		rgb_palette[i] = SDL_MapRGB(display_surface->format, palette[i].r, palette[i].g, palette[i].b);
		yuv_palette[i] = rgb_to_yuv(palette[i]);
	}
}

void JE_fadeColors( Palette fromColors, Palette toColors, unsigned int startColor, unsigned int numColors, int numSteps )
{
	for (int s = 0; s <= numSteps; ++s)
	{
		setdelay(1);

		for (unsigned int i = startColor; i <= startColor + numColors; ++i)
		{
			//long difference = toColors[i].r - fromColors[i].r;
			//long each_step = difference / static_cast<long>(numSteps);
			//double steps_so_far = each_step * s;
			//palette[i].r = static_cast<Uint8>(steps_so_far) + fromColors[i].r;
			//palette[i].r = (toColors[i].r - fromColors[i].r) / numSteps * s + fromColors[i].r;
			palette[i].r = fromColors[i].r + (toColors[i].r - fromColors[i].r) * s / numSteps;
			palette[i].g = fromColors[i].g + (toColors[i].g - fromColors[i].g) * s / numSteps;
			palette[i].b = fromColors[i].b + (toColors[i].b - fromColors[i].b) * s / numSteps;
			rgb_palette[i] = SDL_MapRGB(display_surface->format, palette[i].r, palette[i].g, palette[i].b);
			yuv_palette[i] = rgb_to_yuv(palette[i]);
		}
		JE_showVGA();
		wait_delay();
	}
}

void JE_fadeBlack( unsigned int numSteps )
{
	JE_fadeColors(colors, black, 0, 255, numSteps);
}

void JE_fadeColor( unsigned int numSteps )
{
	JE_fadeColors(black, colors, 0, 255, numSteps);
}

void JE_fadeWhite( unsigned int numSteps )
{
	static const SDL_Color whiteCol = {255,255,255};
	std::fill_n(black, 256, whiteCol);

	JE_fadeColors(colors, black, 0, 255, numSteps);
	std::copy(black, black+256, colors);

	static const SDL_Color blackCol = {0,0,0};
	std::fill_n(black, 256, blackCol);
}

void JE_setPalette( Uint8 col, Uint8 r, Uint8 g, Uint8 b )
{
	palette[col].r = r;
	palette[col].g = g;
	palette[col].b = b;
	rgb_palette[col] = SDL_MapRGB(display_surface->format, palette[col].r, palette[col].g, palette[col].b);
	yuv_palette[col] = rgb_to_yuv(palette[col]);
}

Uint32 rgb_to_yuv( SDL_Color col )
{
	const int y = (col.r+col.g+col.b) >> 2;
	const int u = 128 + ((col.r-col.b) >> 2);
	const int v = 128 + ((-col.r + 2*col.g - col.b) >> 3);
	return (y << 16) | (u << 8) | v;
}