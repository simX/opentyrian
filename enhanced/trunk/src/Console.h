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
#ifndef CONSOLE_H
#define CONSOLE_H

#include "SDL.h"
#include <vector>
#include <deque>
#include <iostream>

class Console : public std::ostream
{
private:
	class ConsoleStreamBuffer : public std::streambuf
	{
	private:
		std::string mOutputStr;
		typedef std::char_traits<char> mTraits;
	protected:
		virtual int overflow(int c = mTraits::eof());
	};

	static const unsigned int LINE_HEIGHT = 8;
	static const unsigned int CELL_WIDTH = 6;
	static const unsigned int BLINK_RATE = 5;
	static const unsigned int TEXT_COLOR = 0xE;
	static const unsigned int BUFFER_SIZE = 64; // TODO: Replace with CVAR

	bool mDown;
	unsigned int mHeight;
	unsigned int mConsoleHeight; // TODO: Replace with CVAR	
	std::vector<std::string> mScrollback;
	unsigned int mScrollbackHead;
	unsigned int mCurScroll;
	unsigned int mColor;

	std::string mEditLine;
	unsigned int mCursorPos;
	bool mCursorVisible;
	unsigned int mCursorTimeout;

	ConsoleStreamBuffer mStreambuf;

	void drawText( SDL_Surface* const surf, unsigned int x, unsigned int y, std::string text );
	void drawArrow( SDL_Surface* const surf, unsigned int x, unsigned int y, Uint8 col );

	// Singleton stuff
	static Console* sInstance;
public:
	Console();
	void enable( const bool anim = true );
	void disable( const bool anim = true );
	void draw( SDL_Surface* const surf );
	void think( const SDL_keysym& keysym );
	void println( std::string text );
	std::deque<std::string> parseLine( std::string text );
	void runCommand( std::deque<std::string> tokens );

	bool isDown( );

	// TODO: When we have a real main loop, remove this
	void consoleMain( void );

	// Singleton stuff
	static void initialize();
	static Console& get();
	static void deinitialize();
};

#endif // CONSOLE_H
