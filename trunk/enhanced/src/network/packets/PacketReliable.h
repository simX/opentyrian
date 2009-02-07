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
#ifndef NETWORK_PACKETS_PACKETRELIABLE_H
#define NETWORK_PACKETS_PACKETRELIABLE_H
#include "opentyr.h"

#include "../Packet.h"

/**
 * Reliable packet that is always guaranteed to arrive at the other peer.
 *
 * These kinds of packets are always guaranteed to arrive. The game will wait
 * for a PacketAcknowledge and resend if it doesn't receive it.
 *
 * Layout:
 *
 *   - Packet data
 *   - 0: Uint16 Packet ID (Network ID)
 *
 * Size: 2
 */
class PacketReliable : public Packet
{
public:
	PacketReliable();

	PacketReliable *clone() const = 0;
	void handle();
	PacketFactory::PacketTypes getTypeId() const = 0;

	void serialize(Uint8 *data) const;
	void deserialize(Uint8 *data);
	int getPacketSize() const;

	Uint16 packetId;
};

#endif // NETWORK_PACKETS_PACKETRELIABLE_H
