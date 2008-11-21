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
#include "varz.h"

#include "config.h"
#include "editship.h"
#include "episodes.h"
#include "joystick.h"
#include "network.h"
#include "newshape.h"
#include "nortsong.h"
#include "video.h"
#include "vga256d.h"
#include "fonthand.h"
#include "loudness.h"
#include "explosion.h"
#include "mtrand.h"

#include <cmath>
#include <sstream>


int fromTime;
int tempDat, tempDat2, tempDat3;
bool tempb2;

JE_word test1;

const int SANextShip[SA + 2] /* [0..SA + 1] */ = { 3, 8, 6, 2, 5, 1, 4, 3, 7};
const JE_word SASpecialWeapon[SA] /* [1..SA] */  = { 7, 8, 9,10,11,12,13};
const JE_word SASpecialWeaponB[SA] /* [1..SA] */ = {37, 6,15,40,16,14,41};
const int SAShip[SA] /* [1..SA] */ = {3,1,5,10,2,11,12};
const JE_word SAWeapon[SA][5] /* [1..SA, 1..5] */ =
{  /* R Bl Bk G  P */
	{ 9,31,32,33,34},  /* Stealth Ship */
	{19, 8,22,41,34},  /* StormWind    */
	{27, 5,20,42,31},  /* Techno       */
	{15, 3,28,22,12},  /* Enemy        */
	{23,35,25,14, 6},  /* Weird        */
	{ 2, 5,21, 4, 7},  /* Unknown      */
	{40,38,37,41,36}   /* NortShip Z   */
};

const int specialArcadeWeapon[PORT_NUM] /* [1..Portnum] */ =
{
	17,17,18,0,0,0,10,0,0,0,0,0,44,0,10,0,19,0,0,-0,0,0,0,0,0,0,
	-0,0,0,0,45,0,0,0,0,0,0,0,0,0,0,0
};

const int optionSelect[16][3][2] /* [0..15, 1..3, 1..2] */ =
{	/*  MAIN    OPT    FRONT */
	{ { 0, 0},{ 0, 0},{ 0, 0} },  /**/
	{ { 1, 1},{16,16},{30,30} },  /*Single Shot*/
	{ { 2, 2},{29,29},{29,20} },  /*Dual Shot*/
	{ { 3, 3},{21,21},{12, 0} },  /*Charge Cannon*/
	{ { 4, 4},{18,18},{16,23} },  /*Vulcan*/
	{ { 0, 0},{ 0, 0},{ 0, 0} },  /**/
	{ { 6, 6},{29,16},{ 0,22} },  /*Super Missile*/
	{ { 7, 7},{19,19},{19,28} },  /*Atom Bomb*/
	{ { 0, 0},{ 0, 0},{ 0, 0} },  /**/
	{ { 0, 0},{ 0, 0},{ 0, 0} },  /**/
	{ {10,10},{21,21},{21,27} },  /*Mini Missile*/
	{ { 0, 0},{ 0, 0},{ 0, 0} },  /**/
	{ { 0, 0},{ 0, 0},{ 0, 0} },  /**/
	{ {13,13},{17,17},{13,26} },  /*MicroBomb*/
	{ { 0, 0},{ 0, 0},{ 0, 0} },  /**/
	{ {15,15},{15,16},{15,16} }   /*Post-It*/
};

const JE_word PGR[21] /* [1..21] */ =
{
	4,
	1,2,3,
	41-21,57-21,73-21,89-21,105-21,
	121-21,137-21,153-21,
	151,151,151,151,73-21,73-21,1,2,4
	/*151,151,151*/
};
const int PAni[21] /* [1..21] */ = {1,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1};

const int pItemButtonMap[7] /* [1..7] */ = {12,1,2,10,6,4,5}; /*Financial Stuff*/

const JE_word linkGunWeapons[38] /* [1..38] */ =
{
	0,0,0,0,0,0,0,0,444,445,446,447,0,448,449,0,0,0,0,0,450,451,0,506,0,564,
	  445,446,447,448,449,445,446,447,448,449,450,451
};
const JE_word chargeGunWeapons[38] /* [1..38] */ =
{
	0,0,0,0,0,0,0,0,476,458,464,482,0,488,470,0,0,0,0,0,494,500,0,528,0,558,
	  458,458,458,458,458,458,458,458,458,458,458,458
};
const JE_word linkMultiGr[17] /* [0..16] */ =
	{77,221,183,301,1,282,164,202,58,201,163,281,39,300,182,220,77};
const JE_word linkSonicGr[17] /* [0..16] */ =
	{85,242,131,303,47,284,150,223,66,224,149,283,9,302,130,243,85};
const JE_word linkMult2Gr[17] /* [0..16] */ =
	{78,299,295,297,2,278,276,280,59,279,275,277,40,296,294,298,78};

const int randomEnemyLaunchSounds[3] /* [1..3] */ = {13,6,26};

/* YKS: Twiddle cheat sheet:
 * 1: UP
 * 2: DOWN
 * 3: LEFT
 * 4: RIGHT
 * 5: UP+FIRE
 * 6: DOWN+FIRE
 * 7: LEFT+FIRE
 * 8: RIGHT+FIRE
 * 9: Release all keys (directions and fire)
 */
