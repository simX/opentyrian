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
#ifndef OPENTYR_H
#define OPENTYR_H

#include "SDL.h"
#include "SDL_endian.h"

#include "console/Console.h"

#ifdef _MSC_VER
#define __func__ __FUNCTION__
#endif

#define STUB() (Console::get() << "!!! STUB: " << __FILE__ << ":" << __LINE__ << ":" << __func__ << std::endl)

/* Gets number of elements in an array.
 * !!! USE WITH ARRAYS ONLY !!! */
#define COUNTOF(x) (sizeof(x) / sizeof *(x))

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923f
#endif

int ot_round( const float x );
float ot_abs( const float x );

#ifdef PASCAL_TYPES
//typedef Sint32 JE_longint; - Done
//typedef Sint16 JE_integer; - Done
//typedef Sint8  JE_shortint; - Done
typedef Uint16 JE_word;
typedef Uint8  JE_byte;
//typedef bool   JE_boolean; - Done
//typedef char   JE_char; - Done
//typedef double JE_real; - Done
#endif

extern std::string opentyrian_version;

char *strnztcpy( char *to, const char *from, size_t count );

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
size_t efread( void *buffer, size_t size, size_t num, FILE *stream );
size_t efwrite( void *buffer, size_t size, size_t num, FILE *stream );
#else
#define efread fread
#define efwrite fwrite
#endif

#define vfread(var, source, file) { \
	source tmptmp_src_var; \
	efread(&tmptmp_src_var, sizeof(source), 1, file); \
	var = tmptmp_src_var; \
}

#include "boost/foreach.hpp"
#define foreach BOOST_FOREACH

void opentyrian_menu( void );

#endif /* OPENTYR_H */
