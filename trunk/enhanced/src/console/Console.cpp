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
#include "Console.h"

#include "fonthand.h"
#include "newshape.h"
#include "keyboard.h"
#include "video.h"
#include "vga256d.h"
#include "CCmd.h"
#include "cvar/CVar.h"
#include "Filesystem.h"

#include "SDL.h"
#include <cassert>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>
#include <cctype>
#include <deque>
#include <fstream>
#include "boost/filesystem.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

namespace CVars
{
	CVarInt con_buffer_size("con_buffer_size", CVar::CONFIG, "Size of the console scrollback in lines.", 64);
	CVarInt con_height("con_height", CVar::CONFIG | CVar::CONFIG_AUTO, "Height of the console, in lines.", 10, rangeBind(4u, 200/Console::LINE_HEIGHT));
}

Console::ConsoleStreamBuffer::ConsoleStreamBuffer(Console& parentConsole)
	: parentConsole(parentConsole)
{ }

int Console::ConsoleStreamBuffer::overflow( int c )
{
	if (c != mTraits::eof()) {
		if (c == '\n') {
			mOutputStr.append("\ax");
			parentConsole.println(mOutputStr);
			mOutputStr.clear();
		} else if (c == '\t') {
			mOutputStr.append(8, ' ');
		} else {
			mOutputStr.append(1, c);
		}
	}
	return c;
}

void Console::drawText( Uint8* const surf, unsigned int x, unsigned int y, const std::string& text )
{
	for (unsigned int i = 0; i < text.length(); i++)
	{
		const unsigned char c = text[i];

		if ((c > 32) && (c < 169) && (fontMap[c-33] != 255) && (shapeArray[TINY_FONT][fontMap[c-33]] != NULL))
		{
			const int xoff = CELL_WIDTH/2 - shapeX[TINY_FONT][fontMap[c-33]]/2;
			JE_newDrawCShapeBright(shapeArray[TINY_FONT][fontMap[c-33]], shapeX[TINY_FONT][fontMap[c-33]], shapeY[TINY_FONT][fontMap[c-33]], x+xoff, y, mColor, 4);
			x += CELL_WIDTH;
		} else {
			if (c == '\t') {
				x += CELL_WIDTH*8;
			} else if (c == '\a') {
				i++;
				const unsigned char c = text[i];
				if (c >= '0' && c <= '9')
				{
					mColor = c-'0';
				} else if (c >= 'a' && c <= 'f') {
					mColor = c-'a'+10;
				} else if (c == 'x') {
					mColor = TEXT_COLOR;
				}
			} else {
				x += CELL_WIDTH;
			}
		}
	}
}

void Console::drawArrow( Uint8* const surf, unsigned int x, unsigned int y, Uint8 col )
{
	JE_pix(x, y, col);
	for (int i = 1; i < 3; i++)
	{
		JE_pix(x-i, y-i, col);
		JE_pix(x+i, y-i, col);
	}
}

Console::Console()
	: std::ostream(&mStreambuf), mDown(false), mHeight(0), mScrollback(CVars::con_buffer_size),
	mScrollbackHead(0), mCurScroll(0), mColor(TEXT_COLOR), mCursorPos(0), mCursorVisible(true), mCursorTimeout(BLINK_RATE),
	mStreambuf(*this)
{
	const std::string fname = Filesystem::getHomeDir()+"log.txt";
	logStream.open(fname.c_str(), std::ios::app);

	if (logStream.fail())
		*this << "\a7Warning:\ax Failed to open log file: " << fname << std::endl;
	else
	{
		namespace time = boost::posix_time;
		const time::ptime now = time::second_clock::local_time();
		logStream << "* Log opened on " << time::to_simple_string(now) << std::endl;
	}
}

