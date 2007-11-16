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
#ifndef HELPTEXT_H
#define HELPTEXT_H

#include "opentyr.h"


#define MAX_HELP_MESSAGE 39
#define MAX_MENU 14

#define DESTRUCT_MODES 5

#ifndef NO_EXTERNS
extern JE_byte verticalHeight;
extern JE_byte helpBoxColor, helpBoxBrightness, helpBoxShadeType;
extern char helpTxt[MAX_HELP_MESSAGE][231];
extern char pName[21][16];              /* [1..21] of string [15] */
extern char miscText[68][42];           /* [1..68] of string [41] */
extern char miscTextB[5][11];           /* [1..5] of string [10] */
extern char keyName[8][18];             /* [1..8] of string [17] */
extern char menuText[7][21];            /* [1..7] of string [20] */
extern char outputs[9][31];             /* [1..9] of string [30] */
extern char topicName[6][21];           /* [1..6] of string [20] */
extern char mainMenuHelp[34][66];
extern char inGameText[6][21];          /* [1..6] of string [20] */
extern char detailLevel[6][13];         /* [1..6] of string [12] */
extern char gameSpeedText[5][13];       /* [1..5] of string [12] */
extern char episodeName[6][31];         /* [0..5] of string [30] */
extern char difficultyName[7][21];      /* [0..6] of string [20] */
extern char playerName[5][26];          /* [0..4] of string [25] */
extern char inputDevices[3][13];        /* [1..3] of string [12] */
extern char networkText[4][21];         /* [1..4] of string [20] */
extern char difficultyNameB[10][21];    /* [0..9] of string [20] */
extern char joyButtonNames[5][21];      /* [1..5] of string [20] */
extern char superShips[11][26];         /* [0..10] of string [25] */
extern char specialName[9][10];         /* [1..9] of string [9] */
extern char destructHelp[25][22];
extern char weaponNames[17][17];        /* [1..17] of string [16] */
extern char destructModeName[DESTRUCT_MODES][13]; /* [1..destructmodes] of string [12] */
extern char shipInfo[13][2][256];
extern char menuInt[MAX_MENU+1][11][18]; /* [0..maxmenu, 1..11] of string [17] */
extern JE_byte menuHelp[MAX_MENU][11];   /* [1..maxmenu, 1..11] */
#endif

/*JE_byte temp, temp2;*/

void JE_helpBox( JE_word x, JE_word y, char *message, JE_byte boxwidth );
void JE_HBox( JE_word x, JE_word y, JE_byte messagenum, JE_byte boxwidth );
void JE_loadHelpText( void );

#endif /* HELPTEXT_H */
