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
#include "lvlmast.h"
#include "Filesystem.h"
#include "BinaryStream.h"

#include "boost/format.hpp"

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
std::string macroFile; // string [12]
std::string cubeFile;

unsigned long episode1DataLoc;

/* Tells the game whether the level currently loaded is a bonus level. */
bool bonusLevel;

/* Tells if the game jumped back to Episode 1 */
bool jumpBackToEpisode1;

static JE_word lvlNum;
unsigned long lvlPos[43];
std::string levelFile; /* string [12] */

void JE_loadItemDat( void )
{
	std::fstream lvlFile;
	JE_word itemNum[7]; /* [1..7] */

	IBinaryStream bs(lvlFile);

	if (episodeNum > 3)
	{
		Filesystem::get().openDatafileFail(lvlFile, levelFile);
		lvlFile.seekg(lvlPos[lvlNum-1]);
	} else {
		Filesystem::get().openDatafileFail(lvlFile, "tyrian.hdt");
		episode1DataLoc = bs.getS32();
		lvlFile.seekg(episode1DataLoc);
	}

	for (int i = 0; i < 7; ++i)
	{
		itemNum[i] = bs.get16();
	}

	for (int i = 0; i < WEAP_NUM + 1; i++)
	{
		weapons[i].drain = bs.get16();
		weapons[i].shotrepeat = bs.get8();
		weapons[i].multi = bs.get8();
		weapons[i].weapani = bs.get16();
		weapons[i].max = bs.get8();
		weapons[i].tx = bs.get8();
		weapons[i].ty = bs.get8();
		weapons[i].aim = bs.get8();
		for (int j = 0; j < 8; j++)
			weapons[i].attack[j] = bs.get8();
		for (int j = 0; j < 8; j++)
			weapons[i].del[j] = bs.get8();
		for (int j = 0; j < 8; j++)
			weapons[i].sx[j] = bs.getS8();
		for (int j = 0; j < 8; j++)
			weapons[i].sy[j] = bs.getS8();
		for (int j = 0; j < 8; j++)
			weapons[i].bx[j] = bs.getS8();
		for (int j = 0; j < 8; j++)
			weapons[i].by[j] = bs.getS8();
		for (int j = 0; j < 8; j++)
			weapons[i].sg[j] = bs.get16();
		weapons[i].acceleration = bs.getS8();
		weapons[i].accelerationx = bs.getS8();
		weapons[i].circlesize = bs.get8();
		weapons[i].sound = bs.get8();
		weapons[i].trail = bs.get8();
		weapons[i].shipblastfilter = bs.get8();
	}

	for (unsigned int i = 0; i < PORT_NUM + 1; i++)
	{
		weaponPort[i].name = bs.getStr();
		weaponPort[i].opnum = bs.get8();
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 11; ++k)
			{
				weaponPort[i].op[j][k] = bs.get16();
			}
		}
		weaponPort[i].cost = bs.get16();
		weaponPort[i].itemgraphic = bs.get16();
		weaponPort[i].poweruse = bs.get16();
	}

	for (int i = 0; i < SPECIAL_NUM + 1; i++)
	{
		special[i].name = bs.getStr();
		special[i].itemgraphic = bs.get16();
		special[i].pwr = bs.get8();
		special[i].stype = bs.get8();
		special[i].wpn = bs.get16();
	}

	for (int i = 0; i < POWER_NUM + 1; i++)
	{
		powerSys[i].name = bs.getStr();
		powerSys[i].itemgraphic = bs.get16();
		powerSys[i].power = bs.get8();
		powerSys[i].speed = bs.get8();
		powerSys[i].cost = bs.get16();
	}

	for (int i = 0; i < SHIP_NUM + 1; i++)
	{
		ships[i].name = bs.getStr();
		ships[i].shipgraphic = bs.get16();
		ships[i].itemgraphic = bs.get16();
		ships[i].ani = bs.get8();
		ships[i].spd = bs.get8();
		ships[i].dmg = bs.get8();
		ships[i].cost = bs.get16();
		ships[i].bigshipgraphic = bs.get8();
	}

	for (int i = 0; i < OPTION_NUM + 1; i++)
	{
		options[i].name = bs.getStr();
		options[i].pwr = bs.get8();
		options[i].itemgraphic = bs.get16();
		options[i].cost = bs.get16();
		options[i].tr = bs.get8();
		options[i].option = bs.get8();
		options[i].opspd = bs.getS8();
		options[i].ani = bs.get8();
		for (int j = 0; j < 20; ++j)
			options[i].gr[j] = bs.get16();
		options[i].wport = bs.get8();
		options[i].wpnum = bs.get16();
		options[i].ammo = bs.get8();
		options[i].stop = bs.get8();
		options[i].icongr = bs.get8();
	}

	for (int i = 0; i < SHIELD_NUM + 1; i++)
	{
		shields[i].name = bs.getStr();
		shields[i].tpwr = bs.get8();
		shields[i].mpwr = bs.get8();
		shields[i].itemgraphic = bs.get16();
		shields[i].cost = bs.get16();
	}

	for (int i = 0; i < ENEMY_NUM + 1; i++)
	{
		enemyDat[i].ani = bs.get8();
		for (int j = 0; j < 3; j++)
			enemyDat[i].tur[j] = bs.get8();
		for (int j = 0; j < 3; j++)
			enemyDat[i].freq[j] = bs.get8();
		enemyDat[i].xmove = bs.getS8();
		enemyDat[i].ymove = bs.getS8();
		enemyDat[i].xaccel = bs.getS8();
		enemyDat[i].yaccel = bs.getS8();
		enemyDat[i].xcaccel = bs.getS8();
		enemyDat[i].ycaccel = bs.getS8();
		enemyDat[i].startx = bs.getS16();
		enemyDat[i].starty = bs.getS16();
		enemyDat[i].startxc = bs.getS8();
		enemyDat[i].startyc = bs.getS8();
		enemyDat[i].armor = bs.get8();
		enemyDat[i].esize = bs.get8();
		for (int j = 0; j < 20; ++j)
			enemyDat[i].egraphic[j] = bs.get16();
		enemyDat[i].explosiontype = bs.get8();
		enemyDat[i].animate = bs.get8();
		enemyDat[i].shapebank = bs.get8();
		enemyDat[i].xrev = bs.getS8();
		enemyDat[i].yrev = bs.getS8();
		enemyDat[i].dgr = bs.get16();
		enemyDat[i].dlevel = bs.getS8();
		enemyDat[i].dani = bs.getS8();
		enemyDat[i].elaunchfreq = bs.get8();
		enemyDat[i].elaunchtype = bs.get16();
		enemyDat[i].value = bs.getS16();
		enemyDat[i].eenemydie = bs.get16();
	}

	lvlFile.close();
}

