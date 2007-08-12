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
	JE_byte     fillbyte;
	JE_integer  ex, ey;     /* POSITION */
	JE_shortint exc, eyc;   /* CURRENT SPEED */
	JE_shortint exca, eyca; /* RANDOM ACCELERATION */
	JE_shortint excc, eycc; /* FIXED ACCELERATION WAITTIME */
	JE_shortint exccw, eyccw;
	JE_byte     armorleft;
	JE_byte     eshotwait[3], eshotmultipos[3]; /* [1..3] */
	JE_byte     enemycycle;
	JE_byte     ani;
	JE_word     egr[20]; /* [1..20] */
	JE_byte     size;
	JE_byte     linknum;
	JE_byte     aniactive;
	JE_byte     animax;
	JE_byte     aniwhenfire;
	void       *shapeseg;
	JE_shortint exrev, eyrev;
	JE_integer  exccadd, eyccadd;
	JE_byte     exccwmax, eyccwmax;
	void       *enemydatofs;
	JE_boolean  edamaged;
	JE_word     enemytype;
	JE_byte     animin;
	JE_word     edgr;
	JE_shortint edlevel;
	JE_shortint edani;
	JE_byte     fill1;
	JE_byte     filter;
	JE_integer  evalue;
	JE_integer  fixedmovey;
	JE_byte     freq[3]; /* [1..3] */
	JE_byte     launchwait;
	JE_word     launchtype;
	JE_byte     launchfreq;
	JE_byte     xaccel;
	JE_byte     yaccel;
	JE_byte     tur[3]; /* [1..3] */
	JE_word     enemydie; /* Enemy created when this one dies */
	JE_boolean  enemyground;
	JE_byte     explonum;
	JE_word     mapoffset;
	JE_boolean  scoreitem;

	JE_boolean  special;
	JE_byte     flagnum;
	JE_boolean  setto;

	JE_byte     iced; /*Duration*/

	JE_byte     launchspecial;

	JE_integer  xminbounce;
	JE_integer  xmaxbounce;
	JE_integer  yminbounce;
	JE_integer  ymaxbounce;
	JE_byte     fill[3]; /* [1..3] */
};

typedef struct JE_SingleEnemyType JE_MultiEnemyType[100]; /* [1..100] */

typedef JE_word JE_DanCShape[(24 * 28) / 2]; /* [1..(24*28) div 2] */

typedef JE_char JE_CharString[256]; /* [1..256] */

typedef JE_byte JE_Map1Buffer[24 * 28 * 13 * 4]; /* [1..24*28*13*4] */

typedef JE_byte *JE_MapType[300][14]; /* [1..300, 1..14] */
typedef JE_byte *JE_MapType2[600][14]; /* [1..600, 1..14] */
typedef JE_byte *JE_MapType3[600][15]; /* [1..600, 1..15] */

struct JE_EventRecType
{
	JE_word     eventtime;
	JE_byte     eventtype;
	JE_integer  eventdat, eventdat2;
	JE_shortint eventdat3, eventdat5, eventdat6;
	JE_byte     eventdat4;
};

struct JE_MegaDataType1
{
	JE_MapType mainmap;
	struct
	{
		JE_DanCShape sh;
	} shapes[72]; /* [0..71] */
	JE_byte tempdat1;
	/*JE_DanCShape filler;*/
};

struct JE_MegaDataType2
{
	JE_MapType2 mainmap;
	struct
	{
		JE_byte nothing[3]; /* [1..3] */
		JE_byte fill;
		JE_DanCShape sh;
	} shapes[71]; /* [0..70] */
	JE_byte tempdat2;
};

struct JE_MegaDataType3
{
	JE_MapType3 mainmap;
	struct
	{
		JE_byte nothing[3]; /* [1..3] */
		JE_byte fill;
		JE_DanCShape sh;
	} shapes[70]; /* [0..69] */
	JE_byte tempdat3;
};

typedef JE_MultiEnemyType JE_EnemyType;
typedef JE_byte JE_EnemyAvailType[100]; /* [1..100] */

typedef JE_byte JE_REXtype[20]; /* [1..20] */

