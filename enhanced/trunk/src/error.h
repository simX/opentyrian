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
#ifndef ERROR_H
#define ERROR_H

#include "opentyr.h"

#include <string>
#include <stdexcept>

extern JE_word randomcount;
extern bool dont_die;
extern std::string dir;
extern bool errorActive;
extern bool errorOccurred;

class FileOpenErrorException : public std::runtime_error
{
public:
	FileOpenErrorException( const std::string& file ) : runtime_error(std::string("Error opening file: ") + file) {}
};

long get_stream_size( FILE *f );
FILE *fopen_check( const char *file, const char *mode );

void JE_errorHand( const std::string& s );
bool JE_find( const std::string& s );
void JE_resetFile( FILE **f, const char *filename );
void JE_resetText( FILE **f, const char *filename );
std::string JE_locateFile( const std::string& filename );
void JE_findTyrian( const std::string& filename );
bool JE_isCFGThere( void );
unsigned long JE_getFileSize( const char *filename );
void open_datafile( std::ifstream& stream, const std::string& filename  );

#endif /* ERROR_H */
