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
#include "opentyr.h"
#include "vga256d.h"
#include "keyboard.h"
#include "joystick.h"
#include "nortvars.h"
#include "nortsong.h"
#include "network.h"
#include "starfade.h"
#include "loudness.h"
#include "starlib.h"
#include "fonthand.h"
#include "newshape.h"
#include "config.h"
#include "jukebox.h"

#define NO_EXTERNS
#include "setup.h"
#undef NO_EXTERNS

#include "SDL.h"

JE_boolean volumeActive, fx;
JE_word fxNum;
JE_byte tempVolume;
JE_boolean repeatedFade, continuousPlay;

void JE_textMenuWait( JE_word *waitTime, JE_boolean doGamma )
{
#ifdef NDEBUG /* Disable mouse grabbing/etc in debug builds */
	SDL_WarpMouse(160, 100);
#endif

	do
	{
		JE_showVGA();

		service_SDL_events(TRUE);

		inputDetected = newkey | mousedown | JE_joystickNotHeld();

		if (lastkey_sym == SDLK_SPACE)
		{
			lastkey_sym = SDLK_RETURN;
		}

		if (mousedown)
		{
			newkey = TRUE;
			lastkey_sym = SDLK_RETURN;
		}

#ifdef NDEBUG
		if (mouseInstalled)
		{
			if (abs(mouse_y - 100) > 10)
			{
				inputDetected = TRUE;
				if (mouse_y - 100 < 0)
				{
					lastkey_sym = SDLK_UP;
				} else {
					lastkey_sym = SDLK_DOWN;
				}
				newkey = TRUE;
			}
			if (abs(mouse_x - 160) > 10)
			{
				inputDetected = TRUE;
				if (mouse_x - 160 < 0)
				{
					lastkey_sym = SDLK_LEFT;
				} else {
					lastkey_sym = SDLK_RIGHT;
				}
				newkey = TRUE;
			}
		}
#endif

		if (*waitTime > 0)
		{
			*waitTime--;
		}
	} while (!(inputDetected || *waitTime == 1 || haltGame || netQuit));
}


