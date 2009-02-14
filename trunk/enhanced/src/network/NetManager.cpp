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
#include "NetManager.h"

#include "Network.h"
#include "NetCVars.h"
#include "packets/PacketNetwork.h"
#include "packets/PacketConnect.h"
#include "episodes.h"
#include "keyboard.h"
#include "joystick.h"

#include <bitset>
#include "boost/static_assert.hpp"
#include "boost/format.hpp"

namespace network
{

NetManager::NetManager()
	: opponentHost(CVars::net_host), opponentPort(CVars::net_port), localPort(CVars::net_local_port),
	localPlayerNum(CVars::net_player), localPlayerName(CVars::net_name), networkDelay(CVars::net_delay), connected(false), connecting(false),
	nextReliableSeqId(0), socket(0), peerInfoSet(false)
{
	Console::get() << "Initializing network..." << std::endl;

	network_version = get_svn_rev_int(); // 0 will be the default if SVN_REV is invalid.

	if (SDLNet_Init() == -1)
	{
		NET_DEBUG("\a7Error:\ax Failed to initialized SDL_net: " << SDLNet_GetError());
		throw NetInitException(std::string("Failed to initialized SDL_net: ") + SDLNet_GetError());
	}
}

NetManager::~NetManager()
{
	if (socket != 0)
	{
		SDLNet_UDP_Close(socket);
	}

	{
		typedef std::pair<const Uint16, ReliablePairType> Pair;
		foreach (Pair& i, sentReliablePackets)
		{
			delete i.second.first;
		}
	}

	{
		typedef std::pair<const Uint16, PacketReliable*> Pair;
		foreach (Pair& i, recvReliablePackets)
		{
			delete i.second;
		}
	}

	SDLNet_Quit();
}

bool NetManager::sendPacket(const Packet& packet)
{
	UDPPacket udpPacket(packet.getPacketSize());
	packet.serialize(udpPacket->data);

	NET_DEBUG("Sending packet. id: " << packet.getTypeId() << " size: " << packet.getPacketSize());

	if (!SDLNet_UDP_Send(socket, 0, udpPacket))
	{
		NET_DEBUG("\a7Warning:\ax Failed to send packet: " << SDLNet_GetError());
		return false;
	}

	if (const PacketReliable* reliable = dynamic_cast<const PacketReliable*>(&packet))
	{
		// Insert only if no such packet is already in the queue.
		if (sentReliablePackets.insert(std::make_pair(nextReliableSeqId, ReliablePairType(reliable->clone(), SDL_GetTicks()))).second)
		{
			nextReliableSeqId++;
		}
	}

	lastPacketSentTick = SDL_GetTicks();

	return true;
}

Packet *NetManager::receivePacket()
{
	UDPPacket recvPacket;

	switch(SDLNet_UDP_Recv(socket, recvPacket))
	{
	case -1:
		NET_DEBUG("\a7Warning:\ax Failed to receive packet: " << SDLNet_GetError());
		return 0;
	case 0:
		return 0;
	default:
		try
		{
			Packet *const packet = Packet::createPacket(recvPacket);
			NET_DEBUG("Received packet. id: " << packet->getTypeId() << " size: " << packet->getPacketSize());
			return packet;
		}
		catch (PacketFactory::UnknownPacketException&)
		{
			NET_DEBUG("\a7Warning:\ax Unknown packet received.");
			return 0;
		}
	}
}

bool NetManager::receiveAndProcessPacket()
{
	std::auto_ptr<Packet> packet(receivePacket());

	if (packet.get() == 0)
		return false;
	else
	{
		if (PacketReliable *reliable = dynamic_cast<PacketReliable*>(packet.get()))
		{
			// Ack this to stop being floodzored with re-transmits.
			sendPacket(PacketAcknowledge(reliable->packetId));

			if (reliable->packetId <= lastProcessedReliablePacket)
			{
				// Already processed this one, drop it.
				return false;
			}
			else
			{
				if (reliable->packetId+1 == lastProcessedReliablePacket)
				{
					// Sequencing numbers match, process.
					reliable->handle(*this);
					lastProcessedReliablePacket++;
					return true;
				}
				else
				{
					// Add it to the queue
					PacketReliable *cloned = reliable->clone();
					if (recvReliablePackets.insert(std::make_pair(reliable->packetId, cloned)).second == false)
					{
						delete cloned;
					}

					// See if we have packets to process on the queue
					bool processed = false;

					for (
						std::map<Uint16, PacketReliable*>::iterator i = recvReliablePackets.find(lastProcessedReliablePacket+1);
						i != recvReliablePackets.end();
						i = recvReliablePackets.find(lastProcessedReliablePacket+1)
					)
					{
						// Process and delete packet
						i->second->handle(*this);
						delete i->second;
						recvReliablePackets.erase(i);

						lastProcessedReliablePacket++;
						processed = true;
					}

					return processed;
				}
			}
		}
		else
		{
			packet->handle(*this);
			return true;
		}
	}
}

void NetManager::connect()
{
	NET_DEBUG("Opening local socket on port: " << localPort);
	socket = SDLNet_UDP_Open(localPort);
	if (socket == 0)
	{
		NET_DEBUG("\a7Error:\ax Failed to open UDP socket: " << SDLNet_GetError());
		throw ConnectErrorException(std::string("Failed to open UDP socket: ") + SDLNet_GetError());
	}

	{
		if (CVars::net_debug)
			Console::get() << "NET: Resolving " << opponentHost << "...";

		IPaddress addr;
		if (SDLNet_ResolveHost(&addr, opponentHost.c_str(), opponentPort) == -1)
		{
			NET_DEBUG(std::endl << "\a7Error:\ax Failed to resolve " << opponentHost);
			throw ConnectErrorException(std::string("Failed to resolve ") + opponentHost);
		}

		if (CVars::net_debug)
			Console::get() << (addr.host & 0x000000FF) << '.' <<
				((addr.host & 0x0000FF00) >> 8) << '.' <<
				((addr.host & 0x00FF0000) >> 16) << '.' <<
				((addr.host & 0xFF000000) >> 24) << std::endl;

		if (SDLNet_UDP_Bind(socket, 0, &addr) == -1)
		{
			NET_DEBUG("\a7Error:\ax Failed to bind socket: " << SDLNet_GetError());
			throw ConnectErrorException(std::string("Failed to bind socket: ") + SDLNet_GetError());
		}
	}

	// NAT Punch
	NET_DEBUG("Starting NAT punch.");
	int i = 0;
	while (!updateNetwork())
	{
		if (i-- == 0)
		{
			PacketNull packet;
			sendPacket(packet);
			i = 100;
		}

		service_SDL_events(true);
		JE_joystickTranslate();

		if (newkey && lastkey_sym == SDLK_ESCAPE)
			quit();

		SDL_Delay(10);
	}

	connecting = true;

	NET_DEBUG("Sending player info.");

	{
		// Compact available episodes into a byte
		BOOST_STATIC_ASSERT(EPISODE_MAX <= std::numeric_limits<Uint8>::digits);
		std::bitset<std::numeric_limits<Uint8>::digits> localAvailEpisodes;

		for (unsigned int i = 0; i < localAvailEpisodes.size(); ++i)
		{
			localAvailEpisodes[i] = (episodeAvail[i] != 0);
		}

		PacketConnect packet;
		packet.version = network_version;
		packet.delay = networkDelay;
		packet.episodes = static_cast<Uint8>(localAvailEpisodes.to_ulong());
		packet.player = localPlayerNum;
		packet.playerName = localPlayerName;
		sendPacket(packet);

		while (!peerInfoSet)
		{
			updateNetwork();

			service_SDL_events(true);
			JE_joystickTranslate();

			if (newkey && lastkey_sym == SDLK_ESCAPE)
				quit();

			SDL_Delay(10);
		}

		if (peerInfo.magic != 0xFFFF)
			quit("Invalid packet header.");

		if (peerInfo.version != network_version && !(network_version == 0 || peerInfo.version == 0 || CVars::net_ignore_version))
			quit((boost::format("Version mismatch. Local: %1%; Remote: %2%") % network_version % peerInfo.version).str());

		if (localPlayerNum == peerInfo.player)
			quit("Player number conflict.");

		std::bitset<std::numeric_limits<Uint8>::digits> remoteAvailEpisodes(peerInfo.episodes);

		for (unsigned int i = 0; i < remoteAvailEpisodes.size(); ++i)
		{
			episodeAvail[i] = episodeAvail[i] && remoteAvailEpisodes[i];
		}
	}

	connected = true;
}

bool NetManager::updateNetwork()
{
	if (connected)
	{
		// Send a PacketNull if we excedded the keep alive interval.
		if (SDL_GetTicks() - lastPacketSentTick > CVars::net_keepalive_interval)
		{
			PacketNull packet;
			sendPacket(packet);
		}

		// Check if any reliable packets excedded the resend interval without being acked.
		typedef std::pair<const Uint16, ReliablePairType> Pair;
		foreach (Pair& pair, sentReliablePackets)
		{
			if (SDL_GetTicks() - pair.second.second > CVars::net_resend_interval)
			{
				pair.second.second = SDL_GetTicks();
				sendPacket(*pair.second.first);
			}
		}
	}

	bool packetReceived = false;

	while(receiveAndProcessPacket())
	{
		packetReceived = true;
	}

	return packetReceived;
}

}

