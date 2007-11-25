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
#include "opentyr.h"

#include "episodes.h"
#include "error.h"
#include "joystick.h"
#include "loudness.h"
#include "nortsong.h"
#include "nortvars.h"
#include "varz.h"
#include "vga256d.h"
#include "iniparser.h"

#include "config.h"

#include <stdio.h>
#include <string.h>


/******** MAJOR TODO:
  SYN: High score data is stored one per save file slot. That makes 2 * 11 = 22 high scores.
  Each episode has six high scores. 6 * 4 = 24 OH SHI--

  I have no idea what is up with this, but I'm going to have to change substantial amounts and
  possibly partially break compatibility with the original. This will also get sorted out
  if/when we add support for Tyrian2000 data files, as I'll have to figure out what its save
  file format is (besides a couple kilobytes larger...).

  As it stands high scores are going to be broked for episode 4 (nevermind 5) and there's not
  much I can do about it. *emo tear* :'(

  I hope there aren't any other surprises like this waiting. We are using the code for v2.0,
  right? Right? :|
*/


/* Configuration Load/Save handler */

const unsigned char cryptKey[10] = /* [1..10] */
{
	15, 50, 89, 240, 147, 34, 86, 9, 32, 208
};

const JE_KeySettingType defaultKeySettings =
{
	SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_SPACE, SDLK_RETURN, SDLK_LCTRL, SDLK_LALT
};


const char tyrian_ini_template[] = \
	"[video]\n"
	"game_speed = %d\n"
	"processor_type = %d\n"
	"gamma_correction = %d\n"
	"fullscreen = %s\n"
	"\n"
	"[sound]\n"
	"sound_effects = %s\n"
	"music_volume = %d\n"
	"fx_volume = %d\n"
	"\n"
	"[input]\n"
	"device1 = %d\n"
	"device2 = %d\n"
	"\n"
	"[keyboard]\n"
	"; See SDL_keysym.h for values\n"
	"up = %d\n"
	"down = %d\n"
	"left = %d\n"
	"right = %d\n"
	"fire = %d\n"
	"change_fire = %d\n"
	"left_sidekick = %d\n"
	"right_sidekick = %d\n"
	"\n"
	"[joystick]\n"
	"; 1 = fire main weapons\n"
	"; 2 = fire left sidekick\n"
	"; 3 = fire right sidekick\n"
	"; 4 = fire both sidekicks\n"
	"; 5 = change rear weapon\n"
	"button1 = %d\n"
	"button2 = %d\n"
	"button3 = %d\n"
	"button4 = %d\n";

const char defaultHighScoreNames[34][23] = /* [1..34] of string [22] */
{/*1P*/
/*TYR*/   "The Prime Chair", /*13*/
          "Transon Lohk",
          "Javi Onukala",
          "Mantori",
          "Nortaneous",
          "Dougan",
          "Reid",
          "General Zinglon",
          "Late Gyges Phildren",
          "Vykromod",
          "Beppo",
          "Borogar",
          "ShipMaster Carlos",

/*OTHER*/ "Jill", /*5*/
          "Darcy",
          "Jake Stone",
          "Malvineous Havershim",
          "Marta Louise Velasquez",

/*JAZZ*/  "Jazz Jackrabbit", /*3*/
          "Eva Earlong",
          "Devan Shell",

/*OMF*/   "Crystal Devroe", /*11*/
          "Steffan Tommas",
          "Milano Angston",
          "Christian",
          "Shirro",
          "Jean-Paul",
          "Ibrahim Hothe",
          "Angel",
          "Cossette Akira",
          "Raven",
          "Hans Kreissack",

/*DARE*/  "Tyler", /*2*/
          "Rennis the Rat Guard"
};

const char defaultTeamNames[22][25] = /* [1..22] of string [24] */
{
	"Jackrabbits",
	"Team Tyrian",
	"The Elam Brothers",
	"Dare to Dream Team",
	"Pinball Freaks",
	"Extreme Pinball Freaks",
	"Team Vykromod",
	"Epic All-Stars",
	"Hans Keissack's WARriors",
	"Team Overkill",
	"Pied Pipers",
	"Gencore Growlers",
	"Microsol Masters",
	"Beta Warriors",
	"Team Loco",
	"The Shellians",
	"Jungle Jills",
	"Murderous Malvineous",
	"The Traffic Department",
	"Clan Mikal",
	"Clan Patrok",
	"Carlos' Crawlers"
};


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
JE_word lastCubeMax, cubeMax;
JE_word cubeList[4]; /* [1..4] */

