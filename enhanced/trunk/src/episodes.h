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
#ifndef EPISODES_H
#define EPISODES_H

#include "opentyr.h"

#include "lvlmast.h"


/* Episodes and general data */

static const int FIRST_LEVEL = 1;
static const int EPISODE_MAX = 5;
static const int NUM_EPISODES = 4;

struct JE_WeaponType
{
	JE_word drain;
	int shotrepeat;
	int multi;
	JE_word weapani;
	int max;
	int tx, ty, aim;
	int attack[8], del[8]; /* [1..8] */
	int sx[8], sy[8]; /* [1..8] */
	int bx[8], by[8]; /* [1..8] */
	JE_word sg[8]; /* [1..8] */
	int acceleration, accelerationx;
	int circlesize;
	int sound;
	int trail;
	int shipblastfilter;
};

struct JE_WeaponPortType
{
	char name[31]; /* string [30] */
	int opnum;
	JE_word op[2][11]; /* [1..2, 1..11] */
	JE_word cost;
	JE_word itemgraphic;
	JE_word poweruse;
};

struct JE_PowerType
{
	char name[31]; /* string [30] */
	JE_word itemgraphic;
	int power;
	int speed;
	JE_word cost;
};

struct JE_SpecialType
{
	char name[31]; /* string [30] */
	JE_word itemgraphic;
	int pwr;
	int stype;
	JE_word wpn;
};

struct JE_OptionType
{
	char name[31]; /* string [30] */
	int pwr;
	JE_word itemgraphic;
	JE_word cost;
	int tr, option;
	int opspd;
	int ani;
	JE_word gr[20]; /* [1..20] */
	int wport;
	JE_word wpnum;
	int ammo;
	int stop;
	int icongr;
};

struct JE_ShieldType
{
	char name[31]; /* string [30] */
	int tpwr;
	int mpwr;
	JE_word itemgraphic;
	JE_word cost;
};

struct JE_ShipType
{
	char name[31]; /* string [30] */
	JE_word shipgraphic;
	JE_word itemgraphic;
	int ani;
	int spd;
	int dmg;
	JE_word cost;
	int bigshipgraphic;
};

/* EnemyData */
struct JE_EnemyDatType
{
	int ani;
	int tur[3]; /* [1..3] */
	int freq[3]; /* [1..3] */
	int xmove;
	int ymove;
	int xaccel;
	int yaccel;
	int xcaccel;
	int ycaccel;
	int startx;
	int starty;
	int startxc;
	int startyc;
	int armor;
	int esize;
	JE_word egraphic[20];  /* [1..20] */
	int explosiontype;
	int animate;       /* 0:Not Yet   1:Always   2:When Firing Only */
	int shapebank;     /* See LEVELMAK.DOC */
	int xrev, yrev;
	JE_word dgr;
	int dlevel;
	int dani;
	int elaunchfreq;
	JE_word elaunchtype;
	int value;
	JE_word eenemydie;
};

extern JE_WeaponType weapons[WEAP_NUM + 1]; /* [0..weapnum] */
extern JE_WeaponPortType weaponPort[PORT_NUM + 1]; /* [0..portnum] */
extern JE_PowerType powerSys[POWER_NUM + 1]; /* [0..powernum] */
extern JE_SpecialType special[SPECIAL_NUM + 1]; /* [0..specialnum] */
extern JE_OptionType options[OPTION_NUM + 1]; /* [0..optionnum] */
extern JE_ShieldType shields[SHIELD_NUM + 1]; /* [0..shieldnum] */
extern JE_ShipType ships[SHIP_NUM + 1]; /* [0..shipnum] */
extern JE_EnemyDatType enemyDat[ENEMY_NUM + 1]; /* [0..enemynum] */
extern int episodeNum;
extern bool episodeAvail[EPISODE_MAX];
extern char macroFile[13], cubeFile[13];
extern unsigned long episode1DataLoc;
extern bool bonusLevel;
extern bool jumpBackToEpisode1;

void JE_loadItemDat( void );
void JE_initEpisode( int newEpisode );
bool JE_findNextEpisode( void );
void JE_scanForEpisodes( void );

#endif /* EPISODES_H */
