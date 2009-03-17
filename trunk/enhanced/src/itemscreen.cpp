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
#include "opentyr.h"
#include "itemscreen.h"

#include "tyrian2.h"
#include "varz.h"
#include "config.h"
#include "video.h"
#include "newshape.h"
#include "picload.h"
#include "nortsong.h"
#include "network.h"
#include "fonthand.h"
#include "joystick.h"
#include "nortvars.h"
#include "console/GameActions.h"
#include "console/BindManager.h"
#include "vga256d.h"
#include "loudness.h"
#include "keyboard.h"
#include "params.h"
#include "mainint.h"
#include "Filesystem.h"
#include "helptext.h"
#include "network/Network.h"

#include "boost/format.hpp"
#include "boost/lexical_cast.hpp"
using boost::lexical_cast;

typedef int JE_MenuChoiceType[MAX_MENU];

static const unsigned int LINE_WIDTH = 150;

const JE_MenuChoiceType menuChoicesDefault = { 7, 9, 8, 0, 0, 11, (SAVE_FILES_NUM / 2) + 2, 0, 0, 6, 4, 6, 7, 5 };
const int itemAvailMap[7] = { 1, 2, 3, 9, 4, 6, 7 };
const JE_word generatorX[5] = { 61, 63, 66, 65, 62 };
const JE_word generatorY[5] = { 83, 84, 85, 83, 96 };
const int tyrian2_weapons[42] = {
	 1,  2,  3,  4,  5,  6,  7,  8, 9, 10,
	11, 12, 22,  6, 14,  0, 15, 16, 1, 15,
	10,  9,  3, 16,  1, 14,  1,  9, 9, 12,
	 2,  1,  1,  1,  1,  1,  1,  1, 1,  1,
	 3,  2
};
const int frontWeaponList[42] = {
	 5, 10, 4, 9, 3, 6, 11, 2, 0, 0,
	 0,  0, 8, 9, 0, 0,  1, 0, 5, 1,
	 0,  0, 4, 0, 5, 0,  5, 0, 0, 0,
	10,  1, 1, 1, 1, 1,  1, 1, 1, 1,
	 4, 10
};
const JE_word frontWeaponX[12] = { 58, 65, 65, 53, 60, 50, 57, 50, 60, 51, 52, 57 };
const JE_word frontWeaponY[12] = { 38, 53, 41, 36, 48, 35, 41, 35, 53, 41, 39, 31 };
const int rearWeaponList[40] = {
	0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3,
	4, 0, 5, 6, 0, 0, 7, 0, 0, 2, 1,
	0, 7, 0, 6, 0, 1, 1, 4, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1
};
const JE_word rearWeaponX[7] = { 41, 27,  49,  43, 51, 39, 41 };
const JE_word rearWeaponY[7] = { 92, 92, 113, 102, 97, 96, 76 };
const int mouseSelectionY[MAX_MENU] = { 16, 16, 16, 16, 26, 12, 11, 28, 0, 16, 16, 16, 24, 16 };
const int menuEsc[MAX_MENU] = { 0, 1, 1, 1, 2, 3, 3, 1, 8, 0, 0, 11, 3, 0 };
const JE_word planetX[21] = { 200, 150, 240, 300, 270, 280, 320, 260, 220, 150, 160, 210, 80, 240, 220, 180, 310, 330, 150, 240, 200 };
const JE_word planetY[21] = {  40,  90,  90,  80, 170,  30,  50, 130, 120, 150, 220, 200, 80,  50, 160,  10,  55,  55,  90,  90,  40 };

int songBuy;
// Data used for ItemScreen procedure to indicate items available
int itemAvail[9][10]; /* [1..9, 1..10] */
int itemAvailMax[9]; /* [1..9] */

static JE_MenuChoiceType menuChoices;
static int newPal, curPal, oldPal; // SYN: Originally bytes, I hope this doesn't break anything
static int col, colC;
static int curItemType, curItem, cursor;
static int curSel[MAX_MENU];
static int curMenu;
static JE_word curX, curY, curWindow, selectX, selectY, tempAvail, textYPos;
static bool firstMenu9, paletteChanged;
static bool leftPower, rightPower, rightPowerAfford;
static int planetAni, planetAniWait;
static int currentDotNum, currentDotWait;
static float navX, navY, newNavX, newNavY;
static int tempNavX, tempNavY;
static int planetDots[5]; /* [1..5] */
static int planetDotX[5][10], planetDotY[5][10]; /* [1..5, 1..10] */
static std::string tempStr;
static int lastDirection;
static JE_word upgradeCost;
static JE_word downgradeCost;
static std::string cubeHdr[4];
static std::string cubeText[4][90];
static std::string cubeHdr2[4];
static int faceNum[4];
static JE_word cubeMaxY[4];
static int currentCube;
static JE_word faceX, faceY;
static int currentFaceNum;
static int lastSelect;
static JE_word yLoc;
static int yChg;
static bool quikSave;
static int oldMenu;
static int tempPowerLevel[7];
static int lastCurSel;
static bool keyboardUsed;

void JE_drawMenuChoices();
void JE_updateNavScreen();
void JE_drawMainMenuHelpText();
void JE_menuFunction(int select);
void JE_weaponSimUpdate();
void JE_genItemMenu(int itemNum);
unsigned long JE_cashLeft();
void JE_drawMenuHeader();
void JE_drawDots();
void JE_drawScore();
void JE_drawNavLines(bool dark);
void JE_drawPlanet(int planetNum);
void JE_computeDots();
unsigned long JE_getCost(int itemType, JE_word itemNum);
void JE_drawCube(JE_word x, JE_word y, int filter, int brightness);
void JE_loadCubes();
void JE_drawItem(int itemType, JE_word itemNum, JE_word x, JE_word y);
int JE_partWay(int start, int finish, int dots, int dist);
void JE_doFunkyScreen();
bool JE_quitRequest(bool useMouse);
void JE_initWeaponView();
void JE_weaponViewFrame( int testshotnum );
void JE_funkyScreen();
void JE_drawLines(bool dark);
void JE_scaleInPicture();
void JE_scaleBitmap( Uint8 *bitmap, JE_word x, JE_word y, JE_word x1, JE_word y1, JE_word x2, JE_word y2 );