#ifndef NO_EXTERNS
extern JE_byte fromTime;
extern JE_integer tempDat, tempDat2, tempDat3;
extern JE_boolean tempb2;
extern JE_word test1;
extern const JE_byte SANextShip[SA + 2];
extern const JE_word SASpecialWeapon[SA];
extern const JE_word SASpecialWeaponB[SA];
extern const JE_byte SAShip[SA];
extern const JE_word SAWeapon[SA][5];
extern const JE_byte specialArcadeWeapon[PORT_NUM];
extern const JE_byte optionSelect[16][3][2];
extern const JE_word PGR[21];
extern const JE_byte PAni[21];
extern const JE_byte pItemButtonMap[7];
extern const JE_word linkGunWeapons[38];
extern const JE_word chargeGunWeapons[38];
extern const JE_word linkMultiGr[17];
extern const JE_word linkSonicGr[17];
extern const JE_word linkMult2Gr[17];
extern const JE_byte randomEnemyLaunchSounds[3];
extern const JE_byte keyboardCombos[26][8];
extern const JE_byte shipCombosB[21];
extern const JE_byte superTyrianSpecials[4];
extern const JE_byte shipCombos[13][3];
extern JE_byte SFCurrentcode[2][21];
extern JE_byte SFExecuted[2];
extern JE_boolean stoppedDemo;
extern JE_byte lvlFileNum;
extern JE_word maxEvent, eventLoc;
extern JE_word tempBackMove, explodeMove;
extern JE_byte levelEnd;
extern JE_word levelEndFxWait;
extern JE_shortint levelEndWarp;
extern JE_boolean showMemLeft, endLevel, reallyEndLevel, waitToEndLevel, playerEndLevel, normalBonusLevelCurrent, bonusLevelCurrent, smallEnemyAdjust, readyToEndLevel, quitRequested;
extern JE_byte newPL[10];
extern JE_word returnLoc;
extern JE_boolean returnActive;
extern JE_word galagaShotFreq;
extern JE_longint galagaLife;
extern JE_boolean debug;
extern Uint32 debugTime, lastDebugTime;
extern JE_longint debugHistCount;
extern JE_real debugHist;
extern JE_word curLoc;
extern JE_boolean firstGameOver, gameLoaded, enemyStillExploding;
extern JE_word tempSS;
extern JE_word totalEnemy;
extern JE_word enemyKilled;
extern JE_byte statBar[2], statCol[2];
extern JE_Map1Buffer *map1BufferTop, *map1BufferBot;
extern struct JE_MegaDataType1 *megaData1;
extern struct JE_MegaDataType2 *megaData2;
extern struct JE_MegaDataType3 *megaData3;
extern JE_byte flash;
extern JE_shortint flashChange;
extern JE_byte displayTime;
extern JE_boolean firstEvent;
extern FILE *recordFile;
extern JE_word recordFileNum;
extern JE_byte lastKey[8];
extern JE_word lastMoveWait, nextMoveWait;
extern JE_byte nextDemoOperation;
extern JE_boolean playDemo;
extern JE_byte playDemoNum;
extern JE_byte soundQueue[8];
extern JE_boolean enemyContinualDamage;
extern JE_boolean enemiesActive;
extern JE_boolean forceEvents;
extern JE_boolean stopBackgrounds;
extern JE_byte stopBackgroundNum;
extern JE_byte damageRate;
extern JE_boolean background3x1;
extern JE_boolean background3x1b;
extern JE_boolean levelTimer;
extern JE_word levelTimerCountdown;
extern JE_word levelTimerJumpTo;
extern JE_boolean randomExplosions;
extern JE_boolean editShip1, editShip2;
extern JE_boolean globalFlags[10];
extern JE_byte levelSong;
extern JE_boolean drawGameSaved;
extern JE_boolean repause;
extern JE_boolean loadDestruct;
extern JE_word mapOrigin, mapPNum;
extern JE_byte mapPlanet[5], mapSection[5];
extern JE_boolean loadTitleScreen;
extern JE_boolean moveTyrianLogoUp;
extern JE_boolean skipStarShowVGA;
extern JE_EnemyType enemy;
extern JE_EnemyAvailType enemyAvail;
extern JE_word enemyAvailOfs, topEnemyAvailOfs, groundEnemyAvailOfs, groundEnemyAvailOfs2, enemyOffset;
extern JE_word enemyOnScreen;
extern JE_byte enemyShapeTables[6];
extern JE_boolean uniqueEnemy;
extern JE_word superEnemy254Jump;
extern struct {
	JE_word explodeLoc;
	JE_word explodeGr;
	JE_byte followPlayer;
	JE_byte fixedExplode;
	JE_word fixedMovement;
} explosions[EXPLOSION_MAX];
extern JE_byte explodeAvail[EXPLOSION_MAX];
extern JE_integer explosionFollowAmount;
extern JE_boolean playerFollow, fixedExplosions;
extern JE_integer explosionMoveUp;
extern JE_boolean fireButtonHeld;
extern JE_byte /*boolean*/ enemyShotAvail[ENEMY_SHOT_MAX];
extern struct {
	JE_integer sx, sy;
	JE_integer sxm, sym;
	JE_shortint sxc, syc;
	JE_byte tx, ty;
	JE_word sgr;
	JE_byte sdmg;
	JE_byte duration;
	JE_word animate;
	JE_word animax;
	JE_byte fill[12];
} enemyShot[ENEMY_SHOT_MAX];
extern JE_byte zinglonDuration;
extern JE_byte astralDuration;
extern JE_word flareDuration;
extern JE_boolean flareStart;
extern JE_shortint flareColChg;
extern JE_byte specialWait;
extern JE_byte nextSpecialWait;
extern JE_boolean spraySpecial;
extern JE_byte doIced;
extern JE_boolean infiniteShot;
extern JE_byte superBomb[2];
extern JE_integer tempShotX, tempShotY;
extern struct {
	JE_integer shotX, shotY, shotXM, shotYM, shotXC, shotYC;
	JE_boolean shotComplicated;
	JE_integer shotDevX, shotDirX, shotDevY, shotDirY, shotCirSizeX, shotCirSizeY;
	JE_byte shotTrail;
	JE_word shotGr, shotAni, shotAniMax;
	JE_shortint shotDmg;
	JE_byte shotBlastFilter, chainReaction, playerNumber, aimAtEnemy, aimDelay, aimDelayMax, fill[1];
} playerShotData[MAX_PWEAPON + 1];
extern JE_byte chain;
extern JE_boolean allPlayersGone;
extern JE_byte shotAvail[MAX_PWEAPON];
extern JE_byte shadowyDist;
extern JE_byte purpleBallsRemaining[2];
extern JE_boolean playerAlive, playerAliveB;
extern JE_byte playerStillExploding, playerStillExploding2;
extern JE_byte *eShapes1, *eShapes2, *eShapes3, *eShapes4, *eShapes5, *eShapes6;
extern JE_byte *shapesC1, *shapes6, *shapes9, *shapesW2;
extern JE_word eShapes1Size, eShapes2Size, eShapes3Size, eShapes4Size, eShapes5Size, eShapes6Size, shapesC1Size, shapes6Size, shapes9Size, shapesW2Size;
extern JE_byte sAni;
extern JE_integer sAniX, sAniY, sAniXNeg, sAniYNeg;
extern JE_integer baseSpeedOld, baseSpeedOld2, baseSpeed, baseSpeedB, baseSpeed2, baseSpeed2B, baseSpeedKeyH, baseSpeedKeyV;
extern JE_boolean keyMoveWait;
extern JE_boolean makeMouseDelay;
extern JE_word playerInvulnerable1, playerInvulnerable2;
extern JE_integer lastPXShotMove, lastPYShotMove;
extern JE_integer PXB, PYB, lastPXB, lastPYB, lastPX2B, lastPY2B, PXChangeB, PYChangeB, lastTurnB, lastTurn2B, tempLastTurn2B;
extern JE_byte stopWaitXB, stopWaitYB;
extern JE_word mouseXB, mouseYB;
extern JE_integer PX, PY, lastPX, lastPY, lastPX2, lastPY2, PXChange, PYChange, lastTurn, lastTurn2, tempLastTurn2;
extern JE_byte stopWaitX, stopWaitY;
extern JE_integer PYHist[3], PYHistB[3];
extern JE_word option1Draw, option2Draw, option1Item, option2Item;
extern JE_byte option1AmmoMax, option2AmmoMax;
extern JE_word option1AmmoRechargeWait, option2AmmoRechargeWait, option1AmmoRechargeWaitMax, option2AmmoRechargeWaitMax;
extern JE_integer option1Ammo, option2Ammo;
extern JE_integer optionAni1, optionAni2, optionCharge1, optionCharge2, optionCharge1Wait, optionCharge2Wait, option1X, option1LastX, option1Y, option1LastY, option2X, option2LastX, option2Y, option2LastY, option1MaxX, option1MinX, option2MaxX, option2MinX, option1MaxY, option1MinY, option2MaxY, option2MinY;
extern JE_boolean optionAni1Go, optionAni2Go, option1Stop, option2Stop;
extern JE_real optionSatelliteRotate;
extern JE_integer optionAttachmentMove;
extern JE_boolean optionAttachmentLinked, optionAttachmentReturn;
extern JE_byte chargeWait, chargeLevel, chargeMax, chargeGr, chargeGrWait;
extern JE_boolean playerHNotReady;
extern JE_word playerHX[20], playerHY[20];
extern JE_word neat;
extern JE_REXtype REXavail;
extern struct {
	JE_byte delay;
	JE_word ex, ey;
	JE_boolean big;
} REXdat[20];
extern JE_byte SPZ[MAX_SP + 1];
extern struct {
	JE_word location;
	JE_word movement;
	JE_byte color;
} SPL[MAX_SP + 1];
extern JE_word lastSP;
extern JE_word megaDataOfs, megaData2Ofs, megaData3Ofs;
extern JE_word avail;
extern JE_word tempCount;
extern JE_integer tempI, tempI2, tempI3, tempI4, tempI5;
extern JE_longint tempL;
extern JE_real tempR, tempR2;
extern JE_boolean tempB;
extern JE_byte temp, temp2, temp3, temp4, temp5, tempREX, tempPos;
extern JE_word tempX, tempY, tempX2, tempY2;
extern JE_word tempW, tempW2, tempW3, tempW4, tempW5, tempOfs;
extern JE_boolean doNotSaveBackup;
extern JE_boolean tempSpecial;
extern JE_word x, y;
extern JE_integer a, b, c, d, z, zz;
extern JE_byte playerNum;
extern JE_byte **BKwrap1to, **BKwrap2to, **BKwrap3to, **BKwrap1, **BKwrap2, **BKwrap3;
extern JE_byte min, max;
extern JE_shortint specialWeaponFilter, specialWeaponFreq;
extern JE_word specialWeaponWpn;
extern JE_boolean linkToPlayer;
extern JE_integer baseArmor, baseArmor2;
extern JE_word shipGr, shipGr2;
extern JE_byte *shipGrPtr, *shipGr2ptr;
#endif

