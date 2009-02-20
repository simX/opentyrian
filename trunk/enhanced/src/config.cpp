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
#include "config.h"

#include "episodes.h"
#include "error.h"
#include "joystick.h"
#include "loudness.h"
#include "nortsong.h"
#include "nortvars.h"
#include "varz.h"
#include "video.h"
#include "vga256d.h"
#include "BinaryStream.h"
#include "console/Console.h"
#include "HighScores.h"
#include "console/cvar/CVar.h"
#include "console/cvar/CVarManager.h"
#include "console/BindManager.h"
#include "Filesystem.h"

#include <string>
#include <fstream>
#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif

namespace fs = boost::filesystem;

const unsigned char cryptKey[10] = /* [1..10] */
{
	15, 50, 89, 240, 147, 34, 86, 9, 32, 208
};

/*
const JE_KeySettingType defaultKeySettings =
{
	SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_SPACE, SDLK_RETURN, SDLK_LCTRL, SDLK_LALT
};
*/

const JE_EditorItemAvailType initialItemAvail =
{
	1,1,1,0,0,1,1,0,1,1,1,1,1,0,1,0,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0, /* Front/Rear Weapons 1-38  */
	0,0,0,0,0,0,0,0,0,0,1,                                                           /* Fill                     */
	1,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0,0,0,                                             /* Sidekicks          51-68 */
	0,0,0,0,0,0,0,0,0,0,0,                                                           /* Fill                     */
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                                   /* Special Weapons    81-93 */
	0,0,0,0,0                                                                        /* Fill                     */
};

/* Last 2 bytes = Word
 *
 * Max Value = 1680
 * X div  60 = Armor  (1-28)
 * X div 168 = Shield (1-12)
 * X div 280 = Engine (1-06)
 */


bool smoothies[9] = /* [1..9] */
{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };


int starShowVGASpecialCode;

/* Stars */
StarDatType starDat[MAX_STARS]; /* [1..Maxstars] */
JE_word starY;


/* CubeData */
unsigned int lastCubeMax, cubeMax;
JE_word cubeList[4]; /* [1..4] */

/* High-Score Stuff */
bool gameHasRepeated;

/* Difficulty */
unsigned int difficultyLevel, oldDifficultyLevel, initialDifficulty;

/* Player Stuff */
unsigned long score, score2;

JE_PItemsType pItems, pItemsPlayer2, pItemsBack, pItemsBack2;
int power, lastPower, powerAdd;
int shield, shieldMax, shieldSet;
int shield2, shieldMax2;
int armorLevel, armorLevel2;
int shieldWait, shieldT;

int shotRepeat[11], shotMultiPos[11]; /* [1..11] */  /* 7,8 = Superbomb */
int portConfig[10]; /* [1..10] */
bool portConfigDone;
JE_PortPowerType portPower, lastPortPower;


/* Level Data */
std::string lastLevelName;
std::string levelName; /* string [10] */
unsigned int mainLevel, nextLevel, saveLevel;   /*Current Level #*/

/* Keyboard Junk */
//JE_KeySettingType keySettings;

/* Configuration */
int levelFilter, levelFilterNew, levelBrightness, levelBrightnessChg;
bool filterActive, filterFade, filterFadeStart;

bool gameJustLoaded;

bool galagaMode;

bool extraGame;

bool twoPlayerMode, twoPlayerLinked, onePlayerAction, superTyrian;
bool trentWin = false;
int superArcadeMode;

int superArcadePowerUp;

float linkGunDirec;
int playerDevice1, playerDevice2;
/* 0=Mouse   1=Joystick   2=Gravis GamePad */
int inputDevice1, inputDevice2;

int secretHint;
int background3over;
int background2over;
int gammaCorrection;
bool superPause = false;
bool youAreCheating, displayScore, soundHasChanged, starActive, topEnemyOver, skyEnemyOverAll, background2notTransparent, tempb;

int fastPlay;
bool pentiumMode;

/* Savegame files */
bool playerPasswordInput;

