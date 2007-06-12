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
extern JE_shortint constantLastX;
extern JE_word textErase;
extern JE_word upgradeCost;
extern JE_word downgradeCost;
extern JE_boolean performSave;
extern JE_boolean jumpSection;
extern JE_boolean useLastBank;
#endif

/*void JE_textMenuWait ( JE_word waittime, JE_boolean dogamma );*/

void JE_drawTextWindow( char *text );
void JE_initPlayerData( void );
void JE_highScoreScreen( void );
void JE_gammaCorrect( JE_ColorType *colorBuffer, JE_byte gamma );
JE_boolean JE_gammaCheck( void );
/* void JE_textMenuWait( JE_word *waitTime, JE_boolean doGamma ); /!\ In setup.h */
void JE_loadOrderingInfo( void );
JE_boolean JE_nextEpisode( void );
JE_boolean JE_episodeSelect( void );
JE_boolean JE_difficultySelect( void );
JE_boolean JE_playerSelect( void );
void JE_helpSystem( JE_byte startTopic );
void JE_doInGameSetup( void );
JE_boolean JE_inGameSetup( void );
void JE_inGameHelp( void );
void JE_sortHighScores( void );
void JE_highScoreCheck( void );
void JE_setNewGameVol( void );
void JE_changeDifficulty( void );
void JE_doDemoKeys( void );
void JE_readDemoKeys( void );
void JE_sfCodes( void );
void JE_func( JE_byte col );
void JE_sort( void );

JE_word JE_powerLevelCost( JE_word base, JE_byte level );
JE_longint JE_getCost( JE_byte itemType, JE_word itemNum );
JE_longint JE_getValue( JE_byte itemType, JE_word itemNum );
JE_longint JE_totalScore( JE_longint score, JE_PItemsType pitems );

void JE_drawPortConfigButtons( void );
void JE_outCharGlow( JE_word x, JE_word y, char *s );
JE_boolean JE_getPassword( void );

void JE_playCredits( void );
void JE_endLevelAni( void );
void JE_drawCube( JE_word x, JE_word y, JE_byte filter, JE_byte brightness );
void JE_handleChat( void );
JE_boolean JE_getNumber( char *s, JE_byte *x );
void JE_loadScreen( void );
void JE_operation( JE_byte slot );
void JE_inGameDisplays( void );
void JE_mainKeyboardInput( void );
void JE_pauseGame( void );

void JE_loadCompShapesB( JE_byte **shapes, FILE *f, JE_word shapeSize );

void JE_loadMainShapeTables( void );
void JE_playerMovement( JE_byte inputDevice, JE_byte playerNum, JE_word shipGr, JE_byte *shapes9ptr, JE_integer *armorLevel, JE_integer *baseArmor, JE_shortint *shield, JE_shortint *shieldMax, JE_word *playerInvulnerable, JE_integer *PX, JE_integer *PY, JE_integer *lastPX, JE_integer *lastPY, JE_integer *lastPX2, JE_integer *lastPY2, JE_integer *PXChange, JE_integer *PYChange, JE_integer *lastTurn, JE_integer *lastTurn2, JE_integer *tempLastTurn2, JE_byte *stopWaitX, JE_byte *stopWaitY, JE_word *mouseX, JE_word *mouseY, JE_boolean *playerAlive, JE_byte *playerStillExploding, JE_PItemsType pItems );
void JE_mainGamePlayerFunctions( void );
char *JE_getName( JE_byte pnum );

void JE_playerCollide( JE_integer *px, JE_integer *py, JE_integer *lastTurn, JE_integer *lastTurn2,
                       JE_longint *score, JE_integer *armorLevel, JE_shortint *shield, JE_boolean *playerAlive,
                       JE_byte *playerStillExploding, JE_byte playerNum, JE_byte playerInvulnerable );


#endif /* MAININT_H */
