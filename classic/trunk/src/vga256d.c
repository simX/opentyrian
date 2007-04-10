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
#include "opentyr.h"

#define NO_EXTERNS
#include "vga256d.h"
#undef NO_EXTERNS

#include "SDL.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

JE_boolean mouse_installed = TRUE;
JE_char k;

SDL_Surface *VGAScreen;
Uint8 VGAScreen2Seg[320*200];

/* JE: From Nortsong */
JE_word speed; /* JE: holds timer speed for 70Hz */

JE_byte scancode;
JE_byte outcol;

void JE_initvga256( void )
{
	if ((SDL_InitSubSystem(SDL_INIT_VIDEO) == -1) ||
	   !(VGAScreen = SDL_SetVideoMode(320,200,8, SDL_SWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF)))
	{
		printf("Display initialization failed: %s\n", SDL_GetError());
		exit(1);
	}
}

void JE_initVGA256X( void )
{
	JE_initvga256();
}

void JE_closevga256( void )
{
	/* SDL_QuitSubSystem(SDL_INIT_VIDEO); */
	SDL_Quit();
}

void JE_clr256( void )
{
	memset(VGAScreen->pixels, 0, VGAScreen->pitch * VGAScreen->h);
}

void JE_showVGA( void )
{
	SDL_Flip(VGAScreen);
}

void JE_showVGARetrace( void )
{
	SDL_Flip(VGAScreen);
}

void JE_getVGA( void )
{
	SDL_Flip(VGAScreen); /* TODO: YKS: This is probably not what we should do, but I don't see a way of doing it either. */
}

void JE_onScreen( void )
{
	printf("!!! STUB: %s:%d:JE_onScreen\n", __FILE__, __LINE__);
}

void JE_offScreen( void )
{
	printf("!!! STUB: %s:%d:JE_offScreen\n", __FILE__, __LINE__);
}

void JE_disable_refresh( void )
{
	/* This would normally blank the screen, but since it's hard to implement and not used I'll leave it as a no-op. */
}

void JE_enable_refresh( void )
{
	/* Same as JE_disable_refresh */
}

void JE_waitRetrace( void )
{
	/* nop */
}

void JE_waitPartialRetrace( void )
{
}

void JE_waitNotRetrace( void )
{
}



void JE_pix( JE_word x, JE_word y, JE_byte c )
{
	JE_pix2(x,y,c);
}

void JE_pix2( JE_word x, JE_word y, JE_byte c )
{
	/* Bad things happen if we don't clip */
	if (x < 320 && y < 200)
	{
		char *vga = VGAScreen->pixels;
		vga[y*320+x] = c;
	}
}

void JE_pixcool( JE_word x, JE_word y, JE_byte c )
{
	JE_pix3(x,y,c);
}

void JE_pix3( JE_word x, JE_word y, JE_byte c )
{
	/* Originally impemented as several direct accesses */
	JE_pix2(x,y,c);
	JE_pix2(x-1,y,c);
	JE_pix2(x+1,y,c);
	JE_pix2(x,y-1,c);
	JE_pix2(x,y+1,c);
}

void JE_pixabs( JE_word x, JE_byte c )
{
	if (x < 320*200)
	{
		char *vga = VGAScreen->pixels;
		vga[x] = c;
	}
}

void JE_getpix( JE_word x, JE_word y, JE_byte *c )
{
	/* Bad things happen if we don't clip */
	if (x < 320 && y < 200)
	{
		char *vga = VGAScreen->pixels;
		*c = vga[y*320+x];
	}
}

JE_byte JE_getpixel( JE_word x, JE_word y )
{
	/* Bad things happen if we don't clip */
	if (x < 320 && y < 200)
	{
		char *vga = VGAScreen->pixels;
		return vga[y*320+x];
	}

	return 0;
}

