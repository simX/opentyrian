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
#ifndef NETWORK_NETMANAGER_H
#define NETWORK_NETMANAGER_H
#include "opentyr.h"

#include "Packet.h"
#include "svn_rev.h"

#include <queue>
#include "SDL_net.h"

class NetManager
{
private:
	NetManager(const NetManager&);
	void operator=(const NetManager&);

	const std::string opponentHost;
	const Uint16 opponentPort;
	const Uint16 localPort;
	const unsigned int localPlayerNum;
	const std::string localPlayerName;
	const unsigned int networkDelay;

	UDPsocket socket;

	// Head is the last-acked packet, Tail is last-send packet
	std::queue<Packet*> outHistory;

	bool connected, connecting;
	Uint32 lastKeepAliveTick;

	bool sendPacket(Packet& packet);
	bool checkNullPacket();

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

	static const Uint16 NETWORK_VERSION = SVN_REV;

	NetManager();

	void connect();
	void updateNetwork();
};

#endif // NETWORK_NETMANAGER_H
