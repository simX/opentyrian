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
#ifndef NETWORK_H
#define NETWORK_H

#include "opentyr.h"


#define PACKET_MAXIMUM 3

struct JE_PacketData
{
	int buttons;
	int xchg, ychg;
	JE_word randomenemytype;    /* 0=no enemy */
	int ex, ey;
	unsigned long sync;
};

extern unsigned long startTime;
extern unsigned long frames;
extern int netPlayers, thisPlayerNum, otherPlayerNum;
extern bool haltGame;
extern bool netQuit;
extern bool done;
extern bool moveOk;
extern bool firstTime;
extern bool netResult;
extern int gameQuitDelay;
extern Uint8 outputData[10];
extern Uint8 inputData[10];
extern bool pauseRequest, skipLevelRequest, helpRequest, nortShipRequest;
extern bool yourInGameMenuRequest, inGameMenuRequest;
extern bool portConfigChange;
extern int exchangeCount;
extern bool netSuccess;

void JE_initNetwork( void );

void JE_exchangePacket( int size );
void JE_recordPacket( int xchg, int ychg, int xachg, int yachg);
void JE_clearSpecialRequests( void );

void JE_updateStream( void );
void JE_setNetByte( Uint8 send );
bool JE_scanNetByte( Uint8 scan );
void JE_syncNet( Uint8 syncByte );

void JE_flushNet( void );

#endif /* NETWORK_H */
