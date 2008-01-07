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

#include "newshape.h" // For tempScreenSeg
#include "config.h" // For fullscreen stuff

#include "SDL.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

SDL_Surface *VGAScreen, *VGAScreenSeg;
SDL_Surface *game_screen;
SDL_Surface *VGAScreen2;

/* JE: From Nortsong */
JE_word speed; /* JE: holds timer speed for 70Hz */

SDL_Color vga_palette[] = {
	{0, 0, 0}, {0, 0, 168}, {0, 168, 0}, {0, 168, 168}, {168, 0, 0}, {168, 0, 168}, {168, 84, 0}, {168, 168, 168}, {84, 84, 84}, {84, 84, 252}, {84, 252, 84}, {84, 252, 252}, {252, 84, 84}, {252, 84, 252}, {252, 252, 84}, {252, 252, 252},
	{0, 0, 0}, {20, 20, 20}, {32, 32, 32}, {44, 44, 44}, {56, 56, 56}, {68, 68, 68}, {80, 80, 80}, {96, 96, 96}, {112, 112, 112}, {128, 128, 128}, {144, 144, 144}, {160, 160, 160}, {180, 180, 180}, {200, 200, 200}, {224, 224, 224}, {252, 252, 252},
	{0, 0, 252}, {64, 0, 252}, {124, 0, 252}, {188, 0, 252}, {252, 0, 252}, {252, 0, 188}, {252, 0, 124}, {252, 0, 64}, {252, 0, 0}, {252, 64, 0}, {252, 124, 0}, {252, 188, 0}, {252, 252, 0}, {188, 252, 0}, {124, 252, 0}, {64, 252, 0},
	{0, 252, 0}, {0, 252, 64}, {0, 252, 124}, {0, 252, 188}, {0, 252, 252}, {0, 188, 252}, {0, 124, 252}, {0, 64, 252}, {124, 124, 252}, {156, 124, 252}, {188, 124, 252}, {220, 124, 252}, {252, 124, 252}, {252, 124, 220}, {252, 124, 188}, {252, 124, 156},
	{252, 124, 124}, {252, 156, 124}, {252, 188, 124}, {252, 220, 124}, {252, 252, 124}, {220, 252, 124}, {188, 252, 124}, {156, 252, 124}, {124, 252, 124}, {124, 252, 156}, {124, 252, 188}, {124, 252, 220}, {124, 252, 252}, {124, 220, 252}, {124, 188, 252}, {124, 156, 252},
	{180, 180, 252}, {196, 180, 252}, {216, 180, 252}, {232, 180, 252}, {252, 180, 252}, {252, 180, 232}, {252, 180, 216}, {252, 180, 196}, {252, 180, 180}, {252, 196, 180}, {252, 216, 180}, {252, 232, 180}, {252, 252, 180}, {232, 252, 180}, {216, 252, 180}, {196, 252, 180},
	{180, 252, 180}, {180, 252, 196}, {180, 252, 216}, {180, 252, 232}, {180, 252, 252}, {180, 232, 252}, {180, 216, 252}, {180, 196, 252}, {0, 0, 112}, {28, 0, 112}, {56, 0, 112}, {84, 0, 112}, {112, 0, 112}, {112, 0, 84}, {112, 0, 56}, {112, 0, 28},
	{112, 0, 0}, {112, 28, 0}, {112, 56, 0}, {112, 84, 0}, {112, 112, 0}, {84, 112, 0}, {56, 112, 0}, {28, 112, 0}, {0, 112, 0}, {0, 112, 28}, {0, 112, 56}, {0, 112, 84}, {0, 112, 112}, {0, 84, 112}, {0, 56, 112}, {0, 28, 112},
	{56, 56, 112}, {68, 56, 112}, {84, 56, 112}, {96, 56, 112}, {112, 56, 112}, {112, 56, 96}, {112, 56, 84}, {112, 56, 68}, {112, 56, 56}, {112, 68, 56}, {112, 84, 56}, {112, 96, 56}, {112, 112, 56}, {96, 112, 56}, {84, 112, 56}, {68, 112, 56},
	{56, 112, 56}, {56, 112, 68}, {56, 112, 84}, {56, 112, 96}, {56, 112, 112}, {56, 96, 112}, {56, 84, 112}, {56, 68, 112}, {80, 80, 112}, {88, 80, 112}, {96, 80, 112}, {104, 80, 112}, {112, 80, 112}, {112, 80, 104}, {112, 80, 96}, {112, 80, 88},
	{112, 80, 80}, {112, 88, 80}, {112, 96, 80}, {112, 104, 80}, {112, 112, 80}, {104, 112, 80}, {96, 112, 80}, {88, 112, 80}, {80, 112, 80}, {80, 112, 88}, {80, 112, 96}, {80, 112, 104}, {80, 112, 112}, {80, 104, 112}, {80, 96, 112}, {80, 88, 112},
	{0, 0, 64}, {16, 0, 64}, {32, 0, 64}, {48, 0, 64}, {64, 0, 64}, {64, 0, 48}, {64, 0, 32}, {64, 0, 16}, {64, 0, 0}, {64, 16, 0}, {64, 32, 0}, {64, 48, 0}, {64, 64, 0}, {48, 64, 0}, {32, 64, 0}, {16, 64, 0},
	{0, 64, 0}, {0, 64, 16}, {0, 64, 32}, {0, 64, 48}, {0, 64, 64}, {0, 48, 64}, {0, 32, 64}, {0, 16, 64}, {32, 32, 64}, {40, 32, 64}, {48, 32, 64}, {56, 32, 64}, {64, 32, 64}, {64, 32, 56}, {64, 32, 48}, {64, 32, 40},
	{64, 32, 32}, {64, 40, 32}, {64, 48, 32}, {64, 56, 32}, {64, 64, 32}, {56, 64, 32}, {48, 64, 32}, {40, 64, 32}, {32, 64, 32}, {32, 64, 40}, {32, 64, 48}, {32, 64, 56}, {32, 64, 64}, {32, 56, 64}, {32, 48, 64}, {32, 40, 64},
	{44, 44, 64}, {48, 44, 64}, {52, 44, 64}, {60, 44, 64}, {64, 44, 64}, {64, 44, 60}, {64, 44, 52}, {64, 44, 48}, {64, 44, 44}, {64, 48, 44}, {64, 52, 44}, {64, 60, 44}, {64, 64, 44}, {60, 64, 44}, {52, 64, 44}, {48, 64, 44},
	{44, 64, 44}, {44, 64, 48}, {44, 64, 52}, {44, 64, 60}, {44, 64, 64}, {44, 60, 64}, {44, 52, 64}, {44, 48, 64}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
};

