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
#ifndef VARZ_H
#define VARZ_H

#include "opentyr.h"

#include "episodes.h"


#define SA 7

#define MAX_PWEAPON     81 /* 81*/
#define EXPLOSION_MAX  200 /*200*/
#define ENEMY_SHOT_MAX  60 /* 60*/

#define CURRENT_KEY_SPEED 1  /*Keyboard/Joystick movement rate*/

#define MAX_SP 100

struct JE_SingleEnemyType
{
	int ex, ey;     /* POSITION */
	int exc, eyc;   /* CURRENT SPEED */
	int exca, eyca; /* RANDOM ACCELERATION */
	int excc, eycc; /* FIXED ACCELERATION WAITTIME */
	int exccw, eyccw;
	int armorleft;
	int eshotwait[3], eshotmultipos[3]; /* [1..3] */
	int enemycycle;
	int ani;
	JE_word egr[20]; /* [1..20] */
	int size;
	int linknum;
	int aniactive;
	int animax;
	int aniwhenfire;
	void *shapeseg;
	int exrev, eyrev;
	int exccadd, eyccadd;
	int exccwmax, eyccwmax;
	void *enemydatofs;
	bool edamaged;
	JE_word enemytype;
	int animin;
	JE_word edgr;
	int edlevel;
	int edani;
	int filter;
	int evalue;
	int fixedmovey;
	int freq[3]; /* [1..3] */
	int launchwait;
	JE_word launchtype;
	int launchfreq;
	int xaccel;
	int yaccel;
	int tur[3]; /* [1..3] */
	JE_word enemydie; /* Enemy created when this one dies */
	bool enemyground;
	int explonum;
	JE_word mapoffset;
	bool scoreitem;

	bool special;
	int flagnum;
	bool setto;

	int iced; /*Duration*/

	int launchspecial;

	int xminbounce;
	int xmaxbounce;
	int yminbounce;
	int ymaxbounce;
};

typedef JE_SingleEnemyType JE_MultiEnemyType[100]; /* [1..100] */

typedef JE_word JE_DanCShape[(24 * 28) / 2]; /* [1..(24*28) div 2] */

typedef char JE_CharString[256]; /* [1..256] */

typedef char JE_Map1Buffer[24 * 28 * 13 * 4]; /* [1..24*28*13*4] */

typedef unsigned char *JE_MapType[300][14]; /* [1..300, 1..14] */
typedef unsigned char *JE_MapType2[600][14]; /* [1..600, 1..14] */
typedef unsigned char *JE_MapType3[600][15]; /* [1..600, 1..15] */

struct JE_EventRecType
{
	JE_word eventtime;
	int eventtype;
	int eventdat, eventdat2;
	int eventdat3, eventdat5, eventdat6;
	int eventdat4;
};

struct JE_MegaDataType1
{
	JE_MapType mainmap;
	struct
	{
		JE_DanCShape sh;
	} shapes[72]; /* [0..71] */
};

struct JE_MegaDataType2
{
	JE_MapType2 mainmap;
	struct
	{
		JE_DanCShape sh;
	} shapes[71]; /* [0..70] */
};

struct JE_MegaDataType3
{
	JE_MapType3 mainmap;
	struct
	{
		JE_DanCShape sh;
	} shapes[70]; /* [0..69] */
};

typedef JE_MultiEnemyType JE_EnemyType;
typedef int JE_EnemyAvailType[100]; /* [1..100] */

typedef int JE_REXtype[20]; /* [1..20] */

struct ExplosionsType
{
	Sint32 explodeLoc;
	JE_word explodeGr;
	int followPlayer;
	int fixedExplode;
	Sint16 fixedMovement;
};

struct EnemyShotType
{
	int sx, sy;
	int sxm, sym;
	int sxc, syc;
	int tx, ty;
	JE_word sgr;
	int sdmg;
	int duration;
	JE_word animate;
	JE_word animax;
};

struct PlayerShotDataType
{
	int shotX, shotY, shotXM, shotYM, shotXC, shotYC;
	bool shotComplicated;
	int shotDevX, shotDirX, shotDevY, shotDirY, shotCirSizeX, shotCirSizeY;
	int shotTrail;
	JE_word shotGr, shotAni, shotAniMax;
	Uint8 shotDmg;
	int shotBlastFilter, chainReaction, playerNumber, aimAtEnemy, aimDelay, aimDelayMax;
};

struct REXDatType
{
	int delay;
	JE_word ex, ey;
	bool big;
};

struct SPLType
{
	JE_word location;
	JE_word movement;
	Uint8 color;
};

