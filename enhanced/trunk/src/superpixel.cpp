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
#include "superpixel.h"

#include "video.h"
#include "mtrand.h"

#include <cmath>

static const unsigned int MAX_SUPERPIXELS = 101;

struct SuperPixel
{
	unsigned int x, y;
	unsigned int life;
	int delta_x, delta_y;
	Uint8 color;
};

static SuperPixel superpixels[MAX_SUPERPIXELS];
static unsigned int last_superpixel;

void init_superpixels( )
{
	last_superpixel = 0;
	std::fill_n(superpixels, MAX_SUPERPIXELS, SuperPixel());
}

void create_superpixels( JE_word x, JE_word y, unsigned int num, int explowidth, Uint8 color )
{
	for (unsigned int i = 0; i < num; i++)
	{
		float tempr = ((float)mt::rand() / MT_RAND_MAX) * M_PI*2;
		int tempx = ot_round(sin(tempr) * ((float)mt::rand() / MT_RAND_MAX) * explowidth);
		int tempy = ot_round(cos(tempr) * ((float)mt::rand() / MT_RAND_MAX) * explowidth);

		if (++last_superpixel >= MAX_SUPERPIXELS) last_superpixel = 0;

		superpixels[last_superpixel].x = tempx + x;
		superpixels[last_superpixel].y = tempy + y;
		superpixels[last_superpixel].delta_x = tempx;
		superpixels[last_superpixel].delta_y = tempy;
		superpixels[last_superpixel].color = color;
		superpixels[last_superpixel].life = 15;
	}
}

void draw_superpixels( )
{
	for (unsigned int i = MAX_SUPERPIXELS; i--;)
	{
		if (superpixels[i].life > 0)
		{
			superpixels[i].x += superpixels[i].delta_x;
			superpixels[i].y += superpixels[i].delta_y;

			if (superpixels[i].x < scr_width && superpixels[i].y < scr_height)
			{
				Uint8* s = (Uint8 *)VGAScreen; /* screen pointer, 8-bit specific */
				s += superpixels[i].y * scr_width + superpixels[i].x;
				
				*s = (((*s & 0x0f) + superpixels[i].life) >> 1) + superpixels[i].color;
				if (superpixels[i].x > 0)
					*(s-1) = (((*(s-1) & 0x0f) + (superpixels[i].life >> 1)) >> 1) + superpixels[i].color;
				if (superpixels[i].x < scr_width-1)
					*(s+1) = (((*(s+1) & 0x0f) + (superpixels[i].life >> 1)) >> 1) + superpixels[i].color;
				if (superpixels[i].y > 0)
					*(s-scr_width) = (((*(s-scr_width) & 0x0f) + (superpixels[i].life >> 1)) >> 1) + superpixels[i].color;
				if (superpixels[i].y < scr_height - 1)
					*(s+scr_width) = (((*(s+scr_width) & 0x0f) + (superpixels[i].life >> 1)) >> 1) + superpixels[i].color;
			}
			
			superpixels[i].life--;
		}
	}
}