void JE_getShipInfo( void );
JE_word JE_SGr( JE_word ship, JE_byte **ptr );

void JE_calcPurpleBall( JE_byte playernum );
void JE_drawOptions( void );

void JE_tyrianHalt( JE_byte code ); /* This ends the game */
void JE_initPlayerShot( JE_word portnum, JE_byte temp, JE_word px, JE_word py,
                        JE_word mousex, JE_word mousey,
                        JE_word wpnum, JE_byte playernum );
void JE_specialComplete( JE_byte playernum, JE_integer *armor, JE_shortint *shield, JE_byte specialType );
void JE_doSpecialShot( JE_byte playernum, JE_integer *armor, JE_shortint *shield );

void JE_powerUp( JE_byte port );
void JE_wipeShieldArmorBars( void );
JE_byte JE_playerDamage( JE_word tempX, JE_word tempY, JE_byte temp,
                         JE_integer *PX, JE_integer *PY,
                         JE_boolean *playerAlive,
                         JE_byte *playerStillExploding,
                         JE_integer *armorLevel,
                         JE_shortint *shield );

void JE_setupExplosion( JE_integer x, JE_integer y, JE_integer explodetype );
void JE_setupExplosionLarge( JE_boolean enemyground, JE_byte explonum, JE_integer x, JE_integer y );

void JE_drawShield( void );
void JE_drawArmor( void );

void JE_portConfigs( void );

void JE_resetPlayerH( void );

void JE_doSP( JE_word x, JE_word y, JE_word num, JE_byte explowidth, JE_byte color ); /*SuperPixels*/
void JE_drawSP( void );

void JE_drawOptionLevel( void );


#endif /* VARZ_H */
