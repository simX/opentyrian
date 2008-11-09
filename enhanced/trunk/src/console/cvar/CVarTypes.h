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
#ifndef CONSOLE_CVAR_CVARTYPES_H
#define CONSOLE_CVAR_CVARTYPES_H
#include "opentyr.h"

#include "CVarTemplate.h"

#include <sstream>

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

/*
void CVarString::serialize(  ) const
{
	s.put('"');

	for (std::string::const_iterator i = mValue.begin(); i != mValue.end(); i++)
	{
		switch (*i)
		{
		case '"':
			s << "\\\"";
			break;
		case '\\':
			s << "\\\\";
			break;
		case '\n':
			s << "\\n";
			break;
		default:
			s << *i;
			break;
		}
	}

	s.put('"');
}

void CVarString::unserialize( std::istream& s )
{
	s >> std::ws;

	if (s.get() != '"') throw ParseErrorException("\" expected");

	while (int c = s.get() != '"')
	{
		if (c == '\\')
		{
			switch (s.get())
			{
			case '\\':
				mValue.append(1, '\\');
				break;
			case '"':
				mValue.append(1, '"');
				break;
			case 'n':
				mValue.append(1, '\n');
				break;
			default:
				throw ParseErrorException("Invalid character after \\");
				break;
			}
		} else {
			mValue.append(1, c);
		}
	}
}
*/

#endif // CONSOLE_CVAR_CVARTYPES_H
