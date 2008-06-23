/* vim: set noet:
 *
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
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


JE_PalType palettes;
JE_word palNum;

palette_t palette;
Uint32 rgb_palette[256], yuv_palette[256];

palette_t black = {{0,0,0}}; /* Rest is filled with 0's too */
palette_t colors, colors2;

void JE_loadPals( void )
{
	FILE *f;
	int i;

	palNum = 0;

	JE_resetFile(&f, "palette.dat");
	while (palNum < MAX_PAL && !feof(f))
	{
		for (i = 0; i < 256; i++)
		{
			palettes[palNum][i].r = getc(f) << 2;
			palettes[palNum][i].g = getc(f) << 2;
			palettes[palNum][i].b = getc(f) << 2;
		}
		palNum++;
	}
	fclose(f);
}

void JE_zPal( JE_byte palette )
{
	JE_updateColorsFast(palettes[palette - 1]);
}

void JE_updateColorsFast( palette_t colorBuffer )
{
	for (int i = 0; i < 256; i++)
	{
		palette[i].r = colorBuffer[i].r;
		palette[i].g = colorBuffer[i].g;
		palette[i].b = colorBuffer[i].b;
		rgb_palette[i] = SDL_MapRGB(display_surface->format, palette[i].r, palette[i].g, palette[i].b);
		yuv_palette[i] = rgb_to_yuv(palette[i].r, palette[i].g, palette[i].b);
	}
	
#ifdef TARGET_GP2X
	SDL_SetColors(display_surface, palette, 0, 256);
#endif /* TARGET_GP2X */
}

void JE_fadeColors( palette_t fromColors, palette_t toColors, JE_byte startCol, JE_byte noColors, JE_byte noSteps )
{
	int s, i;

	for (s = 0; s <= noSteps; s++)
	{
		setdelay(1);
		
		for (i = startCol; i <= startCol + noColors; i++)
		{
			palette[i].r = fromColors[i].r + (((toColors[i].r - fromColors[i].r) * s) / noSteps);
			palette[i].g = fromColors[i].g + (((toColors[i].g - fromColors[i].g) * s) / noSteps);
			palette[i].b = fromColors[i].b + (((toColors[i].b - fromColors[i].b) * s) / noSteps);
			rgb_palette[i] = SDL_MapRGB(display_surface->format, palette[i].r, palette[i].g, palette[i].b);
			yuv_palette[i] = rgb_to_yuv(palette[i].r, palette[i].g, palette[i].b);
		}
		
#ifndef TARGET_GP2X
		JE_showVGA();
#else /* TARGET_GP2X */
		SDL_SetColors(display_surface, palette, 0, 256);
#endif /* TARGET_GP2X */
		
		wait_delay();
	}
}

void JE_fadeBlack( JE_byte steps )
{
	JE_fadeColors(colors, black, 0, 255, steps);
}

void JE_fadeColor( JE_byte steps )
{
	JE_fadeColors(black, colors, 0, 255, steps);
}

void JE_fadeWhite( JE_byte steps )
{
	memset(black, 255, sizeof(black));
	JE_fadeColors(colors, black, 0, 255, steps);
	memcpy(colors, black, sizeof(colors));
	memset(black, 0, sizeof(black));
}

void JE_setPalette( JE_byte col, JE_byte red, JE_byte green, JE_byte blue )
{
	palette[col].r = red;
	palette[col].g = green;
	palette[col].b = blue;
	rgb_palette[col] = SDL_MapRGB(display_surface->format, palette[col].r, palette[col].g, palette[col].b);
	yuv_palette[col] = rgb_to_yuv(palette[col].r, palette[col].g, palette[col].b);
	
#ifdef TARGET_GP2X
	SDL_SetColors(display_surface, palette, 0, 256);
#endif /* TARGET_GP2X */
}

Uint32 rgb_to_yuv( int r, int g, int b )
{
	int y = (r + g + b) >> 2,
	    u = 128 + ((r - b) >> 2),
	    v = 128 + ((-r + 2 * g - b) >> 3);
	return (y << 16) + (u << 8) + v;
}