void JE_initVGA256( void )
{
	static bool initd = false;

	if (!initd)
	{
		initd = true;
		fullscreen_enabled = fullscreen_set;

		if (SDL_InitSubSystem(SDL_INIT_VIDEO) != -1)
		{
			int w = surface_width, h = surface_height;

			Uint32 flags = SDL_SWSURFACE | SDL_HWPALETTE | (fullscreen_enabled ? SDL_FULLSCREEN : 0);
			VGAScreen = VGAScreenSeg = SDL_SetVideoMode(w, h, 8, flags);

			if (VGAScreen == NULL)
			{
				goto video_error;
			}
		} else {
		video_error:
			Console::get() << "Display initialization failed: " << SDL_GetError() << std::endl;
			exit(1);
		}
		
		VGAScreen2 = SDL_CreateRGBSurface(SDL_SWSURFACE, surface_width, surface_height, 8, 0, 0, 0, 0);
		game_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, surface_width, surface_height, 8, 0, 0, 0, 0);
	}

#if defined(TARGET_GP2X) || defined(NDEBUG)
	/* Remove the cursor from the top-left corner of the screen  */
	SDL_ShowCursor(0);
#endif

	SDL_FillRect(VGAScreenSeg, NULL, 0x0);
	SDL_FillRect(game_screen, NULL, 0x0);
	
	JE_showVGA();
}

void JE_closeVGA256( void )
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void JE_clr256( void )
{
	memset(VGAScreen->pixels, 0, VGAScreen->pitch * VGAScreen->h);
}

void JE_showVGA( void )
{
	SDL_Flip(VGAScreenSeg);
}