void JE_rectangle( JE_word a, JE_word b, JE_word c, JE_word d, JE_word e ) /* x1, y1, x2, y2, color */
{
	if (a < 320 && c < 320 && b < 200 && d < 200)
	{
		char *vga = VGAScreen->pixels;
		int i;

		/* Top line */
		memset(vga+(b*320+a), e, c-a+1);

		/* Bottom line */
		memset(vga+(d*320+a), e, c-a+1);

		/* Left line */
		for (i=(b+1)*320+a; i < (d*320+a); i += 320)
		{
			vga[i] = e;
		}
		
		/* Right line */
		for (i=(b+1)*320+c; i < (d*320+c); i += 320)
		{
			vga[i] = e;
		}
	} else {
		printf("!!! WARNING: Rectangle clipped: %d %d %d %d %d\n", a,b,c,d,e);
	}
}

void JE_bar( JE_word a, JE_word b, JE_word c, JE_word d, JE_byte e ) /* x1, y1, x2, y2, color */
{
	if (a < 320 && c < 320 && b < 200 && d < 200)
	{
		char *vga = VGAScreen->pixels;
		int i, width;

		width = c-a+1;

		for (i = b*320+a; i <= d*320+a; i += 320)
		{
			memset(vga+i, e, width);
		}
	} else {
		printf("!!! WARNING: Filled Rectangle clipped: %d %d %d %d %d\n", a,b,c,d,e);
	}
}

void JE_barshade( JE_word a, JE_word b, JE_word c, JE_word d ) /* x1, y1, x2, y2 */
{
	if (a < 320 && c < 320 && b < 200 && d < 200)
	{
		char *vga = VGAScreen->pixels;
		int i,j, width;

		width = c-a+1;

		for (i = b*320+a; i <= d*320+a; i += 320)
		{
			for (j = 0; j < width; j++)
			{
				vga[i+j] = ((vga[i+j] & 0x0F) >> 1) | (vga[i+j] & 0xF0);
			}
		}
	} else {
		printf("!!! WARNING: Darker Rectangle clipped: %d %d %d %d\n", a,b,c,d);
	}
}

void JE_barshade2( JE_word a, JE_word b, JE_word c, JE_word d )
{
	JE_barshade(a+3, b+2, c-3, d-2);
}

void JE_barbright( JE_word a, JE_word b, JE_word c, JE_word d ) /* x1, y1, x2, y2 */
{
	if (a < 320 && c < 320 && b < 200 && d < 200)
	{
		char *vga = VGAScreen->pixels;
		int i,j, width;

		width = c-a+1;

		for (i = b*320+a; i <= d*320+a; i += 320)
		{
			for (j = 0; j < width; j++)
			{
				JE_byte al, ah;
				al = ah = vga[i+j];

				ah &= 0xF0;
				al = (al & 0x0F) + 2;

				if (al > 0x0F)
				{
					al = 0x0F;
				}

				vga[i+j] = al + ah;
			}
		}
	} else {
		printf("!!! WARNING: Brighter Rectangle clipped: %d %d %d %d\n", a,b,c,d);
	}
}

void JE_circle( JE_word x, JE_byte y, JE_word z, JE_byte c ) /* z == radius */
{
	JE_real a = 0, rx,ry,rz, b; char *vga;

	while (a < 6.29)
	{
		a += (160-z)/16000.0; /* Magic numbers everywhere! */

		rx = x; ry = y; rz = z;

		b = x + floor(sin(a)*z+(y+floor(cos(a)*z))*320);

		vga = VGAScreen->pixels;
		vga[(int)b] = c;
	}
}

void JE_line( JE_word a, JE_byte b, JE_longint c, JE_byte d, JE_byte e )
{
	JE_real g,h,x,y;
	JE_integer z,v;
	char *vga;

	v = ROUND(sqrt(abs((a*a)-(c*c))+abs((b*b)-(d*d)) / 4));
	g = (c-a)/(double)v; h = (d-b)/(double)v;
	x = a; y = b;

	vga = VGAScreen->pixels;

	for (z = 0; z <= v; z++)
	{
		vga[(int)(ROUND(x)+ROUND(y)*320.0)] = e;
		x += g; y += h;
	}
}

