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
#include "network.h"
#include "nortsong.h"
#include "nortvars.h"
#include "vga256d.h"
#include "Console.h"

#include "animlib.h"


JE_word currentpageofs;
JE_word currentpageseg;

void JE_loadPage( JE_word pagenumber )
{
	STUB();
}

void JE_drawFrame( JE_word framenumber )
{
	STUB();
}

JE_word JE_findPage ( JE_word framenumber )
{
	STUB();
	return 0;
}

void JE_renderFrame( JE_word framenumber )
{
	STUB();
}

void JE_playAnim( const char *animfile, int startingframe, bool keyhalt, int speed )
{
	STUB();
}

void JE_loadAnim( const char *filename )
{
	STUB();
}

void JE_closeAnim( void )
{
	STUB();
}

void JE_playRunSkipDump( JE_word bufferoffset )
{
	STUB();
}
