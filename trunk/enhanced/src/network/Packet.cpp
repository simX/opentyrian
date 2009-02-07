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
#include "Packet.h"

#include "PacketFactory.h"

//#include "SDL_net.h"

Packet::Packet()
{
}

Packet::~Packet()
{
}

void Packet::serialize(Uint8 *data) const
{
	data[0] = getTypeId();
}

Packet *Packet::createPacket(const UDPPacket& data)
{
	Packet *packet = globalPacketFactory.createFromTypeId(PacketFactory::PacketTypes(data.packet->data[0]));

	// Check that the data received it big enough to deserialize
	if (data->len <= packet->getPacketSize())
	{
		delete packet;
		return 0;
	}
	else
	{
		packet->deserialize(data->data);
		return packet;
	}
}

void Packet::deserialize(Uint8 *data)
{
}

int Packet::getPacketSize() const
{
	return 1;
}
