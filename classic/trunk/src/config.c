/* 
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
#include "config.h"

#include "episodes.h"
#include "error.h"
#include "joystick.h"
#include "loudness.h"
#include "mtrand.h"
#include "nortsong.h"
#include "nortvars.h"
#include "varz.h"
#include "vga256d.h"
#include "video.h"
#include "video_scale.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>


/* Configuration Load/Save handler */

const JE_byte cryptKey[10] = /* [1..10] */
{
	15, 50, 89, 240, 147, 34, 86, 9, 32, 208
};

const JE_KeySettingType defaultKeySettings =
{
	SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_SPACE, SDLK_RETURN, SDLK_LCTRL, SDLK_LALT
/*	72, 80, 75, 77, 57, 28, 29, 56*/
};

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


JE_boolean smoothies[9] = /* [1..9] */
{ 0, 0, 0, 0, 0, 0, 0, 0, 0 };


JE_byte starShowVGASpecialCode;

/* Stars */
StarDatType starDat[MAX_STARS]; /* [1..Maxstars] */
JE_word starY;


/* CubeData */
JE_word lastCubeMax, cubeMax;
JE_word cubeList[4]; /* [1..4] */

/* High-Score Stuff */
JE_boolean gameHasRepeated;

/* Difficulty */
JE_shortint difficultyLevel, oldDifficultyLevel, initialDifficulty;

/* Player Stuff */
JE_longint score, score2;

JE_integer    power, lastPower, powerAdd;
JE_PItemsType pItems, pItemsPlayer2, pItemsBack, pItemsBack2;
JE_shortint   shield, shieldMax, shieldSet;
JE_shortint   shield2, shieldMax2;
JE_integer    armorLevel, armorLevel2;
JE_byte       shieldWait, shieldT;

JE_byte          shotRepeat[11], shotMultiPos[11]; /* [1..11] */  /* 7,8 = Superbomb */
JE_byte          portConfig[10]; /* [1..10] */
JE_boolean       portConfigDone;
JE_PortPowerType portPower, lastPortPower;

JE_boolean resetVersion;

/* Level Data */
char    lastLevelName[11], levelName[11]; /* string [10] */
JE_byte mainLevel, nextLevel, saveLevel;   /*Current Level #*/

/* Keyboard Junk */
JE_KeySettingType keySettings;

/* Configuration */
JE_shortint levelFilter, levelFilterNew, levelBrightness, levelBrightnessChg;
JE_boolean  filtrationAvail, filterActive, filterFade, filterFadeStart;

JE_boolean gameJustLoaded;

JE_boolean galagaMode;

JE_boolean extraGame;

JE_boolean twoPlayerMode, twoPlayerLinked, onePlayerAction, superTyrian;
JE_boolean trentWin = false;
JE_byte    superArcadeMode;

JE_byte    superArcadePowerUp;

JE_real linkGunDirec;
JE_byte playerDevice1, playerDevice2;
JE_byte inputDevice1, inputDevice2;

JE_byte secretHint;
JE_byte background3over;
JE_byte background2over;
JE_byte gammaCorrection;
JE_boolean superPause = false;
JE_boolean explosionTransparent,
           youAreCheating,
           displayScore,
           soundHasChanged,
           background2, smoothScroll, wild, superWild, starActive,
           topEnemyOver,
           skyEnemyOverAll,
           background2notTransparent,
           tempb;

JE_byte versionNum;   /* SW 1.0 and SW/Reg 1.1 = 0 or 1
                       * EA 1.2 = 2 */

JE_byte    fastPlay;
JE_boolean pentiumMode;

/* Savegame files */
JE_boolean playerPasswordInput;
JE_byte    inputDevice;  /* 0=Mouse   1=Joystick   2=Gravis GamePad */
JE_byte    gameSpeed;
JE_byte    processorType;  /* 1=386 2=486 3=Pentium Hyper */

JE_SaveFilesType saveFiles; /*array[1..saveLevelnum] of savefiletype;*/
JE_SaveGameTemp saveTemp;