const int keyboardCombos[26][8] /* [1..26, 1..8] */ =
{
	{ 2, 1,   2,   5, 137,           0, 0, 0}, /*Invulnerability*/
	{ 4, 3,   2,   5, 138,           0, 0, 0}, /*Atom Bomb*/
	{ 3, 4,   6, 139,             0, 0, 0, 0}, /*Seeker Bombs*/
	{ 2, 5, 142,               0, 0, 0, 0, 0}, /*Ice Blast*/
	{ 6, 2,   6, 143,             0, 0, 0, 0}, /*Auto Repair*/
	{ 6, 7,   5,   8,   6,   7,  5, 112     }, /*Spin Wave*/
	{ 7, 8, 101,               0, 0, 0, 0, 0}, /*Repulsor*/
	{ 1, 7,   6, 146,             0, 0, 0, 0}, /*Protron Field*/
	{ 8, 6,   7,   1, 120,           0, 0, 0}, /*Minefield*/
	{ 3, 6,   8,   5, 121,           0, 0, 0}, /*Post-It Blast*/
	{ 1, 2,   7,   8, 119,           0, 0, 0}, /*Drone Ship - TBC*/
	{ 3, 4,   3,   6, 123,           0, 0, 0}, /*Repair Player 2*/
	{ 6, 7,   5,   8, 124,           0, 0, 0}, /*Super Bomb - TBC*/
	{ 1, 6, 125,               0, 0, 0, 0, 0}, /*Hot Dog*/
	{ 9, 5, 126,               0, 0, 0, 0, 0}, /*Lightning UP      */
	{ 1, 7, 127,               0, 0, 0, 0, 0}, /*Lightning UP+LEFT */
	{ 1, 8, 128,               0, 0, 0, 0, 0}, /*Lightning UP+RIGHT*/
	{ 9, 7, 129,               0, 0, 0, 0, 0}, /*Lightning    LEFT */
	{ 9, 8, 130,               0, 0, 0, 0, 0}, /*Lightning    RIGHT*/
	{ 4, 2,   3,   5, 131,           0, 0, 0}, /*Warfly            */
	{ 3, 1,   2,   8, 132,           0, 0, 0}, /*FrontBlaster      */
	{ 2, 4,   5, 133,             0, 0, 0, 0}, /*Gerund            */
	{ 3, 4,   2,   8, 134,           0, 0, 0}, /*FireBomb          */
	{ 1, 4,   6, 135,             0, 0, 0, 0}, /*Indigo            */
	{ 1, 3,   6, 137,             0, 0, 0, 0}, /*Invulnerability [easier] */
	{ 1, 4,   3,   4,   7, 136,         0, 0}  /*D-Media Protron Drone    */
};

const int shipCombosB[21] /* [1..21] */ =
	{15,16,17,18,19,20,21,22,23,24, 7, 8, 5,25,14, 4, 6, 3, 9, 2,26};
  /*!! SUPER Tyrian !!*/
const int superTyrianSpecials[4] /* [1..4] */ = {1,2,4,5};

const int shipCombos[14][3] /* [0..12, 1..3] */ =
{
	{ 5, 4, 7},  /*2nd Player ship*/
	{ 1, 2, 0},  /*USP Talon*/
	{14, 4, 0},  /*Super Carrot*/
	{ 4, 5, 0},  /*Gencore Phoenix*/
	{ 6, 5, 0},  /*Gencore Maelstrom*/
	{ 7, 8, 0},  /*MicroCorp Stalker*/
	{ 7, 9, 0},  /*MicroCorp Stalker-B*/
	{10, 3, 5},  /*Prototype Stalker-C*/
	{ 5, 8, 9},  /*Stalker*/
	{ 1, 3, 0},  /*USP Fang*/
	{ 7,16,17},  /*U-Ship*/
	{ 2,11,12},  /*1st Player ship*/
	{ 3, 8,10},  /*Nort ship*/
	{ 0, 0, 0}   // Dummy entry added for Stalker 21.126
};

/*Street-Fighter Commands*/
int SFCurrentCode[2][21]; /* [1..2, 1..21] */
int SFExecuted[2]; /* [1..2] */


/*Happy*/
bool stoppedDemo;

/*Special General Data*/
int lvlFileNum;
JE_word maxEvent, eventLoc;
/*JE_word maxenemies;*/
JE_word tempBackMove, explodeMove; /*Speed of background movement*/
int levelEnd;
JE_word levelEndFxWait;
int levelEndWarp;
bool showMemLeft, endLevel, reallyEndLevel, waitToEndLevel, playerEndLevel, normalBonusLevelCurrent,
     bonusLevelCurrent, smallEnemyAdjust, readyToEndLevel, quitRequested;

int newPL[10]; /* [0..9] */ /*Eventsys event 75 parameter*/
JE_word returnLoc;
bool returnActive;
JE_word galagaShotFreq;
unsigned long galagaLife;

bool debug; /*Debug Mode*/
unsigned long debugTime, lastDebugTime;
unsigned long debugHistCount;
float debugHist;
JE_word curLoc; /*Current Pixel location of background 1*/

bool firstGameOver, gameLoaded;

/* Error Checking */
JE_word tempSS;


/* Destruction Ratio */
JE_word totalEnemy;
JE_word enemyKilled;

int statBar[2], statCol[2]; /* [1..2] */

/* Buffer */
JE_Map1Buffer *map1BufferTop, *map1BufferBot;

/* Shape/Map Data - All in one Segment! */
JE_MegaDataType1 *megaData1;
JE_MegaDataType2 *megaData2;
JE_MegaDataType3 *megaData3;

/* Secret Level Display */
int flash;
int flashChange;
int displayTime;

/* Demo Stuff */
bool firstEvent;
std::fstream recordFile;
JE_word recordFileNum;
int lastKey[8]; /* [1..8] */
JE_word lastMoveWait, nextMoveWait;
int nextDemoOperation;
bool playDemo;
int playDemoNum;

/* Sound Effects Queue */
int soundQueue[8]; /* [0..7] */

/*Level Event Data*/
bool enemyContinualDamage;
bool enemiesActive;
bool forceEvents;
bool stopBackgrounds;
int stopBackgroundNum;
int damageRate;  /*Rate at which a player takes damage*/
bool background3x1;  /*Background 3 enemies use Background 1 X offset*/
bool background3x1b; /*Background 3 enemies moved 8 pixels left*/

bool levelTimer;
JE_word levelTimerCountdown;
JE_word levelTimerJumpTo;
bool randomExplosions;

bool editShip1, editShip2;

bool globalFlags[10]; /* [1..10] */
int levelSong;

bool drawGameSaved;

bool repause;

/* DESTRUCT game */
bool loadDestruct;

/* MapView Data */
JE_word mapOrigin, mapPNum;
int mapPlanet[5], mapSection[5]; /* [1..5] */

/* Interface Constants */
bool loadTitleScreen;
bool moveTyrianLogoUp;
bool skipStarShowVGA;

