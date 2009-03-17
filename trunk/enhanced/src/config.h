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
#ifndef CONFIG_H
#define CONFIG_H

#include "opentyr.h"

#include "console/cvar/CVar.h"

#include <stdio.h>
#include <string>


static const int SAVE_FILES_NUM = 11*2;

static const int MAX_STARS = 100;

/* These are necessary because the size of the structure has changed from the original, but we
   need to know the original sizes in order to find things in TYRIAN.SAV */
static const int SAVE_FILES_SIZE = 2398;
static const int SIZEOF_SAVEGAMETEMP = SAVE_FILES_SIZE+4+100;
static const int SAVE_FILE_SIZE = (SIZEOF_SAVEGAMETEMP - 4);

enum PItemsEnum
{
	PITEM_FRONT_WEAPON = 0,
	PITEM_REAR_WEAPON = 1,
	PITEM_SUPER_ARCADE_MODE = 2,
	PITEM_LEFT_SIDEKICK = 3,
	PITEM_RIGHT_SIDEKICK = 4,
	PITEM_GENERATOR = 5,
	PITEM_P2_SIDEKICK_UPGRADE = 6,
	PITEM_P2_SIDEKICK = 7,
	PITEM_EPISODE = 8,
	PITEM_SHIELD = 9,
	PITEM_SPECIAL = 10,
	PITEM_SHIP = 11
};
typedef int JE_PItemsType[12]; /* [1..12] */

typedef char JE_EditorItemAvailType[100]; /* [1..100] */

struct JE_SaveFileType
{
	JE_word encode;
	unsigned int level;
	JE_PItemsType items;
	unsigned long score;
	unsigned long score2;
	std::string levelName; /* string [9]; */ /* SYN: Added one more byte to match lastLevelName below */
	std::string name; /* [1..14] */ /* SYN: Added extra byte for null */
	int cubes;
	int power[2];
	int episode;
	JE_PItemsType lastItems;
	int difficulty;
	int secretHint;
	int input1;
	int input2;
	bool gameHasRepeated; /*See if you went from one episode to another*/
	int initialDifficulty;
};

typedef JE_SaveFileType JE_SaveFilesType[SAVE_FILES_NUM]; /* [1..savefilesnum] */
typedef Uint8 JE_SaveGameTemp[SAVE_FILES_SIZE + 4 + 100]; /* [1..sizeof(savefilestype) + 4 + 100] */

typedef int JE_PortPowerType[7]; /* [1..7] */

struct StarDatType
{
	int sC;
	JE_word sLoc;
	JE_word sMov;
};

extern const unsigned char cryptKey[10];
//extern const JE_KeySettingType defaultKeySettings;

extern const JE_EditorItemAvailType initialItemAvail;
extern bool smoothies[9];
extern int starShowVGASpecialCode;
extern StarDatType starDat[MAX_STARS];
extern JE_word starY;
extern unsigned int lastCubeMax, cubeMax;
extern JE_word cubeList[4];
extern bool gameHasRepeated;
extern unsigned int difficultyLevel, oldDifficultyLevel, initialDifficulty;
extern unsigned long score, score2;
extern int power, lastPower, powerAdd;
extern JE_PItemsType pItems, pItemsPlayer2, pItemsBack, pItemsBack2;
extern int shield, shieldMax, shieldSet;
extern int shield2, shieldMax2;
extern int armorLevel, armorLevel2;
extern int shieldWait, shieldT;
extern int shotRepeat[11], shotMultiPos[11];
extern int portConfig[10];
extern bool portConfigDone;
extern JE_PortPowerType portPower, lastPortPower;
extern std::string lastLevelName;
extern std::string levelName;
extern unsigned int mainLevel, nextLevel, saveLevel;
//extern JE_KeySettingType keySettings;
extern int levelFilter, levelFilterNew, levelBrightness, levelBrightnessChg;
extern bool filterActive, filterFade, filterFadeStart;
extern bool gameJustLoaded;
extern bool galagaMode;
extern bool extraGame;
extern bool twoPlayerMode, twoPlayerLinked, onePlayerAction, superTyrian, trentWin;
extern int superArcadeMode;
extern int superArcadePowerUp;
extern float linkGunDirec;
extern int playerDevice1, playerDevice2;
extern int inputDevice1, inputDevice2;
extern int secretHint;
extern int background3over;
extern int background2over;
extern int gammaCorrection;
extern bool superPause, youAreCheating, displayScore, soundHasChanged, starActive, topEnemyOver, skyEnemyOverAll, background2notTransparent, tempb;
extern int fastPlay;
extern bool pentiumMode;
extern bool playerPasswordInput;
extern JE_SaveFilesType saveFiles;
extern JE_SaveFilesType *saveFilePointer;
extern JE_SaveGameTemp saveTemp;
extern JE_SaveGameTemp *saveTempPointer;

namespace CVars
{
	extern CVarInt game_speed;
}

void JE_initProcessorType( void );
void JE_setNewGameSpeed( void );
void JE_loadConfiguration( void );
void JE_saveConfiguration( void );

struct StringReadingException : public std::runtime_error
{
	StringReadingException(const std::string& message)
		: std::runtime_error(message)
	{}
};

std::string JE_readCryptLn( std::fstream& f );
void JE_skipCryptLn( std::fstream& f );

void JE_setupStars( void );

void JE_saveGame( int slot, const std::string& name );
void JE_loadGame( int slot );

std::string JE_decryptString( std::string s );

void scan_autorun( );
void scan_autorun( std::string dir );

#endif /* CONFIG_H */
