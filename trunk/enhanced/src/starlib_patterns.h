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
#ifndef STARLIB_PATTERNS_H
#define STARLIB_PATTERNS_H
#include "opentyr.h"

#include "starlib.h"

namespace starlib { namespace patterns
{
	/**
	 * Returns a new instance of \p T. Intended to be used as a delegate for Starfield::addPattern.
	 *
	 * @return a heap allocated instance of \p T.
	 */
	template<class T> Pattern *factory()
	{
		return new T;
	}

	/**
	 * Places stars randomly.
	 */
	class Pattern00 : public Pattern
	{
	public:
		Star newStar();
	};
}}

#endif // STARLIB_PATTERNS_H
