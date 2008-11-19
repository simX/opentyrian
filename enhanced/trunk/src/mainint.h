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
#ifndef MAININT_H
#define MAININT_H

#include "opentyr.h"

#include "config.h"
#include "nortvars.h"
#include "video.h"


extern int constantLastX;
extern JE_word textErase;
extern JE_word upgradeCost;
extern JE_word downgradeCost;
extern bool performSave;
extern bool jumpSection;
extern bool useLastBank;
extern bool loadedMainShapeTables;

extern int player_delta_x[2], player_delta_y[2];

/*void JE_textMenuWait ( JE_word waittime, bool dogamma );*/

void JE_drawTextWindow( const std::string& text );
void JE_initPlayerData( void );
void JE_highScoreScreen( void );
void JE_gammaCorrect_func( Uint8 *col, float r );
void JE_gammaCorrect( Palette colorBuffer, int gamma );
void JE_gammaCheck( );
/* void JE_textMenuWait( JE_word *waitTime, bool doGamma ); /!\ In setup.h */
bool JE_nextEpisode( void );
void JE_helpSystem( int startTopic );
void JE_doInGameSetup( void );
bool JE_inGameSetup( void );
void JE_inGameHelp( void );
void JE_highScoreCheck( void );
void JE_changeDifficulty( void );
void JE_doDemoKeys( void );
void JE_readDemoKeys( void );
void JE_SFCodes( int playerNum_, int PX_, int PY_, int mouseX_, int mouseY_, JE_PItemsType pItems_ );

JE_word JE_powerLevelCost( JE_word base, int level );
unsigned long JE_getCost( int itemType, JE_word itemNum );
unsigned int JE_getValue( int itemType, JE_word itemNum );
unsigned long JE_totalScore( unsigned long score, JE_PItemsType pitems );

void JE_drawPortConfigButtons( void );
void JE_outCharGlow( JE_word x, JE_word y, const std::string& s );
bool JE_getPassword( void );

void JE_playCredits( void );
void JE_endLevelAni( void );
void JE_drawCube( JE_word x, JE_word y, int filter, int brightness );
void JE_handleChat( void );
bool JE_getNumber( char *s, int *x );
void JE_loadScreen( void );
void JE_operation( int slot );
void JE_inGameDisplays( void );
void JE_mainKeyboardInput( void );
void JE_pauseGame( void );

void JE_loadCompShapesB( Uint8 **shapes, std::fstream& f, unsigned long shapeSize );

void JE_loadMainShapeTables( void );
void JE_playerMovement( int inputDevice, int playerNum, JE_word shipGr, Uint8 *shapes9ptr, int *armorLevel, int *baseArmor, int *shield, int *shieldMax, JE_word *playerInvulnerable, int *PX, int *PY, int *lastPX, int *lastPY, int *lastPX2, int *lastPY2, int *PXChange, int *PYChange, int *lastTurn, int *lastTurn2, int *tempLastTurn2, int *stopWaitX, int *stopWaitY, JE_word *mouseX, JE_word *mouseY, bool *playerAlive, int *playerStillExploding, JE_PItemsType pItems );
void JE_mainGamePlayerFunctions( void );
char *JE_getName( int pnum );

void JE_playerCollide( int *px, int *py, int *lastTurn, int *lastTurn2,
                       unsigned long *score, int *armorLevel, int *shield, bool *playerAlive,
                       int *playerStillExploding, int playerNum, int playerInvulnerable );


#endif /* MAININT_H */