void Console::enable( const bool anim )
{
	mDown = true;
	
	if (!anim)
	{
		mHeight = CVars::con_height;
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

void Console::draw( Uint8* const surf )
{
	if (mHeight == 0) return;

	JE_barShade(0, 0, 319, mHeight*LINE_HEIGHT-1);
	JE_barShade(0, 0, 319, mHeight*LINE_HEIGHT-1);

	// Draws text
	int head = mScrollbackHead - mCurScroll - mHeight + 2;
	if (head < 0) head = mScrollback.size() + head;

	std::vector<std::string>::const_iterator iter = mScrollback.begin() + head;
	for (unsigned int i = 0; i < mHeight-1; ++i)
	{
		if (iter == mScrollback.end()) iter = mScrollback.begin();
		drawText(surf, 8, i*LINE_HEIGHT, *iter);
		++iter;
	}

	// Draw "more text" arrows
	if (mCurScroll > 0)
	{
		const int base = (mHeight-1)*LINE_HEIGHT-1;
		for (int i = 1; i <= 9; i += 3)
		{
			drawArrow(surf, 3, base-i, 0x0f);
		}
	}

	// Draw input line
	drawText(surf, 8, (mHeight-1)*LINE_HEIGHT, mEditLine);
	if (mCursorVisible)
	{
		JE_bar(mCursorPos*CELL_WIDTH+8, (mHeight-1)*LINE_HEIGHT+1, mCursorPos*CELL_WIDTH+8, mHeight*LINE_HEIGHT-2, 0x0f);
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
			///// Console Resizing
			case SDLK_EQUALS:
				if (!(keysym.mod & KMOD_CTRL)) goto type_char;

				CVars::con_height = CVars::con_height+1;

				if (mCurScroll + static_cast<unsigned int>(CVars::con_height) >= static_cast<unsigned int>(CVars::con_buffer_size))
				{
					mCurScroll = CVars::con_buffer_size - CVars::con_height;
				}
				break;
			case SDLK_MINUS:
				if (!(keysym.mod & KMOD_CTRL)) goto type_char;
				CVars::con_height = CVars::con_height-1;
				break;
			///// Console scrolling
			case SDLK_PAGEUP:
				{
				unsigned int amount = CVars::con_height / 3;
				if (amount < 1) amount = 1;

				if (mCurScroll + amount + static_cast<unsigned int>(CVars::con_height)-1 >= static_cast<unsigned int>(CVars::con_buffer_size))
				{
					mCurScroll = CVars::con_buffer_size - (CVars::con_height-1);
				} else {
					mCurScroll += amount;
				}
				break;
				}
			case SDLK_PAGEDOWN:
				{
				unsigned int amount = CVars::con_height / 3;
				if (amount < 1) amount = 1;

				if (amount < mCurScroll)
				{
					mCurScroll -= amount;
				} else {
					mCurScroll = 0;
				}
				break;
				}
			///// Line editing
			case SDLK_END:
				if (lastkey_mod & KMOD_SHIFT)
				{
					mCurScroll = 0;
				} else {
					mCursorPos = mEditLine.length();
				}
				break;
			case SDLK_HOME:
				mCursorPos = 0;
				break;
			case SDLK_LEFT:
				if (mCursorPos > 0)
				{
					mCursorPos--;
				}
				break;
			case SDLK_RIGHT:
				if (mCursorPos < mEditLine.length())
				{
					mCursorPos++;
				}
				break;
			case SDLK_BACKSPACE:
				if (mCursorPos > 0) // Can't backspace at the start of the line!
				{
					mEditLine.erase(mCursorPos-1, 1);
					mCursorPos--;
				}
				break;
			case SDLK_DELETE:
				if (mCursorPos < mEditLine.length()) // Can't delete at the end of the line!
				{
					mEditLine.erase(mCursorPos, 1);
				}
				break;
			case SDLK_RETURN:
				*this << "\a3" << mEditLine << std::endl;
				runCommand(mEditLine);
				mEditLine.clear();
				mCursorPos = 0;
				break;
			default:
type_char:
				if (std::isprint(lastkey_char))
				{
					mEditLine.insert(mCursorPos, 1, lastkey_char);
					mCursorPos++;
				}
			}
		}

		if (mHeight < static_cast<unsigned long>(CVars::con_height)) mHeight++;
		if (mHeight > static_cast<unsigned long>(CVars::con_height)) mHeight--;

		if (mCursorTimeout > 0)
		{
			mCursorTimeout--;
		} else {
			mCursorVisible = !mCursorVisible;
			mCursorTimeout = BLINK_RATE;
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

void Console::println( const std::string& text )
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
			if (logStream.good())
				logStream << text[i];

			if (line_len == 52)
			{
				wrap.push_back(i);
				line_len = 0;
			}
			line_len++;
		}
	}
	std::cout << '\n';
	if (logStream.good())
		logStream << std::endl;

	wrap.push_back(text.length());

	for (unsigned int i = 1; i < wrap.size(); i++)
	{
		mScrollbackHead++;
		if (mScrollbackHead == mScrollback.size()) mScrollbackHead = 0;

		mScrollback[mScrollbackHead] = std::string(text, wrap[i-1], wrap[i]-wrap[i-1]);
	}
}

