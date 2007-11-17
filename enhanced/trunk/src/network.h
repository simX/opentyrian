/* vim: set noet:
 *
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
 * Copyright (C) 2007  The OpenTyrian Team
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

typedef struct
{
	JE_byte    buttons;
	int        xchg, ychg;
	JE_word    randomenemytype;    /* 0=no enemy */
	int        ex, ey;
	unsigned long sync;
} JE_PacketData;

#ifndef NO_EXTERNS
extern unsigned long startTime;
extern unsigned long frames;
extern int netPlayers, thisPlayerNum, otherPlayerNum;
extern bool haltGame;
extern bool netQuit;
extern bool done;
extern bool moveOk;
extern bool firstTime;
extern bool netResult;
extern JE_byte gameQuitDelay;
extern JE_byte outputData[10];
extern JE_byte inputData[10];
extern bool pauseRequest, skipLevelRequest, helpRequest, nortShipRequest;
extern bool yourInGameMenuRequest, inGameMenuRequest;
extern bool portConfigChange, portConfigDone;
extern JE_byte exchangeCount;
extern bool netSuccess;
#endif

void JE_initNetwork( void );

void JE_exchangePacket( JE_byte size );
void JE_recordPacket( int xchg, int ychg, int xachg, int yachg);
void JE_clearSpecialRequests( void );

void JE_updateStream( void );
void JE_setNetByte( JE_byte send );
bool JE_scanNetByte( JE_byte scan );
void JE_syncNet( JE_byte syncByte );

void JE_flushNet( void );

#endif /* NETWORK_H */
