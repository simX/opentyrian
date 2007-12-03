/* vim: set noet:
 *
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
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
#ifndef NEWSHAPE_H
#define NEWSHAPE_H

#include "opentyr.h"

#include "SDL.h"


#define OLD_BLACK         0
#define NEW_BLACK         253

/* Font faces */
#define PLANET_SHAPES     0
#define FONT_SHAPES       1
#define SMALL_FONT_SHAPES 2
#define FACE_SHAPES       3
#define OPTION_SHAPES     4 /*Also contains help shapes*/
#define TINY_FONT         5
#define WEAPON_SHAPES     6
#define EXTRA_SHAPES      7 /*Used for Ending pics*/

#define MAXIMUM_SHAPE     151
#define MAX_TABLE         8

typedef Uint8 *JE_ShapeArrayType[MAX_TABLE][MAXIMUM_SHAPE]; /* [1..maxtable, 1..maximumshape] */

extern SDL_Surface *tempScreenSeg;
extern JE_ShapeArrayType *shapeArray;
extern JE_word shapeX[MAX_TABLE][MAXIMUM_SHAPE],
               shapeY[MAX_TABLE][MAXIMUM_SHAPE];
extern JE_word shapeSize[MAX_TABLE][MAXIMUM_SHAPE];
extern bool shapeExist[MAX_TABLE][MAXIMUM_SHAPE];
extern int maxShape[MAX_TABLE];
extern Uint8 mouseGrabShape[24*28];
extern bool loadOverride;
/*extern JE_word min, max;*/

void JE_newLoadShapes( int table, char *shapefile );
void JE_newLoadShapesB( int table, FILE *f );
void JE_newCompressBlock( Uint8 **shape, JE_word xsize, JE_word ysize, JE_word *shapesize );
void JE_newDrawShape( Uint8 *shape, JE_word xsize, JE_word ysize );
void JE_newDrawCShape( Uint8 *shape, JE_word xsize, JE_word ysize );
void JE_newDrawCShapeNum( int table, int shape, JE_word x, JE_word y );
void JE_newPurgeShapes( int table );
/*void JE_OverrideLoadShapes( int table, char *shapefile, JE_word minimum, JE_word maximum );*/

void JE_drawShapeTypeOne( JE_word x, JE_word y, Uint8 *shape );
void JE_grabShapeTypeOne( JE_word x, JE_word y, Uint8 *shape );

bool JE_waitAction( int time, bool checkjoystick );
void JE_mouseStart( void );
void JE_mouseReplace( void );

void newshape_init( void );

void JE_drawNext( Uint8 draw );
void JE_drawNShape (void *shape, JE_word xsize, JE_word ysize);

#endif /* NEWSHAPE_H */
