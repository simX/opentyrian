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
#ifndef ERROR_H
#define ERROR_H

#include "opentyr.h"

#ifndef NO_EXTERNS
extern JE_boolean dont_die;
extern JE_char dir[12];
extern JE_boolean errorActive;
extern JE_boolean errorOccurred;
extern char err_msg[128];
#endif

long get_stream_size( FILE *f );
FILE *fopen_check( const char *file, const char *mode );

void JE_errorHand( const char *s );
JE_boolean JE_find( const char *s );
void JE_resetFile( FILE **f, const char *filename );
void JE_resetText( FILE **f, const char *filename );
char *JE_locateFile( const char *filename );
void JE_findTyrian( const char *filename );
JE_boolean JE_isCFGThere( void );
/* void JE_OutputString( char *s ); Obscure DOS trick (?) */
JE_longint JE_getFileSize( const char *filename );

#endif /* ERROR_H */
