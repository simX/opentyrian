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
#include "PacketNetwork.h"

#include "../NetManager.h"

namespace network
{

////
// PacketAcknowledge
////

PacketAcknowledge::PacketAcknowledge()
{
}

PacketAcknowledge *PacketAcknowledge::clone() const
{
	return new PacketAcknowledge(*this);
}

void PacketAcknowledge::handle(NetManager& manager)
{
	manager.sentReliablePackets.erase(packetId);
}

PacketFactory::PacketTypes PacketAcknowledge::getTypeId() const
{
	return PacketFactory::PACKET_ACKNOWLEDGE;
}

void PacketAcknowledge::serialize(Uint8 *data) const
{
	Packet::serialize(data);
	data += Packet::getPacketSize();

	SDLNet_Write16(packetId, data);
}

void PacketAcknowledge::deserialize(Uint8 *data)
{
	Packet::deserialize(data);
	data += Packet::getPacketSize();

	packetId = SDLNet_Read16(data+1);
}

int PacketAcknowledge::getPacketSize() const
{
	return Packet::getPacketSize() + 2;
}



////
// PacketNull
////

PacketNull::PacketNull()
{
}

PacketNull *PacketNull::clone() const
{
	return new PacketNull(*this);
}

void PacketNull::handle(NetManager& manager)
{
	// Null packet, null handling. =P
}

PacketFactory::PacketTypes PacketNull::getTypeId() const
{
	return PacketFactory::PACKET_NULL;
}

void PacketNull::serialize(Uint8 *data) const
{
	Packet::serialize(data);
	data += Packet::getPacketSize();
}

void PacketNull::deserialize(Uint8 *data)
{
	Packet::deserialize(data);
	data += Packet::getPacketSize();
}

int PacketNull::getPacketSize() const
{
	return Packet::getPacketSize() + 0;
}

////
// PacketTerminate
////

PacketTerminate::PacketTerminate()
{
}

PacketTerminate *PacketTerminate::clone() const
{
	return new PacketTerminate(*this);
}

void PacketTerminate::handle(NetManager& manager)
{
	manager.quit("Disconnected by peer: " + message);
}

PacketFactory::PacketTypes PacketTerminate::getTypeId() const
{
	return PacketFactory::PACKET_TERMINATE;
}

void PacketTerminate::serialize(Uint8 *data) const
{
	Packet::serialize(data);
	data += Packet::getPacketSize();

	Uint16 messageLength = (message.length() > 65535 ? 65535 : message.length());
	SDLNet_Write16(messageLength, data+0);
	message.copy(reinterpret_cast<char*>(data+2), 65535);
}

void PacketTerminate::deserialize(Uint8 *data)
{
	Packet::deserialize(data);
	data += Packet::getPacketSize();

	Uint16 messageLength = SDLNet_Read16(data+0);
	const char *messageText = reinterpret_cast<const char*>(data+2);
	message.assign(messageText, messageLength);
}

int PacketTerminate::getPacketSize() const
{
	return Packet::getPacketSize() + 2 + message.length();
}

}
