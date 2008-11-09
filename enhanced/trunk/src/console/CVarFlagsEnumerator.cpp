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
#include "CVarFlagsEnumerator.h"

#include "CVar.h"

#include <string>
#include <utility>
#include "boost/algorithm/string/join.hpp"

typedef std::pair<CVar::Flags, std::string> FlagPair;

// This must be kept in sync with the flags in CVars or else it won't show up in descriptions
// But maybe it can be useful to hide flags from the user
static const FlagPair flags[] =
{
	FlagPair(CVar::CONFIG, "CONFIG"),
	FlagPair(CVar::CONFIG_AUTO, "CONFIG_AUTO"),
	FlagPair(CVar::CHEAT, "CHEAT"),
	FlagPair(CVar::NONE, "") // Sentinel
};

std::string enumerate_cvar_flags( const CVar& cvar )
{
	std::list<std::string> stringList;

	for (unsigned int i = 0; flags[i].first != CVar::NONE; ++i)
	{
		if (cvar.getFlags() & flags[i].first)
			stringList.push_back(flags[i].second);
	}

	return boost::algorithm::join(stringList, ", ");
}