extern int fromTime;
extern int tempDat, tempDat2, tempDat3;
extern bool tempb2;
extern JE_word test1;
extern const int SANextShip[SA + 2];
extern const JE_word SASpecialWeapon[SA];
extern const JE_word SASpecialWeaponB[SA];
extern const int SAShip[SA];
extern const JE_word SAWeapon[SA][5];
extern const int specialArcadeWeapon[PORT_NUM];
extern const int optionSelect[16][3][2];
extern const JE_word PGR[21];
extern const int PAni[21];
extern const int pItemButtonMap[7];
extern const JE_word linkGunWeapons[38];
extern const JE_word chargeGunWeapons[38];
extern const JE_word linkMultiGr[17];
extern const JE_word linkSonicGr[17];
extern const JE_word linkMult2Gr[17];
extern const int randomEnemyLaunchSounds[3];
extern const int keyboardCombos[26][8];
extern const int shipCombosB[21];
extern const int superTyrianSpecials[4];
extern const int shipCombos[13][3];
extern int SFCurrentCode[2][21];
extern int SFExecuted[2];
extern bool stoppedDemo;
extern int lvlFileNum;
extern JE_word maxEvent, eventLoc;
extern JE_word tempBackMove, explodeMove;
extern int levelEnd;
extern JE_word levelEndFxWait;
extern int levelEndWarp;
extern bool showMemLeft, endLevel, reallyEndLevel, waitToEndLevel, playerEndLevel, normalBonusLevelCurrent, bonusLevelCurrent, smallEnemyAdjust, readyToEndLevel, quitRequested;
extern int newPL[10];
extern JE_word returnLoc;
extern bool returnActive;
extern JE_word galagaShotFreq;
extern unsigned long galagaLife;
extern bool debug;
extern unsigned long debugTime, lastDebugTime;
extern unsigned long debugHistCount;
extern float debugHist;
extern JE_word curLoc;
extern bool firstGameOver, gameLoaded, enemyStillExploding;
extern JE_word tempSS;
extern JE_word totalEnemy;
extern JE_word enemyKilled;
extern int statBar[2], statCol[2];
extern JE_Map1Buffer *map1BufferTop, *map1BufferBot;
extern JE_MegaDataType1 *megaData1;
extern JE_MegaDataType2 *megaData2;
extern JE_MegaDataType3 *megaData3;
extern int flash;
extern int flashChange;
extern int displayTime;
extern bool firstEvent;
extern FILE *recordFile;
extern JE_word recordFileNum;
extern int lastKey[8];
extern JE_word lastMoveWait, nextMoveWait;
extern int nextDemoOperation;
extern bool playDemo;
extern int playDemoNum;
extern int soundQueue[8];
extern bool enemyContinualDamage;
extern bool enemiesActive;
extern bool forceEvents;
extern bool stopBackgrounds;
extern int stopBackgroundNum;
extern int damageRate;
extern bool background3x1;
extern bool background3x1b;
extern bool levelTimer;
extern JE_word levelTimerCountdown;
extern JE_word levelTimerJumpTo;
extern bool randomExplosions;
extern bool editShip1, editShip2;
extern bool globalFlags[10];
extern int levelSong;
extern bool drawGameSaved;
extern bool repause;
extern bool loadDestruct;
extern JE_word mapOrigin, mapPNum;
extern int mapPlanet[5], mapSection[5];
extern bool loadTitleScreen;
extern bool moveTyrianLogoUp;
extern bool skipStarShowVGA;
extern JE_EnemyType enemy;
extern JE_EnemyAvailType enemyAvail;
extern JE_word enemyAvailOfs, topEnemyAvailOfs, groundEnemyAvailOfs, groundEnemyAvailOfs2, enemyOffset;
extern JE_word enemyOnScreen;
extern int enemyShapeTables[6];
extern bool uniqueEnemy;
extern JE_word superEnemy254Jump;
extern ExplosionsType explosions[EXPLOSION_MAX];
extern int explodeAvail[EXPLOSION_MAX];
extern int explosionFollowAmount;
extern bool playerFollow, fixedExplosions;
extern int explosionMoveUp;
extern bool fireButtonHeld;
extern bool enemyShotAvail[ENEMY_SHOT_MAX];
extern EnemyShotType enemyShot[ENEMY_SHOT_MAX];
extern int zinglonDuration;
extern int astralDuration;
extern JE_word flareDuration;
extern bool flareStart;
extern int flareColChg;
extern int specialWait;
extern int nextSpecialWait;
extern bool spraySpecial;
extern int doIced;
extern bool infiniteShot;
extern int superBomb[2];
extern int tempShotX, tempShotY;
extern PlayerShotDataType playerShotData[MAX_PWEAPON + 1];
extern int chain;
extern bool allPlayersGone;
extern int shotAvail[MAX_PWEAPON];
extern int shadowyDist;
extern int purpleBallsRemaining[2];
extern bool playerAlive, playerAliveB;
extern int playerStillExploding, playerStillExploding2;
extern Uint8 *eShapes1, *eShapes2, *eShapes3, *eShapes4, *eShapes5, *eShapes6;
extern Uint8 *shapesC1, *shapes6, *shapes9, *shapesW2;
extern unsigned long eShapes1Size, eShapes2Size, eShapes3Size, eShapes4Size, eShapes5Size, eShapes6Size, shapesC1Size, shapes6Size, shapes9Size, shapesW2Size;
extern int sAni;
extern int sAniX, sAniY, sAniXNeg, sAniYNeg;
extern int baseSpeedOld, baseSpeedOld2, baseSpeed, baseSpeedB, baseSpeed2, baseSpeed2B, baseSpeedKeyH, baseSpeedKeyV;
extern bool keyMoveWait;
extern bool makeMouseDelay;
extern JE_word playerInvulnerable1, playerInvulnerable2;
extern int lastPXShotMove, lastPYShotMove;
extern int PXB, PYB, lastPXB, lastPYB, lastPX2B, lastPY2B, PXChangeB, PYChangeB, lastTurnB, lastTurn2B, tempLastTurn2B;
extern int stopWaitXB, stopWaitYB;
extern JE_word mouseXB, mouseYB;
extern int PX, PY, lastPX, lastPY, lastPX2, lastPY2, PXChange, PYChange, lastTurn, lastTurn2, tempLastTurn2;
extern int stopWaitX, stopWaitY;
extern int PYHist[3], PYHistB[3];
extern JE_word option1Draw, option2Draw, option1Item, option2Item;
extern int option1AmmoMax, option2AmmoMax;
extern JE_word option1AmmoRechargeWait, option2AmmoRechargeWait, option1AmmoRechargeWaitMax, option2AmmoRechargeWaitMax;
extern int option1Ammo, option2Ammo;
extern int optionAni1, optionAni2, optionCharge1, optionCharge2, optionCharge1Wait, optionCharge2Wait, option1X, option1LastX, option1Y, option1LastY, option2X, option2LastX, option2Y, option2LastY, option1MaxX, option1MinX, option2MaxX, option2MinX, option1MaxY, option1MinY, option2MaxY, option2MinY;
extern bool optionAni1Go, optionAni2Go, option1Stop, option2Stop;
extern float optionSatelliteRotate;
extern int optionAttachmentMove;
extern bool optionAttachmentLinked, optionAttachmentReturn;
extern int chargeWait, chargeLevel, chargeMax, chargeGr, chargeGrWait;
extern bool playerHNotReady;
extern JE_word playerHX[20], playerHY[20];
extern JE_word neat;
extern JE_REXtype REXavail;
extern REXDatType REXdat[20];
extern int SPZ[MAX_SP + 1];
extern SPLType SPL[MAX_SP + 1];
extern JE_word lastSP;
extern JE_word megaDataOfs, megaData2Ofs, megaData3Ofs;
extern JE_word avail;
extern JE_word tempCount;
extern int tempI, tempI2, tempI3, tempI4, tempI5;
extern long tempL;
extern float tempR, tempR2;
extern bool tempB;
extern int temp, temp2, temp3, temp4, temp5, tempREX, tempPos;
extern JE_word tempX, tempY, tempX2, tempY2;
extern JE_word tempW, tempW2, tempW3, tempW4, tempW5, tempOfs;
extern bool doNotSaveBackup;
extern bool tempSpecial;
extern int a, b, c, d, zz;
extern int playerNum;
extern unsigned char **BKwrap1to, **BKwrap2to, **BKwrap3to, **BKwrap1, **BKwrap2, **BKwrap3;
extern int min, max;
extern int specialWeaponFilter, specialWeaponFreq;
extern JE_word specialWeaponWpn;
extern bool linkToPlayer;
extern int baseArmor, baseArmor2;
extern JE_word shipGr, shipGr2;
extern Uint8 *shipGrPtr, *shipGr2ptr;

