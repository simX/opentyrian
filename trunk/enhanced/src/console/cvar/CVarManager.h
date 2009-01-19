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
#ifndef CONSOLE_CVAR_CVARMANAGER_H
#define CONSOLE_CVAR_CVARMANAGER_H
#include "opentyr.h"

#include "Singleton.h"
#include "CVar.h"

#include <string>
#include <map>
#include <list>

class CVarManager : public Singleton<CVarManager>
{
public:
	typedef std::map<std::string, CVar*> MapType;
	typedef std::pair<std::string, CVar*> PairType;

private:
	MapType mCVars;

public:
	void registerCVar( CVar* cvar );
	CVar* getCVar( const std::string& name );
	const MapType& getCVars( ) { return mCVars; }
	const std::list<CVar*> getCVars( CVar::Flags flags, bool all );
};

#endif // CONSOLE_CVAR_CVARMANAGER_H
