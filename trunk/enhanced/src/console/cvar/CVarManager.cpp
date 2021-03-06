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
#include "CVarManager.h"

#include "CVar.h"

void CVarManager::registerCVar( CVar* const cvar )
{
	if (!mCVars.insert(CVarManager::PairType(cvar->getName(), cvar)).second)
	{
		Console::get() << "\a7Warning:\ax Two CVars with name \"" << cvar->getName() << "\" registered!" << std::endl;
	}
}

CVar* CVarManager::getCVar( const std::string& name )
{
	const MapType::iterator i = mCVars.find(name);
	if (i != mCVars.end())
	{
		return i->second;
	} else {
		return 0;
	}
}

const std::list<CVar*> CVarManager::getCVars( CVar::Flags flags, bool all ) // all: True: must have all flags, False: must have any flags
{
	std::list<CVar*> list;

	for (CVarManager::MapType::const_iterator i = mCVars.begin(); i != mCVars.end(); i++)
	{
		bool cond;
		if (all)
		{
			cond = (i->second->getFlags() & flags) == flags;
		} else {
			cond = (i->second->getFlags() & flags) != 0;
		}

		if (cond)
		{
			list.push_back(i->second);
		}
	}

	return list;
}
