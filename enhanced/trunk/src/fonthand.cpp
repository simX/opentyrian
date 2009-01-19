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
#include "fonthand.h"

#include "newshape.h"
#include "nortsong.h"
#include "video.h"
#include "vga256d.h"
#include "network.h"


const unsigned char fontMap[136] = /* [33..168] */
{
	26,33,60,61,62,255,32,64,65,63,84,29,83,28,80,79,70,71,72,73,74,75,76,77,
	78,31,30,255,255,255,27,255,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
	20,21,22,23,24,25,68,82,69,255,255,255,34,35,36,37,38,39,40,41,42,43,44,45,46,
	47,48,49,50,51,52,53,54,55,56,57,58,59,66,81,67,255,255,

	86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,
	107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,
	125,126
};

/* shape constants included in newshape.h */

int defaultBrightness = -3;
int textGlowFont, textGlowBrightness = 6;

bool levelWarningDisplay;
int levelWarningLines;
std::string levelWarningText[10]; /* [1..10] of string [60] */
bool warningRed;

int warningSoundDelay;
JE_word armorShipDelay;
int warningCol;
int warningColChange;

void JE_dString( JE_word x, JE_word y, const std::string& s, int font, bool bright )
{
	char b;

	for (int a = 0; s[a] != 0; a++)
	{
		b = s[a];

		if ((b > 32) && (b < 126))
		{
			if (fontMap[b-33] != 255)
			{
				JE_newDrawCShapeDarken(shapeArray[font][fontMap[b-33]], shapeX[font][fontMap[b-33]],
				                       shapeY[font][fontMap[b-33]], x + 2, y + 2);
				JE_newDrawCShapeBright(shapeArray[font][fontMap[b-33]], shapeX[font][fontMap[b-33]],
				                       shapeY[font][fontMap[b-33]], x, y, 15, defaultBrightness + (bright << 1));
				x += shapeX[font][fontMap[b-33]] + 1;
			}
		} else {
			if (b == 32)
			{
				x += 6;
			} else {
				if (b == 126)
				{
					bright = !bright;
				}
			}
		}
	}
}

void JE_newDrawCShapeBright( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y, int filter, int brightness )
{
	JE_word xloop = 0, yloop = 0;
	Uint8 *p;       /* shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	s = tempScreenSeg;
	s += y * scr_width + x;

	s_limit = tempScreenSeg;
	s_limit += scr_height * scr_width;

	filter <<= 4;

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
			default: /* set a pixel */
				if (s >= s_limit)
					return;
				if ((void *)s >= tempScreenSeg)
					*s = ((*p & 0x0f) | filter) + brightness;
				s++; xloop++;
				break;
		}

		if (xloop == xsize)
		{
			s -= xloop; xloop = 0;
			s += scr_width; yloop++;
		}
	}
}

void JE_newDrawCShapeShadow( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y )
{
	JE_word xloop = 0, yloop = 0;
	Uint8 *p;       /* shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	s = tempScreenSeg;
	s += y * scr_width + x;

	s_limit = tempScreenSeg;
	s_limit += scr_height * scr_width;

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
			default: /* set a pixel */
				if (s >= s_limit)
					return;
				if ((void *)s >= tempScreenSeg)
					*s = 0;
				s++; xloop++;
				break;
		}
		if (xloop == xsize)
		{
			s -= xloop; xloop = 0;
			s += scr_width; yloop++;
		}
	}
}

void JE_newDrawCShapeDarken( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y )
{
	JE_word xloop = 0, yloop = 0;
	Uint8 *p;       /* shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	s = tempScreenSeg;
	s += y * scr_width + x;

	s_limit = tempScreenSeg;
	s_limit += scr_height * scr_width;

	for (p = shape; yloop < ysize; p++)
	{
		/* (unported) compare the screen offset to 65535, if equal do case 253 */
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
					return;
				if ((void *)s >= tempScreenSeg)
					*s = ((*s & 0x0f) >> 1) + (*s & 0xf0);
				s++; xloop++;
				break;
		}
		if (xloop == xsize)
		{
			s -= xloop; xloop = 0;
			s += scr_width; yloop++;
		}
	}
}

void JE_newDrawCShapeDarkenNum( int table, int shape, int x, int y )
{
	shape--; /* re-index */
	JE_newDrawCShapeDarken(shapeArray[table][shape], shapeX[table][shape], shapeY[table][shape], x, y);
}