// TODO Ice Bolt, erm, Refactor This Mofo
void JE_itemScreen()
{
	/* SYN: Okay, here's the menu numbers. All are reindexed by -1 from the original code.
		0: full game menu
		1: upgrade ship main
		2: full game options
		3: play next level
		4: upgrade ship submenus
		5: keyboard settings
		6: load/save menu
		7: data cube menu
		8: read data cube
		9: 2 player arcade game menu
		10: 1 player arcade game menu
		11: different options menu
		12: joystick settings
	*/

	// Load data cubes only in full game mode
	if (!(onePlayerAction || twoPlayerMode))
	{
		JE_loadCubes();
	}

	tempScreenSeg = VGAScreen;

	memcpy(menuChoices, menuChoicesDefault, sizeof(menuChoices));

	JE_playSong(songBuy);

	JE_loadPic(1, false);

	newPal = 0;
	JE_showVGA();

	JE_updateColorsFast(colors);

	col = 1;
	gameLoaded = false;
	curItemType = 1;
	cursor = 1;
	curItem = 0;

	for (JE_word x = 0; x < MAX_MENU; x++)
	{
		curSel[x] = 2;
	}

	curMenu = 0;
	curX = 1;
	curY = 1;
	curWindow = 1;

	/* JE: (* Check for where Pitems and Select match up - if no match then add to the
     itemavail list *) */
	for (JE_word x = 0; x < 7; x++)
	{
		temp = pItemsBack2[pItemButtonMap[x] - 1];
		temp2 = 0;

		for (int y = 0; y < itemAvailMax[itemAvailMap[x]-1]; y++)
		{
			if (itemAvail[itemAvailMap[x]-1][y] == temp)
			{
				temp2 = 1;
			}
		}

		if (temp2 == 0)
		{
			itemAvailMax[itemAvailMap[x]-1]++;
			itemAvail[itemAvailMap[x]-1][itemAvailMax[itemAvailMap[x]-1]-1] = temp;
		}
	}

	memcpy(VGAScreen2, VGAScreen, scr_width * scr_height);

	keyboardUsed = false;
	firstMenu9 = false;
	bool backFromHelp = false;

	do
	{
		JE_getShipInfo();

		quit = false;


		/* JE: If curMenu==1 and twoPlayerMode is on, then force move to menu 10 */
		if (curMenu == 0)
		{
			if (twoPlayerMode)
			{
				curMenu = 9;
			}
			if (netmanager || onePlayerAction)
			{
				curMenu = 10;
			}
			if (superTyrian)
			{
				curMenu = 13;
			}
		}

		paletteChanged = false;

		leftPower = false;
		rightPower = false;

		/* JE: Sort items in merchant inventory */
		for (JE_word x = 0; x < 9; x++)
		{
			if (itemAvailMax[x] > 1)
			{
				for (temp = 0; temp < itemAvailMax[x] - 1; temp++)
				{
					for (temp2 = temp; temp2 < itemAvailMax[x]; temp2++)
					{
						if (itemAvail[x][temp] == 0 || (itemAvail[x][temp] > itemAvail[x][temp2] && itemAvail[x][temp2] != 0))
						{
							temp3 = itemAvail[x][temp];
							itemAvail[x][temp] = itemAvail[x][temp2];
							itemAvail[x][temp2] = temp3;
						}
					}
				}
			}
		}

		/* SYN: note reindexing... "firstMenu9" refers to Menu 8 here :( */
		if (curMenu != 8 || firstMenu9)
		{
			memcpy(VGAScreen, VGAScreen2, scr_width * scr_height);
		}

		/* JE: --- STEP I - Draw the menu --- */

		/* play next level menu */
		if (curMenu == 3)
		{
			planetAni = 0;
			keyboardUsed = false;
			currentDotNum = 0;
			currentDotWait = 8;
			planetAniWait = 3;
			JE_updateNavScreen();
		}

		/* Draw menu title for everything but upgrade ship submenus */
		if (curMenu != 4)
		{
			JE_drawMenuHeader();
		}

		/* Draw menu choices for simple menus */
		if ((curMenu >= 0 && curMenu <= 3) || (curMenu >= 9 && curMenu <= 13))
		{
			JE_drawMenuChoices();
		}

		/* Data cube icons */
		if (curMenu == 0)
		{
			for (JE_word x = 1; x <= cubeMax; x++)
			{
				JE_newDrawCShapeDarkenNum(OPTION_SHAPES, 35, 190 + x*18 + 2, 37+1);
				JE_newDrawCShapeNum(OPTION_SHAPES, 35, 190 + x*18, 37);
			}
		}

		/* Joystick settings menu */
		if (curMenu == 12)
		{
			for (temp = 1; temp <= 4; temp++)
			{
				JE_textShade(214, 34 + temp*24 - 8, joyButtonNames[joyButtonAssign[temp-1]-1], 15, 2, DARKEN);
			}
		}

		/* load/save menu */
		if (curMenu == 6)
		{
			if (twoPlayerMode)
			{
				min = 13;
				max = 24;
			} else {
				min = 2;
				max = 13;
			}

			for (JE_word x = min; x <= max; x++)
			{
				/* Highlight if current selection */
				if (x - min + 2 == curSel[curMenu])
				{
					temp2 = 15;
				} else {
					temp2 = 28;
				}

				/* Write save game slot */
				if (x == max)
				{
					tempStr = miscText[6-1];
				} else {
					if (saveFiles[x-2].level == 0)
					{
						tempStr = miscText[3-1];
					} else {
						tempStr = saveFiles[x-2].name;
					}
				}

				tempY = 38 + (x - min)*11;

				JE_textShade(163, tempY, tempStr, temp2 / 16, temp2 % 16 - 8, DARKEN);

				/* If selected with keyboard, move mouse pointer to match? Or something. */
				if (x - min + 2 == curSel[curMenu])
				{
					if (keyboardUsed)
					{
						JE_setMousePosition(305, 38 + (x - min) * 11);
					}
				}

				if (x < max) /* x == max isn't a save slot */
				{
					/* Highlight if current selection */
					if (x - min + 2 == curSel[curMenu])
					{
						temp2 = 252;
					} else {
						temp2 = 250;
					}

					if (saveFiles[x-2].level == 0)
					{
						// Empty save slot
						tempStr = "-----";
					} else {
						tempStr = saveFiles[x-2].levelName;

						std::ostringstream buf;
						buf << miscTextB[0] << saveFiles[x-2].episode;
						JE_textShade(297, tempY, buf.str().c_str() , temp2 / 16, temp2 % 16 - 8, DARKEN);
					}

					JE_textShade(245, tempY, tempStr, temp2 / 16, temp2 % 16 - 8, DARKEN);
				}

				JE_drawMenuHeader();
			}
		}

		/* keyboard settings menu */
		if (curMenu == 5)
		{
			std::string names[num_keyConfigs];

			for (int i = 0; i < num_keyConfigs; i++) {
				const std::set<Bind*> b = BindManager::get().findBinds(keyConfigs[i].command);
				if (b.empty()) {
					names[i] = "---";
				} else {
					for (std::set<Bind*>::const_iterator j = b.begin(); j != b.end(); ++j) {
						if (j != b.begin()) names[i].append(", ");
						names[i].append((*j)->getKeyDescription());
					}
				}
			}

			for (int i = 0; i < num_keyConfigs+2; i++) {
				int textBright;
				if (i == curSel[curMenu]-2) {
					textBright = 15;
					if (keyboardUsed) {
						JE_setMousePosition(305, 38 + (i)*12);
					}
				} else {
					textBright = 28;
				}

				if (i < num_keyConfigs) {
					JE_textShade(166, 38 + i*12, keyConfigs[i].title, textBright / 16, textBright % 16 - 8, DARKEN);
				} else {
					JE_textShade(166, 38 + i*12, menuInt[5][i-num_keyConfigs+1], textBright / 16, textBright % 16 - 8, DARKEN);
				}

				if (i < num_keyConfigs) {
					if (i == curSel[curMenu]-2) {
						textBright = 252;
					} else {
						textBright = 250;
					}

					JE_textShade(236, 38 + (i)*12, names[i], textBright / 16, textBright % 16 - 8, DARKEN);
				}
			}

			menuChoices[5] = num_keyConfigs + 3;
		}

		/* Upgrade weapon submenus, with weapon sim */
		if (curMenu == 4)
		{
			/* Move cursor until we hit either "Done" or a weapon the player can afford */
			while (curSel[4] < menuChoices[4] && JE_getCost(curSel[1], itemAvail[itemAvailMap[curSel[1]-2]-1][curSel[4]-2]) > score)
			{
				curSel[4] += lastDirection;
				if (curSel[4] < 2)
				{
					curSel[4] = menuChoices[4];
				}
				if (curSel[4] > menuChoices[4])
				{
					curSel[4] = 2;
				}
			}

			if (curSel[4] == menuChoices[4])
			{
				/* If cursor on "Done", use previous weapon */
				pItems[pItemButtonMap[curSel[1]-2]-1] = pItemsBack[pItemButtonMap[curSel[1]-2]-1];
			} else {
				/* Otherwise display the selected weapon */
				pItems[pItemButtonMap[curSel[1]-2]-1] = itemAvail[itemAvailMap[curSel[1]-2]-1][curSel[4]-2];
			}

			/* Get power level info for front and rear weapons */
			if ((curSel[1] == 3 && curSel[4] < menuChoices[4]) || (curSel[1] == 4 && curSel[4] < menuChoices[4]-1))
			{
				if (curSel[1] == 3)
				{
					temp = portPower[0]; /* Front weapon */
				} else {
					temp = portPower[1]; /* Rear weapon */
				}

				/* JE: Only needed if change */
				// PASCAL
				tempW3 = (JE_word)JE_getCost(curSel[1], itemAvail[itemAvailMap[curSel[1]-2]-1][curSel[5]-2]);

				leftPower  = portPower[curSel[1] - 3] > 1; /* Can downgrade */
				rightPower = portPower[curSel[1] - 3] < 11; /* Can upgrade */

				if (rightPower)
				{
					rightPowerAfford = JE_cashLeft() >= upgradeCost; /* Can player afford an upgrade? */
				}
			} else {
				/* Nothing else can be upgraded / downgraded */
				leftPower = false;
				rightPower = false;
			}

			/* Write title for which submenu this is, e.g., "Left Sidekick" */
			JE_dString(74 + JE_fontCenter(menuInt[1][curSel[1]-1], FONT_SHAPES), 10, menuInt[1][curSel[1]-1], FONT_SHAPES);

			temp2 = pItems[pItemButtonMap[curSel[1]-2]-1]; /* get index into pItems for current submenu  */

			/* Iterate through all submenu options */
			for (tempW = 1; tempW < menuChoices[curMenu]; tempW++)
			{
				tempY = 40 + (tempW-1) * 26; /* Calculate y position */

				/* Is this a item or None/DONE? */
				if (tempW < menuChoices[4] - 1)
				{
					/* Get base cost for choice */
					// PASCAL
					tempW3 = (JE_word)JE_getCost(curSel[1], itemAvail[itemAvailMap[curSel[1]-2]-1][tempW-1]);
				} else {
					/* "None" is free :) */
					tempW3 = 0;
				}

				if (tempW3 > score) /* Can player afford current weapon at all */
				{
					temp4 = 4;
				} else {
					temp4 = 0;
				}

				temp = itemAvail[itemAvailMap[curSel[1]-2]-1][tempW-1]; /* Item ID */
				switch (curSel[1]-1)
				{
					case 1: /* ship */
						if (temp > 90)
						{
							std::ostringstream buf;
							buf << "Custom Ship " << temp - 90;
							tempStr = buf.str().substr(0, 30);
						} else {
							tempStr = ships[temp].name;
						}
						break;
					case 2: /* front and rear weapon */
					case 3:
						tempStr = weaponPort[temp].name;
						break;
					case 4: /* shields */
						tempStr = shields[temp].name;
						break;
					case 5: /* generator */
						tempStr = powerSys[temp].name;
						break;
					case 6: /* sidekicks */
					case 7:
						tempStr = options[temp].name;
						break;
				}
				if (tempW == curSel[curMenu]-1)
				{
					if (keyboardUsed)
					{
						JE_setMousePosition(305, tempY+10);
					}
					temp2 = 15;
				} else {
					temp2 = 28;
				}

				JE_getShipInfo();

				/* Owned item bar */
				if (temp == pItemsBack[pItemButtonMap[curSel[1]-2]-1] && temp != 0 && tempW != menuChoices[curMenu]-1)
				{
					JE_bar(160, tempY+7, 300, tempY+11, 227);
					JE_drawShape2(298, tempY+2, 247, shapes6);
				}

				/* Draw DONE */
				if (tempW == menuChoices[curMenu]-1)
				{
					tempStr = miscText[13];
				}
				JE_textShade(185, tempY, tempStr, temp2 / 16, temp2 % 16 -8-temp4, DARKEN);

				/* Draw icon if not DONE. NOTE: None is a normal item with a blank icon. */
				if (tempW < menuChoices[curMenu]-1)
				{
					JE_drawItem(curSel[1]-1, temp, 160, tempY-4);
				}

				/* Make selected text brigther */
				if (tempW == curSel[curMenu]-1)
				{
					temp2 = 15;
				} else {
					temp2 = 28;
				}

				/* Draw Cost: if it's not the DONE option */
				if (tempW != menuChoices[curMenu]-1)
				{
					std::ostringstream buf;
					buf << "Cost: " << tempW3;
					JE_textShade(187, tempY+10, buf.str().c_str(), temp2 / 16, temp2 % 16 -8-temp4, DARKEN);
				}
			}
		} /* /weapon upgrade */

		/* Draw current money and shield/armor bars, when appropriate */
		/* YKS: Ouch */
		if ((curMenu == 0 || curMenu == 1 || curMenu == 6) || ((curMenu == 10 || curMenu == 11) && onePlayerAction) || ((curMenu == 2 || curMenu == 5) && !twoPlayerMode) || (curMenu == 4 && (curSel[1] >= 1 && curSel[1] <= 6)))
		{
			if (curMenu != 4)
			{
				std::ostringstream buf;
				buf << score;
				JE_textShade(65, 173, buf.str().c_str(), 1, 6, DARKEN);
			}
			JE_barDrawShadow(42, 152, 3, 14, armorLevel, 2, 13);
			JE_barDrawShadow(104, 152, 2, 14, shields[pItems[PITEM_SHIELD]].mpwr * 2, 2, 13);
		}

		/* Draw crap on the left side of the screen, i.e. two player scores, ship graphic, etc. */
		if (((curMenu >= 0 && curMenu <= 2) || curMenu == 5 || curMenu == 6 || (curMenu >= 9 && curMenu <= 11) || curMenu == 13) || (curMenu == 4 && (curSel[1] == 2 || curSel[1] == 5)))
		{
			if (twoPlayerMode)
			{
				{
					std::ostringstream buf;
					buf << miscText[40] << " " << score;
					JE_textShade(25, 50, buf.str().c_str(), 15, 0, FULL_SHADE);
				}

				{
					std::ostringstream buf;
					buf << miscText[41] << " " << score2;
					JE_textShade(25, 60, buf.str().c_str(), 15, 0, FULL_SHADE);
				}
			} else if (superArcadeMode > 0 || superTyrian) {
				helpBoxColor = 15;
				helpBoxBrightness = 4;
				if (!superTyrian)
				{
					JE_helpBox(35, 25, superShips[superArcadeMode], 18);
				} else {
					JE_helpBox(35, 25, superShips[SA+3], 18);
				}
				helpBoxBrightness = 1;

				JE_textShade(25, 50, superShips[SA+1], 15, 0, FULL_SHADE);
				JE_helpBox(25, 60, weaponPort[pItems[PITEM_FRONT_WEAPON]].name, 22);
				JE_textShade(25, 120, superShips[SA+2], 15, 0, FULL_SHADE);
				JE_helpBox(25, 130, special[pItems[PITEM_SPECIAL]].name, 22);
			} else {
				if (pItems[PITEM_SHIP] > 90)
				{
					temp = 32;
				} else if (pItems[PITEM_SHIP] > 0) {
					temp = ships[pItems[PITEM_SHIP]].bigshipgraphic;
				} else {
					temp = ships[pItemsBack[PITEM_SHIP]].bigshipgraphic;
				}

				switch (temp)
				{
					case 32:
						tempW = 35;
						tempW2 = 33;
						break;
					case 28:
						tempW = 31;
						tempW2 = 36;
						break;
					case 33:
						tempW = 31;
						tempW2 = 35;
						break;
				}

				JE_newDrawCShapeNum(OPTION_SHAPES, temp, tempW, tempW2);

				temp = pItems[PITEM_GENERATOR];

				if (temp > 1)
				{
					temp--;
				}

				JE_newDrawCShapeNum(WEAPON_SHAPES, temp + 16, generatorX[temp-1]+1, generatorY[temp-1]+1);

				if (pItems[PITEM_FRONT_WEAPON] > 0)
				{
					temp = tyrian2_weapons[pItems[PITEM_FRONT_WEAPON] - 1];
					temp2 = frontWeaponList[pItems[PITEM_FRONT_WEAPON] - 1] - 1;
					JE_newDrawCShapeNum(WEAPON_SHAPES, temp, frontWeaponX[temp2]+1, frontWeaponY[temp2]);
				}
				if (pItems[PITEM_REAR_WEAPON] > 0)
				{
					temp = tyrian2_weapons[pItems[PITEM_REAR_WEAPON] - 1];
					temp2 = rearWeaponList[pItems[PITEM_REAR_WEAPON] - 1] - 1;
					JE_newDrawCShapeNum(WEAPON_SHAPES, temp, rearWeaponX[temp2], rearWeaponY[temp2]);
				}

				JE_drawItem(6, pItems[PITEM_LEFT_SIDEKICK], 3, 84);
				JE_drawItem(7, pItems[PITEM_RIGHT_SIDEKICK], 129, 84);
				JE_newDrawCShapeAdjustNum(OPTION_SHAPES, 27, 28, 23, 15, shields[pItems[PITEM_SHIELD]].mpwr - 10);
			}
		}

		// Draw volume bars
		if ((curMenu == 2) || (curMenu == 11))
		{
			JE_barDrawShadow(225, 70, 1, 16, int(CVars::snd_music_vol*14.f), 3, 13); // TODO: Implement a barDraw that can draw partial bars
			JE_barDrawShadow(225, 86, 1, 16, int(CVars::snd_fx_vol*14.f), 3, 13);
		}

		/* 7 is data cubes menu, 8 is reading a data cube, "firstmenu9" refers to menu 8 because of reindexing */
		if ( (curMenu == 7) || ( (curMenu == 8) && firstMenu9) )
		{
			firstMenu9 = false;
			menuChoices[7] = cubeMax + 2;
			JE_bar(1, 1, 145, 170, 0);

			JE_newDrawCShapeNum(OPTION_SHAPES, 21, 1, 1); /* Portrait area background */

			if (curMenu == 7)
			{
				if (cubeMax == 0)
				{
					JE_helpBox(166, 80, miscText[16 - 1], 30);
					tempW = 160;
					temp2 = 252;
				} else {
					for (JE_word x = 1; x <= cubeMax; x++)
					{
						JE_drawCube(166, 38 + (x - 1) * 28, 13, 0);
						if (x + 1 == curSel[curMenu])
						{
							if (keyboardUsed)
							{
								JE_setMousePosition(305, 38 + (x - 1) * 28 + 6);
							}
							temp2 = 252;
						} else {
							temp2 = 250;
						}

						helpBoxColor = temp2 / 16;
						helpBoxBrightness = (temp2 % 16) - 8;
						helpBoxShadeType = DARKEN;
						JE_helpBox(192, 44 + (x - 1) * 28, cubeHdr[x-1].c_str(), 24);
					}
					JE_word x = cubeMax + 1;
					if (x + 1 == curSel[curMenu])
					{
						if (keyboardUsed)
						{
							JE_setMousePosition(305, 38 + (x - 1) * 28 + 6);
						}
						temp2 = 252;
					} else {
						temp2 = 250;
					}
					tempW = 44 + (x - 1) * 28;
				}
			}

			JE_textShade(172, tempW, miscText[6 - 1], temp2 / 16, (temp2 % 16) - 8, DARKEN);

			currentFaceNum = 0;
			if (curSel[7] < menuChoices[7])
			{
				/* SYN: Be careful reindexing some things here, because faceNum 0 is blank, but
				   faceNum 1 is at index 0 in several places! */
				currentFaceNum = faceNum[curSel[7] - 2];

				if (lastSelect != curSel[7] && currentFaceNum > 0)
				{
					static const int facepal[12] = { 1, 2, 3, 4, 6, 9, 11, 12, 16, 13, 14, 15};

					faceX = 77 - (shapeX[FACE_SHAPES][currentFaceNum - 1] >> 1);
					faceY = 92 - (shapeY[FACE_SHAPES][currentFaceNum - 1] >> 1);

					paletteChanged = true;
					temp2 = facepal[currentFaceNum - 1];
					newPal = 0;

					for (temp = 1; temp <= 255 - (3 * 16); temp++)
					{
						colors[temp].r = palettes[temp2][temp].r;
						colors[temp].g = palettes[temp2][temp].g;
						colors[temp].b = palettes[temp2][temp].b;
					}
				}
			}

			if (currentFaceNum > 0)
			{
				JE_newDrawCShapeNum(FACE_SHAPES, currentFaceNum, faceX, faceY);
			}

			lastSelect = curSel[7];
		}

		/* 2 player input devices */
		if (curMenu == 9)
		{
			JE_dString(186, 38 + 2 * 16, inputDevices[inputDevice1-1], SMALL_FONT_SHAPES, curSel[9] == 3);
			JE_dString(186, 38 + 4 * 16, inputDevices[inputDevice2-1], SMALL_FONT_SHAPES, curSel[9] == 4);
		}


		/* JE: { - Step VI - Help text for current cursor location } */

		flash = false;

		/* scanCode = 0; */
		/* k = 0; */

		/* JE: {Reset player weapons} */
		memset(shotMultiPos, 0, sizeof(shotMultiPos));

		JE_drawScore();

		JE_drawMainMenuHelpText();

		if (newPal > 0) /* can't reindex this :( */
		{
			curPal = newPal;
			load_palette(newPal-1, true);
			newPal = 0;
		}

		/* datacube title */
		if ( ( (curMenu == 7) || (curMenu == 8) ) && (curSel[7] < menuChoices[7]) )
		{
			JE_textShade (75 - JE_textWidth(cubeHdr2[curSel[7]-2].c_str(), TINY_FONT) / 2, 173,
				cubeHdr2[curSel[7]-2].c_str(), 14, 3, DARKEN);
		}

		/* SYN: Everything above was just drawing the screen. In the rest of it, we process
		   any user input (and do a few other things) */

		/* SYN: Let's start by getting fresh events from SDL */
		service_SDL_events(true);

		if (CVars::ch_constant_play)
		{
			mainLevel = mapSection[mapPNum-1];
			jumpSection = true;
		} else {
			do
			{
				/*
				Inner loop -- this handles animations on menus that need them and handles
				some keyboard events. Events it can't handle end the loop and fall through
				to the main keyboard handler below.

				Also, I think all timing is handled in here. Somehow.
				*/

				mouseCursor = 0;

				col += colC;
				if (col < -2 || col > 6)
				{
					colC = (-1 * colC);
				}

				if (curMenu == 8)
				{
					if (mouseX > 164 && mouseX < 299 && mouseY > 47 && mouseY < 153)
					{
						if (mouseY > 100)
						{
							mouseCursor = 2;
						} else {
							mouseCursor = 1;
						}
					}

					JE_bar(160, 49, 310, 158, 228);
					if (yLoc + yChg < 0)
					{
						yChg = 0;
						yLoc = 0;
					}

					yLoc += yChg;
					temp = yLoc / 12;
					temp2 = yLoc % 12;
					tempW = 38 + 12 - temp2;
					temp3 = cubeMaxY[curSel[7] - 2];

					for (JE_word x = temp + 1; x <= temp + 10; x++)
					{
						if (x <= temp3)
						{
							JE_outTextAndDarken(161, tempW, cubeText[curSel[7]-2][x-1].c_str(), 14, 3, TINY_FONT);
							tempW += 12;
						}
					}

					JE_bar(160, 39, 310, 48, 228);
					JE_bar(160, 157, 310, 166, 228);

					std::ostringstream buf;
					int percent = cubeMaxY[currentCube] > 9 ? (yLoc * 100) / ((cubeMaxY[currentCube] - 9) * 12) : 100;
					buf << miscText[11] << " " << percent << "%";
					JE_outTextAndDarken(176, 160, buf.str().c_str(), 14, 1, TINY_FONT);

					JE_dString(260, 160, miscText[13 - 1], SMALL_FONT_SHAPES);

					if (temp2 == 0)
					{
						yChg = 0;
					}

					JE_mouseStart();

					JE_showVGA();

					if (backFromHelp)
					{
						JE_fadeColor(10);
						backFromHelp = false;
					}
					JE_mouseReplace();

					setjasondelay(1);
					wait_delay();
				} else {
					/* current menu is not 8 (read data cube) */

					if (curMenu == 3)
					{
						JE_updateNavScreen();
						JE_drawMainMenuHelpText();
						JE_drawMenuHeader();
						JE_drawMenuChoices();
						if (extraGame)
						{
							JE_dString(170, 140, miscText[68 - 1], FONT_SHAPES);
						}
					}

					if (curMenu == 12 && curSel[curMenu] == 6 && joystick_installed)
					{
						//JE_drawJoystick();
					}

					if (curMenu == 7 && curSel[7] < menuChoices[7])
					{
						/* Draw flashy cube */
						JE_newDrawCShapeModifyNum(OPTION_SHAPES, 26, 166, 38 + (curSel[7] - 2) * 28, 13, col);
					}

					/* IF (curmenu = 5) AND (cursel [2] IN [3, 4, 6, 7, 8]) */
					if (curMenu == 4 && ( curSel[1] == 3 || curSel[1] == 4 || ( curSel[1] >= 6 && curSel[1] <= 8) ) )
					{
						setjasondelay(3);
						JE_weaponSimUpdate();
						JE_drawScore();
						service_SDL_events(false);

						if (newPal > 0)
						{
							curPal = newPal;
							JE_zPal(newPal);
							newPal = 0;
						}

						JE_mouseStart();

						if (paletteChanged)
						{
							JE_updateColorsFast(colors);
							paletteChanged = false;
						}

						JE_showVGA(); /* SYN: This is where it updates the screen for the weapon sim */

						if (backFromHelp)
						{
							JE_fadeColor(10);
							backFromHelp = false;
						}

						JE_mouseReplace();
						
						int delaycount_temp;
						if ((delaycount_temp = target - SDL_GetTicks()) > 0)
							SDL_Delay(delaycount_temp);
					} else { /* current menu is anything but weapon sim or datacube */

						if (curMenu == 8) /* SYN: menu 8 is a datacube... this should never happen?! */
						{
							setjasondelay(0);
						} else {
							setjasondelay(2);
						}

						JE_drawScore();

						if (newPal > 0)
						{
							curPal = newPal;
							JE_zPal(newPal);
							newPal = 0;
						}

						JE_mouseStart();

						if (paletteChanged)
						{
							JE_updateColorsFast(colors);
							paletteChanged = false;
						}

						JE_showVGA(); /* SYN: This is the where the screen updates for most menus */

						JE_mouseReplace();

						if (backFromHelp)
						{
							JE_fadeColor(10);
							backFromHelp = false;
						}
						
						int delaycount_temp;
						if ((delaycount_temp = target - SDL_GetTicks()) > 0)
							SDL_Delay(delaycount_temp);
					}
				}
				
				 do {
					service_SDL_events(false);
					mouseButton = JE_mousePosition(&mouseX, &mouseY); 
					inputDetected = ((mouseButton > 0) || newkey);
					
					if (JE_joystickTranslate())
					{
							if (lastkey_sym == SDLK_RETURN || lastkey_sym == SDLK_ESCAPE)
							{
								if (buttonHeld)
								{
									lastkey_sym = SDLK_UNKNOWN;
									inputDetected = false;
								} else {
									buttonHeld = true;
									inputDetected = true;
								}
							} else {
								buttonHeld = false;
								inputDetected = true;
							}
					} else {
						buttonHeld = false;
					}
				} while (!inputDetected && delaycount() != 0);
				
				if (curMenu != 6)
				{
					if (keysactive[SDLK_s] && (keysactive[SDLK_LALT] || keysactive[SDLK_RALT]) )
					{
						if (curMenu == 8 || curMenu == 7)
						{
							curMenu = 0;
						}
						quikSave = true;
						oldMenu = curMenu;
						curMenu = 6;
						performSave = true;
						newPal = 1;
						oldPal = curPal;
					}
					if (keysactive[SDLK_l] && (keysactive[SDLK_LALT] || keysactive[SDLK_RALT]) )
					{
						if (curMenu == 8 || curMenu == 7)
						{
							curMenu = 0;
						}
						quikSave = true;
						oldMenu = curMenu;
						curMenu = 6;
						performSave = false;
						newPal = 1;
						oldPal = curPal;
					}
				}

				if (curMenu == 8)
				{
					if (mouseButton > 0 && mouseCursor >= 1)
					{
						inputDetected = false;
						if (mouseCursor == 1)
						{
							yChg = -1;
						} else {
							yChg = 1;
						}
					}

					if (keysactive[SDLK_PAGEUP])
					{
						yChg = -2;
						inputDetected = false;
					}
					if (keysactive[SDLK_PAGEDOWN])
					{
						yChg = 2;
						inputDetected = false;
					}

					if (keysactive[SDLK_UP] || joystickUp)
					{
						yChg = -1;
						inputDetected = false;
					}

					if (keysactive[SDLK_DOWN] || joystickDown)
					{
						yChg = 1;
						inputDetected = false;
					}

					if (yChg < 0 && yLoc == 0)
					{
						yChg = 0;
					}
					if (yChg  > 0 && (yLoc / 12) > cubeMaxY[currentCube] - 10)
					{
						yChg = 0;
					}
				}

			} while (!inputDetected);
		}

		keyboardUsed = false;

		/* The rest of this just grabs input events, handles them, then proceeds on. */

		if (mouseButton > 0)
		{
			lastDirection = 1;

			mouseButton = JE_mousePosition(&mouseX, &mouseY);

			if (curMenu == 7 && cubeMax == 0)
			{
				curMenu = 0;
				JE_playSampleNum(ESC);
				newPal = 1;
			}

			if (curMenu == 8)
			{
				if ((mouseX > 258) && (mouseX < 290) && (mouseY > 159) && (mouseY < 171))
				{
					curMenu = 7;
					JE_playSampleNum(ESC);
				}
			}

			tempB = true;

			if (curMenu == 2 || curMenu == 11)
			{
				if ((mouseX >= 221) && (mouseX <= 303) && (mouseY >= 70) && (mouseY <= 82))
				{
					if (CVars::snd_enabled)
					{
						temp = currentSong;
						currentSong = 0;
						JE_playSong(temp);
					}

					curSel[2] = 4;

					const float tmp_vol = (mouseX - 221) / 14.f;

					if (tmp_vol < 0.f) {
						CVars::snd_music_vol = CVars::snd_music_vol-.05f;
					} else if (tmp_vol > 1.5f) {
						CVars::snd_music_vol = CVars::snd_music_vol+.05f;
					} else {
						CVars::snd_music_vol = tmp_vol;
					}
					tempB = false;
				}

				if ((mouseX >= 221) && (mouseX <= 303) && (mouseY >= 86) && (mouseY <= 98))
				{
					curSel[2] = 5;
					const float tmp_vol = (mouseX - 221) / 14.f;

					if (tmp_vol < 0.f) {
						CVars::snd_fx_vol = CVars::snd_fx_vol-.05f;
					} else if (tmp_vol > 1.5f) {
						CVars::snd_fx_vol = CVars::snd_fx_vol+.05f;
					} else {
						CVars::snd_fx_vol = tmp_vol;
					}
				}

				JE_playSampleNum(CURSOR_MOVE);
			}

			if (tempB && (mouseY > 20) && (mouseX > 170) && (mouseX < 308) && (curMenu != 8))
			{
				tempI = (mouseY - 38) / mouseSelectionY[curMenu]+2;

				if (curMenu == 9)
				{
					if (tempI > 5)
					{
						tempI--;
					}
					if (tempI > 3)
					{
						tempI--;
					}
				}

				if (curMenu == 0)
				{
					if (tempI > 7)
					{
						tempI = 7;
					}
				}

				if (curMenu == 3)
				{
					if (tempI == menuChoices[curMenu]+1)
					{
						tempI = menuChoices[curMenu];
					}
				}

				if (tempI <= menuChoices[curMenu])
				{
					if ((curMenu == 4) && (tempI == menuChoices[4]))
					{
						score = JE_cashLeft();
						curMenu = 1;
						JE_playSampleNum(ITEM);
					} else {
						JE_playSampleNum(CLICK);
						if (curSel[curMenu] == tempI)
						{
							JE_menuFunction(curSel[curMenu]);
						} else {
							if ((curMenu == 4) && ((curSel[1] == 3) || (curSel[1] == 4)))
							{
								tempPowerLevel[curSel[4]-2] = portPower[curSel[1]-3];
							}
							if ((curMenu == 5) && (JE_getCost(curSel[1], itemAvail[itemAvailMap[curSel[2]-1]][tempI-1]) > score))
							{
								JE_playSampleNum(WRONG);
							} else {
								if (curSel[1] == 4)
								{
									portConfig[1] = 1;
								}
								curSel[curMenu] = tempI;
							}

							if ((curMenu == 4) && ((curSel[1] == 3) || (curSel[1] == 4)))
							{
								portPower[curSel[1]-3] = tempPowerLevel[curSel[4]-2];
							}
						}
					}
				}

				/*do {
					mouseButton = JE_mousePosition(&tempX, &tempY);
				} while (!(mouseButton == 0));*/
				wait_noinput(false,true,false);
			}

			if ((curMenu == 4) && ((curSel[1] == 3) || (curSel[1] == 4)))
			{
				if ((mouseX >= 23) && (mouseX <= 36) && (mouseY >= 149) && (mouseY <= 168))
				{
					JE_playSampleNum(CURSOR_MOVE);
					switch (curSel[1])
					{
						case 3:
							if (leftPower)
							{
								portPower[0]--;
							} else {
								JE_playSampleNum(WRONG);
							}
							break;
						case 4:
							if (leftPower)
							{
								portPower[1]--;
							} else {
								JE_playSampleNum(WRONG);
							}
							break;
					}
					wait_noinput(false,true,false);
				}

				if ((mouseX >= 119) && (mouseX <= 131) && (mouseY >= 149) && (mouseY <= 168))
				{
					JE_playSampleNum(CURSOR_MOVE);
					switch (curSel[1])
					{
						case 3:
							if (rightPower && rightPowerAfford)
							{
								portPower[0]++;
							} else {
								JE_playSampleNum(WRONG);
							}
							break;
						case 4:
							if (rightPower && rightPowerAfford)
							{
								portPower[1]++;
							} else {
								JE_playSampleNum(WRONG);
							}
							break;
					}
					wait_noinput(false,true,false);
				}
			}
		} else {
			if (newkey)
			{
				switch (lastkey_sym)
				{
				case SDLK_SPACE:
					if ( (curMenu == 4) && (curSel[1] == 4))
					{
						portConfig[1]++;
						if (portConfig[1] > weaponPort[pItems[PITEM_FRONT_WEAPON]].opnum)
						{
							portConfig[1] = 1;
						}
					}
					/* SYN: Intentional fall-through! */
				case SDLK_RETURN:
					JE_menuFunction(curSel[curMenu]);
					keyboardUsed = true;
					break;
				case SDLK_ESCAPE:
					JE_playSampleNum(ESC);
					if ( (curMenu == 6) && quikSave)
					{
						curMenu = oldMenu;
						newPal = oldPal;
					}
					else if (menuEsc[curMenu] == 0)
					{
						if (JE_quitRequest(true))
						{
							gameLoaded = true;
							mainLevel = 0;
						}
					} else {
						if (curMenu == 4)
						{
							memcpy(pItems, pItemsBack, sizeof(pItems));
							memcpy(portPower, lastPortPower, sizeof(portPower));
							curSel[4] = lastCurSel;
							score = JE_cashLeft();
						}
						if (curMenu != 8)
						{
							newPal = 1;
						}
						curMenu = menuEsc[curMenu] - 1;

					}
					keyboardUsed = true;
					break;

				case SDLK_F1:
					if (!netmanager)
					{
						JE_helpSystem(2);
						JE_fadeBlack(10);

						JE_playSong(songBuy);

						JE_loadPic(1, false);
						newPal = 1;

						switch (curMenu)
						{
						case 3:
							newPal = 18;
							break;
						case 7:
						case 8:
							lastSelect = 0;
							break;
						}

						memcpy(VGAScreen2, VGAScreen, scr_width * scr_height);

						curPal = newPal;
						load_palette(newPal-1, false);
						JE_showVGA();
						newPal = 0;
						backFromHelp = true;
					}
					break;

				case SDLK_UP:
					keyboardUsed = true;
					lastDirection = -1;
					if (curMenu != 8)
					{
						JE_playSampleNum(CURSOR_MOVE);
					}
					if ( (curMenu == 4) && (curSel[1] == 3 || curSel[1] == 4) )
					{
						tempPowerLevel[curSel[4] - 2] = portPower[curSel[1] - 3];
						if (curSel[1] == 4)
						{
							// If Rear Weapon, reset firing pattern to default
							portConfig[1] = 1;
						}
					}
					curSel[curMenu]--;
					if (curSel[curMenu] < 2)
					{
						curSel[curMenu] = menuChoices[curMenu];
					}
					if ( (curMenu == 4) && (curSel[1] == 3 || curSel[1] == 4) )
					{
						portPower[curSel[1] - 3] = tempPowerLevel[curSel[4] - 2];
					}
					break;

				case SDLK_DOWN:
					keyboardUsed = true;
					lastDirection = 1;
					if (curMenu != 8)
					{
						JE_playSampleNum(CURSOR_MOVE);
					}
					if ( (curMenu == 4) && (curSel[1] == 3 || curSel[1] == 4) )
					{
						tempPowerLevel[curSel[4] - 2] = portPower[curSel[1] - 3];
						if (curSel[1] == 4)
						{
							// If Rear Weapon, reset firing pattern to default
							portConfig[1] = 1;
						}
					}
					curSel[curMenu]++;
					if (curSel[curMenu] > menuChoices[curMenu])
					{
						curSel[curMenu] = 2;
					}
					if ( (curMenu == 4) && (curSel[1] == 3 || curSel[1] == 4) )
					{
						portPower[curSel[1] - 3] = tempPowerLevel[curSel[4] - 2];
					}
					break;

				case SDLK_HOME:
					if (curMenu == 8)
					{
						yLoc = 0;
					}
					break;

				case SDLK_END:
					if (curMenu == 8)
					{
						yLoc = (cubeMaxY[currentCube] - 9) * 12;
					}
					break;

				case SDLK_x: /* alt-X */
					if (lastkey_mod == KMOD_RALT || lastkey_mod == KMOD_LALT)
					{
						JE_tyrianHalt(0);
					}
					break;

				case SDLK_LEFT:
					if (curMenu == 12 && curSel[curMenu] < 6)
					{
						joyButtonAssign[curSel[curMenu] - 2]--;
						if (joyButtonAssign[curSel[curMenu] - 2] < 1)
						{
							joyButtonAssign[curSel[curMenu] - 2] = 5;
						}
					}

					if (curMenu == 9)
					{
						switch (curSel[curMenu])
						{
						case 3:
							JE_playSampleNum(CURSOR_MOVE);
							do {
								inputDevice1--;
								if (inputDevice1 < 1)
								{
									inputDevice1 = 3;
								}
							} while (inputDevice1 == inputDevice2);
							break;
						case 4:
							JE_playSampleNum(CURSOR_MOVE);
							do {
								inputDevice2--;
								if (inputDevice2 < 1)
								{
									inputDevice2 = 3;
								}
							} while (inputDevice1 == inputDevice2);
							break;
						}
					}

					if (curMenu == 2 || curMenu == 4  || curMenu == 11)
					{
						JE_playSampleNum(CURSOR_MOVE);
					}

					switch (curMenu)
					{
					case 2:
					case 11:
						switch (curSel[curMenu])
						{
						case 4:
							if (CVars::snd_music_vol == 0) {
								JE_playSampleNum(WRONG);
							} else {
								CVars::snd_music_vol = CVars::snd_music_vol - .05f;
							}
							break;
						case 5:
							if (CVars::snd_fx_vol == 0) {
								JE_playSampleNum(WRONG);
							} else {
								CVars::snd_fx_vol = CVars::snd_fx_vol - .05f;
							}
							break;
						}
						break;
					case 4:
						switch (curSel[1])
						{
						case 3:
							if (leftPower)
							{
								portPower[0]--;
							} else {
								JE_playSampleNum(WRONG);
							}
							break;
						case 4:
							if (leftPower)
							{
								portPower[1]--;
							} else {
								JE_playSampleNum(WRONG);
							}
							break;
						}
						break;
					}
					break;
				case SDLK_RIGHT:
					if (curMenu == 12 && curSel[curMenu] < 6)
					{
						joyButtonAssign[curSel[curMenu] - 2]++;
						if (joyButtonAssign[curSel[curMenu] - 2] > 5)
						{
							joyButtonAssign[curSel[curMenu] - 2] = 1;
						}
					}

					if (curMenu == 9)
					{
						switch (curSel[curMenu])
						{
						case 3:
							JE_playSampleNum(CURSOR_MOVE);
							do {
								inputDevice1++;
								if (inputDevice1 > 3)
								{
									inputDevice1 = 1;
								}
							} while (inputDevice1 == inputDevice2);
							break;
						case 4:
							JE_playSampleNum(CURSOR_MOVE);
							do {
								inputDevice2++;
								if (inputDevice2 > 3)
								{
									inputDevice2 = 1;
								}
							} while (inputDevice1 == inputDevice2);
							break;
						}
					}

					if (curMenu == 2 || curMenu == 4  || curMenu == 11)
					{
						JE_playSampleNum(CURSOR_MOVE);
					}

					switch (curMenu)
					{
					case 2:
					case 11:
						switch (curSel[curMenu])
						{
						case 4:
							if (CVars::snd_music_vol == 1.5f) {
								JE_playSampleNum(WRONG);
							} else {
								CVars::snd_music_vol = CVars::snd_music_vol + .05f;
							}
							break;
						case 5:
							if (CVars::snd_fx_vol == 1.5f) {
								JE_playSampleNum(WRONG);
							} else {
								CVars::snd_fx_vol = CVars::snd_fx_vol + .05f;
							}
							break;
						}
						break;
					case 4:
						switch (curSel[1])
						{
						case 3:
							if (rightPower && rightPowerAfford)
							{
								portPower[0]++;
							} else {
								JE_playSampleNum(WRONG);
							}
							break;
						case 4:
							if (rightPower && rightPowerAfford)
							{
								portPower[1]++;
							} else {
								JE_playSampleNum(WRONG);
							}
							break;
						}
						break;
					}
					break;

				default:
					break;
				}
			}
		}
		
	} while (!(quit || gameLoaded || jumpSection));

	if (netmanager)
	{
		/* TODO NETWORK
		if (!quit)
		{
			JE_barShade(3, 3, 316, 196);
			JE_barShade(1, 1, 318, 198);
			JE_dString(10, 160, "Waiting for other player.", SMALL_FONT_SHAPES);

			network::prepare(network::PACKET_WAITING);
			network::send(4);
			
			for (;;)
			{
				service_SDL_events(false);
				JE_showVGA();

				if (network::packet_in[0] && SDLNet_Read16(network::packet_in[0]->data+0) == network::PACKET_WAITING)
				{
					network::update();
					break;
				}
				network::update();
				network::check();

				SDL_Delay(16);
			}

			while (!network::is_sync())
			{
				service_SDL_events(false);
				JE_showVGA();

				network::check();
				SDL_Delay(16);
			}

			network::state_reset();
		}

		while (!network::is_sync())
		{
			service_SDL_events(false);
			JE_showVGA();

			network::check();
			SDL_Delay(16);
		}*/
	}
	
	if (gameLoaded)
	{
		JE_fadeBlack(10);
	}

}


