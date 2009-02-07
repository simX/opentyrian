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

/**
 * @file video.h Display related routines.
 */
#ifndef VIDEO_H
#define VIDEO_H

#include "opentyr.h"

#include "palette.h"
#include "video_scale.h"

#include "SDL.h"

static const unsigned int scr_width = 320;
static const unsigned int scr_height = 200;

static const unsigned int surface_width = 320;
static const unsigned int surace_height = 200;

extern bool fullscreen_enabled;

extern const Palette vga_palette;

extern SDL_Surface *display_surface;
extern Uint8 *VGAScreen, *VGAScreenSeg;
extern Uint8 *game_screen;
extern Uint8 *VGAScreen2;

/** Initializes SDL video and creates the game window. */
void init_video();

/** Re-initializes the video do change video modes. */
void reinit_video();

/** Closes the game window and shuts down SDL video. */
void deinit_video();

/** Flips the screen buffers to update the display. */
void update_video();

/** @deprecated Use update_video instead. */
inline void JE_showVGA() { update_video(); }

/**
 * Clears entire screen to \p color.
 *
 * @param color the color used clear the screen
 */
void clear_screen(Uint8 color = 0);

/** @deprecated Use clear_screen instead. */
inline void JE_clr256() { clear_screen(0); }

/**
 * Calculates a linear off set off a set of \p x and \p y coordinates.
 *
 * @param x the X coordinate. (Ranges from 0 to scr_width.)
 * @param y the Y coordinate. (Ranges from 0 to scr_height.)
 * @return the linear offset.
 */
inline unsigned long xy2off(unsigned int x, unsigned int y)
{
	return y*scr_width + x;
}

#endif // VIDEO_H
