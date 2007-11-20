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
#ifndef LVLMAST_H
#define LVLMAST_H

#include "opentyr.h"


#define EVENT_MAXIMUM 2500

#define WEAP_NUM    780
#define PORT_NUM    42
#define ARMOR_NUM   4
#define POWER_NUM   6
#define ENGINE_NUM  6
#define OPTION_NUM  30
#define SHIP_NUM    13
#define SHIELD_NUM  10
#define SPECIAL_NUM 46

#define ENEMY_NUM   850

#define LVL_NUM   (18 * 2)
#define LVL_NUM_2 (12 * 2)
#define LVL_NUM_3 (12 * 2)
#define LVL_NUM_4 (20 * 2)
#define LVL_NUM_5 (1 * 2)

/*! typedef JE_longint JE_LvlPosType[LVLNum + 2];*/ /* [1..LVLnum + 2] */

extern const JE_char shapeFile[34]; /* [1..34] */
extern const char lvlFile[LVL_NUM][9]; /* [1..LVLnum] of string [8] */
extern const char lvlFile2[LVL_NUM_2][9]; /* [1..LVLnum2] of string [8] */
extern const char lvlFile3[LVL_NUM_3][9]; /* [1..LVLnum3] of string [8] */
extern const char lvlFile4[LVL_NUM_4][9]; /* [1..LVLnum4] of string [8] */
extern const char lvlFile5[LVL_NUM_5][9]; /* [1..lvlnum5] of string [8] */
/*! extern JE_LvlPosType lvlPos;*/

#endif /* LVLMAST_H */