void JE_drawMenuChoices()
{
	int x;

	for (x = 2; x <= menuChoices[curMenu]; x++)
	{
		if (curMenu == 12)
		{
			tempY = 38 + (x-1) * 24 - 8;
		} else {
			tempY = 38 + (x-1) * 16;
		}

		if (curMenu == 0)
		{
			if (x == 7)
			{
				tempY += 16;
			}
		}

		if (curMenu == 9)
		{
			if (x > 3)
			{
				tempY += 16;
			}
			if (x > 4)
			{
				tempY += 16;
			}
		}

		if (!(curMenu == 3 && x == menuChoices[curMenu]))
		{
			tempY -= 16;
		}

		std::ostringstream buf;
		if (curSel[curMenu] == x)
		{
			buf << "~";
		}
		buf << menuInt[curMenu][x-1];
		JE_dString(166, tempY, buf.str().c_str(), SMALL_FONT_SHAPES);

		if (keyboardUsed && curSel[curMenu] == x)
		{
			JE_setMousePosition(305, tempY + 6);
		}
	}
}

void JE_updateNavScreen()
{
	int x;

	/* minor issues: */
	/* TODO: The scroll to the new planet is too fast, I think */
	/* TODO: The starting coordinates for the scrolling effect may be wrong, the
	   yellowish planet below Tyrian isn't visible for as many frames as in the
	   original. */

	tempNavX = ot_round(navX);
	tempNavY = ot_round(navY);
	JE_bar(19, 16, 135, 169, 2);
	JE_drawNavLines(true);
	JE_drawNavLines(false);
	JE_drawDots();

	for (x = 0; x < 11; x++)
	{
		JE_drawPlanet(x);
	}

	for (x = 0; x < menuChoices[3]-1; x++)
	{
		if (mapPlanet[x] > 11)
		{
			JE_drawPlanet(mapPlanet[x] - 1);
		}
	}

	if (mapOrigin > 11)
	{
		JE_drawPlanet(mapOrigin - 1);
	}

	JE_newDrawCShapeNum(OPTION_SHAPES, 29, 0, 0);

	if (curSel[3] < menuChoices[3])
	{
		newNavX = (planetX[mapOrigin-1] - shapeX[PLANET_SHAPES][PGR[mapOrigin-1]-1] / 2
		          + planetX[mapPlanet[curSel[3]-2] - 1]
		          - shapeX[PLANET_SHAPES][PGR[mapPlanet[curSel[3]-2] - 1]-1] / 2) / 2.f;
		newNavY = (planetY[mapOrigin-1] - shapeY[PLANET_SHAPES][PGR[mapOrigin-1]-1] / 2
		          + planetY[mapPlanet[curSel[3]-2] - 1]
		          - shapeY[PLANET_SHAPES][PGR[mapPlanet[curSel[3]-2] - 1]-1] / 2) / 2.f;
	}

	navX = navX + (newNavX - navX) / 2.f;
	navY = navY + (newNavY - navY) / 2.f;

	if (ot_abs(newNavX - navX) < 1.f)
	{
		navX = newNavX;
	}
	if (ot_abs(newNavY - navY) < 1.f)
	{
		navY = newNavY;
	}

	JE_bar(314, 0, 319, 199, 230);

	if (planetAniWait > 0)
	{
		planetAniWait--;
	} else {
		planetAni++;
		if (planetAni > 14)
		{
			planetAni = 0;
		}
		planetAniWait = 3;
	}

	if (currentDotWait > 0)
	{
		currentDotWait--;
	} else {
		if (currentDotNum < planetDots[curSel[3]-2])
		{
			currentDotNum++;
		}
		currentDotWait = 5;
	}
}

