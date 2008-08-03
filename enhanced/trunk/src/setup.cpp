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

#include "config.h"
#include "fonthand.h"
#include "joystick.h"
#include "jukebox.h"
#include "keyboard.h"
#include "loudness.h"
#include "network.h"
#include "newshape.h"
#include "nortsong.h"
#include "nortvars.h"
#include "starlib.h"
#include "varz.h"
#include "video.h"
#include "vga256d.h"
#include "loudness.h"

#include "setup.h"

#include "SDL.h"


bool volumeActive = true;
bool fx;
JE_word fxNum;
bool repeatedFade;

void JE_textMenuWait( JE_word *waitTime, bool doGamma )
{
	JE_setMousePosition(160, 100);

	do
	{
		JE_showVGA();

		service_SDL_events(true);

		inputDetected = newkey | mousedown | JE_joystickNotHeld();

		if (lastkey_sym == SDLK_SPACE)
		{
			lastkey_sym = SDLK_RETURN;
		}

		if (mousedown)
		{
			newkey = true;
			lastkey_sym = SDLK_RETURN;
		}

		if (input_grabbed)
		{
			if (abs(mouse_y - 100) > 10)
			{
				inputDetected = true;
				if (mouse_y - 100 < 0)
				{
					lastkey_sym = SDLK_UP;
				} else {
					lastkey_sym = SDLK_DOWN;
				}
				newkey = true;
			}
			if (abs(mouse_x - 160) > 10)
			{
				inputDetected = true;
				if (mouse_x - 160 < 0)
				{
					lastkey_sym = SDLK_LEFT;
				} else {
					lastkey_sym = SDLK_RIGHT;
				}
				newkey = true;
			}
		}

		SDL_Delay(16); /* <MXD> attempt non-processor-based wait, implement a real delay later */
		if (*waitTime > 0)
		{
			(*waitTime)--;
		}
	} while (!(inputDetected || *waitTime == 1 || haltGame || netQuit));
}