void JE_jukeboxGo( void )
{
	JE_boolean weirdMusic, weirdCurrent;
	JE_byte weirdSpeed;
	char tempStr[64];
	
	JE_byte lastSong;
	JE_boolean youStopped, drawText, quit, fade;
	
	
	weirdMusic = FALSE;
	weirdCurrent = TRUE;
	drawText = TRUE;
	
	fx = FALSE;
	fxNum = 1;
	
	lastSong = currentJukeboxSong;
	
	/*JE_fadeBlack(10);
	JE_initVGA256();
	JE_fadeColor(10);*/
	
	JE_initVGA256();
	
	quit = FALSE;
	fade = FALSE;
	repeatedFade = FALSE;
	
	tempVolume = tyrMusicVolume;
	youStopped = FALSE;
	
	JE_wipeKey();
	
	do 
	{
		tempScreenSeg = VGAScreen; /*sega000*/
		
		if (weirdMusic)
		{ 
			/*
			IF framecount2 = 0 THEN
			BEGIN
				framecount2 := weirdspeed;

				IF weirdcurrent THEN
					nortsong.setvol (tempvolume SHR 1, FXvolume)
				ELSE
					nortsong.setvol (tempvolume, FXvolume);

				weirdcurrent := NOT weirdcurrent;
			END;
			*/
		}
      
		if (repeated && !repeatedFade)
		{
		  fade = TRUE;
		  repeatedFade = TRUE;
		}

		if ( ( (repeated && !fade) || !playing) && !youStopped)
		{
		  currentSong = ( rand() % MUSIC_NUM );
		  JE_playNewSong();
		}

		frameCount = 1;
		/*k = 0;
		scanCode = 0;*/

		/* JE: {STARFIELD processing} */

		/* starlib.main; */

		if (lastSong != currentSong)
		{
		  lastSong = currentSong;
		  JE_bar(50, 190, 250, 198, 0); /* vga256c.BAR (50, 190, 250, 198, 0); */
		}

		if (drawText)
		{
			/* TODO: Put in actual song titles here */
			if (fx)
			{
				sprintf(tempStr, "%d %s", fxNum, soundTitle[fxNum - 1]);
				JE_bar(50, 190, 250, 198, 0); /* vga256c.BAR (50, 190, 250, 198, 0); */
				JE_outText(JE_fontCenter(tempStr, TINY_FONT), 190, tempStr, 1, 4);
			} else {
				sprintf(tempStr, "%d %s", currentJukeboxSong, musicTitle[currentJukeboxSong - 1]);
				JE_outText(JE_fontCenter(tempStr, TINY_FONT), 190, tempStr, 1, 4);
			}
		}
	
		/* TODO: figure out what this loop was about, and where the value of frameCount would have been changed. It doesn't work as-is. */
		/*do
		{*/
			if (drawText)
			{
				tempScreenSeg = VGAScreen; /*sega000*/
				JE_outText(JE_fontCenter("Press ESC to quit the jukebox.", TINY_FONT), 170, "Press ESC to quit the jukebox.", 1, 0);
				tempScreenSeg = VGAScreen; /*sega000*/
				JE_outText(JE_fontCenter("Arrow keys change the song being played.", TINY_FONT), 180, "Arrow keys change the song being played.", 1, 0);
			}
		/*} while (frameCount != 0);*/
		
		JE_showVGA();
		
		if (fade)
		{
		/*
          IF volumeactive THEN
            IF tempvolume > 5 THEN
              BEGIN
                DEC (tempvolume, 1);
                nortsong.setvol (tempvolume, FXvolume)
              END 
            ELSE
              BEGIN
                fade := FALSE;
              END
          ELSE
          IF vga256d.speed < $E000 THEN
            INC (vga256d.speed, $800) 
          ELSE
            BEGIN
              vga256d.speed := $E000;
              fade := FALSE;
            END;
          resettimerint;
          nortsong.settimerint;
        */
		}
      
		/*
      joystick2;
      IF (mouseposition (x, y) > 0) OR button [1] THEN
        BEGIN
          quit := TRUE;
          wipekey;
        END;
      */
		
		JE_showVGA();
	
		tempw = 0;
		JE_textMenuWait(&tempw, FALSE);
	
		if (newkey) {
			JE_newSpeed();
			
			switch (lastkey_sym)
			{
			case SDLK_ESCAPE: /* quit jukebox */
			case SDLK_q:
				quit = TRUE;
				break;
			case SDLK_r: /* restart song */
				JE_selectSong(1);
				break;
			case SDLK_n: /* toggle continuous play */
				continuousPlay = !continuousPlay;
				break;
			case SDLK_v:
				volumeActive = !volumeActive;
				break;
			case SDLK_t: /* No idea what this is doing :( */
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
/*
            #13 : BEGIN
                    INC (currentsong);
                    playnewsong;
                    youstopped := FALSE;
                  END;
*/			
			case SDLK_s:
				JE_selectSong(0);
				youStopped = TRUE;
				break;
		/*
          CASE UPCASE (k) OF
            'W' : IF NOT weirdmusic THEN
                    BEGIN
                      weirdmusic := TRUE;
                      weirdspeed := 10;
                    END 
                  ELSE
                  IF weirdspeed > 1 THEN
                    DEC (weirdspeed) 
                  ELSE
                    BEGIN
                      weirdmusic := FALSE;
                      IF NOT fade THEN
                        nortsong.setvol (tempvolume, FXvolume);
                    END;
            ' ' : BEGIN
                    drawtext := NOT drawtext;
                    IF NOT drawtext THEN
                      vga256c.BAR (30, 170, 270, 198, 0);
                  END;
          END;
          
          CASE scancode OF
            75, 72 : BEGIN
                       DEC (currentsong);
                       playnewsong;
                       youstopped := FALSE;
                     END;
            77, 80 : BEGIN
                       INC (currentsong);
                       playnewsong;
                       youstopped := FALSE;
                     END;
          END;
		*/
			case SDLK_LEFT:
			case SDLK_UP:
				currentJukeboxSong--;
				JE_playNewSong();
				youStopped = FALSE;
				break;
			case SDLK_RIGHT:
			case SDLK_DOWN:
				currentJukeboxSong++;
				JE_playNewSong();
				youStopped = FALSE;			
				break;
			default:
				break;
			}
		}
	} while (!quit);

	JE_fadeBlack(10);
}

void JE_newSpeed( void )
{
		/*
          vga256d.speed := $5300 - starlib.speed * $800;
          IF starlib.speed > 5 THEN
            vga256d.speed := $1000;
          resettimerint;
          nortsong.settimerint;
        */
}

void JE_playNewSong( void )
{
	currentJukeboxSong = (currentJukeboxSong > MUSIC_NUM) ? 1 : (currentJukeboxSong < 1) ? MUSIC_NUM : currentJukeboxSong;
	JE_playSong(currentJukeboxSong);
	playing = TRUE;
	repeatedFade = FALSE;
	tempVolume = tyrMusicVolume;
	JE_setVol(tempVolume, fxVolume);
}



/* TODO */
