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

#include "vga256d.h"

#include "keyboard.h"
#include "newshape.h" // For tempScreenSeg
#include "config.h" // For fullscreen stuff
#include "video.h"

#include "SDL.h"
#include <cassert>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

void JE_pix( JE_word x, JE_word y, Uint8 c )
{
	/* Bad things happen if we don't clip */
	if (x <  scr_width && y <  scr_height)
	{
		Uint8 *vga = (Uint8 *)VGAScreen;
		vga[y * scr_width + x] = c;
	}
}

void JE_pixCool( JE_word x, JE_word y, Uint8 c )
{
	/* Originally impemented as several direct accesses */
	JE_pix(x, y, c);
	JE_pix(x - 1, y, c);
	JE_pix(x + 1, y, c);
	JE_pix(x, y - 1, c);
	JE_pix(x, y + 1, c);
}

void JE_pixAbs( JE_word x, Uint8 c )
{
	if (x < scr_width * scr_height)
	{
		Uint8 *vga = (Uint8 *)VGAScreen;
		vga[x] = c;
	}
}

void JE_getPix( JE_word x, JE_word y, Uint8 *c )
{
	/* Bad things happen if we don't clip */
	if (x <  scr_width && y <  scr_height)
	{
		Uint8 *vga = (Uint8 *)VGAScreen;
		*c = vga[y * scr_width + x];
	}
}

Uint8 JE_getPixel( JE_word x, JE_word y )
{
	/* Bad things happen if we don't clip */
	if (x <  scr_width && y <  scr_height)
	{
		Uint8 *vga = (Uint8 *)VGAScreen;
		return vga[y * scr_width + x];
	}

	return 0;
}

void JE_rectangle( JE_word a, JE_word b, JE_word c, JE_word d, JE_word e ) /* x1, y1, x2, y2, color */
{
	if (a < scr_width && b < scr_height &&
	    c < scr_width && d < scr_height)
	{
		Uint8 *vga = (Uint8 *)VGAScreen;

		/* Top line */
		memset(&vga[b * scr_width + a], e, c - a + 1);

		/* Bottom line */
		memset(&vga[d * scr_width + a], e, c - a + 1);

		/* Left line */
		for (unsigned int i = (b + 1) * scr_width + a; i < (d * scr_width + a); i += scr_width)
		{
			vga[i] = (Uint8)e;
		}

		/* Right line */
		for (unsigned int i = (b + 1) * scr_width + c; i < (d * scr_width + c); i += scr_width)
		{
			vga[i] = (Uint8)e;
		}
	} else {
		Console::get() << "!!! WARNING: Rectangle clipped: " << a << " " << b << " " << c << " " << d << " " << e << std::endl;
	}
}

void JE_bar( JE_word a, JE_word b, JE_word c, JE_word d, Uint8 e ) /* x1, y1, x2, y2, color */
{
	if (a < scr_width && b < scr_height &&
	    c < scr_width && d < scr_height)
	{
		Uint8 *vga = (Uint8 *)VGAScreen;
		int width = c - a + 1;

		for (unsigned int i = b * scr_width + a; i <= d * scr_width + a; i += scr_width)
		{
			memset(&vga[i], e, width);
		}
	} else {
		Console::get() << "!!! WARNING: Filled Rectangle clipped: " << a << " " << b << " " << c << " " << d << " " << e << std::endl;
	}
}

void JE_c_bar( JE_word a, JE_word b, JE_word c, JE_word d, Uint8 e )
{
	if (a < scr_width && b < scr_height &&
	    c < scr_width && d < scr_height)
	{
		Uint8 *vga = VGAScreenSeg;
		int width;

		width = c - a + 1;

		for (unsigned int i = b * scr_width + a; i <= d * scr_width + a; i += scr_width)
		{
			memset(&vga[i], e, width);
		}
	} else {
		Console::get() << "!!! WARNING: C Filled Rectangle clipped: " << a << " " << b << " " << c << " " << d << " " << e << std::endl;
	}
}

void JE_barShade( JE_word a, JE_word b, JE_word c, JE_word d ) /* x1, y1, x2, y2 */
{
	if (a < scr_width && b < scr_height &&
	    c < scr_width && d < scr_height)
	{
		Uint8 *vga = (Uint8 *)VGAScreen;
		unsigned int width = c - a + 1;

		for (unsigned int i = b * scr_width + a; i <= d * scr_width + a; i += scr_width)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				vga[i + j] = ((vga[i + j] & 0x0F) >> 1) | (vga[i + j] & 0xF0);
			}
		}
	} else {
		Console::get() << "!!! WARNING: Darker clipped: " << a << " " << b << " " << c << " " << d << std::endl;
	}
}

void JE_barShade2( JE_word a, JE_word b, JE_word c, JE_word d )
{
	JE_barShade(a + 3, b + 2, c - 3, d - 2);
}

void JE_barBright( JE_word a, JE_word b, JE_word c, JE_word d ) /* x1, y1, x2, y2 */
{
	if (a < scr_width && b < scr_height &&
	    c < scr_width && d < scr_height)
	{
		Uint8 *vga = (Uint8 *)VGAScreen;
		unsigned int width = c-a+1;

		for (unsigned int i = b * scr_width + a; i <= d * scr_width + a; i += scr_width)
		{
			for (unsigned int j = 0; j < width; j++)
			{
				Uint8 al, ah;
				al = ah = vga[i + j];

				ah &= 0xF0;
				al = (al & 0x0F) + 2;

				if (al > 0x0F)
				{
					al = 0x0F;
				}

				vga[i + j] = al + ah;
			}
		}
	} else {
		Console::get() << "!!! WARNING: Brighter Rectangle clipped: " << a << " " << b << " " << c << " " << d << std::endl;
	}
}

void JE_circle( JE_word x, int y, JE_word z, Uint8 c ) /* z == radius */
{
	float a = 0, rx, ry, rz, b;
	Uint8 *vga;

	while (a < 6.29)
	{
		a += (160-z)/16000.f; /* Magic numbers everywhere! */

		rx = x; ry = (float)y; rz = z;

		b = x + floor(sin(a)*z+(y+floor(cos(a)*z))*320);

		vga = (Uint8 *)VGAScreen;
		vga[(int)b] = c;
	}
}

void JE_line( JE_word a, int b, int c, int d, Uint8 e )
{
	float g, h, x, y;
	int v;
	Uint8 *vga;

	v = ot_round(sqrt((float)(abs((a*a)-(c*c))+abs((b*b)-(d*d))) / 4));
	g = (c-a)/(float)v; h = (d-b)/(float)v;
	x = a; y = (float)b;

	vga = (Uint8 *)VGAScreen;

	for (int z = 0; z <= v; z++)
	{
		vga[(int)(ot_round(x) + ot_round(y)) * scr_width] = e;
		x += g; y += h;
	}
}

void JE_drawGraphic( JE_word x, JE_word y, JE_ShapeTypeOne s )
{
	Uint8 *vga = (Uint8 *)VGAScreen;

	vga += y * scr_width + x;

	for (int i = 0; i < 14; i++)
	{
		memcpy(vga, s, 12);
		vga += scr_width; s += 12;
	}
}