/* High-Score Stuff */
bool gameHasRepeated;

/* Difficulty */
int difficultyLevel, oldDifficultyLevel, initialDifficulty;

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

bool resetVersion;

/* Level Data */
char lastLevelName[11], levelName[11]; /* string [10] */
int mainLevel, nextLevel, saveLevel;   /*Current Level #*/

/* Keyboard Junk */
JE_KeySettingType keySettings;

/* Configuration */
int levelFilter, levelFilterNew, levelBrightness, levelBrightnessChg;
bool filtrationAvail, filterActive, filterFade, filterFadeStart;

bool gameJustLoaded;

bool galagaMode;

bool extraGame;

bool twoPlayerMode, twoPlayerLinked, onePlayerAction, superTyrian;
bool trentWin = false;
int superArcadeMode;

int superArcadePowerUp;

double linkGunDirec;
int playerDevice1, playerDevice2;
int inputDevice1, inputDevice2;

int secretHint;
int background3over;
int background2over;
int gammaCorrection;
bool superPause = false;
bool explosionTransparent, youAreCheating, displayScore, soundHasChanged, background2, smoothScroll,
     wild, superWild, starActive, topEnemyOver, skyEnemyOverAll, background2notTransparent, tempb;

int versionNum; /* SW 1.0 and SW/Reg 1.1 = 0 or 1
                 * EA 1.2 = 2 */

int fastPlay;
bool pentiumMode;

/* Savegame files */
bool playerPasswordInput;
int inputDevice;  /* 0=Mouse   1=Joystick   2=Gravis GamePad */
int gameSpeed;
int processorType;  /* 1=386 2=486 3=Pentium Hyper */

JE_SaveFilesType saveFiles; /*array[1..saveLevelnum] of savefiletype;*/
JE_SaveFilesType *saveFilePointer = &saveFiles;
JE_SaveGameTemp saveTemp;
JE_SaveGameTemp *saveTempPointer = &saveTemp;

JE_word x;

bool fullscreen_set = false, fullscreen_enabled;

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

void JE_readCryptLn( FILE* f, char *s )
{
	int size;

	size = getc(f);
	if (size < 0)
		size = 0;
	efread(s, 1, size, f);
	s[size] = '\0';
	JE_decryptString(s, size);
}

void JE_skipCryptLn( FILE* f )
{
	char size;

	size = getc(f);
	fseek(f, size, SEEK_CUR);
}


void JE_setupStars( void )
{
	for (int z = MAX_STARS; z--; )
	{
		starDat[z].sLoc = (rand() % 320) + (rand() % 200) * 320;
		starDat[z].sMov = ((rand() % 3) + 2) * 320;
		starDat[z].sC = (rand() % 16) + (9 * 16);
	}
}

