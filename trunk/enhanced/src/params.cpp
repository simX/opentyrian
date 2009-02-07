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
#include "params.h"

#include "console/cvar/CVar.h"
#include "error.h"
#include "loudness.h"
#include "mainint.h"
#include "nortsong.h"
#include "Filesystem.h"

#include <ctime>
#include <string>

static bool defaultEnableChristmas( )
{
	std::time_t now = std::time(NULL);
	return std::localtime(&now)->tm_mon == 11;
}

static bool christmasCallback( const bool& val )
{
	if (!Filesystem::get().fileExists("tyrianc.shp") || !Filesystem::get().fileExists("voicesc.snd"))
	{
		Console::get() << "\a7Error:\ax Christmas datafiles not found." << std::endl;
		return false;
	}

	if (CVars::snd_enabled && loadedSoundData)
	{
		JE_loadSndFile();
	}

	if (loadedMainShapeTables)
	{
		JE_loadMainShapeTables();
	}

	return val;
}

namespace CVars
{
	CVarBool ch_loot("ch_loot", CVar::CHEAT, "Gives you mucho bucks.", false);
	CVarBool ch_constant_play("ch_constant_play", CVar::CHEAT, "Constant play for testing purposes (C key activates invincibility)", false);
	CVarBool ch_constant_death("ch_constant_death", CVar::CHEAT, "Constant death mode. Only useful for testing.", false);
	CVarBool ch_xmas("ch_xmas", CVar::CHEAT, "Enables christmas mode.", defaultEnableChristmas(), christmasCallback);

	CVarBool record_demo("record_demo", CVar::NONE, "Enables demo recording. WARNING: For now, never set this while the game is running.", false);
}

void scan_parameters( int argc, char *argv[] )
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '+') {
			Console::get().runCommand(std::string(argv[i]+1));
		} else {
			Console::get() << "\a7Warning:\ax Legacy Tyrian commandline options aren't supported anymore. See README for details." << std::endl;
		}
	}
}