/*EnemyData*/
JE_EnemyType enemy;
JE_EnemyAvailType enemyAvail;
JE_word enemyAvailOfs, topEnemyAvailOfs, groundEnemyAvailOfs, groundEnemyAvailOfs2, enemyOffset;
JE_word enemyOnScreen;
int enemyShapeTables[6]; /* [1..6] */
bool uniqueEnemy;
JE_word superEnemy254Jump;

/*EnemyShotData*/
bool fireButtonHeld;
bool enemyShotAvail[ENEMY_SHOT_MAX]; /* [1..Enemyshotmax] */
EnemyShotType enemyShot[ENEMY_SHOT_MAX]; /* [1..Enemyshotmax]  */

/* Player Shot Data */
int zinglonDuration;
int astralDuration;
JE_word flareDuration;
bool flareStart;
int flareColChg;
int specialWait;
int nextSpecialWait;
bool spraySpecial;
int doIced;
bool infiniteShot;

int superBomb[2]; /* [1..2] */

int tempShotX, tempShotY;
PlayerShotDataType playerShotData[MAX_PWEAPON + 1]; /* [1..MaxPWeapon+1] */

int chain;

/*PlayerData*/
bool allPlayersGone; /*Both players dead and finished exploding*/

int shotAvail[MAX_PWEAPON]; /* [1..MaxPWeapon] */   /*0:Avail 1-255:Duration left*/
int shadowyDist;
int purpleBallsRemaining[2]; /* [1..2] */

bool playerAlive, playerAliveB;
int playerStillExploding, playerStillExploding2;

Uint8 *eShapes1 = NULL, *eShapes2 = NULL, *eShapes3 = NULL,
      *eShapes4 = NULL, *eShapes5 = NULL, *eShapes6 = NULL;
Uint8 *shapesC1 = NULL, *shapes6  = NULL, *shapes9  = NULL, *shapesW2 = NULL;

unsigned long eShapes1Size, eShapes2Size, eShapes3Size, eShapes4Size, eShapes5Size, eShapes6Size, shapesC1Size, shapes6Size, shapes9Size, shapesW2Size;

int sAni;
int sAniX, sAniY, sAniXNeg, sAniYNeg;  /* X,Y ranges of field of hit */
int baseSpeedOld, baseSpeedOld2, baseSpeed, baseSpeedB, baseSpeed2, baseSpeed2B,
    baseSpeedKeyH, baseSpeedKeyV;
bool keyMoveWait;

bool makeMouseDelay;

JE_word playerInvulnerable1, playerInvulnerable2;

int lastPXShotMove, lastPYShotMove;

int PXB, PYB, lastPXB, lastPYB, lastPX2B, lastPY2B, PXChangeB, PYChangeB,
    lastTurnB, lastTurn2B, tempLastTurn2B;
int stopWaitXB, stopWaitYB;
JE_word mouseXB, mouseYB;

int PX, PY, lastPX, lastPY, lastPX2, lastPY2, PXChange, PYChange,
    lastTurn, lastTurn2, tempLastTurn2;
int stopWaitX, stopWaitY;

int PYHist[3], PYHistB[3]; /* [1..3] */

/*int optionMoveX[10], optionMoveY[10]; \* [1..10] *\ */
JE_word option1Draw, option2Draw, option1Item, option2Item;
int option1AmmoMax, option2AmmoMax;
JE_word option1AmmoRechargeWait, option2AmmoRechargeWait,
        option1AmmoRechargeWaitMax, option2AmmoRechargeWaitMax;
int option1Ammo, option2Ammo;
int optionAni1, optionAni2, optionCharge1, optionCharge2, optionCharge1Wait, optionCharge2Wait,
    option1X, option1LastX, option1Y, option1LastY,
    option2X, option2LastX, option2Y, option2LastY,
    option1MaxX, option1MinX, option2MaxX, option2MinX,
    option1MaxY, option1MinY, option2MaxY, option2MinY;
bool optionAni1Go, optionAni2Go, option1Stop, option2Stop;
float optionSatelliteRotate;

int optionAttachmentMove;
bool optionAttachmentLinked, optionAttachmentReturn;


int chargeWait, chargeLevel, chargeMax, chargeGr, chargeGrWait;

bool playerHNotReady;
JE_word playerHX[20], playerHY[20]; /* [1..20] */

JE_word neat;

/*MegaData*/
JE_word megaDataOfs, megaData2Ofs, megaData3Ofs;

/*Temporary Numbers*/
JE_word avail;
JE_word tempCount;
int tempI, tempI2, tempI3, tempI4, tempI5;
long tempL;
float tempR, tempR2;

bool tempB;
int temp, temp2, temp3, temp4, temp5, tempPos;
JE_word tempX, tempY, tempX2, tempY2;
JE_word tempW, tempW2, tempW3, tempW4, tempW5, tempOfs;


bool doNotSaveBackup;

bool tempSpecial;

int a, b, c, d, zz;
int playerNum;

unsigned char **BKwrap1to, **BKwrap2to, **BKwrap3to, **BKwrap1, **BKwrap2, **BKwrap3;

int min, max;

int specialWeaponFilter, specialWeaponFreq;
JE_word specialWeaponWpn;
bool linkToPlayer;

int baseArmor, baseArmor2;
JE_word shipGr, shipGr2;
Uint8 *shipGrPtr, *shipGr2ptr;

static int special_recharge_time;


void JE_getShipInfo( void )
{
	bool extraShip, extraShip2;

	shipGrPtr = shapes9;
	shipGr2ptr = shapes9;

	powerAdd = powerSys[pItems[PITEM_GENERATOR]].power;

	extraShip = pItems[PITEM_SHIP] > 90;
	if (extraShip)
	{
		shipGr = JE_SGr(pItems[PITEM_SHIP] - 90, &shipGrPtr);

		int base = (pItems[PITEM_SHIP] - 91) * 15;
		armorLevel = extraShips[base+7];
	} else {
		shipGr = ships[pItems[PITEM_SHIP]].shipgraphic;
		armorLevel = ships[pItems[PITEM_SHIP]].dmg;
	}

	extraShip2 = pItemsPlayer2[PITEM_SHIP] > 90;
	if (extraShip2)
	{
		shipGr2 = JE_SGr(pItemsPlayer2[PITEM_SHIP] - 90, &shipGr2ptr);

		int base2 = (pItemsPlayer2[PITEM_SHIP] - 91) * 15;
		baseArmor2 = extraShips[base2+7];
	} else {
		shipGr2 = 0;
		armorLevel2 = 10;
	}

	baseArmor = armorLevel;
	baseArmor2 = armorLevel2;

	if (extraShip)
	{
		sAni = 2;
	} else {
		sAni = ships[pItems[PITEM_SHIP]].ani;
	}

	if (sAni == 0)
	{
		sAniX = 12;
		sAniY = 10;
		sAniXNeg = -12;
		sAniYNeg = -10;
	} else {
		sAniX = 11;
		sAniY = 14;
		sAniXNeg = -11;
		sAniYNeg = -14;
	}
}

