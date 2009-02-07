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
#include "opentyr.h"
#include "PacketFactory.h"

#include "Packet.h"

#include "packets/PacketNetwork.h"
#include "packets/PacketConnect.h"
// long list of packet includes

template<class T> Packet *makePacket()
{
	return static_cast<Packet *>(new T());
}

PacketFactory::PacketFactory()
{
	// Register packet types here
	idMap[PacketFactory::PACKET_ACKNOWLEDGE] = makePacket<PacketAcknowledge>;
	idMap[PacketFactory::PACKET_NULL] = makePacket<PacketNull>;

	idMap[PacketFactory::PACKET_CONNECT] = makePacket<PacketConnect>;
}

Packet *PacketFactory::createFromTypeId(PacketTypes type)
{
	IdMapType::const_iterator mapping = idMap.find(type);

	if (mapping == idMap.end())
		throw UnknownPacketException();

	return mapping->second();
}

PacketFactory globalPacketFactory;