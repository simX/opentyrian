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
#include "Console.h"

#include "SDL.h"
#include "fonthand.h"
#include "newshape.h"
#include "keyboard.h"
#include "vga256d.h"
#include <cassert>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>

/////
// Singleton stuff
/////

Console* Console::sInstance = 0;

void Console::initialize()
{
	assert(sInstance == 0);
	sInstance = new Console();
}

Console& Console::get()
{
	assert(sInstance != 0);
	return *sInstance;
}

void Console::deinitialize()
{
	assert(sInstance != 0);
	delete sInstance;
}

///// End singleton stuff

int Console::ConsoleStreamBuffer::overflow( int c )
{
	if (c != mTraits::eof()) {
		if (c == '\n') {
			mOutputStr.append("\ae");
			Console::get().println(mOutputStr);
			mOutputStr.clear();
		} else if (c == '\t') {
			mOutputStr.append(8, ' ');
		} else {
			mOutputStr.append(1, c);
		}
	}
	return c;
}

void Console::drawText( SDL_Surface* const surf, unsigned int x, unsigned int y, std::string text )
{
	int color = 14;

	for (unsigned int i = 0; i < text.length(); i++)
	{
		const unsigned char c = text[i];

		if ((c > 32) && (c < 169) && (fontMap[c-33] != 255) && ((*shapeArray)[TINY_FONT][fontMap[c-33]] != NULL))
		{
			const int xoff = CELL_WIDTH/2 - shapeX[TINY_FONT][fontMap[c-33]]/2;
			JE_newDrawCShapeBright((*shapeArray)[TINY_FONT][fontMap[c-33]], shapeX[TINY_FONT][fontMap[c-33]], shapeY[TINY_FONT][fontMap[c-33]], x+xoff, y, mColor, 4);
			x += CELL_WIDTH;;
		} else {
			if (c == ' ')
			{
				x += CELL_WIDTH;
			} else if (c == '\t') {
				x += CELL_WIDTH*8;
			} else if (c == '\a') {
				i++;
				const unsigned char c = text[i];
				if (c >= '0' && c <= '9')
				{
					mColor = c-'0';
				} else if (c >= 'a' && c <= 'f') {
					mColor = c-'a'+10;
				}
			}
		}
	}
}

Console::Console()
	: mDown(false), mHeight(0), mConsoleHeight(10), mScrollback(4096), // TODO: Replace constant value with cvar
	mScrollbackHead(0), mCurScroll(0), mColor(14), std::ostream(&mStreambuf)
{
	if (mConsoleHeight * LINE_HEIGHT > 200)
	{
		mConsoleHeight = 200 / LINE_HEIGHT;
	}
}

void Console::enable( const bool anim )
{
	mDown = true;
	
	if (!anim)
	{
		mHeight = mConsoleHeight;
	}
}

void Console::disable( const bool anim )
{
	mDown = false;

	if (!anim)
	{
		mHeight = 0;
	}
}

void Console::draw( SDL_Surface* const surf )
{
	if (mHeight == 0) return;

	JE_barShade(0, 0, 319, mHeight*LINE_HEIGHT);
	JE_barShade(0, 0, 319, mHeight*LINE_HEIGHT);

	int head = mScrollbackHead - mCurScroll;
	if (head < 0) head = mScrollback.size()-head;

	std::vector<std::string>::const_iterator iter = mScrollback.begin() + head;
	for (int i = mHeight-1; i >= 0; i--)
	{
		drawText(surf, 8, i*LINE_HEIGHT, *iter);
		if (iter == mScrollback.begin()) iter = mScrollback.end();
		--iter;
	}
}

void Console::think( const SDL_keysym& keysym )
{
	if (mDown)
	{
		if (newkey)
		{
			switch (keysym.sym)
			{
			case SDLK_BACKQUOTE:
				disable();
				break;
			case SDLK_a:
				*this << "Test Text" << std::endl;
				break;
			case SDLK_s:
				*this << "\tTest Indent" << std::endl;
				break;
			case SDLK_d:
				*this << "Separate" << " " << "words" << std::endl;
				break;
			case SDLK_f:
				*this << "Number " << 15 << std::endl;
				break;
			case SDLK_g:
				*this << "\a00\a11\a22\a33\a44\a55\a66\a77\a88\a99\aaA\abB\acC\adD\aeE\afF" << std::endl;
				break;
			default:
				break;
			}
		}

		if (mHeight < mConsoleHeight)
		{
			mHeight++;
		}
	} else {
		if (newkey)
		{
			switch (keysym.sym)
			{
			case SDLK_BACKQUOTE:
				enable();
				break;
			default:
				break;
			}
		}

		if (mHeight > 0)
		{
			mHeight--;
		}
	}
}

bool Console::isDown( )
{
	return mDown;
}

void Console::println( std::string text )
{
	// Strips the control codes and calculates wrapping.
	std::vector<unsigned int> wrap;
	wrap.push_back(0);
	unsigned int line_len = 0;
	for (unsigned int i = 0; i < text.length(); i++)
	{
		if (text[i] == '\a')
		{
			i++; // Skip next char
		} else {
			std::cout << text[i];
			if (line_len == 52)
			{
				wrap.push_back(i);
				line_len = 0;
			}
			line_len++;
		}
	}
	std::cout << '\n';

	wrap.push_back(text.length());

	for (unsigned int i = 1; i < wrap.size(); i++)
	{
		mScrollbackHead++;
		if (mScrollbackHead == mScrollback.size()) mScrollbackHead = 0;

		mScrollback[mScrollbackHead] = std::string(text, wrap[i-1], wrap[i]-wrap[i-1]);
	}
}

void Console::consoleMain()
{
	SDL_Surface* const prev_VGAScreen = VGAScreen;
	SDL_Surface* const prev_tempScreenSeg = tempScreenSeg;
	VGAScreen = VGAScreenSeg;
	tempScreenSeg = VGAScreenSeg;

	Uint8 screen_copy[320*200];
	Uint8 *screen = static_cast<Uint8*>(VGAScreen->pixels);
	std::copy(screen, screen+VGAScreen->h*VGAScreen->pitch, screen_copy);

	service_SDL_events(true);
	while (lastkey_keysym.sym != SDLK_DELETE)
	{
		service_SDL_events(true);
		think(lastkey_keysym);
		draw(VGAScreen);

		SDL_Delay(1000/30);
		JE_showVGA();

		std::copy(screen_copy, screen_copy+320*200, screen);
	}

	tempScreenSeg = prev_tempScreenSeg;
	VGAScreen = prev_VGAScreen;
}
