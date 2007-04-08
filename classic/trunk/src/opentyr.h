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
#ifndef OPENTYRIAN_H
#define OPENTYRIAN_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h> /* For the ROUND() macro */

#if __STDC_VERSION__ >= 199901L
#define INLINE inline
#elif defined __GNUC__
#define INLINE __inline__
#else
#define INLINE
#endif

#define ROUND(x) (floor((x)+0.5))

/* Gets number of elements in an array.
 * !!! USE WITH ARRAYS ONLY !!! */
#define COUNTOF(x) (sizeof(x) / sizeof *(x))

#define TRUE 1
#define FALSE 0

typedef signed long    JE_longint;
typedef signed short   JE_integer;
typedef signed char    JE_shortint;
typedef unsigned short JE_word;
typedef unsigned char  JE_byte;
typedef int            JE_boolean;

typedef char          *JE_string;
typedef char           JE_char;

typedef double         JE_real;

#endif /* OPENTYRIAN_H */