JE_SaveFilesType saveFiles; /*array[1..saveLevelnum] of savefiletype;*/
JE_SaveFilesType *saveFilePointer = &saveFiles;
JE_SaveGameTemp saveTemp;
JE_SaveGameTemp *saveTempPointer = &saveTemp;

const unsigned char StringCryptKey[10] = {99, 204, 129, 63, 255, 71, 19, 25, 62, 1};

void JE_decryptString( char *s, int len )
{
	for (int i = len-1; i >= 0; i--)
	{
		s[i] ^= StringCryptKey[((i+1) % 10)];
		if (i > 0)
		{
			s[i] ^= s[i-1];
		}
	}
}

std::string JE_decryptString( std::string s )
{
	for (int i = s.length()-1; i >= 0; i--)
	{
		s[i] ^= StringCryptKey[((i+1) % 10)];
		if (i > 0)
		{
			s[i] ^= s[i-1];
		}
	}
	return s;
}

void JE_readCryptLn( FILE* f, char *s )
{
	int size = getc(f);
	if (size < 0) size = 0;
	efread(s, 1, size, f);
	s[size] = '\0';
	JE_decryptString(s, size);
}

std::string JE_readCryptLn( std::fstream& f )
{
	unsigned int size = f.get();
	char* buf = new char[size];
	f.read(buf, size);
	std::string str(buf, size);
	delete buf;

	return JE_decryptString(str);
}

void JE_skipCryptLn( FILE* f )
{
	unsigned int size = getc(f);
	fseek(f, size, SEEK_CUR);
}

void JE_skipCryptLn( std::fstream& f )
{
	f.seekg(f.get(), std::ios::cur);
}

void JE_setupStars( void )
{
	for (int z = MAX_STARS; z--; )
	{
		starDat[z].sLoc = (mt::rand() % 320) + (rand() % 200) * scr_width;
		starDat[z].sMov = ((mt::rand() % 3) + 2) * scr_width;
		starDat[z].sC = (mt::rand() % 16) + (9 * 16);
	}
}

void JE_saveGame( int slot, const std::string& name )
{
	saveFiles[slot-1].initialDifficulty = initialDifficulty;
	saveFiles[slot-1].gameHasRepeated = gameHasRepeated;
	saveFiles[slot-1].level = saveLevel;

	pItems[PITEM_SUPER_ARCADE_MODE] = superArcadeMode;
	if (superArcadeMode == 0 && onePlayerAction)
	{
		pItems[PITEM_SUPER_ARCADE_MODE] = 255;
	}
	if (superTyrian)
	{
		pItems[PITEM_SUPER_ARCADE_MODE] = 254;
	}

	memcpy(&saveFiles[slot-1].items, &pItems, sizeof(pItems));

	if (superArcadeMode > 253)
	{
		pItems[PITEM_SUPER_ARCADE_MODE] = 0;
	}
	if (twoPlayerMode)
	{
		memcpy(&saveFiles[slot-1].lastItems, &pItemsPlayer2, sizeof(pItemsPlayer2));
	} else {
		memcpy(&saveFiles[slot-1].lastItems, &pItemsBack2, sizeof(pItemsBack2));
	}

	saveFiles[slot-1].score  = score;
	saveFiles[slot-1].score2 = score2;
	saveFiles[slot-1].levelName = lastLevelName;
	saveFiles[slot-1].cubes  = lastCubeMax;

	if (lastLevelName == "Completed")
	{
		temp = episodeNum - 1;
		if (temp < 1)
		{
			temp = 4; /* JE: {Episodemax is 4 for completion purposes} */
		}
		saveFiles[slot-1].episode = temp;
	} else {
		saveFiles[slot-1].episode = episodeNum;
	}

	saveFiles[slot-1].difficulty = difficultyLevel;
	saveFiles[slot-1].secretHint = secretHint;
	saveFiles[slot-1].input1 = inputDevice1;
	saveFiles[slot-1].input2 = inputDevice2;

	saveFiles[slot-1].name = name;

	for (int x = 0; x < 2; x++)
	{
		saveFiles[slot-1].power[x] = portPower[x];
	}

	JE_saveConfiguration();
}

