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
#ifndef KEYNAMES_H
#define KEYNAMES_H

#include "SDL.h"

#include "Singleton.h"

#include "boost/bimap.hpp"
#include <map>
#include <stdexcept>

class KeyNames : public Singleton<KeyNames>
{
public:
	class UnknownKeyError : public std::runtime_error
	{
	public:
		UnknownKeyError( const std::string& error ) : runtime_error(error) {}
	};

	KeyNames( );

	std::string getNameFromKey( const SDLKey sym );
	SDLKey getKeyFromName( const std::string& name );
private:
	struct KeyName { const char* name; SDLKey key; };
	static const KeyName key_names[];

	typedef boost::bimap<std::string, SDLKey> BimapType;
	typedef BimapType::value_type EntryType;
	BimapType nameMap;
};

#endif // KEYNAMES_H
