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
#include "KeyNames.h"

#include "SDL.h"
#include <stdexcept>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <stdexcept>
#include <sstream>

struct BindCommand
{
	BindCommand() {}
	BindCommand( const std::string& cmd, bool toggle )
		: command(cmd), toggle(toggle)
	{}

	std::string command;
	bool toggle;

	void operator() ( bool press = true ) const;
};

class BindCommandSort
{
public:
	bool operator() ( const BindCommand& a, const BindCommand& b ) const
	{
		if (a.toggle < b.toggle) return true;
		if (a.toggle > b.toggle) return false;
		return a.command < b.command;
	}
};

struct Bind
{
	virtual ~Bind() {}

	void addCommand( const std::string& cmd, bool toggle )
	{
		commands.insert(BindCommand(cmd, toggle));
	}

	virtual std::string getKeyDescription( ) const = 0;

	typedef std::set<BindCommand, BindCommandSort> SetType;
	SetType commands;
};

struct KeyBind : public Bind
{
	KeyBind() {}
	KeyBind( SDLKey key )
		: key(key)
	{}

	KeyBind( SDLKey key, const std::string& command, bool toggle )
		: key(key)
	{
		addCommand(command, toggle);
	}

	std::string getKeyDescription( ) const
	{
		return KeyNames::get().getNameFromKey(key);
	}

	SDLKey key;
};

struct MouseBind : public Bind
{
	MouseBind() {}
	MouseBind( Uint8 button )
		: button(button)
	{}

	MouseBind( Uint8 button, const std::string& command, bool toggle )
		: button(button)
	{
		addCommand(command, toggle);
	}

	std::string getKeyDescription( ) const
	{
		std::ostringstream str;
		str << "mouse" << int(button);
		return str.str();
	}

	Uint8 button;
};

class BindManager : public Singleton<BindManager>
{
private:
	class BindPtrSort
	{
	public:
		bool operator() ( const Bind* a, const Bind* b ) const
		{
			return a->getKeyDescription() < b->getKeyDescription();
		}
	};

	std::map<SDLKey, KeyBind*> bindMap;
	std::map<Uint8, MouseBind*> mouseBindMap;

public:
	typedef std::set<Bind*, BindPtrSort> SetType;
private:
	SetType binds;
public:
	class UnknownBindError : public std::runtime_error
	{
	public:
		UnknownBindError( const std::string& error ) : runtime_error(error) {}
	};

	KeyBind& getBind( SDLKey key ) const;
	MouseBind& getBindMouse( Uint8 button ) const;
	Bind* getBind( const std::string& name ) const;
	Bind* findBind( const std::string& cmd ) const;
	std::set<Bind*> findBinds( const std::string& cmd ) const;

	void runBind( SDLKey key, bool press = true );
	void runBindMouse( unsigned int button, bool press = true );

	void addBind( SDLKey key, const std::string& cmd, bool toggle );
	void addBind( SDLKey key, std::string cmd );
	void addBind( const std::string& key_name, const std::string& cmd );
	void addBindMouse( Uint8 button, const std::string& cmd, bool toggle );
	void addBindMouse( Uint8 button, std::string cmd );

	void removeBind( Bind* bind );
	void removeBind( Bind* bind, std::string cmd, bool toggle );
	void removeBind( SDLKey key );
	void removeBindMouse( Uint8 button );
	void removeBind( const std::string& key_name );

	friend static void bind( const std::vector<std::string>& params );
};

#endif // BINDMANAGER_H
