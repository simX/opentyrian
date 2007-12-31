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
#ifndef PCXLOAD_H
#define PCXLOAD_H

#include "opentyr.h"

#include "nortvars.h"
#include "error.h"


struct JE_PCXHeader_rec {
	Uint8 manufacturer;
	Uint8 version;
	Uint8 encoding;
	Uint8 bits_per_pixel;
	JE_word xmin, ymin;
	JE_word xmax, ymax;
	JE_word hres, vres;
	Uint8 palette[48];  /* [0..47] */
	Uint8 reserved;
	Uint8 colour_planes;
	JE_word bytes_per_line;
	JE_word palette_type;
};

extern JE_word width, depth;
extern JE_word bytes;
extern char /*c,*/ c2;
extern bool overrideBlack;

/*void JE_unpackPCX( void );*/

void JE_loadPCX( const char *name, bool storePalette );
void JE_updatePCXColorsSlow( JE_ColorType *colorBuffer );

#endif /* PCXLOAD_H */