void JE_getPalette( JE_byte col, JE_byte *red, JE_byte *green, JE_byte *blue )
{
	SDL_Color color;

	color = VGAScreen->format->palette->colors[col];

	*red = color.r;
	*green = color.g;
	*blue = color.b;
}

void JE_setPalette( JE_byte col, JE_byte red, JE_byte green, JE_byte blue )
{
	SDL_Color color;

	color.r = red;
	color.g = green;
	color.b = blue;

	SDL_SetColors(VGAScreen, &color, col, 1);
}

void JE_drawgraphic( JE_word x, JE_word y, JE_ShapeTypeOne s )
{
	char *vga = VGAScreen->pixels;
	int i;

	vga += y*320+x;
	
	for (i = 0; i <14; i++)
	{
		memcpy(vga, s, 12);
		vga += 320; s += 12;
	}
}

void JE_getk( JE_char *k )
{
	SDL_Event ev;

	printf("!!! WARNING: JE_getk is deprecated! %s:%d\n", __FILE__, __LINE__);
	for (;;)
	{
		SDL_WaitEvent(&ev);

		if (ev.type == SDL_KEYDOWN)
		{
			scancode = ev.key.keysym.scancode;
			*k = ev.key.keysym.unicode & 0x7F;
			break;
		}
	}
}

void JE_getupk( JE_char *k )
{
	printf("!!! WARNING: JE_getupk is deprecated! %s:%d\n", __FILE__, __LINE__);
	JE_getk(k);
	*k = toupper(*k);
}

JE_boolean JE_keypressed( JE_char *kp )
{
	SDL_Event ev;

	SDL_PumpEvents();

	printf("!!! WARNING: JE_keypressed is deprecated! %s:%d\n", __FILE__, __LINE__);
	if (SDL_PeepEvents(&ev, 1, SDL_GETEVENT, SDL_EVENTMASK(SDL_KEYDOWN)) > 0)
	{
		scancode = ev.key.keysym.scancode;
		*kp = ev.key.keysym.unicode & 0x7F;
		return 1;
	} else {
		return 0;
	}
}

JE_boolean JE_kp( void )
{
	printf("!!! WARNING: JE_kp is deprecated! %s:%d\n", __FILE__, __LINE__);
	SDL_PumpEvents();

	return SDL_PeepEvents(NULL, 1, SDL_PEEKEVENT, SDL_EVENTMASK(SDL_KEYDOWN));
}

/*****************************************/

void JE_getimage16( JE_word a, JE_byte b, JE_shape16B *p )
{
	printf("!!! STUB: %s:%d:JE_getimage16\n", __FILE__, __LINE__);
}
void JE_putimage16( JE_word a, JE_byte b, JE_shape16B *p )
{
	printf("!!! STUB: %s:%d:JE_putimage16\n", __FILE__, __LINE__);
}
void JE_absDrawGraphic( JE_ShapeTypeOne s )
{
	printf("!!! STUB: %s:%d:JE_absDrawGraphic\n", __FILE__, __LINE__);
}
void JE_drawgraphicover( JE_word x, JE_word y, JE_ShapeTypeOne s )
{
	printf("!!! STUB: %s:%d:JE_drawgraphicover\n", __FILE__, __LINE__);
}
void JE_absDrawGraphicOver( JE_ShapeTypeOne s )
{
	printf("!!! STUB: %s:%d:JE_absDrawGraphicOver\n", __FILE__, __LINE__);
}
void JE_readgraphic( JE_integer x, JE_integer y, JE_ShapeTypeOne s )
{
	printf("!!! STUB: %s:%d:JE_readgraphic\n", __FILE__, __LINE__);
}
void JE_wait( JE_byte min, JE_byte sec, JE_byte hun )
{
	printf("!!! STUB: %s:%d:JE_wait\n", __FILE__, __LINE__);
}
void JE_darkenscreen( void )
{
	printf("!!! STUB: %s:%d:JE_darkenscreen\n", __FILE__, __LINE__);
}