void JE_drawMainMenuHelpText()
{
	std::string tempStr;
	int temp;

	temp = curSel[curMenu] - 2;
	if (curMenu < 3 || curMenu == 9 || curMenu > 10)
	{
		tempStr = mainMenuHelp[(menuHelp[curMenu][temp])-1];
	} else {
		if (curMenu == 5 && curSel[5] == 10)
		{
			tempStr = mainMenuHelp[25-1];
		}
		else if (leftPower || rightPower)
		{
			tempStr = mainMenuHelp[24-1];
		}
		else if ( (temp == menuChoices[curMenu] - 1) || ( (curMenu == 7) && (cubeMax == 0) ) )
		{
			tempStr = mainMenuHelp[12-1];
		}
		else
		{
			tempStr = mainMenuHelp[17 + curMenu - 3];
		}
	}
	JE_textShade(10, 187, tempStr, 14, 1, DARKEN);
}

void JE_menuFunction(int select)
{
	int x;
	unsigned long tempScore;
	JE_word curSelect;

	col = 0;
	colC = -1;
	JE_playSampleNum(CLICK);

	curSelect = curSel[curMenu];

	switch (curMenu)
	{
	case 0:
		switch (select)
		{
		case 2:
			curMenu = 7;
			lastSelect = 0;
			curSel[7] = 2;
			break;
		case 3:
			JE_doFunkyScreen();
			break;
		case 4:
			curMenu = 1;
			break;
		case 5:
			curMenu = 2;
			break;
		case 6:
			curMenu = 3;
			newPal = 18;
			JE_computeDots();
			navX = planetX[mapOrigin - 1];
			navY = planetY[mapOrigin - 1];
			newNavX = navX;
			newNavY = navY;
			menuChoices[3] = mapPNum + 2;
			curSel[3] = 2;
			menuInt[3][0] = "Next Level";
			for (x = 0; x < mapPNum; x++)
			{
				menuInt[3][x + 1] = pName[mapPlanet[x]-1];
			}
			menuInt[3][x+1] = miscText[6-1];
			break;
		case 7:
			if (JE_quitRequest(true))
			{
				gameLoaded = true;
				mainLevel = 0;
			}
			break;
		}
		break;

	case 1:
		if (select == 9)
		{
			curMenu = 0;
		} else {
			lastDirection = 1;
			memcpy(lastPortPower, portPower, sizeof(lastPortPower));
			memcpy(pItemsBack, pItems, sizeof(pItemsBack));
			tempScore = score;
			JE_genItemMenu(select);
			JE_initWeaponView();
			curMenu = 4;
			lastCurSel = curSel[4];
			score = tempScore - JE_cashLeft() + tempScore;
		}
		break;

	case 2:
		switch (select)
		{
		case 2:
			curMenu = 6;
			performSave = false;
			quikSave = false;
			break;
		case 3:
			curMenu = 6;
			performSave = true;
			quikSave = false;
			break;
		case 6:
			curMenu = 12;
			break;
		case 7:
			curMenu = 5;
			break;
		case 8:
			curMenu = 0;
			break;
		}
		break;

	case 3:
		if (select == menuChoices[3])
		{
			curMenu = 0;
			newPal = 1;
		} else {
			mainLevel = mapSection[curSelect - 2];
			jumpSection = true;
		}
		break;

	case 4:
		if (curSel[4] < menuChoices[4])
		{
			tempPowerLevel[curSel[4] - 1] = portPower[curSel[1] - 2];
			curSel[4] = menuChoices[4];
		} else {
			JE_playSampleNum(ITEM);
			score = JE_cashLeft();
			curMenu = 1;
		}
		break;

	case 5: // keyboard settings
		if (curSelect == 10) // reset to defaults
		{
			// TODO : Write default bind script and load it here
		} else if (curSelect == 11) { // done
			if (netmanager || onePlayerAction)
			{
				curMenu = 11;
			} else {
				curMenu = 2;
			}
		}
		else // change key
		{
			temp2 = 254;
			tempY = 38 + (curSelect - 2) * 12;
			//JE_textShade(236, tempY, SDL_GetKeyName(keySettings[curSelect-2]), (temp2 / 16), (temp2 % 16) - 8, DARKEN);
			JE_showVGA();

			col = 248;
			colC = 1;

			newkey = false;
			newmouse = false;

			do {
				col += colC;
				if (col < 243 || col > 248)
				{
					colC *= -1;
				}
				JE_rectangle(230, tempY - 2, 300, tempY + 7, col);

				setjasondelay(1);
				service_SDL_events(false);

				JE_showVGA();
			} while (!newkey && !newmouse);

			// Assign new key bindings
			if (newkey) {
				if ( lastkey_sym != SDLK_ESCAPE &&
					 lastkey_sym != SDLK_F11 &&
					 lastkey_sym != SDLK_m &&
					 lastkey_sym != SDLK_p )
				{
					if (lastkey_sym == SDLK_DELETE) {
						std::set<Bind*> b = BindManager::get().findBinds(keyConfigs[curSelect-2].command);
						for (std::set<Bind*>::iterator i = b.begin(); i != b.end(); ++i) {
							BindManager::get().removeBind(*i, keyConfigs[curSelect-2].command, keyConfigs[curSelect-2].toggle);
						}
					} else {
						BindManager::get().addBind(lastkey_sym, keyConfigs[curSelect-2].command, keyConfigs[curSelect-2].toggle);
						curSelect++;
					}
					JE_playSampleNum(CLICK);
				} else {
					JE_playSampleNum(WRONG);
				}
			} else if (newmouse) {
				BindManager::get().addBindMouse(lastmouse_but, keyConfigs[curSelect-2].command, keyConfigs[curSelect-2].toggle);
				curSelect++;
				JE_playSampleNum(CLICK);

				newmouse = false;
			}
		}
		break;

	case 6:
		if (curSelect == 13)
		{
			if (quikSave)
			{
				curMenu = oldMenu;
				newPal = oldPal;
			} else {
				curMenu = 2;
			}
		} else {
			if (twoPlayerMode)
			{
				temp = 11;
			} else {
				temp = 0;
			}
			JE_operation(curSelect - 1 + temp);
			if (quikSave)
			{
				curMenu = oldMenu;
				newPal = oldPal;
			}
		}
		break;

	case 7:
		if (curSelect == menuChoices[curMenu])
		{
			curMenu = 0;
			newPal = 1;
		} else {
			if (cubeMax > 0)
			{
				firstMenu9 = true;
				curMenu = 8;
				yLoc = 0;
				yChg = 0;
				currentCube = curSel[7] - 2;
			} else {
				curMenu = 0;
				newPal = 1;
			}
		}
		break;

	case 8:
		curMenu = 7;
		/*scanCode = 0;*/
		joystickUp = false;
		joystickDown = false;
		break;

	case 9:
		switch (curSel[curMenu])
		{
		case 2:
			mainLevel = mapSection[mapPNum-1];
			jumpSection = true;
			break;
		case 3:
			JE_playSampleNum(CURSOR_MOVE);
			do {
				inputDevice1++;
				if (inputDevice1 > 3)
				{
					inputDevice1 = 1;
				}
			} while (inputDevice1 == inputDevice2);
			break;
		case 4:
			JE_playSampleNum(CURSOR_MOVE);
			do {
				inputDevice2++;
				if (inputDevice2 > 3)
				{
					inputDevice2 = 1;
				}
			} while (inputDevice1 == inputDevice2);
			break;
		case 5:
			curMenu = 2;
			break;
		case 6:
			if (JE_quitRequest(true))
			{
				gameLoaded = true;
				mainLevel = 0;
			}
			break;
		}
		break;

	case 10:
		switch (curSel[curMenu])
		{
		case 2:
			mainLevel = mapSection[mapPNum-1];
			jumpSection = true;
			break;
		case 3:
			curMenu = 11;
			break;
		case 4:
			if (JE_quitRequest(true))
			{
				gameLoaded = true;
				mainLevel = 0;
			}
			break;
		}
		break;

	case 11:
		switch (select)
		{
		case 2:
			curMenu = 12;
			break;
		case 3:
			curMenu = 5;
			break;
		case 6:
			curMenu = 10;
			break;
		}
		break;

	case 12:
		switch (select)
		{
		case 2:
		case 3:
		case 4:
		case 5:
			joyButtonAssign[select - 2]++;
			if (joyButtonAssign[select - 2] > 5)
			{
				joyButtonAssign[select - 2] = 1;
			}
			break;
		case 6:
			if (joystick_installed)
			{
				do
				{
					//JE_drawJoystick();
					JE_helpBox(35, 35, mainMenuHelp[33 - 1], 18);
					JE_showVGA();
					if ( (joystickUp || joystickDown || joystickLeft || joystickRight) &&
						 !( button[0] || button[1] || button[2] || button[3]) )
					{
						inputDetected = false;
					}
				} while (!JE_anyButton());

				if (button[0] || button[1] || button[2] || button[3])
				{
					/* TODO?  joystick calibration seems unnecessary */
				}
			}
			break;
		case 7:
			if (netmanager || onePlayerAction)
			{
				curMenu = 11;
			} else {
				curMenu = 2;
			}
			break;
		}
		break;

	case 13:
		switch (curSel[curMenu])
		{
		case 2:
			mainLevel = mapSection[mapPNum-1];
			jumpSection = true;
			break;
		case 3:
			JE_doFunkyScreen();
			break;
		case 4:
			curMenu = 2;
			break;
		case 5:
			if (JE_quitRequest(true))
			{
				if (netmanager)
				{
					JE_tyrianHalt(0);
				}
				gameLoaded = true;
				mainLevel = 0;
			}
		}
		break;
	}

	memcpy(lastPortPower, portPower, sizeof(lastPortPower));
	memcpy(pItemsBack, pItems, sizeof(pItemsBack));

}

