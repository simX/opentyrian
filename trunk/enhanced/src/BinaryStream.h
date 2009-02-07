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
#ifndef BINARYSTREAM_H
#define BINARYSTREAM_H

#include <iostream>
#include <string>
#include <vector>

#include "SDL_endian.h"

class OBinaryStream
{
private:
	std::ostream& mStream;

	OBinaryStream& operator=(const OBinaryStream&);
public:
	OBinaryStream(std::ostream& stream)
		: mStream(stream)
	{}

	inline std::ostream& getOStream( ) const
	{
		return mStream;
	}

	void put8( Uint8 data );
	void put16( Uint16 data );
	void put32( Uint32 data );
	void put( const std::string& data );
	void putStrLong( const std::string& data );
	void put( const std::string& data, std::string::size_type count );

	template<class iter> void put( iter begin, iter end )
	{
		for (; begin != end; ++begin) {
			put8(*begin);
		}
	}
};

class IBinaryStream
{
private:
	std::istream& mStream;

	IBinaryStream& operator=(const IBinaryStream&);
public:
	IBinaryStream(std::istream& stream)
		: mStream(stream)
	{}

	inline std::istream& getIStream( ) const
	{
		return mStream;
	}

	Uint8 get8( );
	Sint8 getS8( );
	Uint16 get16( );
	Sint16 getS16( );
	Uint32 get32( );
	Sint32 getS32( );
	std::string getStr( );
	std::string getStrLong( );
	std::string getStr( std::string::size_type count );

	std::vector<Uint8> getArray( unsigned int len );
	
	template<class iter> void getIter( iter begin, iter end )
	{
		for (; begin != end; ++begin)
		{
			*begin = get8();
		}
	}

	template<class iter> void getIter16( iter begin, iter end )
	{
		for (; begin != end; ++begin)
		{
			*begin = get16();
		}
	}

	unsigned long getSize( );
};

#endif // BINARYSTREAM_H
