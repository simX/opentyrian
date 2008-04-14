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
	virtual ~CVar() {}

	std::string getName() const { return mName; }
	Flags getFlags() const { return mFlags; }
	std::string getHelp() const { return mHelp; }
	virtual std::string serialize( ) const = 0;
	virtual void unserialize( std::string str ) = 0;
	virtual std::string getType() const = 0;
private:
	CVar( const CVar& );
	CVar& operator=( const CVar& );

	const std::string mName;
	const Flags mFlags;
	const std::string mHelp;
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
	const std::list<CVar*> getCVars( CVar::Flags flags, bool all );
};


template<long low, long hi> long rangeCheck( const long& val )
{
	if (val < low) return low;
	if (val > hi) return hi;
	return val;
}

template<class T, class NAME> class CVarTemplate : public CVar
{
protected:
	T mValue;
private:
	T (* const mValidationFunc) (const T& value);
public:
	CVarTemplate( std::string name, Flags flags, std::string help, T def, T (*validationFunc)(const T& value) = 0 ) : CVar(name, flags, help), mValue(def), mValidationFunc(validationFunc) {}
	virtual ~CVarTemplate() {}
	T get( ) const { return mValue; }
 	void set( const T& val ) {
		if (mValidationFunc)
		{
			mValue = (*mValidationFunc)(val);
		} else {
			mValue = val;
		}
	}
	virtual std::string serialize( ) const {
		std::ostringstream s;
		s << get();
		return s.str();
	}
	virtual void unserialize( std::string str ) {
		T tmp;
		std::istringstream s(str);
		if (!(s >> tmp)) throw CVar::ConversionErrorException();
		s >> tmp;
		set(tmp);
	}
	std::string getType() const { return NAME::get(); }

	virtual operator T() const { return get(); }
	virtual void operator=( const T& val ) { set(val); }
};

struct IntString { inline static const char* get() { return "int"; } };
struct FloatString { inline static const char* get() { return "float"; } };
struct BoolString { inline static const char* get() { return "bool"; } };

typedef CVarTemplate<long, IntString> CVarInt;
typedef CVarTemplate<float, FloatString> CVarFloat;

class CVarBool : public CVarTemplate<bool, BoolString>
{
public:
	CVarBool( std::string name, Flags flags, std::string help, bool def, bool (*validationFunc)(const bool& value) = 0 )
		: CVarTemplate<bool, BoolString>(name, flags, help, def, validationFunc)
	{}

	std::string serialize( ) const
	{
		return get() ? "true" : "false";
	}

	void unserialize( std::string str )
	{
		if (str == "true") {
			set(true);
		} else if (str == "false") {
			set(false);
		} else {
			std::istringstream s(str);
			int tmp;
			s >> tmp;
			set(s > 0 ? true : false);
		}
	}

	void operator=( const bool& val ) { set(val); }
};
//typedef CVarTemplate<bool, BoolString> CVarBool;

class ParseErrorException : public std::runtime_error {
public:
	ParseErrorException( const std::string& text ) : runtime_error(text) {};
};

struct StringString { inline static const char* get() { return "string"; } };
class CVarString : public CVarTemplate<std::string, StringString>
{
public:
	CVarString( std::string name, Flags flags, std::string help, std::string def ) : CVarTemplate<std::string, StringString>(name, flags, help, def) {}
	std::string serialize( ) const { return get(); }
	void unserialize( std::string str ) { set(str); }
};

#endif // CVAR_H
