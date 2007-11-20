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
#ifndef SNDMAST_H
#define SNDMAST_H

#include "opentyr.h"


#define SOUND_NUM 29

#define CLICK        24
#define WRONG        23
#define ESC          16
#define ITEM         18
#define SELECT       8
#define CURSOR_MOVE  28
#define POWERUP      29

#define V_GOOD_LUCK (SOUND_NUM + 4)
#define V_LEVEL_END (SOUND_NUM + 5)
#define V_DATA_CUBE (SOUND_NUM + 8)

extern const char soundTitle[SOUND_NUM + 9][9];
extern const JE_byte windowTextSamples[9];

#endif /* SNDMAST_H */
