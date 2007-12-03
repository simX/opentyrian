/* vim: set noet:
 *
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
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
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "opentyr.h"

#ifndef TARGET_GP2X
typedef bool JE_ButtonType[4]; /* [1..4] */
typedef int JE_ButtonAssign[4]; /* [1..4] */
#else
typedef bool JE_ButtonType[19];
typedef int JE_ButtonAssign[19];
#endif

extern const JE_ButtonAssign defaultJoyButtonAssign;
extern JE_ButtonType tempButton, button;
extern bool buttonHeld;
extern JE_ButtonAssign joyButtonAssign;
extern bool useButtonAssign;
extern Sint16 jMinX, jMaxX, jMinY, jMaxY, jCenterX, jCenterY, joyX, joyY;
extern int joystickError;
extern bool joystickUp, joystickDown, joystickLeft, joystickRight, joystickInput;
extern bool joystick_installed;
extern JE_word joystickWait, joystickWaitMax;

void JE_joystick1( void ); /*procedure to get JoyX, JoyY, Button1, Button2 of Joystick 1*/
void JE_joystick2( void );
bool JE_nextJoystickCheck( void );
bool JE_joystickTranslate( void );
void JE_joystickInit( void );
bool JE_joystickNotHeld( void );
void JE_updateButtons( void ); /*Uses ButtonAssign to find out*/

void joystick_init( void );

#endif /* JOYSTICK_H */
