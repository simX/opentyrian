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
#include "PacketConnect.h"


PacketConnect::PacketConnect()
{
}

PacketConnect *PacketConnect::clone() const
{
	return new PacketConnect(*this);
}

void PacketConnect::handle()
{
	// TODO
}

PacketFactory::PacketTypes PacketConnect::getTypeId() const
{
	return PacketFactory::PACKET_CONNECT;
}

void PacketConnect::serialize(Uint8 *data) const
{
	PacketReliable::serialize(data);
	data += PacketReliable::getPacketSize();

	SDLNet_Write16(0xFFFF, data+0);
	SDLNet_Write16(version, data+2);
	data[4] = delay;
	data[5] = episodes;
	data[6] = player;

	data[7] = (playerName.length() > 255 ? 255 : playerName.length());
	playerName.copy(reinterpret_cast<char*>(data+8), 255);
}

void PacketConnect::deserialize(Uint8 *data)
{
	PacketReliable::deserialize(data);
	data += PacketReliable::getPacketSize();

	magic = SDLNet_Read16(data+0);
	version = SDLNet_Read16(data+2);
	delay = data[4];
	episodes = data[5];
	player = data[6];
	playerName.assign(data[8], data[7]);
}

int PacketConnect::getPacketSize() const
{
	return PacketReliable::getPacketSize() + 8 + playerName.length();
}
