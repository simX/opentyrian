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

#include "Singleton.h"

#include "SDL.h"
#include <vector>
#include <deque>
#include <iostream>
#include <string>

class Console : public std::ostream, public Singleton<Console>
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

public:
	static const unsigned int LINE_HEIGHT = 8;
private:
	static const unsigned int CELL_WIDTH = 6;
	static const unsigned int BLINK_RATE = 5;
	static const unsigned int TEXT_COLOR = 0xE;

	bool mDown;
	unsigned int mHeight;
	std::vector<std::string> mScrollback;
	unsigned int mScrollbackHead;
	unsigned int mCurScroll;
	unsigned int mColor;

	std::string mEditLine;
	unsigned int mCursorPos;
	bool mCursorVisible;
	unsigned int mCursorTimeout;

	ConsoleStreamBuffer mStreambuf;

	void drawText( Uint8* const surf, unsigned int x, unsigned int y, const std::string& text );
	void drawArrow( Uint8* const surf, unsigned int x, unsigned int y, Uint8 col );

	Console( const Console& );
	Console& operator=( const Console& );

public:
	Console();
	void enable( const bool anim = true );
	void disable( const bool anim = true );
	void draw( Uint8* const surf );
	void think( const SDL_keysym& keysym );
	void println( const std::string& text );
	static std::vector<std::string> parseLine( std::string text );
	void runCommand( std::vector<std::string> tokens );
	void runCommand( const std::string& str ) { runCommand(parseLine(str)); }
	void runScript( const std::string& fname, std::ifstream& fstream );

	bool isDown( );

	// TODO: When we have a real main loop, remove this
	void consoleMain( void );
};

#ifdef NDEBUG
#	define DEBUG_MSG(str)
#else
#	define DEBUG_MSG(str) {Console::get() << "\a4" << str << std::endl;}
#endif

/*
0: Gray
1: Light Brown
2: Dark Green
3: Dark Blue (?)
4: Pinky Red
5: Desaturated Purple
6: Grayish Blue
7: Bright Orange
8: Sky Blue
9: Bright Blue
A: Light Brown
B: Light Brown
C: Green
D: Light Blue
E: Lighter Brown
F: Brown
*/

#endif // CONSOLE_H
