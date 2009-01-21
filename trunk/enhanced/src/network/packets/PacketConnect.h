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
#ifndef NETWORK_PACKETS_PACKETCONNECT_H
#define NETWORK_PACKETS_PACKETCONNECT_H
#include "opentyr.h"

#include "PacketReliable.h"

/**
 * Holds information about a player sent during the handshake.
 *
 * Layout:
 *
 *   - PacketReliable data
 *   - 0: Uint16 = 0xFFFF Enhanced magic number
 *   - 2: Uint16 Version number
 *   - 4: Uint8 Network delay
 *   - 5: Uint8 Available episodes bitset
 *   - 6: Uint8 Local player number
 *   - 7: Uint8 Player name length
 *   - 8: ... Player name
 *
 * Size: 8 + player name length
 */
class PacketConnect : public PacketReliable
{
public:
	PacketConnect();

	PacketConnect *clone() const;
	void handle();
	PacketFactory::PacketTypes getTypeId() const;

	void serialize(Uint8 *data) const;
	void deserialize(Uint8 *data);
	int getPacketSize() const;

	/** @note Ignored on send, always 0xFFFF. */
	Uint16 magic;
	Uint16 version;
	Uint8 delay;
	Uint8 episodes;
	Uint8 player;
	std::string playerName;
};

#endif // NETWORK_PACKETS_PACKETCONNECT_H
