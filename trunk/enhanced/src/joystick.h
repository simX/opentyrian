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
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "opentyr.h"

#include "console/cvar/CVar.h"

typedef bool JE_ButtonType[4]; /* [1..4] */
typedef int JE_ButtonAssign[4]; /* [1..4] */

namespace CVars
{
	extern CVarBool input_joy_enabled;
}

extern const JE_ButtonAssign defaultJoyButtonAssign;
extern JE_ButtonType tempButton, button, joyButton;
extern bool buttonHeld;
extern JE_ButtonAssign joyButtonAssign;
extern bool useButtonAssign;
extern Sint16 joyX, joyY;
extern int joystickError;
extern bool joystickUp, joystickDown, joystickLeft, joystickRight, joystickInput;
extern bool joystick_installed;
extern JE_word joystickWait, joystickWaitMax;

void JE_joystick1( void ); /*procedure to get JoyX, JoyY, Button1, Button2 of Joystick 1*/
void JE_joystick2( void );
bool JE_nextJoystickCheck( void );
bool JE_joystickTranslate( void );
bool JE_joystickNotHeld( void );
void JE_updateButtons( void ); /*Uses ButtonAssign to find out*/

bool init_joystick( void );
bool deinit_joystick( void );

#endif /* JOYSTICK_H */