JE_word JE_SGr( JE_word ship, Uint8 **ptr )
{
	const JE_word GR[15] /* [1..15] */ = {233, 157, 195, 271, 81, 0, 119, 5, 43, 81, 119, 157, 195, 233, 271};

	JE_word tempW = 1;

	tempW = extraShips[(ship-1)*15];
	if (tempW > 7)
	{
		*ptr = extraShapes;
	}
	return GR[tempW-1];
}

void JE_calcPurpleBall( int playernum )
{
	static const int purpleBallMax[12] /* [0..11] */ = {1, 1, 2, 4, 8, 12, 16, 20, 25, 30, 40, 50};

	purpleBallsRemaining[playernum-1] = purpleBallMax[portPower[playernum-1]];
}

void JE_drawOptions( void )
{
	Uint8 *temp_surface = 0;

	if (tempScreenSeg == VGAScreenSeg)
	{
		temp_surface = VGAScreen;
		VGAScreen = VGAScreenSeg;
	}

	if(twoPlayerMode)
	{
		option1Draw = 108;
		option2Draw = 126;
		if (pItemsPlayer2[PITEM_LEFT_SIDEKICK] == 0)
		{
			option1Draw = 0;
		}
		if (pItemsPlayer2[PITEM_RIGHT_SIDEKICK] == 0)
		{
			option2Draw = 0;
		}
		option1Item = pItemsPlayer2[PITEM_LEFT_SIDEKICK];
		option2Item = pItemsPlayer2[PITEM_RIGHT_SIDEKICK];
		tempX = PXB;
		tempY = PYB;
	} else {
		option1Draw = 64;
		option2Draw = 82;
		option1Item = pItems[PITEM_LEFT_SIDEKICK];
		option2Item = pItems[PITEM_RIGHT_SIDEKICK];
		if (option1Item == 0)
		{
			option1Draw = 0;
		}
		if (option2Item == 0)
		{
			option2Draw = 0;
		}
		tempX = PX;
		tempY = PY;
	}
	option1X = tempX - 8;
	option1LastX = 0;
	option1Y = tempY + 2;
	option1LastY = 0;
	option2X = tempX + 8;
	option2LastX = 0;
	option2Y = tempY + 2;
	option2LastY = 0;

	option1Ammo = options[option1Item].ammo;
	option2Ammo = options[option2Item].ammo;

	optionAni1Go = options[option1Item].option == 1;
	optionAni2Go = options[option2Item].option == 1;
	option1Stop  = (options[option1Item].stop != 0);
	option1MaxX  = options[option1Item].opspd;
	option1MinX  = -option1MaxX;
	option1MaxY  = options[option1Item].opspd;
	option1MinY  = -option1MaxY;
	option2Stop  = (options[option2Item].stop != 0);
	option2MaxX  = options[option2Item].opspd;
	option2MinX  = -option2MaxX;
	option2MaxY  = options[option2Item].opspd;
	option2MinY  = -option2MaxY;

	if (option1Ammo > 0)
	{
		option1AmmoMax = option1Ammo / 10;
	} else {
		option1AmmoMax = 2;
	}
	if (option1AmmoMax == 0)
	{
		option1AmmoMax++;
	}
	option1AmmoRechargeWaitMax = (105 - option1Ammo) * 4;
	option1AmmoRechargeWait = option1AmmoRechargeWaitMax;

	if (option2Ammo > 0)
	{
		option2AmmoMax = option2Ammo / 10;
	} else {
		option2AmmoMax = 2;
	}
	if (option2AmmoMax == 0)
	{
		option2AmmoMax++;
	}
	option2AmmoRechargeWaitMax = (105 - option2Ammo) * 4;
	option2AmmoRechargeWait = option2AmmoRechargeWaitMax;

	if (option1Draw > 0)
	{
		JE_c_bar(284, option1Draw, 284 + 28, option1Draw + 15, 0);
	}
	if (option2Draw > 0)
	{
		JE_c_bar(284, option2Draw, 284 + 28, option2Draw + 15, 0);
	}

	if (options[option1Item].icongr > 0)
	{
		JE_newDrawCShapeNum(OPTION_SHAPES, options[option1Item].icongr, 284, option1Draw);
	}
	if (options[option2Item].icongr > 0)
	{
		JE_newDrawCShapeNum(OPTION_SHAPES, options[option2Item].icongr, 284, option2Draw);
	}

	if (option1Draw > 0)
	{
		JE_barDrawDirect(284, option1Draw + 13, option1AmmoMax, 112, option1Ammo, 2, 2);
	}
	if (option2Draw > 0)
	{
		JE_barDrawDirect(284, option2Draw + 13, option2AmmoMax, 112, option2Ammo, 2, 2);
	}

	if (option1Ammo == 0)
	{
		option1Ammo = -1;
	}
	if (option2Ammo == 0)
	{
		option2Ammo = -1;
	}

	optionAni1 = 1;
	optionAni2 = 1;
	optionCharge1Wait = 20;
	optionCharge2Wait = 20;
	optionCharge1 = 0;
	optionCharge2 = 0;

	if (temp_surface)
	{
		VGAScreen = temp_surface;
	}

	JE_drawOptionLevel();
}

