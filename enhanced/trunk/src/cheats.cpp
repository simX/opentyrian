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
#include "opentyr.h"

#include "console/CCmd.h"
#include "console/Console.h"
#include "config.h"
#include "episodes.h"

namespace CCmds
{
	namespace Func
	{
		static void ch_give_item( const std::vector<std::string>& params )
		{
			unsigned int param1 = CCmd::convertParam<unsigned int>(params, 0);
			unsigned int param2 = 0;
			
			if (params.size() >= 2)
			{
				param2 = CCmd::convertParam<unsigned int>(params, 1);

				if (param2 > 11)
					throw CCmd::RuntimeCCmdError("Slot must be less than 12.");
			}

			if (param1 == 1)
			{
				if (params.size() >= 3)
				{
					pItems[param2] = CCmd::convertParam<int>(params, 2);
				}
				else
				{
					if (params.size() >= 2)
					{
						Console::get() << pItems[param2] << std::endl;
					}
					else
					{
						for (int i = 0; i < 12; ++i)
						{
							Console::get() << pItems[i] << ' ';
						}
						Console::get() << std::endl;
					}
				}
			}
			else if (param1 == 2)
			{
				if (params.size() >= 3)
				{
					pItemsPlayer2[param2] = CCmd::convertParam<int>(params, 2);
				}
				else
				{
					if (params.size() >= 2)
					{
						Console::get() << pItemsPlayer2[param2] << std::endl;
					}
					else
					{
						for (int i = 0; i < 12; ++i)
						{
							Console::get() << pItemsPlayer2[i] << ' ';
						}
						Console::get() << std::endl;
					}
				}
			}
			else
			{
				throw CCmd::RuntimeCCmdError("Player must be 1 or 2.");
			}
		}

		static void debug_dump_weapons( const std::vector<std::string>& params )
		{
			for (unsigned int i = 0; i <= PORT_NUM; ++i)
			{
				Console::get() << i << ": " << weaponPort[i].name << '\n';
			}
			Console::get() << std::flush;
		}
	}

	CCmd ch_give_item("ch_give_item", CCmd::CHEAT, "Gives an item to a player. If no item id is given prints current item in slot. Usage: ch_give_item [player] [slot_id] [item_id]", Func::ch_give_item);
	CCmd debug_dump_weapons("debug_dump_weapons", CCmd::NONE, "Lists all weapons. Usage: debug_dump_weapons", Func::debug_dump_weapons);
}