void JE_getShipInfo( void );
JE_word JE_SGr( JE_word ship, Uint8 **ptr );

void JE_calcPurpleBall( int playernum );
void JE_drawOptions( void );

void JE_tyrianHalt( int code ); /* This ends the game */
void JE_initPlayerShot( JE_word portnum, int temp, JE_word px, JE_word py,
                        JE_word mousex, JE_word mousey,
                        JE_word wpnum, int playernum );
void JE_specialComplete( int playernum, int *armor, int *shield, int specialType );
void JE_doSpecialShot( int playernum, int *armor, int *shield );

void JE_powerUp( int port );
void JE_wipeShieldArmorBars( void );
int JE_playerDamage( JE_word tempX, JE_word tempY, int temp,
                         int *PX, int *PY,
                         bool *playerAlive,
                         int *playerStillExploding,
                         int *armorLevel,
                         int *shield );

void JE_setupExplosion( int x, int y, int explodetype );
void JE_setupExplosionLarge( bool enemyground, int explonum, int x, int y );

void JE_drawShield( void );
void JE_drawArmor( void );

void JE_portConfigs( void );

void JE_resetPlayerH( void );

void JE_doSP( JE_word x, JE_word y, JE_word num, int explowidth, Uint8 color ); /*SuperPixels*/
void JE_drawSP( void );

void JE_drawOptionLevel( void );


#endif /* VARZ_H */
