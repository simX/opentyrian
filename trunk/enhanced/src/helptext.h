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
#ifndef HELPTEXT_H
#define HELPTEXT_H

#include "opentyr.h"


static const int MAX_HELP_MESSAGE = 39;
static const int MAX_MENU = 14;

static const int DESTRUCT_MODES = 5;

extern int verticalHeight;
extern int helpBoxColor, helpBoxBrightness, helpBoxShadeType;
extern std::string helpTxt[MAX_HELP_MESSAGE];
extern std::string pName[21];              /* [1..21] of string [15] */
extern std::string miscText[68];           /* [1..68] of string [41] */
extern std::string miscTextB[5];           /* [1..5] of string [10] */
extern std::string keyName[8];             /* [1..8] of string [17] */
extern std::string menuText[7];            /* [1..7] of string [20] */
extern std::string outputs[9];             /* [1..9] of string [30] */
extern std::string topicName[6];           /* [1..6] of string [20] */
extern std::string mainMenuHelp[34];
extern std::string inGameText[6];          /* [1..6] of string [20] */
extern std::string detailLevel[5];         /* [1..6] of string [12] */
extern std::string gameSpeedText[5];       /* [1..5] of string [12] */
extern std::string episodeName[6];         /* [0..5] of string [30] */
extern std::string difficultyName[7];      /* [0..6] of string [20] */
extern std::string playerName[5];          /* [0..4] of string [25] */
extern std::string inputDevices[3];        /* [1..3] of string [12] */
extern std::string networkText[4];         /* [1..4] of string [20] */
extern std::string difficultyNameB[11];    /* [0..9] of string [20] */
extern std::string joyButtonNames[5];      /* [1..5] of string [20] */
extern std::string superShips[11];         /* [0..10] of string [25] */
extern std::string specialName[9];         /* [1..9] of string [9] */
extern std::string destructHelp[25];
extern std::string weaponNames[17];        /* [1..17] of string [16] */
extern std::string destructModeName[DESTRUCT_MODES]; /* [1..destructmodes] of string [12] */
extern std::string shipInfo[13][2];
extern std::string menuInt[MAX_MENU+1][11]; /* [0..maxmenu, 1..11] of string [17] */
extern const int menuHelp[MAX_MENU][11];   /* [1..maxmenu, 1..11] */

/*int temp, temp2;*/

void JE_helpBox( JE_word x, JE_word y, const std::string& message, unsigned int boxwidth );
void JE_HBox( JE_word x, JE_word y, int messagenum, unsigned int boxwidth );
void JE_loadHelpText( void );

#endif /* HELPTEXT_H */
