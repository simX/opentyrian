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
#include "params.h"

#include <ctime>
#include <string>


bool richMode, recordDemo, robertWeird, constantPlay, constantDie, scanForJoystick, noSound, joyMax, forceAveraging;

/* JE: Special Note:
 * The two booleans to detect network play for Tim's stuff.
 * It's in here because of the dumb port 60 bug.
 */
bool isNetworkGame, isNetworkActive;

bool tyrianXmas;

static const std::string pars[11] = {
	"LOOT", "RECORD", "NOJOY", "NOROBERT", "CONSTANT", "DEATH", "NOSOUND", "JOYMAX", "WEAKJOY", "NOXMAS", "YESXMAS"
};

void JE_paramCheck( int argc, char *argv[] )
{
	robertWeird     = true;
	richMode        = false;
	recordDemo      = false;
	scanForJoystick = true;
	constantPlay    = false;
	noSound         = false;
	joyMax          = false;
	forceAveraging  = false;

	std::time_t now = std::time(NULL);
	tyrianXmas = std::localtime(&now)->tm_mon == 11;

/* JE: Note:
Parameters are no longer case-sensitive.

LOOTþ    - Gives you mucho bucks.
BORDER   - Network border mode (useless)
NOROBERT - Alternate selection of items in upgrade menu.
RECORD   - Records all level and stores the last one in DEMOREC.num file
           (When a file is taken, it increments "num" until no file is found)

        Note: Mouse and Joystick are disabled when recording.

NOJOY    - Disables joystick detection
NOKEY    - Forces Tyrian to send keys to the BIOS just in case some other
           device driver needs input.  It still uses the keyboard input
           driver, though.

CONSTANT - Constant play for testing purposes (C key activates invincibility)
           This might be useful for publishers to see everything - especially
           those who can't play it.

DEATH    - Constant death mode.  Only useful for testing.

NOSOUND  - Disables Sound/Music usage even if FX or Music are in the config
           file.  It tells the initialization routine to NEVER check for
           initialization and will not load the Sound Effects, saving memory
           in SETUP.
           FM sound will still be automatically detected since I have no
           control over that.
           You can, however, select sound and music selections that would
           otherwise not work and it won't return an error since it does
           not try to detect them.  This might help if someone has a problem
           only in SETUP and not in Tyrian.

JUKEBOX  - Only works in SETUP.  Takes you directly to the jukebox.

MAXVOL   - Force Tyrian or SETUP to set the card's main volume to maximum
           if it is able to instead of reading the current value.

SOUND?   - Display IRQ/DMA junk so Andreas has more info.  Only works in SETUP.
           This will save a file called SOUND.RPT to your HD detailing your
           sound selections and any initialization errors for convenient
           uploading on CServe/AOL/Internet.

FLICKER  - This will force Tyrian and SETUP to NEVER WAIT FOR A RETRACE.
           Might be helpful for unknown reasons.  Perhaps a sound card is
           cutting out for some related reason?    [V1.2]

JOYMAX   - Sets your joystick to maximum sensitivity.   [V1.2]
*/

	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '+') {
			Console::get().runCommand(std::string(argv[i]+1));
		} else {
			std::string tempStr(argv[i]);
			for (std::string::iterator i = tempStr.begin(); i != tempStr.end(); i++)
			{
				*i = toupper(*i);
			}

			for (unsigned int j = 0; j < COUNTOF(pars); j++)
			{
				if (tempStr == pars[j])
				{
					switch (j)
					{
						case 0:
							richMode = true;
							break;
						case 1:
							recordDemo = true;
							Console::get() << "Use a keyboard to record a demo." << std::endl;
							break;
						case 2:
							scanForJoystick = false;
							break;
						case 3:
							robertWeird = false;
							break;
						case 4:
							constantPlay = true;
							break;
						case 5:
							constantDie = true;
							break;
						case 6:
							noSound = true;
							break;
						case 7:
							joyMax = true;
							break;
						case 8:
							forceAveraging = true;
							break;
						case 9:
							tyrianXmas = false;
							break;
						case 10:
							tyrianXmas = true;
							break;
						default:
							/* YKS: This shouldn't ever be reached. */
							DEBUG_MSG("!!! WARNING: Something's very wrong on " << __FILE__ << ":" << __LINE__);
							break;
					}
					break;
				}
			}
		}
	}
}
