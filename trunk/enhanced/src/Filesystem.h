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
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Singleton.h"
#include "console/cvar/CVarTypes.h"
#include "console/CCmd.h"

#include <string>
#include <fstream>
#include <stdexcept>

class Filesystem : public Singleton<Filesystem>
{
public:
	class FileOpenErrorException : public std::runtime_error
	{
	private:
		const std::string filename;
	public:
		FileOpenErrorException( const std::string& file )
			: runtime_error(std::string("Error opening file: ") + file), filename(file)
		{ }

		~FileOpenErrorException( ) throw() {}

		virtual const std::string& getFilename( ) const
		{
			return filename;
		}
	};

	Filesystem( );

	bool fileExists( const std::string& filename ) const;
	std::string findFile( const std::string& filename ) const;
	std::string findDatafile( const std::string& filename ) const;
	static std::string getHomeDir( );

	void addSearchPath( std::string filename );
	const std::list<std::string>& getSearchPaths( ) const;

	void openDatafileFail( std::fstream& stream, const std::string& filename ) const;
	void openDatafile( std::fstream& stream, const std::string& filename ) const;

private:
	static void showDatafileError( const std::string& filename );

	std::list<std::string> searchPaths;
};

namespace CVars
{
	extern CVarBool cfg_use_home;
}

#endif // FILESYSTEM_H