JE_word editorLevel;   /*Initial value 800*/

JE_word x;

const JE_byte StringCryptKey[10] = {99, 204, 129, 63, 255, 71, 19, 25, 62, 1};

void JE_decryptString( char *s, JE_byte len )
{
	int i;

	for (i = len-1; i >= 0; i--)
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
	int size;

	size = getc(f);
	fseek(f, size, SEEK_CUR);
}


void JE_setupStars( void )
{
	int z;

	for (z = MAX_STARS; z--; )
	{
		starDat[z].sLoc = (mt_rand() % 320) + (mt_rand() % 200) * VGAScreen->pitch;
		starDat[z].sMov = ((mt_rand() % 3) + 2) * VGAScreen->pitch;
		starDat[z].sC = (mt_rand() % 16) + (9 * 16);
	}
}

void JE_saveGame( JE_byte slot, char *name )
{
	int i;

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

	memcpy(&saveFiles[slot-1].items, &pItems, sizeof(pItems));

	if (superArcadeMode > 253)
	{
		pItems[3-1] = 0;
	}
	if (twoPlayerMode)
	{
		memcpy(&saveFiles[slot-1].lastItems, &pItemsPlayer2, sizeof(pItemsPlayer2));
	} else {
		memcpy(&saveFiles[slot-1].lastItems, &pItemsBack2, sizeof(pItemsBack2));
	}

	saveFiles[slot-1].score  = score;
	saveFiles[slot-1].score2 = score2;
	memcpy(&saveFiles[slot-1].levelName, &lastLevelName, sizeof(lastLevelName));
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

	for (x = 0; x < 2; x++)
	{
		saveFiles[slot-1].power[x] = portPower[x];
	}

	JE_saveConfiguration();
}

void JE_loadGame( JE_byte slot )
{
	JE_byte temp5;

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
		memcpy(&pItemsPlayer2, &saveFiles[slot-1].lastItems, sizeof(pItemsPlayer2));
		onePlayerAction = false;
	} else {
		memcpy(&pItemsBack2, &saveFiles[slot-1].lastItems, sizeof(pItemsBack2));
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

	for (temp = 0; temp < 2; temp++)
	{
		portPower[temp] = saveFiles[slot-1].power[temp];
	}

	temp5 = saveFiles[slot-1].episode;

	memcpy(&levelName, &saveFiles[slot-1].levelName, sizeof(levelName));

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
	memcpy(&lastLevelName, &levelName, sizeof(levelName));
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
	JE_SaveGameTemp s2, s3;
	char c;
	JE_word x;
	JE_byte y, z;

	memcpy(&s3, &saveTemp, sizeof(s3));

	y = 0;
	for (x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y += s3[x];
	}
	saveTemp[SAVE_FILE_SIZE] = y;

	y = 0;
	for (x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y -= s3[x];
	}
	saveTemp[SAVE_FILE_SIZE+1] = y;

	y = 1;
	for (x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y = (y * s3[x]) + 1;
	}
	saveTemp[SAVE_FILE_SIZE+2] = y;

	y = 0;
	for (x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y = y ^ s3[x];
	}
	saveTemp[SAVE_FILE_SIZE+3] = y;

	for (x = 0; x < SAVE_FILE_SIZE; x++)
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
	JE_boolean correct = true;
	JE_SaveGameTemp s2;
	/*JE_word x;*/
	int x;
	JE_byte y, z;

	/* Decrypt save game file */
	for (x = (SAVE_FILE_SIZE - 1); x >= 0; x--)
	{
		s2[x] = (JE_byte)saveTemp[x] ^ (JE_byte)(cryptKey[(x+1) % 10]);
		if (x > 0)
		{
			s2[x] ^= (JE_byte)saveTemp[x - 1];
		}

	}

	/* for (x = 0; x < SAVE_FILE_SIZE; x++) printf("%c", s2[x]); */

	/* Check save file for correctitude */
	y = 0;
	for (x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y += s2[x];
	}
	if (saveTemp[SAVE_FILE_SIZE] != y)
	{
		correct = false;
		printf("Failed additive checksum: %d vs %d\n", saveTemp[SAVE_FILE_SIZE], y);
	}

	y = 0;
	for (x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y -= s2[x];
	}
	if (saveTemp[SAVE_FILE_SIZE+1] != y)
	{
		correct = false;
		printf("Failed subtractive checksum: %d vs %d\n", saveTemp[SAVE_FILE_SIZE+1], y);
	}

	y = 1;
	for (x = 0; x < SAVE_FILE_SIZE; x++)
	{
		y = (y * s2[x]) + 1;
	}
	if (saveTemp[SAVE_FILE_SIZE+2] != y)
	{
		correct = false;
		printf("Failed multiplicative checksum: %d vs %d\n", saveTemp[SAVE_FILE_SIZE+2], y);
	}

	y = 0;
	for (x = 0; x < SAVE_FILE_SIZE; x++)
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
	memcpy(&saveTemp, &s2, sizeof(s2));
}