void JE_weaponSimUpdate()
{
	JE_weaponViewFrame(0);

	if ( (curSel[1] == 3 && curSel[4] < menuChoices[4]) || (curSel[1] == 4 && curSel[4] < menuChoices[4] - 1) )
	{

		if (leftPower)
		{
			JE_outText(26, 137, lexical_cast<std::string>(downgradeCost), 1, 4);
		} else {
			JE_newDrawCShapeNum(OPTION_SHAPES, 14, 24, 149);
		}

		if (rightPower)
		{
			if (!rightPowerAfford)
			{
				JE_outText(108, 137, lexical_cast<std::string>(upgradeCost), 7, 4);
				JE_newDrawCShapeNum(OPTION_SHAPES, 15, 119, 149);
			} else {
				JE_outText(108, 137, lexical_cast<std::string>(upgradeCost), 1, 4);
			}
		} else {
			JE_newDrawCShapeNum(OPTION_SHAPES, 15, 119, 149);
		}

		if (curSel[1] == 3)
		{
			temp = portPower[1 - 1];
		} else {
			temp = portPower[2 - 1];
		}
		for (JE_word x = 1; x <= temp; x++)
		{
			JE_bar(39 + x * 6, 151, 39 + x * 6 + 4, 151, 251);
			JE_pix(39 + x * 6, 151, 252);
			JE_bar(39 + x * 6, 152, 39 + x * 6 + 4, 164, 250);
			JE_bar(39 + x * 6, 165, 39 + x * 6 + 4, 165, 249);
		}

		JE_outText(58, 137, (boost::format("POWER: %1%") % temp).str(), 15, 4);
	} else {
		leftPower = false;
		rightPower = false;
		JE_newDrawCShapeNum(OPTION_SHAPES, 18, 20, 146);
	}

	JE_drawItem(1, pItems[PITEM_SHIP], PX - 5, PY - 7);
}

