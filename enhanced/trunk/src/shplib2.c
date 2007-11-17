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

#include "newshape.h"
#include "pcxload.h"
#include "starfade.h"
/*#include "vga256c.h"*/
#include "vga256d.h"

#define NO_EXTERNS
#include "shplib2.h"
#undef NO_EXTERNS


JE_word shapex [MAXIMUM_SHAPE];
unsigned long memory;
JE_word shapey [MAXIMUM_SHAPE];
JE_word maxshape;
JE_boolean shapexist [MAXIMUM_SHAPE];
void * shapearray [MAXIMUM_SHAPE];
char* f;

void JE_purgeShapes( void )
{
	STUB();
}

void JE_drawShapeNumS( JE_word shape, JE_word x, JE_word y )
{
	STUB();
}

void JE_drawShapeNum( JE_word shape, JE_word x, JE_word y )
{
	STUB();
}

void JE_loadShapes( const char *shapefile )
{
	STUB();
}

void JE_drawShape( void* shape, JE_word shapex, JE_word shapey, JE_word x, JE_word y )
{
	STUB();
}

void JE_drawShapeS( void* shape, JE_word shapex, JE_word shapey, JE_word x, JE_word y )
{
	STUB();
}
