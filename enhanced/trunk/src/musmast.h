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
#ifndef MUSMAST_H
#define MUSMAST_H

#include "opentyr.h"


static const int DEFAULT_SONG_BUY = 3;
static const int SONG_LEVELEND = 10;
static const int SONG_GAMEOVER = 11;
static const int SONG_MAPVIEW = 20;
static const int SONG_ENDGAME1 = 8;
static const int SONG_ZANAC = 32;
static const int SONG_TITLE = 30;

static const int MUSIC_NUM = 41;

extern int songBuy;
extern const char musicFile[MUSIC_NUM][13];
extern const char musicTitle[MUSIC_NUM][48];
extern bool musicFade;

#endif /* MUSMAST_H */