void JE_loadGame( int slot )
{
	int temp5;

	superTyrian = false;
	onePlayerAction = false;
	twoPlayerMode = false;
	extraGame = false;
	galagaMode = false;

	initialDifficulty = saveFiles[slot-1].initialDifficulty;
	gameHasRepeated   = saveFiles[slot-1].gameHasRepeated;
	twoPlayerMode     = (slot-1) > 10;
	difficultyLevel   = saveFiles[slot-1].difficulty;
	memcpy(&pItems, &saveFiles[slot-1].items, sizeof(pItems));
	superArcadeMode   = pItems[PITEM_SUPER_ARCADE_MODE];

	if (superArcadeMode == 255)
	{
		onePlayerAction = true;
		superArcadeMode = 0;
		pItems[PITEM_SUPER_ARCADE_MODE] = 0;
	} else if (superArcadeMode == 254) {
		onePlayerAction = true;
		superArcadeMode = 0;
		pItems[PITEM_SUPER_ARCADE_MODE] = 0;
		superTyrian = true;
	} else if (superArcadeMode > 0) {
		onePlayerAction = true;
	}

	if (twoPlayerMode)
	{
		memcpy(&pItemsPlayer2, &saveFiles[slot-1].lastItems, sizeof(pItemsPlayer2));
		onePlayerAction = false;
	} else {
		memcpy(&pItemsBack2, &saveFiles[slot-1].lastItems, sizeof(pItemsBack2));
	}

	score       = saveFiles[slot-1].score;
	score2      = saveFiles[slot-1].score2;
	mainLevel   = saveFiles[slot-1].level;
	cubeMax     = saveFiles[slot-1].cubes;
	lastCubeMax = cubeMax;

	secretHint   = saveFiles[slot-1].secretHint;
	inputDevice1 = saveFiles[slot-1].input1;
	inputDevice2 = saveFiles[slot-1].input2;

	for (int temp = 0; temp < 2; temp++)
	{
		portPower[temp] = saveFiles[slot-1].power[temp];
	}

	temp5 = saveFiles[slot-1].episode;

	levelName = saveFiles[slot-1].levelName;

	if (levelName == "Completed")
	{
		if (temp5 == 4)
		{
			temp5 = 1;
		} else if (temp5 < 4) {
			temp5++;
		}
		/* Increment 1-3 to 2-4.  Episode 4 goes to 1.  Episode 5 stands still. */
	}

	JE_initEpisode(temp5);
	saveLevel = mainLevel;
	lastLevelName = levelName;
}

namespace CVars
{
	CVarInt game_speed("game_speed", CVar::CONFIG | CVar::CONFIG_AUTO, "Game speed. 0-4", 3, rangeBind(0, 4));
}

void JE_initProcessorType( void )
{
	/*
	0 - Low
	1 - Medium
	2 - High
	3 - Pentium

	backgroundTransparent | 3-4
	            superWild | 4
	          background2 | 1-4
	 explosionTransparent | 1-4
	      filtrationAvail | 3-4
	*/

	/*switch (CVars::detail_level)
	{
		case 0: // 386
			backgroundTransparent = false;
			superWild = false;
			background2 = false;
			explosionTransparent = false;
			filtrationAvail = false;
			break;
		case 1: // 486
			backgroundTransparent = false;
			superWild = false;
			background2 = true;
			explosionTransparent = true;
			filtrationAvail = false;
			break;
		case 2: // High Detail
			backgroundTransparent = false;
			superWild = false;
			background2 = true;
			explosionTransparent = true;
			filtrationAvail = false;
			break;
		case 3: // Pentium
			backgroundTransparent = true;
			superWild = false;
			background2 = true;
			explosionTransparent = true;
			filtrationAvail = true;
			break;
		case 4: // SuperWild
			backgroundTransparent = true;
			superWild = true;
			background2 = true;
			explosionTransparent = true;
			filtrationAvail = true;
			break;
	}*/

	switch (CVars::game_speed)
	{
		case 0:  /* Slug Mode */
			fastPlay = 3;
			break;
		case 1:  /* Slower */
			fastPlay = 4;
			break;
		case 2: /* Slow */
			fastPlay = 5;
			break;
		case 3: /* Normal */
			fastPlay = 0;
			break;
		case 4: /* Pentium Hyper */
			fastPlay = 1;
			break;
	}
}

