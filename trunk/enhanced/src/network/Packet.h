/* 
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
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
#ifndef NETWORK_PACKET_H
#define NETWORK_PACKET_H
#include "opentyr.h"

#include "PacketFactory.h"
#include "UDPPacket.h"

//#include "SDL.h"
//#include "SDL_net.h"

/**
 * Base Packet class which all other packet classes should inherit from.
 *
 * Layout:
 *
 *   - 0: Uint8 Packet Type (Class serialization ID)
 *
 * Size: 1
*/
class Packet
{
public:
	struct PacketInitException : std::runtime_error
	{
		PacketInitException(const std::string& msg) : runtime_error(msg)
		{}
	};

	Packet();
	virtual ~Packet();

	virtual Packet* clone() const = 0;

	virtual void handle() = 0;

	virtual void serialize(Uint8 *data) const;
	static Packet* createPacket(const UDPPacket& data);
	virtual void deserialize(Uint8 *data);
	virtual int getPacketSize() const;

	virtual PacketFactory::PacketTypes getTypeId() const = 0;
};

#endif // NETWORK_PACKET_H
