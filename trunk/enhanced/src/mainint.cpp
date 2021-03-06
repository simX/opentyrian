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
#include "opentyr.h"
#include "mainint.h"

#include "backgrnd.h"
#include "config.h"
#include "editship.h"
#include "episodes.h"
#include "error.h"
#include "fonthand.h"
#include "helptext.h"
#include "helptext.h"
#include "joystick.h"
#include "keyboard.h"
#include "loudness.h"
#include "newshape.h"
#include "nortsong.h"
#include "nortvars.h"
#include "params.h"
#include "picload.h"
#include "setup.h"
#include "sndmast.h"
#include "varz.h"
#include "vga256d.h"
#include "tyrian2.h"
#include "console/GameActions.h"
#include "HighScores.h"
#include "console/cvar/CVar.h"
#include "explosion.h"
#include "Filesystem.h"
#include "network/Network.h"

#include <cassert>
#include <cctype>
#include <cmath>
#include <sstream>
#include <string>
#include "boost/format.hpp"
#include "boost/lexical_cast.hpp"

using boost::lexical_cast;

static const int MAX_PAGE = 8;
static const int TOPICS = 6;
const int topicStart[TOPICS] = { 0, 1, 2, 3, 7, 255 };

int constantLastX;
JE_word textErase;
bool performSave;

bool useLastBank; /* See if I want to use the last 16 colors for DisplayText */
bool loadedMainShapeTables = false;

int player_delta_x[2], player_delta_y[2];

/* Draws a message at the bottom text window on the playing screen */
void JE_drawTextWindow( const std::string& text )
{
	tempScreenSeg = VGAScreenSeg; /*sega000*/
	if (textErase > 0)
	{
		JE_newDrawCShapeNum(OPTION_SHAPES, 37, 16, 189);
	}
	textErase = 100;
	tempScreenSeg = VGAScreenSeg; /*sega000*/
	JE_outText(20, 190, text, 0, 4);
}

void JE_outCharGlow( JE_word x, JE_word y, const std::string& s )
{
	int maxloc, loc;
	char glowcol[60]; /* [1..60] */
	char glowcolc[60]; /* [1..60] */
	JE_word textloc[60]; /* [1..60] */
	int b = 0;
	int bank;

	setjasondelay2(1);

	if (useLastBank)
	{
		bank = 15;
	} else {
		bank = 14;
	}
	if (warningRed)
	{
		bank = 7;
	}

	if (s[0])
	{
		if (frameCountMax == 0)
		{
			JE_textShade(x, y, s, bank, 0, PART_SHADE);
			JE_showVGA();
		} else {

			maxloc = s.length();
			tempScreenSeg = VGAScreen;
			for (int z = 0; z < 60; z++)
			{
				glowcol[z] = -8;
				glowcolc[z] = 1;
			}

			loc = x;
			for (int z = 0; z < maxloc; z++)
			{
				textloc[z] = loc;
				if (s[z] == ' ')
				{
					loc += 6;
				} else {
					loc += shapeX[TINY_FONT][fontMap[(int)s[z]-33]] + 1;
				}
			}

			for (int loc = 0; (unsigned)loc < s.length() + 28; loc++)
			{
				if (!ESCPressed)
				{
					setjasondelay(frameCountMax);

					if (netmanager)
						netmanager->updateNetwork();

					int z;
					for (z = loc - 28; z <= loc; z++)
					{
						if (z >= 0 && z < maxloc)
						{
							b = s[z];
							if (b > 32 && b < 126)
							{
								JE_newDrawCShapeAdjust(shapeArray[TINY_FONT][fontMap[b-33]], shapeX[TINY_FONT][fontMap[b-33]], shapeY[TINY_FONT][fontMap[b-33]], textloc[z], y, bank, glowcol[z]);
								glowcol[z] += glowcolc[z];
								if (glowcol[z] > 9)
								{
									glowcolc[z] = -1;
								}
							}
						}
					}
					if (b > 32 && b < 126 && --z < maxloc)
					{
						JE_newDrawCShapeShadow(shapeArray[TINY_FONT][fontMap[b-33]], shapeX[TINY_FONT][fontMap[b-33]], shapeY[TINY_FONT][fontMap[b-33]], textloc[z] + 1, y + 1);
					}
					if (JE_anyButton())
					{
						frameCountMax = 0;
					}
					do {
						if (levelWarningDisplay)
						{
							JE_updateWarning();
						}
						SDL_Delay(16);
					} while (!(delaycount() == 0 || ESCPressed));
					JE_showVGA();
				}
			}
		}
	}
}

void JE_drawPortConfigButtons( void )
{
	if (!twoPlayerMode)
	{
		if (portConfig[1] == 1)
		{
			tempScreenSeg = VGAScreenSeg;
			JE_newDrawCShapeNum(OPTION_SHAPES, 19, 285, 44);
			tempScreenSeg = VGAScreenSeg;
			JE_newDrawCShapeNum(OPTION_SHAPES, 20, 302, 44);
		} else {
			tempScreenSeg = VGAScreenSeg;
			JE_newDrawCShapeNum(OPTION_SHAPES, 20, 285, 44);
			tempScreenSeg = VGAScreenSeg;
			JE_newDrawCShapeNum(OPTION_SHAPES, 19, 302, 44);
		}
	}
}

void JE_helpSystem( int startTopic )
{
	int page, lastPage = 0;
	int menu;

	page = topicStart[startTopic-1];

	JE_fadeBlack(10);
	JE_loadPic(2, false);
	JE_playSong(20);
	JE_showVGA();
	JE_fadeColor(10);

	memcpy(VGAScreen2, VGAScreen, scr_width * scr_height);

	do
	{
		memcpy(VGAScreen, VGAScreen2, scr_width * scr_height);

		temp2 = 0;

		for (int temp = 0; temp < TOPICS; temp++)
		{
			if (topicStart[temp] <= page)
			{
				temp2 = temp;
			}
		}

		if (page > 0)
		{
			JE_outText(10, 192, (boost::format("%1% %2%") % miscText[24] % (page-topicStart[temp2]+1)).str(), 13, 5);

			JE_outText(220, 192, (boost::format("%1% %2% of %3%") % miscText[25] % page % MAX_PAGE).str(), 13, 5);

			JE_dString(JE_fontCenter(topicName[temp2], SMALL_FONT_SHAPES), 1, topicName[temp2], SMALL_FONT_SHAPES);
		}

		menu = 0;

		helpBoxBrightness = 3;
		verticalHeight = 8;

		switch (page)
		{
			case 0:
				menu = 2;
				if (lastPage == MAX_PAGE)
				{
					menu = TOPICS;
				}
				JE_dString(JE_fontCenter(topicName[0], FONT_SHAPES), 30, topicName[0], FONT_SHAPES);

				do
				{
					for (int temp = 1; temp < TOPICS; temp++)
					{
						std::string buf;

						if (temp == menu-1)
						{
							buf = "~"+topicName[temp];
						} else {
							buf = topicName[temp];
						}

						JE_dString(JE_fontCenter(topicName[temp], SMALL_FONT_SHAPES), temp * 20 + 40, buf, SMALL_FONT_SHAPES);
					}

					JE_showVGA();

					tempW = 0;
					JE_textMenuWait(&tempW, false);
					if (newkey)
					{
						switch (lastkey_sym)
						{
							case SDLK_UP:
								menu--;
								if (menu < 2)
								{
									menu = TOPICS;
								}
								JE_playSampleNum(CURSOR_MOVE);
								break;
							case SDLK_DOWN:
								menu++;
								if (menu > TOPICS)
								{
									menu = 2;
								}
								JE_playSampleNum(CURSOR_MOVE);
								break;
							default:
								break;
						}
					}
				} while (!(lastkey_sym == SDLK_ESCAPE || lastkey_sym == SDLK_RETURN));

				if (lastkey_sym == SDLK_RETURN)
				{
					page = topicStart[menu-1];
					JE_playSampleNum(CLICK);
				}

				break;
			case 1: /* One-Player Menu */
				JE_HBox(10,  20,  2, 60);
				JE_HBox(10,  50,  5, 60);
				JE_HBox(10,  80, 21, 60);
				JE_HBox(10, 110,  1, 60);
				JE_HBox(10, 140, 28, 60);
				break;
			case 2: /* Two-Player Menu */
				JE_HBox(10,  20,  1, 60);
				JE_HBox(10,  60,  2, 60);
				JE_HBox(10, 100, 21, 60);
				JE_HBox(10, 140, 28, 60);
				break;
			case 3: /* Upgrade Ship */
				JE_HBox(10,  20,  5, 60);
				JE_HBox(10,  70,  6, 60);
				JE_HBox(10, 110,  7, 60);
				break;
			case 4:
				JE_HBox(10,  20,  8, 60);
				JE_HBox(10,  55,  9, 60);
				JE_HBox(10,  87, 10, 60);
				JE_HBox(10, 120, 11, 60);
				JE_HBox(10, 170, 13, 60);
				break;
			case 5:
				JE_HBox(10,  20, 14, 60);
				JE_HBox(10,  80, 15, 60);
				JE_HBox(10, 120, 16, 60);
				break;
			case 6:
				JE_HBox(10,  20, 17, 60);
				JE_HBox(10,  40, 18, 60);
				JE_HBox(10, 130, 20, 60);
				break;
			case 7: /* Options */
				JE_HBox(10,  20, 21, 60);
				JE_HBox(10,  70, 22, 60);
				JE_HBox(10, 110, 23, 60);
				JE_HBox(10, 140, 24, 60);
				break;
			case 8:
				JE_HBox(10,  20, 25, 60);
				JE_HBox(10,  60, 26, 60);
				JE_HBox(10, 100, 27, 60);
				JE_HBox(10, 140, 28, 60);
				JE_HBox(10, 170, 29, 60);
				break;
		}

		helpBoxBrightness = 1;
		verticalHeight = 7;

		lastPage = page;

		if (menu == 0)
		{
			JE_showVGA();
			do
			{
				service_SDL_events(false);
			} while (mousedown);
			while (!JE_waitAction(1, true));

			if (newmouse)
			{
				switch (lastmouse_but)
				{
					case SDL_BUTTON_LEFT:
						lastkey_sym = SDLK_RIGHT;
						break;
					case SDL_BUTTON_RIGHT:
						lastkey_sym = SDLK_LEFT;
						break;
					case SDL_BUTTON_MIDDLE:
						lastkey_sym = SDLK_ESCAPE;
						break;
				}
				do
				{
					service_SDL_events(false);
				} while (mousedown);
				newkey = true;
			}

			if (newkey)
			{
				switch (lastkey_sym)
				{
					case SDLK_LEFT:
					case SDLK_UP:
					case SDLK_PAGEUP:
						page--;
						JE_playSampleNum(CURSOR_MOVE);
						break;
					case SDLK_RIGHT:
					case SDLK_DOWN:
					case SDLK_PAGEDOWN:
					case SDLK_RETURN:
					case SDLK_SPACE:
						if (page == MAX_PAGE)
						{
							page = 0;
						} else {
							page++;
						}
						JE_playSampleNum(CURSOR_MOVE);
						break;
					case SDLK_F1:
						page = 0;
						JE_playSampleNum(CURSOR_MOVE);
						break;
					default:
						break;
				}
			}
		}

		if (page == 255)
		{
			lastkey_sym = SDLK_ESCAPE;
		}
	} while (lastkey_sym != SDLK_ESCAPE);
}

void JE_loadCompShapesB( Uint8 **shapes, std::fstream& f, unsigned long shapeSize )
{
	*shapes = new Uint8[shapeSize];
	IBinaryStream bs(f);
	bs.getIter(*shapes, (*shapes)+shapeSize);
}

void JE_loadMainShapeTables( void )
{
	static const int shapeReorderList[7] /* [1..7] */ = {1, 2, 5, 0, 3, 4, 6};
	static const int SHP_NUM = 12;

	std::fstream f;
	IBinaryStream bs(f);

	unsigned long shpPos[SHP_NUM + 1];
	JE_word shpNumb;

	if (CVars::ch_xmas)
	{
		Filesystem::get().openDatafileFail(f, "tyrianc.shp");
	} else {
		Filesystem::get().openDatafileFail(f, "tyrian.shp");
	}

	shpNumb = bs.get16();
	for (int i = 0; i < shpNumb; ++i)
	{
		shpPos[i] = bs.getS32();
	}
	f.seekg(0, std::ios::end);
	shpPos[shpNumb] = f.tellg();

	int i;

	// Load interface and option sprites
	for (i = 0; i < 7; i++)
	{
		f.seekg(shpPos[i]);
		JE_newLoadShapesB(shapeReorderList[i], f);
	}

	// Player shot sprites
	shapesC1Size = shpPos[i+1] - shpPos[i];
	JE_loadCompShapesB(&shapesC1, f, shapesC1Size);
	++i;

	shapes9Size = shpPos[i+1] - shpPos[i];
	JE_loadCompShapesB(&shapes9 , f, shapes9Size);
	++i;

	eShapes6Size = shpPos[i+1] - shpPos[i];
	JE_loadCompShapesB(&eShapes6, f, eShapes6Size);
	++i;

	eShapes5Size = shpPos[i+1] - shpPos[i];
	JE_loadCompShapesB(&eShapes5, f, eShapes5Size);
	++i;

	shapesW2Size = shpPos[i+1] - shpPos[i];
	JE_loadCompShapesB(&shapesW2, f, shapesW2Size);

	f.close();

	loadedMainShapeTables = true;
}

JE_word JE_powerLevelCost( JE_word base, int level )
{
	JE_word tempCost = 0;

	if (level > 0 && level < 12)
	{
		for (int x = 1; x <= level; x++)
		{
			tempCost += base * x;
		}
	}

	return tempCost;
}

void JE_loadScreen( void )
{
	bool quit;
	int sel, screen, min = 0, max = 0;
	std::string tempstr;
	bool mal_str = false;

	JE_fadeBlack(10);
	JE_loadPic(2, false);
	JE_showVGA();
	JE_fadeColor(10);

	screen = 1;
	sel = 1;
	quit = false;

	memcpy(VGAScreen2, VGAScreen, scr_width * scr_height);

	do
	{
		while (mousedown)
		{
			service_SDL_events(false);
			tempX = mouse_x;
			tempY = mouse_y;
		}

		memcpy(VGAScreen, VGAScreen2, scr_width * scr_height);

		JE_dString(JE_fontCenter(miscText[38 + screen - 1], FONT_SHAPES), 5, miscText[38 + screen - 1], FONT_SHAPES);

		switch (screen)
		{
		case 1:
			min = 1;
			max = 12;
			break;
		case 2:
			min = 12;
			max = 23;
		}

		/* SYN: Go through text line by line */
		for (int x = min; x <= max; x++)
		{
			tempY = 30 + (x - min) * 13;

			if (x == max)
			{
				/* Last line is return to main menu, not a save game */
				if (mal_str)
				{
					mal_str = false;
				}
				tempstr = miscText[34 - 1];

				if (x == sel) /* Highlight if selected */
				{
					temp2 = 254;
				} else {
					temp2 = 250;
				}
			} else {
				if (x == sel) /* Highlight if selected */
				{
					temp2 = 254;
				} else {
					temp2 = 250 - ((saveFiles[x - 1].level == 0) << 1);
				}

				if (saveFiles[x - 1].level == 0) /* I think this means the save file is unused */
				{
					if (mal_str)
					{
						mal_str = false;
					}
					tempstr = miscText[3 - 1];
				} else {
					if (mal_str)
					{
						mal_str = false;
					}
					tempstr = saveFiles[x - 1].name;
				}
			}

			/* Write first column text */
			JE_textShade(10, tempY, tempstr, 13, (temp2 % 16) - 8, FULL_SHADE);

			if (x < max) /* Write additional columns for all but the last row */
			{
				if (saveFiles[x - 1].level == 0)
				{
					mal_str = true;
					tempstr = "-----"; /* Unused save slot */
				} else {
					tempstr = saveFiles[x - 1].levelName;
					JE_textShade(250, tempY, (boost::format("%1% %2%") % miscTextB[2-1] % saveFiles[x - 1].episode).str(), 5, (temp2 % 16) - 8, FULL_SHADE);
				}

				JE_textShade(120, tempY, (boost::format("%s %s") % miscTextB[3 - 1] % tempstr).str(), 5, (temp2 % 16) - 8, FULL_SHADE);
			}

		}

		if (screen == 2)
		{
			JE_drawShape2x2(90, 180, 279, shapes6);
		}
		if (screen == 1)
		{
			JE_drawShape2x2(220, 180, 281, shapes6);
		}

		helpBoxColor = 15;
		JE_helpBox(110, 182, miscText[56-1], 25);

		JE_showVGA();

		tempW = 0;
		JE_textMenuWait(&tempW, false);


		if (newkey)
		{
			switch (lastkey_sym)
			{
			case SDLK_UP:
				sel--;
				if (sel < min)
				{
					sel = max;
				}
				JE_playSampleNum(CURSOR_MOVE);
				break;
			case SDLK_DOWN:
				sel++;
				if (sel > max)
				{
					sel = min;
				}
				JE_playSampleNum(CURSOR_MOVE);
				break;
			case SDLK_LEFT:
			case SDLK_RIGHT:
				if (screen == 1)
				{
					screen = 2;
					sel += 11;
				} else {
					screen = 1;
					sel -= 11;
				}
				break;
			case SDLK_RETURN:
				if (sel < max)
				{
					if (saveFiles[sel - 1].level > 0)
					{
						JE_playSampleNum (SELECT);
						performSave = false;
						JE_operation(sel);
						quit = true;
					} else {
						JE_playSampleNum (WRONG);
					}
				} else {
					quit = true;
				}


				break;
			case SDLK_ESCAPE:
				quit = true;
				break;
			default:
				break;
			}

		}
	} while (!quit);
}

