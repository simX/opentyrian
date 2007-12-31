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

class Console
{
private:
	static const int LINE_HEIGHT = 8;
	static const int CELL_WIDTH = 6;

	bool mDown;
	int mHeight;
	int mConsoleHeight; // TODO: Replace with CVAR	
	std::vector<std::string> mScrollback;
	int mScrollbackHead;
	int mCurScroll;

	void drawText( SDL_Surface* const surf, unsigned int x, unsigned int y, std::string text );

	// Singleton stuff
	static Console* sInstance;
public:
	Console();
	void enable( const bool anim = true );
	void disable( const bool anim = true );
	void draw( SDL_Surface* const surf );
	void think( const SDL_keysym& keysym );
	void println( std::string text );

	bool isDown( );

	// TODO: When we have a real main loop, remove this
	void consoleMain( void );

	// Singleton stuff
	static void initialize();
	static Console& getConsole();
	static void deinitialize();
};

#endif // CONSOLE_H