void JE_drawOptionLevel( void )
{
	if (twoPlayerMode)
	{
		for (int temp = 1; temp <= 3; temp++)
		{
			JE_c_bar(268, 127 + (temp - 1) * 6, 269, 127 + 3 + (temp - 1) * 6, 193 + ((pItemsPlayer2[PITEM_P2_SIDEKICK] - 100) == temp) * 11);
		}
	}
}

void JE_tyrianHalt( int code )
{
	deinit_video();

	deinit_joystick();

	/* TODO: NETWORK */

	/* YKS: Here the original and classic free'd everythig in the memory.
	 * Since the OS does this for us I can save some typing. =] */

	deinit_sound();

	/* JE_endMusic(soundeffects); TODO */

	if (code != 9)
	{
		JE_saveConfiguration();
	}	

	if (code == 9)
	{
		/* OutputString('call=file0002.EXE' + #0'); TODO? */
	}

	if (code == 5)
	{
		code = 0;
	}

	if (trentWin)
	{
		Console::get() << std::endl << std::endl << std::endl
			<< "Sleep well, Trent, you deserve the rest." << std::endl
			<< "You now have permission to borrow my ship on your next mission." << std::endl
			<< std::endl
			<< "Also, you might want to try out the YESXMAS parameter." << std::endl
			<< "\tType: tyrian YESXMAS" << std::endl;
	}

	SDL_Quit();
	exit(code);
}

void JE_initPlayerShot( JE_word portNum, int temp, JE_word PX, JE_word PY, JE_word mouseX, JE_word mouseY, JE_word wpNum, int playerNum )
{
	const int soundChannel[11] /* [1..11] */ = {0, 2, 4, 4, 2, 2, 5, 5, 1, 4, 1};

	if (portNum <= PORT_NUM)
	{
		if (wpNum > 0 && wpNum <= WEAP_NUM)
		{
			if (power >= weaponPort[portNum].poweruse)
			{

				power -= weaponPort[portNum].poweruse;

				if (weapons[wpNum].sound > 0)
				{
					soundQueue[soundChannel[temp-1]] = weapons[wpNum].sound;
				}

				/*Rot*/
				for (int tempW = 1; tempW <= weapons[wpNum].multi; tempW++)
				{

					int b;

					for (b = 0; b < MAX_PWEAPON; b++)
					{
						if (shotAvail[b] == 0)
						{
							break;
						}
					}
					if (b == MAX_PWEAPON)
					{
						return;
					}

					if (shotMultiPos[temp-1] == weapons[wpNum].max || shotMultiPos[temp-1] > 8)
					{
						shotMultiPos[temp-1] = 1;
					} else {
						shotMultiPos[temp-1]++;
					}

					playerShotData[b].chainReaction = 0;

					playerShotData[b].playerNumber = playerNum;

					playerShotData[b].shotAni = 0;

					playerShotData[b].shotComplicated = weapons[wpNum].circlesize != 0;

					if (weapons[wpNum].circlesize == 0)
					{
						playerShotData[b].shotDevX = 0;
						playerShotData[b].shotDirX = 0;
						playerShotData[b].shotDevY = 0;
						playerShotData[b].shotDirY = 0;
						playerShotData[b].shotCirSizeX = 0;
						playerShotData[b].shotCirSizeY = 0;
					} else {
						temp2 = weapons[wpNum].circlesize;

						if (temp2 > 19)
						{
							temp3 = temp2 % 20;
							playerShotData[b].shotCirSizeX = temp3;
							playerShotData[b].shotDevX = temp3 >> 1;

							temp2 = temp2 / 20;
							playerShotData[b].shotCirSizeY = temp2;
							playerShotData[b].shotDevY = temp2 >> 1;
						} else {
							playerShotData[b].shotCirSizeX = temp2;
							playerShotData[b].shotCirSizeY = temp2;
							playerShotData[b].shotDevX = temp2 >> 1;
							playerShotData[b].shotDevY = temp2 >> 1;
						}
						playerShotData[b].shotDirX = 1;
						playerShotData[b].shotDirY = -1;
					}

					playerShotData[b].shotTrail = weapons[wpNum].trail;

					if (weapons[wpNum].attack[shotMultiPos[temp-1]-1] > 99 && weapons[wpNum].attack[shotMultiPos[temp-1]-1] < 250)
					{
						playerShotData[b].chainReaction = weapons[wpNum].attack[shotMultiPos[temp-1]-1] - 100;
						playerShotData[b].shotDmg = 1;
					} else {
						playerShotData[b].shotDmg = weapons[wpNum].attack[shotMultiPos[temp-1]-1];
					}

					playerShotData[b].shotBlastFilter = weapons[wpNum].shipblastfilter;

					tempI = weapons[wpNum].by[shotMultiPos[temp-1]-1];

					/*Note: Only front selection used for player shots...*/

					playerShotData[b].shotX = PX + weapons[wpNum].bx[shotMultiPos[temp-1]-1];

					playerShotData[b].shotY = PY + tempI;
					playerShotData[b].shotYC = -weapons[wpNum].acceleration;
					playerShotData[b].shotXC = weapons[wpNum].accelerationx;

					playerShotData[b].shotXM = weapons[wpNum].sx[shotMultiPos[temp-1]-1];

					temp2 = weapons[wpNum].del[shotMultiPos[temp-1]-1];

					if (temp2 == 121)
					{
						playerShotData[b].shotTrail = 0;
						temp2 = 255;
					}

					playerShotData[b].shotGr = weapons[wpNum].sg[shotMultiPos[temp-1]-1];
					if (playerShotData[b].shotGr == 0)
					{
						shotAvail[b] = 0;
					} else {
						shotAvail[b] = temp2;
					}
					if (temp2 > 100 && temp2 < 120)
					{
						playerShotData[b].shotAniMax = (temp2 - 100 + 1);
					} else {
						playerShotData[b].shotAniMax = weapons[wpNum].weapani + 1;
					}

					if (temp2 == 99 || temp2 == 98)
					{
						tempI = PX - mouseX;
						if (tempI < -5)
						{
							tempI = -5;
						}
						if (tempI > 5)
						{
							tempI = 5;
						}
						playerShotData[b].shotXM += tempI;
					}


					if (temp2 == 99 || temp2 == 100)
					{
						tempI = PY - mouseY - weapons[wpNum].sy[shotMultiPos[temp-1]-1];
						if (tempI < -4)
						{
							tempI = -4;
						}
						if (tempI > 4)
						{
							tempI = 4;
						}
						playerShotData[b].shotYM = tempI;
					} else {
						if (weapons[wpNum].sy[shotMultiPos[temp-1]-1] == 98)
						{
							playerShotData[b].shotYM = 0;
							playerShotData[b].shotYC = -1;
						} else {
							if (weapons[wpNum].sy[shotMultiPos[temp-1]-1] > 100)
							{
								playerShotData[b].shotYM = weapons[wpNum].sy[shotMultiPos[temp-1]-1];
								playerShotData[b].shotY -= PYChange;
							} else {
								playerShotData[b].shotYM = -weapons[wpNum].sy[shotMultiPos[temp-1]-1];
							}
						}
					}

					if (weapons[wpNum].sx[shotMultiPos[temp-1]-1] > 100)
					{
						playerShotData[b].shotXM = weapons[wpNum].sx[shotMultiPos[temp-1]-1];
						playerShotData[b].shotX -= PXChange;
						if (playerShotData[b].shotXM == 101)
						{
							playerShotData[b].shotY -= PYChange;
						}
					}


					if (weapons[wpNum].aim > 5)
					{  /*Guided Shot*/

						tempW3 = 65000;
						temp3 = 0;
						/*Find Closest Enemy*/
						for (JE_word x = 0; x < 100; x++)
						{
							if (enemyAvail[x] != 1 && !enemy[x].scoreitem)
							{
								JE_word y = abs(enemy[x].ex - playerShotData[b].shotX) + abs(enemy[x].ey - playerShotData[b].shotY);
								if (y < tempW3)
								{
									tempW3 = y;
									temp3 = x + 1;
								}
							}
						}
						playerShotData[b].aimAtEnemy = temp3;
						playerShotData[b].aimDelay = 5;
						playerShotData[b].aimDelayMax = weapons[wpNum].aim - 5;
					} else {
						playerShotData[b].aimAtEnemy = 0;
					}

					shotRepeat[temp-1] = weapons[wpNum].shotrepeat;
				}
			}
		}
	}
}

