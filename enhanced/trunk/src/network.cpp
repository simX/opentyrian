/* vim: set noet:
 *
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

#include "network.h"


unsigned long startTime;
unsigned long frames;

int netPlayers,     /* Number of net players: 0, 1 or 2 */
    thisPlayerNum = 0,  /* Player number on this PC (1 or 2) */
    otherPlayerNum; /* Player number on remote PC (1 or 2) */

/* TODO UseOutPacket : SMALL_PACKET;*/   /* This is the original player's packet - time lag */

bool haltGame;
bool netQuit;
bool done;

bool moveOk;
bool firstTime;
bool netResult;

int gameQuitDelay;  /* Make sure the game doesn't quit within the first few frames of starting */

Uint8 outputData[10]; /* [1..10] */
Uint8 inputData[10]; /* [1..10] */

/* Special Requests */
bool pauseRequest, skipLevelRequest, helpRequest, nortShipRequest;
bool yourInGameMenuRequest, inGameMenuRequest;

bool portConfigChange;

int exchangeCount;

/* Network Stuff */
bool netSuccess;

void JE_updateStream( void )
{
//	STUB();
}

void JE_setNetByte( Uint8 send )
{
//	STUB();
}

bool JE_scanNetByte( Uint8 scan )
{
//	STUB();
	return true;
}

void JE_clearSpecialRequests( void )
{
	pauseRequest = false;
	inGameMenuRequest = false;
	skipLevelRequest = false;
	helpRequest = false;
	nortShipRequest = false;
}

/* TODO */