void JE_newDrawCShapeTrick( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y )
{
	JE_word xloop = 0, yloop = 0;
	Uint8 *p;       /* shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	s = tempScreenSeg;
	s += y * scr_width + x;

	s_limit = tempScreenSeg;
	s_limit += scr_height * scr_width;

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
				if (s >= s_limit)
					return;
				if ((void *)s >= tempScreenSeg)
					*s = (((*s & 0x0f) >> 1) | (*p & 0xf0)) + ((*p & 0x0f) >> 1);
				s++; xloop++;
				break;
		}
		if (xloop == xsize)
		{
			s -= xloop; xloop = 0;
			s += scr_width; yloop++;
		}
	}
}

void JE_newDrawCShapeTrickNum( int table, int shape, int x, int y )
{
	shape--; /* re-index */
	JE_newDrawCShapeTrick(shapeArray[table][shape], shapeX[table][shape], shapeY[table][shape], x, y);
}

void JE_newDrawCShapeModify( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y, int filter, int brightness )
{
	JE_word xloop = 0, yloop = 0;
	Uint8 *p;       /* shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */
	Uint8 temp;

	s = tempScreenSeg;
	s += y * scr_width + x;

	s_limit = tempScreenSeg;
	s_limit += scr_height * scr_width;

	filter <<= 4;

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
				temp = (*p & 0x0f) + brightness;
				if (temp >= 0x1f)
				{
					temp = 0;
				}
				if (temp >= 0x0f)
				{
					temp = 0x0f;
				}
				temp >>= 1;
				if (s >= s_limit)
					return;
				if ((void *)s >= tempScreenSeg)
					*s = (((*s & 0x0f) >> 1) | filter) + temp;
				s++; xloop++;
				break;
		}
		if (xloop == xsize)
		{
			s -= xloop; xloop = 0;
			s += scr_width; yloop++;
		}
	}
}

void JE_newDrawCShapeModifyNum( int table, int shape, int x, int y, int filter, int brightness )
{
	shape--; /* re-index */
	JE_newDrawCShapeModify(shapeArray[table][shape], shapeX[table][shape], shapeY[table][shape], x, y, filter, brightness);
}

void JE_newDrawCShapeAdjust( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y, int filter, int brightness )
{
	JE_word xloop = 0, yloop = 0;
	Uint8 *p;       /* shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */
	Uint8 temp;

	s = tempScreenSeg;
	s += y * scr_width + x;

	s_limit = tempScreenSeg;
	s_limit += scr_height * scr_width;

	filter <<= 4;

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
				temp = (*p & 0x0f) + brightness;
				if (temp >= 0x1f)
				{
					temp = 0;
				}
				if (temp >= 0x0f)
				{
					temp = 0x0f;
				}
				if (s >= s_limit)
					return;
				if ((void *)s >= tempScreenSeg)
					*s = temp | filter;
				s++; xloop++;
				break;
		}
		if (xloop == xsize)
		{
			s -= xloop; xloop = 0;
			s += scr_width; yloop++;
		}
	}
}

void JE_newDrawCShapeAdjustNum( int table, int shape, int x, int y, int filter, int brightness )
{
	shape--; /* re-index */
	JE_newDrawCShapeAdjust(shapeArray[table][shape], shapeX[table][shape], shapeY[table][shape], x, y, filter, brightness);
}

void JE_newDrawCShapeBrightAndDarken( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y, int filter, int brightness )
{
	JE_word xloop = 0, yloop = 0;
	Uint8 *p;       /* shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	s = tempScreenSeg;
	s += y * scr_width + x;

	s_limit = tempScreenSeg;
	s_limit += scr_height * scr_width;

	filter <<= 4;

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
				if (s >= s_limit)
					return;
				if ((void *)s >= tempScreenSeg)
					*s = ((*p & 0x0f) | filter) + brightness;
				s++; xloop++;
				s += scr_width; /* jump a pixel down (after incrementing x) */
				if (s >= s_limit)
					return;
				if ((void *)s >= tempScreenSeg)
					*s = ((*s & 0x0f) >> 1) + (*s & 0xf0);
				s -= scr_width; /* jump back up */
				break;
		}

		if (xloop == xsize)
		{
			s -= xloop; xloop = 0;
			s += scr_width; yloop++;
		}
	}
}