void JE_specialComplete( int playerNum, int *armor, int *shield, int specialType )
{
	nextSpecialWait = 0;
	switch (special[specialType].stype)
	{
		/*Weapon*/
		case 1:
			if (playerNum == 1)
			{
				JE_initPlayerShot(0, 11, PX, PY, mouseX, mouseY, special[specialType].wpn, playerNum);
			} else {
				JE_initPlayerShot(0, 11, PXB, PYB, mouseX, mouseY, special[specialType].wpn, playerNum);
			}
			shotRepeat[8] = shotRepeat[10];
			break;
		/*Repulsor*/
		case 2:
			for (int temp = 0; temp < ENEMY_SHOT_MAX; temp++)
				if (!enemyShotAvail[temp])
				{
					if (PX > enemyShot[temp].sx)
					{
						enemyShot[temp].sxm--;
					} else if (PX < enemyShot[temp].sx) {
						enemyShot[temp].sxm++;
					}
					if (PY > enemyShot[temp].sy)
					{
						enemyShot[temp].sym--;
					} else if (PY < enemyShot[temp].sy) {
						enemyShot[temp].sym++;
					}
				}
			break;
		/*Zinglon Blast*/
		case 3:
			zinglonDuration = 50;
			shotRepeat[8] = 100;
			soundQueue[7] = 21;
			break;
		/*Attractor*/
		case 4:
			for (int temp = 0; temp < 100; temp++)
			{
				if (enemyAvail[temp] != 1 && enemy[temp].scoreitem
				    && enemy[temp].evalue != 0)
				{
					if (PX > enemy[temp].ex)
					{
						enemy[temp].exc++;
					} else if (PX < enemy[temp].ex) {
						enemy[temp].exc--;
					}
					if (PY > enemy[temp].ey) {
						enemy[temp].eyc++;
					} else if (PY < enemy[temp].ey) {
						enemy[temp].eyc--;
					}
				}
			}
			break;
		/*Flare*/
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 16:
			if (flareDuration == 0)
			{
				flareStart = true;
			}
			specialWeaponWpn = special[specialType].wpn;
			linkToPlayer = false;
			spraySpecial = false;
			switch (special[specialType].stype)
			{
				case 5:
					specialWeaponFilter = 7;
					specialWeaponFreq = 2;
					flareDuration = 50;
					break;
				case 6:
					specialWeaponFilter = 1;
					specialWeaponFreq = 7;
					flareDuration = 200 + portPower[1-1] * 25;
					break;
				case 7:
					specialWeaponFilter = 3;
					specialWeaponFreq = 3;
					flareDuration = 50 + portPower[1-1] * 10;
					zinglonDuration = 50;
					shotRepeat[8] = 100;
					soundQueue[7] = 21;
					break;
				case 8:
					specialWeaponFilter = -99;
					specialWeaponFreq = 7;
					flareDuration = 10 + portPower[1-1];
					break;
				case 9:
					specialWeaponFilter = -99;
					specialWeaponFreq = 8;
					flareDuration = portPower[1-1] * 2 + 8;
					linkToPlayer = true;
					nextSpecialWait = special[specialType].pwr;
					break;
				case 10:
					specialWeaponFilter = -99;
					specialWeaponFreq = 8;
					flareDuration = 14 + portPower[1-1] * 4;
					linkToPlayer = true;
					break;
				case 11:
					specialWeaponFilter = -99;
					specialWeaponFreq = special[specialType].pwr;
					flareDuration = 10 + portPower[1-1] * 10;
					astralDuration = 20 + portPower[1-1] * 10;
					break;
				case 16:
					specialWeaponFilter = -99;
					specialWeaponFreq = 8;
					flareDuration = temp2 * 16 + 8;
					linkToPlayer = true;
					spraySpecial = true;
					break;
			}
			break;
		case 12:
			switch (playerNum)
			{
				case 1:
					playerInvulnerable1 = temp2 * 10;
					break;
				case 2:
					playerInvulnerable2 = temp2 * 10;
					break;
			}
			if (superArcadeMode > 0 && superArcadeMode <= SA)
			{
				shotRepeat[8] = 250;
				JE_initPlayerShot(0, 11, PX, PY, mouseX, mouseY, 707, 1);
				playerInvulnerable1 = 100;
			}
			break;
		case 13:
			if (playerNum == 1)
			{
				*armor += temp2 / 4 + 1;
			} else {
				armorLevel += temp2 / 4 + 1;
			}
			soundQueue[3] = 29;
			break;
		case 14:
			if (playerNum == 2)
			{
				*armor += temp2 / 4 + 1;
			} else {
				armorLevel2 += temp2 / 4 + 1;
			}
			soundQueue[3] = 29;
			break;
		case 17:
			soundQueue[3] = 29;
			if (pItems[PITEM_LEFT_SIDEKICK] == special[specialType].wpn)
			{
				pItems[PITEM_RIGHT_SIDEKICK] = special[specialType].wpn;
				shotMultiPos[4] = 0;
			} else {
				pItems[PITEM_LEFT_SIDEKICK] = special[specialType].wpn;
				shotMultiPos[3] = 0;
			}
			
			tempScreenSeg = VGAScreenSeg;
			JE_drawOptions();
			break;
		case 18:
			pItems[PITEM_RIGHT_SIDEKICK] = special[specialType].wpn;
			
			tempScreenSeg = VGAScreenSeg;
			JE_drawOptions();
			soundQueue[4] = 29;
			shotMultiPos[3] = 0;
			break;
	}

	special_recharge_time = shotRepeat[8];
	if (special_recharge_time == 0) special_recharge_time = 1;
}

