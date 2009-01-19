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
#include "opentyr.h"
#include "PacketNetwork.h"

///////////////////////
// PacketAcknowledge //
///////////////////////

PacketAcknowledge::PacketAcknowledge()
{
}

PacketAcknowledge *PacketAcknowledge::clone() const
{
	return new PacketAcknowledge(*this);
}

void PacketAcknowledge::handle()
{
	// TODO
}

PacketFactory::PacketTypes PacketAcknowledge::getTypeId() const
{
	return PacketFactory::PACKET_ACKNOWLEDGE;
}

void PacketAcknowledge::serialize(Uint8 *data) const
{
	PacketReliable::serialize(data);
	data += PacketReliable::getPacketSize();
}

void PacketAcknowledge::deserialize(Uint8 *data)
{
	PacketReliable::deserialize(data);
	data += PacketReliable::getPacketSize();
}

int PacketAcknowledge::getPacketSize() const
{
	return PacketReliable::getPacketSize() + 0;
}



/////////////////
// PacketNull //
/////////////////

PacketNull::PacketNull()
{
}

PacketNull *PacketNull::clone() const
{
	return new PacketNull(*this);
}

void PacketNull::handle()
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
