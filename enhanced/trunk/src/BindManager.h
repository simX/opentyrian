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
#ifndef BINDMANAGER_H
#define BINDMANAGER_H

#include "Singleton.h"

#include "SDL.h"
#include <stdexcept>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

struct Bind
{
	Bind() {}
	Bind( SDLKey key, std::string command, bool toggle )
		: key(key), command(command), toggle(toggle)
	{}

	SDLKey key;
	std::string command;
	bool toggle;
};

class BindManager : public Singleton<BindManager>
{
private:
	std::map<SDLKey, Bind> bindMap;
public:
	class UnknownBindError : public std::runtime_error
	{
	public:
		UnknownBindError( const std::string& error ) : runtime_error(error) {}
	};

	const Bind& getBind( SDLKey key ) const;
	const Bind* findBind( const std::string& cmd ) const;
	void runBind( SDLKey key, bool press = true );
	void addBind( SDLKey key, std::string cmd );
	void addBind( std::string key_name, const std::string& cmd );
	void removeBind( SDLKey key );
	void removeBind( std::string key_name );
};

#endif // BINDMANAGER_H