void JE_doSpecialShot( int playerNum, int *armor, int *shield )
{
	if (shotRepeat[8] > 0)
	{
		shotRepeat[8]--;
	}
	if (specialWait > 0)
	{
		specialWait--;
	}
	temp = SFExecuted[playerNum-1];
	if (temp > 0 && shotRepeat[8] == 0 && flareDuration == 0)
	{
		temp2 = special[temp].pwr;
		
		tempB = true;
		if (temp2 > 0)
		{
			if (temp2 < 98)
			{
				if (*shield >= temp2)
				{
					*shield -= temp2;
				} else {
					tempB = false;
				}
			} else if (temp2 == 98) {
				if (*shield < 4)
				{
					tempB = false;
				}
				temp2 = *shield;
				*shield = 0;
			} else if (temp2 == 99) {
				temp2 = *shield / 2;
				*shield = temp2;
			} else {
				temp2 -= 100;
				if (*armor > temp2)
				{
					*armor -= temp2;
				} else {
					tempB = false;
				}
			}
		}
		
		shotMultiPos[8] = 0;
		shotMultiPos[10] = 0;
		if (tempB)
		{
			JE_specialComplete(playerNum, armor, shield, temp);
		}
		SFExecuted[playerNum-1] = 0;
		
		JE_wipeShieldArmorBars();
		VGAScreen = VGAScreenSeg; /* side-effect of game_screen */
		JE_drawShield();
		JE_drawArmor();
		VGAScreen = game_screen; /* side-effect of game_screen */
	}
	
	if (playerNum == 1 && pItems[PITEM_SPECIAL] > 0)
	{  /*Main Begin*/
		
		if (superArcadeMode > 0 && (button[1] || button[2]))
		{
			fireButtonHeld = false;
		}
		if (!button[0] && !(superArcadeMode > 0 && (button[1] || button[2])))
		{
			fireButtonHeld = false;
		} else if (shotRepeat[8] == 0 && !fireButtonHeld && !(flareDuration > 0) && specialWait == 0) {
			fireButtonHeld = true;
			JE_specialComplete(playerNum, armor, shield, pItems[PITEM_SPECIAL]);
		}
		
	}  /*Main End*/
	
	if (astralDuration > 0)
	{
		astralDuration--;
	}
	
	shotAvail[MAX_PWEAPON-1] = 0;
	if (flareDuration > 1)
	{
		if (specialWeaponFilter != -99)
		{
			if (levelFilter == -99 && levelBrightness == -99)
			{
				filterActive = false;
			}
			if (!filterActive)
			{
				levelFilter = specialWeaponFilter;
				if (levelFilter == 7)
				{
					levelBrightness = 0;
				}
				filterActive = true;
			}
			
			if (mt::rand() % 2 == 0)
				flareColChg = -1;
			else
				flareColChg = 1;
			
			if (levelFilter == 7)
			{
				if (levelBrightness < -6)
				{
					flareColChg = 1;
				}
				if (levelBrightness > 6)
				{
					flareColChg = -1;
				}
				levelBrightness += flareColChg;
			}
		}
		
		if ((signed)(mt::rand() % 6) < specialWeaponFreq)
		{
			b = 0;
			
			if (linkToPlayer)
			{
				if (shotRepeat[8] == 0)
				{
					JE_initPlayerShot(0, 9, PX, PY, mouseX, mouseY, specialWeaponWpn, playerNum);
				}
			} else {
				JE_initPlayerShot(0, 9, mt::rand() % 280, rand() % 180, mouseX, mouseY, specialWeaponWpn, playerNum);
			}
			
			if (spraySpecial && b > 0)
			{
				playerShotData[b].shotXM = (mt::rand() % 5) - 2;
				playerShotData[b].shotYM = (mt::rand() % 5) - 2;
				if (playerShotData[b].shotYM == 0)
				{
					playerShotData[b].shotYM++;
				}
			}
		}
		
		flareDuration--;
		if (flareDuration == 1)
		{
			specialWait = nextSpecialWait;
		}
	} else if (flareStart) {
		flareStart = false;
		if (linkToPlayer)
		{
			shotRepeat[8] = 15;
			special_recharge_time = 15;
		} else {
			shotRepeat[8] = 200;
			special_recharge_time = 200;
		}
		flareDuration = 0;
		if (levelFilter == specialWeaponFilter)
		{
			levelFilter = -99;
			levelBrightness = -99;
			filterActive = false;
		}
	}
		
	if (zinglonDuration > 1)
	{
		temp = 25 - abs(zinglonDuration - 25);
		
		JE_barBright(PX + 7 - temp,     0, PX + 7 + temp,     184);
		JE_barBright(PX + 7 - temp - 2, 0, PX + 7 + temp + 2, 184);
		
		zinglonDuration--;
		if (zinglonDuration % 5 == 0)
		{
			shotAvail[MAX_PWEAPON-1] = 1;
		}
	}
}

