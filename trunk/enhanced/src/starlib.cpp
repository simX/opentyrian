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
#include "starlib.h"

#include "starlib_patterns.h"
#include "mtrand.h"
#include "video.h"
#include "fonthand.h"
#include "newshape.h"
#include "keyboard.h"

namespace starlib
{

Starfield::Starfield()
	: pattern(0), patternIter(patternList), displayPatternTime(0)
{
	patterns::addPatterns(*this);
	changePattern(IterType(patternList.begin(), patternList));

	resetValues();

	for (unsigned int i = 0; i < stars.size(); ++i)
	{
		stars[i] = pattern->newStar();
		stars[i].z = i * stars.size() / 500;
	}
}

void Starfield::draw()
{
	clear_screen(0);

	foreach (Star& star, stars)
	{
		if (star.z <= movementSpeed)
		{
			star = newStar();
		}
		else
		{
			const int scrX = star.x / static_cast<signed>(star.z) + scr_width / 2;
			const int scrY = star.y / static_cast<signed>(star.z) + scr_height / 2;
			const Uint8 drawColor = color + ((star.z / 16) & 0x1F);

			drawStar(scrX, scrY, drawColor, VGAScreen);

			star.z -= movementSpeed;
		}
	}

	if (displayPatternTime > 0)
	{
		// Do the nice fade-in and fade-outs
		// TODO: Maybe split out color calculation to separate function?
		int color;
		if (displayPatternTime < 6 || displayPatternTime > MESSAGE_TIME-3)
			color = 11;
		else if (displayPatternTime < 12 || displayPatternTime > MESSAGE_TIME-6)
			color = 8;
		else
			color = 2;

		JE_outText(JE_fontCenter(pattern->title(), TINY_FONT), 4, pattern->title(), color, 4);

		--displayPatternTime;
	}

	pattern->step(speed, speed2);
}

void Starfield::handle_input()
{
	if (newkey)
	{
		switch (lastkey_sym)
		{
		case SDLK_PAGEDOWN:
			nextPattern();
			displayPatternTime = MESSAGE_TIME;
			break;
		case SDLK_PAGEUP:
			prevPattern();
			displayPatternTime = MESSAGE_TIME;
			break;
		}
	}
}

void Starfield::resetValues()
{
	movementSpeed = 2;
	speed = 1.f;
	speed2 = 1.f;
	color = 0x20;
	changePattern(patternIter);
}

void Starfield::drawStar(int x, int y, Uint8 color, Uint8 *surface)
{
	if (x >= 2 && y >= 2 && x <= scr_width-3 && y <= scr_height-3)
	{
		surface[xy2off(x,y)] = color;

		color += 72;
		surface[xy2off(x-1,y)] = color;
		surface[xy2off(x+1,y)] = color;
		surface[xy2off(x,y-1)] = color;
		surface[xy2off(x,y+1)] = color;

		color += 72;
		surface[xy2off(x-2,y)] = color;
		surface[xy2off(x+2,y)] = color;
		surface[xy2off(x,y-2)] = color;
		surface[xy2off(x,y+2)] = color;
	}
}

Star Starfield::newStar()
{
	Star star = pattern->newStar();
	star.z = 500;

	return star;
}

////
// Pattern functions
////

void Starfield::changePattern(const IterType& iter)
{
	if (pattern)
		delete pattern;

	patternIter = iter;
	pattern = (*iter)();
}

void Starfield::nextPattern()
{
	++patternIter;
	changePattern(patternIter);
}

void Starfield::prevPattern()
{
	--patternIter;
	changePattern(patternIter);
}

void Starfield::addPattern(boost::function<Pattern*()> factory)
{
	if (factory)
		patternList.push_back(factory);
}

}