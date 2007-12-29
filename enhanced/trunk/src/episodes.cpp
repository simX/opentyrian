/* vim: set noet:
 *
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

#include "error.h"
#include "lvllib.h"
#include "lvlmast.h"

#include "episodes.h"


/* MAIN Weapons Data */
JE_WeaponPortType weaponPort[PORT_NUM + 1];
JE_WeaponType     weapons[WEAP_NUM + 1];

/* Items */
JE_PowerType   powerSys[POWER_NUM + 1];
JE_ShipType    ships[SHIP_NUM + 1];
JE_OptionType  options[OPTION_NUM + 1];
JE_ShieldType  shields[SHIELD_NUM + 1];
JE_SpecialType special[SPECIAL_NUM + 1];

/* Enemy data */
JE_EnemyDatType enemyDat[ENEMY_NUM + 1];

/* EPISODE variables */
int episodeNum = 0;
bool episodeAvail[EPISODE_MAX]; /* [1..episodemax] */
char macroFile[13], cubeFile[13]; /* string [12] */

unsigned long episode1DataLoc;

/* Tells the game whether the level currently loaded is a bonus level. */
bool bonusLevel;

/* Tells if the game jumped back to Episode 1 */
bool jumpBackToEpisode1;

void JE_loadItemDat( void )
{
	FILE *lvlFile;
	JE_word itemNum[7]; /* [1..7] */

	if (episodeNum > 3)
	{
		JE_resetFile(&lvlFile, levelFile);
		fseek(lvlFile, lvlPos[lvlNum-1], SEEK_SET);
	} else {
		JE_resetFile(&lvlFile, "tyrian.hdt");
		vfread(episode1DataLoc, Sint32, lvlFile);
		fseek(lvlFile, episode1DataLoc, SEEK_SET);
	}

	efread(&itemNum, sizeof(JE_word), 7, lvlFile);

	for (int i = 0; i < WEAP_NUM + 1; i++)
	{
		efread(&weapons[i].drain,           sizeof(JE_word), 1, lvlFile);
		vfread(weapons[i].shotrepeat,      Uint8, lvlFile);
		vfread(weapons[i].multi,           Uint8, lvlFile);
		efread(&weapons[i].weapani,         sizeof(JE_word), 1, lvlFile);
		vfread(weapons[i].max,             Uint8, lvlFile);
		vfread(weapons[i].tx,              Uint8, lvlFile);
		vfread(weapons[i].ty,              Uint8, lvlFile);
		vfread(weapons[i].aim,             Uint8, lvlFile);
		for (int j = 0; j < 8; j++)
			vfread(weapons[i].attack[j],   Uint8, lvlFile);
		for (int j = 0; j < 8; j++)
			vfread(weapons[i].del[j],      Uint8, lvlFile);
		// YKS: I broke the style rules to save space on this huge wall of text. Stab me.
		for (int j = 0; j < 8; j++)
			vfread(weapons[i].sx[j],       Sint8, lvlFile);
		for (int j = 0; j < 8; j++)
			vfread(weapons[i].sy[j],       Sint8, lvlFile);
		for (int j = 0; j < 8; j++)
			vfread(weapons[i].bx[j],       Sint8, lvlFile);
		for (int j = 0; j < 8; j++)
			vfread(weapons[i].by[j],       Sint8, lvlFile);
		efread(&weapons[i].sg,              sizeof(JE_word), 8, lvlFile);
		vfread(weapons[i].acceleration,    Sint8, lvlFile);
		vfread(weapons[i].accelerationx,   Sint8, lvlFile);
		vfread(weapons[i].circlesize,      Uint8, lvlFile);
		vfread(weapons[i].sound,           Uint8, lvlFile);
		vfread(weapons[i].trail,           Uint8, lvlFile);
		vfread(weapons[i].shipblastfilter, Uint8, lvlFile);
	}

	for (int i = 0; i < PORT_NUM + 1; i++)
	{
		fseek(lvlFile, 1, SEEK_CUR); /* skip string length */
		efread(&weaponPort[i].name,        1, 30, lvlFile);
		weaponPort[i].name[30] = '\0';
		vfread(weaponPort[i].opnum,       Uint8, lvlFile);
		for (int j = 0; j < 2; j++)
		{
			efread(&weaponPort[i].op[j],   sizeof(JE_word), 11, lvlFile);
		}
		efread(&weaponPort[i].cost,        sizeof(JE_word), 1, lvlFile);
		efread(&weaponPort[i].itemgraphic, sizeof(JE_word), 1, lvlFile);
		efread(&weaponPort[i].poweruse,    sizeof(JE_word), 1, lvlFile);
	}

	for (int i = 0; i < SPECIAL_NUM + 1; i++)
	{
		fseek(lvlFile, 1, SEEK_CUR); /* skip string length */
		efread(&special[i].name,        1, 30, lvlFile);
		special[i].name[30] = '\0';
		efread(&special[i].itemgraphic, sizeof(JE_word), 1, lvlFile);
		vfread(special[i].pwr,         Uint8, lvlFile);
		vfread(special[i].stype,       Uint8, lvlFile);
		efread(&special[i].wpn,         sizeof(JE_word), 1, lvlFile);
	}

	for (int i = 0; i < POWER_NUM + 1; i++)
	{
		fseek(lvlFile, 1, SEEK_CUR); /* skip string length */
		efread(&powerSys[i].name,        1, 30, lvlFile);
		powerSys[i].name[30] = '\0';
		efread(&powerSys[i].itemgraphic, sizeof(JE_word), 1, lvlFile);
		vfread(powerSys[i].power,       Uint8, lvlFile);
		vfread(powerSys[i].speed,       Uint8, lvlFile);
		efread(&powerSys[i].cost,        sizeof(JE_word), 1, lvlFile);
	}

	for (int i = 0; i < SHIP_NUM + 1; i++)
	{
		fseek(lvlFile, 1, SEEK_CUR); /* skip string length */
		efread(&ships[i].name,           1, 30, lvlFile);
		ships[i].name[30] = '\0';
		efread(&ships[i].shipgraphic,    sizeof(JE_word), 1, lvlFile);
		efread(&ships[i].itemgraphic,    sizeof(JE_word), 1, lvlFile);
		vfread(ships[i].ani,            Uint8, lvlFile);
		vfread(ships[i].spd,            Uint8, lvlFile);
		vfread(ships[i].dmg,            Uint8, lvlFile);
		efread(&ships[i].cost,           sizeof(JE_word), 1, lvlFile);
		vfread(ships[i].bigshipgraphic, Uint8, lvlFile);
	}

	for (int i = 0; i < OPTION_NUM + 1; i++)
	{
		fseek(lvlFile, 1, SEEK_CUR); /* skip string length */
		efread(&options[i].name,        1, 30, lvlFile);
		options[i].name[30] = '\0';
		vfread(options[i].pwr,         Uint8, lvlFile);
		efread(&options[i].itemgraphic, sizeof(JE_word), 1, lvlFile);
		efread(&options[i].cost,        sizeof(JE_word), 1, lvlFile);
		vfread(options[i].tr,          Uint8, lvlFile);
		vfread(options[i].option,      Uint8, lvlFile);
		vfread(options[i].opspd,       Sint8, lvlFile);
		vfread(options[i].ani,         Uint8, lvlFile);
		efread(&options[i].gr,          sizeof(JE_word), 20, lvlFile);
		vfread(options[i].wport,       Uint8, lvlFile);
		efread(&options[i].wpnum,       sizeof(JE_word), 1, lvlFile);
		vfread(options[i].ammo,        Uint8, lvlFile);
		vfread(options[i].stop,        Uint8, lvlFile);
		vfread(options[i].icongr,      Uint8, lvlFile);
	}

	for (int i = 0; i < SHIELD_NUM + 1; i++)
	{
		fseek(lvlFile, 1, SEEK_CUR); /* skip string length */
		efread(&shields[i].name,        1, 30, lvlFile);
		shields[i].name[30] = '\0';
		vfread(shields[i].tpwr,        Uint8, lvlFile);
		vfread(shields[i].mpwr,        Uint8, lvlFile);
		efread(&shields[i].itemgraphic, sizeof(JE_word), 1, lvlFile);
		efread(&shields[i].cost,        sizeof(JE_word), 1, lvlFile);
	}

	for (int i = 0; i < ENEMY_NUM + 1; i++)
	{
		vfread(enemyDat[i].ani,           Uint8, lvlFile);
		for (int j = 0; j < 3; j++)
			vfread(enemyDat[i].tur[j],       Uint8, lvlFile);
		for (int j = 0; j < 3; j++)
			vfread(enemyDat[i].freq[j],      Uint8, lvlFile);
		vfread(enemyDat[i].xmove,         Sint8, lvlFile);
		vfread(enemyDat[i].ymove,         Sint8, lvlFile);
		vfread(enemyDat[i].xaccel,        Sint8, lvlFile);
		vfread(enemyDat[i].yaccel,        Sint8, lvlFile);
		vfread(enemyDat[i].xcaccel,       Sint8, lvlFile);
		vfread(enemyDat[i].ycaccel,       Sint8, lvlFile);
		vfread(enemyDat[i].startx,        Sint16, lvlFile);
		vfread(enemyDat[i].starty,        Sint16, lvlFile);
		vfread(enemyDat[i].startxc,       Sint8, lvlFile);
		vfread(enemyDat[i].startyc,       Sint8, lvlFile);
		vfread(enemyDat[i].armor,         Uint8, lvlFile);
		vfread(enemyDat[i].esize,         Uint8, lvlFile);
		efread(&enemyDat[i].egraphic,      sizeof(JE_word), 20, lvlFile);
		vfread(enemyDat[i].explosiontype, Uint8, lvlFile);
		vfread(enemyDat[i].animate,       Uint8, lvlFile);
		vfread(enemyDat[i].shapebank,     Uint8, lvlFile);
		vfread(enemyDat[i].xrev,          Sint8, lvlFile);
		vfread(enemyDat[i].yrev,          Sint8, lvlFile);
		efread(&enemyDat[i].dgr,           sizeof(JE_word), 1, lvlFile);
		vfread(enemyDat[i].dlevel,        Sint8, lvlFile);
		vfread(enemyDat[i].dani,          Sint8, lvlFile);
		vfread(enemyDat[i].elaunchfreq,   Uint8, lvlFile);
		efread(&enemyDat[i].elaunchtype,   sizeof(JE_word), 1, lvlFile);
		vfread(enemyDat[i].value,         Sint16, lvlFile);
		efread(&enemyDat[i].eenemydie,     sizeof(JE_word), 1, lvlFile);
	}

	fclose(lvlFile);
}

