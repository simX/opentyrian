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
#include "network.h"
#include "nortsong.h"
#include "nortvars.h"
#include "varz.h"
#include "video.h"

#include "newshape.h"


Uint8 *tempScreenSeg = VGAScreen;

JE_ShapeArrayType shapeArray;

JE_word shapeX[MAX_TABLE][MAXIMUM_SHAPE],        /* [1..maxtable,1..maximumshape] */
        shapeY[MAX_TABLE][MAXIMUM_SHAPE];        /* [1..maxtable,1..maximumshape] */
JE_word shapeSize[MAX_TABLE][MAXIMUM_SHAPE];     /* [1..maxtable,1..maximumshape] */
bool shapeExist[MAX_TABLE][MAXIMUM_SHAPE]; /* [1..maxtable,1..maximumshape] */

int maxShape[MAX_TABLE];                    /* [1..maxtable] */

Uint8 mouseGrabShape[24*28];                 /* [1..24*28] */

/*
Colors:
253: Black
254: Jump to next line

Skip X Pixels
Draw X pixels of color Y
*/


void JE_newLoadShapesB( int table, FILE *f )
{
	JE_word temp;
	efread(&temp, sizeof(JE_word), 1, f);
	maxShape[table] = temp;

	for (int i = 0; i < maxShape[table]; ++i)
	{
		shapeExist[table][i] = (getc(f) != 0);

		if (shapeExist[table][i])
		{
			efread(&shapeX   [table][i], sizeof(JE_word), 1, f);
			efread(&shapeY   [table][i], sizeof(JE_word), 1, f);
			efread(&shapeSize[table][i], sizeof(JE_word), 1, f);

			shapeArray[table][i] = new Uint8[shapeX[table][i] * shapeY[table][i]];

			// TODO convert to stream
			efread(shapeArray[table][i], sizeof(JE_byte), shapeSize[table][i], f);
		}
	}
}

void JE_newLoadShapes( int table, const char *shapefile )
{
	FILE *f;
	
	JE_newPurgeShapes(table);
	
	JE_resetFile(&f, shapefile);
	
	JE_newLoadShapesB(table, f);
	
	fclose(f);
}

void JE_newDrawCShape( Uint8 *shape, JE_word xsize, JE_word ysize )
{
	JE_word x = 2, y = 2;
	JE_word xloop = 0, yloop = 0;
	Uint8 *p; /* shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */

	s = tempScreenSeg;
	s += y * scr_width + x;

	for (p = shape; yloop < ysize; p++)
	{
		switch (*p)
		{
			case 255: /* p transparent pixels */
				p++;
				s += *p; xloop += *p;
				break;
			case 254: /* next y */
				s -= xloop; xloop = 0;
				s += scr_width; yloop++;
				break;
			case 253: /* 1 transparent pixel */
				s++; xloop++;
				break;
			default:  /* set a pixel */
				*s = *p;
				s++; xloop++;
				break;
		}
		if (xloop == xsize)
		{
			s -= xloop; xloop = 0;
			s += scr_width; yloop++;
		}
	}

	tempScreenSeg = VGAScreen;
}

void JE_newDrawCShapeNum( int table, int shape, JE_word x, JE_word y )
{
	JE_word xsize, ysize;
	JE_word xloop = 0, yloop = 0;
	Uint8 *p; /* shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	if ((shape-- /* re-index */ > maxShape[table]) || (!shapeExist[table][shape]) || (shape == 255))
	{
		exit(99); /* pascalism */
	}

	xsize = shapeX[table][shape]; ysize = shapeY[table][shape];

	s = tempScreenSeg;
	s += y * scr_width + x;

	s_limit = tempScreenSeg;
	s_limit += scr_height * scr_width;

	for (p = shapeArray[table][shape]; yloop < ysize; p++)
	{
		switch (*p)
		{
			case 255: /* p transparent pixels */
				p++;
				s += *p; xloop += *p;
				break;
			case 254: /* next y */
				s -= xloop; xloop = 0;
				s += scr_width; yloop++;
				break;
			case 253: /* 1 transparent pixel */
				s++; xloop++;
				break;
			default:  /* set a pixel */
				if (s >= s_limit)
					{ tempScreenSeg = VGAScreen; return; }
				if ((void *)s >= tempScreenSeg)
					*s = *p;
				s++; xloop++;
				break;
		}
		if (xloop == xsize)
		{
			s -= xloop; xloop = 0;
			s += scr_width; yloop++;
		}
	}

	tempScreenSeg = VGAScreen;
}