#include "palette.h"
#include "picload.h"
#include "video.h"
#include "newshape.h"
#include "fonthand.h"
#include "varz.h"

namespace network
{

void NetManager::quit()
{
	Console::get() << "Network quit: Aborted by user." << std::endl;

	PacketTerminate packet;
	packet.message = "Aborted by user.";
	sendPacket(packet);

	JE_fadeBlack(10);

	JE_tyrianHalt(5);
}

void NetManager::quit(std::string message)
{
	Console::get() << "Network quit: " << message << std::endl;

	PacketTerminate packet;
	packet.message = message;
	sendPacket(packet);

	JE_fadeBlack(10);

	tempScreenSeg = VGAScreen = VGAScreenSeg;

	JE_loadPic(2, false);
	JE_dString(JE_fontCenter(message, SMALL_FONT_SHAPES), 140, message, SMALL_FONT_SHAPES);
	JE_showVGA();
	JE_fadeColor(10);

	wait_noinput(true, true, true);
	wait_input(true, true, true);

	quit();
}

void NetManager::displayMessage(std::string message)
{
	JE_fadeBlack(10);

	tempScreenSeg = VGAScreen = VGAScreenSeg;

	JE_loadPic(2, false);
	JE_dString(JE_fontCenter(message, SMALL_FONT_SHAPES), 140, message, SMALL_FONT_SHAPES);
	JE_showVGA();
	JE_fadeColor(10);

	wait_noinput(true, true, true);
	wait_input(true, true, true);

	JE_fadeBlack(10);
}

}

network::NetManager *netmanager = 0;
