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
#include "opentyr.h"

#include "keyboard.h"
#include "vga256d.h"
#include "console/Console.h"
#include "console/CVar.h"
#include "network.h"

#include "joystick.h"

#include "SDL.h"
#include <algorithm>

const JE_ButtonAssign defaultJoyButtonAssign = {1, 4, 5, 5};

JE_ButtonType tempButton, button, joyButton;
bool buttonHeld;

JE_ButtonAssign joyButtonAssign;

bool useButtonAssign = false;

/* Joystick Data */
Sint16 joyX, joyY;
Sint16 lastJoyX,  lastJoyY;
Sint16 lastJoyXb, lastJoyYb;
Sint16 lastJoyXc, lastJoyYc;
Sint16 lastJoyXd, lastJoyYd;

int joystickError;
bool joystickUp, joystickDown, joystickLeft, joystickRight, joystickInput;
bool joystick_installed = false;
JE_word joystickWait, joystickWaitMax;

/*int SJoyCD;*/ /*NortVars anybutton check - problems with checking too fast*/

SDL_Joystick *joystick = NULL;

static bool joystick_initialized = false;

static bool input_joy_enabled_callback( const bool& init )
{
	if (init)
	{
		return init_joystick();
	}
	else
	{
		return !deinit_joystick();
	}
}

namespace CVars
{
	CVarFloat input_joy_sensitivity("input_joy_sensitivity", CVar::CONFIG, "Joystick sensitivity.", .5f, rangeBind(0.f, 1.f));
	CVarBool input_joy_filter("input_joy_filter", CVar::CONFIG, "Enables averaging of joystick input.", false);
	CVarBool input_joy_enabled("input_joy_enabled", CVar::CONFIG, "Enables joystick subsystem.", true);
}

void JE_joystick1( void ) /* procedure to get x and y */
{
	for (unsigned int i = 0; i < COUNTOF(joyButton); i++)
	{
		joyButton[i] = (SDL_JoystickGetButton(joystick, i) == 1);
	}

	joyX = SDL_JoystickGetAxis(joystick, 0);
	joyY = SDL_JoystickGetAxis(joystick, 1);

	if (CVars::input_joy_filter || isNetworkGame)
	{
		lastJoyXd = lastJoyXc;
		lastJoyXc = lastJoyXb;
		lastJoyXb = lastJoyX;
		lastJoyX = joyX;
		joyX = (joyX + lastJoyX + lastJoyXb + lastJoyXc + lastJoyXd) / 5;

		lastJoyYd = lastJoyYc;
		lastJoyYc = lastJoyYb;
		lastJoyYb = lastJoyY;
		lastJoyY = joyY;
		joyY = (joyY + lastJoyY + lastJoyYb + lastJoyYc + lastJoyYd) / 5;
	}
}

void JE_updateButtons( void ) /* Uses ButtonAssign to find out */
{
	/* Button Functions:
	 * 1 : Fire Normal
	 * 2 : Fire Left Sidekick
	 * 3 : Fire Right Sidekick
	 * 4 : Fire Both Sidekicks
	 * 5 : Switch Rear Weapon
	 */

	memset(button, 0, sizeof(button));

	for (unsigned int i = 0; i < COUNTOF(joyButton); i++)
	{
		switch (joyButtonAssign[i])
		{
			case 1:
				button[1-1] |= joyButton[i];
				break;
			case 2:
				button[2-1] |= joyButton[i];
				break;
			case 3:
				button[3-1] |= joyButton[i];
				break;
			case 4:
				button[2-1] |= joyButton[i];
				button[3-1] |= joyButton[i];
				break;
			case 5:
				button[4-1] |= joyButton[i];
				break;
		}
	}
}

