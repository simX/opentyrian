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
#ifndef STARLIB_H
#define STARLIB_H
#include "opentyr.h"

#include "util.h"

#include "boost/array.hpp"
#include "boost/function.hpp"
#include <vector>
#include <list>
#include <string>

namespace starlib
{

/**
 * Represents a single star in the animation.
 */
struct Star
{
	int x, y;

	/** Depth. @note The Star is recycled when this reaches 0. */
	unsigned int z;
};

/**
 * Pattern used by Starfield to determine the position of new \link Star Stars\endlink.
 */
class Pattern
{
public:
	virtual ~Pattern() {};

	/** Called every frame.
	 *
	 * @param speed Current animation speed multiplier. Use for time based effects.
	 */
	virtual void step(float speed) {}
	/** @see step(float) */
	virtual void step(float speed, float speed2) { step(speed); }

	/** Called to create a new Star. The \c z component is ignored. */
	virtual Star newStar() = 0;

	/** Called to get the name of the current pattern. This name is displayed on the screen when the pattern changes. */
	virtual std::string title() = 0;
};

/**
 * Draws the colored animated starfield seen in the jukebox.
 */
class Starfield
{
public:
	Starfield();

	/** Draws and updates the animation. */
	void draw();

	/** Handles keyboard input. */
	void handle_input();

	/**
	 * Adds a Pattern to the list of available patterns.
	 *
	 * @param factory the Pattern factory function that will return a new instance of the class. Can be generated using patterns::makeFactory.
	 */
	void addPattern(boost::function<Pattern*()> factory);

private:
	static const unsigned int NUM_STARS = 512;

	boost::array<Star, NUM_STARS> stars;
	unsigned int movementSpeed;
	float speed;
	float speed2;
	Uint8 color;

	void resetValues();
	static void drawStar(int x, int y, Uint8 color, Uint8 *surface);
	Star newStar();

	//////////

	typedef std::list<boost::function<Pattern*()> > PatternListType;
	typedef CircularIter<PatternListType::iterator, PatternListType> IterType;

	void changePattern(const IterType& iter);
	void nextPattern();
	void prevPattern();

	Pattern *pattern;
	PatternListType patternList;
	IterType patternIter;

	//////////

	static const unsigned int MESSAGE_TIME = 150;

	unsigned int messageDisplayTime;
	std::string message;
	std::string messageTag;

	void displayMessage(const std::string& message, const std::string& tag);
	static int fadeColors(unsigned int current, unsigned int max, int color1, int color2, int color3);

	//////////

	std::vector<std::string> helpText;
	bool showHelp;

	void loadHelp();
	void drawHelp();
};

}

#endif /* STARLIB_H */
