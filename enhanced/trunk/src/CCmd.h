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
#ifndef CCMD_H
#define CCMD_H

#include "Singleton.h"

#include <string>
#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>

class CCmd
{
public:
	class RuntimeCCmdErrorException : public std::runtime_error
	{
	public:
		RuntimeCCmdErrorException( const std::string& error ) : runtime_error(error) {}
	};

	static void assertParam( const std::vector<std::string>& vec, unsigned int index )
	{
		if (vec.size() <= index)
		{
			throw RuntimeCCmdErrorException("Insufficient number of parameters");
		}
	}

	template<class T> static T convertParam( const std::vector<std::string>& vec, unsigned int index )
	{
		assertParam(vec, index);

		std::istringstream s(vec[index]);
		T val;

		if (s >> val)
		{
			return val;
		} else {
			std::ostringstream s;
			s << index+1;

			throw RuntimeCCmdErrorException("Incorrect format on param " + s.str());
		}
	}

	enum Flags {
		NONE = 0
	};

	CCmd( std::string name, Flags flags, std::string help, void (*func)(const std::vector<std::string>&));

	std::string getName() const { return mName; }
	Flags getFlags() const { return mFlags; }
	std::string getHelp() const { return mHelp; }
	void operator()( const std::vector<std::string>& params ) { (*mFunction)(params); }
private:
	CCmd( const CCmd& );
	CCmd& operator=( const CCmd& );

	const std::string mName;
	const Flags mFlags;
	const std::string mHelp;
	void (* const mFunction) (const std::vector<std::string>&);
};

class CCmdManager : public Singleton<CCmdManager>
{
public:
	typedef std::map<std::string, CCmd*> MapType;
	typedef std::pair<std::string, CCmd*> PairType;

private:
	MapType mCCmds;

public:
	void registerCCmd( CCmd* cvar );
	CCmd* getCCmd( std::string name );
	const MapType& getCCmds( ) { return mCCmds; }
	const std::list<CCmd*> getCCmds( CCmd::Flags flags, bool all );
};

template<> inline std::string CCmd::convertParam<std::string>( const std::vector<std::string>& vec, unsigned int index )
{
	assertParam(vec, index);
	return vec[index];
}

#endif // CCMD_H
