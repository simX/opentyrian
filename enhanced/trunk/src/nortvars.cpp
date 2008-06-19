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

#include "error.h"
#include "joystick.h"
#include "keyboard.h"
#include "vga256d.h"

#include "nortvars.h"

#include <ctype.h>


bool inputDetected;
JE_word lastMouseX, lastMouseY;

/*Mouse Data*/
/*Mouse_Installed is in VGA256d*/
int mouseCursor;
bool mouse_threeButton;
JE_word mouseX, mouseY, mouseButton;

JE_word JE_btow(Uint8 a, Uint8 b)
{
	return (JE_word)(((Uint16) b) * 256 + a);
}

void JE_loadShapeFile( JE_ShapeType *shapes, char s )
{
	FILE *f;
	bool active;

	char buffer[12];
	sprintf(buffer, "shapes%c.dat", tolower(s));

	JE_resetFile(&f, buffer);

	for (int x = 0; x < 304; x++)
	{
		active = (getc(f) != 0);

		if (active)
		{
			efread((*shapes)[x], sizeof(Uint8), sizeof(*(*shapes)[x]), f);
		} else {
			memset((*shapes)[x], 0, sizeof(*(*shapes)[x]));
		}
	}

	fclose(f);

	/*fprintf(stderr, "Shapes%c completed.\n", s);*/
}

void JE_loadNewShapeFile( JE_NewShapeType *shapes, char s )
{
	FILE *f;
	bool active;
	JE_ShapeTypeOne tempshape;
	int black, color;

	char buffer[12];
	sprintf(buffer, "shapes%c.dat", tolower(s));

	JE_resetFile(&f, buffer);

	for (int z = 0; z < 304; z++)
	{
		active = (getc(f) != 0);

		if (active)
		{
			efread(tempshape, sizeof(Uint8), sizeof(tempshape), f);

			for (int y = 0; y <= 13; y++)
			{

				black = 0;
				color = 0;
				for (int x = 0; x <= 11; x++)
				{
					if (tempshape[x + y * 12] == 0)
					{
						black++;
					} else {
						color++;
					}
				}

				if (black == 12)
				{  /* Compression Value 0 - All black */
					(*shapes)[z][y * 13] = 0;
				} else {
					if (color == 12)
					{  /* Compression Value 1 - All color */
						(*shapes)[z][y * 13] = 1;
						for (int x = 0; x <= 11; x++)
						{
							(*shapes)[z][x + 1 + y * 13] = tempshape[x + y * 12];
						}
					} else {
						(*shapes)[z][y * 13] = 2;
						for (int x = 0; x <= 11; x++)
						{
							(*shapes)[z][x + 1 + y * 13] = tempshape[x + y * 12];
						}
					}
				}
			}
		} else {
			memset((*shapes)[z], 0, sizeof((*shapes)[z]));
		}
	}

	fclose(f);

	/*fprintf(stderr, "Shapes%c completed.\n", s);*/
}

void JE_loadCompShapes( Uint8 **shapes, unsigned long *shapeSize, char s )
{
	FILE *f;

	char buffer[11];
	sprintf(buffer, "newsh%c.shp", tolower(s));

	if (*shapes != NULL)
	{
		free(*shapes);
	}

	JE_resetFile(&f, buffer);

	fseek(f, 0, SEEK_END);
	*shapeSize = ftell(f);
	fseek(f, 0, SEEK_SET);

	*shapes = (Uint8 *)	malloc(*shapeSize);

	efread(*shapes, sizeof(Uint8), *shapeSize, f);

	fclose(f);
}

