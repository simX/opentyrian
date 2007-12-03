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
#ifndef TYRIAN2_H
#define TYRIAN2_H

#include "opentyr.h"

#include "varz.h"
#include "helptext.h"


#define CUBE_WIDTH 35
#define LINE_WIDTH 150

extern JE_word statDmg[2];

void JE_scaleInPicture( void );

void JE_createNewEventEnemy( int enemytypeofs, JE_word enemyoffset );

void JE_genItemMenu( int itemnum );

/*void JE_drawBackground3( void );*/

void JE_weaponViewFrame( int testshotnum );

void JE_doNetwork( void );

void JE_makeEnemy( struct JE_SingleEnemyType *enemy );

void JE_weaponSimUpdate( void );

void JE_funkyScreen( void );

void JE_drawJoystick( void );

void JE_eventJump( JE_word jump );

void JE_menuFunction( int select );

void JE_drawScore( void );

void JE_doStatBar( void );

bool JE_quitRequest( bool usemouse );

void JE_whoa( void );

void JE_drawMainMenuHelpText( void );

void JE_doFunkyScreen( void );

int JE_partWay( int start, int finish, int dots, int dist );

void JE_computeDots( void );

void JE_barX ( JE_word x1, JE_word y1, JE_word x2, JE_word y2, Uint8 col );

void JE_initWeaponView( void );

void JE_scaleBitmap ( JE_word bitmap, JE_word x, JE_word y, JE_word x1, JE_word y1, JE_word x2, JE_word y2 );

typedef int JE_MenuChoiceType[MAX_MENU];

void JE_newEnemy( int enemyOffset );
void JE_drawEnemy( int enemyOffset );
void JE_starShowVGA( void );

void JE_main( void );
void JE_loadMap( void );
void JE_titleScreen( bool animate );
void JE_openingAnim( void );
void JE_readTextSync( void );
void JE_displayText( void );

bool JE_searchFor( int PLType );
void JE_eventSystem( void );

unsigned long JE_cashLeft( void );
void JE_loadCubes( void );
void JE_drawItem( int itemType, JE_word itemNum, JE_word x, JE_word y );
void JE_itemScreen( void );

void JE_drawMenuHeader( void );
void JE_drawMenuChoices( void );
void JE_updateNavScreen( void );

void JE_drawNavLines( bool dark );
void JE_drawLines( bool dark );
void JE_drawDots( void );
void JE_drawPlanet( int planetNum );

#endif /* TYRIAN2_H */
