/* 
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
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
#include "NetCVars.h"

#include "console/cvar/CVar.h"
#include "console/CCmd.h"

#include "boost/lexical_cast.hpp"
using boost::lexical_cast;

namespace CVars
{
	CVarBool net_enabled("net_enabled", CVar::NONE, "Plays a networked multiplayer game.", false);

	CVarString net_host("net_host", CVar::NONE, "Multiplayer opponent's hostname/ip.", "");
	CVarInt net_port("net_port", CVar::NONE, "Multiplayer opponent's port number.", 1333, rangeBind(0, 65535));
	CVarInt net_player("net_player", CVar::NONE, "Multiplayer's local player number. Must be 1 or 2.", 1, rangeBind(1, 2));
	CVarString net_name("net_name", CVar::CONFIG, "Multiplayer name.", "Player");
	CVarInt net_local_port("net_local_port", CVar::CONFIG, "Multiplayer local port number.", 1333, rangeBind(0, 65535));
	CVarInt net_delay("net_delay", CVar::NONE, "Multiplayer lag compensation delay.", 1+1, rangeBind(1, 6));

	// Less useful ones
	CVarInt net_keepalive_interval("net_keepalive_interval", CVar::NONE, "Number of ticks between sending keep alives.", 1500, rangeBind(0, 65535));
	CVarInt net_resend_interval("net_resend_interval", CVar::NONE, "Number of ticks between resend of reliable packets.", 500, rangeBind(0, 65535));
	CVarBool net_ignore_version("net_ignore_version", CVar::NONE, "Disables version check. Don't enable unless you know what you're doing.", false);
	CVarBool net_debug("net_debug", CVar::NONE, "Prints networking debugging information.", false);
}

namespace CCmds
{
	namespace Func
	{
		static void net_set( const std::vector<std::string>& params )
		{
			std::string param1 = CCmd::convertParam<std::string>(params, 0);
			int param2 = CCmd::convertParam<int>(params, 1);

			std::string::size_type pos = param1.rfind(':');
			if (pos != std::string::npos)
			{
				std::string port = param1.substr(pos);
				param1 = param1.substr(0, pos);
				int port_num;

				try
				{
					try
					{
						port_num = lexical_cast<int>(port.substr(1));
					}
					catch (std::out_of_range&)
					{
						throw boost::bad_lexical_cast();
					}

					if (port_num < 0 || port_num > 65535)
						throw boost::bad_lexical_cast();

					CVars::net_port = port_num;
				}
				catch (boost::bad_lexical_cast&)
				{
					Console::get() << "\a7Warning:\ax Bad port number." << std::endl;
				}
			}

			CVars::net_host = param1;
			CVars::net_player = param2;

			if (params.size() >= 3)
			{
				std::string param3 = CCmd::convertParam<std::string>(params, 2);
				CVars::net_name = param3;
			}

			CVars::net_enabled = true;
		}
	}

	CCmd net_set("net_set", CCmd::NONE, "Convenience ccmd to set networking options. Usage: net_set [host[:port] [player] [name]", Func::net_set);
}
