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
#ifndef BACKGRND_H
#define BACKGRND_H

#include "opentyr.h"

#include <inttypes.h>

#ifndef NO_EXTERNS
extern JE_word backPos, backPos2, backPos3;
extern JE_word backMove, backMove2, backMove3;
extern JE_word megaDataSeg, megaDataOfs, megaData2Seg, megaData2Ofs, megaData3Seg, megaData3Ofs;
extern JE_word mapX, mapY, mapX2, mapX3, mapY2, mapY3;
extern JE_byte **mapYPos, **mapY2Pos, **mapY3Pos;
extern JE_word mapXPos, oldMapXOfs, mapXOfs, mapX2Ofs, mapX2Pos, mapX3Pos, oldMapX3Ofs, mapX3Ofs, tempMapXOfs;
extern intptr_t mapXbpPos, mapX2bpPos, mapX3bpPos;
extern JE_byte map1YDelay, map1YDelayMax, map2YDelay, map2YDelayMax;
extern void *smoothiesScreen;
extern JE_word smoothiesSeg;
extern JE_boolean anySmoothies;
extern JE_word TSS;
extern JE_byte SDAT[9];
#endif

void JE_darkenBackground( JE_word neat );
void JE_drawBackground2( void );
void JE_superBackground2( void );
void JE_filterScreen( JE_shortint col, JE_shortint generic_int );

void JE_drawBackground3( void );

void JE_initSmoothies( void );

void JE_checkSmoothies( void );
void JE_smoothies1( void );
void JE_smoothies2( void );
void JE_smoothies3( void );
void JE_smoothies4( void );
/*smoothies #5 is used for 3*/
void JE_smoothies6( void );
/*smoothies #9 is a vertical flip*/

#endif /* BACKGRND_H */
