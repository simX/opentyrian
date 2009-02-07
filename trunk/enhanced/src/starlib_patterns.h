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
#ifndef STARLIB_PATTERNS_H
#define STARLIB_PATTERNS_H
#include "opentyr.h"

#include "starlib.h"

#include <string>

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

	class Pattern00 : public Pattern
	{
	public:
		Star newStar();
		std::string title() { return "Starfield"; }
	};

	class Pattern01 : public Pattern
	{
		float time;
	public:
		Pattern01();
		void step(float speed);
		Star newStar();
		std::string title() { return "Horizontal Wave"; }
	};

	class Pattern02 : public Pattern
	{
		float time;
	public:
		Pattern02();
		void step(float speed);
		Star newStar();
		std::string title() { return "Star Helix"; }
	};

	class Pattern03 : public Pattern
	{
		float time;
	public:
		Pattern03();
		void step(float speed);
		Star newStar();
		std::string title() { return "Wormhole"; }
	};

	class Pattern04 : public Pattern
	{
		float time;
	public:
		Pattern04();
		void step(float speed);
		Star newStar();
		std::string title() { return "Figure 8"; }
	};

	class Pattern05 : public Pattern
	{
		float time;
	public:
		Pattern05();
		void step(float speed);
		Star newStar();
		std::string title() { return "Light Valley"; }
	};

	class Pattern06 : public Pattern
	{
		float time;
	public:
		Pattern06();
		void step(float speed);
		Star newStar();
		std::string title() { return "Through the Stargate"; }
	};

	class Pattern07 : public Pattern
	{
		float time;
		float time2;
		bool reverse;
	public:
		Pattern07();
		void step(float speed, float speed2);
		Star newStar();
		std::string title() { return "Heart-Shaped Space"; }
	};

	class Pattern08 : public Pattern
	{
		float time;
		float time2;
	public:
		Pattern08();
		void step(float speed, float speed2);
		Star newStar();
		std::string title() { return "Magic Dust"; }
	};

	class Pattern09 : public Pattern
	{
		float time;
		float time2;
		bool reverse;
	public:
		Pattern09();
		void step(float speed, float speed2);
		Star newStar();
		std::string title() { return "Strands of Light"; }
	};

	class Pattern10 : public Pattern
	{
		float time;
		float time2;
	public:
		Pattern10();
		void step(float speed, float speed2);
		Star newStar();
		std::string title() { return "Circle Trail"; }
	};

	/**
	 * Adds the default set of Starfield patterns.
	 *
	 * @param starfield the Starfield instance to add the patterns to.
	 */
	extern void addPatterns(Starfield& starfield);
}}

#endif // STARLIB_PATTERNS_H