void JE_pix( JE_word x, JE_word y, Uint8 c )
{
	/* Bad things happen if we don't clip */
	if (x <  VGAScreen->w && y <  VGAScreen->h)
	{
		Uint8 *vga = (Uint8 *)VGAScreen->pixels;
		vga[y * VGAScreen->w + x] = c;
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
	if (x < VGAScreen->w * VGAScreen->h)
	{
		Uint8 *vga = (Uint8 *)VGAScreen->pixels;
		vga[x] = c;
	}
}

void JE_getPix( JE_word x, JE_word y, Uint8 *c )
{
	/* Bad things happen if we don't clip */
	if (x <  VGAScreen->w && y <  VGAScreen->h)
	{
		Uint8 *vga = (Uint8 *)VGAScreen->pixels;
		*c = vga[y * VGAScreen->w + x];
	}
}

Uint8 JE_getPixel( JE_word x, JE_word y )
{
	/* Bad things happen if we don't clip */
	if (x <  VGAScreen->w && y <  VGAScreen->h)
	{
		Uint8 *vga = (Uint8 *)VGAScreen->pixels;
		return vga[y * VGAScreen->w + x];
	}

	return 0;
}

void JE_rectangle( JE_word a, JE_word b, JE_word c, JE_word d, JE_word e ) /* x1, y1, x2, y2, color */
{
	if (a < VGAScreen->w && b < VGAScreen->h &&
	    c < VGAScreen->w && d < VGAScreen->h)
	{
		Uint8 *vga = (Uint8 *)VGAScreen->pixels;

		/* Top line */
		memset(&vga[b * VGAScreen->w + a], e, c - a + 1);

		/* Bottom line */
		memset(&vga[d * VGAScreen->w + a], e, c - a + 1);

		/* Left line */
		for (int i = (b + 1) * VGAScreen->w + a; i < (d * VGAScreen->w + a); i += VGAScreen->w)
		{
			vga[i] = (Uint8)e;
		}

		/* Right line */
		for (int i = (b + 1) * VGAScreen->w + c; i < (d * VGAScreen->w + c); i += VGAScreen->w)
		{
			vga[i] = (Uint8)e;
		}
	} else {
		Console::get() << "!!! WARNING: Rectangle clipped: " << a << " " << b << " " << c << " " << d << " " << e << std::endl;
	}
}

void JE_bar( JE_word a, JE_word b, JE_word c, JE_word d, Uint8 e ) /* x1, y1, x2, y2, color */
{
	if (a < VGAScreen->w && b < VGAScreen->h &&
	    c < VGAScreen->w && d < VGAScreen->h)
	{
		Uint8 *vga = (Uint8 *)VGAScreen->pixels;
		int width = c - a + 1;

		for (int i = b * VGAScreen->w + a; i <= d * VGAScreen->w + a; i += VGAScreen->w)
		{
			memset(&vga[i], e, width);
		}
	} else {
		Console::get() << "!!! WARNING: Filled Rectangle clipped: " << a << " " << b << " " << c << " " << d << " " << e << std::endl;
	}
}

void JE_c_bar( JE_word a, JE_word b, JE_word c, JE_word d, Uint8 e )
{
	if (a < VGAScreen->w && b < VGAScreen->h &&
	    c < VGAScreen->w && d < VGAScreen->h)
	{
		Uint8 *vga = (Uint8 *)VGAScreenSeg->pixels;
		int width;

		width = c - a + 1;

		for (int i = b * VGAScreen->w + a; i <= d * VGAScreen->w + a; i += VGAScreen->w)
		{
			memset(&vga[i], e, width);
		}
	} else {
		Console::get() << "!!! WARNING: C Filled Rectangle clipped: " << a << " " << b << " " << c << " " << d << " " << e << std::endl;
	}
}

void JE_barShade( JE_word a, JE_word b, JE_word c, JE_word d ) /* x1, y1, x2, y2 */
{
	if (a < VGAScreen->w && b < VGAScreen->h &&
	    c < VGAScreen->w && d < VGAScreen->h)
	{
		Uint8 *vga = (Uint8 *)VGAScreen->pixels;
		int width;

		width = c - a + 1;

		for (int i = b * VGAScreen->w + a; i <= d * VGAScreen->w + a; i += VGAScreen->w)
		{
			for (int j = 0; j < width; j++)
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
	if (a < VGAScreen->w && b < VGAScreen->h &&
	    c < VGAScreen->w && d < VGAScreen->h)
	{
		Uint8 *vga = (Uint8 *)VGAScreen->pixels;
		int width;

		width = c-a+1;

		for (int i = b * VGAScreen->w + a; i <= d * VGAScreen->w + a; i += VGAScreen->w)
		{
			for (int j = 0; j < width; j++)
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

		vga = (Uint8 *)VGAScreen->pixels;
		vga[(int)b] = c;
	}
}

void JE_line( JE_word a, int b, int c, int d, Uint8 e )
{
	float g, h, x, y;
	int v;
	Uint8 *vga;

	v = round(sqrt((float)(abs((a*a)-(c*c))+abs((b*b)-(d*d))) / 4));
	g = (c-a)/(float)v; h = (d-b)/(float)v;
	x = a; y = (float)b;

	vga = (Uint8 *)VGAScreen->pixels;

	for (int z = 0; z <= v; z++)
	{
		vga[(int)(round(x) + round(y)) * VGAScreen->w] = e;
		x += g; y += h;
	}
}

void JE_getPalette( Uint8 col, Uint8 *red, Uint8 *green, Uint8 *blue )
{
	SDL_Color color;

	color = VGAScreen->format->palette->colors[col];

	*red = color.r >> 2;
	*green = color.g >> 2;
	*blue = color.b >> 2;
}

void JE_setPalette( Uint8 col, Uint8 red, Uint8 green, Uint8 blue )
{
	SDL_Color color;

	color.r = red << 2;
	color.g = green << 2;
	color.b = blue << 2;

	SDL_SetColors(VGAScreenSeg, &color, col, 1);
}

void JE_drawGraphic( JE_word x, JE_word y, JE_ShapeTypeOne s )
{
	Uint8 *vga = (Uint8 *)VGAScreen->pixels;

	vga += y * VGAScreen->w + x;

	for (int i = 0; i < 14; i++)
	{
		memcpy(vga, s, 12);
		vga += VGAScreen->w; s += 12;
	}
}
