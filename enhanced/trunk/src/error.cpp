/* vim: set noet:
 *
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
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
#include "vga256d.h"

#include "error.h"

#include <errno.h>
#include <stdio.h>
#include <string>
#include <iostream>

JE_word randomcount;
char dir[256]; /* increase me */

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

void JE_errorHand( const char *s )
{
	if (errorActive)
	{
		JE_closeVGA256();
		printf("WARNING: Unable to find Tyrian data files.\n"
		       "Stopped on file %s.\n"
		       "OpenTyrian needs the Tyrian data files to run. Please read the README file.\n\n", s);
		exit(1);
	} else {
		errorOccurred = 1;
	}
}

bool JE_find( const char *s )
{
	FILE *f;

	if ((f = fopen(s, "r")))
	{
		fclose(f);
		return true;
	} else {
		return false;
	}
}

void JE_findTyrian( const char *filename )
{
	std::string strbuf;

	if (JE_find(filename))
	{
		dir[0] = '\0';
	} else {
		/* Let's find it! */
		std::cout << "Searching for Tyrian files...\n\n";

		for (int i = 0; i < COUNTOF(tyrian_searchpaths); i++)
		{			
			strbuf = std::string(tyrian_searchpaths[i]) + "/" + filename;
			if (JE_find(strbuf.c_str()))
			{				
				sprintf(dir, "%s/", tyrian_searchpaths[i]);
				std::cout << "Tyrian data files found at " << dir << "\n\n";
				return;
			}
		}
	}
}

std::string JE_locateFile( const char *filename ) /* !!! WARNING: Non-reentrant !!! */
{
	std::string buf;

	if (JE_find(filename))
	{
		buf = std::string(filename);
	} else {
		if (strcmp(dir, "") == 0 && errorActive)
		{
			JE_findTyrian(filename);
		}

		buf = std::string(dir) + filename;
		if (!JE_find(buf.c_str()))
		{
			if (dont_die)
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

void JE_resetText( FILE **f, const char *filename )
{
	std::string tmp = JE_locateFile(filename);

	*f = tmp.empty() ? NULL : fopen_check(tmp.c_str(), "r");
}

bool JE_isCFGThere( void ) /* Warning: It actually returns false when the config file exists */
{
	FILE *f;

	dont_die = true;
	JE_resetFile(&f, "tyrian.cfg");
	dont_die = false;

	if (f && get_stream_size(f) == 17 + sizeof(keySettings) + sizeof(joyButtonAssign))
	{
		fclose(f);
		return false;
	} else {
		printf("\nInvalid or missing TYRIAN.CFG! Continuing using defaults.\n");
		return true;
	}
}
