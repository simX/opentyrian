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
#include "Filesystem.h"

#include "boost/format.hpp"
#include <fstream>

namespace starlib
{

Starfield::Starfield()
	: pattern(0), patternIter(patternList), messageDisplayTime(0), showHelp(false)
{
	loadHelp();
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

	if (!helpText.empty())
	{
		if (showHelp)
			drawHelp();
		else
			JE_outText(4, 4, "F1: Help", 1, 0);
	}

	if (messageDisplayTime > 0)
	{
		JE_outText(JE_fontCenter(message, TINY_FONT), 4, message, fadeColors(messageDisplayTime, MESSAGE_TIME, 2, 8, 11), 4);

		--messageDisplayTime;
	}

	pattern->step(speed, speed2);
}

void Starfield::handle_input()
{
	static boost::format speed_format("Speed %1%: %|2$.2f|");

	if (newkey)
	{
		switch (lastkey_sym)
		{
		case SDLK_PAGEDOWN:
			nextPattern();
			displayMessage(pattern->title(), "patternName");
			break;
		case SDLK_PAGEUP:
			prevPattern();
			displayMessage(pattern->title(), "patternName");
			break;
		case SDLK_END:
			speed += 0.05f;
			displayMessage((speed_format % 1 % speed).str(), "speed1");
			break;
		case SDLK_DELETE:
			speed -= 0.05f;
			displayMessage((speed_format % 1 % speed).str(), "speed1");
			break;
		case SDLK_HOME:
			speed2 += 0.05f;
			displayMessage((speed_format % 2 % speed2).str(), "speed2");
			break;
		case SDLK_INSERT:
			speed2 -= 0.05f;
			displayMessage((speed_format % 2 % speed2).str(), "speed2");
			break;
		case SDLK_F1:
			showHelp = !showHelp;
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

void Starfield::displayMessage(const std::string& message, const std::string& tag)
{
	this->message = message;

	// Fade in if message is hidden or tag is different.
	bool fade_in = messageDisplayTime == 0 || tag != messageTag;
	unsigned int dispTime = MESSAGE_TIME - (fade_in ? 0 : 6);

	if (messageDisplayTime < dispTime)
		messageDisplayTime = MESSAGE_TIME - (fade_in ? 0 : 6);

	messageTag = tag;
}

int Starfield::fadeColors(unsigned int current, unsigned int max, int color1, int color2, int color3)
{
	if (current < 6 || current > max-3)
		return color3;
	else if (current < 12 || current > max-6)
		return color2;
	else
		return color1;
}

void Starfield::loadHelp()
{
	static const char *HELP_FILE = "jukebox_help.txt";

	if (!helpText.empty())
		return;

	std::fstream f;
	try
	{
		Filesystem::get().openDatafile(f, HELP_FILE);
	}
	catch (Filesystem::FileOpenErrorException&)
	{
		Console::get() << "\a7Error:\ax Failed to open file " << HELP_FILE << ". Jukebox help won't be available." << std::endl;
		return;
	}

	std::string line;
	while(getline(f, line))
	{
		helpText.push_back(line);
	}
}

void Starfield::drawHelp()
{
	unsigned int y = 4;

	foreach (const std::string& line, helpText)
	{
		JE_outText(4, y, line, 1, 4);
		y += 8;
	}
}

}
