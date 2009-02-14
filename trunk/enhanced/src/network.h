/* vim: set noet:
 *
 * OpenTyrian Enhanced: A modern cross-platform port of Tyrian
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
#ifndef NETWORK_H
#define NETWORK_H
#include "opentyr.h"

#include "console/cvar/CVar.h"

#include "SDL_net.h"

//extern int thisPlayerNum;
extern bool haltGame; // TODO NETWORK check uses
extern bool netQuit; // TODO NETWORK check uses
//extern bool pauseRequest, skipLevelRequest, helpRequest, nortShipRequest;
extern bool yourInGameMenuRequest, inGameMenuRequest; // TODO NETWORK check uses
/*extern bool portConfigChange;

//extern bool network::netmanager;

namespace network
{
	enum NetworkPackets
	{
		PACKET_ACKNOWLEDGE = 0x00,
		PACKET_KEEP_ALIVE = 0x01,

		PACKET_CONNECT = 0x10, // version, delay, episodes, player_number, name
		PACKET_DETAILS = 0x11, // episode, difficulty

		PACKET_QUIT = 0x20,
		PACKET_WAITING = 0x21,
		PACKET_BUSY = 0x22,

		PACKET_GAME_QUIT = 0x30,
		PACKET_GAME_PAUSE = 0x31,
		PACKET_GAME_MENU = 0x32,

		PACKET_STATE_RESEND = 0x40, // state_id
		PACKET_STATE = 0x41, // <state> (not acknowledged)
		PACKET_STATE_XOR = 0x42 // <xor state> (not acknowledged)
	};

	extern unsigned int delay;

	extern std::string player_name;
	extern std::string opponent_name;

	extern UDPpacket *packet_out_temp;
	extern UDPpacket *packet_in[];
	extern UDPpacket *packet_out[];
	extern UDPpacket *packet_state_in[];
	extern UDPpacket *packet_state_out[];

	void prepare( NetworkPackets type );
	bool send( int len );
	bool send_no_ack( int len );

	int check( );
	int acknowledge( NetworkPackets sync );
	bool update( );

	bool is_sync( );
	bool is_alive( );
	static void keep_alive( )
	{
		if (network::netmanager)
			check();
	}

	void state_prepare( );
	int state_send( );
	bool state_update( );
	bool state_is_reset( );
	void state_reset( );

	int connect( );
	void tyrian_halt( unsigned int err, bool attempt_sync );

	int init( );

	void packet_copy( UDPpacket *dst, UDPpacket *src );
	void packets_shift_up( UDPpacket **dst, int max_packets );
	void packets_shift_down( UDPpacket **dst, int max_packets );
}
*/
void JE_clearSpecialRequests( void );

#endif /* NETWORK_H */