void JE_genItemMenu(int itemNum)
{
	menuChoices[4] = itemAvailMax[itemAvailMap[itemNum - 2] - 1] + 2;

	temp3 = 2;
	temp2 = pItems[pItemButtonMap[itemNum - 2] -1];

	menuInt[4][0] = menuInt[1][itemNum - 1];

	for (tempW = 0; tempW < itemAvailMax[itemAvailMap[itemNum-2]-1]; tempW++)
	{
		temp = itemAvail[itemAvailMap[itemNum-2]-1][tempW];
		switch (itemNum)
		{
		case 2:
			tempStr = ships[temp].name;
			break;
		case 3:
		case 4:
			tempStr = weaponPort[temp].name;
			tempPowerLevel[tempW] = 1;
			break;
		case 5:
			tempStr = shields[temp].name;
			break;
		case 6:
			tempStr = powerSys[temp].name;
			break;
		case 7:
		case 8:
			tempStr = options[temp].name;
			break;
		}
		if (temp == temp2)
		{
			temp3 = tempW + 2;
		}
		menuInt[4][tempW+1] = tempStr;
	}

	menuInt[4][tempW+1] = miscText[13];

	// YKS: I have no idea wtf this is doing, but I don't think it matters either, little of this function does
	if (itemNum == 3 || itemNum == 4)
	{
		tempPowerLevel[tempW] = portPower[itemNum-3];
		if (tempPowerLevel[tempW] < 1)
		{
			tempPowerLevel[tempW] = 1;
		}
	}
	curSel[4] = temp3;
}

unsigned long JE_cashLeft()
{
	unsigned long tempL;
	int x;
	JE_word itemNum;

	tempL = score;
	itemNum = pItems[pItemButtonMap[curSel[1] - 2] - 1];

	tempL -= JE_getCost(curSel[1], itemNum);

	tempW = 0;

	switch (curSel[1])
	{
		case 3:
		case 4:
			tempW2 = weaponPort[itemNum].cost;
			for (x = 1; x < portPower[curSel[1] - 3]; x++)
			{
				tempW += tempW2 * x;
				tempL -= tempW;
			}
			break;
	}

	return tempL;
}

void JE_drawMenuHeader()
{
	switch (curMenu)
	{
		case 8:
			tempStr = cubeHdr2[curSel[7]-2].substr(0, 30);
			break;
		case 7:
			tempStr = menuInt[0][1];
			break;
		case 6:
			tempStr = menuInt[2][performSave + 1];
			break;
		default:
			tempStr = menuInt[curMenu][0];
			break;
	}
	JE_dString(74 + JE_fontCenter(tempStr, FONT_SHAPES), 10, tempStr, FONT_SHAPES);
}

void JE_drawDots()
{
	int x, y;
	int tempX, tempY;

	for (x = 0; x < mapPNum; x++)
	{
		for (y = 0; y < planetDots[x]; y++)
		{
			tempX = planetDotX[x][y] - tempNavX + 66 - 2;
			tempY = planetDotY[x][y] - tempNavY + 85 - 2;
			if (tempX > 0 && tempX < 140 && tempY > 0 && tempY < 168)
			{
				if (x == curSel[3]-2 && y < currentDotNum)
				{
					JE_newDrawCShapeNum(OPTION_SHAPES, 31, tempX, tempY);
				} else {
					JE_newDrawCShapeNum(OPTION_SHAPES, 30, tempX, tempY);
				}
			}
		}
	}
}

void JE_drawScore()
{
	if (curMenu == 4)
	{
		JE_textShade(65, 173, lexical_cast<std::string>(JE_cashLeft()), 1, 6, DARKEN);
	}
}

void JE_drawNavLines(bool dark)
{
	int x, y;
	int tempX, tempY;
	int tempX2, tempY2;
	JE_word tempW, tempW2;

	tempX2 = tempNavX >> 1;
	tempY2 = tempNavY >> 1;

	tempW = 0;
	for (x = 1; x <= 20; x++)
	{
		tempW += 15;
		tempX = tempW - tempX2;

		if (tempX > 18 && tempX < 135)
		{
			if (dark)
			{
				JE_rectangle(tempX + 1, 16, tempX + 1, 169, 1);
			} else {
				JE_rectangle(tempX, 16, tempX, 169, 5);
			}
		}
	}

	tempW = 0;
	for (y = 1; y <= 20; y++)
	{
		tempW += 15;
		tempY = tempW - tempY2;

		if (tempY > 15 && tempY < 169)
		{
			if (dark)
			{
				JE_rectangle(19, tempY + 1, 135, tempY + 1, 1);
			} else {
				JE_rectangle(8, tempY, 160, tempY, 5);
			}

			tempW2 = 0;

			for (x = 0; x < 20; x++)
			{
				tempW2 += 15;
				tempX = tempW2 - tempX2;
				if (tempX > 18 && tempX < 135)
				{
					JE_pixCool(tempX, tempY, 7);
				}
			}
		}
	}
}

void JE_drawPlanet(int planetNum)
{
	int tempX, tempY, tempZ;

	tempZ = PGR[planetNum]-1;
	tempX = planetX[planetNum] + 66 - tempNavX - shapeX[PLANET_SHAPES][tempZ] / 2;
	tempY = planetY[planetNum] + 85 - tempNavY - shapeY[PLANET_SHAPES][tempZ] / 2;

	if (tempX > -7 && tempX + shapeX[PLANET_SHAPES][tempZ] < 170 && tempY > 0 && tempY < 160)
	{
		if (PAni[planetNum])
		{
			tempZ += planetAni;
		}
		JE_newDrawCShapeDarken(shapeArray[PLANET_SHAPES][tempZ], shapeX[PLANET_SHAPES][tempZ], shapeY[PLANET_SHAPES][tempZ], tempX + 3, tempY + 3);
		JE_newDrawCShapeNum(PLANET_SHAPES, tempZ+1, tempX, tempY);
	}
}

