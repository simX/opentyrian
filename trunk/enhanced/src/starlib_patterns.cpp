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
#include "starlib_patterns.h"

#include "mtrand.h"

#include <cmath>

namespace starlib { namespace patterns
{
	////
	// Pattern00
	////
	Star Pattern00::newStar()
	{
		Star star = {
			mt::rand() % 64000 - 32000,
			mt::rand() % 40000 - 20000
		};

		return star;
	}

	////
	// Pattern01
	////
	Pattern01::Pattern01()
		: time(0.f)
	{}

	Star Pattern01::newStar()
	{
		Star star = {
			sin(time / 30.f) * 20000,
			mt::rand() % 40000 - 20000
		};

		return star;
	}

	void Pattern01::step(float speed)
	{
		time += speed;
	}

	////
	// Pattern02
	////
	Pattern02::Pattern02()
		: time(0.f)
	{}

	Star Pattern02::newStar()
	{
		Star star = {
			cos(time) * 20000.f,
			sin(time) * 20000.f
		};

		return star;
	}

	void Pattern02::step(float speed)
	{
		time += speed * 0.2f;
	}

	////
	// Pattern03
	////
	Pattern03::Pattern03()
		: time(0.f)
	{}

	Star Pattern03::newStar()
	{
		Star star = {
			(cos(time * 15.f) * 100.f) * (int(time / 6.f) % 200),
			(sin(time * 15.f) * 100.f) * (int(time / 6.f) % 200)
		};

		return star;
	}

	void Pattern03::step(float speed)
	{
		time += speed;
	}

	////
	// Pattern04
	////
	Pattern04::Pattern04()
		: time(0.f)
	{}

	Star Pattern04::newStar()
	{
		Star star = {
			sin(time) * cos(time * 1.1f) * 20000.f,
			cos(time) * int(sin(time / 200.f) * 300.f) * 100
		};

		return star;
	}

	void Pattern04::step(float speed)
	{
		time += speed * 0.1f;
	}

	////
	// Pattern05
	////
	Pattern05::Pattern05()
		: time(0.f)
	{}

	Star Pattern05::newStar()
	{
		Star star = {
			sin(time / 2.f) * 20000.f,
			cos(time) * int(sin(time / 200.f) * 300.f) * 100
		};

		return star;
	}

	void Pattern05::step(float speed)
	{
		time += speed * 0.1f;
	}

	////
	// Pattern06
	////
	Pattern06::Pattern06()
		: time(0.f)
	{}

	Star Pattern06::newStar()
	{
		Star star = {
			(mt::rand() % 65535) - 32768,
			(mt::rand() % 2 == 0 ?
				cos(time / 80.f) * 10000.f + 15000.f :
				50000.f - cos(time / 80.f) * 13000.f
			) - 32768.f
		};

		return star;
	}

	void Pattern06::step(float speed)
	{
		time += speed * 0.5f;
	}

	////
	// Pattern07
	////
	Pattern07::Pattern07()
		: time(0.f), time2(0), reverse(false)
	{}

	Star Pattern07::newStar()
	{
		Star star = {
			cos(sin(time2 / 5.f) + (time / 100.f)) * 32000.f,
			sin(cos(time2 / 5.f) + (time / 100.f)) * 30000.f
		};

		return star;
	}

	void Pattern07::step(float speed, float speed2)
	{
		time += speed * 0.5f;
		time2 += speed2 * (reverse ? -1.f : 1.f);

		if (time2 >= 500 || time2 <= 0)
			reverse = !reverse;
	}

	////
	// Pattern08
	////
	Pattern08::Pattern08()
		: time(0.f), time2(1)
	{}

	Star Pattern08::newStar()
	{
		Star star = {
			cos(sin(time / 1000.f) + (time / 2.1f)) * 32000.f * cos(time2 / 201.f),
			sin(cos(time / 980.f) + (time / 2.f)) * 30000.f * sin(time2 / 183.f)
		};

		return star;
	}

	void Pattern08::step(float speed, float speed2)
	{
		time += speed * 0.1f;
		time2 += speed2;
	}

	////
	// Pattern09
	////
	Pattern09::Pattern09()
		: time(0.f), time2(0), reverse(false)
	{}

	Star Pattern09::newStar()
	{
		Star star = {
			cos(sin(time2 / 2.f) / (sqrt(ot_abs(time)) / 10.f + 1.f) + (time2 / 100.f)) * 32000.f,
			sin(cos(time2 / 2.f) / (sqrt(ot_abs(time)) / 10.f + 1.f) + (time2 / 100.f)) * 30000.f
		};

		return star;
	}

	void Pattern09::step(float speed, float speed2)
	{
		time += speed * 0.2f;
		time2 += speed2 * (reverse ? -1.5f : 1.5f);

		if (time2 >= 2500 || time2 <= 0)
			reverse = !reverse;
	}

	////
	// Pattern10
	////
	Pattern10::Pattern10()
		: time(0.f), time2(0)
	{}

	Star Pattern10::newStar()
	{
		Star star = {
			(sin(time) + cos(time2 / 1000.f) * 3.f) * 12000.f,
			cos(time) * 10000.f + sin(time2 / 1123.f) * 10000.f
		};

		return star;
	}

	void Pattern10::step(float speed, float speed2)
	{
		time += speed * 0.1f;
		time2 += speed2 * 5.f;
	}


	void addPatterns(Starfield& starfield)
	{
		starfield.addPattern(factory<Pattern00>);
		starfield.addPattern(factory<Pattern01>);
		starfield.addPattern(factory<Pattern02>);
		starfield.addPattern(factory<Pattern03>);
		starfield.addPattern(factory<Pattern04>);
		starfield.addPattern(factory<Pattern05>);
		starfield.addPattern(factory<Pattern06>);
		starfield.addPattern(factory<Pattern07>);
		starfield.addPattern(factory<Pattern08>);
		starfield.addPattern(factory<Pattern09>);
		starfield.addPattern(factory<Pattern10>);
	}
}}
