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
#ifndef NETWORK_UDPPACKET_H
#define NETWORK_UDPPACKET_H
#include "opentyr.h"

#include "SDL_net.h"

namespace network
{

/**
 * Resource wrapper around a SDL_net UDPpacket.
 */
class UDPPacket
{
private:
	UDPPacket& operator=(const UDPPacket&);

public:
	UDPPacket(int size = 1024) : packet(SDLNet_AllocPacket(size))
	{
		if (packet == 0)
			throw std::runtime_error(SDLNet_GetError());

		packet->len = size;
		packet->channel = 0;
	}

	UDPPacket(const UDPPacket& other) : packet(SDLNet_AllocPacket(other.packet->maxlen))
	{
		if (packet == 0)
			throw std::runtime_error(SDLNet_GetError());

		Uint8 *const tempData = packet->data;
		*packet = *other.packet;
		packet->data = tempData;
	}

	~UDPPacket()
	{
		SDLNet_FreePacket(packet);
	}

	operator UDPpacket*()
	{
		return packet;
	}

	UDPpacket *operator->() const
	{
		return packet;
	}

	UDPpacket *const packet;
};

}

#endif // NETWORK_UDPPACKET_H