void JE_saveGame( int slot, char *name )
{
	saveFiles[slot-1].initialDifficulty = initialDifficulty;
	saveFiles[slot-1].gameHasRepeated = gameHasRepeated;
	saveFiles[slot-1].level = saveLevel;

	pItems[3-1] = superArcadeMode;
	if (superArcadeMode == 0 && onePlayerAction)
	{
		pItems[3-1] = 255;
	}
	if (superTyrian)
	{
		pItems[3-1] = 254;
	}

	memcpy(saveFiles[slot-1].items, pItems, sizeof(pItems));

	if (superArcadeMode > 253)
	{
		pItems[3-1] = 0;
	}
	if (twoPlayerMode)
	{
		memcpy(saveFiles[slot-1].lastItems, pItemsPlayer2, sizeof(pItemsPlayer2));
	} else {
		memcpy(saveFiles[slot-1].lastItems, pItemsBack2, sizeof(pItemsBack2));
	}

	saveFiles[slot-1].score  = score;
	saveFiles[slot-1].score2 = score2;
	memcpy(saveFiles[slot-1].levelName, lastLevelName, sizeof(lastLevelName));
	saveFiles[slot-1].cubes  = lastCubeMax;

	if (strcmp(lastLevelName, "Completed") == 0)
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

	strcpy(saveFiles[slot-1].name, name);

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
	memcpy(pItems, saveFiles[slot-1].items, sizeof(pItems));
	superArcadeMode   = pItems[3-1];

	if (superArcadeMode == 255)
	{
		onePlayerAction = true;
		superArcadeMode = 0;
		pItems[3-1] = 0;
	} else if (superArcadeMode == 254) {
		onePlayerAction = true;
		superArcadeMode = 0;
		pItems[3-1] = 0;
		superTyrian = true;
	} else if (superArcadeMode > 0) {
		onePlayerAction = true;
	}

	if (twoPlayerMode)
	{
		memcpy(pItemsPlayer2, saveFiles[slot-1].lastItems, sizeof(pItemsPlayer2));
		onePlayerAction = false;
	} else {
		memcpy(pItemsBack2, saveFiles[slot-1].lastItems, sizeof(pItemsBack2));
	}

	/* Compatibility with old version */
	if (pItemsPlayer2[7-1] < 101)
	{
		pItemsPlayer2[7-1] = 101;
		pItemsPlayer2[8-1] = pItemsPlayer2[4-1];
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

	memcpy(levelName, saveFiles[slot-1].levelName, sizeof(levelName));

	if (strcmp(levelName, "Completed") == 0)
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
	memcpy(lastLevelName, levelName, sizeof(levelName));
}

void JE_initProcessorType( void )
{
	/* SYN: Originally this proc looked at your hardware specs and chose appropriate options. We don't care, so I'll just set
	   decent defaults here. */

	wild = false;
	superWild = false;
	smoothScroll = true;
	explosionTransparent = true;
	filtrationAvail = false;
	background2 = true;
	displayScore = true;

	switch (processorType)
	{
		case 1: /* 386 */
			background2 = false;
			displayScore = false;
			explosionTransparent = false;
			break;
		case 2: /* 486 - Default */
			break;
		case 3: /* High Detail */
			smoothScroll = false;
			break;
		case 4: /* Pentium */
			wild = true;
			filtrationAvail = true;
			break;
		case 5: /* Nonstandard VGA */
			smoothScroll = false;
			break;
		case 6: /* SuperWild */
			wild = true;
			superWild = true;
			filtrationAvail = true;
			break;
	}

	switch (gameSpeed)
	{
		case 1:  /* Slug Mode */
			fastPlay = 3;
			break;
		case 2:  /* Slower */
			fastPlay = 4;
			break;
		case 3: /* Slow */
			fastPlay = 5;
			break;
		case 4: /* Normal */
			fastPlay = 0;
			break;
		case 5: /* Pentium Hyper */
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
		smoothScroll = true;
		frameCountMax = 2;
		break;
	case 1:
		speed = 0x3000;
		smoothScroll = true;
		frameCountMax = 2;
		break;
	case 2:
		speed = 0x2000;
		smoothScroll = false;
		frameCountMax = 2;
		break;
	case 3:
		speed = 0x5300;
		smoothScroll = true;
		frameCountMax = 4;
		break;
	case 4:
		speed = 0x4300;
		smoothScroll = true;
		frameCountMax = 3;
		break;
	case 5:
		speed = 0x4300;
		smoothScroll = true;
		frameCountMax = 2;
		pentiumMode = true;
		break;
	}

  frameCount = frameCountMax;
  JE_resetTimerInt();
  JE_setTimerInt();
}

void JE_encryptSaveTemp( void )
{
	JE_SaveGameTemp s3;
	char y;

	memcpy(s3, saveTemp, sizeof(s3));

	y = 0;
	for (int x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y += s3[x];
	}
	saveTemp[SAVE_FILE_SIZE] = y;

	y = 0;
	for (int x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y -= s3[x];
	}
	saveTemp[SAVE_FILE_SIZE+1] = y;

	y = 1;
	for (int x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y = (y * s3[x]) + 1;
	}
	saveTemp[SAVE_FILE_SIZE+2] = y;

	y = 0;
	for (int x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y = y ^ s3[x];
	}
	saveTemp[SAVE_FILE_SIZE+3] = y;

	for (int x = 0; x < SAVE_FILE_SIZE; x++)
	{
		saveTemp[x] = saveTemp[x] ^ cryptKey[(x+1) % 10];
		if (x > 0)
		{
			saveTemp[x] = saveTemp[x] ^ saveTemp[x - 1];
		}
	}
}

void JE_decryptSaveTemp( void )
{
	bool correct = true;
	JE_SaveGameTemp s2;
	/*JE_word x;*/
	unsigned int y;

	/* Decrypt save game file */
	for (int x = (SAVE_FILE_SIZE - 1); x >= 0; x--)
	{
		s2[x] = (char)saveTemp[x] ^ (char)(cryptKey[(x+1) % 10]);
		if (x > 0)
		{
			s2[x] ^= (char)saveTemp[x - 1];
		}

	}

	/* for (x = 0; x < SAVE_FILE_SIZE; x++) printf("%c", s2[x]); */

	/* Check save file for correctitude */
	y = 0;
	for (int x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y += s2[x];
	}
	y %= 256;
	if (saveTemp[SAVE_FILE_SIZE] != y)
	{
		correct = false;
		printf("Failed additive checksum: %d vs %d\n", saveTemp[SAVE_FILE_SIZE], y);
	}

	y = 0;
	for (int x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y -= s2[x];
	}
	y %= 256;
	if (saveTemp[SAVE_FILE_SIZE+1] != y)
	{
		correct = false;
		printf("Failed subtractive checksum: %d vs %d\n", saveTemp[SAVE_FILE_SIZE+1], y);
	}

	y = 1;
	for (int x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y = (y * s2[x]) + 1;
	}
	y %= 256;
	if (saveTemp[SAVE_FILE_SIZE+2] != y)
	{
		correct = false;
		printf("Failed multiplicative checksum: %d vs %d\n", saveTemp[SAVE_FILE_SIZE+2], y);
	}

	y = 0;
	for (int x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y = y ^ s2[x];
	}
	if (saveTemp[SAVE_FILE_SIZE+3] != y)
	{
		correct = false;
		printf("Failed XOR'd checksum: %d vs %d\n", saveTemp[SAVE_FILE_SIZE+3], y);
	}

	/* Barf and die if save file doesn't validate */
	if (!correct)
	{
		printf("Error reading save file!\n");
		exit(255);
	}

	/* Keep decrypted version plz */
	memcpy(saveTemp, s2, sizeof(s2));
}

void JE_loadConfiguration( void )
{
	FILE *fi;
	Uint8 *p;
	char junk;

	errorActive = true;

	dictionary *ini = iniparser_new("tyrian.ini");

	gameSpeed = iniparser_getint(ini, "video:game_speed", 4);
	processorType = iniparser_getint(ini, "video:processor_type", 3);
	gammaCorrection = iniparser_getint(ini, "video:gamma_correction", 0);
	fullscreen_set = iniparser_getboolean(ini, "video:fullscreen", false);

	soundEffects = iniparser_getboolean(ini, "sound:sound_effects", true);
	tyrMusicVolume = iniparser_getint(ini, "sound:music_volume", 255);
	fxVolume = iniparser_getint(ini, "sound:fx_volume", 128);

	inputDevice1 = iniparser_getint(ini, "input:device1", 0);
	inputDevice2 = iniparser_getint(ini, "input:device2", 0);

	keySettings[0] = iniparser_getint(ini, "keyboard:up", 273);
	keySettings[1] = iniparser_getint(ini, "keyboard:down", 274);
	keySettings[2] = iniparser_getint(ini, "keyboard:left", 276);
	keySettings[3] = iniparser_getint(ini, "keyboard:right", 275);
	keySettings[4] = iniparser_getint(ini, "keyboard:fire", 32);
	keySettings[5] = iniparser_getint(ini, "keyboard:change_fire", 13);
	keySettings[6] = iniparser_getint(ini, "keyboard:left_sidekick", 306);
	keySettings[7] = iniparser_getint(ini, "keyboard:right_sidekick", 308);

	joyButtonAssign[0] = iniparser_getint(ini, "joystick:button1", 1);
	joyButtonAssign[1] = iniparser_getint(ini, "joystick:button2", 4);
	joyButtonAssign[2] = iniparser_getint(ini, "joystick:button3", 5);
	joyButtonAssign[3] = iniparser_getint(ini, "joystick:button4", 5);

	iniparser_free(ini);

	tyrMusicVolume = (tyrMusicVolume > 255) ? 255 : tyrMusicVolume;
	fxVolume = (fxVolume > 254) ? 254 : ((fxVolume < 14) ? 14 : fxVolume);

	soundActive = true;
	musicActive = true;

	JE_setVol(tyrMusicVolume, fxVolume);

	dont_die = true;
	JE_resetFile(&fi, "tyrian.sav");
	dont_die = false;

	if (fi)
	{

		fseek(fi, 0, SEEK_SET);
		efread(saveTemp, 1, sizeof(saveTemp), fi);
		JE_decryptSaveTemp();

		/* SYN: The original mostly blasted the save file into raw memory. However, our lives are not so
		   easy, because the C struct is necessarily a different size. So instead we have to loop
		   through each record and load fields manually. *emo tear* :'( */

		p = saveTemp;
		for (int z = 0; z < SAVE_FILES_NUM; z++)
		{
			saveFiles[z].encode = *((JE_word*)p);
			p += sizeof(JE_word);

			saveFiles[z].level = *((JE_word*)p);
			p += sizeof(JE_word);

			for (int i = 0; i < 12; i++)
			{
				saveFiles[z].items[i] = *((Uint8*)p);
				p += sizeof(Uint8);
			}

			saveFiles[z].score = *((Sint32 *)p);
			p += sizeof(Sint32);

			saveFiles[z].score2 = *((Sint32 *)p);
			p += sizeof(Sint32);

			/* SYN: Pascal strings are prefixed by a byte holding the length! */
			p += 1; /* Skip length byte */
			memcpy(saveFiles[z].levelName, ((char*)p), 9);
			saveFiles[z].levelName[9] = 0;
			p += 9;

			/* This was a BYTE array, not a STRING, in the original. Go fig. */
			memcpy(saveFiles[z].name, ((char*)p), 14);
			saveFiles[z].name[14] = 0;
			p += 14;

			saveFiles[z].cubes = *((Uint8 *)p);
			p += sizeof(Uint8);

			saveFiles[z].power[0] = *((Uint8 *)p);
			p += sizeof(Uint8);
			saveFiles[z].power[1] = *((Uint8 *)p);
			p += sizeof(Uint8);

			saveFiles[z].episode = *((Uint8*)p);
			p += sizeof(Uint8);

			for (int i = 0; i < 12; i++)
			{
				saveFiles[z].lastItems[i] = *((Uint8*)p);
				p += sizeof(Uint8);
			}

			saveFiles[z].difficulty = *((Uint8*)p);
			p += sizeof(Uint8);

			saveFiles[z].secretHint = *((Uint8*)p);
			p += sizeof(Uint8);

			saveFiles[z].input1 = *((Uint8*)p);
			p += sizeof(Uint8);

			saveFiles[z].input2 = *((Uint8*)p);
			p += sizeof(Uint8);

			saveFiles[z].gameHasRepeated = *((Uint8 *)p);
			p += sizeof(Uint8);

			saveFiles[z].initialDifficulty = *((Uint8*)p);
			p += sizeof(Uint8);

			saveFiles[z].highScore1 = *((Sint32 *)p);
			p += sizeof(Sint32);

			saveFiles[z].highScore2 = *((Sint32 *)p);
			p += sizeof(Sint32);

			p += 1; /* Skip length byte wheeee */
			memcpy(saveFiles[z].highScoreName, ((char*)p), 29);
			saveFiles[z].highScoreName[29] = 0;
			p += 29;

			saveFiles[z].highScoreDiff = *((Uint8*)p);
			p += sizeof(Uint8);
		}

		fclose(fi);
	} else {
		/* We didn't have a save file! Let's make up random stuff! */
		for (int z = 0; z < 100; z++)
		{
			saveTemp[SAVE_FILES_SIZE + z] = initialItemAvail[z];
		}

		for (int z = 0; z < SAVE_FILES_NUM; z++)
		{
			saveFiles[z].level = 0;

			for (int y = 0; y < 14; y++)
			{
				saveFiles[z].name[y] = ' ';
			}
			saveFiles[z].name[14] = 0;

			saveFiles[z].highScore1 = ((rand() % 20) + 1) * 1000;

			if (z % 6 > 2)
			{
				saveFiles[z].highScore2 = ((rand() % 20) + 1) * 1000;
				strcpy(saveFiles[z].highScoreName, defaultHighScoreNames[rand() % 22]);
			} else {
				strcpy(saveFiles[z].highScoreName, defaultHighScoreNames[rand() % 34]);
			}
		}
	}

	errorActive = false;

	JE_calcFXVol();
	JE_initProcessorType();
}

void JE_saveConfiguration( void )
{
	FILE *f;
	Uint8 *p, junk = 0;

	p = saveTemp;

	for (int z = 0; z < SAVE_FILES_NUM; z++)
	{
		*((JE_word*)p) = saveFiles[z].encode;
		p += sizeof(JE_word);

		*((JE_word*)p) = saveFiles[z].level;
		p += sizeof(JE_word);

		for (int i = 0; i < 12; i++)
		{
			*((Uint8*)p) = saveFiles[z].items[i];
			p += sizeof(Uint8);
		}

		*((Sint32 *)p) = saveFiles[z].score ;
		p += sizeof(Sint32);

		*((Sint32 *)p) = saveFiles[z].score2;
		p += sizeof(Sint32);

		/* SYN: Pascal strings are prefixed by a byte holding the length! */
		*((Uint8*)p) = strlen(saveFiles[z].levelName);
		p++;
		memcpy(((char*)p), saveFiles[z].levelName, 9);
		p += 9;

		/* This was a BYTE array, not a STRING, in the original. Go fig. */
		memcpy(((char*)p), saveFiles[z].name, 14);
		p += 14;

		*((Uint8*)p) = saveFiles[z].cubes;
		p += sizeof(Uint8);

		*((Uint8*)p) = saveFiles[z].power[0];
		p += sizeof(Uint8);
		*((Uint8*)p) = saveFiles[z].power[1];
		p += sizeof(Uint8);

		*((Uint8*)p) = saveFiles[z].episode ;
		p += sizeof(Uint8);

		for (int i = 0; i < 12; i++)
		{
			*((Uint8*)p) = saveFiles[z].lastItems[i];
			p += sizeof(Uint8);
		}

		*((Uint8*)p) = saveFiles[z].difficulty;
		p += sizeof(Uint8);

		*((Uint8*)p) = saveFiles[z].secretHint ;
		p += sizeof(Uint8);

		*((Uint8*)p) = saveFiles[z].input1;
		p += sizeof(Uint8);

		*((Uint8*)p) = saveFiles[z].input2;
		p += sizeof(Uint8);

		*((Uint8 *)p) = saveFiles[z].gameHasRepeated ;
		p += sizeof(Uint8);

		*((Uint8*)p) = saveFiles[z].initialDifficulty;
		p += sizeof(Uint8);

		*((Sint32 *)p) = saveFiles[z].highScore1;
		p += sizeof(Sint32);

		*((Sint32 *)p) = saveFiles[z].highScore2 ;
		p += sizeof(Sint32);

		p++; /* Skip length byte wheeee */
		memcpy(((char*)p), saveFiles[z].highScoreName, 29);
		p += 29;

		*((Uint8*)p) = saveFiles[z].highScoreDiff ;
		p += sizeof(Uint8);
	}

	JE_encryptSaveTemp();
	f = fopen_check("tyrian.sav", "wb");
	if (f)
	{
		efwrite(saveTemp, 1, sizeof(saveTemp), f);
		fclose(f);
	}
	JE_decryptSaveTemp();


	FILE *ini = fopen("tyrian.ini", "w");
	if (ini == NULL)
	{
		printf("Couldn't write tyrian.ini.");
	} else {
		// YKS: Yes, this is horrible, need to come up with a better way
		fprintf(ini, tyrian_ini_template,
			gameSpeed, processorType, gammaCorrection, (fullscreen_set ? "true" : "false"), // [video]
			(soundEffects ? "true" : "false"), tyrMusicVolume, fxVolume, // [sound]
			inputDevice1, inputDevice2, // [input]
			keySettings[0], keySettings[1], keySettings[2], keySettings[3], // [keyboard]
			keySettings[4], keySettings[5], keySettings[6], keySettings[7],
			joyButtonAssign[0], joyButtonAssign[1], joyButtonAssign[2], joyButtonAssign[3] // [joystick]
		);
		fclose(ini);
	}
}