unsigned long JE_totalScore( unsigned long score, JE_PItemsType pitems )
{
	unsigned long tempL = score;

	tempL += JE_getValue(2, pitems[PITEM_SHIP]);
	tempL += JE_getValue(3, pitems[PITEM_FRONT_WEAPON]);
	tempL += JE_getValue(4, pitems[PITEM_REAR_WEAPON]);
	tempL += JE_getValue(5, pitems[PITEM_SHIELD]);
	tempL += JE_getValue(6, pitems[PITEM_GENERATOR]);
	tempL += JE_getValue(7, pitems[PITEM_LEFT_SIDEKICK]);
	tempL += JE_getValue(8, pitems[PITEM_RIGHT_SIDEKICK]);

	return tempL;
}

unsigned int JE_getValue( int itemType, JE_word itemNum )
{
	unsigned int tempW2 = 0;

	switch (itemType)
	{
		case 2:
			tempW2 = ships[itemNum].cost;
			break;
		case 3:
		case 4:
			tempW2 = weaponPort[itemNum].cost;
			tempW3 = tempW2;
			for (int z = 0; z < portPower[itemType-3]; z++)
			{
				tempW2 += JE_powerLevelCost(tempW3, z);
			}
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

bool JE_nextEpisode( void )
{
	bool found;
	int x;
	
	lastLevelName = "Completed";
	x = episodeNum;
	found = JE_findNextEpisode();
	
	if (!netmanager && !gameHasRepeated
	    && (jumpBackToEpisode1 || x == 3)
	    && x != 4
		&& !CVars::ch_constant_play
	    && !(pItems[PITEM_EPISODE] == 4))
	{
		JE_highScoreCheck();
		gameHasRepeated = true;
	}
	
	if (found)
	{
		gameLoaded = true;
		mainLevel = FIRST_LEVEL;
		saveLevel = FIRST_LEVEL;
		
		if (jumpBackToEpisode1 && x > 2 && !CVars::ch_constant_play)
		{
			JE_playCredits();
		}
		
		if (jumpBackToEpisode1 && (mt::rand() % 6) == 0)
		{
			pItems[PITEM_FRONT_WEAPON] = 23;
			pItems[PITEM_REAR_WEAPON] = 24;
			pItems[PITEM_SHIP] = 2;
			portPower[0] = 1;
			portPower[1] = 1;
			pItemsPlayer2[PITEM_REAR_WEAPON] = 24;
			memcpy(pItemsBack2, pItems, sizeof(pItemsBack2));
		}
		
		JE_playSong(27);
		
		JE_clr256();
		load_palette(5, false);
		
		tempScreenSeg = VGAScreen;
		
		JE_dString(JE_fontCenter(episodeName[episodeNum], SMALL_FONT_SHAPES),
		           130, episodeName[episodeNum], SMALL_FONT_SHAPES);
		
		JE_dString(JE_fontCenter(miscText[5-1], SMALL_FONT_SHAPES), 185,
		           miscText[5-1], SMALL_FONT_SHAPES);
		
		JE_showVGA();
		JE_fadeColor(15);
		
		if (!CVars::ch_constant_play)
		{
			do
			{
				if (netmanager)
					netmanager->updateNetwork();
				SDL_Delay(16);
			} while (!JE_anyButton());
		}
		JE_fadeBlack(15);
		
	} else {
		mainLevel = 0;
	}
	
	return found;
}

void JE_initPlayerData( void )
{
	/* JE: New Game Items/Data */
	pItems[PITEM_FRONT_WEAPON] = 1;  /* Normally 1 - Front Weapon */
	pItems[PITEM_REAR_WEAPON] = 0;  /* Normally 0 - Rear Weapon */
	pItems[PITEM_SUPER_ARCADE_MODE] = 0;  /* SuperArcade mode */
	pItems[PITEM_LEFT_SIDEKICK] = 0;  /* Left Option */
	pItems[PITEM_RIGHT_SIDEKICK] = 0;  /* Right Option */
	pItems[PITEM_GENERATOR] = 2;  /* Generator */
	pItems[PITEM_P2_SIDEKICK_UPGRADE] = 2; // Unused for Player 1
	pItems[PITEM_P2_SIDEKICK] = 1; // Unused for Player 1
	pItems[PITEM_EPISODE] = 0;  /* Starting Episode num */
	pItems[PITEM_SHIELD] = 4;  /* Shields */
	pItems[PITEM_SPECIAL] = 0; /* Secret Weapons - Normally 0 */
	pItems[PITEM_SHIP] = 1; /* Normally 1 - Player Ship */
	memcpy(pItemsBack2, pItems, sizeof(pItems));
	memcpy(pItemsPlayer2, pItems, sizeof(pItems));
	pItemsPlayer2[PITEM_REAR_WEAPON] = 15; /* Player 2 starts with 15 - MultiCannon and 2 single shot options */
	pItemsPlayer2[PITEM_LEFT_SIDEKICK] = 0;
	pItemsPlayer2[PITEM_RIGHT_SIDEKICK] = 0;
	pItemsPlayer2[PITEM_P2_SIDEKICK_UPGRADE] = 101; /* Player 2  Option Mode 101,102,103 */
	pItemsPlayer2[PITEM_P2_SIDEKICK] = 0; /* Player 2  Option Type */
	for (int temp = 0; temp < 2; temp++)
	{
		portConfig[temp] = 1;
	}
	gameHasRepeated = false;
	onePlayerAction = false;
	superArcadeMode = 0;
	superTyrian = false;
	/* twoplayerarcade:=false; */
	twoPlayerMode = false;

	secretHint = (mt::rand()%3) + 1;

	armorLevel = ships[pItems[PITEM_SHIP]].dmg;
	portPower[0] = 1;
	portPower[1] = 1;
	portConfig[1] = 1;

	mainLevel = FIRST_LEVEL;
	saveLevel = FIRST_LEVEL;

	lastLevelName = miscText[20-1];
}

void JE_highScoreScreen( void )
{
	JE_fadeBlack(10);
	JE_loadPic(2, false);
	JE_showVGA();
	JE_fadeColor(10);
	tempScreenSeg = VGAScreen;

	int cur_ep = 0;

	memcpy(VGAScreen2, VGAScreen, scr_width * scr_height);

	bool quit = false;
	while (!quit)
	{
		memcpy(VGAScreen, VGAScreen2, scr_width * scr_height);

		JE_dString( JE_fontCenter( miscText[50], FONT_SHAPES), 03, miscText[50], FONT_SHAPES);
		JE_dString( JE_fontCenter( episodeName[cur_ep+1], SMALL_FONT_SHAPES), 30, episodeName[cur_ep+1], SMALL_FONT_SHAPES);

		/* Player 1 */
		JE_dString( JE_fontCenter( miscText[46], SMALL_FONT_SHAPES), 55, miscText[46], SMALL_FONT_SHAPES);
		for (int i = 0; i < 3; i++)
		{
			HighScore& score = highScores.getScore(cur_ep, 0, i);

			if (score.getDifficulty() > 9)
			{
				score.setDifficulty(0);
			}

			std::ostringstream strs;
			strs << "~#" << i+1 << ":~ " << score.getScore();

			const unsigned char tmp = score.getDifficulty();
			JE_textShade(250, ((i+1) * 10) + 65 , difficultyNameB[tmp], 15, tmp + (tmp == 0) - 1, FULL_SHADE);
			JE_textShade(20, ((i+1) * 10) + 65 , strs.str().c_str(), 15, 0, FULL_SHADE);
			JE_textShade(110, ((i+1) * 10) + 65 , score.getName().c_str(), 15, 2, FULL_SHADE);
		}

		/* Player 2 */
		JE_dString( JE_fontCenter( miscText[47], SMALL_FONT_SHAPES), 120, miscText[47], SMALL_FONT_SHAPES);
		for (int i = 0; i < 3; i++)
		{
			HighScore& score = highScores.getScore(cur_ep, 1, i);

			if (score.getDifficulty() > 9)
			{
				score.setDifficulty(0);
			}

			std::ostringstream strs;
			strs << "~#" << i+1 << ":~ " << score.getScore();

			const unsigned char tmp = score.getDifficulty();
			JE_textShade(250, ((i+1) * 10) + 125 , difficultyNameB[tmp], 15, tmp + (tmp == 0) - 1, FULL_SHADE);
			JE_textShade(20, ((i+1) * 10) + 125 , strs.str().c_str(), 15, 0, FULL_SHADE);
			JE_textShade(110, ((i+1) * 10) + 125 , score.getName().c_str(), 15, 2, FULL_SHADE);
		}

		// Draw left arrow
		if (cur_ep > 0)
		{
			JE_drawShape2x2( 90, 180, 279, shapes6);
		}

		// Draw right arrow
		if (cur_ep+1 < NUM_EPISODES)
		{
			JE_drawShape2x2( 220, 180, 281, shapes6);
		}

		// Draw help text "Press left/right to change episodes", needed?
		helpBoxColor = 15;
		JE_helpBox(110, 182, miscText[56], 25);

		JE_showVGA();

		tempW = 0;
		JE_textMenuWait(&tempW, false);

		if (newkey)
		{
			switch (lastkey_sym)
			{
			case SDLK_LEFT:
				if (cur_ep > 0)
				{
					cur_ep--;
				} else {
					cur_ep = NUM_EPISODES - 1;
				}
				break;
			case SDLK_RIGHT:
				if (cur_ep < NUM_EPISODES - 1)
				{
					cur_ep++;
				} else {
					cur_ep = 0;
				}
				break;
			case SDLK_RETURN:
			case SDLK_ESCAPE:
				quit = true;
				break;
			default:
				break;
			}
		}
	}
}

void JE_gammaCorrect_func( Uint8 *col, float r )
{
	int temp = ot_round(*col * r);
	if (temp > 255)
	{
		temp = 255;
	}
	*col = temp;
}

void JE_gammaCorrect( Palette colorBuffer, int gamma )
{
	float r = 1 + (float)gamma / 10;
	
	for (int x = 0; x < 256; x++)
	{
		JE_gammaCorrect_func(&colorBuffer[x].r, r);
		JE_gammaCorrect_func(&colorBuffer[x].g, r);
		JE_gammaCorrect_func(&colorBuffer[x].b, r);
	}
}

void JE_gammaCheck( )
{
	gammaCorrection = (gammaCorrection + 1) % 4;
	load_palette(5, false);
	JE_gammaCorrect(colors, gammaCorrection);
	JE_updateColorsFast(colors);
}

/* void JE_textMenuWait( JE_word *waitTime, bool doGamma ); /!\ In setup.h */

void JE_doInGameSetup( void )
{
	haltGame = false;

	if (netmanager)
	{
		/* TODO NETWORK
		network::prepare(network::PACKET_GAME_MENU);
		network::send(4);

		for (;;)
		{
			service_SDL_events(false);

			if (network::packet_in[0] && SDLNet_Read16(network::packet_in[0]->data[0]) == network::PACKET_GAME_MENU)
			{
				network::update();
				break;
			}
			network::update();
			network::check();

			SDL_Delay(16);
		}
		*/
	}

	if (yourInGameMenuRequest)
	{
		useButtonAssign = false; /*Joystick button remapping*/
		if (JE_inGameSetup())
		{
			reallyEndLevel = true;
			playerEndLevel = true;
		}

		quitRequested = false;

		keysactive[SDLK_ESCAPE] = false;
		keysactive[SDLK_RETURN] = false;

		if (netmanager)
		{
			/* TODO NETWORK
			if (!playerEndLevel)
			{
				network::prepare(network::PACKET_WAITING);
				network::send(4);
			}
			else
			{
				network::prepare(network::PACKET_GAME_QUIT);
				network::send(4);
			}*/
		}
	}
	
	if (netmanager)
	{
		/* TODO NETWORK
		Uint8 *temp_surface = VGAScreen;
		VGAScreen = VGAScreenSeg;

		if (!yourInGameMenuRequest)
		{
			JE_barShade(3, 60, 257, 80);
			JE_barShade(5, 62, 255, 78);
			tempScreenSeg = VGAScreen;
			JE_dString(10, 65, "Other player in options menu.", SMALL_FONT_SHAPES);
			JE_showVGA();

			//while (network::state_is_latest() || (SDLNet_Read16(network::packet_in->data+0) != network::PACKET_WAITING && SDLNet_Read16(network::packet_in->data+0) != network::PACKET_GAME_QUIT))
			for (;;)
			{
				service_SDL_events(false);
				JE_showVGA();

				if (network::packet_in[0])
				{
					if (SDLNet_Read16(network::packet_in[0]->data+0) == network::PACKET_WAITING)
					{
						network::check();
						break;
					}
					else if (SDLNet_Read16(network::packet_in[0]->data+0) == network::PACKET_GAME_QUIT)
					{
						reallyEndLevel = true;
						playerEndLevel = true;

						network::check();
						break;
					}
				}

				network::update();
				network::check();
				SDL_Delay(16);
			}
		}
		else
		{
			*//*
			JE_barShade(3, 160, 257, 180);
			JE_barShade(5, 162, 255, 178);
			tempScreenSeg = VGAScreen;
			JE_dString(10, 165, "Waiting for other player.", SMALL_FONT_SHAPES);
			JE_showVGA();
			*//*
		}

		while (!network::is_sync())
		{
			service_SDL_events(false);
			JE_showVGA();

			network::check();
			SDL_Delay(16);
		}

		VGAScreen = temp_surface;*/
	}
	
	useButtonAssign = true;  /*Joystick button remapping*/
	yourInGameMenuRequest = false;

	tempScreenSeg = VGAScreen;
	//skipStarShowVGA = true;
}

bool JE_inGameSetup( void )
{
	Uint8 *temp_surface = VGAScreen;
	VGAScreen = VGAScreenSeg; /* side-effect of game_screen */
	
	bool returnvalue = false;
	
	const int help[6] /* [1..6] */ = {15, 15, 28, 29, 26, 27};
	int sel;
	bool quit;

	tempScreenSeg = VGAScreenSeg; /* <MXD> ? */
	JE_clearKeyboard();
	
	quit = false;
	sel = 1;
	
	JE_barShade(3, 13, 217, 137); /*Main Box*/
	JE_barShade(5, 15, 215, 135);
	
	JE_barShade(3, 143, 257, 157); /*Help Box*/
	JE_barShade(5, 145, 255, 155);
	memcpy(VGAScreen2, VGAScreen, scr_width * scr_height);
	
	do
	{
		memcpy(VGAScreen, VGAScreen2, scr_width * scr_height);
		
		for (int x = 0; x < 6; x++)
		{
			JE_outTextAdjust(10, (x + 1) * 20, inGameText[x], 15, ((sel == x+1) << 1) - 4, SMALL_FONT_SHAPES, true);
		}
		
		//JE_outTextAdjust(120, 3 * 20, detailLevel[CVars::detail_level], 15, ((sel == 3) << 1) - 4, SMALL_FONT_SHAPES, true);
		JE_outTextAdjust(120, 4 * 20, gameSpeedText[CVars::game_speed],   15, ((sel == 4) << 1) - 4, SMALL_FONT_SHAPES, true);
		
		JE_outTextAdjust(10, 147, mainMenuHelp[help[sel-1]-1], 14, 6, TINY_FONT, true);
		
		JE_barDrawShadow(120, 20, 1, 16, int(CVars::snd_music_vol*14.f), 3, 13);
		JE_barDrawShadow(120, 40, 1, 16, int(CVars::snd_fx_vol*14.f), 3, 13);
		
		JE_showVGA();
		
		tempW = 0;
		JE_textMenuWait(&tempW, true);
		
		if (inputDetected)
		{
			switch (lastkey_sym)
			{
				case SDLK_RETURN:
					JE_playSampleNum(SELECT);
					switch (sel)
					{
						case 1:
						case 2:
						case 3:
						case 4:
							sel = 5;
							break;
						case 5:
							quit = true;
							break;
						case 6:
							returnvalue = true;
							quit = true;
							if (CVars::ch_constant_play)
							{
								JE_tyrianHalt(0);
							}
							
							if (netmanager)
							{ /*Tell other computer to exit*/
								netQuit = true;
								haltGame = true;
								playerEndLevel = true;
							}
							break;
					}
					break;
				case SDLK_ESCAPE:
					quit = true;
					JE_playSampleNum(ESC);
					break;
				case SDLK_UP:
					if (--sel < 1)
					{
						sel = 6;
					}
					JE_playSampleNum(CURSOR_MOVE);
					break;
				case SDLK_DOWN:
					if (++sel > 6)
					{
						sel = 1;
					}
					JE_playSampleNum(CURSOR_MOVE);
					break;
				case SDLK_LEFT:
					switch (sel)
					{
						case 1:
							if (CVars::snd_music_vol == 0) {
								JE_playSampleNum(WRONG);
							} else {
								CVars::snd_music_vol = CVars::snd_music_vol - .05f;
							}
							break;
						case 2:
							if (CVars::snd_fx_vol == 0) {
								JE_playSampleNum(WRONG);
							} else {
								CVars::snd_fx_vol = CVars::snd_fx_vol - .05f;
							}
							break;
						case 3:
							/*if (CVars::detail_level == 0) {
								CVars::detail_level = 3;
							} else {
								CVars::detail_level = CVars::detail_level - 1;
							}
							JE_initProcessorType();
							JE_setNewGameSpeed();*/
							break;
						case 4:
							if (CVars::game_speed == 0) {
								CVars::game_speed = 4;
							} else {
								CVars::game_speed = CVars::game_speed - 1;
							}
							JE_initProcessorType();
							JE_setNewGameSpeed();
							break;
					}
					if (sel < 5)
					{
						JE_playSampleNum(CURSOR_MOVE);
					}
					break;
				case SDLK_RIGHT:
					switch (sel)
					{
						case 1:
							if (CVars::snd_music_vol == 1.5f) {
								JE_playSampleNum(WRONG);
							} else {
								CVars::snd_music_vol = CVars::snd_music_vol + .05f;
							}
							break;
						case 2:
							if (CVars::snd_fx_vol == 1.5f) {
								JE_playSampleNum(WRONG);
							} else {
								CVars::snd_fx_vol = CVars::snd_fx_vol + .05f;
							}
							break;
						case 3:
							/*if (CVars::detail_level >= 3) {
								CVars::detail_level = 0;
							} else {
								CVars::detail_level = CVars::detail_level + 1;
							}
							JE_initProcessorType();
							JE_setNewGameSpeed();*/
							break;
						case 4:
							if (CVars::game_speed >= 4) {
								CVars::game_speed = 0;
							} else {
								CVars::game_speed = CVars::game_speed + 1;
							}
							JE_initProcessorType();
							JE_setNewGameSpeed();
							break;
					}
					if (sel < 5)
					{
						JE_playSampleNum(CURSOR_MOVE);
					}
					break;
				case SDLK_w:
					/*if (sel == 3)
					{
						CVars::detail_level = 4;
						JE_initProcessorType();
					}*/
				default:
					break;
			}
		}
		
	} while (!(quit || haltGame || netQuit));
	netQuit = false;
	
	VGAScreen = temp_surface; /* side-effect of game_screen */
	
	return returnvalue;
}

void JE_inGameHelp( void )
{
	Uint8 *temp_surface = VGAScreen;
	VGAScreen = VGAScreenSeg; /* side-effect of game_screen */
	
	tempScreenSeg = VGAScreenSeg;
	JE_clearKeyboard();
	
	JE_barShade(1, 1, 262, 182); /*Main Box*/
	JE_barShade(3, 3, 260, 180);
	JE_barShade(5, 5, 258, 178);
	JE_barShade(7, 7, 256, 176);
	JE_bar     (9, 9, 254, 174, 0);
	
	/* Start Draw */
	
	if (twoPlayerMode)
	{  /*Two-Player Help*/
		
		helpBoxColor = 3;
		helpBoxBrightness = 3;
		JE_HBox(20,  4, 36, 50);
		
		JE_newDrawCShapeNum(OPTION_SHAPES, 44, 2, 21);
		helpBoxColor = 5;
		helpBoxBrightness = 3;
		JE_HBox(55, 20, 37, 40);
		
		JE_newDrawCShapeNum(OPTION_SHAPES, 42, 5, 36);
		helpBoxColor = 5;
		helpBoxBrightness = 3;
		JE_HBox(40, 43, 34, 44);
		
		JE_newDrawCShapeNum(OPTION_SHAPES, 43, 2, 79);
		helpBoxColor = 5;
		helpBoxBrightness = 3;
		JE_HBox(54, 84, 35, 40);
		
		helpBoxColor = 5;
		helpBoxBrightness = 3;
		JE_HBox(5, 126, 38, 55);
		helpBoxColor = 5;
		helpBoxBrightness = 3;
		JE_HBox(5, 160, 39, 55);
		
	} else {
		
		JE_newDrawCShapeNum(OPTION_SHAPES, 41, 15, 5);
		helpBoxColor = 5;
		helpBoxBrightness = 3;
		JE_HBox(40, 10, 31, 45);
		
		JE_newDrawCShapeNum(OPTION_SHAPES, 40, 5, 37);
		helpBoxColor = 5;
		helpBoxBrightness = 3;
		JE_HBox(40, 40, 32, 44);
		helpBoxColor = 5;
		helpBoxBrightness = 3;
		JE_HBox(40, 60, 33, 44);
		
		JE_newDrawCShapeNum(OPTION_SHAPES, 42, 5, 98);
		helpBoxColor = 5;
		helpBoxBrightness = 3;
		JE_HBox(40, 103, 34, 44);
		
		JE_newDrawCShapeNum(OPTION_SHAPES, 43, 2, 138);
		helpBoxColor = 5;
		helpBoxBrightness = 3;
		JE_HBox(54, 143, 35, 40);
		
	}
	
	/* End Draw */
	
	JE_newDrawCShapeNum(OPTION_SHAPES, 37, 16, 189);
	JE_outText(120 - JE_textWidth(miscText[5-1], TINY_FONT) / 2 + 20, 190, miscText[5-1], 0, 4);
	
	JE_showVGA();
	
	do {
		
		tempW = 0;
		JE_textMenuWait(&tempW, true);
		
	} while (!inputDetected);
	
	textErase = 1;

	VGAScreen = temp_surface;
}

void JE_highScoreCheck( void )
{
	int cur_ep = pItems[PITEM_EPISODE];

	int real_player = -1;
	for (int player = 0; player < (twoPlayerMode ? 2 : 1); player++)
	{
		highScores.sort();
		
		unsigned long real_score = 0;
		if (twoPlayerMode)
		{
			// Decides who gets to input it's score first
			if (real_player == -1)
			{
				real_player = (score2 > score ? 1 : 0);
			} else {
				real_player = !real_player;
			}

			switch (real_player)
			{
				case 0:
					real_score = score;
					break;
				case 1:
					real_score = score2;
					break;
			}
		} else {
			real_player = 0;
			real_score = JE_totalScore(score, pItems);
		}

		int got_score = highScores.insertScore(cur_ep-1, twoPlayerMode, HighScore(real_score), true);

		if (got_score != -1)
		{
			JE_clr256();
			JE_showVGA();
			load_palette(0, false);

			JE_playSong(34);

			/* Enter Thy name */
			bool quit = false;
			bool cancel = false;
			std::string name;
			int textPos = 0;
			bool fadein = true;
			
			int flash = 8 * 16 + 10;

			JE_barShade(65, 55, 255, 155);

			do {
				JE_dString(JE_fontCenter(miscText[51], FONT_SHAPES), 3, miscText[51], FONT_SHAPES);

				const int temp = twoPlayerMode ? 57 + real_player : 52;
				JE_dString(JE_fontCenter(miscText[temp], SMALL_FONT_SHAPES), 30, miscText[temp], SMALL_FONT_SHAPES);
				
				JE_newDrawCShapeNum(OPTION_SHAPES, 36, 50, 50);

				if (twoPlayerMode)
				{
					JE_textShade(60, 55, (std::string(miscText[47+real_player]) + miscText[53]).c_str(), 11, 4, FULL_SHADE);
				} else {
					JE_textShade(60, 55, miscText[53], 11, 4, FULL_SHADE);
				}

				std::ostringstream buf;
				buf << miscText[37] << " " << real_score;
				JE_textShade(70, 70, buf.str().c_str(), 11, 4, FULL_SHADE);

				do {
					if (flash == 8 * 16 + 10)
					{
						flash = 8 * 16 + 2;
					} else {
						flash = 8 * 16 + 10;
					}

					JE_outText(65, 89, name.c_str(), 8, 3);
					int cursor_pos = 65 + JE_textWidth(name.c_str(), TINY_FONT);
					JE_barShade(cursor_pos + 1, 90, cursor_pos + 5, 95);
					JE_bar(cursor_pos, 89, cursor_pos + 4, 94, flash);

					JE_showVGA();

					if (fadein)
					{
						JE_fadeColor(15);
						fadein = false;
					}
				} while (!JE_waitAction(14, false));

				if (!playing)
				{
					JE_playSong(32);
				}

				if (mouseButton > 0)
				{
					if (mouseX > 56 && mouseX < 142 && mouseY > 123 && mouseY < 149)
					{
						quit = true;
					} else if (mouseX > 151 && mouseX < 237 && mouseY > 123 && mouseY < 149) {
						quit = true;
						cancel = true;
					}
				} else {
					if (newkey)
					{
						switch(lastkey_sym)
						{
						case SDLK_BACKSPACE:
						case SDLK_DELETE:
							if (!name.empty())
							{
								name.erase(name.length()-1);
							}
							break;
						case SDLK_ESCAPE:
							quit = true;
							cancel = true;
							break;
						case SDLK_RETURN:
							quit = true;
							break;
						default:
							if (name.length() < 28 && std::isprint(lastkey_char))
							{
								char buf[2] = {lastkey_char, '\0'};
								name.append(buf);
							}
							break;
						}
					}
				}
			} while (!quit);

			int place = -1;
			bool dont_ask_second_player = false;
			if (!cancel)
			{
				place = highScores.insertScore(cur_ep-1, twoPlayerMode, HighScore(real_score, name, difficultyLevel));
				if (place == 2) // 3rd place
				{
					// Second player didn't get a score then, don't ask him
					dont_ask_second_player = true;
				}
			}

			JE_fadeBlack(15);
			JE_loadPic(2, false);

			JE_dString(JE_fontCenter(miscText[50], FONT_SHAPES), 10, miscText[50], FONT_SHAPES);
			JE_dString(JE_fontCenter(episodeName[cur_ep], SMALL_FONT_SHAPES), 35, episodeName[cur_ep], SMALL_FONT_SHAPES);

			// Draw scores
			for (int i = 0; i < 3; i++)
			{
				// Except the one the player got
				if (i != place)
				{
					const HighScore& score = highScores.getScore(cur_ep-1, twoPlayerMode, i);

					std::ostringstream strs;
					strs << "~#" << i+1 << ":~ " << score.getScore();

					JE_textShade( 20, (i+1) * 12 + 65, strs.str().c_str(), 15, 0, FULL_SHADE);
					JE_textShade(150, (i+1) * 12 + 65, score.getName().c_str(), 15, 2, FULL_SHADE);
				}
			}

			JE_showVGA();

			JE_fadeColor(15);

			if (place != -1)
			{
				const HighScore& score = highScores.getScore(cur_ep-1, twoPlayerMode, place);
				std::ostringstream strs;
				strs << "~#" << place+1 << ":~ " << score.getScore();

				textGlowFont = TINY_FONT;
				frameCountMax = 10;
				textGlowBrightness = 10;
				JE_outTextGlow( 20, (place+1) * 12 + 65, strs.str().c_str());
				textGlowBrightness = 10;
				JE_outTextGlow(150, (place+1) * 12 + 65, score.getName().c_str());

				if (frameCountMax != 0)
				{
					frameCountMax = 6;
					temp = 1;
				} else {
					temp = 0;
				}
				textGlowBrightness = 10;
				JE_outTextGlow(JE_fontCenter(miscText[4], TINY_FONT), 180, miscText[4]);
				JE_showVGA();
			}

			while (!(JE_anyButton() || (frameCountMax == 0 && temp == 1)));

			JE_fadeBlack(15);

			if (dont_ask_second_player)
			{
				break;
			}
		}
	}
}

void JE_changeDifficulty( void )
{
	unsigned int newDifficultyLevel;
	unsigned long temp;

	if (twoPlayerMode)
	{
		temp = score + score2;
	} else {
		temp = JE_totalScore(score, pItems);
	}

	switch (initialDifficulty)
	{
		case 1:
			temp = ot_round(temp * 0.4f);
			break;
		case 2:
			temp = ot_round(temp * 0.8f);
			break;
		case 3:
			temp = ot_round(temp * 1.3f);
			break;
		case 4:
			temp = ot_round(temp * 1.6f);
			break;
		case 5:
		case 6:
			temp = ot_round(temp * 2.f);
			break;
		case 7:
		case 8:
		case 9:
			temp = ot_round(temp * 3.f);
			break;
	}

	if (twoPlayerMode)
	{
		if (temp < 10000)
		{
			newDifficultyLevel = 1; /* Easy */
		} else if (temp < 20000) {
			newDifficultyLevel = 2; /* Normal */
		} else if (temp < 50000) {
			newDifficultyLevel = 3; /* Hard */
		} else if (temp < 80000) {
			newDifficultyLevel = 4; /* Impossible */
		} else if (temp < 125000) {
			newDifficultyLevel = 5; /* Impossible B */
		} else if (temp < 200000) {
			newDifficultyLevel = 6; /* Suicide */
		} else if (temp < 400000) {
			newDifficultyLevel = 7; /* Maniacal */
		} else if (temp < 600000) {
			newDifficultyLevel = 8; /* Zinglon */
		} else {
			newDifficultyLevel = 9; /* Nortaneous */
		}
	} else {
		if (temp < 40000)
		{
			newDifficultyLevel = 1; /* Easy */
		} else if (temp < 70000) {
			newDifficultyLevel = 2; /* Normal */
		} else if (temp < 150000) {
			newDifficultyLevel = 3; /* Hard */
		} else if (temp < 300000) {
			newDifficultyLevel = 4; /* Impossible */
		} else if (temp < 600000) {
			newDifficultyLevel = 5; /* Impossible B */
		} else if (temp < 1000000) {
			newDifficultyLevel = 6; /* Suicide */
		} else if (temp < 2000000) {
			newDifficultyLevel = 7; /* Maniacal */
		} else if (temp < 3000000) {
			newDifficultyLevel = 8; /* Zinglon */
		} else {
			newDifficultyLevel = 9; /* Nortaneous */
		}
	}

	if (newDifficultyLevel > difficultyLevel)
	{
		difficultyLevel = newDifficultyLevel;
	}

}

void JE_doDemoKeys( void )
{
	if (lastKey[0] > 0)
	{
		PY -= CURRENT_KEY_SPEED;
	}
	if (lastKey[1] > 0)
	{
		PY += CURRENT_KEY_SPEED;
	}
	if (lastKey[2] > 0)
	{
		PX -= CURRENT_KEY_SPEED;
	}
	if (lastKey[3] > 0)
	{
		PX += CURRENT_KEY_SPEED;
	}
	if (lastKey[4] > 0)
	{
		button[1-1] = true;
	}
	if (lastKey[5] > 0)
	{
		button[4-1] = true;
	}
	if (lastKey[6] > 0)
	{
		button[2-1] = true;
	}
	if (lastKey[7] > 0)
	{
		button[3-1] = true;
	}
}

void JE_readDemoKeys( void )
{
	temp = nextDemoOperation;

	lastKey[0] = (temp & 0x01) > 0;
	lastKey[1] = (temp & 0x02) > 0;
	lastKey[2] = (temp & 0x04) > 0;
	lastKey[3] = (temp & 0x08) > 0;
	lastKey[4] = (temp & 0x10) > 0;
	lastKey[6] = (temp & 0x40) > 0;
	lastKey[7] = (temp & 0x80) > 0;

	temp = recordFile.get();
	temp2 = recordFile.get();
	lastMoveWait = temp << 8 | temp2;
	nextDemoOperation = recordFile.get();
}

/*Street Fighter codes*/
void JE_SFCodes( int playerNum_, int PX_, int PY_, int mouseX_, int mouseY_, JE_PItemsType pItems_ )
{
	int temp, temp2, temp3, temp4, temp5;
	
	/*Get direction*/
	tempW = pItems_[PITEM_SHIP]; // Get player ship
	if (playerNum_ == 2 && tempW < 15)
	{
		tempW = 0;
	}
	
	if (tempW < 15)
	{
		
		temp2 = (mouseY_ > PY_) +    /*UP*/
		        (mouseY_ < PY_) +    /*DOWN*/
		        (PX_ < mouseX_) +    /*LEFT*/
		        (PX_ > mouseX_);     /*RIGHT*/
		temp = (mouseY_ > PY_) * 1 + /*UP*/
		       (mouseY_ < PY_) * 2 + /*DOWN*/
		       (PX_ < mouseX_) * 3 + /*LEFT*/
		       (PX_ > mouseX_) * 4;  /*RIGHT*/
		
		if (temp == 0) // no direction being pressed
		{
			if (!button[0]) // if fire button is released
			{
				temp = 9;
				temp2 = 1;
			} else {
				temp2 = 0;
				temp = 99;
			}
		}
		
		if (temp2 == 1) // if exactly one direction pressed or firebutton is released
		{
			temp += button[0] * 4;
			
			temp3 = superTyrian ? 21 : 3;
			for (int temp2 = 0; temp2 < temp3; temp2++)
			{
				
				/*Use SuperTyrian ShipCombos or not?*/
				temp5 = superTyrian ? shipCombosB[temp2] : shipCombos[tempW][temp2];
				
				// temp5 == selected combo in ship
				if (temp5 == 0) /* combo doesn't exists */
				{
					// mark twiddles as cancelled/finished
					SFCurrentCode[playerNum_-1][temp2] = 0;
				} else {
					// get next combo key
					temp4 = keyboardCombos[temp5-1][SFCurrentCode[playerNum_-1][temp2]];
					
					// correct key
					if (temp4 == temp)
					{
						SFCurrentCode[playerNum_-1][temp2]++;
						
						temp4 = keyboardCombos[temp5-1][SFCurrentCode[playerNum_-1][temp2]];
						if (temp4 > 100 && temp4 <= 100 + SPECIAL_NUM)
						{
							SFCurrentCode[playerNum_-1][temp2] = 0;
							SFExecuted[playerNum_-1] = temp4 - 100;
						}
					} else {
						if ((temp != 9) &&
						    (temp4 - 1) % 4 != (temp - 1) % 4 &&
						    (SFCurrentCode[playerNum_-1][temp2] == 0 ||
						     keyboardCombos[temp5-1][SFCurrentCode[playerNum_-1][temp2]-1] != temp))
						{
							SFCurrentCode[playerNum_-1][temp2] = 0;
						}
					}
				}
			}
		}
		
	}
}

bool JE_getPassword( void )
{
	STUB();
	return false;
}

void JE_playCredits( void )
{
	std::vector<std::string> credstr;
	JE_word maxlen = 0;
	unsigned int curpos, newpos;
	int yloc;
	std::fstream f;
	int currentpic = 1, fade = 0;
	int fadechg = 1;
	int currentship = 0;
	int shipx = 0, shipxwait = 0;
	int shipxc = 0, shipxca = 0;
	
	JE_newLoadShapes(EXTRA_SHAPES, "estsc.shp");
	
	setjasondelay2(1000);
	
	if (currentSong != 9)
		JE_playSong(9);

	Filesystem::get().openDatafileFail(f, "tyrian.cdt");

	try
	{
		for (;;)
		{
			credstr.push_back(JE_readCryptLn(f));
		}
	}
	catch (StringReadingException&)
	{
	}
	maxlen += 20 * 3 * credstr.size();
	
	load_palette(5, false);
	JE_clr256();
	JE_showVGA();
	JE_fadeColor(2);
	
	tempScreenSeg = VGAScreenSeg;
	
	for (int x = 0; x < maxlen; x++)
	{
		setjasondelay(1);
		JE_clr256();
		
		JE_newDrawCShapeAdjust(shapeArray[EXTRA_SHAPES][currentpic-1], shapeX[EXTRA_SHAPES][currentpic-1], shapeY[EXTRA_SHAPES][currentpic-1], 319 - shapeX[EXTRA_SHAPES][currentpic-1], 100 - (shapeY[EXTRA_SHAPES][currentpic-1] / 2), 0, fade - 15);
		
		fade += fadechg;
		if (fade == 0 && fadechg == -1)
		{
			fadechg = 1;
			currentpic++;
			if (currentpic > maxShape[EXTRA_SHAPES])
				currentpic = 1;
		}
		if (fade == 15)
			fadechg = 0;
		
		if (delaycount2() == 0)
		{
			fadechg = -1;
			setjasondelay2(900);
		}
		
		curpos = (x / 3) / 20;
		yloc = 20 - ((x / 3) % 20);
		
		if (x % 200 == 0)
		{
			currentship = (mt::rand() % 11) + 1;
			shipxwait = (mt::rand() % 80) + 10;
			if ((mt::rand() % 2) == 1)
			{
				shipx = 1;
				shipxc = 0;
				shipxca = 1;
			}
			else
			{
				shipx = 900;
				shipxc = 0;
				shipxca = -1;
			}
		}
		
		shipxwait--;
		if (shipxwait == 0)
		{
			if (shipx == 1 || shipx == 900)
				shipxc = 0;
			shipxca = -shipxca;
			shipxwait = (mt::rand() % 40) + 15;
		}
		shipxc += shipxca;
		shipx += shipxc;
		if (shipx < 1)
		{
			shipx = 1;
			shipxwait = 1;
		}
		if (shipx > 900)
		{
			shipx = 900;
			shipxwait = 1;
		}
      	tempI = shipxc * shipxc;
		if (450 + tempI < 0 || 450 + tempI > 900)
		{
			if (shipxca < 0 && shipxc < 0)
				shipxwait = 1;
			if (shipxca > 0 && shipxc > 0)
				shipxwait = 1;
		}
		tempW = ships[currentship].shipgraphic;
		if (shipxc < -10)
			tempW -= 2;
		if (shipxc < -20)
			tempW -= 2;
		if (shipxc > 10)
			tempW += 2;
		if (shipxc > 20)
			tempW += 2;
		JE_drawShape2x2(shipx / 40, 184 - (x % 200), tempW, shapes9);
		
		for (newpos = curpos - 9; newpos <= curpos; newpos++)
		{
			if (newpos > 0 && newpos <= credstr.size())
			{
				if (credstr[newpos-1] != "." && !credstr[newpos-1].empty())
				{
					std::string substr = credstr[newpos-1].substr(1);
					JE_outTextAdjust(110 - JE_textWidth(substr, SMALL_FONT_SHAPES) / 2 + abs((yloc / 18) % 4 - 2) - 1, yloc - 1, substr, credstr[newpos-1][0] - 65, -8, SMALL_FONT_SHAPES, false);
					JE_outTextAdjust(110 - JE_textWidth(substr, SMALL_FONT_SHAPES) / 2, yloc, substr, credstr[newpos-1][0] - 65, -2, SMALL_FONT_SHAPES, false);
				}
			}
			
			yloc += 20;
		}
		
		JE_bar(0,  0, 319, 10, 0);
		JE_bar(0, 190, 319, 199, 0);
		
		if (currentpic == maxShape[EXTRA_SHAPES])
			JE_outTextAdjust(5, 180, miscText[55-1], 2, -2, SMALL_FONT_SHAPES, false);
		
		if (netmanager)
			netmanager->updateNetwork();

		wait_delay();
		
		JE_showVGA();
		if (JE_anyButton())
		{
			x = maxlen - 1;
		} else {
			if (newpos == credstr.size() - 8)
				JE_selectSong(0xC001);
			if (x == maxlen - 1)
			{
				x--;
				if (currentSong != 10)
					JE_playSong(10);
			}
		}
	}
	
	JE_fadeBlack(10);
	
	JE_newPurgeShapes(EXTRA_SHAPES);
}

void JE_endLevelAni( void )
{
	JE_word x, y;
	int temp;
	
	if (!CVars::ch_constant_play)
	{
		/*Grant Bonus Items*/
		/*Front/Rear*/
		saveTemp[SAVE_FILES_SIZE + pItems[PITEM_FRONT_WEAPON]] = 1;
		saveTemp[SAVE_FILES_SIZE + pItems[PITEM_REAR_WEAPON]] = 1;
		saveTemp[SAVE_FILES_SIZE + pItemsPlayer2[PITEM_FRONT_WEAPON]] = 1;
		saveTemp[SAVE_FILES_SIZE + pItemsPlayer2[PITEM_REAR_WEAPON]] = 1;
		
		/*Special*/
		if (pItems[PITEM_SPECIAL] < 21)
		{
			saveTemp[SAVE_FILES_SIZE + 81 + pItems[PITEM_SPECIAL]] = 1;
		}
		
		/*Options*/
		saveTemp[SAVE_FILES_SIZE + 51 + pItems[PITEM_LEFT_SIDEKICK]] = 1;
		saveTemp[SAVE_FILES_SIZE + 51 + pItems[PITEM_RIGHT_SIDEKICK]] = 1;
		saveTemp[SAVE_FILES_SIZE + 51 + pItemsPlayer2[PITEM_LEFT_SIDEKICK]] = 1;
		saveTemp[SAVE_FILES_SIZE + 51 + pItemsPlayer2[PITEM_RIGHT_SIDEKICK]] = 1;
	}
	
	JE_changeDifficulty();
	
	memcpy(pItemsBack2, pItems, sizeof(pItemsBack2));
	lastLevelName = levelName;

	frameCountMax = 4;
	textGlowFont = SMALL_FONT_SHAPES;
	
	JE_setPalette(254, 255, 255, 255);
	
	if (!levelTimer || levelTimerCountdown > 0 || !(episodeNum == 4))
	{
		JE_playSampleNum(V_LEVEL_END);
	} else {
		JE_playSong(22);
	}
  
	if (bonusLevel)
	{
		JE_outTextGlow(20, 20, miscText[17-1]);
	} else if (playerAlive && (!twoPlayerMode || playerAliveB)) {
		JE_outTextGlow(20, 20, (boost::format("%1% %2%") % miscText[27-1] % levelName).str());
	} else {
		JE_outTextGlow(20, 20, (boost::format("%1% %2%") % miscText[62-1] % levelName).str());
	}
	
	if (twoPlayerMode)
	{
		JE_outTextGlow(30, 50, (boost::format("%1% %2%") % miscText[41-1] % score).str());
		JE_outTextGlow(30, 70, (boost::format("%1% %2%") % miscText[42-1] % score2).str());
	} else {
		JE_outTextGlow(30, 50, (boost::format("%1% %2%") % miscText[28-1] % score).str());
	}
	
	if (totalEnemy == 0)
	{
		temp = 0;
	} else {
		temp = ot_round(float(enemyKilled * 100 / totalEnemy));
	}
	JE_outTextGlow(40, 90, (boost::format("%1% %2%%%") % miscText[63-1] % temp).str());

	if (!onePlayerAction && !twoPlayerMode)
	{
		JE_outTextGlow(30, 120, miscText[4-1]);   /*Cubes*/

		if (cubeMax > 0)
		{
			if (cubeMax > 4)
			{
				cubeMax = 4;
			}
			if (frameCountMax != 0)
			{
				frameCountMax = 1;
			}
			for (unsigned int temp = 1; temp <= cubeMax; temp++)
			{
				if (netmanager)
					netmanager->updateNetwork();

				JE_playSampleNum(18);
				x = 20 + 30 * temp;
				y = 135;
				JE_drawCube(x, y, 9, 0);
				JE_showVGA();
				
				for (int i = -15; i <= 10; i++)
				{
					setjasondelay(frameCountMax);
					tempScreenSeg = VGAScreenSeg; /* sega000 */
					JE_newDrawCShapeAdjustNum(OPTION_SHAPES, 26, x, y, 9, i);
					if (JE_anyButton())
					{
						frameCountMax = 0;
					}
					JE_showVGA();

					int delaycount_temp;
					if ((delaycount_temp = target - SDL_GetTicks()) > 0)
						SDL_Delay(delaycount_temp);
				}
				for (int i = 10; i >= 0; i--)
				{
					setjasondelay(frameCountMax);
					tempScreenSeg = VGAScreenSeg; /* sega000 */
					JE_newDrawCShapeAdjustNum(OPTION_SHAPES, 26, x, y, 9, i);
					if (JE_anyButton())
					{
						frameCountMax = 0;
					}
					JE_showVGA();

					int delaycount_temp;
					if ((delaycount_temp = target - SDL_GetTicks()) > 0)
						SDL_Delay(delaycount_temp);
				}
			}
		} else {
			JE_outTextGlow(50, 135, miscText[15-1]);
		}
		
	}

	if (frameCountMax != 0)
	{
		frameCountMax = 6;
		temp = 1;
	} else {
		temp = 0;
	}
	temp2 = twoPlayerMode ? 150 : 160;
	JE_outTextGlow(90, temp2, miscText[5-1]);

	if (!CVars::ch_constant_play)
	{
		do
		{
			setjasondelay(1);

			if (netmanager)
				netmanager->updateNetwork();

			int delaycount_temp;
			if ((delaycount_temp = target - SDL_GetTicks()) > 0)
				SDL_Delay(delaycount_temp);
		} while (!(JE_anyButton() || (frameCountMax == 0 && temp == 1)));
	}

	JE_fadeBlack(15);
	JE_clr256();
}

void JE_handleChat( void )
{
	// STUB(); Annoying piece of crap =P
}

bool JE_getNumber( char *s, int *x )
{
	bool getNumber = false;
	char buf[256];

	while (strlen(s) > 0)
	{
		if (s[0] == ' ')
		{
			strcpy(buf, s+1);
			strcpy(s, buf);
		} else {
			char *tmp;

			*x = strtol(s, &tmp, 10);
			if (s != tmp)
			{
				getNumber = true;
			}
			strcpy(buf, tmp);
			strcpy(s, buf);
			goto end_loop;
		}
	}

end_loop:
	return getNumber;
}

void JE_operation( int slot )
{
	int flash;
	bool quit;
	std::string stemp;
	std::string tempStr;
	
	if (!performSave) {
		if (saveFiles[slot-1].level > 0)
		{
			gameJustLoaded = true;
			JE_loadGame(slot);
			gameLoaded = true;
			quit = true;
			
			if (pItems[PITEM_SUPER_ARCADE_MODE] > 0)
			{
				onePlayerAction = true;
				if (pItems[PITEM_SUPER_ARCADE_MODE] == 255)
				{
					superTyrian = true;
				} else {
					superArcadeMode = pItems[PITEM_SUPER_ARCADE_MODE];
				}
			}
		}
	} else if (slot % 11 != 0) {
		quit = false;
		// PORT-CHANGE
		stemp = saveFiles[slot-1].name;
		
		flash = 8 * 16 + 10;
		
		while (JE_mousePosition(&tempX, &tempY) != 0); /* TODO non-busy wait */
		
		JE_barShade(65, 55, 255, 155);
		
		do {
			JE_newDrawCShapeNum(OPTION_SHAPES, 36, 50, 50);
			
			JE_textShade(60, 55, miscText[1-1], 11, 4, DARKEN);
			JE_textShade(70, 70, levelName, 11, 4, DARKEN);
			
			do {
				flash = (flash == 8 * 16 + 10) ? 8 * 16 + 2 : 8 * 16 + 10;
				temp3 = (temp3 == 6) ? 2 : 6;
				
				tempStr = miscText[2-1] + stemp;
				JE_outText(65, 89, tempStr, 8, 3);
				tempW = 65 + JE_textWidth(tempStr, TINY_FONT);
				JE_barShade(tempW + 2, 90, tempW + 6, 95);
				JE_bar(tempW + 1, 89, tempW + 5, 94, flash);
				
				JE_showVGA();
			} while (!JE_waitAction(14, false));
			
			if (mouseButton > 0)
			{
				if (mouseX > 56 && mouseX < 142 && mouseY > 123 && mouseY < 149)
				{
					quit = true;
					JE_saveGame(slot, stemp);
					JE_playSampleNum(SELECT);
				} else if (mouseX > 151 && mouseX < 237 && mouseY > 123 && mouseY < 149) {
					quit = true;
					JE_playSampleNum(ESC);
				}
			} else {
				
				if (newkey)
				{
					bool validkey = false;
					lastkey_char = toupper(lastkey_char);
					switch(lastkey_char)
					{
						case ' ':
						case '-':
						case '.':
						case ',':
						case ':':
						case '!':
						case '?':
						case '#':
						case '@':
						case '$':
						case '%':
						case '*':
						case '(':
						case ')':
						case '/':
						case '=':
						case '+':
						case '<':
						case '>':
						case ';':
						case '"':
						case '\'':
							validkey = true;
						default:
							if (stemp.length() < 14 && (validkey || (lastkey_char >= 'A' && lastkey_char <= 'Z') || (lastkey_char >= '0' && lastkey_char <= '9')))
							{
								JE_playSampleNum(CURSOR_MOVE);
								stemp.append(1, lastkey_char);
							}
							break;
						case SDLK_BACKSPACE:
						case SDLK_DELETE:
							if (!stemp.empty())
							{
								stemp.erase(stemp.end()-1);
								JE_playSampleNum(CLICK);
							}
							break;
						case SDLK_ESCAPE:
							quit = true;
							JE_playSampleNum(ESC);
							break;
						case SDLK_RETURN:
							quit = true;
							JE_saveGame(slot, stemp);
							drawGameSaved = true;
							JE_playSampleNum(SELECT);
							break;
					}
				}
				
		  }
		
		} while (!quit);
	}
	
	while (JE_mousePosition(&tempX, &tempY) != 0); /* TODO non-busy wait */
}

void JE_inGameDisplays( void )
{
	std::string stemp;

	JE_textShade(30, 175, lexical_cast<std::string>(score), 2, 4, FULL_SHADE);
	if (twoPlayerMode && !galagaMode)
	{
		JE_textShade(230, 175, lexical_cast<std::string>(score2), 2, 4, FULL_SHADE);
	}

	/*Special Weapon?*/
	if (pItems[PITEM_SPECIAL] > 0)
	{
		JE_drawShape2x2(25, 1, special[pItems[PITEM_SPECIAL]].itemgraphic, eShapes6);
	}
	draw_special_shot_info();

	/*Lives Left*/
	if (onePlayerAction || twoPlayerMode)
	{
		
		for (int temp = 0; temp < (onePlayerAction ? 1 : 2); temp++)
		{
			temp5 = (temp == 0 && pItems[PITEM_SPECIAL] > 0) ? 35 : 15;
			tempW = (temp == 0) ? 30: 270;
			
			if (portPower[temp] > 5)
			{
				JE_drawShape2(tempW, temp5, 285, shapes9);
				tempW = (temp == 0) ? 45 : 250;
				JE_textShade(tempW, temp5 + 3, lexical_cast<std::string>(portPower[temp]-1), 15, 1, FULL_SHADE);
			} else if (portPower[temp] > 1) {
				for (int temp2 = 1; temp2 < portPower[temp]; temp2++)
				{
					JE_drawShape2(tempW, temp5, 285, shapes9);
					tempW = (temp == 0) ? (tempW + 12) : (tempW - 12);
				}
			}
			
			stemp = (temp == 0) ? miscText[49-1] : miscText[50-1];
			if (netmanager)
			{
				stemp = JE_getName(temp+1);
			}
			
			tempW = (temp == 0) ? 28 : (285 - JE_textWidth(stemp, TINY_FONT));
			JE_textShade(tempW, temp5 - 7, stemp, 2, 6, FULL_SHADE);
			
		}
		
	}

	/*Super Bombs!!*/
	for (int temp = 0; temp < 2; temp++)
	{
		if (superBomb[temp] > 0)
		{
			tempW = (temp == 0) ? 30 : 270;
			
			for (int temp2 = 0; temp2 < superBomb[temp]; temp2++)
			{
				JE_drawShape2(tempW, 160, 304, shapes9);
				tempW = (temp == 0) ? (tempW + 12) : (tempW - 12);
			}
		}
	}

	if (youAreCheating)
	{
		JE_outText(90, 170, "Cheaters always prosper.", 3, 4);
	}
}

void JE_mainKeyboardInput( void )
{
	if (keysactive[SDLK_F11])
	{
		JE_gammaCheck();
		keysactive[SDLK_F11] = false;
		newkey = false;
	}

	// Network Request Commands

	if (!netmanager)
	{
		/* { Edited Ships } for Player 1 */
		if (extraAvail && keysactive[SDLK_TAB] && !netmanager && !superTyrian)
		{
			for (int i = SDLK_0; i <= SDLK_9; ++i)
			{
				if (keysactive[i])
				{
					// TODO Refactor this and the one for player 2 into a function
					int ship = (i == SDLK_0 ? 10 : i - SDLK_0);
					pItems[PITEM_SHIP] = 90 + ship; // Ships
					ship = (ship-1)*15;
					pItems[PITEM_FRONT_WEAPON] = extraShips[ship+1];
					pItems[PITEM_REAR_WEAPON] = extraShips[ship+2];
					pItems[PITEM_SPECIAL] = extraShips[ship+3];
					pItems[PITEM_LEFT_SIDEKICK] = extraShips[ship+4];
					pItems[PITEM_RIGHT_SIDEKICK] = extraShips[ship+5];
					pItems[PITEM_GENERATOR] = extraShips[ship+6];
					// Armor
					pItems[PITEM_SHIELD] = extraShips[ship+8];
					std::fill(shotMultiPos, shotMultiPos+COUNTOF(shotMultiPos), 0);
					JE_portConfigs();
					if (portConfig[1] > tempW) portConfig[1] = 1;
					tempW = armorLevel;
					JE_getShipInfo();
					if (armorLevel > tempW && editShip1) armorLevel = tempW;
					editShip1 = true;

					Uint8* temp_surface = VGAScreen;
					VGAScreen = VGAScreenSeg;
					JE_wipeShieldArmorBars();
					JE_drawArmor();
					JE_drawShield();
					VGAScreen = temp_surface;
					tempScreenSeg = VGAScreenSeg;
					JE_drawOptions();
					tempScreenSeg = VGAScreenSeg;

					keysactive[i] = false;
				}
			}
		}

		/* for Player 2 */
		if (extraAvail && keysactive[SDLK_CAPSLOCK] && !netmanager && !superTyrian)
		{
			for (int i = SDLK_0; i <= SDLK_9; ++i)
			{
				if (keysactive[i])
				{
					int ship = (i == SDLK_0 ? 10 : i - SDLK_0);
					pItemsPlayer2[PITEM_SHIP] = 90 + ship;
					ship = (ship-1)*15;
					pItemsPlayer2[PITEM_FRONT_WEAPON] = extraShips[ship+1];
					pItemsPlayer2[PITEM_REAR_WEAPON] = extraShips[ship+2];
					pItemsPlayer2[PITEM_SPECIAL] = extraShips[ship+3];
					pItemsPlayer2[PITEM_LEFT_SIDEKICK] = extraShips[ship+4];
					pItemsPlayer2[PITEM_RIGHT_SIDEKICK] = extraShips[ship+5];
					pItemsPlayer2[PITEM_GENERATOR] = extraShips[ship+6];
					// Armor
					pItemsPlayer2[PITEM_SHIELD] = extraShips[ship+8];
					std::fill(shotMultiPos, shotMultiPos+COUNTOF(shotMultiPos), 0);
					JE_portConfigs();
					if (portConfig[1] > tempW) portConfig[1] = 1;
					tempW = armorLevel2;
					JE_getShipInfo();
					if (armorLevel2 > tempW && editShip2) armorLevel2 = tempW;
					editShip2 = true;

					Uint8* temp_surface = VGAScreen;
					VGAScreen = VGAScreenSeg;
					JE_wipeShieldArmorBars();
					JE_drawArmor();
					JE_drawShield();
					VGAScreen = temp_surface;
					tempScreenSeg = VGAScreenSeg;
					JE_drawOptions();
					tempScreenSeg = VGAScreenSeg;

					keysactive[i] = false;
				}
			}
		}
	}

	/* { In-Game Help } */
	if (keysactive[SDLK_F1])
	{
		if (netmanager)
		{
			// TODO NETWORK
			//helpRequest = true;
		} else {
			JE_inGameHelp();
			skipStarShowVGA = true;
		}
	}

	/* {!Activate Nort Ship!} */
	if (keysactive[SDLK_F2] && keysactive[SDLK_F4] && keysactive[SDLK_F6] && keysactive[SDLK_F7] &&
	    keysactive[SDLK_F9] && keysactive[SDLK_BACKSLASH] && keysactive[SDLK_SLASH])
	{
		if (netmanager)
		{
			// TODO NETWORK
			//nortShipRequest = true;
		} else {
			pItems[PITEM_SHIP] = 12;
			pItems[PITEM_FRONT_WEAPON] = 13;
			pItems[PITEM_REAR_WEAPON] = 36;
			pItems[PITEM_SUPER_ARCADE_MODE] = 37;
			shipGr = 1;
		}
	}

	/* {Cheating} */
	if (!netmanager && !twoPlayerMode && !superTyrian && superArcadeMode == 0)
	{
		if (keysactive[SDLK_F2] && keysactive[SDLK_F3] && keysactive[SDLK_F6])
		{
			youAreCheating = !youAreCheating;
			keysactive[SDLK_F2] = false;
		}

		if (keysactive[SDLK_F2] && keysactive[SDLK_F3] && (keysactive[SDLK_F4] || keysactive[SDLK_F5]) && !superTyrian)
		{
			armorLevel = 0;
			armorLevel2 = 0;
			youAreCheating = !youAreCheating;
			JE_drawTextWindow(miscText[63-1]);
		}

		if (CVars::ch_constant_play && keysactive[SDLK_c] && !superTyrian && superArcadeMode == 0)
		{
			youAreCheating = !youAreCheating;
			keysactive[SDLK_c] = false;
		}
	}

	if (superTyrian)
	{
		youAreCheating = false;
	}

	/* {Personal Commands} */

	/* {DEBUG} */
	if (keysactive[SDLK_F10] && keysactive[SDLK_BACKSPACE])
	{
		keysactive[SDLK_F10] = false;
		debug = !debug;

		debugHist = 1;
		debugHistCount = 1;

		/* YKS: clock ticks since midnight replaced by SDL_GetTicks */
		lastDebugTime = SDL_GetTicks();
	}

	/* {CHEAT-SKIP LEVEL} */
	if (keysactive[SDLK_F2] && keysactive[SDLK_F6] && (keysactive[SDLK_F7] || keysactive[SDLK_F8]) &&
	    !keysactive[SDLK_F9] && !superTyrian && superArcadeMode == 0)
	{
		if (netmanager)
		{
			// TODO NETWORK
			//skipLevelRequest = true;
		} else {
			levelTimer = true;
			levelTimerCountdown = 0;
			endLevel = true;
			levelEnd = 40;
		}
	}

	/* { Pause Game } */
	if (keysactive[SDLK_p])
	{
		if (netmanager)
		{
			// TODO NETWORK
			//pauseRequest = true;
		} else {
			JE_pauseGame();
		}
	}

	/* {In-Game Setup} */
	if (keysactive[SDLK_ESCAPE])
	{
		if (netmanager)
		{
			inGameMenuRequest = true;
		} else {
			yourInGameMenuRequest = true;
			JE_doInGameSetup();
			skipStarShowVGA = true;
		}
	}

	// Mute Sound // TODO Replace with CVar/CCmd!!!
	if (keysactive[SDLK_m])
	{
		keysactive[SDLK_m] = false;
		if (CVars::snd_mute)
		{
			CVars::snd_mute = false;
			JE_drawTextWindow(miscText[18]);
			//JE_selectSong(2);
		} else {
			CVars::snd_mute = true;
			JE_drawTextWindow(miscText[17]);
			//JE_stopSong();
		}
	}

	if (keysactive[SDLK_BACKSPACE])
	{
		/* {SCREENSHOT PAUSE} */
		if (keysactive[SDLK_NUMLOCK])
		{
			superPause = !superPause;
		}

		/* {SMOOTHIES} */
		if (keysactive[SDLK_F12] && keysactive[SDLK_SCROLLOCK])
		{
			for (int temp = SDLK_2; temp <= SDLK_9; temp++)
			{
				if (keysactive[temp])
				{
					smoothies[temp-SDLK_2] = !smoothies[temp-SDLK_2];
				}
			}
			if (keysactive[SDLK_0])
			{
				smoothies[8] = !smoothies[8];
			}
		} else

		/* {CYCLE THROUGH FILTER COLORS} */
		if (keysactive[SDLK_MINUS])
		{
			if (levelFilter == -99)
			{
				levelFilter = 0;
			} else {
				levelFilter++;
				if (levelFilter == 16)
				{
					levelFilter = -99;
				}
			}
		} else

		/* {HYPER-SPEED} */
		if (keysactive[SDLK_1])
		{
			fastPlay++;
			if (fastPlay > 2)
			{
				fastPlay = 0;
			}
			keysactive[SDLK_1] = false;
			JE_setNewGameSpeed();
		}

		/* {IN-GAME RANDOM MUSIC SELECTION} */
		if (keysactive[SDLK_SCROLLOCK])
		{
			tempW = (mt::rand() % MUSIC_NUM)+1;
			JE_playSong(tempW);
		}
	}

	newkey = false;
}

void JE_pauseGame( void )
{
	bool done = false;
	JE_word mouseX, mouseY;
	
	Uint8 *temp_surface;

	tempScreenSeg = VGAScreenSeg; // sega000
	if (!superPause)
	{
		JE_dString(120, 90, miscText[22], FONT_SHAPES);
		
		temp_surface = VGAScreen;  /* side-effect of game_screen */
		VGAScreen = VGAScreenSeg;
		JE_showVGA();
		VGAScreen = temp_surface;
	}
	music_vol_multiplier = .54f;

	if (netmanager)
	{
		/* TODO NETWORK
		network::prepare(network::PACKET_GAME_PAUSE);
		network::send(4);

		for (;;)
		{
			service_SDL_events(false);

			if (network::packet_in[0] && SDLNet_Read16(network::packet_in[0]->data+0) == network::PACKET_GAME_PAUSE)
			{
				network::update();
				break;
			}
			network::update();
			network::check();

			SDL_Delay(16);
		}
		*/
	}

	newkey = false;

	do
	{
		setjasondelay(2);

		JE_joystick2();

		if (superPause)
		{
			if ((newkey && (lastkey_sym != SDLK_F11) && !(lastkey_sym == SDLK_LALT) && !(lastkey_sym == SDLK_c) && !(lastkey_sym == SDLK_SPACE)) || (JE_mousePosition(&mouseX, &mouseY) > 0) || button[1] || button[2] || button[3]) 
			{
				if (netmanager)
				{
					/* TODO NETWORK
					network::prepare(network::PACKET_WAITING);
					network::send(4);*/
				}
				done = true;
			}
		}
		else if ((newkey && lastkey_sym != SDLK_F11) || JE_mousePosition(&mouseX, &mouseY) > 0 || button[0] || button[1] || button[2] || button[3])
		{
			if (netmanager)
			{
				/* TODO NETWORK
				network::prepare(network::PACKET_WAITING);
				network::send(4);*/
			}
			done = true;
		}

		if (netmanager)
		{
			/* TODO NETWORK
			network::check();

			if (network::packet_in[0] && SDLNet_Read16(network::packet_in[0]->data+0) == network::PACKET_WAITING)
			{
				network::check();
				done = true;
			}*/
		}

		if (lastkey_sym == SDLK_p)
		{
			repause = true;
		}

		wait_delay();
	} while (!done);

	music_vol_multiplier = 1.f;

	if (netmanager)
	{
		/* TODO NETWORK
		while (!network::is_sync())
		{
			service_SDL_events(false);
			network::check();
			SDL_Delay(16);
		}
		*/
	}

	tempScreenSeg = VGAScreen;
	//skipStarShowVGA = true;
}

void JE_playerMovement( int inputDevice_,
                        int playerNum_,
                        JE_word shipGr_,
                        Uint8 *shapes9ptr_,
                        int *armorLevel_, int *baseArmor_,
                        int *shield_, int *shieldMax_,
                        JE_word *playerInvulnerable_,
                        int *PX_, int *PY_,
                        int *lastPX_, int *lastPY_,
                        int *lastPX2_, int *lastPY2_,
                        int *PXChange_, int *PYChange_,
                        int *lastTurn_, int *lastTurn2_, int *tempLastTurn2_,
                        int *stopWaitX_, int *stopWaitY_,
                        JE_word *mouseX_, JE_word *mouseY_,
                        bool *playerAlive_,
                        int *playerStillExploding_,
                        JE_PItemsType pItems_ )
{
	int mouseXC, mouseYC;
	int accelXC, accelYC;
	int leftOptionIsSpecial = 0;
	int rightOptionIsSpecial = 0;

	if (playerNum_ == 2 || !twoPlayerMode)
	{
		if (playerNum_ == 2)
		{
			tempW = weaponPort[pItemsPlayer2[PITEM_REAR_WEAPON]].opnum;
		} else {
			tempW = weaponPort[pItems_[PITEM_REAR_WEAPON]].opnum;
		}
		if (portConfig[2-1] > tempW)
		{
			portConfig[2-1] = 1;
		}
	}

	/* TODO NETWORK
	if (network::netmanager && thisPlayerNum == playerNum_)
	{
		network::state_prepare();
		std::fill_n(network::packet_state_out[0]->data+4, 10, 0);
	}*/

redo:
	if (netmanager)
	{
		inputDevice_ = 0;
	}

	mouseXC = 0;
	mouseYC = 0;
	accelXC = 0;
	accelYC = 0;

	/* Draw Player */
	if (!*playerAlive_)
	{
		if (*playerStillExploding_ > 0)
		{

			(*playerStillExploding_)--;

			if (levelEndFxWait > 0)
			{
				levelEndFxWait--;
			} else {
				levelEndFxWait = (mt::rand() % 6) + 3;
				if ((mt::rand() % 3) == 1)
					soundQueue[6] = 9;
				else
					soundQueue[5] = 11;
			}
			tempW = *PX_ + (mt::rand() % 32) - 16;
			tempW2 = *PY_ + (mt::rand() % 32) - 16;

			JE_setupExplosionLarge(false, 0, *PX_ + (mt::rand() % 32) - 16, *PY_ + (rand() % 32) - 16 + 7);
			JE_setupExplosionLarge(false, 0, *PX_, *PY_ + 7);

			if (levelEnd > 0)
				levelEnd--;
		} else {

			if (twoPlayerMode || onePlayerAction)
			{
				if (portPower[playerNum_-1] > 1)
				{
					reallyEndLevel = false;
					shotMultiPos[playerNum_-1] = 0;
					portPower[playerNum_-1]--;
					JE_calcPurpleBall(playerNum_);
					twoPlayerLinked = false;
					if (galagaMode)
						twoPlayerMode = false;
					*PY_ = 160;
					*playerInvulnerable_ = 100;
					*playerAlive_ = true;
					endLevel = false;

					if (galagaMode || episodeNum == 4)
						*armorLevel_ = *baseArmor_;
					else
						*armorLevel_ = *baseArmor_ / 2;

					if (galagaMode)
						*shield_ = 0;
					else
						*shield_ = *shieldMax_ / 2;

					VGAScreen = VGAScreenSeg; /* side-effect of game_screen */
					JE_drawArmor();
					JE_drawShield();
					VGAScreen = game_screen; /* side-effect of game_screen */
					goto redo;
				} else {
					if (galagaMode)
						twoPlayerMode = false;
					if (allPlayersGone && netmanager)
						reallyEndLevel = true;
				}

			}
		}
	}
	else if (CVars::ch_constant_death)
	{
		if (*playerStillExploding_ == 0)
		{

			*shield_ = 0;
			if (*armorLevel_ > 0)
			{
				(*armorLevel_)--;
			}
			else
			{
				*playerAlive_ = false;
				*playerStillExploding_ = 60;
				levelEnd = 40;
			}

			JE_wipeShieldArmorBars();
			VGAScreen = VGAScreenSeg; /* side-effect of game_screen */
			JE_drawArmor();
			VGAScreen = game_screen; /* side-effect of game_screen */
		}
	}


	if (*playerAlive_)
	{  /*Global throughout entire remaining part of routine*/
		if (!endLevel)
		{

			*mouseX_ = *PX_;
			*mouseY_ = *PY_;
			button[1-1] = false;
			button[2-1] = false;
			button[3-1] = false;
			button[4-1] = false;

			/* --- Movement Routine Beginning --- */

			// TODO NETWORK if (!network::netmanager || playerNum_ == thisPlayerNum)
			if (true)
			{

				if (endLevel)
					*PY_ -= 2;
				else {
					service_SDL_events(false);
					/* ---- Mouse Input ---- */
					if (!playDemo)
					{
						/* Get Mouse Status */
						if (input_grabbed)
						{
							mouseXC = mouse_x - 159;
							mouseYC = mouse_y - 100;
						}

						if (( inputDevice_ == 2 || inputDevice_ == 0 )
						    //&& (!network::netmanager || playerNum_ == thisPlayerNum)
						    && (!galagaMode || (playerNum_ == 2 || !twoPlayerMode || playerStillExploding2 > 0)))
							JE_setMousePosition(159, 100);

					}

					memcpy(tempButton, button, sizeof(button));

					/* End of Mouse Routine */

					/* ==== Joystick Input ==== */
					if ((inputDevice_ == 3 || inputDevice_ == 0)
						&& joystick_installed && !playDemo && !CVars::record_demo)
					{  /* Start of Joystick Routine */
						JE_joystick1();
						JE_updateButtons();

						button[1-1] = button[1-1] | tempButton[1-1];
						button[2-1] = button[2-1] | tempButton[2-1];
						button[3-1] = button[3-1] | tempButton[3-1];

						if (joyY < -32768 / 2)
							*PY_ -= CURRENT_KEY_SPEED;
						if (joyY > 32767 / 2)
							*PY_ += CURRENT_KEY_SPEED;

						if (joyX < -32768 / 2)
							*PX_ -= CURRENT_KEY_SPEED;
						if (joyX > 32767 / 2)
							*PX_ += CURRENT_KEY_SPEED;
					}


					/* ---- Keyboard Input ---- */
					if (inputDevice_ == 1 || inputDevice_ == 0)
					{

						if (playDemo)
						{
							if (recordFile.eof())
							{
								endLevel = true;
								levelEnd = 40;
							} else {

								JE_doDemoKeys();
								if (lastMoveWait > 1)
									lastMoveWait--;
								else {
									JE_readDemoKeys();
								}

							}

						} else {
							if (gameInputEnabled[UP])
								*PY_ -= CURRENT_KEY_SPEED;
							if (gameInputEnabled[DOWN])
								*PY_ += CURRENT_KEY_SPEED;

							if (gameInputEnabled[LEFT])
								*PX_ -= CURRENT_KEY_SPEED;
							if (gameInputEnabled[RIGHT])
								*PX_ += CURRENT_KEY_SPEED;

							if (gameInputEnabled[FIRE])
								button[0] = true;
							if (gameInputEnabled[CHANGE_FIRE])
								button[3] = true;
							if (gameInputEnabled[LEFT_SIDEKICK])
								button[1] = true;
							if (gameInputEnabled[RIGHT_SIDEKICK])
								button[2] = true;
						}

						if (CVars::ch_constant_play)
						{
							button[1-1] = true;
							button[2-1] = true;
							button[3-1] = true;
							button[4-1] = true;
							(*PY_)++;
							*PX_ += constantLastX;
						}


						if (CVars::record_demo)
						{
							tempB = false;
							for (int temp = 0; temp < 8; temp++)
							{
								if (0) //lastKey[temp] != keysactive[keySettings[temp]])
								{
									tempB = true;
								}
							}

							lastMoveWait++;
							if (tempB)
							{
								recordFile.put(lastMoveWait >> 8);
								recordFile.put(lastMoveWait & 0xff);

								int temp;
								for (temp = 0; temp < 8; temp++)
									;//lastKey[temp] = keysactive[keySettings[temp]];
								temp = (lastKey[1-1]     ) + (lastKey[2-1] << 1) + (lastKey[3-1] << 2) + (lastKey[4-1] << 3) +
								       (lastKey[5-1] << 4) + (lastKey[6-1] << 5) + (lastKey[7-1] << 6) + (lastKey[8-1] << 7);

								recordFile.put(temp);

								lastMoveWait = 0;
							}
						}
					}

					if (smoothies[9-1])
					{
						*mouseY_ = *PY_ - (*mouseY_ - *PY_);
						mouseYC = -mouseYC;
					}

					accelXC += *PX_ - *mouseX_;
					accelYC += *PY_ - *mouseY_;

					if (mouseXC > 30)
						mouseXC = 30;
					else
						if (mouseXC < -30)
							mouseXC = -30;
					if (mouseYC > 30)
						mouseYC = 30;
					else
						if (mouseYC < -30)
							mouseYC = -30;

					if (mouseXC > 0)
						*PX_ += (mouseXC + 3 ) / 4;
					else
						if (mouseXC < 0)
							*PX_ += (mouseXC - 3 ) / 4;
					if (mouseYC > 0)
						*PY_ += (mouseYC + 3 ) / 4;
					else
						if (mouseYC < 0)
							*PY_ += (mouseYC - 3 ) / 4;

					if (makeMouseDelay)
					{
						if (mouseXC > 3)
							accelXC++;
						else
							if (mouseXC < -2)
								accelXC--;
						if (mouseYC > 2)
							accelYC++;
						else
							if (mouseYC < -2)
								accelYC--;
					}

				}   /*endLevel*/

				/* TODO NETWORK
				if (network::netmanager && playerNum_ == thisPlayerNum)
				{
					Uint16 buttons = 0;
					for (int i = 4-1; i >= 0; --i)
					{
						buttons <<= 1;
//						buttons = button || button[i];
					}

					SDLNet_Write16(*PX_ - *mouseX_, network::packet_state_out[0]->data+4);
					SDLNet_Write16(*PY_ - *mouseY_, network::packet_state_out[0]->data+6);
					SDLNet_Write16(accelXC, network::packet_state_out[0]->data+8);
					SDLNet_Write16(accelYC, network::packet_state_out[0]->data+10);
					SDLNet_Write16(buttons, network::packet_state_out[0]->data+12);

					*PX_ = *mouseX_;
					*PY_ = *mouseY_;

					for (int i = 0; i < 4; ++i)
						button[i] = false;

					accelXC = 0;
					accelYC = 0;
				} */
			}  /*network::netmanager*/

			/* --- Movement Routine Ending --- */

			/* TODO NETWORK if (network::netmanager && !network::state_is_reset())
			{
				if (playerNum_ != thisPlayerNum)
				{
					if (thisPlayerNum == 2)
						difficultyLevel = SDLNet_Read16(network::packet_state_in[0]->data+16);

					Uint16 buttons = SDLNet_Read16(network::packet_state_in[0]->data+12);
					for (int i = 0; i < 4; ++i)
					{
						button[i] = buttons & 1;
						buttons >>= 1;
					}

					*PX_ += static_cast<Sint16>(SDLNet_Read16(network::packet_state_in[0]->data+4));
					*PY_ += static_cast<Sint16>(SDLNet_Read16(network::packet_state_in[0]->data+6));
					accelXC = static_cast<Sint16>(SDLNet_Read16(network::packet_state_in[0]->data+8));
					accelYC = static_cast<Sint16>(SDLNet_Read16(network::packet_state_in[0]->data+10));
				}
				else
				{
					Uint16 buttons = SDLNet_Read16(network::packet_state_out[network::delay]->data+12);
					for (int i = 0; i < 4; ++i)
					{
						button[i] = buttons & 1;
						buttons >>= 1;
					}

					*PX_ += static_cast<Sint16>(SDLNet_Read16(network::packet_state_out[network::delay]->data+4));
					*PY_ += static_cast<Sint16>(SDLNet_Read16(network::packet_state_out[network::delay]->data+6));
					accelXC = static_cast<Sint16>(SDLNet_Read16(network::packet_state_out[network::delay]->data+8));
					accelYC = static_cast<Sint16>(SDLNet_Read16(network::packet_state_out[network::delay]->data+10));
				}
			}*/

			/*Street-Fighter codes*/
			JE_SFCodes(playerNum_, *PX_, *PY_, *mouseX_, *mouseY_, pItems_);

			/* END OF MOVEMENT ROUTINES */

			/*Linking Routines*/

			if (twoPlayerMode && !twoPlayerLinked && *PX_ == *mouseX_ && *PY_ == *mouseY_
			    && abs(PX - PXB) < 8 && abs(PY - PYB) < 8
			    && playerAlive && playerAliveB && !galagaMode)
			{
				twoPlayerLinked = true;
			}

			if (playerNum_ == 1 && (button[3-1] || button[2-1]) && !galagaMode)
				twoPlayerLinked = false;

			if (twoPlayerMode && twoPlayerLinked && playerNum_ == 2
			    && (*PX_ != *mouseX_ || *PY_ != *mouseY_))
			{

				if (button[1-1])
				{
					if (abs(*PX_ - *mouseX_) > abs(*PY_ - *mouseY_))
					{
						if (*PX_ - *mouseX_ > 0)
							tempR = 1.570796f;
						else
							tempR = 4.712388f;
					} else {
						if (*PY_ - *mouseY_ > 0)
							tempR = 0;
						else
							tempR = 3.14159265f;
					}

					tempR2 = linkGunDirec - tempR;

					if (fabs(linkGunDirec - tempR) < 0.3)
						linkGunDirec = tempR;
					else {

						if (linkGunDirec < tempR && linkGunDirec - tempR > -3.24)
							linkGunDirec += 0.2f;
						else
							if (linkGunDirec - tempR < M_PI)
								linkGunDirec -= 0.2f;
							else
								linkGunDirec += 0.2f;

					}

					if (linkGunDirec >= 2 * M_PI)
						linkGunDirec -= 2 * M_PI;
					if (linkGunDirec <  0)
						linkGunDirec += 2 * M_PI;

				} else
				if (!galagaMode)
					twoPlayerLinked = false;
			}

			leftOptionIsSpecial  = options[option1Item].tr;
			rightOptionIsSpecial = options[option2Item].tr;
		} /*if (!endLevel) ...*/


		if (levelEnd > 0 &&
		    !*playerAlive_ && (!twoPlayerMode || !playerAliveB))
			reallyEndLevel = true;
		/* End Level Fade-Out */
		if (*playerAlive_ && endLevel)
		{

			if (levelEnd == 0)
				reallyEndLevel = true;
			else {
				*PY_ -= levelEndWarp;
				if (*PY_ < -200)
					reallyEndLevel = true;

				tempI = 1;
				tempW2 = *PY_;
				tempI2 = abs(41 - levelEnd);
				if (tempI2 > 20)
					tempI2 = 20;

				for (int z = 1; z <= tempI2; z++)
				{
					tempW2 += tempI;
					tempI++;
				}

				for (int z = 1; z <= tempI2; z++)
				{
					tempW2 -= tempI;
					tempI--;
					if (tempW2 > 0 && tempW2 < 170)
					{
						if (shipGr_ == 0)
						{
							JE_drawShape2x2(*PX_ - 17, tempW2 - 7, 13, shapes9ptr_);
							JE_drawShape2x2(*PX_ + 7 , tempW2 - 7, 51, shapes9ptr_);
						} else
							if (shipGr_ == 1)
							{
								JE_drawShape2x2(*PX_ - 17, tempW2 - 7, 220, shapes9ptr_);
								JE_drawShape2x2(*PX_ + 7 , tempW2 - 7, 222, shapes9ptr_);
							} else
								JE_drawShape2x2(*PX_ - 5, tempW2 - 7, shipGr_, shapes9ptr_);
					}
				}

			}
		}

		if (playDemo)
			JE_dString(115, 10, miscText[8-1], SMALL_FONT_SHAPES);

		if (*playerAlive_ && !endLevel)
		{

			if (!twoPlayerLinked || playerNum_ < 2)
			{

				if (!twoPlayerMode || shipGr2 != 0)
				{
					option1X = *mouseX_ - 14;
					option1Y = *mouseY_;

					if (rightOptionIsSpecial == 0)
					{
						option2X = *mouseX_ + 16;
						option2Y = *mouseY_;
					}
				}

				if (*stopWaitX_ > 0)
					(*stopWaitX_)--;
				else {
					*stopWaitX_ = 2;
					if (*lastTurn_ < 0)
						(*lastTurn_)++;
					else
						if (*lastTurn_ > 0)
							(*lastTurn_)--;
				}

				if (*stopWaitY_ > 0)
					(*stopWaitY_)--;
				else {
					*stopWaitY_ = 1;
					if (*lastTurn2_ < 0)
						(*lastTurn2_)++;
					else
						if (*lastTurn2_ > 0)
							(*lastTurn2_)--;
				}

				*lastTurn_ += accelYC;
				*lastTurn2_ += accelXC;

				if (*lastTurn2_ < -4)
					*lastTurn2_ = -4;
				if (*lastTurn2_ > 4)
					*lastTurn2_ = 4;
				if (*lastTurn_ < -4)
					*lastTurn_ = -4;
				if (*lastTurn_ > 4)
					*lastTurn_ = 4;

				*PX_ += *lastTurn2_;
				*PY_ += *lastTurn_;


				/*Option History for special new sideships*/
				if (playerHNotReady)
					JE_resetPlayerH();
				else
					if ((playerNum_ == 1 && !twoPlayerMode)
					    || (playerNum_ == 2 && twoPlayerMode))
						if (*PX_ - *mouseX_ != 0 || *PY_ - *mouseY_ != 0)
						{ /*Option History*/
							for (int temp = 0; temp < 19; temp++)
							{
								playerHX[temp] = playerHX[temp + 1];
								playerHY[temp] = playerHY[temp + 1];
							}
							playerHX[20-1] = *PX_;
							playerHY[20-1] = *PY_;
						}

			} else {  /*twoPlayerLinked*/
				if (shipGr_ == 0)
					*PX_ = PX - 1;
				else
					*PX_ = PX;
				*PY_ = PY + 8;
				*lastTurn2_ = lastTurn2;
				*lastTurn_ = 4;

				shotMultiPos[5-1] = 0;
				JE_initPlayerShot(0, 5, JE_word(*PX_ + 1 + ot_round(sin(linkGunDirec + 0.2f) * 26)), JE_word(*PY_ + ot_round(cos(linkGunDirec + 0.2f) * 26)),
				                  *mouseX_, *mouseY_, 148, playerNum_);
				shotMultiPos[5-1] = 0;
				JE_initPlayerShot(0, 5, JE_word(*PX_ + 1 + ot_round(sin(linkGunDirec - 0.2f) * 26)), JE_word(*PY_ + ot_round(cos(linkGunDirec - 0.2f) * 26)),
				                  *mouseX_, *mouseY_, 148, playerNum_);
				shotMultiPos[5-1] = 0;
				JE_initPlayerShot(0, 5, JE_word(*PX_ + 1 + ot_round(sin(linkGunDirec) * 26)), JE_word(*PY_ + ot_round(cos(linkGunDirec) * 26)),
				                  *mouseX_, *mouseY_, 147, playerNum_);

				if (shotRepeat[2-1] > 0)
					shotRepeat[2-1]--;
				else
					if (button[1-1])
					{
						shotMultiPos[2-1] = 0;
						JE_initPlayerShot(0, 2, JE_word(*PX_ + 1 + ot_round(sin(linkGunDirec) * 20)), JE_word(*PY_ + ot_round(cos(linkGunDirec) * 20)),
						                  *mouseX_, *mouseY_, linkGunWeapons[pItems_[PITEM_REAR_WEAPON]-1], playerNum_);
						playerShotData[b].shotXM = -ot_round(sin(linkGunDirec) * playerShotData[b].shotYM);
						playerShotData[b].shotYM = -ot_round(cos(linkGunDirec) * playerShotData[b].shotYM);

						switch (pItems_[PITEM_REAR_WEAPON])
						{
							case 27:
							case 32:
							case 10:
								temp = ot_round(linkGunDirec * 2.54647908948f);  /*16 directions*/
								playerShotData[b].shotGr = linkMultiGr[temp];
								break;
							case 28:
							case 33:
							case 11:
								temp = ot_round(linkGunDirec * 2.54647908948f);  /*16 directions*/
								playerShotData[b].shotGr = linkSonicGr[temp];
								break;
							case 30:
							case 35:
							case 14:
								if (linkGunDirec > M_PI_2 && linkGunDirec < M_PI + M_PI / 2)
								{
									playerShotData[b].shotYC = 1;
								}
								break;
							case 38:
							case 22:
								temp = ot_round(linkGunDirec * 2.54647908948f);  /*16 directions*/
								playerShotData[b].shotGr = linkMult2Gr[temp];
								break;
						}

					}
			}

		}  /*moveOK*/

		if (!endLevel)
		{
			if (*PX_ > 256)
			{
				*PX_ = 256;
				constantLastX = -constantLastX;
			}
			if (*PX_ < 40)
			{
				*PX_ = 40;
				constantLastX = -constantLastX;
			}

			if (netmanager && playerNum_ == 1)
			{
				if (*PY_ > 154)
					*PY_ = 154;
			} else {
				if (*PY_ > 160)
					*PY_ = 160;
			}

			if (*PY_ < 10)
				*PY_ = 10;

			tempI2 = *lastTurn2_ / 2;
			tempI2 += (*PX_ - *mouseX_) / 6;

			if (tempI2 < -2)
				tempI2 = -2;
			else
				if (tempI2 > 2)
					tempI2 = 2;


			tempI  = tempI2 * 2 + shipGr_;

			tempI4 = *PY_ - *lastPY2_;
			if (tempI4 < 1)
				tempI4 = 0;
			player_delta_x[playerNum_-1] = *PX_ - *lastPX2_;
			player_delta_y[playerNum_-1] = tempI4;
			*lastPX2_ = *PX_;
			*lastPY2_ = *PY_;

			if (CVars::r_background2 || background2over == 3) {
				if (shipGr_ == 0)
				{
					JE_drawShape2x2Shadow(*PX_ - 17 - mapX2Ofs + 30, *PY_ - 7 + shadowyDist, tempI + 13, shapes9ptr_);
					JE_drawShape2x2Shadow(*PX_ + 7 - mapX2Ofs + 30, *PY_ - 7 + shadowyDist, tempI + 51, shapes9ptr_);
					if (CVars::r_wild)
					{
						JE_drawShape2x2Shadow(*PX_ - 16 - mapX2Ofs + 30, *PY_ - 7 + shadowyDist, tempI + 13, shapes9ptr_);
						JE_drawShape2x2Shadow(*PX_ + 6 - mapX2Ofs + 30, *PY_ - 7 + shadowyDist, tempI + 51, shapes9ptr_);
					}
				} else {
					if (shipGr_ == 1)
					{
						JE_drawShape2x2Shadow(*PX_ - 17 - mapX2Ofs + 30, *PY_ - 7 + shadowyDist, 220, shapes9ptr_);
						JE_drawShape2x2Shadow(*PX_ + 7 - mapX2Ofs + 30, *PY_ - 7 + shadowyDist, 222, shapes9ptr_);
					} else {
						JE_drawShape2x2Shadow(*PX_ - 5 - mapX2Ofs + 30, *PY_ - 7 + shadowyDist, tempI, shapes9ptr_);
						if (CVars::r_wild)
						{
							JE_drawShape2x2Shadow(*PX_ - 4 - mapX2Ofs + 30, *PY_ - 7 + shadowyDist, tempI, shapes9ptr_);
						}
					}
				}
			}

			if (*playerInvulnerable_ > 0)
			{
				(*playerInvulnerable_)--;

				if (shipGr_ == 0)
				{
					JE_superDrawShape2x2(*PX_ - 17, *PY_ - 7, tempI + 13, shapes9ptr_);
					JE_superDrawShape2x2(*PX_ + 7 , *PY_ - 7, tempI + 51, shapes9ptr_);
				} else
					if (shipGr_ == 1)
					{
						JE_superDrawShape2x2(*PX_ - 17, *PY_ - 7, 220, shapes9ptr_);
						JE_superDrawShape2x2(*PX_ + 7 , *PY_ - 7, 222, shapes9ptr_);
					} else
						JE_superDrawShape2x2(*PX_ - 5, *PY_ - 7, tempI, shapes9ptr_);

			} else {
				if (shipGr_ == 0)
				{
					JE_drawShape2x2(*PX_ - 17, *PY_ - 7, tempI + 13, shapes9ptr_);
					JE_drawShape2x2(*PX_ + 7, *PY_ - 7, tempI + 51, shapes9ptr_);
				} else
					if (shipGr_ == 1)
					{
						JE_drawShape2x2(*PX_ - 17, *PY_ - 7, 220, shapes9ptr_);
						JE_drawShape2x2(*PX_ + 7, *PY_ - 7, 222, shapes9ptr_);
						switch (tempI)
						{
							case 5:
								JE_drawShape2(*PX_ - 17, *PY_ + 7, 40, shapes9ptr_);
								tempW = *PX_ - 7;
								tempI2 = -2;
								break;
							case 3:
								JE_drawShape2(*PX_ - 17, *PY_ + 7, 39, shapes9ptr_);
								tempW = *PX_ - 7;
								tempI2 = -1;
								break;
							case 1:
								tempI2 = 0;
								break;
							case -1:
								JE_drawShape2(*PX_ + 19, *PY_ + 7, 58, shapes9ptr_);
								tempW = *PX_ + 9;
								tempI2 = 1;
								break;
							case -3:
								JE_drawShape2(*PX_ + 19, *PY_ + 7, 59, shapes9ptr_);
								tempW = *PX_ + 9;
								tempI2 = 2;
								break;
						}
						if (tempI2 != 0)
						{  /*NortSparks*/
							if (shotRepeat[10-1] > 0)
								shotRepeat[10-1]--;
							else {
									JE_initPlayerShot(0, 10, tempW + (mt::rand() % 8) - 4, (*PY_) + (rand() % 8) - 4, *mouseX_, *mouseY_, 671, 1);
									shotRepeat[10-1] = abs(tempI2) - 1;
							}
						}
					} else
						JE_drawShape2x2(*PX_ - 5, *PY_ - 7, tempI, shapes9ptr_);
			}
		}  /*endLevel*/

		/*Options Location*/
		if (playerNum_ == 2 && shipGr_ == 0)
		{
			if (rightOptionIsSpecial == 0)
			{
				option2X = *PX_ + 17 + tempI;
				option2Y = *PY_;
			}

			option1X = *PX_ - 14 + tempI;
			option1Y = *PY_;
		}

		if (*playerAlive_)
		{
			if (!endLevel)
			{
				*PXChange_ = *PX_ - lastPXShotMove;
				*PYChange_ = *PY_ - lastPYShotMove;

				/* PLAYER SHOT Change */
				if (button[4-1])
				{
					// TODO NETWORK
					portConfigChange = true;
					if (portConfigDone)
					{

						shotMultiPos[2-1] = 0;

						if (superArcadeMode > 0 && superArcadeMode <= SA)
						{
							shotMultiPos[9-1] = 0;
							shotMultiPos[11-1] = 0;
							if (pItems[PITEM_SPECIAL] == SASpecialWeapon[superArcadeMode-1])
							{
								pItems[PITEM_SPECIAL] = SASpecialWeaponB[superArcadeMode-1];
								portConfig[2-1] = 2;
							} else {
								pItems[PITEM_SPECIAL] = SASpecialWeapon[superArcadeMode-1];
								portConfig[2-1] = 1;
							}
						} else {
							portConfig[2-1]++;
							JE_portConfigs();
							if (portConfig[2-1] > tempW)
							{
								portConfig[2-1] = 1;
							}

						}

						JE_drawPortConfigButtons();
						portConfigDone = false;
					}
				}

				/* PLAYER SHOT Creation */

				/*SpecialShot*/
				if (!galagaMode)
					JE_doSpecialShot(playerNum_, armorLevel_, shield_);

				/*Normal Main Weapons*/
				if (!(twoPlayerLinked && playerNum_ == 2))
				{
					if (!twoPlayerMode)
					{
						min = 1;
						max = 2;
					} else
						switch (playerNum_)
						{
							case 1:
								min = 1;
								max = 1;
								break;
							case 2:
								min = 2;
								max = 2;
								break;
						}
						for (int temp = min - 1; temp < max; temp++)
							if (pItems_[temp] > 0)
							{
								if (shotRepeat[temp] > 0)
									shotRepeat[temp]--;
								else
									if (button[1-1])
										JE_initPlayerShot(pItems_[temp], temp + 1, *PX_, *PY_, *mouseX_, *mouseY_,
										                  weaponPort[pItems_[temp]].op[portConfig[temp]-1]
										                  [portPower[temp] * !galagaMode + galagaMode - 1],
										                  playerNum_);
							}
				}

				/*Super Charge Weapons*/
				if (playerNum_ == 2)
				{

					if (!twoPlayerLinked)
						JE_drawShape2(*PX_ + (shipGr_ == 0) + 1, *PY_ - 13, 77 + chargeLevel + chargeGr * 19, eShapes6);

					if (chargeGrWait > 0)
					{
						chargeGrWait--;
					} else {
						chargeGr++;
						if (chargeGr == 4)
							chargeGr = 0;
						chargeGrWait = 3;
					}

					if (chargeLevel > 0)
					{
						JE_c_bar(269, 107 + (chargeLevel - 1) * 3, 275, 108 + (chargeLevel - 1) * 3, 193);
					}

					if (chargeWait > 0)
					{
						chargeWait--;
					} else {
						if (chargeLevel < chargeMax)
							chargeLevel++;
						chargeWait = 28 - portPower[2-1] * 2;
						if (difficultyLevel > 3)
							chargeWait -= 5;
					}

					if (chargeLevel > 0)
					{
						JE_c_bar(269, 107 + (chargeLevel - 1) * 3, 275, 108 + (chargeLevel - 1) * 3, 204);
					}

					if (shotRepeat[6-1] > 0)
						shotRepeat[6-1]--;
					else
						if (button[1-1]
						    && (!twoPlayerLinked || chargeLevel > 0))
						{
							shotMultiPos[6-1] = 0;
							JE_initPlayerShot(16, 6, *PX_, *PY_, *mouseX_, *mouseY_,
							                  chargeGunWeapons[pItemsPlayer2[PITEM_REAR_WEAPON]-1] + chargeLevel, playerNum_);

							if (chargeLevel > 0)
							{
								JE_c_bar(269, 107 + (chargeLevel - 1) * 3, 275, 108 + (chargeLevel - 1) * 3, 193);
							}

							chargeLevel = 0;
							chargeWait = 30 - portPower[2-1] * 2;
						}
				}


				/*SUPER BOMB*/
				temp = playerNum_;
				if (temp == 0)
					temp = 1;  /*Get whether player 1 or 2*/

				if (superBomb[temp-1] > 0)
				{
					if (shotRepeat[temp-1 + 6] > 0)
						shotRepeat[temp-1 + 6]--;
					else {
						if (button[3-1] || button[2-1])
						{
							superBomb[temp-1]--;
							shotMultiPos[temp-1 + 6] = 0;
							JE_initPlayerShot(16, temp + 6, *PX_, *PY_, *mouseX_, *mouseY_, 535, playerNum_);
						}
					}
				}


				/*Special option following*/
				switch (leftOptionIsSpecial)
				{
					case 1:
					case 3:
						option1X = playerHX[10-1];
						option1Y = playerHY[10-1];
						break;
					case 2:
						option1X = *PX_;
						option1Y = *PY_ - 20;
						if (option1Y < 10)
							option1Y = 10;
						break;
					case 4:
						if (rightOptionIsSpecial == 4)
							optionSatelliteRotate += 0.2f;
						else
							optionSatelliteRotate += 0.15f;
						option1X = *PX_ + ot_round(sin(optionSatelliteRotate) * 20);
						option1Y = *PY_ + ot_round(cos(optionSatelliteRotate) * 20);
						break;
				}


				switch (rightOptionIsSpecial)
				{
                    case 4:
						if (leftOptionIsSpecial != 4)
							optionSatelliteRotate += 0.15f;
						option2X = *PX_ - ot_round(sin(optionSatelliteRotate) * 20);
						option2Y = *PY_ - ot_round(cos(optionSatelliteRotate) * 20);
						break;
					case 1:
					case 3:
						option2X = playerHX[1-1];
						option2Y = playerHY[1-1];
						break;
					case 2:
						if (!optionAttachmentLinked)
						{
							option2Y += optionAttachmentMove / 2;
							if (optionAttachmentMove >= -2)
							{

								if (optionAttachmentReturn)
									temp = 2;
								else
									temp = 0;
								if (option2Y > (*PY_ - 20) + 5)
								{
									temp = 2;
									optionAttachmentMove -= 1 + optionAttachmentReturn;
								} else
									if (option2Y > (*PY_ - 20) - 0)
									{
										temp = 3;
										if (optionAttachmentMove > 0)
											optionAttachmentMove--;
										else
											optionAttachmentMove++;
									} else
										if (option2Y > (*PY_ - 20) - 5)
										{
											temp = 2;
											optionAttachmentMove++;
										} else
											if (optionAttachmentMove < 2 + optionAttachmentReturn * 4)
												optionAttachmentMove += 1 + optionAttachmentReturn;

								if (optionAttachmentReturn)
									temp = temp * 2;
								if (abs(option2X - *PX_ < temp))
									temp = 1;

								if (option2X > *PX_)
									option2X -= temp;
								else
									if (option2X < *PX_)
										option2X += temp;

								if (abs(option2Y - (*PY_ - 20)) + abs(option2X - *PX_) < 8)
								{
									optionAttachmentLinked = true;
									soundQueue[3-1] = 23;
								}

								if (button[3-1])
									optionAttachmentReturn = true;
							} else {
								optionAttachmentMove += 1 + optionAttachmentReturn;
								JE_setupExplosion(option2X + 1, option2Y + 10, 0);
							}

						} else {
							option2X = *PX_;
							option2Y = *PY_ - 20;
							if (button[3-1])
							{
								optionAttachmentLinked = false;
								optionAttachmentReturn = false;
								optionAttachmentMove = -20;
								soundQueue[3] = 26;
							}
						}

						if (option2Y < 10)
							option2Y = 10;
						break;
				}

				if (playerNum_ == 2 || !twoPlayerMode)
				{
					if (options[option1Item].wport > 0)
					{
						if (shotRepeat[3-1] > 0)
							shotRepeat[3-1]--;
						else {

							if (option1Ammo >= 0)
							{
								if (option1AmmoRechargeWait > 0)
									option1AmmoRechargeWait--;
								else {
									option1AmmoRechargeWait = option1AmmoRechargeWaitMax;
									if (option1Ammo < options[option1Item].ammo)
										option1Ammo++;
									JE_barDrawDirect (284, option1Draw + 13, option1AmmoMax, 112, option1Ammo, 2, 2); /*Option1Ammo*/
								}
							}

							if (option1Ammo > 0)
							{
								if (button[2-1])
								{
									JE_initPlayerShot(options[option1Item].wport, 3,
													option1X, option1Y,
													*mouseX_, *mouseY_,
													options[option1Item].wpnum + optionCharge1,
													playerNum_);
									if (optionCharge1 > 0)
										shotMultiPos[3-1] = 0;
									optionAni1Go = true;
									optionCharge1Wait = 20;
									optionCharge1 = 0;
									option1Ammo--;
									JE_c_bar(284, option1Draw + 13, 312, option1Draw + 15, 0);
									JE_barDrawDirect(284, option1Draw + 13, option1AmmoMax, 112, option1Ammo, 2, 2);
								}
							} else
								if (option1Ammo < 0)
								{
									if (button[1-1] || button[2-1])
									{
										JE_initPlayerShot(options[option1Item].wport, 3,
														option1X, option1Y,
														*mouseX_, *mouseY_,
														options[option1Item].wpnum + optionCharge1,
														playerNum_);
										if (optionCharge1 > 0)
											shotMultiPos[3-1] = 0;
										optionCharge1Wait = 20;
										optionCharge1 = 0;
										optionAni1Go = true;
									}
								}
						}
					}

					if (options[option2Item].wport > 0)
					{
						if (shotRepeat[4-1] > 0)
							shotRepeat[4-1]--;
						else {

							if (option2Ammo >= 0)
							{
								if (option2AmmoRechargeWait > 0)
									option2AmmoRechargeWait--;
								else {
									option2AmmoRechargeWait = option2AmmoRechargeWaitMax;
									if (option2Ammo < options[option2Item].ammo)
										option2Ammo++;
									JE_barDrawDirect(284, option2Draw + 13, option2AmmoMax, 112, option2Ammo, 2, 2);
								}
							}

							if (option2Ammo > 0)
							{
								if (button[3-1])
								{
									JE_initPlayerShot(options[option2Item].wport, 4, option2X, option2Y,
													*mouseX_, *mouseY_,
													options[option2Item].wpnum + optionCharge2,
													playerNum_);
									if (optionCharge2 > 0)
									{
										shotMultiPos[4-1] = 0;
										optionCharge2 = 0;
									}
									optionCharge2Wait = 20;
									optionCharge2 = 0;
									optionAni2Go = true;
									option2Ammo--;
									JE_c_bar(284, option2Draw + 13, 312, option2Draw + 15, 0);
									JE_barDrawDirect(284, option2Draw + 13, option2AmmoMax, 112, option2Ammo, 2, 2);
								}
							} else
								if (option2Ammo < 0)
									if (button[1-1] || button[3-1])
									{
										JE_initPlayerShot(options[option2Item].wport, 4, option2X, option2Y,
													*mouseX_, *mouseY_,
													options[option2Item].wpnum + optionCharge2,
													playerNum_);
										if (optionCharge2 > 0)
										{
											shotMultiPos[4-1] = 0;
											optionCharge2 = 0;
										}
										optionCharge2Wait = 20;
										optionAni2Go = true;
									}
						}
					}
				}
			}  /* !endLevel */
		}

		/* Draw Floating Options */
		if ((playerNum_ == 2 || !twoPlayerMode) && !endLevel)
		{
			if (options[option1Item].option > 0)
			{

				if (optionAni1Go)
				{
					optionAni1++;
					if (optionAni1 > options[option1Item].ani)
					{
						optionAni1 = 1;
						optionAni1Go = options[option1Item].option == 1;
					}
				}

				if (leftOptionIsSpecial == 1 || leftOptionIsSpecial == 2)
					JE_drawShape2x2(option1X - 6, option1Y, options[option1Item].gr[optionAni1-1] + optionCharge1,
					                eShapes6);
				else
					JE_drawShape2(option1X, option1Y, options[option1Item].gr[optionAni1-1] + optionCharge1,
					              shapes9);
			}

			if (options[option2Item].option > 0)
			{

				if (optionAni2Go)
				{
					optionAni2++;
					if (optionAni2 > options[option2Item].ani)
					{
						optionAni2 = 1;
						optionAni2Go = options[option2Item].option == 1;
					}
				}

				if (rightOptionIsSpecial == 1 || rightOptionIsSpecial == 2)
					JE_drawShape2x2(option2X - 6, option2Y, options[option2Item].gr[optionAni2-1] + optionCharge2,
					                eShapes6);
				else
					JE_drawShape2(option2X, option2Y, options[option2Item].gr[optionAni2-1] + optionCharge2,
					              shapes9);
			}

			optionCharge1Wait--;
			if (optionCharge1Wait == 0)
			{
				if (optionCharge1 < options[option1Item].pwr)
					optionCharge1++;
				optionCharge1Wait = 20;
			}
			optionCharge2Wait--;
			if (optionCharge2Wait == 0)
			{
				if (optionCharge2 < options[option2Item].pwr)
					optionCharge2++;
				optionCharge2Wait = 20;
			}
		}
	} else {
		player_delta_x[playerNum_] = 0;
		player_delta_y[playerNum_] = 0;
	}
}

void JE_mainGamePlayerFunctions( void )
{
	/*PLAYER MOVEMENT/MOUSE ROUTINES*/

	useButtonAssign = true;

	if (endLevel && levelEnd > 0)
	{
		levelEnd--;
		levelEndWarp++;
	}

	/*Reset Street-Fighter commands*/
	memset(SFExecuted, 0, sizeof(SFExecuted));

	makeMouseDelay = true;
	portConfigChange = false;

	if (twoPlayerMode)
	{

		JE_playerMovement(playerDevice1 * !galagaMode, 1, shipGr, shipGrPtr,
		                  &armorLevel, &baseArmor,
		                  &shield, &shieldMax,
		                  &playerInvulnerable1,
		                  &PX, &PY, &lastPX, &lastPY, &lastPX2, &lastPY2, &PXChange, &PYChange,
		                  &lastTurn, &lastTurn2, &tempLastTurn2, &stopWaitX, &stopWaitY,
		                  &mouseX, &mouseY,
		                  &playerAlive, &playerStillExploding, pItems);
		JE_playerMovement(playerDevice2 * !galagaMode, 2, shipGr2, shipGr2ptr,
		                  &armorLevel2, &baseArmor2,
		                  &shield2, &shieldMax2,
		                  &playerInvulnerable2,
		                  &PXB, &PYB, &lastPXB, &lastPYB, &lastPX2B, &lastPY2B, &PXChangeB, &PYChangeB,
		                  &lastTurnB, &lastTurn2B, &tempLastTurn2B, &stopWaitXB, &stopWaitYB,
		                  &mouseXB, &mouseYB,
		                  &playerAliveB, &playerStillExploding2, pItemsPlayer2);
	} else {
		JE_playerMovement(0, 1, shipGr, shipGrPtr,
		                  &armorLevel, &baseArmor,
		                  &shield, &shieldMax,
		                  &playerInvulnerable1,
		                  &PX, &PY, &lastPX, &lastPY, &lastPX2, &lastPY2, &PXChange, &PYChange,
		                  &lastTurn, &lastTurn2, &tempLastTurn2, &stopWaitX, &stopWaitY,
		                  &mouseX, &mouseY,
		                  &playerAlive, &playerStillExploding, pItems);
	}

	/*-----Horizontal Map Scrolling-----*/
	if (twoPlayerMode)
	{
		tempX = (PX + PXB) / 2;
	} else {
		tempX = PX;
	}

	tempW = (JE_word)((260.0f - (tempX - 36.0f)) / (260.0f - 36.0f) * (24.0f * 3.0f) - 1.0f);
	mapX3Ofs   = tempW;
	mapX3Pos   = mapX3Ofs % 24;
	mapX3bpPos = 1 - (mapX3Ofs / 24);

	mapX2Ofs   = (tempW * 2) / 3;
	mapX2Pos   = mapX2Ofs % 24;
	mapX2bpPos = 1 - (mapX2Ofs / 24);

	oldMapXOfs = mapXOfs;
	mapXOfs    = mapX2Ofs / 2;
	mapXPos    = mapXOfs % 24;
	mapXbpPos  = 1 - (mapXOfs / 24);

	if (background3x1)
	{
		mapX3Ofs = mapXOfs;
		mapX3Pos = mapXPos;
		mapX3bpPos = mapXbpPos - 1;
	}
}

std::string JE_getName( int pnum )
{
	/* TODO NETWORK
	if (pnum == thisPlayerNum && !network::player_name.empty())
		return network::player_name;
	else if (!network::opponent_name.empty())
		return network::opponent_name;*/

	return miscText[49+pnum-1-1];
}

void JE_playerCollide( int *PX_, int *PY_, int *lastTurn_, int *lastTurn2_,
                       unsigned long *score_, int *armorLevel_, int *shield_, bool *playerAlive_,
                       int *playerStillExploding_, int playerNum_, int playerInvulnerable_ )
{
	std::string tempStr;
	
	for (int z = 0; z < 100; z++)
	{
		if (enemyAvail[z] != 1)
		{
			tempI3 = enemy[z].ex + enemy[z].mapoffset;
			
			if (abs(*PX_ - tempI3) < 12 && abs(*PY_ - enemy[z].ey) < 14)
			{   /*Collide*/
				tempI4 = enemy[z].evalue;
				if (tempI4 > 29999)
				{
					if (tempI4 == 30000)
					{
						*score_ += 100;
						
						if (!galagaMode)
						{
							if (purpleBallsRemaining[playerNum_ - 1] > 1)
								purpleBallsRemaining[playerNum_ - 1]--;
							else {
								JE_powerUp(playerNum_);
								JE_calcPurpleBall(playerNum_);
							}
						} else {
							if (twoPlayerMode)
								*score_ += 2400;
							twoPlayerMode = true;
							twoPlayerLinked = true;
							portPower[2-1] = 1;
							armorLevel2 = 10;
							playerAliveB = true;
						}
						enemyAvail[z] = 1;
						soundQueue[7] = 29;
					} else if (superArcadeMode > 0 && tempI4 > 30000) {
						shotMultiPos[1-1] = 0;
						shotRepeat[1-1] = 10;
						
						tempW = SAWeapon[superArcadeMode-1][tempI4 - 30000-1];
						
						if (tempW == pItems[PITEM_FRONT_WEAPON])
						{
							*score_ += 1000;
							if (portPower[1-1] < 11)
								JE_powerUp(1);
							JE_calcPurpleBall(playerNum_);
						} else if (purpleBallsRemaining[playerNum_-1] > 1)
							purpleBallsRemaining[playerNum_-1]--;
						else {
							JE_powerUp(playerNum_);
							JE_calcPurpleBall(playerNum_);
						}
						
						pItems[PITEM_FRONT_WEAPON] = tempW;
						*score_ += 200;
						soundQueue[7] = 29;
						enemyAvail[z] = 1;
					} else if (tempI4 > 32100) {
						if (playerNum_ == 1)
						{
							*score_ += 250;
							pItems[PITEM_SPECIAL] = tempI4 - 32100;
							shotMultiPos[9-1] = 0;
							shotRepeat[9-1] = 10;
							shotMultiPos[11-1] = 0;
							shotRepeat[11-1] = 0;
							
							if (netmanager)
							{
								tempStr = (boost::format("%1% %2% %3%") % JE_getName(1) % miscTextB[4-1] % special[tempI4 - 32100].name).str();
							} else if (twoPlayerMode) {
								tempStr = (boost::format("%1% %2%") % miscText[43-1] % special[tempI4 - 32100].name).str();
							} else {
								tempStr = (boost::format("%1% %2%") % miscText[64-1] % special[tempI4 - 32100].name).str();
							}
							JE_drawTextWindow(tempStr);
							soundQueue[7] = 29;
							enemyAvail[z] = 1;
						}
					} else if (tempI4 > 32000) {
						if (playerNum_ == 2)
						{
							enemyAvail[z] = 1;
							if (netmanager)
							{
								tempStr = (boost::format("%1% %2% %3%") % JE_getName(2) % miscTextB[4-1] % options[tempI4 - 32000].name).str();
							} else {
								tempStr = (boost::format("%1% %2%") % miscText[44-1] % options[tempI4 - 32000].name).str();
							}
							JE_drawTextWindow(tempStr);
							
							if (tempI4 - 32000 != pItemsPlayer2[PITEM_P2_SIDEKICK])
								pItemsPlayer2[PITEM_P2_SIDEKICK_UPGRADE] = 100;
							pItemsPlayer2[PITEM_P2_SIDEKICK] = tempI4 - 32000;
							
							if (pItemsPlayer2[PITEM_P2_SIDEKICK_UPGRADE] < 103)
								pItemsPlayer2[PITEM_P2_SIDEKICK_UPGRADE]++;
							
							temp = pItemsPlayer2[PITEM_P2_SIDEKICK_UPGRADE] - 100;
							pItemsPlayer2[PITEM_LEFT_SIDEKICK] = optionSelect[pItemsPlayer2[PITEM_P2_SIDEKICK]][temp-1][1-1];
							pItemsPlayer2[PITEM_RIGHT_SIDEKICK] = optionSelect[pItemsPlayer2[PITEM_P2_SIDEKICK]][temp-1][2-1];
							
							
							shotMultiPos[3-1] = 0;
							shotMultiPos[4-1] = 0;
							tempScreenSeg = VGAScreenSeg;
							JE_drawOptions();
							soundQueue[7] = 29;
							tempScreenSeg = VGAScreen;
						} else if (onePlayerAction) {
							enemyAvail[z] = 1;
							tempStr = (boost::format("%1% %2%") % miscText[64-1] % options[tempI4 - 32000].name).str();
							JE_drawTextWindow(tempStr);
							pItems[PITEM_LEFT_SIDEKICK] = tempI4 - 32000;
							pItems[PITEM_RIGHT_SIDEKICK] = tempI4 - 32000;
							shotMultiPos[3-1] = 0;
							shotMultiPos[4-1] = 0;
							tempScreenSeg = VGAScreenSeg;
							JE_drawOptions();
							soundQueue[7] = 29;
						}
						if (enemyAvail[z] == 1)
							*score_ += 250;
					} else if (tempI4 > 31000) {
						*score_ += 250;
						if (playerNum_ == 2)
						{
							if (netmanager)
							{
								tempStr = (boost::format("%1% %2% %3%") % JE_getName(2) % miscTextB[4-1] % weaponPort[tempI4 - 31000].name).str();
							} else {
								tempStr = (boost::format("%1% %2%") % miscText[44-1] % weaponPort[tempI4 - 31000].name).str();
							}
							JE_drawTextWindow(tempStr);
							pItemsPlayer2[PITEM_REAR_WEAPON] = tempI4 - 31000;
							shotMultiPos[2-1] = 0;
							enemyAvail[z] = 1;
							soundQueue[7] = 29;
						} else if (onePlayerAction) {
							tempStr = (boost::format("%1% %2%") % miscText[64-1] % weaponPort[tempI4 - 31000].name).str();
							JE_drawTextWindow(tempStr);
							pItems[PITEM_REAR_WEAPON] = tempI4 - 31000;
							shotMultiPos[2] = 0;
							enemyAvail[z] = 1;
							soundQueue[7] = 29;
							if (portPower[2-1] == 0)
								portPower[2-1] = 1;
						}
					} else if (tempI4 > 30000) {
						
						if (playerNum_ == 1 && twoPlayerMode)
						{
							if (netmanager)
							{
								tempStr = (boost::format("%1% %2% %3%") % JE_getName(1) % miscTextB[4-1] % weaponPort[tempI4 - 30000].name).str();
							} else {
								tempStr = (boost::format("%1% %2%") % miscText[43-1] % weaponPort[tempI4 - 30000].name).str();
							}
							JE_drawTextWindow(tempStr);
							pItems[PITEM_FRONT_WEAPON] = tempI4 - 30000;
							shotMultiPos[1-1] = 0;
							enemyAvail[z] = 1;
							soundQueue[7] = 29;
						} else if (onePlayerAction) {
							tempStr = (boost::format("%1% %2%") % miscText[64-1] % weaponPort[tempI4 - 30000].name).str();
							JE_drawTextWindow(tempStr);
							pItems[PITEM_FRONT_WEAPON] = tempI4 - 30000;
							shotMultiPos[1-1] = 0;
							enemyAvail[z] = 1;
							soundQueue[7] = 29;
						}
						
						if (enemyAvail[z] == 1)
						{
							pItems[PITEM_SPECIAL] = specialArcadeWeapon[tempI4 - 30000-1];
							if (pItems[PITEM_SPECIAL] > 0)
							{
								shotMultiPos[9-1] = 0;
								shotRepeat[9-1] = 0;
								shotMultiPos[11-1] = 0;
								shotRepeat[11-1] = 0;
							}
							*score_ += 250;
						}
						
					}
				} else if (tempI4 > 20000) {
					if (twoPlayerLinked)
					{
						armorLevel += (tempI4 - 20000) / 2;
						if (armorLevel > 28)
							armorLevel = 28;
						armorLevel2 += (tempI4 - 20000) / 2;
						if (armorLevel2 > 28)
							armorLevel2 = 28;
					} else {
						*armorLevel_ += tempI4 - 20000;
						if (*armorLevel_ > 28)
							*armorLevel_ = 28;
					}
					enemyAvail[z] = 1;
					VGAScreen = VGAScreenSeg; /* side-effect of game_screen */
					JE_drawArmor();
					VGAScreen = game_screen; /* side-effect of game_screen */
					soundQueue[7] = 29;
				} else if (tempI4 > 10000 && enemyAvail[z] == 2) {
					if (!bonusLevel)
					{
						JE_playSong(31);  /*Zanac*/
						bonusLevel = true;
						nextLevel = tempI4 - 10000;
						enemyAvail[z] = 1;
						displayTime = 150;
					}
				} else if (enemy[z].scoreitem) {
					enemyAvail[z] = 1;
					soundQueue[7] = 18;
					if (tempI4 == 1)
					{
						cubeMax++;
						soundQueue[3] = V_DATA_CUBE;
					} else if (tempI4 == -1) {
						if (netmanager)
						{
							tempStr = (boost::format("%1% %2% %3%") % JE_getName(1) % miscTextB[4-1] % miscText[45-1]).str();
						} else if (twoPlayerMode) {
							tempStr = (boost::format("%1% %2%") % miscText[43-1] % miscText[45-1]).str();
						} else {
							tempStr = miscText[45-1];
						}
						JE_drawTextWindow(tempStr);
						JE_powerUp(1);
						soundQueue[7] = 29;
					} else if (tempI4 == -2) {
						if (netmanager)
						{
							tempStr = (boost::format("%1% %2% %3%") % JE_getName(2) % miscTextB[4-1] % miscText[46-1]).str();
						} else if (twoPlayerMode) {
							tempStr = (boost::format("%1% %2%") % miscText[44-1] % miscText[46-1]).str();
						} else {
							tempStr = miscText[46-1];
						}
						JE_drawTextWindow(tempStr);
						JE_powerUp(2);
						soundQueue[7] = 29;
					} else if (tempI4 == -3) {
						shotMultiPos[5-1] = 0;
						JE_initPlayerShot(0, 5, *PX_, *PY_, mouseX, mouseY, 104, playerNum_);
						shotAvail[z] = 0;
					} else if (tempI4 == -4) {
						if (superBomb[playerNum_-1] < 10)
							superBomb[playerNum_-1]++;
					} else if (tempI4 == -5) {
						pItems[PITEM_FRONT_WEAPON] = 25;         /*HOT DOG!*/
						pItems[PITEM_REAR_WEAPON] = 26;
						pItemsPlayer2[PITEM_REAR_WEAPON] = 26;
						memcpy(pItemsBack2, pItems, sizeof(pItemsBack2));
						portConfig[2-1] = 1;
						memset(shotMultiPos, 0, sizeof(shotMultiPos));
					} else if (twoPlayerLinked) {
						score += tempI4 / 2;
						score2 += tempI4 / 2;
					} else
						*score_ += tempI4;
					JE_setupExplosion(tempI3, enemy[z].ey, enemyDat[enemy[z].enemytype].explosiontype, 0, true);
				} else
					if (playerInvulnerable_ == 0 && enemyAvail[z] == 0 &&
					    enemyDat[enemy[z].enemytype].explosiontype % 2 == 0)
					{
						
						tempI3 = enemy[z].armorleft;
						if (tempI3 > damageRate)
							tempI3 = damageRate;
						
						JE_playerDamage(tempW, tempW, tempI3, PX_, PY_, playerAlive_, playerStillExploding_, armorLevel_, shield_, playerNum_);
						
						if (enemy[z].armorleft > 0)
						{
							*lastTurn2_ += (enemy[z].exc * enemy[z].armorleft) / 2;
							*lastTurn_  += (enemy[z].eyc * enemy[z].armorleft) / 2;
						}
						
						tempI = enemy[z].armorleft;
						if (tempI == 255)
							tempI = 30000;
						
						temp = enemy[z].linknum;
						if (temp == 0)
							temp = 255;
						
						b = z;
						
						if (tempI > tempI2)
						{
							if (enemy[z].armorleft != 255)
								enemy[z].armorleft -= tempI3;
							soundQueue[5] = 3;
						} else {
							
							for (int temp2 = 0; temp2 < 100; temp2++)
							{
								if (enemyAvail[temp2] != 1)
								{
									temp3 = enemy[temp2].linknum;
									if (temp2 == b ||
										(temp != 255 &&
										 (temp == temp3 || temp - 100 == temp3
										  || (temp3 > 40 && temp3 / 20 == temp / 20 && temp3 <= temp))))
										
									{
										
										tempI3 = enemy[temp2].ex + enemy[temp2].mapoffset;
										
										enemy[temp2].linknum = 0;
										
										enemyAvail[temp2] = 1;
										
										if (enemyDat[enemy[temp2].enemytype].esize == 1)
										{
											JE_setupExplosionLarge(enemy[temp2].enemyground, enemy[temp2].explonum, tempI3, enemy[temp2].ey);
											soundQueue[6] = 9;
										} else {
											JE_setupExplosion(tempI3, enemy[temp2].ey, 1);
											soundQueue[5] = 4;
										}
									}
								}
							}
							enemyAvail[z] = 1;
						}
					}
			}

		}
	}
}

