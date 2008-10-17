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

#include "config.h"
#include "joystick.h"
#include "video.h"
#include "console/Console.h"

#include "error.h"

#include <errno.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

JE_word randomcount;
std::string dir;

bool errorActive = true;
bool errorOccurred = false;

bool dont_die = false;

static const char *tyrian_searchpaths[] = { "data", "tyrian", "tyrian2k" };

long get_stream_size( FILE *f )
{
	long size = 0;
	long pos;

	pos = ftell(f);

	fseek(f, 0, SEEK_END);
	size = ftell(f);

	fseek(f, pos, SEEK_SET);

	return size;
}

FILE *fopen_check( const char *file, const char *mode )
{
	char buf[64];
	FILE *f;

	errno = 0;	
	f = fopen(file, mode);
	if (!f)
	{
		switch (errno)
		{
			case EACCES:
				strcpy(buf, "Access denied");
				break;
			default:
				strcpy(buf, "Unknown error");
				break;
		}
		std::cout << "ERROR opening " << file << ": " << buf << "\n";
		return NULL;
	}

	return f;
}


unsigned long JE_getFileSize( const char *filename )
{
	FILE *f;
	unsigned long size = 0;

	errorActive = false;
	f = fopen(JE_locateFile(filename).c_str(), "rb");
	errorActive = true;
	if (errorOccurred)
	{
		errorOccurred = false;
		return 0;
	}

	fseek(f, 0, SEEK_END);
	size = ftell(f);

	fclose(f);

	return size;
}

void JE_errorHand( const std::string& s )
{
	if (errorActive)
	{
		show_datafile_error(s);
	} else {
		errorOccurred = 1;
	}
}

void show_datafile_error( const std::string& filename )
{
	deinit_video();
	Console::get() << "WARNING: Unable to find Tyrian data files." << std::endl
		<< "Stopped on file " << filename << std::endl
		<< "OpenTyrian needs the Tyrian data files to run. Please read the README file." << std::endl;
	exit(1);
}

bool JE_find( const std::string& s )
{
	FILE *f = fopen(s.c_str(), "r");

	if (f)
	{
		fclose(f);
		return true;
	} else {
		return false;
	}
}

void JE_findTyrian( const std::string& filename )
{
	std::string strbuf;

	if (JE_find(filename))
	{
		dir[0] = '\0';
	} else {
		/* Let's find it! */
		Console::get() << "Searching for Tyrian files...\n" << std::endl;

		for (unsigned int i = 0; i < COUNTOF(tyrian_searchpaths); i++)
		{			
			strbuf = std::string(tyrian_searchpaths[i]) + "/" + filename;
			if (JE_find(strbuf))
			{				
				dir = std::string(tyrian_searchpaths[i]) + "/";
				Console::get() << "Tyrian data files found at " << dir << "\n" << std::endl;
				return;
			}
		}
	}
}

std::string JE_locateFile( const std::string& filename, bool die )
{
	std::string buf;

	if (JE_find(filename))
	{
		buf = std::string(filename);
	} else {
		if (dir.empty() && errorActive)
		{
			JE_findTyrian(filename);
		}

		buf = std::string(dir) + filename;
		if (!JE_find(buf.c_str()))
		{
			if (!die || dont_die)
			{
				return std::string("");
			}
			errorActive = true;
			JE_errorHand(filename);
		}

	}

	return buf;
}

void JE_resetFile( FILE **f, const char *filename )
{
	std::string tmp = JE_locateFile(filename);

	*f = tmp.empty() ? NULL : fopen_check(tmp.c_str(), "rb");
}

void open_datafile_fail( std::ifstream& stream, const std::string& filename )
{
	try
	{
		open_datafile(stream, filename);
	} catch (FileOpenErrorException&) {
		show_datafile_error(filename);
	}
}

void open_datafile( std::ifstream& stream, const std::string& filename ) throw (FileOpenErrorException)
{
	std::string path = JE_locateFile(filename, false);
	if (path.empty()) throw FileOpenErrorException(filename);
	stream.open(path.c_str(), std::ios_base::in | std::ios_base::binary);
	if (stream.fail()) throw FileOpenErrorException(filename);
}

void JE_resetText( FILE **f, const char *filename )
{
	std::string tmp = JE_locateFile(filename);

	*f = tmp.empty() ? NULL : fopen_check(tmp.c_str(), "r");
}
