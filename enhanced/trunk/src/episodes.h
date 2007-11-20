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
#ifndef EPISODES_H
#define EPISODES_H

#include "opentyr.h"

#include "lvlmast.h"


/* Episodes and general data */

#define FIRST_LEVEL 1
#define EPISODE_MAX 5

typedef struct
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
} JE_WeaponType[WEAP_NUM + 1]; /* [0..weapnum] */

typedef struct
{
	char name[31]; /* string [30] */
	int opnum;
	JE_word op[2][11]; /* [1..2, 1..11] */
	JE_word cost;
	JE_word itemgraphic;
	JE_word poweruse;
} JE_WeaponPortType[PORT_NUM + 1]; /* [0..portnum] */

typedef struct
{
	char name[31]; /* string [30] */
	JE_word itemgraphic;
	int power;
	int speed;
	JE_word cost;
} JE_PowerType[POWER_NUM + 1]; /* [0..powernum] */

typedef struct
{
	char name[31]; /* string [30] */
	JE_word itemgraphic;
	int pwr;
	int stype;
	JE_word wpn;
} JE_SpecialType[SPECIAL_NUM + 1]; /* [0..specialnum] */

typedef struct
{
	char name[31]; /* string [30] */
	JE_word itemgraphic;
	int power;
	int speed;
	JE_word cost;
} JE_EngineType[ENGINE_NUM + 1]; /* [0..enginenum] */

typedef struct
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
	bool stop;
	int icongr;
} JE_OptionType[OPTION_NUM + 1]; /* [0..optionnum] */

typedef struct
{
	char name[31]; /* string [30] */
	int tpwr;
	int mpwr;
	JE_word itemgraphic;
	JE_word cost;
} JE_ShieldType[SHIELD_NUM + 1]; /* [0..shieldnum] */

typedef struct
{
	char    name[31]; /* string [30] */
	JE_word shipgraphic;
	JE_word itemgraphic;
	int ani;
	int spd;
	int dmg;
	JE_word cost;
	int bigshipgraphic;
} JE_ShipType[SHIP_NUM + 1]; /* [0..shipnum] */

/* EnemyData */
typedef struct
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
} JE_EnemyDatType[ENEMY_NUM + 1]; /* [0..enemynum] */

#ifndef NO_EXTERNS
extern JE_WeaponPortType weaponPort;
extern JE_WeaponType weapons;
extern JE_PowerType powerSys;
extern JE_ShipType ships;
extern JE_OptionType options;
extern JE_ShieldType shields;
extern JE_SpecialType special;
extern JE_EnemyDatType enemyDat;
extern int episodeNum;
extern bool episodeAvail[EPISODE_MAX];
extern char macroFile[13], cubeFile[13];
extern unsigned long episode1DataLoc;
extern bool bonusLevel;
extern bool jumpBackToEpisode1;
#endif

void JE_loadItemDat( void );
void JE_initEpisode( int newEpisode );
bool JE_findNextEpisode( void );
void JE_scanForEpisodes( void );

#endif /* EPISODES_H */
