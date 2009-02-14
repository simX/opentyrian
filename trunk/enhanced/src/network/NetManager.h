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
#ifndef NETWORK_NETMANAGER_H
#define NETWORK_NETMANAGER_H
#include "opentyr.h"

namespace network {
class NetManager;
}

#include "Packet.h"
#include "packets/PacketReliable.h"
#include "packets/PacketNetwork.h"
#include "packets/PacketConnect.h"
#include "svn_rev.h"

#include <queue>
#include <map>
#include "SDL_net.h"

namespace network
{

class NetManager
{
private:
	NetManager(const NetManager&);
	void operator=(const NetManager&);

public:
	const std::string opponentHost;
	const Uint16 opponentPort;
	const Uint16 localPort;
	const unsigned int localPlayerNum;
	const std::string localPlayerName;
	const unsigned int networkDelay;

private:
	UDPsocket socket;

	// Maintains a list of reliable packets that haven't been acknowledged yet.
	// std::map<seqId, std::pair<packet, sentTime>>
	typedef std::pair<PacketReliable*, Uint32> ReliablePairType;
	std::map<Uint16, ReliablePairType> sentReliablePackets;
	Uint16 nextReliableSeqId;
	Uint16 lastProcessedReliablePacket;
	std::map<Uint16, PacketReliable*> recvReliablePackets; // Used for in-order processing

	bool connected, connecting;
	PacketConnect peerInfo;
	bool peerInfoSet;
	Uint32 lastPacketSentTick;

	bool sendPacket(const Packet& packet);
	Packet *receivePacket();
	bool receiveAndProcessPacket();

public:
	struct NetInitException : std::runtime_error
	{
		NetInitException(const std::string& msg) : runtime_error(msg)
		{}
	};

	struct ConnectErrorException : std::runtime_error
	{
		ConnectErrorException(const std::string& msg) : runtime_error(msg)
		{}
	};

	Uint16 network_version;

	NetManager();
	~NetManager();

	void connect();
	bool updateNetwork();
	void quit();
	void quit(std::string message);
	static void displayMessage(std::string message);

 	friend void PacketAcknowledge::handle(NetManager&);
	friend void PacketConnect::handle(NetManager&);
};

}


// TODO Remove
#include "../network.h"

#endif // NETWORK_NETMANAGER_H
