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
#include "menus.h"

#include "picload.h"
#include "helptext.h"
#include "newshape.h"
#include "fonthand.h"
#include "palette.h"
#include "video.h"
#include "setup.h"
#include "keyboard.h"
#include "sndmast.h"
#include "nortsong.h"
#include "config.h"
#include "episodes.h"

bool select_gameplay( )
{
	JE_loadPic(2, false);
	JE_dString(JE_fontCenter(playerName[0], FONT_SHAPES), 20, playerName[0], FONT_SHAPES);

	unsigned int gameplay = 1;
	static const unsigned int GAMEPLAY_MAX = 4;

	bool fade_in = true;
	while (true)
	{
		for (unsigned int i = 1; i <= GAMEPLAY_MAX; ++i)
		{
			JE_outTextAdjust(JE_fontCenter(playerName[i], SMALL_FONT_SHAPES), i*24 + 30, playerName[i], 15, -4 + (i == gameplay ? 2 : 0) - (i == 4 ? 4 : 0), SMALL_FONT_SHAPES, true);
		}

		if (fade_in)
		{
			JE_fadeColor(10);
			fade_in = false;
		}
		else
		{
			JE_showVGA();
		}

		JE_word temp = 0;
		JE_textMenuWait(&temp, false);

		if (newkey)
		{
			switch (lastkey_sym)
			{
			case SDLK_UP:
				gameplay--;
				if (gameplay < 1)
				{
					gameplay = GAMEPLAY_MAX;
				}
				JE_playSampleNum(CURSOR_MOVE);
				break;
			case SDLK_DOWN:
				gameplay++;
				if (gameplay > GAMEPLAY_MAX)
				{
					gameplay = 1;
				}
				JE_playSampleNum(CURSOR_MOVE);
				break;
			case SDLK_RETURN:
				if (gameplay == 4)
				{
					JE_playSampleNum(ESC);
					// TODO: NETWORK
					Console::get() << "\a7Error:\ax Networking not implemented yet." << std::endl;
				} else {
					JE_playSampleNum(SELECT);
					JE_fadeBlack(10);

					onePlayerAction = (gameplay == 2);
					twoPlayerMode = (gameplay == 3);
					return true;
				}
				break;
			case SDLK_ESCAPE:
				JE_playSampleNum(ESC);
				// Fading handles elsewhere
				// JE_fadeBlack(10);

				return false;
				break;
			default:
				break;
			}
		}
	}
}

bool select_episode( )
{
	JE_loadPic(2, false);
	JE_dString(JE_fontCenter(episodeName[0], FONT_SHAPES), 20, episodeName[0], FONT_SHAPES);

	unsigned int episode = 1;
	static const unsigned int EPISODE_MAX = 4;

	bool fade_in = true;
	while (true)
	{
		for (unsigned int i = 1; i <= EPISODE_MAX; ++i)
		{
			JE_outTextAdjust(20, i*30 + 20, episodeName[i], 15, -4 + (i == episode ? 2 : 0) - (!episodeAvail[i-1] ? 4 : 0), SMALL_FONT_SHAPES, true);
		}
		JE_showVGA();

		if (fade_in)
		{
			JE_fadeColor(10);
			fade_in = false;
		}
		else
		{
			JE_showVGA();
		}

		JE_word temp = 0;
		JE_textMenuWait(&temp, false);

		if (newkey)
		{
			switch (lastkey_sym)
			{
			case SDLK_UP:
				episode--;
				if (episode < 1)
				{
					episode = EPISODE_MAX;
				}
				JE_playSampleNum(CURSOR_MOVE);
				break;
			case SDLK_DOWN:
				episode++;
				if (episode > EPISODE_MAX)
				{
					episode = 1;
				}
				JE_playSampleNum(CURSOR_MOVE);
				break;
			case SDLK_RETURN:
				if (!episodeAvail[episode-1])
				{
					JE_playSampleNum(ESC);
				}
				else
				{
					JE_playSampleNum(SELECT);
					JE_fadeBlack(10);

					JE_initEpisode(episode);
					pItems[PITEM_EPISODE] = episodeNum;
					return true;
				}
				break;
			case SDLK_ESCAPE:
				JE_playSampleNum(ESC);
				// Fading handled elsewhere
				// JE_fadeBlack(10);

				return false;
				break;
			default:
				break;
			}
		}
	}
}

bool select_difficulty( )
{
	JE_loadPic(2, false);
	JE_dString(JE_fontCenter(difficultyName[0], FONT_SHAPES), 20, difficultyName[0], FONT_SHAPES);

	difficultyLevel = 2;
	unsigned int difficulty_max = 3;

	bool fade_in = true;
	while (true)
	{
		for (unsigned int i = 1; i <= difficulty_max; ++i)
		{
			JE_outTextAdjust(JE_fontCenter(difficultyName[i], SMALL_FONT_SHAPES), i*24 + 30, difficultyName[i], 15, -4 + (i == difficultyLevel ? 2 : 0), SMALL_FONT_SHAPES, true);
		}

		if (fade_in)
		{
			JE_fadeColor(10);
			fade_in = false;
		}
		else
		{
			JE_showVGA();
		}

		JE_word temp = 0;
		JE_textMenuWait(&temp, false);

		// Check for secret difficulties
		if (SDL_GetModState() & KMOD_SHIFT)
		{
			if (keysactive[SDLK_g] && difficulty_max < 4)
			{
				difficulty_max = 4;
			}
			else if (keysactive[SDLK_RIGHTBRACKET] && difficulty_max < 5)
			{
				difficulty_max = 5;
			}
		}
		else if (difficulty_max == 5 && keysactive[SDLK_l] && keysactive[SDLK_o] && keysactive[SDLK_r] && keysactive[SDLK_d])
		{
			difficulty_max = 6;
		}

		if (newkey)
		{
			switch (lastkey_sym)
			{
			case SDLK_UP:
				difficultyLevel--;
				if (difficultyLevel < 1)
				{
					difficultyLevel = difficulty_max;
				}
				JE_playSampleNum(CURSOR_MOVE);
				break;
			case SDLK_DOWN:
				difficultyLevel++;
				if (difficultyLevel > difficulty_max)
				{
					difficultyLevel = 1;
				}
				JE_playSampleNum(CURSOR_MOVE);
				break;
			case SDLK_RETURN:
				JE_playSampleNum(SELECT);
				// Fading handled elsewhere
				// JE_fadeBlack(10);

				if (difficultyLevel == 6)
				{
					difficultyLevel = 8;
				}
				else if (difficultyLevel == 5)
				{
					difficultyLevel = 6;
				}

				return true;
				break;
			case SDLK_ESCAPE:
				JE_playSampleNum(ESC);
				// Fading handled elsewhere
				// JE_fadeBlack(10);

				return false;
				break;
			default:
				break;
			}
		}
	}
}
