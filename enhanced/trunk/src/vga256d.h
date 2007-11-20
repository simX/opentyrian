/* vim: set noet:
 *
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
 * Copyright (C) 2007  The OpenTyrian Team
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


#define CRT_ADDRESS 0x3D4
#define STATUS_REG 0x3DA

typedef JE_word JE_shape16B[1]; /* [0.. 0] */
typedef JE_shape16B *JE_shape16;
/*typedef JE_byte JE_shapetypeone[168];*/ /* [0..168-1] originally: JE_word JE_shapetypeone[84]; [1..84] */
typedef Uint8 JE_screentype[65535]; /* [0..65534] */
typedef JE_screentype *JE_screenptr;

#ifndef NO_EXTERNS
extern bool mouseInstalled;
extern char k;
extern SDL_Surface *VGAScreen, *VGAScreenSeg;
extern SDL_Surface *game_screen;
extern SDL_Surface *VGAScreen2;
extern JE_word speed; /* JE: holds timer speed for 70Hz */
extern Uint8 scancode;
#endif

void JE_initVGA256( void );
void set_fullscreen( bool full );
void JE_initVGA256X( void );
void JE_closeVGA256( void );
void JE_clr256( void );
void JE_showVGA( void );
void JE_showVGARetrace( void );
void JE_getVGA( void );
void JE_onScreen( void );
void JE_offScreen( void );
void JE_disableRefresh( void );
void JE_enableRefresh( void );
void JE_waitRetrace( void );
void JE_waitPartialRetrace( void );
void JE_waitNotRetrace( void );
void JE_pix( JE_word x, JE_word y, Uint8 c );
void JE_pix2( JE_word x, JE_word y, Uint8 c );
void JE_pixCool( JE_word x, JE_word y, Uint8 c );
void JE_pix3( JE_word x, JE_word y, Uint8 c );
void JE_pixAbs( JE_word x, Uint8 c );
void JE_getPix( JE_word x, JE_word y, Uint8 *c );
Uint8 JE_getPixel( JE_word x, JE_word y );
void JE_rectangle( JE_word a, JE_word b, JE_word c, JE_word d, JE_word e );
void JE_bar( JE_word a, JE_word b, JE_word c, JE_word d, Uint8 e );
void JE_c_bar( JE_word a, JE_word b, JE_word c, JE_word d, Uint8 e );
void JE_barShade( JE_word a, JE_word b, JE_word c, JE_word d );
void JE_barShade2( JE_word a, JE_word b, JE_word c, JE_word d );
void JE_barBright( JE_word a, JE_word b, JE_word c, JE_word d );
void JE_circle( JE_word x, int y, JE_word z, Uint8 c );
void JE_line( JE_word a, int b, int c, int d, Uint8 e );
void JE_drawGraphic( JE_word x, JE_word y, JE_ShapeTypeOne s );
void JE_getPalette( Uint8 col, Uint8 *red, Uint8 *green, Uint8 *blue );
void JE_setPalette( Uint8 col, Uint8 red, Uint8 green, Uint8 blue );

#endif /* VGA256D_H */