JE_word JE_fontCenter( const std::string& s, int font )
{
	return 160 - (JE_textWidth(s, font) / 2);
}

JE_word JE_textWidth( const std::string& s, int font )
{
	Uint8 b;
	JE_word x = 0;

	for (int a = 0; s[a] != 0; a++)
	{
		b = s[a];

		if ((b > 32) && (b < 126))
		{
			if (fontMap[b-33] != 255)
			{
				x += shapeX[font][fontMap[b-33]] + 1;
			}
		} else {
			if (b == 32)
			{
				x += 6;
			}
		}
   }
   return x;
}

void JE_textShade( JE_word x, JE_word y, const std::string& s, int colorbank, int brightness, int shadetype )
{
	switch (shadetype)
	{
		case PART_SHADE:
			JE_outText(x+1, y+1, s, 0, -1);
			JE_outText(x, y, s, colorbank, brightness);
			break;
		case FULL_SHADE:
			JE_outText(x-1, y, s, 0, -1);
			JE_outText(x+1, y, s, 0, -1);
			JE_outText(x, y-1, s, 0, -1);
			JE_outText(x, y+1, s, 0, -1);
			JE_outText(x, y, s, colorbank, brightness);
			break;
		case DARKEN:
			JE_outTextAndDarken(x+1, y+1, s, colorbank, brightness, TINY_FONT);
			break;
		case TRICK:
			JE_outTextModify(x, y, s, colorbank, brightness, TINY_FONT);
			break;
   }
}

void JE_outText( JE_word x, JE_word y, const std::string& s, int colorbank, int brightness )
{
	unsigned char b;
	int bright = 0;

	for (int a = 0; s[a] != 0; a++)
	{
		b = s[a];

		if ((b > 32) && (b < 169) && (fontMap[b-33] != 255) && (shapeArray[TINY_FONT][fontMap[b-33]] != NULL))
		{
			if (brightness >= 0)
			{
				JE_newDrawCShapeBright(shapeArray[TINY_FONT][fontMap[b-33]], shapeX[TINY_FONT][fontMap[b-33]],
				                       shapeY[TINY_FONT][fontMap[b-33]], x, y, colorbank, brightness + bright);
			} else {
				JE_newDrawCShapeShadow(shapeArray[TINY_FONT][fontMap[b-33]], shapeX[TINY_FONT][fontMap[b-33]],
				                       shapeY[TINY_FONT][fontMap[b-33]], x, y);
			}

			x += shapeX[TINY_FONT][fontMap[b-33]] + 1;
		} else {
			if (b == 32)
			{
				x += 6;
			} else {
				if (b == 126)
				{
					if (bright > 0)
					{
						bright = 0;
					} else {
						bright = 4;
					}
				}
			}
		}
	}
	if (brightness >= 0)
	{
		tempScreenSeg = VGAScreen;
	}
}

void JE_outTextModify( JE_word x, JE_word y, const std::string& s, int filter, int brightness, int font )
{
	unsigned char b;

	for (int a = 0; s[a] != 0; a++)
	{
		b = s[a];

		if ((b > 32) && (b < 169) && (fontMap[b-33] != 255))
		{
			JE_newDrawCShapeModify(shapeArray[font][fontMap[b-33]], shapeX[font][fontMap[b-33]], shapeY[font][fontMap[b-33]], x, y, filter, brightness);

			x += shapeX[font][fontMap[b-33]] + 1;
		} else {
			if (b == 32)
			{
				x += 6;
			}
		}
	}
}

void JE_outTextShade( JE_word x, JE_word y, const char *s, int font )
{
	unsigned char b;

	for (int a = 0; s[a] != 0; a++)
	{
		b = s[a];

		if ((b > 32) && (b < 169) && (fontMap[b-33] != 255))
		{
			JE_newDrawCShapeDarken(shapeArray[font][fontMap[b-33]], shapeX[font][fontMap[b-33]], shapeY[font][fontMap[b-33]], x, y);

			x += shapeX[font][fontMap[b-33]] + 1;
		} else {
			if (b == 32)
			{
				x += 6;
			}
		}
	}
}

