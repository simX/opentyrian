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
#include "NetManager.h"

#include "NetCVars.h"
#include "packets/PacketNetwork.h"
#include "packets/PacketConnect.h"
#include "episodes.h"
#include "keyboard.h"
#include "joystick.h"

#include <bitset>
#include "boost/static_assert.hpp"

NetManager::NetManager()
	: opponentHost(CVars::net_host), opponentPort(CVars::net_port), localPort(CVars::net_local_port),
	localPlayerNum(CVars::net_player), networkDelay(CVars::net_delay), connected(false), connecting(false)
{
	Console::get() << "Initializing network..." << std::endl;

	{
		network_version = 0; // This will be the default if SVN_REV is invalid.
		std::istringstream s(SVN_REV);
		s >> network_version;
	}

	if (SDLNet_Init() == -1)
	{
		Console::get() << "\a7Error:\ax Failed to initialized SDL_net: " << SDLNet_GetError() <<  std::endl;
		throw NetInitException(std::string("Failed to initialized SDL_net: ") + SDLNet_GetError());
	}
}

bool NetManager::sendPacket(Packet& packet)
{
	UDPPacket udpPacket(packet.getPacketSize());
	packet.serialize(udpPacket->data);
	
	if (!SDLNet_UDP_Send(socket, 0, udpPacket))
	{
		Console::get() << "\a7Warning:\ax Failed to send packet: " << SDLNet_GetError() << std::endl;
		return false;
	}

	return true;
}

bool NetManager::checkNullPacket()
{
	UDPPacket recvPacket;

	switch(SDLNet_UDP_Recv(socket, recvPacket))
	{
	case -1:
		Console::get() << "\a7Warning:\ax Failed to receive packet: " << SDLNet_GetError() << std::endl;
		return false;
	case 0:
		return false;
	default:
		Packet *packet = Packet::createPacket(recvPacket);
		return false;
	}
}

void NetManager::connect()
{
	DEBUG_MSG("Opening local socket on port: " << localPort);
	socket = SDLNet_UDP_Open(localPort);
	if (socket == 0)
	{
		Console::get() << "\a7Error:\ax Failed to open UDP socket: " << SDLNet_GetError() << std::endl;
		throw ConnectErrorException(std::string("Failed to open UDP socket: ") + SDLNet_GetError());
	}

	{
		Console::get() << "Resolving " << opponentHost << "...";

		IPaddress addr;
		if (SDLNet_ResolveHost(&addr, opponentHost.c_str(), opponentPort) == -1)
		{
			Console::get() << std::endl << "\a7Error:\ax Failed to resolve " << opponentHost << std::endl;
			throw ConnectErrorException(std::string("Failed to resolve ") + opponentHost);
		}

		Console::get() << (addr.host & 0x000F) << '.' <<
			(addr.host & 0x00F0 >> 8) << '.' <<
			(addr.host & 0x0F00 >> 16) << '.' <<
			(addr.host & 0xF000 >> 24) << std::endl;

		if (SDLNet_UDP_Bind(socket, 0, &addr) == -1)
		{
			Console::get() << "\a7Error:\ax Failed to bind socket: " << SDLNet_GetError() << std::endl;
			throw ConnectErrorException(std::string("Failed to bind socket: ") + SDLNet_GetError());
		}
	}

	// NAT Punch
	while (!checkNullPacket())
	{
		PacketNull packet;
		sendPacket(packet);

		service_SDL_events(true);
		JE_joystickTranslate();

		if (newkey && lastkey_sym == SDLK_ESCAPE)
			;// TODO quit

		SDL_Delay(10);
	}

	{
		// Compact available episodes into a byte
		BOOST_STATIC_ASSERT(EPISODE_MAX <= 8);
		std::bitset<8> localAvailEpisodes;

		for (unsigned int i = 0; i < localAvailEpisodes.size(); ++i)
		{
			localAvailEpisodes[i] = (episodeAvail[i] != 0);
		}

		PacketConnect packet;
		packet.version = network_version;
		packet.delay = networkDelay;
		packet.episodes = localAvailEpisodes.to_ulong();
		packet.player = localPlayerNum;
		packet.playerName = localPlayerName;
		sendPacket(packet);

		// TODO Stuff
		// TODO Ignore version if it's 0
	}

	connected = true;
}

void NetManager::updateNetwork()
{
	if (connected)
	{
		// Send a PacketNull if we excedded the keep alive interval.
		if (SDL_GetTicks() - lastKeepAliveTick > CVars::net_keepalive_interval)
		{
			PacketNull packet;
			sendPacket(packet);
			lastKeepAliveTick = SDL_GetTicks();
		}
	}
}