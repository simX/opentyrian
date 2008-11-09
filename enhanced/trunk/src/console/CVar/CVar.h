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
#ifndef CONSOLE_CVAR_CVAR_H
#define CONSOLE_CVAR_CVAR_H

#include <string>
#include <stdexcept>

class CVar
{
public:
	class ConversionErrorException : public std::runtime_error
	{
	public:
		ConversionErrorException() : std::runtime_error("CVar type conversion failed") {}
	};

	class NoArchiveException : public std::runtime_error
	{
	public:
		NoArchiveException( const CVar& cvar )
			: runtime_error(cvar.getName() + " doesn't have the CONFIG flag!") {}
	};

	enum Flags {
		NONE = 0,
		CONFIG = 1,
		CONFIG_AUTO = 2,
		CHEAT = 4
	};

	CVar( const std::string& name, int flags, const std::string& help );
	virtual ~CVar() {}

	const std::string& getName() const { return mName; }
	int getFlags() const { return mFlags; }
	const std::string& getHelp() const { return mHelp; }
	virtual std::string serialize( ) const = 0;
	virtual void unserialize( const std::string& str ) = 0;
	virtual std::string serializeArchive( ) const = 0;
	virtual void unserializeArchive( const std::string& str ) = 0;
	virtual bool archiveDirty( ) const = 0;
	virtual std::string getType() const = 0;
private:
	CVar( const CVar& );
	CVar& operator=( const CVar& );

	const std::string mName;
	const int mFlags;
	const std::string mHelp;
};


#if 0
class ParseErrorException : public std::runtime_error {
public:
	ParseErrorException( const std::string& text ) : runtime_error(text) {};
};
#endif

#include "CVarTypes.h" // Included here for convenience

#endif // CONSOLE_CVAR_CVAR_H