void draw_special_shot_info()
{
	if (pItems[PITEM_SPECIAL] > 0)
	{
		if (special_recharge_time == 0)
			special_recharge_time = 1;

		int special_progress;
		bool done = false;
		JE_drawShape2(47, 4, 93, shapes9);
		if (shotRepeat[8] == 0 && specialWait == 0 && flareDuration < 1 && zinglonDuration < 2)
		{
			// Special is done
			special_progress = 10;
			done = true;
		} else {
			if (flareDuration > 0)
			{
				special_progress = 0;
			} else {
				special_progress = 10-shotRepeat[8]*10/special_recharge_time;
			}
		}

		// Draw progress bar
		if (special_progress > 0)
		{
			JE_bar(51,6+10-special_progress,51+3,6+9, done ? 0x77: 0x74);
		}
	}
}

void JE_wipeShieldArmorBars( void )
{
	if (!twoPlayerMode || galagaMode)
	{
		JE_c_bar(270, 137, 278, 194 - shield * 2, 0);
	} else {
		JE_c_bar(270, 60 - 44, 278, 60, 0);
		JE_c_bar(270, 194 - 44, 278, 194, 0);
	}
	if (!twoPlayerMode || galagaMode)
	{
		JE_c_bar(307, 137, 315, 194 - armorLevel * 2, 0);
	} else {
		JE_c_bar(307, 60 - 44, 315, 60, 0);
		JE_c_bar(307, 194 - 44, 315, 194, 0);
	}
}

int JE_playerDamage( JE_word tempX, JE_word tempY,
                         int temp,
                         int *PX, int *PY,
                         bool *playerAlive,
                         int *playerStillExploding,
                         int *armorLevel,
                         int *shield,
						 unsigned int player )
{
	int playerDamage = 0;
	soundQueue[7] = 27;
	
	/* Player Damage Routines */
	if (*shield < temp)
	{
		playerDamage = temp;
		temp -= *shield;
		*shield = 0;
		
		if (temp > 0)
		{
			
			/*Through Shields - Now Armor */
			if (*armorLevel < temp)
			{
				temp -= *armorLevel;
				*armorLevel = 0;
				if (playerAlive && !youAreCheating)
				{
					levelTimer = false;
					gameQuitDelay = 10;
					*playerAlive = false;
					*playerStillExploding = 60;
					levelEnd = 40;
					tempVolume = CVars::snd_music_vol;
					soundQueue[1] = 22;
				}
				
				/*Through Armor - Now What? */
				
			} else {
				*armorLevel -= temp;
				soundQueue[7] = 19;
			}
		}
		
	} else {
		*shield -= temp;
		
		JE_setupExplosion(*PX-17, *PY-12, 14, 0, false, player);
		JE_setupExplosion(*PX-5 , *PY-12, 15, 0, false, player);
		JE_setupExplosion(*PX+7 , *PY-12, 16, 0, false, player);
		JE_setupExplosion(*PX+19, *PY-12, 17, 0, false, player);
		
		JE_setupExplosion(*PX-17, *PY+2,  18, 0, false, player);
		JE_setupExplosion(*PX+19, *PY+2,  19, 0, false, player);
		
		JE_setupExplosion(*PX-17, *PY+16, 20, 0, false, player);
		JE_setupExplosion(*PX-5 , *PY+16, 21, 0, false, player);
		JE_setupExplosion(*PX+7 , *PY+16, 22, 0, false, player);
	}
	
	JE_wipeShieldArmorBars();
	VGAScreen = VGAScreenSeg; /* side-effect of game_screen */
	JE_drawShield();
	JE_drawArmor();
	VGAScreen = game_screen; /* side-effect of game_screen */
	
	return playerDamage;
}

void JE_powerUp( int port )
{
	shotMultiPos[port-1] = 0;
	if (portPower[port-1] < 11 && (pItems[port-1] > 0 || twoPlayerMode))
		portPower[port-1]++;
	else
		score += 1000;
}

void JE_portConfigs( void )
{
	if (twoPlayerMode)
	{
		tempW = weaponPort[pItemsPlayer2[PITEM_REAR_WEAPON]].opnum;
	} else {
		tempW = weaponPort[pItems[PITEM_REAR_WEAPON]].opnum;
	}
}

void JE_drawShield( void )
{
	if (twoPlayerMode && !galagaMode)
	{
		JE_dBar3(270, 60, ot_round(shield * 0.8f), 144);
		JE_dBar3(270, 194, ot_round(shield2 * 0.8f), 144);
	} else {
		JE_dBar3(270, 194, shield, 144);
		if (shield != shieldMax)
		{
			JE_rectangle(270, 193 - (shieldMax << 1), 278, 193 - (shieldMax << 1), 68); /* <MXD> SEGa000 */
		}
	}
}

void JE_drawArmor( void )
{
	if (armorLevel > 28)
	{
		armorLevel = 28;
	}
	if (armorLevel2 > 28)
	{
		armorLevel2 = 28;
	}

	if (twoPlayerMode && !galagaMode)
	{
		JE_dBar3(307, 60, ot_round(armorLevel * 0.8f), 224);
		JE_dBar3(307, 194, ot_round(armorLevel2 * 0.8f), 224);
	} else {
		JE_dBar3(307, 194, armorLevel, 224);
	}
}

void JE_resetPlayerH( void )
{
	for (int temp = 0; temp < 20; temp++)
	{
		if (twoPlayerMode)
		{
			playerHX[temp] = PXB - (20 - temp);
			playerHY[temp] = PYB - 18;
		} else {
			playerHX[temp] = PX - (20 - temp);
			playerHY[temp] = PY - 18;
		}
	}
	playerHNotReady = false;
}
