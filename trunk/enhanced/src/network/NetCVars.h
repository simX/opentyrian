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
#ifndef NETWORK_NETCVARS_H
#define NETWORK_NETCVARS_H
#include "opentyr.h"

#include "console/cvar/CVar.h"

namespace CVars
{
	extern CVarBool net_enabled;

	extern CVarString net_host;
	extern CVarInt net_port;
	extern CVarInt net_player;
	extern CVarString net_name;
	extern CVarInt net_local_port;
	extern CVarInt net_delay;

	extern CVarInt net_keepalive_interval;
	extern CVarInt net_resend_interval;
	extern CVarBool net_ignore_version;
	extern CVarBool net_debug;
}

#endif // NETWORK_NETCVARS_H