void JE_computeDots()
{
	int tempX, tempY;
	int distX, distY;
	int x, y;

	for (x = 0; x < mapPNum; x++)
	{
		distX = (int)(planetX[mapPlanet[x]-1]) - (int)(planetX[mapOrigin-1]);
		distY = (int)(planetY[mapPlanet[x]-1]) - (int)(planetY[mapOrigin-1]);
		tempX = abs(distX) + abs(distY);

		if (tempX != 0)
		{
			planetDots[x] = ot_round(sqrt(sqrt((float)((distX * distX) + (distY * distY))))) - 1;
		} else {
			planetDots[x] = 0;
		}

		if (planetDots[x] > 10)
		{
			planetDots[x] = 10;
		}

		for (y = 0; y < planetDots[x]; y++)
		{
			tempX = JE_partWay(planetX[mapOrigin-1], planetX[mapPlanet[x]-1], planetDots[x], y);
			tempY = JE_partWay(planetY[mapOrigin-1], planetY[mapPlanet[x]-1], planetDots[x], y);
			/* ??? Why does it use temp? =P */
			planetDotX[x][y] = tempX;
			planetDotY[x][y] = tempY;
		}
	}
}

unsigned long JE_getCost(int itemType, JE_word itemNum)
{
	switch (itemType)
	{
		case 2:
			if (itemNum > 90)
			{
				tempW2 = 100;
			} else {
				tempW2 = ships[itemNum].cost;
			}
			break;
		case 3:
		case 4:
			tempW2 = weaponPort[itemNum].cost;
			downgradeCost = JE_powerLevelCost(tempW2, portPower[itemType-3]-1);
			upgradeCost = JE_powerLevelCost(tempW2, portPower[itemType-3]);
			break;
		case 5:
			tempW2 = shields[itemNum].cost;
			break;
		case 6:
			tempW2 = powerSys[itemNum].cost;
			break;
		case 7:
		case 8:
			tempW2 = options[itemNum].cost;
			break;
	}

	return tempW2;
}

void JE_drawCube(JE_word x, JE_word y, int filter, int brightness)
{
	JE_newDrawCShapeDarken(shapeArray[OPTION_SHAPES][26-1], shapeX[OPTION_SHAPES][26-1],
	  shapeY[OPTION_SHAPES][26 - 1], x + 4, y + 4);
	JE_newDrawCShapeDarken(shapeArray[OPTION_SHAPES][26-1], shapeX[OPTION_SHAPES][26-1],
	  shapeY[OPTION_SHAPES][26 - 1], x + 3, y + 3);
	JE_newDrawCShapeAdjustNum(OPTION_SHAPES, 26, x, y, filter, brightness);
}

struct pair_second_cmp
{
	template<class A, class B> inline bool operator()( const std::pair<A,B>& a, const std::pair<A,B>& b )
	{
		return a.second < b.second;
	}
};

// TODO refactor globals into arguments
void JE_loadCubes()
{
	std::vector<std::pair<int,int> > cubes(cubeMax);
	for (unsigned int i = 0; i < cubeMax; ++i)
	{
		cubes[i].first = i;
		cubes[i].second = cubeList[i];
	}
	std::sort(cubes.begin(), cubes.end(), pair_second_cmp());

	int current_cube = 0; // In which cube we are in the file
	std::fstream f;
	Filesystem::get().openDatafileFail(f, cubeFile);

	std::string file_line;
	for (unsigned int cube = 0; cube < cubeMax; ++cube)
	{
		// Seek to correct cube
		for (;current_cube < cubes[cube].second; ++current_cube)
		{
			do
			{
				file_line = JE_readCryptLn(f);
			} while (file_line[0] != '*');
		}
		
		// Index into the other cube structures
		unsigned int cubei = cubes[cube].first;

		// Read face number, or assign a blank one if it's missing
		if (file_line.length() >= 6)
		{
			std::istringstream str(file_line.substr(4, 2));
			if (!(str >> faceNum[cubei]))
			{
				Console::get() << "\a7Warning:\ax Couldn't read face number for datacube " << cubes[cube].second << std::endl;
				faceNum[cubei] = 0;
			}
		} else {
			faceNum[cubei] = 0;
		}

		// Read cube titles
		cubeHdr[cubei] = JE_readCryptLn(f);
		cubeHdr2[cubei] = JE_readCryptLn(f);

		unsigned int cur_cube_line = 0;
		do
		{
			// This is a complete line, after joining several lines in the file
			std::string line;

			for (;;)
			{
				file_line = JE_readCryptLn(f);
				if (file_line.empty() || file_line[0] == '*') break;

				line += ' ';
				line += file_line;
			}

			// Now find the wrapping points and copy the text!
			// current line width, in pixels
			unsigned int line_w = 0;
			// index of string of last word
			unsigned int last_index = 0;
			// index of start of current wrapped line
			unsigned int start_index = 0;
			for (unsigned int i = 0; i < line.length(); ++i)
			{
				unsigned char c = static_cast<unsigned char>(line[i]);
				if (c > ' ' && c < 169 && c != '~' && fontMap[c] != 255 && shapeArray[5][fontMap[c]] != 0)
				{
					line_w += shapeX[5][fontMap[c]]+1;
				} else if (c == ' ') {
					line_w += 6;
					last_index = i;
				}

				// If we're on the last character, force to print rest of the line
				if (i == line.length()-1)
				{
					line_w = LINE_WIDTH;
					last_index = i+1;
				}

				if (line_w >= LINE_WIDTH)
				{
					cubeText[cubei][cur_cube_line] = line.substr(start_index, last_index-start_index);
					line_w = 0;
					i = last_index;
					start_index = ++last_index;
					++cur_cube_line;
				}
			}
		} while (file_line[0] != '*');
		cubeMaxY[cubei] = cur_cube_line;
		++current_cube;
	}
}

void JE_drawItem( int itemType, JE_word itemNum, JE_word x, JE_word y )
{
	JE_word tempW = 0;

	if (itemNum > 0)
	{
		switch (itemType)
		{
			case 2:
			case 3:
				tempW = weaponPort[itemNum].itemgraphic;
				break;
			case 5:
				tempW = powerSys[itemNum].itemgraphic;
				break;
			case 6:
			case 7:
				tempW = options[itemNum].itemgraphic;
				break;
			case 4:
				tempW = shields[itemNum].itemgraphic;
				break;
		}

		if (itemType == 1)
		{
			if (itemNum > 90)
			{
				shipGrPtr = shapes9;
				shipGr = JE_SGr(itemNum - 90, &shipGrPtr);
				JE_drawShape2x2(x, y, shipGr, shipGrPtr);
			} else {
				JE_drawShape2x2(x, y, ships[itemNum].shipgraphic, shapes9);
			}
		} else {
			if (tempW > 0)
			{
				JE_drawShape2x2(x, y, tempW, shapes6);
			}
		}
	}
}

int JE_partWay( int start, int finish, int dots, int dist )
{
	return (finish - start) / (dots + 2) * (dist + 1) + start;
}

void JE_doFunkyScreen()
{
	if (pItems[PITEM_SHIP] > 90)
	{
		temp = 32;
	} else if (pItems[PITEM_SHIP] > 0) {
		temp = ships[pItems[PITEM_SHIP]].bigshipgraphic;
	} else {
		temp = ships[pItemsBack[PITEM_SHIP]].bigshipgraphic;
	}

	switch (temp)
	{
		case 32:
			tempW = 35;
			tempW2 = 33;
			break;
		case 28:
			tempW = 31;
			tempW2 = 36;
			break;
		case 33:
			tempW = 31;
			tempW2 = 35;
			break;
	}
	tempW -= 30;
	
	VGAScreen = tempScreenSeg = VGAScreen2;
	JE_clr256();
	
	JE_newDrawCShapeNum(OPTION_SHAPES, temp, tempW, tempW2);
	JE_funkyScreen();
	
	tempScreenSeg = VGAScreenSeg;
	
	JE_loadPic(1, false);
	memcpy(VGAScreen2, VGAScreen, scr_width * scr_height);
}

bool JE_quitRequest(bool useMouse)
{
	bool retval;
	int sel;
	bool quit, select;
	int col, colC;

	if (useMouse)
	{
		JE_mouseReplace();
	}

	JE_showVGA();

	JE_clearKeyboard();

	quit = false;
	select = false;

	sel = 1;

	wait_noinput(true,true,true);

	JE_barShade(65, 55, 255, 155);

	do {
		col = 8;
		colC = 1;

		do {
			/* TODO NETWORK Probably remove
			if (netQuit)
			{
				JE_tyrianHalt(0);
			}*/

			setjasondelay(4);

			/*scanCode = 0;
			k = 0;*/

			JE_newDrawCShapeNum(OPTION_SHAPES, 36, 50, 50);
			JE_textShade(70, 60, miscText[29 - 1], 0, 5, FULL_SHADE);
			JE_helpBox(70, 90, miscText[30 + useMouse - 1], 30);

			col += colC;
			if (col > 8 || col < 2)
			{
				colC *= -1;
			}

			tempW = 54 + 45 - (JE_textWidth(miscText[10 - 1], FONT_SHAPES) >> 1);
			tempW2 = 149 + 45 - (JE_textWidth(miscText[11 - 1], FONT_SHAPES) >> 1);

			if (sel == 1)
			{
				tempI = col - 12;
				tempI2 = -5;
			} else {
				tempI = -5;
				tempI2 = col - 12;
			}

			JE_outTextAdjust(tempW, 128, miscText[10-1], 15, tempI, FONT_SHAPES, true);
			JE_outTextAdjust(tempW2, 128, miscText[11-1], 15, tempI2, FONT_SHAPES, true);

			if (useMouse)
			{
				JE_mouseStart();
				JE_showVGA();
				JE_mouseReplace();
			} else {
				JE_showVGA();
			}

			temp = useMouse;

		} while (!JE_waitAction(0, false)); /* JE: {Use previous framecount} */


		if (mouseButton > 0 && useMouse)
		{
			if (mouseX > 56 && mouseX < 142 && mouseY > 123 && mouseY < 149)
			{
				quit = true;
				select = true;
			}
			else if (mouseX > 151 && mouseX < 237 && mouseY > 123 && mouseY < 149)
			{
				quit = true;
			}
		} else {
			if (newkey)
			{
				switch (lastkey_sym)
				{
				case SDLK_LEFT:
				case SDLK_RIGHT:
					sel = !sel;
					JE_playSampleNum(CURSOR_MOVE);
					break;
				case SDLK_TAB:
					sel = !sel;
					break;
				case SDLK_RETURN:
				case SDLK_SPACE:
					quit = true;
					select = (sel == 1);
					break;
				case SDLK_ESCAPE:
					quit = true;
					break;
				default:
					break;
				}
			}
		}

	} while (!quit);

	/* TODO PROBABLY REMOVE
	if (netQuit)
	{
		JE_tyrianHalt(0);
	}*/

	retval = select;
	if (select)
	{
		JE_playSampleNum(ESC);
	} else {
		JE_playSampleNum(CLICK);
	}

	if (netmanager && select)
	{
		/* TODO NETWORK
		network::prepare(network::PACKET_QUIT);
		network::send(4);

		network::tyrian_halt(0, true);
		*/
	}

	return retval;
}

void JE_initWeaponView()
{
	JE_bar(8, 8, 144, 177, 0);

	option1X = 72 - 15;
	option1Y = 120;
	option2X = 72 + 15;
	option2Y = 120;

	PX    = 72;
	PY    = 110;
	PXChange = 0;
	PYChange = 0;
	lastPX = 72;
	lastPY = 110;
	lastPX2 = 72;
	lastPY2 = 110;
	power = 500;
	lastPower = 500;

	memset(shotAvail, 0, sizeof(shotAvail));

	for (int i = 0; i < 11; i++) {
		shotRepeat[i] = 1;
	}
	memset(shotMultiPos, 0, sizeof(shotMultiPos));

	JE_setupStars();
}

