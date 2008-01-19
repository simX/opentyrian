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
#ifndef CVAR_H
#define CVAR_H

#include "Singleton.h"

#include <string>
#include <map>
#include <list>
#include <deque>
#include <iostream>
#include <sstream>
#include <stdexcept>

class CVar
{
public:
	class ConversionErrorException : public std::runtime_error
	{
	public:
		ConversionErrorException() : std::runtime_error("CVar type conversion failed") {}
	};

	enum Flags {
		NONE = 0,
		CONFIG = 1
	};

	CVar( std::string name, Flags flags, std::string help );

	std::string getName() const { return mName; }
	Flags getFlags() const { return mFlags; }
	std::string getHelp() const { return mHelp; }
	virtual std::string serialize( ) const = 0;
	virtual void unserialize( std::string str ) = 0;
private:
	CVar( const CVar& );
	CVar& operator=( const CVar& );

	std::string mName;
	Flags mFlags;
	std::string mHelp;
};

class CVarManager : public Singleton<CVarManager>
{
public:
	typedef std::map<std::string, CVar*> MapType;
	typedef std::pair<std::string, CVar*> PairType;

private:
	MapType mCVars;

public:
	void registerCVar( CVar* cvar );
	CVar* getCVar( std::string name );
	const MapType& getCVars( ) { return mCVars; }
	const std::list<CVar*> CVarManager::getCVars( CVar::Flags flags, bool all );
};


template<class T> class CVarTemplate : public CVar
{
protected:
	T mValue;
public:
	CVarTemplate( std::string name, Flags flags, std::string help, T def ) : CVar(name, flags, help), mValue(def) {}
	T get( ) const { return mValue; }
	void set( T val ) { mValue = val; }
	virtual std::string serialize( ) const {
		std::ostringstream s;
		if (!(s << get())) throw CVar::ConversionErrorException();
		return s.str();
	}
	virtual void unserialize( std::string str ) { std::istringstream s(str); s >> mValue; }
};

typedef CVarTemplate<long> CVarInt;
typedef CVarTemplate<double> CVarFloat;
typedef CVarTemplate<bool> CVarBool;

class ParseErrorException : public std::runtime_error {
public:
	ParseErrorException( const std::string& text ) : runtime_error(text) {};
};

class CVarString : public CVarTemplate<std::string>
{
public:
	virtual std::string serialize( ) const { return get(); }
	virtual void unserialize( std::string str ) { set(str); }
};

#endif // CVAR_H