void JE_setNewGameSpeed( void )
{
	pentiumMode = false;

	switch (fastPlay)
	{
	case 0:
		speed = 0x4300;
		frameCountMax = 2;
		break;
	case 1:
		speed = 0x3000;
		frameCountMax = 2;
		break;
	case 2:
		speed = 0x2000;
		frameCountMax = 2;
		break;
	case 3:
		speed = 0x5300;
		frameCountMax = 4;
		break;
	case 4:
		speed = 0x4300;
		frameCountMax = 3;
		break;
	case 5:
		speed = 0x4300;
		frameCountMax = 2;
		pentiumMode = true;
		break;
	}

  frameCount = frameCountMax;
  JE_resetTimerInt();
  JE_setTimerInt();
}

namespace CVars
{
	// Video
	CVarInt gamma_correction("gamma_correction", CVar::CONFIG | CVar::CONFIG_AUTO, "Gamma correction. 0-3", 0, rangeBind(0, 3));

	// Input devices // TODO will probably be removed
	CVarInt input_dev1("input_dev1", CVar::CONFIG | CVar::CONFIG_AUTO, "Input device for player 1. 1-3", 1, rangeBind(1, 3));
	CVarInt input_dev2("input_dev2", CVar::CONFIG | CVar::CONFIG_AUTO, "Input device for player 2. 1-3", 2, rangeBind(1, 3));
}

void JE_loadConfiguration( void )
{
	for (int i = 0; i < SAVE_FILES_NUM; i++)
	{
		JE_SaveFileType& save = saveFiles[i];

		const std::string fname = Filesystem::getHomeDir()+"save/tyrian" + char(i<11 ? '1' : '2') + char(i==10 || i==21 ? 'L' : '0'+(i<11 ? i : i-11)) + ".sav";
		std::ifstream file(fname.c_str(), std::ios_base::in | std::ios_base::binary);

		if (file)
		{
			IBinaryStream f(file);

			save.encode = f.get16();
			save.level = f.get16();
			for (int j = 0; j < 12; j++)
			{
				save.items[j] = f.get16();
			}
			for (int j = 0; j < 12; j++)
			{
				save.lastItems[j] = f.get16();
			}
			save.score = f.get32();
			save.score2 = f.get32();
			save.levelName = f.getStr();
			save.name = f.getStr();
			save.cubes = f.get8();
			save.power[0] = f.get8();
			save.power[1] = f.get8();
			save.episode = f.get8();
			save.difficulty = f.get8();
			save.secretHint = f.get8();
			save.input1 = f.get8();
			save.input2 = f.get8();
			save.gameHasRepeated = (f.get8() != 0);
			save.initialDifficulty = f.get8();
		} else {
			// Savefile not found, initialize to defaults
			save.level = 0;
			save.name = "";
		}
	}

	std::ifstream file((Filesystem::getHomeDir()+"save/tyrian.hi").c_str(), std::ios_base::in | std::ios_base::binary);
	if (file)
	{
		IBinaryStream s(file);
		highScores.unserialize(s);
	} // Making up random stuff handled by the default ctor

	JE_initProcessorType();
}

static void save_cvars( std::ofstream& f )
{
	std::list<CVar*> cvars = CVarManager::get().getCVars(CVar::CONFIG, false);

	for (std::list<CVar*>::const_iterator i = cvars.begin(); i != cvars.end(); ++i)
	{
		const CVar& cvar = **i;
		if (cvar.archiveDirty())
			f << "seta \"" << cvar.getName() << "\" \"" << cvar.serializeArchive() << "\"\n";
	}
}

static void save_binds( std::ofstream& f )
{
	f << "unbind\n";

	BindManager::SetType binds = BindManager::get().getBinds();

	for (BindManager::SetType::const_iterator i = binds.begin(); i != binds.end(); ++i)
	{
		const Bind& bind = **i;

		for (Bind::SetType::const_iterator command = bind.commands.begin(); command != bind.commands.end(); ++command)
		{
			f << "bind \"" << bind.getKeyDescription() << "\" \"";
			if (command->toggle)
				f << '+';
			f << command->command << "\"\n";
		}
	}
}

