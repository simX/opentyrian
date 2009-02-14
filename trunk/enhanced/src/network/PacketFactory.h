/* 
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
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
#ifndef NETWORK_PACKETFACTORY_H
#define NETWORK_PACKETFACTORY_H
#include "opentyr.h"

namespace network {
class Packet;
}

#include <map>
#include "boost/function.hpp"
#include "boost/static_assert.hpp"

namespace network
{

class PacketFactory
{
public:
	// I guess this goes here, could go elsewhere though.
	enum PacketTypes
	{
		PACKET_ACKNOWLEDGE = 0x00,
		PACKET_NULL = 0x01,

		PACKET_CONNECT = 0x10,
		PACKET_GAME_INFO = 0x11, // Old DETAILS
		PACKET_TERMINATE = 0x12,

		PACKET_STATE = 0x20,
		//PACKET_STATE_XOR = 0x21,
		PACKET_STATE_RESEND = 0x22,

		MAX_PACKET
	};
private:
	// Ensures packet ids fit in one byte.
	BOOST_STATIC_ASSERT(MAX_PACKET <= 0x100);
public:
	struct UnknownPacketException : std::runtime_error
	{
		UnknownPacketException() : runtime_error("Unknown packet id.") {}
	};

	PacketFactory();

	Packet *createFromTypeId(PacketTypes id);
private:
	PacketFactory(const PacketFactory&);
	PacketFactory& operator=(const PacketFactory&);

	typedef std::map<PacketTypes, boost::function<Packet*()> > IdMapType;
	IdMapType idMap;
};

extern PacketFactory globalPacketFactory;

}

#endif // NETWORK_PACKETFACTORY_H