void JE_newPurgeShapes( int table )
{
	for (int i = 0; i < maxShape[table]; ++i)
	{
		if (shapeExist[table][i])
		{
			delete shapeArray[table][i];
			shapeExist[table][i] = false;
		}
	}

	maxShape[table] = 0;
}

void JE_drawShapeTypeOne( JE_word x, JE_word y, Uint8 *shape )
{
	Uint8 *p = shape; /* shape pointer */
	Uint8 *s;   /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	s = (Uint8 *)VGAScreen;
	s += y * scr_width + x;

	s_limit = tempScreenSeg;
	s_limit += scr_height * scr_width;

	for (int yloop = 0; yloop < 28; yloop++)
	{
		for (int xloop = 0; xloop < 24; xloop++)
		{
			if (s >= s_limit) return;
			*s = *p;
			s++; p++;
		}
		s -= 24;
		s += scr_width;
	}
}

void JE_grabShapeTypeOne( JE_word x, JE_word y, Uint8 *shape )
{
	Uint8 *p = shape; /* shape pointer */
	Uint8 *s;   /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	s = (Uint8 *)VGAScreen;
	s += y * scr_width + x;

	s_limit = tempScreenSeg;
	s_limit += scr_height * scr_width;

	for (int yloop = 0; yloop < 28; yloop++)
	{
		for (int xloop = 0; xloop < 24; xloop++)
		{
			if (s >= s_limit) return;
			*p = *s;
			s++; p++;
		}
		s -= 24;
		s += scr_width;
	}
}

bool JE_waitAction( int time, bool checkJoystick )
{
	if (time > 0)
	{
		setjasondelay(time);
	}

	do
	{
		service_SDL_events(true);
		mouseButton = mousedown ? lastmouse_but : 0; /* incorrect, possibly unimportant */
		mouseX = mouse_x;
		mouseY = mouse_y;
		inputDetected = newkey | mousedown | JE_joystickNotHeld();

		if (time == 0 && temp != 0)
		{
			JE_mouseStart();
			JE_showVGA();
			JE_mouseReplace();
		}

		if (time == 0 && isNetworkGame)
		{
			/* TODO: NETWORK */
		}

	} while (!(inputDetected || delaycount() == 0 || netQuit));

	return inputDetected;
}

void JE_mouseStart( void )
{
	const JE_word mouseCursorGr[3] /* [1..3] */ = {273, 275, 277};

	JE_word tempW;

	tempW = mouseCursorGr[mouseCursor];

	service_SDL_events(false);
	mouseButton = mousedown ? lastmouse_but : 0; /* incorrect, possibly unimportant */
	lastMouseX = mouse_x;
	lastMouseY = mouse_y;

	if (lastMouseX > 320 - 13)
	{
		lastMouseX = 320 - 13;
	}
	if (lastMouseY > 200 - 16)
	{
		lastMouseY = 200 - 16;
	}

	JE_grabShapeTypeOne(lastMouseX, lastMouseY, mouseGrabShape);

	/*JE_drawShape2x2shadow(lastmousex+2,lastmousey+2,tempW,shapes6);*/
	JE_drawShape2x2(lastMouseX, lastMouseY, tempW, shapes6);
}

void JE_mouseReplace( void )
{
	JE_drawShapeTypeOne(lastMouseX, lastMouseY, mouseGrabShape);
}
