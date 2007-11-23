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
#ifndef FONTHAND_H
#define FONTHAND_H

#include "opentyr.h"


#define PART_SHADE 0
#define FULL_SHADE 1
#define DARKEN     2
#define TRICK      3
#define NO_SHADE 255

extern const unsigned char fontMap[136];
extern int defaultBrightness;
extern int textGlowFont, textGlowBrightness;
extern bool levelWarningDisplay;
extern int levelWarningLines;
extern char levelWarningText[10][61];
extern bool warningRed;
extern int warningSoundDelay;
extern JE_word armorShipDelay;
extern int warningCol;
extern int warningColChange;

void JE_dString( JE_word x, JE_word y, const char *s, int font );
void JE_newDrawCShapeBright( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y, int filter, int brightness );
void JE_newDrawCShapeShadow( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y );
void JE_newDrawCShapeDarken( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y );
void JE_newDrawCShapeDarkenNum( int table, int shape, int x, int y );
void JE_newDrawCShapeTrick( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y );
void JE_newDrawCShapeTrickNum( int table, int shape, int x, int y );
void JE_newDrawCShapeModify( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y, int filter, int brightness );
void JE_newDrawCShapeModifyNum( int table, int shape, int x, int y, int filter, int brightness );
void JE_newDrawCShapeAdjust( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y, int filter, int brightness );
void JE_newDrawCShapeAdjustNum( int table, int shape, int x, int y, int filter, int brightness );
void JE_newDrawCShapeBrightAndDarken( Uint8 *shape, JE_word xsize, JE_word ysize, int x, int y, int filter, int brightness );
/*void JE_newDrawCShapeZoom( int table, int shape, JE_word x, JE_word y, double scale );*/
JE_word JE_fontCenter( const char *s, int font );
JE_word JE_textWidth( const char *s, int font );
void JE_textShade( JE_word x, JE_word y, const char *s, int colorbank, int brightness, int shadetype );
void JE_outText( JE_word x, JE_word y, const char *s, int colorbank, int brightness );
void JE_outTextModify( JE_word x, JE_word y, const char *s, int filter, int brightness, int font );
void JE_outTextShade( JE_word x, JE_word y, const char *s, int font );
void JE_outTextAdjust( JE_word x, JE_word y, const char *s, int filter, int brightness, int font, bool shadow );
void JE_outTextAndDarken( JE_word x, JE_word y, const char *s, int colorbank, int brightness, int font );
char JE_bright( bool makebright );

void JE_updateWarning( void );
void JE_outTextGlow( JE_word x, JE_word y, const char *s );

#endif /* FONTHAND_H */
