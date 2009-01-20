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
#ifndef STARLIB_H
#define STARLIB_H
#include "opentyr.h"

#include "util.h"

#include "boost/array.hpp"
#include "boost/function.hpp"
#include <vector>
#include <list>

namespace starlib
{

struct Star
{
	int x, y;
	unsigned int z;
};

class Pattern
{
public:
	virtual ~Pattern() {};
	virtual void step() {};
	virtual Star newStar() = 0;
};

/**
 * Draws the colored animated starfield seen in the jukebox.
 */
class Starfield
{
public:
	Starfield();
	void draw();

	void addPattern(boost::function<Pattern*()> factory);

private:
	static const unsigned int NUM_STARS = 512;
	boost::array<Star, NUM_STARS> stars;

	unsigned int movementSpeed;
	Uint8 color;

	typedef std::list<boost::function<Pattern*()>> PatternListType;
	typedef CircularIter<PatternListType::iterator, PatternListType> IterType;

	Pattern *pattern;
	PatternListType patternList;
	IterType patternIter;

	void resetValues();
	static void drawStar(int x, int y, Uint8 color, Uint8 *surface);
	Star newStar();

	void changePattern(const IterType& iter);
	void nextPattern();
	void prevPattern();
	void addDefaultPatterns();
};

}

#endif /* STARLIB_H */
