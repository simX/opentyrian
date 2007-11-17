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
#ifndef MAININT_H
#define MAININT_H

#include "opentyr.h"

#include "config.h"
#include "nortvars.h"


#ifndef NO_EXTERNS
extern int constantLastX;
extern JE_word textErase;
extern JE_word upgradeCost;
extern JE_word downgradeCost;
extern bool performSave;
extern bool jumpSection;
extern bool useLastBank;
#endif

/*void JE_textMenuWait ( JE_word waittime, bool dogamma );*/

void JE_drawTextWindow( char *text );
void JE_initPlayerData( void );
void JE_highScoreScreen( void );
void JE_gammaCorrect_func( JE_byte *col, double r );
void JE_gammaCorrect( JE_ColorType *colorBuffer, JE_byte gamma );
bool JE_gammaCheck( void );
/* void JE_textMenuWait( JE_word *waitTime, bool doGamma ); /!\ In setup.h */
void JE_loadOrderingInfo( void );
bool JE_nextEpisode( void );
bool JE_episodeSelect( void );
bool JE_difficultySelect( void );
bool JE_playerSelect( void );
void JE_helpSystem( JE_byte startTopic );
void JE_doInGameSetup( void );
bool JE_inGameSetup( void );
void JE_inGameHelp( void );
void JE_sortHighScores( void );
void JE_highScoreCheck( void );
void JE_setNewGameVol( void );
void JE_changeDifficulty( void );
void JE_doDemoKeys( void );
void JE_readDemoKeys( void );
void JE_SFCodes( JE_byte playerNum_, int PX_, int PY_, int mouseX_, int mouseY_, JE_PItemsType pItems_ );
void JE_func( JE_byte col );
void JE_sort( void );

JE_word JE_powerLevelCost( JE_word base, JE_byte level );
unsigned long JE_getCost( JE_byte itemType, JE_word itemNum );
unsigned int JE_getValue( JE_byte itemType, JE_word itemNum );
unsigned long JE_totalScore( unsigned long score, JE_PItemsType pitems );

void JE_drawPortConfigButtons( void );
void JE_outCharGlow( JE_word x, JE_word y, char *s );
bool JE_getPassword( void );

void JE_playCredits( void );
void JE_endLevelAni( void );
void JE_drawCube( JE_word x, JE_word y, JE_byte filter, JE_byte brightness );
void JE_handleChat( void );
bool JE_getNumber( char *s, JE_byte *x );
void JE_loadScreen( void );
void JE_operation( JE_byte slot );
void JE_inGameDisplays( void );
void JE_mainKeyboardInput( void );
void JE_pauseGame( void );

void JE_loadCompShapesB( JE_byte **shapes, FILE *f, JE_word shapeSize );

void JE_loadMainShapeTables( void );
void JE_playerMovement( JE_byte inputDevice, JE_byte playerNum, JE_word shipGr, JE_byte *shapes9ptr, int *armorLevel, int *baseArmor, int *shield, int *shieldMax, JE_word *playerInvulnerable, int *PX, int *PY, int *lastPX, int *lastPY, int *lastPX2, int *lastPY2, int *PXChange, int *PYChange, int *lastTurn, int *lastTurn2, int *tempLastTurn2, JE_byte *stopWaitX, JE_byte *stopWaitY, JE_word *mouseX, JE_word *mouseY, bool *playerAlive, JE_byte *playerStillExploding, JE_PItemsType pItems );
void JE_mainGamePlayerFunctions( void );
char *JE_getName( JE_byte pnum );

void JE_playerCollide( int *px, int *py, int *lastTurn, int *lastTurn2,
                       unsigned long *score, int *armorLevel, int *shield, bool *playerAlive,
                       JE_byte *playerStillExploding, JE_byte playerNum, JE_byte playerInvulnerable );


#endif /* MAININT_H */