void JE_jukeboxGo( void )
{
	bool weirdMusic, weirdCurrent;
	int weirdSpeed = 0;
	char tempStr[64];

	int lastSong;
	float tempVolume;
	bool youStopped, drawText, quit, fade;


	weirdMusic = false;
	weirdCurrent = true;
	drawText = true;

	fx = false;
	fxNum = 1;

	lastSong = currentJukeboxSong;

	JE_fadeBlack(10);
	std::fill_n(VGAScreenSeg, scr_width*scr_height, 0x0);
	JE_showVGA();
	JE_updateColorsFast(vga_palette);

	JE_starlib_init();

	quit = false;
	fade = false;
	repeatedFade = false;

	tempVolume = CVars::s_music_vol;
	youStopped = false;

	JE_wipeKey();

	do
	{
		tempScreenSeg = VGAScreenSeg;

		if (weirdMusic)
		{
			if (delaycount2() == 0)
			{
				setjasondelay2(weirdSpeed);

				if (weirdCurrent)
				{
					music_vol_multiplier = .5f;
				} else {
					music_vol_multiplier = 1.f;
				}

				weirdCurrent = !weirdCurrent;
			}
		}

		if (repeated && !repeatedFade)
		{
			fade = true;
			repeatedFade = true;
		}

		if ( ( (repeated && !fade) || !playing) && !youStopped)
		{
			currentJukeboxSong = ( rand() % MUSIC_NUM );
			JE_playNewSong();
		}

		setdelay(1);
		service_SDL_events(true);

		JE_starlib_main();

		if (lastSong != currentJukeboxSong)
		{
			lastSong = currentJukeboxSong;
			JE_bar(50, 190, 250, 198, 0);
		}

		if (drawText)
		{
			tempScreenSeg = VGAScreenSeg;
			if (fx)
			{
				sprintf(tempStr, "%d %s", fxNum, soundTitle[fxNum - 1]);
				JE_bar(50, 190, 250, 198, 0);
				JE_outText(JE_fontCenter(tempStr, TINY_FONT), 190, tempStr, 1, 4);
			} else {
				sprintf(tempStr, "%d %s", currentJukeboxSong, musicTitle[currentJukeboxSong - 1]);
				JE_outText(JE_fontCenter(tempStr, TINY_FONT), 190, tempStr, 1, 4);
			}

			tempScreenSeg = VGAScreenSeg;
			JE_outText(JE_fontCenter("Press ESC to quit the jukebox.", TINY_FONT), 170, "Press ESC to quit the jukebox.", 1, 0);
			tempScreenSeg = VGAScreenSeg;
			JE_outText(JE_fontCenter("Arrow keys change the song being played.", TINY_FONT), 180, "Arrow keys change the song being played.", 1, 0);
		}

		int delaycount_temp;
		if ((delaycount_temp = target - SDL_GetTicks()) > 0)
			SDL_Delay(delaycount_temp);

		JE_showVGA();

		if (fade)
		{
			if (volumeActive)
			{
				if (tempVolume > 5)
				{
					tempVolume -= 2;
					music_vol_multiplier = tempVolume; // TODO
				} else {
					fade = false;
				}
			}
			else if (speed < 0xE000)
			{
				speed += 0x800;
			} else {
				speed = 0xE000;
				fade = false;
			}
			JE_resetTimerInt();
			JE_setTimerInt();
		}

		JE_joystick2();
		if (JE_mousePosition(&tempX, &tempY) > 0 || button[0])
		{
			quit = true;
			JE_wipeKey();
		}
		
		if (joystickInput)
		{
			if (joystickUp || joystickLeft)
			{
				currentJukeboxSong--;
				JE_playNewSong();
			} else if (joystickDown || joystickRight) {
				currentJukeboxSong++;
				JE_playNewSong();
			}
		}

		if (joystickInput)
		{
			if (joystickUp || joystickLeft)
			{
				currentJukeboxSong--;
				JE_playNewSong();
			} else if (joystickDown || joystickRight) {
				currentJukeboxSong++;
				JE_playNewSong();
			}
		}

		if (newkey) {
			JE_newSpeed();
			switch (lastkey_sym)
			{
			case SDLK_ESCAPE: /* quit jukebox */
			case SDLK_q:
				quit = true;
				break;
			case SDLK_r: /* restart song */
				JE_jukebox_selectSong(1);
				break;
			case SDLK_n: /* toggle continuous play */
				continuousPlay = !continuousPlay;
				break;
			case SDLK_v:
				volumeActive = !volumeActive;
				break;
			case SDLK_t: /* No idea what this is doing -- possibly resetting to default speed? */
				speed = 0x4300;
				JE_resetTimerInt();
				JE_setTimerInt();
				break;
			case SDLK_f:
				fade = !fade;
				break;
			case SDLK_COMMA: /* dec sound effect */
				fxNum = (fxNum - 1 < 1) ? SOUND_NUM + 9 : fxNum - 1;
				break;
			case SDLK_PERIOD: /* inc sound effect */
				fxNum = (fxNum + 1 > SOUND_NUM + 9) ? 1 : fxNum + 1;
				break;
			case SDLK_SLASH: /* switch to sfx mode */
				fx = !fx;
				break;
			case SDLK_SEMICOLON:
				JE_playSampleNum(fxNum);
				break;
			case SDLK_RETURN:
				currentJukeboxSong++;
				JE_playNewSong();
				youStopped = false;
				break;
			case SDLK_s:
				JE_jukebox_selectSong(0);
				youStopped = true;
				break;
			case SDLK_w:
				if (!weirdMusic)
				{
					weirdMusic = true;
					weirdSpeed = 10;
				}
				else if (weirdSpeed > 1)
				{
					weirdSpeed--;
				}
				else
				{
					weirdMusic = false;
					if (!fade)
					{
						music_vol_multiplier = tempVolume;
					}
				}
				break;
			case SDLK_SPACE:
				drawText = !drawText;
				if (!drawText)
				{
					JE_bar(30, 170, 270, 198, 0);
				}
				break;
			case SDLK_LEFT:
			case SDLK_UP:
				currentJukeboxSong--;
				JE_playNewSong();
				youStopped = false;
				break;
			case SDLK_RIGHT:
			case SDLK_DOWN:
				currentJukeboxSong++;
				JE_playNewSong();
				youStopped = false;
				break;
			default:
				break;
			}
		}
	} while (!quit);
	
	//JE_fadeBlack(10);
	JE_updateColorsFast(black);
}

void JE_newSpeed( void )
{
	/* deprecated; changed music tempo to match star speed */
}

void JE_playNewSong( void )
{
	currentJukeboxSong = (currentJukeboxSong > MUSIC_NUM) ? 1 : (currentJukeboxSong < 1) ? MUSIC_NUM : currentJukeboxSong;
	JE_playSong(currentJukeboxSong);
	playing = true;
	repeatedFade = false;
	tempVolume = CVars::s_music_vol;
}
