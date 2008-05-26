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
#include "BinaryStream.h"

#include "SDL_endian.h"
#include <memory>
#include <string>

void OBinaryStream::put8( Uint8 data )
{
	mStream.put(data);
}

void OBinaryStream::put16( Uint16 data )
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	mStream.put(data >> 8);
	mStream.put(data & 0xFF);
#else
	mStream.put(data & 0xFF);
	mStream.put(data >> 8);
#endif
}

void OBinaryStream::put32( Uint32 data )
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	mStream.put(data >> 24);
	mStream.put(data >> 16 & 0xFF);
	mStream.put(data >> 8 & 0xFF);
	mStream.put(data & 0xFF);
#else
	mStream.put(data & 0xFF);
	mStream.put(data >> 8 & 0xFF);
	mStream.put(data >> 16 & 0xFF);
	mStream.put(data >> 24);
#endif
}

void OBinaryStream::put( const std::string& data )
{
	put32(data.length());

	for (unsigned int i = 0; i < data.length(); i++)
	{
		mStream.put(data[i]);
	}
}



Uint8 IBinaryStream::get8( )
{
	return mStream.get();
}

Uint16 IBinaryStream::get16( )
{
	const Uint8 b1 = mStream.get();
	const Uint8 b2 = mStream.get();

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	return (b1<<8 | b2);
#else
	return (b2<<8 | b1);
#endif
}

Uint32 IBinaryStream::get32( )
{
	const Uint8 b1 = mStream.get();
	const Uint8 b2 = mStream.get();
	const Uint8 b3 = mStream.get();
	const Uint8 b4 = mStream.get();

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	return (b1<<24 | b2<<16 | b3<<8 | b4);
#else
	return (b4<<24 | b3<<16 | b2<<8 | b1);
#endif
}

std::string IBinaryStream::getStr( )
{
	const unsigned int size = get32();

	std::string str(size, '\0');

	for (unsigned int i = 0; i < size; i++)
	{
		str[i] = mStream.get();
	}

	return str;
}

std::vector<Uint8> IBinaryStream::getArray( unsigned int len )
{
	std::vector<Uint8> vec;
	vec.reserve(len);

	for (; len > 0; --len) {
		vec.push_back(get8());
	}

	return vec;
}

unsigned long IBinaryStream::getSize( )
{
	// Save current position
	std::streampos old_pos = mStream.tellg();

	// Seek to end of file to get size
	mStream.seekg(0, std::ios_base::end);
	std::streampos size = mStream.tellg();

	// Restored original position
	mStream.seekg(old_pos);

	return size;
}
