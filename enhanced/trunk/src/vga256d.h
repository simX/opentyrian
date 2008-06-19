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
#ifndef VGA256D_H
#define VGA256D_H

#include "opentyr.h"

#include "nortvars.h"

#include "SDL.h"

static const int scr_width = 320;
static const int scr_height = 200;

static const int surface_width = 320;
#ifdef TARGET_GP2X
#      define surface_height 240
#else
#      define surface_height 200
#endif // TARGET_GP2X

typedef JE_word JE_shape16B[1]; /* [0.. 0] */
typedef JE_shape16B *JE_shape16;
/*typedef JE_byte JE_shapetypeone[168];*/ /* [0..168-1] originally: JE_word JE_shapetypeone[84]; [1..84] */
typedef Uint8 JE_screentype[65535]; /* [0..65534] */
typedef JE_screentype *JE_screenptr;

extern const SDL_Color vga_palette[];
extern SDL_Surface *display_surface;
extern Uint8 *VGAScreen, *VGAScreenSeg;
extern Uint8 *game_screen;
extern Uint8 *VGAScreen2;
extern JE_word speed; /* JE: holds timer speed for 70Hz */

void JE_initVGA256( void );
void set_fullscreen( bool full );
void JE_closeVGA256( void );
void JE_clr256( void );
void JE_showVGA( void );
void JE_pix( JE_word x, JE_word y, Uint8 col );
void JE_pixCool( JE_word x, JE_word y, Uint8 col );
void JE_pixAbs( JE_word offset, Uint8 col );
void JE_getPix( JE_word x, JE_word y, Uint8 *col );
Uint8 JE_getPixel( JE_word x, JE_word y );
void JE_rectangle( JE_word x1, JE_word y1, JE_word x2, JE_word y2, JE_word col );
void JE_bar( JE_word x1, JE_word y1, JE_word x2, JE_word y2, Uint8 col );
void JE_c_bar( JE_word x1, JE_word y1, JE_word x2, JE_word y2, Uint8 col );
void JE_barShade( JE_word x1, JE_word y1, JE_word x2, JE_word y2 );
void JE_barShade2( JE_word x1, JE_word y1, JE_word x2, JE_word y2 );
void JE_barBright( JE_word x1, JE_word y1, JE_word x2, JE_word y2 );
void JE_circle( JE_word x, int y, JE_word radius, Uint8 col );
void JE_line( JE_word x1, int y1, int x2, int y2, Uint8 col );
void JE_drawGraphic( JE_word x, JE_word y, JE_ShapeTypeOne s );
void JE_getPalette( Uint8 col, Uint8 *red, Uint8 *green, Uint8 *blue );
void JE_setPalette( Uint8 col, Uint8 red, Uint8 green, Uint8 blue );

#endif /* VGA256D_H */