std::vector<std::string> Console::parseLine( std::string text )
{
	text.append(1, ' ');

	std::vector<std::string> tokens;
	bool in_quote = false;
	bool escaped = false;
	std::string cur_token;

	for (std::string::const_iterator i = text.begin(); i != text.end(); i++)
	{
		switch (*i)
		{
		case ' ':
			if (in_quote)
			{
				cur_token.append(1, *i);
			} else {
				if (!cur_token.empty())
				{
					tokens.push_back(cur_token);
					cur_token.clear();
				}
			}
			break;
		case '"':
			if (!escaped)
			{
				in_quote = !in_quote;
			} else {
				cur_token.append(1, '"');
				escaped = false;
			}
			break;
		case '\\':
			if (!escaped)
			{
				escaped = true;
			} else {
				cur_token.append(1, '\\');
				escaped = false;
			}
			break;
		case '\r':
		case '\n':
			break;
		default:
			cur_token.append(1, *i);
			break;
		}
	}

	if (in_quote)
	{
		Console::get() << "\a7Warning:\ax No quote at end of input!" << std::endl;
	}

	return tokens;
}

void Console::runCommand( std::vector<std::string> tokens )
{
	if (tokens.empty()) return;

	const std::string command_name = tokens[0];
	tokens.erase(tokens.begin());

	if (boost::trim_copy(command_name).size() == 0)
		return;

	CCmd* const command = CCmdManager::get().getCCmd(command_name);
	if (command)
	{
		try
		{
			(*command)(tokens);
		} catch(CCmd::RuntimeCCmdError& e) {
			*this << "\a7Warning:\ax " << e.what() << std::endl;
		}
	} else {
		*this << "\a7Warning:\ax Unknown CCmd: " << command_name << std::endl;
	}
}

void Console::consoleMain()
{
	Uint8* const prev_VGAScreen = VGAScreen;
	Uint8* const prev_tempScreenSeg = tempScreenSeg;
	VGAScreen = VGAScreenSeg;
	tempScreenSeg = VGAScreenSeg;

	Uint8 screen_copy[320*200];
	Uint8 *screen = VGAScreen;
	std::copy(screen, screen+scr_height*scr_width, screen_copy);

	service_SDL_events(true);
	while (!(lastkey_keysym.sym == SDLK_DELETE && lastkey_keysym.mod & KMOD_CTRL))
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

namespace CCmds
{
	namespace Func
	{
		static void exec( const std::vector<std::string>& params )
		{
			namespace fs = boost::filesystem;

			std::string param1 = CCmd::convertParam<std::string>(params, 0);

			std::string path;
			try
			{
				path = Filesystem::get().findFile(param1);
			}
			catch (Filesystem::FileOpenErrorException&)
			{
				throw CCmd::RuntimeCCmdError("Script file not found.");
			}

			std::ifstream stream(path.c_str(), std::ios_base::in);
			Console::get().runScript(path, stream);
		}
	}

	CCmd exec("exec", CCmd::NONE, "Executes a script file. Usage: exec [filename]", Func::exec);
}

void Console::runScript( const std::string& fname, std::ifstream& fstream )
{
	*this << "\acExecuting script " << fname << "..." << std::endl;

	while (!fstream.eof())
	{
		std::string line;
		std::getline(fstream, line);

		this->runCommand(line);
	}
}
