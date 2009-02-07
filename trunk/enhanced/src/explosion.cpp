/* vim: set noet:
 *
 * OpenTyrian Enhanced: A modern cross-platform port of Tyrian
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
#include "explosion.h"

#include "backgrnd.h"
#include "mainint.h"
#include "varz.h"
#include "mtrand.h"

static const int MAX_EXPLOSIONS = 200;
static const int MAX_REPEATING_EXPLOSIONS = 20;

struct Explosion
{
	unsigned int life;
	int x, y;
	int delta_x, delta_y;
	bool fixed_position;
	unsigned char follow_player;
	unsigned int sprite;
};

struct RepeatingExplosion
{
	unsigned int delay;
	unsigned int life;
	int x, y;
	bool big;
};	

static Explosion explosions[MAX_EXPLOSIONS];
static RepeatingExplosion rep_explosions[MAX_REPEATING_EXPLOSIONS];

bool enemyStillExploding;

void init_explosions( )
{
	std::fill_n(explosions, MAX_EXPLOSIONS, Explosion());
	std::fill_n(rep_explosions, MAX_REPEATING_EXPLOSIONS, RepeatingExplosion());
}

void draw_explosions( )
{
	/*-------------------------- Sequenced Explosions -------------------------*/
	enemyStillExploding = false;
	for (int i = 0; i < MAX_REPEATING_EXPLOSIONS; i++)
	{
		if (rep_explosions[i].life > 0)
		{
			enemyStillExploding = true;
			if (rep_explosions[i].delay > 0)
			{
				rep_explosions[i].delay--;
				continue;
			}
			
			rep_explosions[i].y += backMove2 + 1;
			int explosion_x = rep_explosions[i].x + (mt::rand() % 24) - 12;
			int explosion_y = rep_explosions[i].y + (mt::rand() % 27) - 24;

			if (rep_explosions[i].big)
			{
				JE_setupExplosionLarge(false, 2, explosion_x, explosion_y);
				if (rep_explosions[i].life == 1 || mt::rand() % 5 == 1)
				{
					soundQueue[7] = 11;
				} else {
					soundQueue[6] = 9;
				}
				rep_explosions[i].delay = 4 + (mt::rand() % 3);
			} else {
				JE_setupExplosion(explosion_x, explosion_y, 1);
				soundQueue[5] = 4;
				rep_explosions[i].delay = 3;
			}
			rep_explosions[i].life--;
		}
	}

	/*---------------------------- Draw Explosions ----------------------------*/
	for (int j = 0; j < MAX_EXPLOSIONS; j++)
	{
		if (explosions[j].life > 0)
		{
			if (!explosions[j].fixed_position)
			{
				explosions[j].sprite++;
				explosions[j].y += explodeMove;
			} else if (explosions[j].follow_player > 0) {
				unsigned int player = explosions[j].follow_player - 1;
				explosions[j].x += player_delta_x[player];
				explosions[j].y += player_delta_y[player];
			}

			explosions[j].x += explosions[j].delta_x;
			explosions[j].y += explosions[j].delta_y;
			
			Uint8* s = VGAScreen; // screen pointer, 8-bit specific
			s += explosions[j].y * scr_width + explosions[j].x;
			
			Uint8* s_limit = VGAScreen + scr_width*scr_height;
			
			if (s + scr_width * 14 > s_limit)
			{
				explosions[j].life = 0;
			} else {
				Uint8* p = shapes6;
				p += SDL_SwapLE16(((JE_word *)p)[explosions[j].sprite]);
				
				if (CVars::r_explosion_blend)
				{
					while (*p != 0x0f)
					{
						s += *p & 0x0f;
						int i = (*p & 0xf0) >> 4;
						if (i)
						{
							while (i--)
							{
								p++;
								if (s >= s_limit)
									goto explosion_draw_overflow;
								if ((void *)s >= VGAScreen)
									*s = (((*p & 0x0f) + (*s & 0x0f)) >> 1) | (*p & 0xf0);
								s++;
							}
						} else {
							s -= 12;
							s += scr_width;
						}
						p++;
					}
				} else {
					while (*p != 0x0f)
					{
						s += *p & 0x0f;
						int i = (*p & 0xf0) >> 4;
						if (i)
						{
							while (i--)
							{
								p++;
								if (s >= s_limit)
									goto explosion_draw_overflow;
								if ((void *)s >= VGAScreen)
									*s = *p;
								s++;
							}
						} else {
							s -= 12;
							s += scr_width;
						}
						p++;
					}
				}
explosion_draw_overflow:
				explosions[j].life--;
			}
		}
	}
}

