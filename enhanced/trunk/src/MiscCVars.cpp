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
#include "CVar.h"
#include "CCmd.h"
#include "Console.h"

namespace CVars
{
	namespace Func
	{
		void set( const std::vector<std::string>& params )
		{
			std::string param1 = CCmd::convertParam<std::string>(params, 0);
			CVar* const var = CVarManager::get().getCVar(param1);
			if (!var) throw CCmd::RuntimeCCmdErrorException("Unknow CVar/CCmd");

			if (params.size() == 1)
			{
				Console::get() << var->serialize() << std::endl;
			} else {
				std::string param2 = CCmd::convertParam<std::string>(params, 1);
				try
				{
					var->unserialize(param2);
				} catch (CVar::ConversionErrorException&) {
					throw CCmd::RuntimeCCmdErrorException("Incorrect format");
				}
			}
		}

		void echo( const std::vector<std::string>& params )
		{
			std::string str = CCmd::convertParam<std::string>(params, 0);

			Console::get() << str << std::endl;
		}

		void help( const std::vector<std::string>& params )
		{
			std::string param1 = CCmd::convertParam<std::string>(params, 0);

			const CCmd* cmd = CCmdManager::get().getCCmd(param1);
			if (cmd)
			{
				Console::get() << cmd->getHelp() << std::endl;
			} else {
				const CVar* var = CVarManager::get().getCVar(param1);
				if (var)
				{
					Console::get() << var->getHelp() << std::endl;
				} else {
					throw CCmd::RuntimeCCmdErrorException("Unknow CVar/CCmd");
				}
			}
		}
	}
 
	CCmd set("set", CCmd::NONE, "Sets the value of a CVar. Usage: set [cvar] [value]", Func::set);
	CCmd echo("echo", CCmd::NONE, "Prints a message to console. Usage: echo [message]", Func::echo);
	CCmd help("help", CCmd::NONE, "Prints help text for a CCmd or CVar. Usage: help [cvar|ccmd]", Func::help);

	CVarInt testint("testint", CVar::NONE, "This is a test ConVar, hello", 0);
	CVarFloat testfloat("testfloat", CVar::NONE, "This is a test ConVar, hello", 3.14);
	CVarBool testbool("testbool", CVar::NONE, "This is a test ConVar, hello", true);
	CVarString teststr("teststr", CVar::NONE, "This is a test ConVar, hello", "");
}
