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
#include <set>
#include <string>
#include <algorithm>

static void printBind( const Bind* const bind )
{
	Console::get() << '"' << bind->getKeyDescription() << '"';
	for (Bind::SetType::const_iterator i = bind->commands.begin(); i != bind->commands.end(); ++i) {
		Console::get() << " \"";
		if (i->toggle) Console::get() << '+';
		Console::get() << i->command << '"';
	}
	Console::get() << std::endl;
}

// This isn't in CCmds::Func because then I can't friend it in BindManager
void bind( const std::vector<std::string>& params )
{
	try {
		if (params.empty()) {
			const BindManager::SetType& bindSet = BindManager::get().binds;
			for (BindManager::SetType::const_iterator i = bindSet.begin(); i != bindSet.end(); ++i) {
				printBind(*i);
			}
		} else if (params.size() == 1) {
			std::string param1 = CCmd::convertParam<std::string>(params, 0);
			try {
				printBind(BindManager::get().getBind(param1));
			} catch (BindManager::UnknownBindError&) {}
		} else {
			std::string param1 = CCmd::convertParam<std::string>(params, 0);
			std::string param2 = CCmd::convertParam<std::string>(params, 1);

			BindManager::get().addBind(param1, param2);
		}
	} catch (KeyNames::UnknownKeyError& e) {
		throw CCmd::RuntimeCCmdError(std::string("Unknown key: ") + e.what());
	}
}

namespace CCmds
{
	namespace Func
	{
		static void unbind( const std::vector<std::string>& params )
		{
			std::string param1 = CCmd::convertParam<std::string>(params, 0);

			try {
				BindManager::get().removeBind(param1);
			} catch (KeyNames::UnknownKeyError& e) {
				throw CCmd::RuntimeCCmdError(std::string("Unknown key: ") + e.what());
			}
		}
	}

	CCmd bind("bind", CCmd::NONE, "Binds a key to a command. Prefix the command with + to make it act as a toggle. Usage: bind [key name] [command]", ::bind);
	CCmd unbind("unbind", CCmd::NONE, "Unbinds all commands from a key. Usage: [key name]", Func::unbind);
}

static int isMouse( const std::string& name )
{
	static const std::string str = "mouse";
	if (name.compare(0, str.size(), str) == 0) {
		std::istringstream stream(name.substr(str.size()));
		unsigned int val;

		if (stream >> val) {
			return val;
		}
	}
	return -1;
}

void BindCommand::operator() ( bool press ) const
{
	if (toggle) {
		std::string s = command + (press ? " true" : " false");
		Console::get().runCommand(s);
	} else {
		if (press) {
			Console::get().runCommand(command);
		}
	}
}

KeyBind& BindManager::getBind( SDLKey key ) const
{
	std::map<SDLKey, KeyBind*>::const_iterator p = bindMap.find(key);
	if (p == bindMap.end()) {
		std::ostringstream s;
		s << "key" << key;
		throw UnknownBindError(s.str());
	}

	return *p->second;
}

MouseBind& BindManager::getBindMouse( Uint8 button ) const
{
	std::map<Uint8, MouseBind*>::const_iterator p = mouseBindMap.find(button);
	if (p == mouseBindMap.end()) {
		std::ostringstream s;
		s << "mouse" << button;
		throw UnknownBindError(s.str());
	}

	return *p->second;
}

Bind* BindManager::getBind( const std::string& name ) const
{
	int mouse = isMouse(name);
	if (mouse != -1) {
		return &getBindMouse(mouse);
	} else {
		return &getBind(KeyNames::get().getKeyFromName(name));
	}
}
Bind* BindManager::findBind( const std::string& cmd ) const
{
	for (BindManager::SetType::const_iterator i = binds.begin(); i != binds.end(); ++i) {
		const Bind::SetType& cmds = (*i)->commands;
		for (Bind::SetType::const_iterator j = cmds.begin(); j != cmds.end(); ++j) {
			if (j->command == cmd) {
				return *i;
			}
		}
	}
	return 0;
}

std::set<Bind*> BindManager::findBinds( const std::string& cmd ) const
{
	std::set<Bind*> set;
	for (BindManager::SetType::const_iterator i = binds.begin(); i != binds.end(); ++i) {
		const Bind::SetType& cmds = (*i)->commands;
		for (Bind::SetType::const_iterator j = cmds.begin(); j != cmds.end(); ++j) {
			if (j->command == cmd) {
				set.insert(*i);
			}
		}
	}
	return set;
}

