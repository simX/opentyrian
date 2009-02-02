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
#ifndef CONSOLE_CVAR_CVARTEMPLATE_H
#define CONSOLE_CVAR_CVARTEMPLATE_H
#include "opentyr.h"

#include "CVar.h"

#include "boost/bind.hpp"
#include "boost/function.hpp"
#include <sstream>

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

	~CVarTemplate() {}

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

#endif // CONSOLE_CVAR_CVARTEMPLATE_H
