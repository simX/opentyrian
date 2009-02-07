/* vim: set noet:
 *
 * OpenTyrian Enhanced: A modern cross-platform port of Tyrian
 * Copyright (C) 2007-2009  The OpenTyrian Development Team
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


static const int SOUND_NUM = 29;

static const int CLICK = 24;
static const int WRONG = 23;
static const int ESC = 16;
static const int ITEM = 18;
static const int SELECT = 8;
static const int CURSOR_MOVE = 28;
static const int POWERUP = 29;

static const int V_GOOD_LUCK = SOUND_NUM+4;
static const int V_LEVEL_END = SOUND_NUM+5;
static const int V_DATA_CUBE = SOUND_NUM+8;

extern const char soundTitle[SOUND_NUM + 9][9];
extern const int windowTextSamples[9];

#endif /* SNDMAST_H */
