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
#ifndef GAMEACTIONS_H
#define GAMEACTIONS_H

enum GameActionsEnum
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	FIRE,
	CHANGE_FIRE,
	LEFT_SIDEKICK,
	RIGHT_SIDEKICK,

	NUM_GAME_ACTIONS
};

struct KeyConfig { std::string title; std::string command; bool toggle; };
static const int num_keyConfigs = 8;
extern KeyConfig keyConfigs[num_keyConfigs];

extern bool gameInputEnabled[NUM_GAME_ACTIONS];

#endif // GAMEACTIONS_H
