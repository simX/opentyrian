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

/**
 * @file PacketNetwork.h Connection maintenance packets.
 */
#ifndef NETWORK_PACKETS_PACKETNETWORK_H
#define NETWORK_PACKETS_PACKETNETWORK_H
#include "opentyr.h"

#include "PacketReliable.h"
#include "../Packet.h"

namespace network
{

/**
 * Packet used to acknowledge the receivement of a reliable packet.
 *
 * Layout:
 *
 *   - Packet data
 *   - 0: Uint16 Unique Packet ID to Ack (Network ID)
 *
 * Size: 2
 */
class PacketAcknowledge : public Packet
{
public:
	PacketAcknowledge();
	PacketAcknowledge(Uint16 packetId)
		: packetId(packetId)
	{}

	PacketAcknowledge *clone() const;
	void handle(NetManager& manager);
	PacketFactory::PacketTypes getTypeId() const;

	void serialize(Uint8 *data) const;
	void deserialize(Uint8 *data);
	int getPacketSize() const;

	Uint16 packetId;
};

/**
 * Null packet, used mostly to keep the connection alive and NAT-punch.
 *
 * Layout:
 *
 *   - Packet data
 *
 * Size: 0
 */
class PacketNull : public Packet
{
public:
	PacketNull();

	PacketNull *clone() const;
	void handle(NetManager& manager);
	PacketFactory::PacketTypes getTypeId() const;

	void serialize(Uint8 *data) const;
	void deserialize(Uint8 *data);
	int getPacketSize() const;
};

/**
 * Indicates to the other peer that the game session should be terminated.
 *
 * Layout:
 *
 *   - Packet data
 *   - 0: Uint16 Message length
 *   - 2: ... Message
 *
 * Size: 2 + message length
 *
 * @note This isn't a PacketReliable because I don't really care if it doesn't gets there, it'll time out anyway.
 */
class PacketTerminate : public Packet
{
public:
	PacketTerminate();

	PacketTerminate *clone() const;
	void handle(NetManager& manager);
	PacketFactory::PacketTypes getTypeId() const;

	void serialize(Uint8 *data) const;
	void deserialize(Uint8 *data);
	int getPacketSize() const;

	std::string message;
};

}

#endif // NETWORK_PACKETS_PACKETNETWORK_H
