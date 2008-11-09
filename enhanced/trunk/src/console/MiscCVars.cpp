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

#include "cvar/CVar.h"
#include "cvar/CVarManager.h"
#include "cvar/CVarFlagsEnumerator.h"
#include "CCmd.h"
#include "Console.h"

namespace CCmds
{
	namespace Func
	{
		static void set( const std::vector<std::string>& params )
		{
			std::string param1 = CCmd::convertParam<std::string>(params, 0);
			CVar* const var = CVarManager::get().getCVar(param1);
			if (!var) throw CCmd::RuntimeCCmdError("Unknow CVar/CCmd");

			if (params.size() == 1)
			{
				Console::get() << var->serialize() << std::endl;
			} else {
				std::string param2 = CCmd::convertParam<std::string>(params, 1);
				try
				{
					var->unserialize(param2);
				} catch (CVar::ConversionErrorException&) {
					throw CCmd::RuntimeCCmdError("Incorrect format");
				}
			}
		}

		static void seta( const std::vector<std::string>& params )
		{
			std::string param1 = CCmd::convertParam<std::string>(params, 0);
			CVar* const var = CVarManager::get().getCVar(param1);
			if (!var) throw CCmd::RuntimeCCmdError("Unknow CVar/CCmd");

			try
			{
				if (params.size() == 1)
				{
					Console::get() << var->serializeArchive() << std::endl;
				} else {
					std::string param2 = CCmd::convertParam<std::string>(params, 1);
					try
					{
						var->unserializeArchive(param2);
					}
					catch (CVar::ConversionErrorException&)
					{
						throw CCmd::RuntimeCCmdError("Incorrect format");
					}
				}
			}
			catch (CVar::NoArchiveException&)
			{
				throw CCmd::RuntimeCCmdError(var->getName() + " isn't a configuration CVar.");
			}
		}

		static void echo( const std::vector<std::string>& params )
		{
			std::string str = CCmd::convertParam<std::string>(params, 0);

			Console::get() << str << std::endl;
		}

		static void help( const std::vector<std::string>& params )
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
					throw CCmd::RuntimeCCmdError("Unknow CVar/CCmd");
				}
			}
		}

		static void list( const std::vector<std::string>& params )
		{
			std::string param1 = CCmd::convertParam<std::string>(params, 0);

			if (param1 == "cvar") // Lists all CVars
			{
				const CVarManager::MapType& map = CVarManager::get().getCVars();
				for (CVarManager::MapType::const_iterator i = map.begin(); i != map.end(); ++i)
				{
					CVar& cvar = *i->second;
					
					Console::get() << "\a2" << cvar.getName() << "\ax [" << cvar.getType();

					std::string flags = enumerate_cvar_flags(cvar);
					if (!flags.empty())
						Console::get() << ": " << enumerate_cvar_flags(cvar);

					Console::get() << "] " << cvar.getHelp() << std::endl;
				}
			} else if (param1 == "ccmd") { // Lists all CCmds
				const CCmdManager::MapType& map = CCmdManager::get().getCCmds();
				for (CCmdManager::MapType::const_iterator i = map.begin(); i != map.end(); ++i)
				{
					CCmd& ccmd = *i->second;
					Console::get() << "\a2" << ccmd.getName() << "\ax: " << ccmd.getHelp() << std::endl;
				}
			} else { // Searches for given text
				using std::string;

				// Search CVars first
				const CVarManager::MapType& varmap = CVarManager::get().getCVars();
				for (CVarManager::MapType::const_iterator i = varmap.begin(); i != varmap.end(); ++i)
				{
					CVar& cvar = *i->second;
					if (cvar.getName().find(param1) != string::npos || cvar.getHelp().find(param1) != string::npos)
					{
						Console::get() << "\a2" << cvar.getName() << "\ax [" << cvar.getType() << "] " << cvar.getHelp() << std::endl;
					}
				}

				// Search CCmds aftewards
				const CCmdManager::MapType& cmdmap = CCmdManager::get().getCCmds();
				for (CCmdManager::MapType::const_iterator i = cmdmap.begin(); i != cmdmap.end(); ++i)
				{
					CCmd& ccmd = *i->second;
					if (ccmd.getName().find(param1) != string::npos || ccmd.getHelp().find(param1) != string::npos)
					{
						Console::get() << "\a2" << ccmd.getName() << "\ax: " << ccmd.getHelp() << std::endl;
					}
				}
			}
		}

		static void comment( const std::vector<std::string>& params )
		{
			// No-op
		}
	}
 
	CCmd set("set", CCmd::NONE, "Sets the value of a CVar. Usage: set [cvar] [value]", Func::set);
	CCmd seta("seta", CCmd::NONE, "Sets the value of a CVar and archives it. Only valid for configuration CVars. Usage: seta [cvar] [value]", Func::seta);
	CCmd echo("echo", CCmd::NONE, "Prints a message to console. Usage: echo [message]", Func::echo);
	CCmd help("help", CCmd::NONE, "Prints help text for a CCmd or CVar. Usage: help [cvar|ccmd]", Func::help);
	CCmd list("list", CCmd::NONE, "Searches cvar help text or lists all cvars or ccmds. Usage: list [cvar|ccmd|string to search]", Func::list);
	CCmd comment("#", CCmd::NONE, "Does nothing. Useful for placing comments in scripts.", Func::comment);
}

namespace CVars
{
	CVarInt testint("testint", CVar::NONE, "This is a test ConVar, hello", 0);
	CVarFloat testfloat("testfloat", CVar::NONE, "This is a test ConVar, hello", 3.14f);
	CVarBool testbool("testbool", CVar::NONE, "This is a test ConVar, hello", true);
	CVarString teststr("teststr", CVar::NONE, "This is a test ConVar, hello", "");
}
