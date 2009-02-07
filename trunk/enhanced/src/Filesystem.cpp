/* vim: set noet:
 *
 * OpenTyrian Enhanced: A modern cross-platform port of Tyrian
 * Copyright (C) 2007-2009  The OpenTyrian Development Team
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
#include "Filesystem.h"

#include "console/CCmd.h"
#include "video.h"

#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;

Filesystem::Filesystem()
{
	Console::get() << "Initializing filesystem..." << std::endl;
}

bool Filesystem::fileExists( const std::string& filename ) const
{
	try
	{
		findFile(filename);
	}
	catch (FileOpenErrorException&)
	{
		return false;
	}

	return true;
}

std::string Filesystem::findFile( const std::string& filename ) const
{
	if (CVars::cfg_use_home)
	{
		fs::path path(getHomeDir());
		path /= filename;
		if (fs::exists(path) && fs::is_regular(path))
		{
			return path.file_string();
		}
	}

	const fs::path path(filename);
	if (fs::exists(path) && fs::is_regular(path))
	{
		return path.file_string();
	}

	throw FileOpenErrorException(filename);
}

std::string Filesystem::findDatafile( const std::string& filename ) const
{
	foreach (std::string i, searchPaths)
	{
		fs::path path(i);
		path /= filename;
		if (fs::exists(path) && fs::is_regular(path))
		{
			return path.file_string();
		}
	}

	throw FileOpenErrorException(filename);
}

// TODO: If this is used anywhere else, move messages to ccmd.
void Filesystem::addSearchPath( std::string filename )
{
	fs::path path(filename);

	if (fs::exists(path) && fs::is_directory(path))
	{
		Console::get() << "\acAdding \"" << filename << "\" to search paths." << std::endl;
		searchPaths.push_front(path.directory_string());
	}
	else
	{
		Console::get() << "\a7Error:\ax Failed to add \"" << filename << "\" to search paths." << std::endl;
	}
}

const std::list<std::string>& Filesystem::getSearchPaths( ) const
{
	return searchPaths;
}

void Filesystem::openDatafileFail( std::fstream& stream, const std::string& filename ) const
{
	try
	{
		openDatafile(stream, filename);
	} catch (FileOpenErrorException&) {
		showDatafileError(filename);
	}
}

void Filesystem::openDatafile( std::fstream& stream, const std::string& filename ) const
{
	std::string path;
	try
	{
		path = findDatafile(filename);
	}
	catch (FileOpenErrorException&)
	{
		throw FileOpenErrorException(filename);
	}

	stream.clear();
	stream.open(path.c_str(), std::ios_base::in | std::ios_base::binary);
	if (stream.fail())
		throw FileOpenErrorException(filename);
}

void Filesystem::showDatafileError( const std::string& filename )
{
	deinit_video();
	Console::get() << "\nERROR: Unable to find Tyrian data file: " << filename << '\n'
		<< "OpenTyrian needs the Tyrian data files to run. Please read the README file." << std::endl;
	exit(1);
}



std::string Filesystem::getHomeDir( )
{
	std::string path;
#ifdef _POSIX_VERSION
	path = std::string(getenv("HOME")) + "/.opentyrian/enhanced/";
#elif defined(_WIN32)
	path = std::string(getenv("APPDATA")) + "\\OpenTyrian Enhanced\\";
#else
#define HOMEDIR_NOT_SUPPORTED
#endif

	if (CVars::cfg_use_home)
	{
		// Create directory if it doesn't exist.
		fs::create_directory(path);
		return path;
	}
	else
	{
		return "./";
	}
}

static bool home_dir_callback( const bool& val )
{
#ifdef HOMEDIR_NOT_SUPPORTED
	return false;
#else
	return val;
#endif
}
#undef HOMEDIR_NOT_SUPPORTED

namespace CVars
{
	CVarBool cfg_use_home("cfg_use_home", CVar::NONE, "Use the user's home directory for saving. Not supported on all platforms.", false, home_dir_callback);
}

namespace CCmds
{
	namespace Func
	{
		static void add_search_path( const std::vector<std::string>& params )
		{
			std::string str = CCmd::convertParam<std::string>(params, 0);

			Filesystem::get().addSearchPath(str);
		}
	}

	CCmd add_search_path("add_search_path", CCmd::NONE, "Adds a directory to the game's datafile search path.", Func::add_search_path);
}

