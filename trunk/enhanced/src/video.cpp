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

#include "video.h"

#include "keyboard.h"

const Palette vga_palette = {
	{0, 0, 0}, {0, 0, 168}, {0, 168, 0}, {0, 168, 168}, {168, 0, 0}, {168, 0, 168}, {168, 84, 0}, {168, 168, 168}, {84, 84, 84}, {84, 84, 252}, {84, 252, 84}, {84, 252, 252}, {252, 84, 84}, {252, 84, 252}, {252, 252, 84}, {252, 252, 252},
	{0, 0, 0}, {20, 20, 20}, {32, 32, 32}, {44, 44, 44}, {56, 56, 56}, {68, 68, 68}, {80, 80, 80}, {96, 96, 96}, {112, 112, 112}, {128, 128, 128}, {144, 144, 144}, {160, 160, 160}, {180, 180, 180}, {200, 200, 200}, {224, 224, 224}, {252, 252, 252},
	{0, 0, 252}, {64, 0, 252}, {124, 0, 252}, {188, 0, 252}, {252, 0, 252}, {252, 0, 188}, {252, 0, 124}, {252, 0, 64}, {252, 0, 0}, {252, 64, 0}, {252, 124, 0}, {252, 188, 0}, {252, 252, 0}, {188, 252, 0}, {124, 252, 0}, {64, 252, 0},
	{0, 252, 0}, {0, 252, 64}, {0, 252, 124}, {0, 252, 188}, {0, 252, 252}, {0, 188, 252}, {0, 124, 252}, {0, 64, 252}, {124, 124, 252}, {156, 124, 252}, {188, 124, 252}, {220, 124, 252}, {252, 124, 252}, {252, 124, 220}, {252, 124, 188}, {252, 124, 156},
	{252, 124, 124}, {252, 156, 124}, {252, 188, 124}, {252, 220, 124}, {252, 252, 124}, {220, 252, 124}, {188, 252, 124}, {156, 252, 124}, {124, 252, 124}, {124, 252, 156}, {124, 252, 188}, {124, 252, 220}, {124, 252, 252}, {124, 220, 252}, {124, 188, 252}, {124, 156, 252},
	{180, 180, 252}, {196, 180, 252}, {216, 180, 252}, {232, 180, 252}, {252, 180, 252}, {252, 180, 232}, {252, 180, 216}, {252, 180, 196}, {252, 180, 180}, {252, 196, 180}, {252, 216, 180}, {252, 232, 180}, {252, 252, 180}, {232, 252, 180}, {216, 252, 180}, {196, 252, 180},
	{180, 252, 180}, {180, 252, 196}, {180, 252, 216}, {180, 252, 232}, {180, 252, 252}, {180, 232, 252}, {180, 216, 252}, {180, 196, 252}, {0, 0, 112}, {28, 0, 112}, {56, 0, 112}, {84, 0, 112}, {112, 0, 112}, {112, 0, 84}, {112, 0, 56}, {112, 0, 28},
	{112, 0, 0}, {112, 28, 0}, {112, 56, 0}, {112, 84, 0}, {112, 112, 0}, {84, 112, 0}, {56, 112, 0}, {28, 112, 0}, {0, 112, 0}, {0, 112, 28}, {0, 112, 56}, {0, 112, 84}, {0, 112, 112}, {0, 84, 112}, {0, 56, 112}, {0, 28, 112},
	{56, 56, 112}, {68, 56, 112}, {84, 56, 112}, {96, 56, 112}, {112, 56, 112}, {112, 56, 96}, {112, 56, 84}, {112, 56, 68}, {112, 56, 56}, {112, 68, 56}, {112, 84, 56}, {112, 96, 56}, {112, 112, 56}, {96, 112, 56}, {84, 112, 56}, {68, 112, 56},
	{56, 112, 56}, {56, 112, 68}, {56, 112, 84}, {56, 112, 96}, {56, 112, 112}, {56, 96, 112}, {56, 84, 112}, {56, 68, 112}, {80, 80, 112}, {88, 80, 112}, {96, 80, 112}, {104, 80, 112}, {112, 80, 112}, {112, 80, 104}, {112, 80, 96}, {112, 80, 88},
	{112, 80, 80}, {112, 88, 80}, {112, 96, 80}, {112, 104, 80}, {112, 112, 80}, {104, 112, 80}, {96, 112, 80}, {88, 112, 80}, {80, 112, 80}, {80, 112, 88}, {80, 112, 96}, {80, 112, 104}, {80, 112, 112}, {80, 104, 112}, {80, 96, 112}, {80, 88, 112},
	{0, 0, 64}, {16, 0, 64}, {32, 0, 64}, {48, 0, 64}, {64, 0, 64}, {64, 0, 48}, {64, 0, 32}, {64, 0, 16}, {64, 0, 0}, {64, 16, 0}, {64, 32, 0}, {64, 48, 0}, {64, 64, 0}, {48, 64, 0}, {32, 64, 0}, {16, 64, 0},
	{0, 64, 0}, {0, 64, 16}, {0, 64, 32}, {0, 64, 48}, {0, 64, 64}, {0, 48, 64}, {0, 32, 64}, {0, 16, 64}, {32, 32, 64}, {40, 32, 64}, {48, 32, 64}, {56, 32, 64}, {64, 32, 64}, {64, 32, 56}, {64, 32, 48}, {64, 32, 40},
	{64, 32, 32}, {64, 40, 32}, {64, 48, 32}, {64, 56, 32}, {64, 64, 32}, {56, 64, 32}, {48, 64, 32}, {40, 64, 32}, {32, 64, 32}, {32, 64, 40}, {32, 64, 48}, {32, 64, 56}, {32, 64, 64}, {32, 56, 64}, {32, 48, 64}, {32, 40, 64},
	{44, 44, 64}, {48, 44, 64}, {52, 44, 64}, {60, 44, 64}, {64, 44, 64}, {64, 44, 60}, {64, 44, 52}, {64, 44, 48}, {64, 44, 44}, {64, 48, 44}, {64, 52, 44}, {64, 60, 44}, {64, 64, 44}, {60, 64, 44}, {52, 64, 44}, {48, 64, 44},
	{44, 64, 44}, {44, 64, 48}, {44, 64, 52}, {44, 64, 60}, {44, 64, 64}, {44, 60, 64}, {44, 52, 64}, {44, 48, 64}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
};

SDL_Surface *display_surface;
Uint8 *VGAScreen, *VGAScreenSeg;
Uint8 *game_screen;
Uint8 *VGAScreen2;

static bool fullscreen_enabled_callback(const bool& val)
{
	reinit_video();
	return val;
}

namespace CVars
{
	CVarBool fullscreen_enabled("fullscreen_enabled", CVar::CONFIG | CVar::CONFIG_AUTO, "Fullscreen.", false, fullscreen_enabled_callback);
}

void init_video( )
{
	if (SDL_WasInit(SDL_INIT_VIDEO) != 0) return;

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1)
	{
		Console::get() << "\a7Error:\ax Display initialization failed: " << SDL_GetError() << std::endl;
		exit(1);
	}

	SDL_WM_SetCaption("OpenTyrian (ctrl-backspace to kill)", NULL);

	VGAScreen = VGAScreenSeg = new Uint8[scr_width*scr_height];

	VGAScreen2 = new Uint8[scr_width*scr_height];
	game_screen = new Uint8[scr_width*scr_height];

	std::fill_n(VGAScreen, scr_width*scr_height, 0x0);
	std::fill_n(VGAScreen2, scr_width*scr_height, 0x0);
	std::fill_n(game_screen, scr_width*scr_height, 0x0);

#if 0 //_WIN32
	if (!SDL_getenv("SDL_VIDEODRIVER"))
	{
		SDL_putenv("SDL_VIDEODRIVER=directx");
	}
#endif

	reinit_video();

	SDL_FillRect(display_surface, NULL, 0x0);
}

void reinit_video( )
{
	if (SDL_WasInit(SDL_INIT_VIDEO) == 0)
		return;

	scale = scalers[CVars::r_scaler].scale;

	const int w = scr_width * scale;
	const int h = scr_height * scale;
	int bpp = 32;
	const int flags = SDL_SWSURFACE | SDL_HWPALETTE | (CVars::fullscreen_enabled ? SDL_FULLSCREEN : 0);

	bpp = SDL_VideoModeOK(w, h, bpp, flags);
	if (bpp == 24) bpp = 32;

	display_surface = SDL_SetVideoMode(w, h, bpp, flags);

	if (display_surface == NULL)
	{
		Console::get() << "\a7Warning:\ax Failed to initialize SDL video: " << SDL_GetError() << std::endl;
		exit(1);
	}

	Console::get() << "Initialized SDL video: " << display_surface->w << 'x' << display_surface->h << 'x' <<
		int(display_surface->format->BitsPerPixel) << (display_surface->flags & SDL_FULLSCREEN ? " Fullscreen" : " Windowed") << std::endl;

	input_grab();

	JE_showVGA();
}

void deinit_video( )
{
	SDL_FreeSurface(display_surface); display_surface = 0;
	delete VGAScreenSeg; VGAScreenSeg = 0;
	delete game_screen; game_screen = 0;
	delete VGAScreen2; VGAScreen2 = 0;

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void update_video( )
{
	SDL_LockSurface(display_surface);

	switch (display_surface->format->BitsPerPixel)
	{
	case 32:
		if (scalers[CVars::r_scaler].scaler32 == 0) CVars::r_scaler = 0;
		scalers[CVars::r_scaler].scaler32(VGAScreen, display_surface, scale);
		break;
	case 16:
		if (scalers[CVars::r_scaler].scaler16 == 0) CVars::r_scaler = 0;
		scalers[CVars::r_scaler].scaler16(VGAScreen, display_surface, scale);
		break;
	default:
		Console::get() << "\a7Error:\ax Unsupported bit-depth: " << display_surface->format->BitsPerPixel<< " (please report this to devs)" << std::endl;
		break;
	}

	SDL_UnlockSurface(display_surface);
	SDL_Flip(display_surface);
}

void clear_screen(Uint8 color)
{
	memset(VGAScreen, 0, scr_width * scr_height);
}