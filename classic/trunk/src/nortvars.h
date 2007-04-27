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
#ifndef NORTVARS_H
#define NORTVARS_H

#include "opentyr.h"

typedef JE_char JE_TextBuffer[4096]; /* [1..4096] */

struct JE_ColorRec {
	JE_byte r, g, b;
};

typedef struct JE_ColorRec JE_ColorType[256]; /* [0..255] */

typedef JE_byte JE_ShapeTypeOne[168]; /* [0..168-1] */
typedef JE_ShapeTypeOne JE_ShapeType[304]; /* [1..304] */

typedef JE_byte JE_NewShapeTypeOne[182]; /* [0..168+14-1] */
typedef JE_NewShapeTypeOne JE_NewShapeType[304]; /* [1..304] */

#ifndef NO_EXTERNS
extern JE_boolean scanForJoystick;
extern JE_boolean inputDetected;
extern JE_word lastMouseX, lastMouseY;
extern JE_byte mouseCursor;
extern JE_boolean mouse_threeButton;
extern JE_word mouseX, mouseY, mouseButton;
#endif

void JE_loadShapeFile( JE_ShapeType *shapes, JE_char s );
void JE_loadNewShapeFile( JE_NewShapeType *shapes, JE_char s );

void JE_compressShapeFileC( JE_char s );
void JE_loadCompShapes( JE_byte **shapes, JE_word *shapeSize, JE_char s );

JE_word JE_mousePosition( JE_word *mouseX, JE_word *mouseY );
JE_boolean JE_buttonPressed( void );
void JE_setMousePosition( JE_word mouseX, JE_word mouseY );

JE_boolean JE_anyButton( void );

/*void JE_showMouse( void );
void JE_hideMouse( void );*/

void JE_dBar( JE_word x, JE_word y, JE_word num, JE_word col );
void JE_dBar2( JE_word x, JE_word y, JE_word num, JE_word col );
void JE_dBar3( JE_integer x,  JE_integer y,  JE_integer num,  JE_integer col );
void JE_dBar4( JE_word x, JE_word y, JE_word num, JE_word col );
void JE_barDraw( JE_word x, JE_word y, JE_word res, JE_word col, JE_word amt, JE_word xsize, JE_word ysize );
void JE_barDrawShadow( JE_word x, JE_word y, JE_word res, JE_word col, JE_word amt, JE_word xsize, JE_word ysize );
void JE_barDrawDirect( JE_word x, JE_word y, JE_word res, JE_word col, JE_word amt, JE_word xsize, JE_word ysize );
void JE_wipeKey( void );
void JE_drawANSI( char *ansiscreen );
/*JE_boolean JE_waitaction( JE_byte time );*/

void JE_drawShape2( JE_word x, JE_word y, JE_word s, JE_byte *shape );
void JE_superDrawShape2( JE_word x, JE_word y, JE_word s, JE_byte *shape );
void JE_drawShape2Shadow( JE_word x, JE_word y, JE_word s, JE_byte *shape );

void JE_drawShape2x2( JE_word x, JE_word y, JE_word s, JE_byte *shape );
void JE_superDrawShape2x2( JE_word x, JE_word y, JE_word s, JE_byte *shape );
void JE_drawShape2x2Shadow( JE_word x, JE_word y, JE_word s, JE_byte *shape );

#endif /* NORTVARS_H */