void JE_setupExplosion( int x, int y, unsigned int type, int delta_y, bool fixed_position, unsigned int follow_player )
{
	assert(follow_player <= 2);

	const struct {
		JE_word sprite;
		int life;
	} explosion_data[53] = {
		{ 144,  7 },
		{ 120, 12 },
		{ 190, 12 },
		{ 209, 12 },
		{ 152, 12 },
		{ 171, 12 },
		{ 133,  7 },   /*White Smoke*/
		{   1, 12 },
		{  20, 12 },
		{  39, 12 },
		{  58, 12 },
		{ 110,  3 },
		{  76,  7 },
		{  91,  3 },
/*15*/	{ 227,  3 },
		{ 230,  3 },
		{ 233,  3 },
		{ 252,  3 },
		{ 246,  3 },
/*20*/	{ 249,  3 },
		{ 265,  3 },
		{ 268,  3 },
		{ 271,  3 },
		{ 236,  3 },
/*25*/	{ 239,  3 },
		{ 242,  3 },
		{ 261,  3 },
		{ 274,  3 },
		{ 277,  3 },
/*30*/	{ 280,  3 },
		{ 299,  3 },
		{ 284,  3 },
		{ 287,  3 },
		{ 290,  3 },
/*35*/	{ 293,  3 },
		{ 165,  8 },   /*Coin Values*/
		{ 184,  8 },
		{ 203,  8 },
		{ 222,  8 },
		{ 168,  8 },
		{ 187,  8 },
		{ 206,  8 },
		{ 225, 10 },
		{ 169, 10 },
		{ 188, 10 },
		{ 207, 20 },
		{ 226, 14 },
		{ 170, 14 },
		{ 189, 14 },
		{ 208, 14 },
		{ 246, 14 },
		{ 227, 14 },
		{ 265, 14 }
	};
	
	if (y > -16 && y < 190)
	{
		for (int i = 0; i < MAX_EXPLOSIONS; i++)
		{
			if (explosions[i].life == 0)
			{
				explosions[i].x = x;
				explosions[i].y = y;

				if (type == 6)
				{
					explosions[i].x += 2;
					explosions[i].y += 12;
				} else if (type == 98) {
					type = 6;
				}

				explosions[i].sprite = explosion_data[type].sprite;
				explosions[i].life = explosion_data[type].life;
				explosions[i].follow_player = follow_player;
				explosions[i].fixed_position = fixed_position;
				explosions[i].delta_x = 0;
				explosions[i].delta_y = delta_y;
				break;
			}
		}
	}
}

void JE_setupExplosionLarge( bool enemyGround, int exploNum, int x, int y )
{
	if (y >= 0)
	{
		if (enemyGround)
		{
			JE_setupExplosion(x-6, y-14, 2);
			JE_setupExplosion(x+6, y-14, 4);
			JE_setupExplosion(x-6, y,    3);
			JE_setupExplosion(x+6, y,    5);
		} else {
			JE_setupExplosion(x-6, y-14, 7);
			JE_setupExplosion(x+6, y-14, 9);
			JE_setupExplosion(x-6, y,    8);
			JE_setupExplosion(x+6, y,   10);
		}
		
		if (exploNum > 10)
		{
			exploNum -= 10;
			tempB = true;
		} else {
			tempB = false;
		}
		
		if (exploNum > 0)
		{
			for (int i = 0; i < MAX_REPEATING_EXPLOSIONS; i++)
			{
				if (rep_explosions[i].life == 0)
				{
					rep_explosions[i].life = exploNum;
					rep_explosions[i].delay = 2;
					rep_explosions[i].x = x;
					rep_explosions[i].y = y;
					rep_explosions[i].big = tempB;
					break;
				}
			}
		}
	}
}