void JE_drawShape2( int x, int y, int s_, Uint8 *shape )
{
	Uint8 *p; /* shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	int i;

	s = (Uint8 *)VGAScreen;
	s += y * scr_width + x;

	s_limit = (Uint8 *)VGAScreen;
	s_limit += scr_height * scr_width;

	p = shape;
	p += SDL_SwapLE16(((JE_word *)p)[s_ - 1]);

	while (*p != 0x0f)
	{
		s += *p & 0x0f;
		i = (*p & 0xf0) >> 4;
		if (i)
		{
			while (i--)
			{
				p++;
				if (s >= s_limit)
					return;
				if ((void *)s >= VGAScreen)
					*s = *p;
				s++;
			}
		} else {
			s -= 12;
			s += scr_width;
		}
		p++;
	}
}

void JE_superDrawShape2( int x, int y, int s_, Uint8 *shape )
{
	Uint8 *p; /* shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	int i;

	s = (Uint8 *)VGAScreen;
	s += y * scr_width + x;

	s_limit = (Uint8 *)VGAScreen;
	s_limit += scr_height * scr_width;

	p = shape;
	p += SDL_SwapLE16(((JE_word *)p)[s_ - 1]);

	while (*p != 0x0f)
	{
		s += *p & 0x0f;
		i = (*p & 0xf0) >> 4;
		if (i)
		{
			while (i--)
			{
				p++;
				if (s >= s_limit)
					return;
				if ((void *)s >= VGAScreen)
					*s = (((*p & 0x0f) + (*s & 0x0f)) >> 1) | (*p & 0xf0);
				s++;
			}
		} else {
			s -= 12;
			s += scr_width;
		}
		p++;
	}
}

void JE_drawShape2Shadow( int x, int y, int s_, Uint8 *shape )
{
	Uint8 *p; /* shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	int i;

	s = (Uint8 *)VGAScreen;
	s += y * scr_width + x;

	s_limit = (Uint8 *)VGAScreen;
	s_limit += scr_height * scr_width;

	p = shape;
	p += SDL_SwapLE16(((JE_word *)p)[s_ - 1]);

	while (*p != 0x0f)
	{
		s += *p & 0x0f;
		i = (*p & 0xf0) >> 4;
		if (i)
		{
			while (i--)
			{
				p++;
				if (s >= s_limit)
					return;
				if ((void *)s >= VGAScreen)
					*s = ((*s & 0x0f) >> 1) + (*s & 0xf0);
				s++;
			}
		} else {
			s -= 12;
			s += scr_width;
		}
		p++;
	}
}

void JE_drawShape2x2( int x, int y, int s, Uint8 *shape )
{
	JE_drawShape2(x,    y,    s,    shape);
	JE_drawShape2(x+12, y,    s+1,  shape);
	JE_drawShape2(x,    y+14, s+19, shape);
	JE_drawShape2(x+12, y+14, s+20, shape);
}

void JE_superDrawShape2x2( int x, int y, int s, Uint8 *shape )
{
	JE_superDrawShape2(x,    y,    s,    shape);
	JE_superDrawShape2(x+12, y,    s+1,  shape);
	JE_superDrawShape2(x,    y+14, s+19, shape);
	JE_superDrawShape2(x+12, y+14, s+20, shape);
}

void JE_drawShape2x2Shadow( int x, int y, int s, Uint8 *shape )
{
	JE_drawShape2Shadow(x,    y,    s,    shape);
	JE_drawShape2Shadow(x+12, y,    s+1,  shape);
	JE_drawShape2Shadow(x,    y+14, s+19, shape);
	JE_drawShape2Shadow(x+12, y+14, s+20, shape);
}

bool JE_anyButton( void )
{
	button[0] = false;
	service_SDL_events(true);
	JE_joystick2();
	return newkey || mousedown || button[0];
}

void JE_setMousePosition( JE_word mouseX, JE_word mouseY )
{
	if (input_grabbed)
	{
		SDL_WarpMouse(mouseX, mouseY);
	}
}

void JE_dBar3( int x, int y, int num, int col )
{
	int zWait = 2;

	col += 2;

	for (int z = 0; z <= num; z++)
	{
		JE_rectangle(x, y - 1, x + 8, y, col); /* <MXD> SEGa000 */
		if (zWait > 0)
		{
			zWait--;
		} else {
			col++;
			zWait = 1;
		}
		y -= 2;
	}
}

void JE_barDraw( JE_word x, JE_word y, JE_word res, JE_word col, JE_word amt, JE_word xsize, JE_word ysize )
{
	xsize--;
	ysize--;
	for (int z = 1; z <= amt / res; z++)
	{
		JE_bar(x, y,         x + xsize, y + ysize, col + 12);
		JE_bar(x, y,         x + xsize, y,         col + 13);
		JE_bar(x, y + ysize, x + xsize, y + ysize, col + 11);
		x += xsize + 2;
	}
	
	amt %= res;
	if (amt > 0)
	{
		JE_bar(x, y, x + xsize, y + ysize, col + ((12 / res) * amt));
	}
}

void JE_barDrawShadow( JE_word x, JE_word y, JE_word res, JE_word col, JE_word amt, JE_word xsize, JE_word ysize )
{
	xsize--;
	ysize--;

	for (int z = 1; z <= amt / res; z++)
	{
		JE_barShade(x+2, y+2, x+xsize+2, y+ysize+2);
		JE_bar(x, y, x+xsize, y+ysize, col+12);
		JE_bar(x, y, x+xsize, y, col+13);
		JE_pix(x, y, col+15);
		JE_bar(x, y+ysize, x+xsize, y+ysize, col+11);
		x += xsize + 2;
	}

	amt %= res;
	if (amt > 0)
	{
		JE_barShade(x+2, y+2, x+xsize+2, y+ysize+2);
		JE_bar(x,y, x+xsize, y+ysize, col+(12 / res * amt));
	}
}

void JE_barDrawDirect( JE_word x, JE_word y, JE_word res, JE_word col, JE_word amt, JE_word xsize, JE_word ysize )
{
	xsize--;
	ysize--;
	for (int z = 1; z <= amt / res; z++)
	{
		JE_c_bar(x, y, x + xsize, y + ysize, col + 12);
		x += xsize + 2;
	}
	
	amt %= res;
	if (amt > 0)
	{
		JE_c_bar(x, y, x + xsize, y + ysize, col + ((12 / res) * amt));
	}
}

void JE_wipeKey( void )
{
	// /!\ Doesn't seems to affect anything.
}

JE_word JE_mousePosition( JE_word *mouseX, JE_word *mouseY )
{
	service_SDL_events(false);
	*mouseX = mouse_x;
	*mouseY = mouse_y;
	return mousedown ? lastmouse_but : 0;
}

/* TODO */
