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
#ifndef STARLIB_H
#define STARLIB_H

#include "opentyr.h"


#define starlib_MAX_STARS 1000
#define MAX_TYPES 14

struct JE_StarType
{
	int spX, spY, spZ;
	int lastX, lastY;
};

#ifndef NO_EXTERNS
#endif

void JE_starlib_main( void );
void JE_makeGray( void );
void JE_wackyCol( void );
void JE_starlib_init( void );
void JE_resetValues( void );
void JE_changeSetup( JE_byte setupType );
void JE_newStar( void );

#endif /* STARLIB_H */
