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
#include "opentyr.h"
#include "GameActions.h"

#include "CCmd.h"

KeyConfig keyConfigs[num_keyConfigs] =
{
	{"UP", "up", true},
	{"DOWN", "down", true},
	{"LEFT", "left", true},
	{"RIGHT", "right", true},
	{"FIRE", "fire", true},
	{"CHANGE FIRE", "change_fire", true},
	{"LEFT SIDEKICK", "left_sidekick", true},
	{"RIGHT SIDEKICK", "right_sidekick", true}
};

bool gameInputEnabled[NUM_GAME_ACTIONS];

namespace CCmds
{
	namespace Func
	{
		template<GameActionsEnum i> static void setGameInput( const std::vector<std::string>& params )
		{
			bool v = CCmd::convertParam<bool>(params,0);
			gameInputEnabled[i] = v;
		}
	}

	CCmd up("up", CCmd::NONE, "Moves up", Func::setGameInput<UP>);
	CCmd down("down", CCmd::NONE, "Moves down", Func::setGameInput<DOWN>);
	CCmd left("left", CCmd::NONE, "Moves left", Func::setGameInput<LEFT>);
	CCmd right("right", CCmd::NONE, "Moves right", Func::setGameInput<RIGHT>);
	CCmd fire("fire", CCmd::NONE, "Moves fire both primary and secondary weapons", Func::setGameInput<FIRE>);
	CCmd change_fire("change_fire", CCmd::NONE, "Toggles secondary weapon firing mode", Func::setGameInput<CHANGE_FIRE>);
	CCmd left_sidekick("left_sidekick", CCmd::NONE, "Fires left sidekick", Func::setGameInput<LEFT_SIDEKICK>);
	CCmd right_sidekick("right_sidekick", CCmd::NONE, "Fires right sidekick", Func::setGameInput<RIGHT_SIDEKICK>);
}