void BindManager::runBind( SDLKey key, bool press )
{
	try {
		const KeyBind& bind = getBind(key);

		for (Bind::SetType::const_iterator i = bind.commands.begin(); i != bind.commands.end(); ++i) {
			(*i)(press);
		}
	} catch (UnknownBindError&) {
		// Do nothing if there isn't anything binded to the key
	}
}

void BindManager::runBindMouse( unsigned int button, bool press )
{
	try {
		const MouseBind& bind = getBindMouse(button);

		for (Bind::SetType::const_iterator i = bind.commands.begin(); i != bind.commands.end(); ++i) {
			(*i)(press);
		}
	} catch (UnknownBindError&) {
		// Nothing
	}
}

void BindManager::addBind( SDLKey key, const std::string& cmd, bool toggle )
{
	if (cmd.empty()) return;

	std::map<SDLKey, KeyBind*>::iterator i = bindMap.find(key);
	if (i == bindMap.end()) {
		KeyBind* bind = new KeyBind(key, cmd, toggle);
		bindMap[key] = bind;
		binds.insert(bind);
	} else {
		i->second->addCommand(cmd, toggle);
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

	addBind(key, cmd, toggle);
}

void BindManager::addBindMouse( Uint8 button, const std::string& cmd, bool toggle )
{
	if (cmd.empty()) return;

	std::map<Uint8, MouseBind*>::iterator i = mouseBindMap.find(button);
	if (i == mouseBindMap.end()) {
		MouseBind* bind = new MouseBind(button, cmd, toggle);
		mouseBindMap[button] = bind;
		binds.insert(bind);
	} else {
		i->second->addCommand(cmd, toggle);
	}
}

void BindManager::addBindMouse( Uint8 button, std::string cmd )
{
	if (cmd.empty()) return;

	bool toggle = false;
	if (cmd[0] == '+') {
		toggle = true;
		cmd.erase(0,1);
	}

	addBindMouse(button, cmd, toggle);
}

void BindManager::addBind( const std::string& key_name, const std::string& cmd )
{
	int mouse = isMouse(key_name);
	if (mouse != -1) {
		addBindMouse(mouse, cmd);
	} else {
		addBind(KeyNames::get().getKeyFromName(key_name), cmd);
	}
}


void BindManager::removeBind( Bind* bind )
{
	if (binds.find(bind) != binds.end()) {
		binds.erase(bind);

		if (dynamic_cast<KeyBind*>(bind))
		{
			for (std::map<SDLKey, KeyBind*>::iterator i = bindMap.begin(); i != bindMap.end(); ++i) {
				if (i->second == bind) {
					bindMap.erase(i);
					break;
				}
			}
		} else if (dynamic_cast<MouseBind*>(bind)) {
			for (std::map<Uint8, MouseBind*>::iterator i = mouseBindMap.begin(); i != mouseBindMap.end(); ++i) {
				if (i->second == bind) {
					mouseBindMap.erase(i);
					break;
				}
			}
		}
		delete bind;
	}
}

void BindManager::removeBind( Bind* bind, std::string cmd, bool toggle )
{
	for (Bind::SetType::iterator i = bind->commands.begin(); i != bind->commands.end(); ++i) {
		if (i->toggle == toggle && i->command == cmd) {
			bind->commands.erase(i);
			break;
		}
	}

	if (bind->commands.empty()) removeBind(bind);
}

void BindManager::removeBind( SDLKey key )
{
	std::map<SDLKey, KeyBind*>::iterator p = bindMap.find(key);
	if (p != bindMap.end()) {
		binds.erase(binds.find(p->second));
		delete p->second;
		bindMap.erase(p);
	}
}

void BindManager::removeBindMouse( Uint8 button )
{
	std::map<Uint8, MouseBind*>::iterator p = mouseBindMap.find(button);
	if (p != mouseBindMap.end()) {
		binds.erase(binds.find(p->second));
		delete p->second;
		mouseBindMap.erase(p);
	}
}

void BindManager::removeBind( const std::string& key_name )
{
	int mouse = isMouse(key_name);
	if (mouse != -1) {
		removeBindMouse(mouse);
	} else {
		removeBind(KeyNames::get().getKeyFromName(key_name));
	}
}