#ifndef TARGET_MACOSX
const char *get_user_directory( void )
{
	static char userdir[500] = "";
#ifdef TARGET_UNIX
	if (strlen(userdir) == 0 && getenv("HOME"))
		snprintf(userdir, sizeof(userdir), "%s/.opentyrian/", getenv("HOME"));
#endif /* TARGET_UNIX */
	return userdir;
}
#endif /* TARGET_MACOSX */

void JE_loadConfiguration( void )
{
	FILE *fi;
	int z;
	JE_byte *p;
	JE_byte junk;
	int y;
	
	errorActive = true;
	
	char cfgfile[1000];
	snprintf(cfgfile, sizeof(cfgfile), "%s" "tyrian.cfg", get_user_directory());
	
	fi = fopen_check(cfgfile, "rb");
	if (fi && get_stream_size(fi) == 18 + sizeof(keySettings) + sizeof(joyButtonAssign))
	{
		/* SYN: I've hardcoded the sizes here because the .CFG file format is fixed
		   anyways, so it's not like they'll change. */
		background2 = 0;
		efread(&background2, 1, 1, fi);
		efread(&gameSpeed, 1, 1, fi);

		/* Wait what? */
		efread(&inputDevice, 1, 1, fi);
		inputDevice = 0;

		efread(&jConfigure, 1, 1, fi);
		if (jConfigure == 0) /* Dunno what this is about. */
		{
			jConfigure = 1;
		}

		efread(&versionNum, 1, 1, fi);
		if (resetVersion)
		{
			versionNum = 2; /* JE: {Shareware 1.0 and Registered 1.1 = 1} */
		}

		efread(&processorType, 1, 1, fi);
		efread(&junk, 1, 1, fi); /* Unused variable -- was "BLOCKREAD (f, midiport   , 1);" */
		efread(&soundEffects, 1, 1, fi);
		efread(&gammaCorrection, 1, 1, fi);
		efread(&difficultyLevel, 1, 1, fi);
		efread(joyButtonAssign, sizeof(*joyButtonAssign), COUNTOF(joyButtonAssign), fi); /* typically 4 bytes */

		efread(&tyrMusicVolume, 2, 1, fi);
		efread(&fxVolume, 2, 1, fi);

		efread(&inputDevice1, 1, 1, fi);
		efread(&inputDevice2, 1, 1, fi);

		efread(keySettings, sizeof(*keySettings), COUNTOF(keySettings), fi);

		/* display settings */
		Uint8 temp;
		
		efread(&temp, 1, 1, fi);
		fullscreen_enabled = (temp == true);
		
		efread(&temp, 1, 1, fi);
		scaler = temp;
		
		fclose(fi);
	} else {
		printf("\nInvalid or missing TYRIAN.CFG! Continuing using defaults.\n\n");
		
		memcpy(&joyButtonAssign, &defaultJoyButtonAssign, sizeof(joyButtonAssign));
		/*midiPort = 1;*/ /* We don't care about this. */
		soundEffects = 1;
		jConfigure = 0; /* TODO: Figure out what this is */
		memcpy(&keySettings, &defaultKeySettings, sizeof(keySettings));
		background2 = true;
		inputDevice = 0;
		tyrMusicVolume = 255;
		fxVolume = 128;
		gammaCorrection = 0;
		processorType = 3;
		gameSpeed = 4;
		inputDevice1 = 0;
		inputDevice2 = 0;

		fullscreen_enabled = false;
	}

	tyrMusicVolume = (tyrMusicVolume > 255) ? 255 : tyrMusicVolume;
	fxVolume = (fxVolume > 254) ? 254 : ((fxVolume < 14) ? 14 : fxVolume);

	soundActive = true;
	musicActive = true;

	JE_setVol(tyrMusicVolume, fxVolume);
	
	char savfile[1000];
	snprintf(savfile, sizeof(savfile), "%s" "tyrian.sav", get_user_directory());
	
	fi = fopen_check(savfile, "rb");
	if (fi)
	{

		fseek(fi, 0, SEEK_SET);
		efread(saveTemp, 1, sizeof(saveTemp), fi);
		JE_decryptSaveTemp();

		/* SYN: The original mostly blasted the save file into raw memory. However, our lives are not so
		   easy, because the C struct is necessarily a different size. So instead we have to loop
		   through each record and load fields manually. *emo tear* :'( */

		p = saveTemp;
		for (z = 0; z < SAVE_FILES_NUM; z++)
		{
			memcpy(&saveFiles[z].encode, p, sizeof(JE_word)); p += 2;
			saveFiles[z].encode = SDL_SwapLE16(saveFiles[z].encode);
			
			memcpy(&saveFiles[z].level, p, sizeof(JE_word)); p += 2;
			saveFiles[z].level = SDL_SwapLE16(saveFiles[z].level);
			
			memcpy(&saveFiles[z].items, p, sizeof(JE_PItemsType)); p += sizeof(JE_PItemsType);
			
			memcpy(&saveFiles[z].score, p, sizeof(JE_longint)); p += 4;
			saveFiles[z].score = SDL_SwapLE32(saveFiles[z].score);
			
			memcpy(&saveFiles[z].score2, p, sizeof(JE_longint)); p += 4;
			saveFiles[z].score2 = SDL_SwapLE32(saveFiles[z].score2);
			
			/* SYN: Pascal strings are prefixed by a byte holding the length! */
			memset(&saveFiles[z].levelName, 0, sizeof(saveFiles[z].levelName));
			memcpy(&saveFiles[z].levelName, &p[1], *p);
			p += 10;
			
			/* This was a BYTE array, not a STRING, in the original. Go fig. */
			memcpy(&saveFiles[z].name, p, 14);
			p += 14;
			
			memcpy(&saveFiles[z].cubes, p, sizeof(JE_byte)); p++;
			memcpy(&saveFiles[z].power, p, sizeof(JE_byte) * 2); p += 2;
			memcpy(&saveFiles[z].episode, p, sizeof(JE_byte)); p++;
			memcpy(&saveFiles[z].lastItems, p, sizeof(JE_PItemsType)); p += sizeof(JE_PItemsType);
			memcpy(&saveFiles[z].difficulty, p, sizeof(JE_byte)); p++;
			memcpy(&saveFiles[z].secretHint, p, sizeof(JE_byte)); p++;
			memcpy(&saveFiles[z].input1, p, sizeof(JE_byte)); p++;
			memcpy(&saveFiles[z].input2, p, sizeof(JE_byte)); p++;
			
			/* booleans were 1 byte in pascal -- working around it */
			Uint8 temp;
			memcpy(&temp, p, 1); p++;
			saveFiles[z].gameHasRepeated = temp != 0;
			
			memcpy(&saveFiles[z].initialDifficulty, p, sizeof(JE_byte)); p++;
			
			memcpy(&saveFiles[z].highScore1, p, sizeof(JE_longint)); p += 4;
			saveFiles[z].highScore1 = SDL_SwapLE32(saveFiles[z].highScore1);
			
			memcpy(&saveFiles[z].highScore2, p, sizeof(JE_longint)); p += 4;
			saveFiles[z].highScore2 = SDL_SwapLE32(saveFiles[z].highScore2);
			
			memset(&saveFiles[z].highScoreName, 0, sizeof(saveFiles[z].highScoreName));
			memcpy(&saveFiles[z].highScoreName, &p[1], *p);
			p += 30;
			
			memcpy(&saveFiles[z].highScoreDiff, p, sizeof(JE_byte)); p++;
		}

		/* SYN: This is truncating to bytes. I have no idea what this is doing or why. */
		/* TODO: Figure out what this is about and make sure it isn't broked. */
		editorLevel = JE_btow((JE_byte) (SIZEOF_SAVEGAMETEMP - 6), (JE_byte) (SIZEOF_SAVEGAMETEMP - 5));

		fclose(fi);
	} else {
		/* We didn't have a save file! Let's make up random stuff! */
		editorLevel = 800;

		for (z = 0; z < 100; z++)
		{
			saveTemp[SAVE_FILES_SIZE + z] = initialItemAvail[z];
		}

		for (z = 0; z < SAVE_FILES_NUM; z++)
		{
			saveFiles[z].level = 0;

			for (y = 0; y < 14; y++)
			{
				saveFiles[z].name[y] = ' ';
			}
			saveFiles[z].name[14] = 0;

			saveFiles[z].highScore1 = ((mt_rand() % 20) + 1) * 1000;

			if (z % 6 > 2)
			{
				saveFiles[z].highScore2 = ((mt_rand() % 20) + 1) * 1000;
				strcpy(saveFiles[z].highScoreName, defaultTeamNames[mt_rand() % 22]);
			} else {
				strcpy(saveFiles[z].highScoreName, defaultHighScoreNames[mt_rand() % 34]);
			}
		}
	}

	errorActive = false;

	JE_calcFXVol();
	JE_initProcessorType();
}

