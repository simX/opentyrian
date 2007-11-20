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
#ifndef CONFIG_H
#define CONFIG_H

#include "opentyr.h"

#include <stdio.h>


#define SAVE_FILES_NUM (11 * 2)

#define MAX_STARS 100

/* These are necessary because the size of the structure has changed from the original, but we
   need to know the original sizes in order to find things in TYRIAN.SAV */
#define SAVE_FILES_SIZE 2398
#define SIZEOF_SAVEGAMETEMP SAVE_FILES_SIZE + 4 + 100
#define SAVE_FILE_SIZE (SIZEOF_SAVEGAMETEMP - 4)

/*#define SAVE_FILES_SIZE (2502 - 4)
#define SAVE_FILE_SIZE (SAVE_FILES_SIZE)*/

typedef SDLKey JE_KeySettingType[8]; /* [1..8] */
typedef int JE_PItemsType[12]; /* [1..12] */

typedef char JE_EditorItemAvailType[100]; /* [1..100] */

typedef struct
{
	JE_word encode;
	JE_word level;
	JE_PItemsType items;
	unsigned long score;
	unsigned long score2;
	char levelName[11]; /* string [9]; */ /* SYN: Added one more byte to match lastLevelName below */
	char name[15]; /* [1..14] */ /* SYN: Added extra byte for null */
	int cubes;
	int power[2]; /* [1..2] */
	int episode;
	JE_PItemsType lastItems;
	int difficulty;
	int secretHint;
	int input1;
	int input2;
	bool gameHasRepeated; /*See if you went from one episode to another*/
	int initialDifficulty;

	/* High Scores - Each episode has both sets of 1&2 player selections - with 3 in each */
	unsigned long highScore1, highScore2;
	char          highScoreName[30]; /* string [29] */
	int       highScoreDiff;
} JE_SaveFileType;

typedef JE_SaveFileType JE_SaveFilesType[SAVE_FILES_NUM]; /* [1..savefilesnum] */
typedef Uint8 JE_SaveGameTemp[SAVE_FILES_SIZE + 4 + 100]; /* [1..sizeof(savefilestype) + 4 + 100] */

typedef int JE_PortPowerType[7]; /* [1..7] */

typedef struct
{
	int sC;
	JE_word sLoc;
	JE_word sMov;
} StarDatType;

extern const unsigned char cryptKey[10];
extern const JE_KeySettingType defaultKeySettings;
extern const char defaultHighScoreNames[34][23];
extern const char defaultTeamNames[22][25];
extern const JE_EditorItemAvailType initialItemAvail;
extern bool smoothies[9];
extern int starShowVGASpecialCode;
extern StarDatType starDat[MAX_STARS];
extern JE_word starY;
extern JE_word lastCubeMax, cubeMax;
extern JE_word cubeList[4];
extern bool gameHasRepeated;
extern int difficultyLevel, oldDifficultyLevel, initialDifficulty;
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
extern bool resetVersion;
extern char lastLevelName[11], levelName[11];
extern int mainLevel, nextLevel, saveLevel;
extern JE_KeySettingType keySettings;
extern signed int levelFilter, levelFilterNew, levelBrightness, levelBrightnessChg;
extern bool filtrationAvail, filterActive, filterFade, filterFadeStart;
extern bool gameJustLoaded;
extern bool galagaMode;
extern bool extraGame;
extern bool twoPlayerMode, twoPlayerLinked, onePlayerAction, superTyrian, trentWin;
extern int superArcadeMode;
extern int superArcadePowerUp;
extern double linkGunDirec;
extern int playerDevice1, playerDevice2;
extern int inputDevice1, inputDevice2;
extern int secretHint;
extern int background3over;
extern int background2over;
extern int gammaCorrection;
extern bool superPause, explosionTransparent, youAreCheating, displayScore, soundHasChanged, background2, smoothScroll, wild, superWild, starActive, topEnemyOver, skyEnemyOverAll, background2notTransparent, tempb;
extern int versionNum;
extern int fastPlay;
extern bool pentiumMode;
extern bool playerPasswordInput;
extern int inputDevice;
extern int gameSpeed;
extern int processorType;
extern JE_SaveFilesType saveFiles;
extern JE_SaveFilesType *saveFilePointer;
extern JE_SaveGameTemp saveTemp;
extern JE_SaveGameTemp *saveTempPointer;

extern bool fullscreen_set, fullscreen_enabled;

void JE_initProcessorType( void );
void JE_setNewGameSpeed( void );
void JE_loadConfiguration( void );
void JE_saveConfiguration( void );
void JE_readCryptLn( FILE* f, char *s );
void JE_skipCryptLn( FILE* f );

void JE_setupStars( void );

void JE_saveGame( int slot, char *name );
void JE_loadGame( int slot );

void JE_decryptString( char *s, int len );


void JE_encryptSaveTemp( void );
void JE_decryptSaveTemp( void );

#endif /* CONFIG_H */