void JE_joystick2( void )
{
	joystickInput = false;

	if (joystick_installed)
	{
		JE_joystick1();
		memcpy(button, joyButton, sizeof(button));
		/*JE_UpdateButtons;*/

		joystickUp    = joyY < (-32768 * (1.f - CVars::input_joy_sensitivity));
		joystickDown  = joyY > ( 32768 * (1.f - CVars::input_joy_sensitivity));
		
		joystickLeft  = joyX < (-32768 * (1.f - CVars::input_joy_sensitivity));
		joystickRight = joyX > ( 32768 * (1.f - CVars::input_joy_sensitivity));
		
		joystickInput = joystickUp || joystickDown || joystickLeft || joystickRight || button[0] || button[1] || button[2] || button[3];
	} else {
		std::fill(button, button+COUNTOF(button), false);
	}
}

bool JE_nextJoystickCheck( void )
{
	JE_joystick2();
	if (joystickInput)
	{
		if (joystickWait > 0)
			joystickWait--;
		
		for (unsigned int i = 0; i < COUNTOF(joyButton); i++)
		{
			if (joyButton[i])
				return true;
		}
		
		if (joystickWait == 0)
		{
			joystickWait = joystickWaitMax;
			return true;
		}
	} else {
		joystickWait = 0;
	}
	return false;
}

bool JE_joystickTranslate( void )
{
	bool tempb;

	joystickUp = false;
	joystickDown = false;
	joystickLeft = false;
	joystickRight = false;

	tempb = JE_nextJoystickCheck();

	if (tempb)
	{
		if (joystickUp)
		{
			newkey = true;
			lastkey_char = lastkey_sym = SDLK_UP;
		}
		if (joystickDown)
		{
			newkey = true;
			lastkey_char = lastkey_sym = SDLK_DOWN;
		}
		if (joystickLeft)
		{
			newkey = true;
			lastkey_char = lastkey_sym = SDLK_LEFT;
		}
		if (joystickRight)
		{
			newkey = true;
			lastkey_char = lastkey_sym = SDLK_RIGHT;
		}
		if (button[0])
		{
			newkey = true;
			lastkey_char = lastkey_sym = SDLK_RETURN;
		}
		if (button[1])
		{
			newkey = true;
			lastkey_char = lastkey_sym = SDLK_ESCAPE;
		}
	}

	return tempb;
}

bool JE_joystickNotHeld( void )
{
	if (JE_joystickTranslate())
	{
		if (newkey && (lastkey_sym == SDLK_RETURN || lastkey_sym == SDLK_ESCAPE))
		{
			if (buttonHeld)
			{
				newkey = false;
			} else {
				buttonHeld = true;
				return true;
			}
		} else {
			buttonHeld = false;
			return true;
		}
	} else {
		buttonHeld = false;
	}

	return false;
}

bool init_joystick( void )
{
	if (joystick_initialized)
		return true;

	Console::get() << "Initializing joysticks...";

	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK))
	{
		// New line to complete last line
		Console::get() << "\n\a7Error:\ax Failed to initialize joystick: " << SDL_GetError() << std::endl;
		CVars::input_joy_enabled = false;
		return false;
	}
	if (SDL_NumJoysticks() > 0)
	{
		joystick = SDL_JoystickOpen(0);

		if (joystick)
		{
			if (SDL_JoystickNumButtons(joystick) >= 4 && SDL_JoystickNumAxes(joystick) >= 2)
			{
				joystick_installed = true;
			}

			const char* name = SDL_JoystickName(0);
			Console::get() << " Found";
			if (name)
				Console::get() << ": " << name;
			else
				Console::get() << '.';
			
			Console::get() << std::endl;
		}
	}
	else
	{
		joystick_installed = false;
		Console::get() << " None found." << std::endl;
	}

	joystick_initialized = true;

	return true;
}

bool deinit_joystick( void )
{
	if (!joystick_initialized)
		return true;

	SDL_QuitSubSystem(SDL_INIT_JOYSTICK);

	joystick_installed = false;
	joystick_initialized = false;

	return true;
}