void JE_saveConfiguration( void )
{
	try
	{
		fs::create_directory(Filesystem::getHomeDir()+"save"); // Does nothing if dir already exists
	}
	catch (const fs::filesystem_error& ex)
	{
		Console::get() << "\a7Error:\ax Unable to create save directory: " << ex.what() << '\n'
			<< "Saving disabled." << std::endl;
		return;
	}

	{
		std::ofstream file((Filesystem::getHomeDir()+"save/tyrian.hi").c_str(), std::ios_base::out | std::ios_base::binary);

		if (file)
		{
			OBinaryStream s(file);
			highScores.serialize(s);
		}

		file.close();

#if (_BSD_SOURCE || _XOPEN_SOURCE >= 500)
		sync();
#endif
	}

	for (int i = 0; i < SAVE_FILES_NUM; i++)
	{
		if (saveFiles[i].level != 0)
		{
			std::string fname = std::string(Filesystem::getHomeDir()+"save/tyrian") + char(i<11 ? '1' : '2') + char(i==10 || i==21 ? 'L' : '0'+(i<11 ? i : i-11)) + ".sav";
			std::ofstream file(fname.c_str(), std::ios_base::out | std::ios_base::binary);

			if (!file)
			{
				Console::get() << "\a7Error:\ax Failed to write " << fname << std::endl;
				continue;
			}

			OBinaryStream f(file);

			JE_SaveFileType& save = saveFiles[i];

			f.put16(save.encode);
			f.put16(save.level);
			for (int j = 0; j < 12; j++)
			{
				f.put16(save.items[j]);
			}
			for (int j = 0; j < 12; j++)
			{
				f.put16(save.lastItems[j]);
			}
			f.put32(save.score);
			f.put32(save.score2);
			f.put(save.levelName);
			f.put(save.name);
			f.put8(save.cubes);
			f.put8(save.power[0]);
			f.put8(save.power[1]);
			f.put8(save.episode);
			f.put8(save.difficulty);
			f.put8(save.secretHint);
			f.put8(save.input1);
			f.put8(save.input2);
			f.put8(save.gameHasRepeated);
			f.put8(save.initialDifficulty);
#if (_BSD_SOURCE || _XOPEN_SOURCE >= 500)
			sync();
#endif
		}
	}

	{
		std::ofstream file((Filesystem::getHomeDir()+"autorun/10config.con").c_str(), std::ios_base::out);

		if (file)
		{
			file << "# Configuration file for OpenTyrian Enhanced\n\n" <<
				"# This file is auto-generated by the game.\n" <<
				"# You can edit the options, but any formatting and foreign commands will be lost.\n\n";

			save_cvars(file);

			file << '\n';

			save_binds(file);

#if (_BSD_SOURCE || _XOPEN_SOURCE >= 500)
			sync();
#endif
		}
		else
		{
			Console::get() << "\a7Error:\ax Couldn't write " << Filesystem::getHomeDir() << "autorun/10config.con." << std::endl;
		}
	}
}

void scan_autorun( )
{
	scan_autorun("autorun");

	if (CVars::cfg_use_home)
		scan_autorun(Filesystem::getHomeDir()+"/autorun");
}

void scan_autorun( std::string dir )
{
	try
	{
		fs::create_directory(dir); // Does nothing if dir already exists
	}
	catch (const fs::filesystem_error& ex)
	{
		Console::get() << "\a7Error:\ax Unable to create autorun (" << dir << ") directory: " << ex.what() << std::endl;
		return;
	}

	fs::directory_iterator end_iter;
	for (fs::directory_iterator dir_iter(dir); dir_iter != end_iter; ++dir_iter)
	{
		if (fs::is_regular(dir_iter->status()) && fs::extension(dir_iter->path()) == ".con")
		{
			fs::ifstream stream(dir_iter->path().string(), std::ios_base::in);
			Console::get().runScript(dir_iter->path().leaf(), stream);
		}
	}
}