static void JE_analyzeLevel( void )
{
	std::fstream f;
	Filesystem::get().openDatafileFail(f, levelFile);

	IBinaryStream bs(f);

	lvlNum = bs.get16();
	for (int i = 0; i < lvlNum; ++i)
	{
		lvlPos[i] = bs.getS32();
	}
	f.seekg(0, std::ios::end);
	lvlPos[lvlNum] = f.tellg();
	f.close();
}

void JE_initEpisode( int newEpisode )
{
	if (newEpisode != episodeNum)
	{
		episodeNum = newEpisode;

		levelFile = (boost::format("tyrian%1%.lvl") % episodeNum).str();
		cubeFile = (boost::format("cubetxt%1%.dat") % episodeNum).str();
		macroFile = (boost::format("levels%1%.dat") % episodeNum).str();

		JE_analyzeLevel();
		JE_loadItemDat();
	}
}

void JE_scanForEpisodes( void )
{
	for (int i = 0; i < EPISODE_MAX; i++)
	{
		const std::string fname = std::string("tyrian")+char('1'+i)+".lvl";
		try
		{
			// This will throw if it fails
			Filesystem::get().findDatafile(fname);
			// And thus this won't run
			episodeAvail[i] = true;
		}
		catch (Filesystem::FileOpenErrorException&)
		{
			episodeAvail[i] = false;
		}
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
