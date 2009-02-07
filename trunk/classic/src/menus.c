/* 
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
 * Copyright (C) 2007-2009  The OpenTyrian Development Team
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

#include "config.h"
#include "episodes.h"
#include "fonthand.h"
#include "keyboard.h"
#include "newshape.h"
#include "nortsong.h"
#include "palette.h"
#include "picload.h"
#include "setup.h"
#include "video.h"

char episode_name[6][31], difficulty_name[7][21], gameplay_name[5][26];

bool select_gameplay( void )
{
	JE_loadPic(2, false);
	JE_dString(JE_fontCenter(gameplay_name[0], FONT_SHAPES), 20, gameplay_name[0], FONT_SHAPES);
	
	int gameplay = 1,
	    gameplay_max = 4;
	
	bool fade_in = true;
	for (; ; )
	{
		for (int i = 1; i <= gameplay_max; i++)
		{
			JE_outTextAdjust(JE_fontCenter(gameplay_name[i], SMALL_FONT_SHAPES), i * 24 + 30, gameplay_name[i], 15, - 4 + (i == gameplay ? 2 : 0) - (i == 4 ? 4 : 0), SMALL_FONT_SHAPES, true);
		}
		JE_showVGA();
		
		if (fade_in)
		{
			JE_fadeColor(10);
			fade_in = false;
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
					gameplay = gameplay_max;
				}
				JE_playSampleNum(CURSOR_MOVE);
				break;
			case SDLK_DOWN:
				gameplay++;
				if (gameplay > gameplay_max)
				{
					gameplay = 1;
				}
				JE_playSampleNum(CURSOR_MOVE);
				break;
				
			case SDLK_RETURN:
				if (gameplay == 4)
				{
					JE_playSampleNum(ESC);
					/* TODO: NETWORK */
					printf("error: networking via menu not implemented\n");
					break;
				}
				JE_playSampleNum(SELECT);
				JE_fadeBlack(10);
				
				onePlayerAction = (gameplay == 2);
				twoPlayerMode = (gameplay == 3);
				return true;
				
			case SDLK_ESCAPE:
				JE_playSampleNum(ESC);
				/* fading handled elsewhere
				JE_fadeBlack(10); */
				
				return false;
				
			default:
				break;
			}
		}
	}
	
	return false;
}

bool select_episode( void )
{
	JE_loadPic(2, false);
	JE_dString(JE_fontCenter(episode_name[0], FONT_SHAPES), 20, episode_name[0], FONT_SHAPES);
	
	int episode = 1,
	    episode_max = EPISODE_MAX - 1;
	
	bool fade_in = true;
	for (; ; )
	{
		for (int i = 1; i <= episode_max; i++)
		{
			JE_outTextAdjust(20, i * 30 + 20, episode_name[i], 15, -4 + (i == episode ? 2 : 0) - (!episodeAvail[i - 1] ? 4 : 0), SMALL_FONT_SHAPES, true);
		}
		JE_showVGA();
		
		if (fade_in)
		{
			JE_fadeColor(10);
			fade_in = false;
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
					episode = episode_max;
				}
				JE_playSampleNum(CURSOR_MOVE);
				break;
			case SDLK_DOWN:
				episode++;
				if (episode > episode_max)
				{
					episode = 1;
				}
				JE_playSampleNum(CURSOR_MOVE);
				break;
				
			case SDLK_RETURN:
				if (!episodeAvail[episode - 1])
				{
					JE_playSampleNum(ESC);
					break;
				}
				JE_playSampleNum(SELECT);
				JE_fadeBlack(10);
				
				JE_initEpisode(episode);
				pItems[9-1] = episodeNum;
				return true;
				
			case SDLK_ESCAPE:
				JE_playSampleNum(ESC);
				/* fading handled elsewhere
				JE_fadeBlack(10); */
				
				return false;
				
			default:
				break;
			}
		}
	}
	
	return false;
}

bool select_difficulty( void )
{
	JE_loadPic(2, false);
	JE_dString(JE_fontCenter(difficulty_name[0], FONT_SHAPES), 20, difficulty_name[0], FONT_SHAPES);
	
	difficultyLevel = 2;
	int difficulty_max = 3;
	
	bool fade_in = true;
	for (; ; )
	{
		for (int i = 1; i <= difficulty_max; i++)
		{
			JE_outTextAdjust(JE_fontCenter(difficulty_name[i], SMALL_FONT_SHAPES), i * 24 + 30, difficulty_name[i], 15, -4 + (i == difficultyLevel ? 2 : 0), SMALL_FONT_SHAPES, true);
		}
		JE_showVGA();
		
		if (fade_in)
		{
			JE_fadeColor(10);
			fade_in = false;
		}
		
		JE_word temp = 0;
		JE_textMenuWait(&temp, false);
		
		if (SDL_GetModState() & KMOD_SHIFT)
		{
			if ((difficulty_max < 4 && keysactive[SDLK_g]) ||
			    (difficulty_max == 4 && keysactive[SDLK_RIGHTBRACKET]))
			{
				difficulty_max++;
			}
		} else if (difficulty_max == 5 && keysactive[SDLK_l] && keysactive[SDLK_o] && keysactive[SDLK_r] && keysactive[SDLK_d]) {
			difficulty_max++;
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
				/* fading handled elsewhere
				JE_fadeBlack(10); */
				
				if (difficultyLevel == 6)
				{
					difficultyLevel = 8;
				} else if (difficultyLevel == 5) {
					difficultyLevel = 6;
				}
				return true;
				
			case SDLK_ESCAPE:
				JE_playSampleNum(ESC);
				/* fading handled elsewhere
				JE_fadeBlack(10); */
				
				return false;
				
			default:
				break;
			}
		}
	}
	
	return false;
}

// kate: tab-width 4; vim: set noet:
