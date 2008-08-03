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
#ifndef VIDEO_H
#define VIDEO_H

#include "opentyr.h"

#include "palette.h"
#include "video_scale.h"

#include "SDL.h"

static const int scr_width = 320;
static const int scr_height = 200;

static const int surface_width = 320;
#ifdef TARGET_GP2X
static const int surace_height = 240;
#else
static const int surace_height = 200;
#endif // TARGET_GP2X

extern bool fullscreen_enabled;

extern const Palette vga_palette;

extern SDL_Surface *display_surface;
extern Uint8 *VGAScreen, *VGAScreenSeg;
extern Uint8 *game_screen;
extern Uint8 *VGAScreen2;

void init_video( );
void reinit_video( );
void deinit_video( );
void JE_clr256( );
void JE_showVGA( );

#endif // VIDEO_H
