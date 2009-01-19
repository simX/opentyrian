/* vim: set noet:
 *
 * OpenTyrian Enhanced: A modern cross-platform port of Tyrian
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
#include "network.h"

#include "fonthand.h"
#include "palette.h"
#include "video.h"
#include "picload.h"
#include "newshape.h"
#include "keyboard.h"
#include "varz.h"
#include "joystick.h"
#include "console/cvar/Cvar.h"
#include "console/CCmd.h"
#include "network/NetCVars.h"

#include <cassert>
#include "SDL_types.h"
#include "SDL_net.h"
#include "boost/lexical_cast.hpp"

using boost::lexical_cast;

namespace network
{
	static const unsigned int VERSION = 2;

	static const unsigned int PACKET_SIZE = 256;
	static const unsigned int PACKET_QUEUE = 16;

	static const unsigned int RETRY = 640; // ticks to wait for packet acknowledgement before resending
	static const unsigned int RESEND = 320; // ticks to wait before requesting unreceived game packet
	static const unsigned int KEEP_ALIVE = 1600; // ticks to wait between keep-alive packets
	static const unsigned int TIME_OUT = 16000; // ticks to wait before considering connection dead

	int delay; // minimum is 1 + 0

	static std::string opponent_host;
	static Uint16 player_port;
	static Uint16 opponent_port;
	std::string player_name;
	std::string opponent_name;

	static UDPsocket socket;
	static IPaddress ip;

	UDPpacket *packet_out_temp, *packet_temp;

	UDPpacket *packet_in[PACKET_QUEUE];
	UDPpacket *packet_out[PACKET_QUEUE];

	Uint16 last_out_sync, queue_in_sync, queue_out_sync, last_ack_sync;
	Uint32 last_in_tick, last_out_tick;

	UDPpacket *packet_state_in[PACKET_QUEUE];
	UDPpacket *packet_state_in_xor[PACKET_QUEUE];
	UDPpacket *packet_state_out[PACKET_QUEUE];

	Uint16 last_state_in_sync, last_state_out_sync;
	Uint32 last_state_in_tick;

	static bool connected, quit;
}

// Special Requests
int thisPlayerNum;
bool haltGame, netQuit, moveOk;
bool pauseRequest, skipLevelRequest, helpRequest, nortShipRequest;
bool yourInGameMenuRequest, inGameMenuRequest;
bool portConfigChange;

bool isNetworkGame;

void network::prepare( NetworkPackets type )
{
	SDLNet_Write16(type, packet_out_temp->data+0);
	SDLNet_Write16(last_out_sync, packet_out_temp->data+2);
}

bool network::send( int len )
{
	bool temp = send_no_ack(len);

	Uint16 i = last_out_sync - queue_out_sync;
	if (i < PACKET_QUEUE)
	{
		packet_out[i] = SDLNet_AllocPacket(PACKET_SIZE);
		packet_copy(packet_out[i], packet_out_temp);
	}
	else
	{
		// connection is probably bad now
		Console::get() << "\a7Warning:\ax Outbound packet queue overflow." << std::endl;
	}

	last_out_sync++;

	if (is_sync())
		last_out_tick = SDL_GetTicks();

	return temp;
}

// send packet but don't expect ack of delivery
bool network::send_no_ack( int len )
{
	packet_out_temp->len = len;

	if (!SDLNet_UDP_Send(socket, 0, packet_out_temp))
	{
		Console::get() << "\a7Warning:\ax Failed to send packet: " << SDLNet_GetError() << std::endl;
		return false;
	}

	return true;
}

// poll for new packets received, check that connection is alive, resend queued packets if necessary
int network::check( )
{
	if (connected)
	{
		// timeout
		if (!is_alive())
		{
			if (!quit)
				tyrian_halt(2, false);
		}

		// keep-alive
		static Uint32 keep_alive_tick = 0;
		if (SDL_GetTicks() - keep_alive_tick > KEEP_ALIVE)
		{
			prepare(PACKET_KEEP_ALIVE);
			send_no_ack(4);

			keep_alive_tick = SDL_GetTicks();
		}
	}

	// retry
	if (packet_out[0] && SDL_GetTicks() - last_out_tick > RETRY)
	{
		if (!SDLNet_UDP_Send(socket, 0, packet_out[0]))
		{
			Console::get() << "\a7Warning:\ax Failed to send packet: " << SDLNet_GetError() << std::endl;
			return -1;
		}

		last_out_tick = SDL_GetTicks();
	}
	
	switch (SDLNet_UDP_Recv(socket, packet_temp))
	{
	case -1:
		Console::get() << "\a7Warning:\ax Failed to receive packet: " << SDLNet_GetError() << std::endl;
		return -1;
		break;
	case 0:
		break;
	default:
		if (packet_temp->channel == 0 && packet_temp->len >= 4)
		{
			switch (SDLNet_Read16(packet_temp->data+0))
			{
			case PACKET_ACKNOWLEDGE:
				if ((Uint16)(SDLNet_Read16(packet_temp->data+2) - last_ack_sync) < PACKET_QUEUE)
				{
					last_ack_sync = SDLNet_Read16(packet_temp->data+2);
				}

				{
					Uint16 i = SDLNet_Read16(packet_temp->data+2) - queue_out_sync;
					if (i < PACKET_QUEUE)
					{
						if (packet_out[i])
						{
							SDLNet_FreePacket(packet_out[i]);
							packet_out[i] = 0;
						}
					}
				}

				// remove acknowledged packets from queue
				while (packet_out[0] == 0 && (Uint16)(last_ack_sync - queue_out_sync) < PACKET_QUEUE)
				{
					packets_shift_up(packet_out, PACKET_QUEUE);
					queue_out_sync++;
				}

				last_in_tick = SDL_GetTicks();
				break;
			case PACKET_CONNECT:
				queue_in_sync = SDLNet_Read16(packet_temp->data+2);
				
				for (int i = 0; i < PACKET_QUEUE; ++i)
				{
					if (packet_in[i])
					{
						SDLNet_FreePacket(packet_in[i]);
						packet_in[i] = 0;
					}
				}
				// Intentional fall-through!
			case PACKET_DETAILS:
			case PACKET_WAITING:
			case PACKET_BUSY:
			case PACKET_GAME_QUIT:
			case PACKET_GAME_PAUSE:
			case PACKET_GAME_MENU:
				{
					Uint16 i = SDLNet_Read16(packet_temp->data+2) - queue_in_sync;
					if (i < PACKET_QUEUE)
					{
						if (packet_in[i] == 0)
							packet_in[i] = SDLNet_AllocPacket(PACKET_SIZE);
						packet_copy(packet_in[i], packet_temp);
					}
					else
					{
						// inbound packet queue overflow/underflow
						// under normal circumstances, this is ok
					}
				}

				acknowledge(NetworkPackets(SDLNet_Read16(packet_temp->data+2)));

				// Intentional fall-through!
			case PACKET_KEEP_ALIVE:
				last_in_tick = SDL_GetTicks();
				break;
			case PACKET_QUIT:
				if (!quit)
				{
					prepare(PACKET_QUIT);
					send(4); // PACKET_QUIT
				}

				acknowledge(NetworkPackets(SDLNet_Read16(packet_temp->data+2)));

				if (!quit)
					tyrian_halt(1, true);
				break;
			case PACKET_STATE:
				// place packet in queue if within limits
				{
					Uint16 i = SDLNet_Read16(packet_temp->data+2) - last_state_in_sync+1;
					if (i < PACKET_QUEUE)
					{
						if (packet_state_in[i] == 0)
							packet_state_in[i] = SDLNet_AllocPacket(PACKET_SIZE);
						packet_copy(packet_state_in[i], packet_temp);
					}
				}
				break;
			case PACKET_STATE_XOR:
				// place packet in queue if within limits
				{
					Uint16 i = SDLNet_Read16(packet_temp->data+2) - last_state_in_sync+1;
					if (i < PACKET_QUEUE)
					{
						if (packet_state_in_xor[i] == 0)
						{
							packet_state_in_xor[i] = SDLNet_AllocPacket(PACKET_SIZE);
							packet_copy(packet_state_in_xor[i], packet_temp);
						}
						else if (SDLNet_Read16(packet_state_in_xor[i]->data+0) != PACKET_STATE_XOR)
						{
							for (int j = 4; j < packet_state_in_xor[i]->len; ++j)
								packet_state_in_xor[i]->data[j] ^= packet_temp->data[j];

							SDLNet_Write16(PACKET_STATE_XOR, packet_state_in_xor[i]->data+0);
						}
					}
				}
				break;
			case PACKET_STATE_RESEND:
				// resend requested state packet if still available
				{
					Uint16 i = last_state_out_sync - SDLNet_Read16(packet_temp->data+2);
					if (i > 0 && i < PACKET_QUEUE)
					{
						if (packet_state_out[i])
						{
							if (!SDLNet_UDP_Send(socket, 0, packet_state_out[i]))
							{
								Console::get() << "\a7Warning:\ax Failed to send packet: " << SDLNet_GetError() << std::endl;
								return -1;
							}
						}
					}
				}
				break;
			default:
				Console::get() << "\a7Warning:\ax Unknow packet " << SDLNet_Read16(packet_temp->data+0) << " received." << std::endl;
				return 0;
				break;
			}
			return 1;
		}
		break;
	}

	return 0;
}

// send ack packet
int network::acknowledge( NetworkPackets sync )
{
	SDLNet_Write16(PACKET_ACKNOWLEDGE, packet_out_temp->data+0);
	SDLNet_Write16(sync, packet_out_temp->data+2);
	send_no_ack(4);

	return 0;
}

// discard working packet, now processing next packet in queue
bool network::update( )
{
	if (packet_in[0])
	{
		packets_shift_up(packet_in, PACKET_QUEUE);
		queue_in_sync++;
		return true;
	}
	else
		return false;
}

// has opponent gotten all the packets we've sent?
bool network::is_sync( )
{
	return queue_out_sync - last_ack_sync == 1;
}

// activity lately?
bool network::is_alive( )
{
	return (SDL_GetTicks() - last_in_tick < TIME_OUT ||
		SDL_GetTicks() - last_state_in_tick < TIME_OUT);
}

// prepare new state for sending
void network::state_prepare( )
{
	if (packet_state_out[0])
	{
		Console::get() << "\a7Warning:\ax State packet overwritten (previous packet remains unsent)" << std::endl;
	}
	else
	{
		packet_state_out[0] = SDLNet_AllocPacket(PACKET_SIZE);
		packet_state_out[0]->len = 28;
	}

	SDLNet_Write16(PACKET_STATE, packet_state_out[0]->data);
	SDLNet_Write16(last_state_out_sync, packet_state_out[0]->data + 2);
	std::fill_n(packet_state_out[0]->data + 4, 28-4, 0);
}

// send state packet, xor packet if applicable
int network::state_send( )
{
	if (!SDLNet_UDP_Send(socket, 0, packet_state_out[0]))
	{
		Console::get() << "\a7Warning:\ax Failed to send packet: " << SDLNet_GetError() << std::endl;
		return -1;
	}

	// send xor of last network_delay packets
	if (delay > 1 && (last_state_out_sync+1) % delay == 0 &&
		packet_state_out[delay-1] != 0)
	{
		packet_copy(packet_temp, packet_state_out[0]);
		SDLNet_Write16(PACKET_STATE_XOR, packet_temp->data+0);
		for (int i = 1; i < delay; ++i)
			for (int j = 4; j < packet_temp->len; ++j)
				packet_temp->data[j] ^= packet_state_out[i]->data[j];

		if (!SDLNet_UDP_Send(socket, 0, packet_temp))
		{
			Console::get() << "\a7Warning:\ax Failed to send packet: " << SDLNet_GetError() << std::endl;
			return -1;
		}
	}

	packets_shift_down(packet_state_out, PACKET_QUEUE);
	last_state_out_sync++;

	return 0;
}

// receive state packet, wait until received
bool network::state_update( )
{
	if (state_is_reset())
	{
		return 0;
	}
	else
	{
		packets_shift_up(packet_state_in, PACKET_QUEUE);
		packets_shift_up(packet_state_in_xor, PACKET_QUEUE);
		last_state_in_sync++;

		// current xor packet index
		int x = delay - (last_state_in_sync-1) % delay-1;

		// loop until needed packet is available
		while (!packet_state_in[0])
		{
			// xor the packet from thin air, if possible
			if (packet_state_in_xor[x] && SDLNet_Read16(packet_state_in_xor[x]->data+0) == PACKET_STATE_XOR)
			{
				// check for all other required packets
				bool okay = true;
				for (int i = 1; i <= x; ++i)
				{
					if (packet_state_in[i] == 0)
					{
						okay = false;
						break;
					}
				}

				if (okay)
				{
					packet_state_in[0] = SDLNet_AllocPacket(PACKET_SIZE);
					packet_copy(packet_state_in[0], packet_state_in_xor[x]);
					for (int i = 1; i <= x; ++i)
						for (int j = 4; j < packet_state_in[0]->len; ++j)
							packet_state_in[0]->data[j] ^= packet_state_in[i]->data[j];
					break;
				}
			}

			static Uint32 resend_tick = 0;
			if (SDL_GetTicks() - last_state_in_tick > RESEND && SDL_GetTicks() - resend_tick > RESEND)
			{
				SDLNet_Write16(PACKET_STATE_RESEND, packet_out_temp->data+0);
				SDLNet_Write16(last_state_in_sync-1, packet_out_temp->data+2);
				send_no_ack(4);

				resend_tick = SDL_GetTicks();
			}

			if (check() == 0)
				SDL_Delay(1);
		}

		if (delay > 1)
		{
			// process the current packet against the xor queue
			if (packet_state_in_xor[x] == 0)
			{
				packet_state_in_xor[x] = SDLNet_AllocPacket(PACKET_SIZE);
				packet_copy(packet_state_in_xor[x], packet_state_in[0]);
				packet_state_in_xor[x]->status = 0;
			}
			else
			{
				for (int j = 4; j < packet_state_in_xor[x]->len; ++j)
					packet_state_in_xor[x]->data[j] ^= packet_state_in[0]->data[j];
			}
		}

		last_state_in_tick = SDL_GetTicks();
	}

	return 1;
}

// ignore first network::delay states of level
bool network::state_is_reset( )
{
	return (last_state_out_sync < delay);
}

// reset queue for new level
void network::state_reset( )
{
	last_state_in_sync = last_state_out_sync = 0;

	for (int i = 0; i < PACKET_QUEUE; ++i)
	{
		if (packet_state_in[i])
		{
			SDLNet_FreePacket(packet_state_in[i]);
			packet_state_in[i] = 0;
		}

		if (packet_state_in_xor[i])
		{
			SDLNet_FreePacket(packet_state_in_xor[i]);
			packet_state_in_xor[i] = 0;
		}

		if (packet_state_out[i])
		{
			SDLNet_FreePacket(packet_state_out[i]);
			packet_state_out[i] = 0;
		}
	}

	last_state_in_tick = SDL_GetTicks();
}

// attempt to punch through firewall by firing off UDP packets at the opponent
// exchange game information
int network::connect( )
{
	SDLNet_ResolveHost(&ip, opponent_host.c_str(), opponent_port);

	SDLNet_UDP_Bind(socket, 0, &ip);

	Uint8 episodes = 0, episodes_local = 0;
	assert(EPISODE_MAX <= 16);
	for (int i = EPISODE_MAX-1; i >= 0; --i)
	{
		episodes <<= 1;
		episodes |= (episodeAvail[i] != 0);
	}
	episodes_local = episodes;

	assert(PACKET_SIZE - 11 >= 20+1);
	if (player_name.length() > 20)
		player_name.erase(player_name.begin()+20, player_name.end());

connect_reset:
	prepare(PACKET_CONNECT);
	SDLNet_Write16(0xFFFF, packet_out_temp->data+4);
	SDLNet_Write16(VERSION, packet_out_temp->data+6);
	packet_out_temp->data[8] = delay;
	packet_out_temp->data[9] = episodes;
	packet_out_temp->data[10] = thisPlayerNum;
	packet_out_temp->data[11+player_name.copy(reinterpret_cast<char*>(packet_out_temp->data)+11, 20)] = '\0';

	send(11+ player_name.length() + 1); // PACKET_CONNECT

	// Wait until opponent sends packet
	//while (SDLNet_Read16(packet_in->data+0) != PACKET_CONNECT)
	for (;;)
	{
		service_SDL_events(true);
		JE_joystickTranslate();

		if (newkey && lastkey_sym == SDLK_ESCAPE)
			tyrian_halt(0, false);

		// never time out
		last_in_tick = SDL_GetTicks();

		if (packet_in[0] && SDLNet_Read16(packet_in[0]->data+0) == PACKET_CONNECT)
			break;

		update();
		check();

		SDL_Delay(16);
	}

connect_again:
	if (SDLNet_Read16(packet_in[0]->data+4) != 0xFFFF)
	{
		Console::get() << "\a7Error:\ax Opponent isn't using Enhanced." << std::endl;
		tyrian_halt(4, true);
	}
	if (SDLNet_Read16(packet_in[0]->data+6) != VERSION)
	{
		Console::get() << "\a7Error:\ax Network version doesn't match opponent's." << std::endl;
		tyrian_halt(4, true);
	}
	if (packet_in[0]->data[8] != delay)
	{
		Console::get() << "\a7Error:\ax Network delay doesn't match opponent's." << std::endl;
		tyrian_halt(5, true);
	}
	if (packet_in[0]->data[10] == thisPlayerNum)
	{
		Console::get() << "\a7Error:\ax Player number conflicts with opponent's." << std::endl;
		tyrian_halt(6, true);
	}

	episodes = packet_in[0]->data[9];
	for (int i = 0; i < EPISODE_MAX; ++i)
	{
		episodeAvail[i] = episodeAvail[i] && (episodes & 1);
		episodes >>= 1;
	}

	opponent_name.assign(reinterpret_cast<char*>(packet_in[0]->data)+11);

	update();

	// until opponent has acked
	while (!is_sync())
	{
		service_SDL_events(false);

		if (packet_in[0] && SDLNet_Read16(packet_in[0]->data+0) == PACKET_CONNECT)
			goto connect_again;
		
		check();

		if (SDL_GetTicks() - last_out_tick > RETRY)
			goto connect_reset;

		SDL_Delay(16);
	}

	prepare(PACKET_CONNECT);
	SDLNet_Write16(0xFFFF, packet_out_temp->data+4);
	SDLNet_Write16(VERSION, packet_out_temp->data+6);
	packet_out_temp->data[8] = delay;
	packet_out_temp->data[9] = episodes;
	packet_out_temp->data[10] = thisPlayerNum;
	packet_out_temp->data[11+player_name.copy(reinterpret_cast<char*>(packet_out_temp->data)+11, 20)] = '\0';
	send(11+ player_name.length() + 1); // PACKET_CONNECT

	connected = true;

	return 0;
}

// display error message
// TODO: This should probably be separate somewhere else since it seems to useful
void network::tyrian_halt( int err, bool attempt_sync )
{
	static const std::string err_msg[] = {
		"Quitting...",
		"Other player quit the game.",
		"Network connection was lost.",
		"Network connection failed.",
		"Network version mismatch.",
		"Network delay mismatch.",
		"Network player number conflict."
	};

	quit = true;

	if (err >= COUNTOF(err_msg))
		err = 0;

	JE_fadeBlack(10);

	tempScreenSeg = VGAScreen = VGAScreenSeg;

	JE_loadPic(2, false);
	JE_dString(JE_fontCenter(err_msg[err], SMALL_FONT_SHAPES), 140, err_msg[err], SMALL_FONT_SHAPES);

	JE_showVGA();
	JE_fadeColor(10);

	if (attempt_sync)
	{
		while (!is_sync() && is_alive())
		{
			service_SDL_events(false);

			check();
			SDL_Delay(16);
		}
	}

	if (err > 0)
	{
		wait_input(true, true, true);
	}

	JE_fadeBlack(10);

	SDLNet_Quit();
	JE_tyrianHalt(5);
}

int network::init( )
{
	Console::get() << "Initializing network..." << std::endl;

	opponent_host = CVars::net_host;
	opponent_port = static_cast<Uint16>(CVars::net_port);
	thisPlayerNum = CVars::net_player;
	player_name = CVars::net_name;
	player_port = static_cast<Uint16>(CVars::net_local_port);
	delay = CVars::net_delay;

	if (delay * 2 > PACKET_QUEUE - 2)
	{
		Console::get() << "\a7Error:\ax Network delay would overflow packet queue." << std::endl;
		return -4;
	}

	if (SDLNet_Init() == -1)
	{
		Console::get() << "\a7Error:\ax Failed to initialized SDL_net: " << SDLNet_GetError() <<  std::endl;
		return -1;
	}

	socket = SDLNet_UDP_Open(player_port);
	if (!socket)
	{
		Console::get() << "\a7Error:\ax Failed to open UDP socket: " << SDLNet_GetError() << std::endl;
		return -2;
	}

	packet_temp = SDLNet_AllocPacket(PACKET_SIZE);
	packet_out_temp = SDLNet_AllocPacket(PACKET_SIZE);

	if (!packet_temp || !packet_out_temp)
	{
		Console::get() << "\a7Error:\ax Failed to allocate packets: " << SDLNet_GetError() << std::endl;
		return -3;
	}

	return 0;
}

void network::packet_copy( UDPpacket *dst, UDPpacket *src )
{
	Uint8 *temp = dst->data;
	*dst = *src;
	dst->data = temp;
	std::copy(src->data, src->data+src->len, dst->data);
}

void network::packets_shift_up( UDPpacket **packet, int max_packets )
{
	if (packet[0])
	{
		SDLNet_FreePacket(packet[0]);
	}

	for (int i = 0; i < max_packets-1; ++i)
	{
		packet[i] = packet[i+1];
	}
	packet[max_packets-1] = 0;
}

void network::packets_shift_down( UDPpacket **packet, int max_packets )
{
	if (packet[max_packets-1])
	{
		SDLNet_FreePacket(packet[max_packets-1]);
	}

	for (int i = max_packets-1; i > 0; --i)
	{
		packet[i] = packet[i-1];
	}
	packet[0] = 0;
}

void JE_clearSpecialRequests( )
{
	pauseRequest = false;
	inGameMenuRequest = false;
	skipLevelRequest = false;
	helpRequest = false;
	nortShipRequest = false;
}