void JE_outTextAdjust( JE_word x, JE_word y, const std::string& s, int filter, int brightness, int font, bool shadow )
{
	unsigned char b;
	bool bright = false;
	for (int a = 0; s[a] != 0; a++)
	{
		b = s[a];

		if ((b > 32) && (b < 169) && (fontMap[b-33] != 255))
		{
			if (shadow)
			{
				JE_newDrawCShapeDarken(shapeArray[font][fontMap[b-33]], shapeX[font][fontMap[b-33]], shapeY[font][fontMap[b-33]], x + 2, y + 2);
			}

			JE_newDrawCShapeAdjust(shapeArray[font][fontMap[b-33]], shapeX[font][fontMap[b-33]], shapeY[font][fontMap[b-33]], x, y, filter, brightness + (bright << 2));

			x += shapeX[font][fontMap[b-33]] + 1;
		} else {
			if (b == 126)
			{
				bright = !bright;
			} else {
				if (b == 32)
				{
					x += 6;
				}
			}
		}
	}
}

void JE_outTextAndDarken( JE_word x, JE_word y, const std::string& s, int colorbank, int brightness, int font )
{
	unsigned char b;
	int bright = 0;

	for (int a = 0; s[a] != 0; a++)
	{
		b = s[a];

		if ((b > 32) && (b < 169) && (fontMap[b-33] != 255))
		{
			JE_newDrawCShapeBrightAndDarken(shapeArray[font][fontMap[b-33]], shapeX[font][fontMap[b-33]], shapeY[font][fontMap[b-33]], x, y, colorbank, brightness + bright);

			x += shapeX[font][fontMap[b-33]] + 1;
		} else {
			if (b == 32)
			{
				x += 6;
			} else {
				if (b == 126)
				{
					if (bright > 0)
					{
						bright = 0;
					} else {
						bright = 4;
					}
				}
			}
		}
	}
}

void JE_updateWarning( void )
{
	if (delaycount2() == 0)
    { /*Update Color Bars*/

		warningCol += warningColChange;
		if (warningCol > 14 * 16 + 10 || warningCol < 14 * 16 + 4)
		{
			warningColChange = -warningColChange;
		}
		JE_bar(0, 0, 319, 5, warningCol);
		JE_bar(0, 194, 319, 199, warningCol);
		JE_showVGA();

		setjasondelay2(6);

		if (warningSoundDelay > 0)
		{
			warningSoundDelay--;
		} else {
			warningSoundDelay = 14;
			JE_playSampleNum(17);
		}
	}
}

void JE_outTextGlow( JE_word x, JE_word y, const std::string& s )
{
	int c = 15;

	if (warningRed)
	{
		c = 7;
	}
	
	tempScreenSeg = VGAScreenSeg; /* sega000 */
	JE_outTextAdjust(x - 1, y,     s, 0, -12, textGlowFont, false);
	JE_outTextAdjust(x,     y - 1, s, 0, -12, textGlowFont, false);
	JE_outTextAdjust(x + 1, y,     s, 0, -12, textGlowFont, false);
	JE_outTextAdjust(x,     y + 1, s, 0, -12, textGlowFont, false);
	if (frameCountMax > 0)
		for (int z = 1; z <= 12; z++)
		{
			setjasondelay(frameCountMax);
			tempScreenSeg = VGAScreenSeg; /* sega000 */
			JE_outTextAdjust(x, y, s, c, z - 10, textGlowFont, false);
			if (JE_anyButton())
			{
				frameCountMax = 0;
			}
			
			network::keep_alive();
			
			JE_showVGA();

			int delaycount_temp;
			if ((delaycount_temp = target - SDL_GetTicks()) > 0)
				SDL_Delay(delaycount_temp);
		}
	for (int z = (frameCountMax == 0) ? 6 : 12; z >= textGlowBrightness; z--)
	{
		setjasondelay(frameCountMax);
		tempScreenSeg = VGAScreenSeg; /* sega000 */
		JE_outTextAdjust(x, y, s, c, z - 10, textGlowFont, false);
		if (JE_anyButton())
		{
			frameCountMax = 0;
		}

		network::keep_alive();
		
		JE_showVGA();

		int delaycount_temp;
		if ((delaycount_temp = target - SDL_GetTicks()) > 0)
			SDL_Delay(delaycount_temp);
	}
	textGlowBrightness = 6;
}
