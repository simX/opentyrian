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

#include "config.h"
#include "editship.h"
#include "episodes.h"
#include "error.h"
#include "fonthand.h"
#include "helptext.h"
#include "joystick.h"
#include "jukebox.h"
#include "keyboard.h"
#include "loudness.h"
#include "mainint.h"
#include "newshape.h"
#include "nortsong.h"
#include "nortvars.h"
#include "params.h"
#include "picload.h"
#include "scroller.h"
#include "setup.h"
#include "tyrian2.h"
#include "varz.h"
#include "vga256d.h"
#include "console/Console.h"
#include "console/KeyNames.h"
#include "video.h"
#include "video_scale.h"
#include "Filesystem.h"
#include "mtrand.h"
#include "network/Network.h"
#include "network/NetManager.h"
#include "network/NetCVars.h"

#include "SDL.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


const int shapereorderlist[7] = {1, 2, 5, 0, 3, 4, 6};

#ifdef NDEBUG
#define REL_STRING "Release"
#else
#define REL_STRING "Debug"
#endif

#include "svn_rev.h"

std::string get_opentyrian_version()
{
#ifdef HAVE_SVN_REV
	return std::string("r") + get_svn_rev_str() + " " REL_STRING;
#else
	return REL_STRING;
#endif
}
#undef REL_STRING

const char *opentyrian_menu_items[] =
{
	"About OpenTyrian",
	"Toggle Fullscreen",
	"Scaler: None",
	/* "Play Destruct", */
	"Jukebox",
	"Return to Main Menu"
};

/* zero-terminated strncpy */
char *strnztcpy( char *to, const char *from, size_t count )
{
	to[count] = '\0';
	return strncpy(to, from, count);
}

int ot_round( float x )
{
	return x >= 0 ? int(x + 0.5) : int(x - 0.5);
}

float ot_abs( float x )
{
	return x >= 0 ? x : -x;
}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
/* endian-swapping fread */
size_t efread( void *buffer, size_t size, size_t num, FILE *stream )
{
	size_t f = fread(buffer, size, num, stream);

	switch (size)
	{
		case 2:
			for (int i = 0; i < num; i++)
			{
				((Uint16 *)buffer)[i] = SDL_Swap16(((Uint16 *)buffer)[i]);
			}
			break;
		case 4:
			for (int i = 0; i < num; i++)
			{
				((Uint32 *)buffer)[i] = SDL_Swap32(((Uint32 *)buffer)[i]);
			}
			break;
		case 8:
			for (int i = 0; i < num; i++)
			{
				((Uint64 *)buffer)[i] = SDL_Swap64(((Uint64 *)buffer)[i]);
			}
			break;
		default:
			break;
	}

	return f;
}

/* endian-swapping fwrite */
size_t efwrite( void *buffer, size_t size, size_t num, FILE *stream )
{
	void *swap_buffer;
	size_t f;

	switch (size)
	{
		case 2:
			swap_buffer = malloc(size * num);
			for (int i = 0; i < num; i++)
			{
				((Uint16 *)swap_buffer)[i] = SDL_SwapLE16(((Uint16 *)buffer)[i]);
			}
			break;
		case 4:
			swap_buffer = malloc(size * num);
			for (int i = 0; i < num; i++)
			{
				((Uint32 *)swap_buffer)[i] = SDL_SwapLE32(((Uint32 *)buffer)[i]);
			}
			break;
		case 8:
			swap_buffer = malloc(size * num);
			for (int i = 0; i < num; i++)
			{
				((Uint64 *)swap_buffer)[i] = SDL_SwapLE64(((Uint64 *)buffer)[i]);
			}
			break;
		default:
			swap_buffer = buffer;
			break;
	}

	f = fwrite(swap_buffer, size, num, stream);

	if (swap_buffer != buffer)
	{
		free(swap_buffer);
	}

	return f;
}
#endif