void JE_weaponViewFrame( int testshotnum )
{
	Uint8 *s; /* screen pointer, 8-bit specific */
	int i;

	JE_bar(8, 8, 143, 182, 0);

	/* JE: (* Port Configuration Display *)
	(*    drawportconfigbuttons;*/

	/*===========================STARS==========================*/
	/*DRAWSTARS*/

	for (i = MAX_STARS; i--;)
	{
		s = (Uint8 *)VGAScreen;
		
		starDat[i].sLoc += starDat[i].sMov + scr_width; // <overflow>
		
		if (starDat[i].sLoc < 177 * scr_width)
		{
			if (*(s + starDat[i].sLoc) == 0)
			{
				*(s + starDat[i].sLoc) = starDat[i].sC;
			}
			if (starDat[i].sC - 4 >= 9 * 16)
			{
				if (*(s + starDat[i].sLoc + 1) == 0)
				{
					*(s + starDat[i].sLoc + 1) = starDat[i].sC - 4;
				}
				if (starDat[i].sLoc > 0 && *(s + starDat[i].sLoc - 1) == 0)
				{
					*(s + starDat[i].sLoc - 1) = starDat[i].sC - 4;
				}
				if (*(s + starDat[i].sLoc + scr_width) == 0)
				{
					*(s + starDat[i].sLoc + scr_width) = starDat[i].sC - 4;
				}
				if (starDat[i].sLoc >= scr_width && *(s + starDat[i].sLoc - scr_width) == 0)
				{
					*(s + starDat[i].sLoc - scr_width) = starDat[i].sC - 4;
				}
			}
		}
	}

	mouseX = PX;
	mouseY = PY;

	/* JE: (* PLAYER SHOT Creation *) */
	for (temp = 0; temp <= 1; temp++)
	{
		if (shotRepeat[temp] > 0)
		{
			shotRepeat[temp]--;
		} else {
			JE_initPlayerShot(pItems[temp], temp + 1, PX, PY, mouseX, mouseY,
			  weaponPort[pItems[temp]].op[portConfig[temp] - 1][portPower[temp] - 1], 1);
		}
	}

	if (options[pItems[PITEM_LEFT_SIDEKICK]].wport > 0)
	{
		if (shotRepeat[3 - 1] > 0)
		{
			shotRepeat[3 - 1]--;
		} else {
			JE_initPlayerShot(options[pItems[PITEM_LEFT_SIDEKICK]].wport, 3, option1X, option1Y,
			  mouseX, mouseY, options[pItems[PITEM_LEFT_SIDEKICK]].wpnum, 1);
		}
	}

	if (options[pItems[PITEM_RIGHT_SIDEKICK]].tr == 2)
	{
		option2X = PX;
		option2Y = PY - 20;
		if (option2Y < 10)
		{
			option2Y = 10;
		}
	} else {
		option2X = 72 + 15;
		option2Y = 120;
	}

	if (options[pItems[PITEM_RIGHT_SIDEKICK]].wport > 0)
	{
		if (shotRepeat[4 - 1] > 0)
		{
			shotRepeat[4 - 1]--;
		} else {
			JE_initPlayerShot(options[pItems[PITEM_RIGHT_SIDEKICK]].wport, 4, option2X, option2Y,
			  mouseX, mouseY, options[pItems[PITEM_RIGHT_SIDEKICK]].wpnum, 1);
		}
	}

	/* Player Shot Images */
	for (int z = 0; z < MAX_PWEAPON; z++)
	{
		if (shotAvail[z] != 0)
		{
			shotAvail[z]--;
			if (z != MAX_PWEAPON - 1)
			{

				playerShotData[z].shotXM += playerShotData[z].shotXC;

				if (playerShotData[z].shotXM <= 100)
				{
					playerShotData[z].shotX += playerShotData[z].shotXM;
				}

				playerShotData[z].shotYM += playerShotData[z].shotYC;
				playerShotData[z].shotY += playerShotData[z].shotYM;

				if (playerShotData[z].shotYM > 100)
				{
					playerShotData[z].shotY -= 120;
					playerShotData[z].shotY += PYChange;
				}

				if (playerShotData[z].shotComplicated != 0)
				{
					playerShotData[z].shotDevX += playerShotData[z].shotDirX;
					playerShotData[z].shotX += playerShotData[z].shotDevX;

					if (abs(playerShotData[z].shotDevX) == playerShotData[z].shotCirSizeX)
					{
						playerShotData[z].shotDirX = -playerShotData[z].shotDirX;
					}

					playerShotData[z].shotDevY += playerShotData[z].shotDirY;
					playerShotData[z].shotY += playerShotData[z].shotDevY;

					if (abs(playerShotData[z].shotDevY) == playerShotData[z].shotCirSizeY)
					{
						playerShotData[z].shotDirY = -playerShotData[z].shotDirY;
					}
					/*Double Speed Circle Shots - add a second copy of above loop*/
				}

				tempShotX = playerShotData[z].shotX;
				tempShotY = playerShotData[z].shotY;

				if (playerShotData[z].shotX < 0 || playerShotData[z].shotX > 140 ||
				    playerShotData[z].shotY < 0 || playerShotData[z].shotY > 170)
				{
					shotAvail[z] = 0;
					goto draw_player_shot_loop_end;
				}

/*				if (playerShotData[z].shotTrail != 255)
				{
					if (playerShotData[z].shotTrail == 98)
					{
						JE_setupExplosion(playerShotData[z].shotX - playerShotData[z].shotXM, playerShotData[z].shotY - playerShotData[z].shotYM, playerShotData[z].shotTrail);
					} else {
						JE_setupExplosion(playerShotData[z].shotX, playerShotData[z].shotY, playerShotData[z].shotTrail);
					}
				}*/

				tempW = playerShotData[z].shotGr + playerShotData[z].shotAni;
				if (++playerShotData[z].shotAni == playerShotData[z].shotAniMax)
				{
					playerShotData[z].shotAni = 0;
				}

				if (tempW < 6000)
				{
					if (tempW > 1000)
					{
						tempW = tempW % 1000;
					}
					if (tempW > 500)
					{
						JE_drawShape2(tempShotX+1, tempShotY, tempW - 500, shapesW2);
					} else {
						JE_drawShape2(tempShotX+1, tempShotY, tempW, shapesC1);
					}
				}

			}

draw_player_shot_loop_end:
			;
		}
	}

	JE_newDrawCShapeNum(OPTION_SHAPES, 13, 0, 0);


	/*========================Power Bar=========================*/

	power = power + powerAdd;
	if (power > 900)
	{
		power = 900;
	}

	temp = power / 10;

	for (temp = 147 - temp; temp <= 146; temp++)
	{
		temp2 = 113 + (146 - temp) / 9 + 2;
		temp3 = (temp + 1) % 6;
		if (temp3 == 1)
		{
			temp2 += 3;
		}
		else if (temp3 != 0)
		{
			temp2 += 2;
		}

		JE_pix(141, temp, temp2 - 3);
		JE_pix(142, temp, temp2 - 3);
		JE_pix(143, temp, temp2 - 2);
		JE_pix(144, temp, temp2 - 1);
		JE_bar(145, temp, 149, temp, temp2);

		if (temp2 - 3 < 112)
		{
			temp2++;
		}

	}

	temp = 147 - (power / 10);
	temp2 = 113 + (146 - temp) / 9 + 4;

	JE_pix(141, temp - 1, temp2 - 1);
	JE_pix(142, temp - 1, temp2 - 1);
	JE_pix(143, temp - 1, temp2 - 1);
	JE_pix(144, temp - 1, temp2 - 1);

	JE_bar(145, temp-1, 149, temp-1, temp2);

	lastPower = temp;

	JE_waitFrameCount();
}

void JE_funkyScreen()
{
	wait_noinput(true,true,true);

	Uint8 *s = game_screen; /* 8-bit specific */
	Uint8 *src = (Uint8 *)VGAScreen2; /* 8-bit specific */
	
	for (unsigned int y = 0; y < 200; y++)
	{
		for (unsigned int x = 0; x < 320; x++)
		{
			int avg = 0;
			if (y > 0)
				avg += *(src - scr_width) & 0x0f;
			if (y < scr_height - 1)
				avg += *(src + scr_width) & 0x0f;
			if (x > 0)
				avg += *(src - 1) & 0x0f;
			if (x < scr_width - 1)
				avg += *(src + 1) & 0x0f;
			avg /= 4;
			
			if ((*src & 0x0f) > avg)
			{
				*s = (*src & 0x0f) | 0xc0;
			} else {
				*s = 0;
			}
			
			src++;
			s++;
		}
	}

	JE_clr256();
	JE_drawLines(true);
	JE_drawLines(false);
	JE_rectangle(0, 0, 319, 199, 37);
	JE_rectangle(1, 1, 318, 198, 35);

	s = (Uint8 *)VGAScreen2; /* 8-bit specific */
	src = game_screen; /* 8-bit specific */
	
	for (int y = 0; y < 200; y++)
	{
		for (int x = 0; x < 320; x++)
		{
			if (*src)
				*s = *src;
			
			src++;
			s++;
		}
	}

	verticalHeight = 9;
	JE_outText(10, 2, ships[pItems[PITEM_SHIP]].name, 12, 3);
	JE_helpBox(100, 20, shipInfo[pItems[PITEM_SHIP]-1][0], 40);
	JE_helpBox(100, 100, shipInfo[pItems[PITEM_SHIP]-1][1], 40);
	verticalHeight = 7;

	JE_outText(JE_fontCenter(miscText[4], TINY_FONT), 190, miscText[4], 12, 2);

	JE_playSampleNum(16);
	
	VGAScreen = VGAScreenSeg;
	JE_scaleInPicture();

	wait_input(true, true, true);
}

void JE_drawLines(bool dark)
{
	int x, y;
	int tempX, tempY;
	int tempX2, tempY2;
	JE_word tempW, tempW2;

	tempX2 = -10;
	tempY2 = 0;

	tempW = 0;
	for (x = 0; x < 20; x++)
	{
		tempW += 15;
		tempX = tempW - tempX2;

		if (tempX > 18 && tempX < 135)
		{
			if (dark)
			{
				JE_rectangle(tempX + 1, 0, tempX + 1, 199, 32+3);
			} else {
				JE_rectangle(tempX, 0, tempX, 199, 32+5);
			}
		}
	}

	tempW = 0;
	for (y = 0; y < 20; y++)
	{
		tempW += 15;
		tempY = tempW - tempY2;

		if (tempY > 15 && tempY < 169)
		{
			if (dark)
			{
				JE_rectangle(0, tempY + 1, 319, tempY + 1, 32+3);
			} else {
				JE_rectangle(0, tempY, 319, tempY, 32+5);
			}

			tempW2 = 0;

			for (x = 0; x < 20; x++)
			{
				tempW2 += 15;
				tempX = tempW2 - tempX2;
				if (tempX > 18 && tempX < 135)
				{
					JE_pixCool(tempX, tempY, 32+6);
				}
			}
		}
	}
}

void JE_scaleInPicture()
{
	for (int i = 2; i <= 160; i += 2)
	{
		if (JE_anyButton())
			i = 160;
		JE_scaleBitmap(VGAScreen2, 320, 200, 160 - i, 0, 160 + i - 1, 100 + ot_round(i * 0.625f) - 1);
		JE_showVGA();
		
		SDL_Delay(1);
	}
}

void JE_scaleBitmap( Uint8 *bitmap, JE_word x, JE_word y, JE_word x1, JE_word y1, JE_word x2, JE_word y2 )
{
	JE_word w = x2 - x1 + 1,
	        h = y2 - y1 + 1;
	Uint32 sx = x * 0x10000 / w,
	       sy = y * 0x10000 / h,
	       cx, cy = 0;
	
	Uint8 *s = (Uint8 *)VGAScreen;  /* 8-bit specific */
	Uint8 *src = bitmap;  /* 8-bit specific */
	
	s += y1 * scr_width + x1;
	
	for (; h; h--)
	{
		cx = 0;
		for (int x = w; x; x--)
		{
			*s = *src;
			s++;
			
			cx += sx;
			src += cx >> 16;
			cx &= 0xffff;
		}
		
		s += scr_width - w;
		
		cy += sy;
		src -= ((sx * w) >> 16);
		src += (cy >> 16) * scr_width;
		cy &= 0xffff;
	}
}