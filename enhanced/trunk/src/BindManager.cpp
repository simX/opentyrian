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
#include "BindManager.h"

#include "Console.h"
#include "KeyNames.h"
#include "CCmd.h"

#include <sstream>

namespace CCmds
{
	namespace Func
	{
		static void bind( const std::vector<std::string>& params )
		{
			std::string param1 = CCmd::convertParam<std::string>(params, 0);
			std::string param2 = CCmd::convertParam<std::string>(params, 1);

			BindManager::get().addBind(param1, param2);
		}

		static void unbind( const std::vector<std::string>& params )
		{
			std::string param1 = CCmd::convertParam<std::string>(params, 0);

			BindManager::get().removeBind(param1);
		}
	}

	CCmd bind("bind", CCmd::NONE, "Binds a key to a command. Prefix the command with + to make it act as a toggle. Usage: bind [key name] [command]", Func::bind);
	CCmd unbind("unbind", CCmd::NONE, "Unbinds all commands from a key. Usage: [key name]", Func::unbind);
}


const Bind& BindManager::getBind( SDLKey key ) const
{
	std::map<SDLKey, Bind>::const_iterator p = bindMap.find(key);
	if (p == bindMap.end()) {
		std::ostringstream s;
		s << key;
		throw UnknownBindError(s.str());
	}

	return p->second;
}

const Bind* BindManager::findBind( const std::string& cmd ) const
{
	for (std::map<SDLKey, Bind>::const_iterator i = bindMap.begin(); i != bindMap.end(); ++i) {
		if (i->second.command == cmd) {
			return &i->second;
		}
	}
	return 0;
}

void BindManager::runBind( SDLKey key, bool press )
{
	try {
		const Bind& bind = getBind(key);
	
		if (bind.toggle) {
			std::string s = bind.command + (press ? " true" : " false");
			Console::get().runCommand(s);
		} else {
			if (press) {
				Console::get().runCommand(bind.command);
			}
		}
	} catch (UnknownBindError&) {
		// Do nothing if there wasn't anything binded to the key
	}
}

void BindManager::addBind( SDLKey key, std::string cmd )
{
	if (cmd.empty()) return;

	bool toggle = false;
	if (cmd[0] == '+') {
		toggle = true;
		cmd.erase(0,1);
	}

	Bind bind(key, cmd, toggle);
	bindMap[key] = bind;
}

void BindManager::addBind( std::string key_name, const std::string& cmd )
{
	addBind(KeyNames::get().getKeyFromName(key_name), cmd);
}

void BindManager::removeBind( SDLKey key )
{
	std::map<SDLKey, Bind>::iterator p = bindMap.find(key);
	if (p != bindMap.end()) {
		bindMap.erase(p);
	}
}

void BindManager::removeBind( std::string key_name )
{
	removeBind(KeyNames::get().getKeyFromName(key_name));
}
	 