void opentyrian_menu( void )
{
	int sel = 0;
	const int maxSel = COUNTOF(opentyrian_menu_items) - 1;
	bool quit = false, fade_in = true;

	int temp_scaler = CVars::r_scaler;

	JE_fadeBlack(10);
	JE_loadPic(13, false);

	JE_outTextAdjust(JE_fontCenter("OpenTyrian", FONT_SHAPES), 5, "OpenTyrian", 15, -3, FONT_SHAPES, false);

	memcpy(VGAScreen2, VGAScreen, scr_width * scr_height);

	JE_showVGA();

	if (currentJukeboxSong == 0) currentJukeboxSong = 37; /* A Field for Mag */
	JE_playSong(currentJukeboxSong);

	do
	{
		memcpy(VGAScreen, VGAScreen2, scr_width * scr_height);

		for (int i = 0; i <= maxSel; i++)
		{
			std::string text(opentyrian_menu_items[i]);

			if (i == 2) // Scaler
			{
				text = std::string("Scaler: ") + scalers[temp_scaler].name;
			}

			JE_outTextAdjust(JE_fontCenter(text.c_str(), SMALL_FONT_SHAPES),
			                 (i != maxSel) ? (i * 16 + 32) : 118, text.c_str(),
			                 15, (i != sel ? -4 : -2), SMALL_FONT_SHAPES, true);
		}

		JE_showVGA();

		if (fade_in)
		{
			fade_in = false;
			JE_fadeColor(20);
			wait_noinput(true,false,false);
		}

		tempW = 0;
		JE_textMenuWait(&tempW, false);

		if (newkey) {
			switch (lastkey_sym)
			{
				case SDLK_UP:
					sel--;
					if (sel < 0)
					{
						sel = maxSel;
					}
					JE_playSampleNum(CURSOR_MOVE);
					break;
				case SDLK_DOWN:
					sel++;
					if (sel > maxSel)
					{
						sel = 0;
					}
					JE_playSampleNum(CURSOR_MOVE);
					break;
				case SDLK_LEFT:
					if (sel == 2)
					{
						const bool surf_32bit = display_surface->format->BitsPerPixel == 32;
						do {
							if (temp_scaler == 0)
							{
								temp_scaler = COUNTOF(scalers);
							}
							--temp_scaler;
						} while ((surf_32bit && scalers[temp_scaler].scaler32 == NULL) ||
						         (!surf_32bit && scalers[temp_scaler].scaler16 == NULL));
						JE_playSampleNum(CURSOR_MOVE);
					}
					break;
				case SDLK_RIGHT:
					if (sel == 2)
					{
						const bool surf_32bit = display_surface->format->BitsPerPixel == 32;
						do {
							++temp_scaler;
							if (temp_scaler == COUNTOF(scalers))
							{
								temp_scaler = 0;
							}
						} while ((surf_32bit && scalers[temp_scaler].scaler32 == NULL) ||
						         (!surf_32bit && scalers[temp_scaler].scaler16 == NULL));
						JE_playSampleNum(CURSOR_MOVE);
					}
					break;
				case SDLK_RETURN:
					switch (sel)
					{
						case 0: /* About */
							JE_playSampleNum(SELECT);
							scroller_sine(about_text);
							memcpy(VGAScreen, VGAScreen2, scr_width * scr_height);
							JE_showVGA();
							fade_in = true;
							break;
						case 1: /* Fullscreen */
							CVars::fullscreen_enabled = !CVars::fullscreen_enabled;
							JE_playSampleNum(SELECT);
							break;
						case 2: // Scaler
							CVars::r_scaler.setArchive(temp_scaler);
							break;
						case 3: /* Jukebox */
							JE_playSampleNum(SELECT);
							JE_jukeboxGo();
							memcpy(VGAScreen, VGAScreen2, scr_width * scr_height);
							JE_showVGA();
							fade_in = true;
							break;
						default: /* Return to main menu */
							quit = true;
							JE_playSampleNum(ESC);
							break;
					}
					break;
				case SDLK_ESCAPE:
					quit = true;
					JE_playSampleNum(ESC);
					return;
				default:
					break;
			}
		}
	} while (!quit);
}

int main( int argc, char *argv[] )
{
	mt::seed((unsigned int)time(NULL));

	Console::initialize();
	KeyNames::initialize();

	if (SDL_Init(0))
	{
		Console::get() << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
		// TODO: We should totally bail out here =P
	}

	Console::get() << "Welcome to... >> OpenTyrian Enhanced " << get_opentyrian_version() << " <<\n";

	Console::get() << "Copyright (C) 2007-2009 The OpenTyrian Development Team\n\n";

	Console::get() << "This program comes with ABSOLUTELY NO WARRANTY.\n"
		<< "This is free software, and you are welcome to redistribute it\n"
		<< "under certain conditions.  See the file GPL.txt for details.\n" << std::endl;

	Filesystem::initialize();
	scan_autorun();
	scan_parameters(argc, argv);
	JE_loadConfiguration();

	JE_scanForEpisodes();

	recordFileNum = 1;
	playDemoNum = 0;
	playDemo = false;

	init_video();
	init_keyboard();

	if (CVars::input_joy_enabled)
	{
		init_joystick();
	}

	if (CVars::ch_xmas)
	{
		Console::get()
			<< "********************************" << std::endl
			<< "* Christmas has been detected. *" << std::endl
			<< "*  Activate Christmas?  (Y/N)  *" << std::endl
			<< "********************************" << std::endl;
		wait_input(true, true, true);
		if (lastkey_sym != SDLK_y)
		{
			CVars::ch_xmas = false;
		}
		CVars::ch_xmas = true; // (Re)load data
	}

	/* Default Options */
	youAreCheating = false;
	showMemLeft = false;
	playerPasswordInput = true;

	JE_loadSong(1);

	if (CVars::snd_enabled)
	{
		init_sound();
		JE_loadSndFile();
	}

	if (CVars::record_demo)
	{
		Console::get() << "Game will be recorded." << std::endl;
	}

	megaData1 = new JE_MegaDataType1;
	megaData2 = new JE_MegaDataType2;
	megaData3 = new JE_MegaDataType3;

	JE_loadMainShapeTables();
	JE_loadExtraShapes(); // Editship
	JE_loadHelpText();
	JE_loadPals();

	if (CVars::net_enabled)
	{
		// TODO Network
		try
		{
			netmanager = new network::NetManager();
		}
		catch (network::NetManager::NetInitException& e)
		{
			Console::get() << "\a7Error:\ax Unable to initialize networking: " << e.what() << std::endl;
			CVars::net_enabled = false;
		}
	}

	loadDestruct = false;
	stoppedDemo = false;

	JE_main();

	Console::deinitialize();
	deinit_video();

	return 0;
}