void JE_initEpisode( int newEpisode )
{
	if (newEpisode != episodeNum)
	{
		episodeNum = newEpisode;

		sprintf(levelFile, "tyrian%d.lvl", episodeNum);
		sprintf(cubeFile,  "cubetxt%d.dat", episodeNum);
		sprintf(macroFile, "levels%d.dat", episodeNum);

		JE_analyzeLevel();
		JE_loadItemDat();
	}
}

void JE_scanForEpisodes( void )
{
	char buf[sizeof(dir) + 11];

	JE_findTyrian("tyrian1.lvl"); /* need to know where to scan */

	for (int temp = 0; temp < EPISODE_MAX; temp++)
	{
		sprintf(buf, "%styrian%d.lvl", dir, temp + 1);
		episodeAvail[temp] = JE_find(buf);
	}
}

bool JE_findNextEpisode( void )
{
	bool found = false;
	int newEpisode = episodeNum + 1;

	jumpBackToEpisode1 = false;

	while (!found)
	{
		if (newEpisode > EPISODE_MAX)
		{
			newEpisode = 1;
			jumpBackToEpisode1 = true;
		}

		if (episodeAvail[newEpisode-1])
		{
			found = true;
		}

		newEpisode++;
	}
	newEpisode--;

	if (found)
	{
		JE_initEpisode(newEpisode);
	}

	return found;
}
