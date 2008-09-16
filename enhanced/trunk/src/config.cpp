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
#include "Console.h"
#include "HighScores.h"
#include "CVar.h"

#include "config.h"

#include <fstream>
#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif

namespace fs = boost::filesystem;

/* Configuration Load/Save handler */

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


const char tyrian_ini_template[] = \
	"[video]\n"
	"game_speed = %d\n"
	"processor_type = %d\n"
	"gamma_correction = %d\n"
	"fullscreen = %s\n"
	"\n"
	"[sound]\n"
	"sound_effects = %s\n"
	"s_music_vol = %d\n"
	"s_fx_vol = %d\n"
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
int difficultyLevel, oldDifficultyLevel, initialDifficulty;

/* Player Stuff */
unsigned long score, score2;

JE_PItemsType pItems, pItemsPlayer2, pItemsBack, pItemsBack2;
int power, lastPower, powerAdd;
int shield, shieldMax, shieldSet;
int shield2, shieldMax2;
int armorLevel, armorLevel2;
int shieldWait, shieldT;

int special_recharge_time;
int shotRepeat[11], shotMultiPos[11]; /* [1..11] */  /* 7,8 = Superbomb */
int portConfig[10]; /* [1..10] */
bool portConfigDone;
JE_PortPowerType portPower, lastPortPower;


/* Level Data */
char lastLevelName[11], levelName[11]; /* string [10] */
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

bool fullscreen_enabled;

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

std::string JE_readCryptLn( std::ifstream& f )
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


void JE_setupStars( void )
{
	for (int z = MAX_STARS; z--; )
	{
		starDat[z].sLoc = (rand() % 320) + (rand() % 200) * scr_width;
		starDat[z].sMov = ((rand() % 3) + 2) * scr_width;
		starDat[z].sC = (rand() % 16) + (9 * 16);
	}
}

void JE_saveGame( int slot, const char *name )
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

	for (int temp = 0; temp < 2; temp++)
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

namespace CVars
{
	CVarInt game_speed("game_speed", CVar::CONFIG, "Game speed. 0-4", 3, rangeBind(0, 4));
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
	CVarInt gamma_correction("gamma_correction", CVar::CONFIG, "Gamma correction. 0-3", 0, rangeBind(0, 3));
	CVarBool fullscreen_enabled("fullscreen_enabled", CVar::CONFIG, "Fullscreen.", false);

	// Input devices // TODO will probably be removed
	CVarInt input_dev1("input_dev1", CVar::CONFIG, "Input device for player 1. 1-3", 1, rangeBind(1, 3));
	CVarInt input_dev2("input_dev2", CVar::CONFIG, "Input device for player 2. 1-3", 2, rangeBind(1, 3));
}

void JE_loadConfiguration( void )
{
	/*
	dictionary *ini = iniparser_new("tyrian.ini");

	gameSpeed = iniparser_getint(ini, "video:game_speed", 4);
	processorType = iniparser_getint(ini, "video:processor_type", 3);
	gammaCorrection = iniparser_getint(ini, "video:gamma_correction", 0);
	fullscreen_enabled = iniparser_getboolean(ini, "video:fullscreen", false);

	soundEffects = iniparser_getboolean(ini, "sound:sound_effects", true);
	tyrMusicVolume = iniparser_getint(ini, "sound:s_music_vol", 255);
	fxVolume = iniparser_getint(ini, "sound:s_fx_vol", 128);

	inputDevice1 = iniparser_getint(ini, "input:device1", 0);
	inputDevice2 = iniparser_getint(ini, "input:device2", 0);

	keySettings[0] = (SDLKey)iniparser_getint(ini, "keyboard:up", 273);
	keySettings[1] = (SDLKey)iniparser_getint(ini, "keyboard:down", 274);
	keySettings[2] = (SDLKey)iniparser_getint(ini, "keyboard:left", 276);
	keySettings[3] = (SDLKey)iniparser_getint(ini, "keyboard:right", 275);
	keySettings[4] = (SDLKey)iniparser_getint(ini, "keyboard:fire", 32);
	keySettings[5] = (SDLKey)iniparser_getint(ini, "keyboard:change_fire", 13);
	keySettings[6] = (SDLKey)iniparser_getint(ini, "keyboard:left_sidekick", 306);
	keySettings[7] = (SDLKey)iniparser_getint(ini, "keyboard:right_sidekick", 308);

	joyButtonAssign[0] = iniparser_getint(ini, "joystick:button1", 1);
	joyButtonAssign[1] = iniparser_getint(ini, "joystick:button2", 4);
	joyButtonAssign[2] = iniparser_getint(ini, "joystick:button3", 5);
	joyButtonAssign[3] = iniparser_getint(ini, "joystick:button4", 5);

	iniparser_free(ini);

	tyrMusicVolume = (tyrMusicVolume > 255) ? 255 : tyrMusicVolume;
	fxVolume = (fxVolume > 254) ? 254 : ((fxVolume < 14) ? 14 : fxVolume);

	soundActive = true;
	musicActive = true;
	*/

	for (int i = 0; i < SAVE_FILES_NUM; i++)
	{
		JE_SaveFileType& save = saveFiles[i];

		std::string fname = std::string("save/tyrian") + char(i<11 ? '1' : '2') + char(i==10 || i==21 ? 'L' : '0'+(i<11 ? i : i-11)) + ".sav";
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
			save.levelName[f.getStr().copy(save.levelName, 10)] = '\0';
			save.name[f.getStr().copy(save.name, 14)] = '\0';
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
			for (int i = 0; i < 14; i++)
			{
				save.name[i] = ' ';
			}
			save.name[14] = '\0';
		}
	}

	std::ifstream file("save/tyrian.hi", std::ios_base::in | std::ios_base::binary);
	if (file)
	{
		IBinaryStream s(file);
		highScores.unserialize(s);
	} // Making up random stuff handled by the default ctor

	JE_initProcessorType();
}

void JE_saveConfiguration( void )
{
	try
	{
		fs::create_directory("save"); // Does nothing if dir already exists
	}
	catch (const fs::filesystem_error& ex)
	{
		Console::get() << "\a7Error:\ax Unable to create save directory: " << ex.what() << '\n'
			<< "Saving disabled." << std::endl;
		return;
	}

	{
		std::ofstream file("save/tyrian.hi", std::ios_base::out | std::ios_base::binary);

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
			std::string fname = std::string("save/tyrian") + char(i<11 ? '1' : '2') + char(i==10 || i==21 ? 'L' : '0'+(i<11 ? i : i-11)) + ".sav";
			std::ofstream file(fname.c_str(), std::ios_base::out | std::ios_base::binary);

			if (!file)
			{
				std::cerr << "Failed to write " << fname << std::endl;
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

	FILE *ini = fopen("tyrian.ini", "w");
	if (ini == NULL)
	{
		Console::get() << "Couldn't write tyrian.ini." << std::endl;
	} else {
		// YKS: Yes, this is horrible, need to come up with a better way
/*		fprintf(ini, tyrian_ini_template,
			gameSpeed, processorType, gammaCorrection, (fullscreen_enabled ? "true" : "false"), // [video]
			(soundEffects ? "true" : "false"), tyrMusicVolume, fxVolume, // [sound]
			inputDevice1, inputDevice2, // [input]
			keySettings[0], keySettings[1], keySettings[2], keySettings[3], // [keyboard]
			keySettings[4], keySettings[5], keySettings[6], keySettings[7],
			joyButtonAssign[0], joyButtonAssign[1], joyButtonAssign[2], joyButtonAssign[3] // [joystick]
		);*/
		fclose(ini);
#if (_BSD_SOURCE || _XOPEN_SOURCE >= 500)
		sync();
#endif
	}
}

void scan_autorun( )
{
	try
	{
		fs::create_directory("autorun"); // Does nothing if dir already exists
	}
	catch (const fs::filesystem_error& ex)
	{
		Console::get() << "\a7Error:\ax Unable to create autorun directory: " << ex.what() << std::endl;
		return;
	}

	fs::directory_iterator end_iter;
	for (fs::directory_iterator dir_iter("autorun"); dir_iter != end_iter; ++dir_iter)
	{
		if (fs::is_regular(dir_iter->status()))
		{
			fs::ifstream stream(dir_iter->path().string(), std::ios_base::in);
			Console::get().runScript(dir_iter->path().leaf(), stream);
		}
	}
}
