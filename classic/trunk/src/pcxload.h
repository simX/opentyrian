/*
 * Jumpers Editor++: A cross-platform and extendable editor and player for the Jumper series of games;
 * Copyright (C) 2007  Yuri K. Schlesner
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
#include "nortvars.h"

struct JE_pcxheader {
    JE_byte manufacturer;
    JE_byte version;
    JE_byte encoding;
    JE_byte bits_per_pixel;
    JE_word xmin, ymin;
    JE_word xmax, ymax;
    JE_word hres, vres;
    JE_byte palette[48];  /* [0..47] */
    JE_byte reserved;
    JE_byte colour_planes;
    JE_word Bytes_per_line;
    JE_word palette_Type;
};

#define PCXLOAD_EXTERNS \
extern JE_ColorType Colors2; \
extern JE_word width, \
               depth; \
extern JE_word Bytes; \
extern JE_char c, c2; \
extern JE_boolean overrideblack;

void JE_LoadPCX( JE_string Name, JE_boolean storepalette );
void JE_UpdatePCXColorsSlow( JE_ColorType *ColorBuffer );