void JE_saveConfiguration( void )
{
#ifdef TARGET_UNIX
	if (getenv("HOME"))
	{
		char dir[1000];
		snprintf(dir, sizeof(dir), "%s/.opentyrian", getenv("HOME"));
		mkdir(dir, 0755);
	}
#endif /* HOME */
	
	FILE *f;
	JE_byte *p, junk = 0;
	int z;

	p = saveTemp;
	for (z = 0; z < SAVE_FILES_NUM; z++)
	{
		JE_SaveFileType tempSaveFile;
		memcpy(&tempSaveFile, &saveFiles[z], sizeof(tempSaveFile));
		
		tempSaveFile.encode = SDL_SwapLE16(tempSaveFile.encode);
		memcpy(p, &tempSaveFile.encode, sizeof(JE_word)); p += 2;
		
		tempSaveFile.level = SDL_SwapLE16(tempSaveFile.level);
		memcpy(p, &tempSaveFile.level, sizeof(JE_word)); p += 2;
		
		memcpy(p, &tempSaveFile.items, sizeof(JE_PItemsType)); p += sizeof(JE_PItemsType);
		
		tempSaveFile.score = SDL_SwapLE32(tempSaveFile.score);
		memcpy(p, &tempSaveFile.score, sizeof(JE_longint)); p += 4;
		
		tempSaveFile.score2 = SDL_SwapLE32(tempSaveFile.score2);
		memcpy(p, &tempSaveFile.score2, sizeof(JE_longint)); p += 4;
		
		/* SYN: Pascal strings are prefixed by a byte holding the length! */
		memset(p, 0, sizeof(tempSaveFile.levelName));
		*p = strlen(tempSaveFile.levelName);
		memcpy(&p[1], &tempSaveFile.levelName, *p);
		p += 10;
		
		/* This was a BYTE array, not a STRING, in the original. Go fig. */
		memcpy(p, &tempSaveFile.name, 14);
		p += 14;
		
		memcpy(p, &tempSaveFile.cubes, sizeof(JE_byte)); p++;
		memcpy(p, &tempSaveFile.power, sizeof(JE_byte) * 2); p += 2;
		memcpy(p, &tempSaveFile.episode, sizeof(JE_byte)); p++;
		memcpy(p, &tempSaveFile.lastItems, sizeof(JE_PItemsType)); p += sizeof(JE_PItemsType);
		memcpy(p, &tempSaveFile.difficulty, sizeof(JE_byte)); p++;
		memcpy(p, &tempSaveFile.secretHint, sizeof(JE_byte)); p++;
		memcpy(p, &tempSaveFile.input1, sizeof(JE_byte)); p++;
		memcpy(p, &tempSaveFile.input2, sizeof(JE_byte)); p++;
		
		/* booleans were 1 byte in pascal -- working around it */
		Uint8 temp = tempSaveFile.gameHasRepeated != false;
		memcpy(p, &temp, 1); p++;
		
		memcpy(p, &tempSaveFile.initialDifficulty, sizeof(JE_byte)); p++;
		
		tempSaveFile.highScore1 = SDL_SwapLE32(tempSaveFile.highScore1);
		memcpy(p, &tempSaveFile.highScore1, sizeof(JE_longint)); p += 4;
		
		tempSaveFile.highScore2 = SDL_SwapLE32(tempSaveFile.highScore2);
		memcpy(p, &tempSaveFile.highScore2, sizeof(JE_longint)); p += 4;
		
		memset(p, 0, sizeof(tempSaveFile.highScoreName));
		*p = strlen(tempSaveFile.highScoreName);
		memcpy(&p[1], &tempSaveFile.highScoreName, *p);
		p += 30;
		
		memcpy(p, &tempSaveFile.highScoreDiff, sizeof(JE_byte)); p++;
	}
	
	saveTemp[SIZEOF_SAVEGAMETEMP - 6] = editorLevel >> 8;
	saveTemp[SIZEOF_SAVEGAMETEMP - 5] = editorLevel;
	
	JE_encryptSaveTemp();
	
	char savfile[1000];
	snprintf(savfile, sizeof(savfile), "%s" "tyrian.sav", get_user_directory());
	
	f = fopen_check(savfile, "wb");
	if (f)
	{
		efwrite(saveTemp, 1, sizeof(saveTemp), f);
		fclose(f);
#if (_BSD_SOURCE || _XOPEN_SOURCE >= 500)
		sync();
#endif
	}
	JE_decryptSaveTemp();
	
	char cfgfile[1000];
	snprintf(cfgfile, sizeof(cfgfile), "%s" "tyrian.cfg", get_user_directory());
	
	f = fopen_check(cfgfile, "wb");
	if (f)
	{
		efwrite(&background2, 1, 1, f);
		efwrite(&gameSpeed, 1, 1, f);
		efwrite(&inputDevice, 1, 1, f);
		efwrite(&jConfigure, 1, 1, f);
		efwrite(&versionNum, 1, 1, f);
		efwrite(&processorType, 1, 1, f);
		efwrite(&junk, 1, 1, f); /* This isn't needed. Was: fwrite(midiPort, 1, sizeof(midiPort), f);*/
		efwrite(&soundEffects, 1, 1, f);
		efwrite(&gammaCorrection, 1, 1, f);
		efwrite(&difficultyLevel, 1, 1, f);
		efwrite(joyButtonAssign, sizeof(*joyButtonAssign), COUNTOF(joyButtonAssign), f);

		efwrite(&tyrMusicVolume, 1, 2, f);
		efwrite(&fxVolume, 1, 2, f);

		efwrite(&inputDevice1, 1, 1, f);
		efwrite(&inputDevice2, 1, 1, f);

		efwrite(keySettings, sizeof(*keySettings), COUNTOF(keySettings), f);

		/* display settings */
		Uint8 temp;
		
		temp = fullscreen_enabled;
		efwrite(&temp, 1, 1, f);
		
		temp = scaler;
		efwrite(&temp, 1, 1, f);
		
		fclose(f);
#if (_BSD_SOURCE || _XOPEN_SOURCE >= 500)
		sync();
#endif
	}
}

// kate: tab-width 4; vim: set noet:
