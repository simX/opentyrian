/* vim: set noet:
 *
 * OpenTyrian Enhanced: A modern cross-platform port of Tyrian
 * Copyright (C) 2007-2009  The OpenTyrian Development Team
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


typedef char JE_TextBuffer[4096]; /* [1..4096] */

typedef SDL_Color JE_ColorType[256]; /* [0..255] */

typedef Uint8 JE_ShapeTypeOne[168]; /* [0..168-1] */
typedef JE_ShapeTypeOne JE_ShapeType[304]; /* [1..304] */

typedef Uint8 JE_NewShapeTypeOne[182]; /* [0..168+14-1] */
typedef JE_NewShapeTypeOne JE_NewShapeType[304]; /* [1..304] */

extern bool inputDetected;
extern JE_word lastMouseX, lastMouseY;
extern int mouseCursor;
extern bool mouse_threeButton;
extern JE_word mouseX, mouseY, mouseButton;

void JE_loadShapeFile( JE_ShapeType *shapes, char s );
void JE_loadNewShapeFile( JE_NewShapeType *shapes, char s );

void JE_compressShapeFileC( char s );
void JE_loadCompShapes( Uint8 **shapes, unsigned long *shapeSize, char s );

JE_word JE_btow(Uint8 a, Uint8 b);

JE_word JE_mousePosition( JE_word *mouseX, JE_word *mouseY );
bool JE_buttonPressed( void );
void JE_setMousePosition( JE_word mouseX, JE_word mouseY );

bool JE_anyButton( void );

void JE_dBar( JE_word x, JE_word y, JE_word num, JE_word col );
void JE_dBar2( JE_word x, JE_word y, JE_word num, JE_word col );
void JE_dBar3( int x,  int y,  int num,  int col );
void JE_dBar4( JE_word x, JE_word y, JE_word num, JE_word col );
void JE_barDraw( JE_word x, JE_word y, JE_word res, JE_word col, JE_word amt, JE_word xsize, JE_word ysize );
void JE_barDrawShadow( JE_word x, JE_word y, JE_word res, JE_word col, JE_word amt, JE_word xsize, JE_word ysize );
void JE_barDrawDirect( JE_word x, JE_word y, JE_word res, JE_word col, JE_word amt, JE_word xsize, JE_word ysize );
void JE_drawANSI( char *ansiscreen );

void JE_drawShape2( int x, int y, int s, Uint8 *shape );
void JE_superDrawShape2( int x, int y, int s, Uint8 *shape );
void JE_drawShape2Shadow( int x, int y, int s, Uint8 *shape );

void JE_drawShape2x2( int x, int y, int s, Uint8 *shape );
void JE_superDrawShape2x2( int x, int y, int s, Uint8 *shape );
void JE_drawShape2x2Shadow( int x, int y, int s, Uint8 *shape );

#endif /* NORTVARS_H */
