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
#ifndef PCXMAST_H
#define PCXMAST_H

#include "opentyr.h"

#define PCXnum 13

typedef JE_longint JE_pcxpostype[PCXnum + 1];	/* [1..PCXnum + 1] */

#define PCXMAST_EXTERNS \
extern const JE_string pcxfile[PCXnum];	/* [1..PCXnum] */ \
extern const JE_byte pcxpal[PCXnum];	/* [1..PCXnum] */ \
extern const JE_byte facepal[12];	/* [1..12] */ \
extern JE_pcxpostype pcxpos;

#endif /* PCXMAST_H */
