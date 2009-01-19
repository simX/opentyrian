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
#include "helptext.h"

#include "config.h"
#include "episodes.h"
#include "error.h"
#include "fonthand.h"
#include "newshape.h"
#include "Filesystem.h"
#include "BinaryStream.h"

#include <string.h>


const int menuHelp[MAX_MENU][11] = /* [1..maxmenu, 1..11] */
{
	{  1, 34,  2,  3,  4,  5,                  0, 0, 0, 0, 0 },
	{  6,  7,  8,  9, 10, 11, 11, 12,                0, 0, 0 },
	{ 13, 14, 15, 15, 16, 17, 12,                 0, 0, 0, 0 },
	{                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{  4, 30, 30,  3,  5,                   0, 0, 0, 0, 0, 0 },
	{                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 16, 17, 15, 15, 12,                   0, 0, 0, 0, 0, 0 },
	{ 31, 31, 31, 31, 32, 12,                  0, 0, 0, 0, 0 },
	{  4, 34,  3,  5,                    0, 0, 0, 0, 0, 0, 0 }
};

int verticalHeight = 7;
int helpBoxColor = 12;
int helpBoxBrightness = 1;
int helpBoxShadeType = FULL_SHADE;

std::string helpTxt[MAX_HELP_MESSAGE]; /* [1..maxhelpmessage] of string [230]; */
std::string pName[21];                /* [1..21] of string [15] */
std::string miscText[68];             /* [1..68] of string [41] */
std::string miscTextB[5];             /* [1..5] of string [10] */
std::string keyName[8];               /* [1..8] of string [17] */
std::string menuText[7];              /* [1..7] of string [20] */
std::string outputs[9];               /* [1..9] of string [30] */
std::string topicName[6];             /* [1..6] of string [20] */
std::string mainMenuHelp[34];
std::string inGameText[6];            /* [1..6] of string [20] */
std::string detailLevel[5];           /* [1..6] of string [12] */
std::string gameSpeedText[5];         /* [1..5] of string [12] */
std::string episodeName[6];           /* [0..5] of string [30] */
std::string difficultyName[7];        /* [0..6] of string [20] */
std::string playerName[5];            /* [0..4] of string [25] */
std::string inputDevices[3];          /* [1..3] of string [12] */
std::string networkText[4];           /* [1..4] of string [20] */
std::string difficultyNameB[11];      /* [0..9] of string [20] */
std::string joyButtonNames[5];        /* [1..5] of string [20] */
std::string superShips[11];           /* [0..10] of string [25] */
std::string specialName[9];           /* [1..9] of string [9] */
std::string destructHelp[25];
std::string weaponNames[17];          /* [1..17] of string [16] */
std::string destructModeName[DESTRUCT_MODES]; /* [1..destructmodes] of string [12] */
std::string shipInfo[13][2];
std::string menuInt[MAX_MENU + 1][11];   /* [0..maxmenu, 1..11] of string [17] */


void JE_helpBox( JE_word x, JE_word y, const std::string& message, unsigned int boxwidth )
{
	// TODO REWRITE
	if (message.empty())
	{
		return;
	}

	unsigned int startpos, endpos, pos;
	bool endstring;

	pos = 1;
	endpos = 0;
	endstring = false;

	do
	{
		startpos = endpos + 1;

		do
		{
			endpos = pos;
			do
			{
				pos++;
				if (pos == message.length())
				{
					endstring = true;
					if (pos - startpos < boxwidth)
					{
						endpos = pos + 1;
					}
				}

			} while (!(message[pos-1] == ' ' || endstring));

		} while (!(pos - startpos > boxwidth || endstring));

		JE_textShade(x, y, message.substr(startpos-1, endpos-startpos), helpBoxColor, helpBoxBrightness, helpBoxShadeType);

		y += verticalHeight;

	} while (!endstring);

	if (endpos != pos + 1)
	{
		JE_textShade(x, y, message.substr(endpos), helpBoxColor, helpBoxBrightness, helpBoxShadeType);
	}

	helpBoxColor = 12;
	helpBoxShadeType = FULL_SHADE;
}

void JE_HBox( JE_word x, JE_word y, int messagenum, unsigned int boxwidth )
{
	JE_helpBox(x, y, helpTxt[messagenum-1], boxwidth);
}

void JE_loadHelpText( void )
{
	std::fstream f;
	Filesystem::get().openDatafileFail(f, "tyrian.hdt");
	episode1DataLoc = IBinaryStream(f).getS32();
	JE_skipCryptLn(f);

	for (int i = 0; i < MAX_HELP_MESSAGE; i++)
	{
		helpTxt[i] = JE_readCryptLn(f);       /*Online Help*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 21; i++)
	{
		pName[i] = JE_readCryptLn(f);         /*Planet names*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 68; i++)
	{
		miscText[i] = JE_readCryptLn(f);      /*Miscellaneous text*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 5; i++)
	{
		miscTextB[i] = JE_readCryptLn(f);     /*Little Miscellaneous text*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	menuInt[5][0] = JE_readCryptLn(f);
	for (int i = 0; i < 8; i++) {
		JE_skipCryptLn(f);
	}
	for (int i = 1; i < 3; i++)
	{
		menuInt[5][i] = JE_readCryptLn(f);    /*Key names*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 7; i++)
	{
		menuText[i] = JE_readCryptLn(f);      /*Main Menu*/
	}
	// Replaces "Ordering Info"
	menuText[4] = "OpenTyrian";
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 9; i++)
	{
		outputs[i] = JE_readCryptLn(f);       /*Event text*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 6; i++)
	{
		topicName[i] = JE_readCryptLn(f);     /*Help topics*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 34; i++)
	{
		mainMenuHelp[i] = JE_readCryptLn(f);  /*Main Menu Help*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 7; i++)
	{
		menuInt[0][i] = JE_readCryptLn(f);    /*Menu 1 - Main*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 9; i++)
	{
		menuInt[1][i] = JE_readCryptLn(f);    /*Menu 2 - Items*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 8; i++)
	{
		menuInt[2][i] = JE_readCryptLn(f);    /*Menu 3 - Options*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 6; i++)
	{
		inGameText[i] = JE_readCryptLn(f);    /*InGame Menu*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 5; i++)
	{
		if (i == 4) {
			JE_skipCryptLn(f);
		}
		detailLevel[i] = JE_readCryptLn(f);   /*Detail Level*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 5; i++)
	{
		gameSpeedText[i] = JE_readCryptLn(f); /*Game speed text*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i <= 5; i++)
	{
		episodeName[i] = JE_readCryptLn(f);   /*Episode Names*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i <= 6; i++)
	{
		difficultyName[i] = JE_readCryptLn(f);/*Difficulty Level*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i <= 4; i++)
	{
		playerName[i] = JE_readCryptLn(f);    /*Player Names*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 6; i++)
	{
		menuInt[9][i] = JE_readCryptLn(f);       /*Menu 10 - 2Player Main*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 3; i++)
	{
		inputDevices[i] = JE_readCryptLn(f);      /*Input Devices*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 4; i++)
	{
		networkText[i] = JE_readCryptLn(f);       /*Network text*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 4; i++)
	{
		menuInt[10][i] = JE_readCryptLn(f);       /*Menu 11 - 2Player Network*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i <= 10; i++)
	{
		difficultyNameB[i] = JE_readCryptLn(f);   /*HighScore Difficulty Names*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 6; i++)
	{
		menuInt[11][i] = JE_readCryptLn(f);       /*Menu 12 - Network Options*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 7; i++)
	{
		menuInt[12][i] = JE_readCryptLn(f);       /*Menu 13 - Joystick*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 5; i++)
	{
		joyButtonNames[i] = JE_readCryptLn(f);    /*Joystick Button Assignments*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i <= 10; i++)
	{
		superShips[i] = JE_readCryptLn(f);        /*SuperShips - For Super Arcade Mode*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 9; i++)
	{
		specialName[i] = JE_readCryptLn(f);       /*SuperShips - For Super Arcade Mode*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 25; i++)
	{
		destructHelp[i] = JE_readCryptLn(f);      /*Secret DESTRUCT game*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 17; i++)
	{
		weaponNames[i] = JE_readCryptLn(f);       /*Secret DESTRUCT weapons*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < DESTRUCT_MODES; i++)
	{
		destructModeName[i] = JE_readCryptLn(f);  /*Secret DESTRUCT modes*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 13; i++)
	{
		shipInfo[i][0] = JE_readCryptLn(f);       /*NEW: Ship Info*/
		shipInfo[i][1] = JE_readCryptLn(f);       /*NEW: Ship Info*/
	}
	JE_skipCryptLn(f); JE_skipCryptLn(f);

	for (int i = 0; i < 5; i++)
	{
		menuInt[13][i] = JE_readCryptLn(f);       /*Menu 12 - Network Options*/
	}

	f.close();
}
