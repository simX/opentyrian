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
#include "boost/bind.hpp"
#include "boost/function.hpp"

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


template<class T> T rangeCheck( const T& val, T low, T hi )
{
	if (val < low) return low;
	if (val > hi) return hi;
	return val;
}

template<class T> boost::function<T(const T&)> rangeBind(T low, T hi)
{
	return boost::bind<T, const T&, T, T>(rangeCheck, _1, low, hi);
}

template<class T, class NAME> class CVarTemplate : public CVar
{
protected:
	T mValue;
	T archiveValue;
	const T defaultValue;

	virtual std::string onSerialize( const T& val ) const
	{
		std::ostringstream s;
		s << val;
		return s.str();
	}

	virtual T onUnserialize( const std::string& str ) {
		T tmp;
		std::istringstream s(str);
		if (!(s >> tmp)) throw CVar::ConversionErrorException();
		s >> tmp;
		return tmp;
	}
private:
	boost::function<T(const T&)> mValidationFunc;
public:
	CVarTemplate( const std::string& name, int flags, const std::string& help, T def, boost::function<T(const T&)> validationFunc = 0 )
		: CVar(name, flags, help), mValue(def), archiveValue(def), defaultValue(def), mValidationFunc(validationFunc)
	{}

	virtual ~CVarTemplate() {}

	T get( ) const { return mValue; }

 	void set( const T& val, bool bypassCallback = false ) {
		if (mValidationFunc && !bypassCallback)
		{
			mValue = val;
			mValue = mValidationFunc(val);
		} else {
			mValue = val;
		}

		if (getFlags() & CONFIG_AUTO)
			archiveValue = mValue;
	}

	T getArchive( ) const
	{
		if (!getFlags() & CONFIG)
			throw NoArchiveException(*this);

		return archiveValue;
	}

	void setArchive( const T& val )
	{
		if (!getFlags() & CONFIG)
			throw NoArchiveException(*this);

		set(val);
		archiveValue = mValue;
	}

	std::string serialize( ) const
	{
		return onSerialize(get());
	}
	
	void unserialize( const std::string& str )
	{
		set(onUnserialize(str));
	}

	std::string serializeArchive( ) const
	{
		if (!getFlags() & CONFIG)
			throw NoArchiveException(*this);

		return onSerialize(getArchive());
	}

	void unserializeArchive( const std::string& str )
	{
		if (!getFlags() & CONFIG)
			throw NoArchiveException(*this);

		setArchive(onUnserialize(str));
	}

	// Returns true if the archived value is different than the default value
	bool archiveDirty( ) const
	{
		return archiveValue != defaultValue;
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
protected:
	std::string onSerialize( const bool& val ) const
	{
		return val ? "true" : "false";
	}

	bool onUnserialize( const std::string& str )
	{
		if (str == "true") {
			return true;
		} else if (str == "false") {
			return false;
		} else {
			std::istringstream s(str);
			int tmp;
			s >> tmp;
			return s > 0 ? true : false;
		}
	}

public:
	CVarBool( const std::string& name, int flags, const std::string& help, bool def, boost::function<bool(const bool&)> validationFunc = 0 )
		: CVarTemplate<bool, BoolString>(name, flags, help, def, validationFunc)
	{}

	void operator=( const bool& val ) { set(val); }
};

struct StringString { inline static const char* get() { return "string"; } };
class CVarString : public CVarTemplate<std::string, StringString>
{
protected:
	std::string onSerialize( const std::string& val ) const { return val; }
	std::string onUnserialize( const std::string& str ) { return str; }
public:
	CVarString( const std::string& name, int flags, const std::string& help, const std::string& def )
		: CVarTemplate<std::string, StringString>(name, flags, help, def)
	{}
};

class ParseErrorException : public std::runtime_error {
public:
	ParseErrorException( const std::string& text ) : runtime_error(text) {};
};

#endif // CVAR_H
