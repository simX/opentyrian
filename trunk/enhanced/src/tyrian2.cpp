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
#include "tyrian2.h"

#include "animlib.h"
#include "backgrnd.h"
#include "destruct.h"
#include "episodes.h"
#include "explosion.h"
#include "fonthand.h"
#include "console/GameActions.h"
#include "helptext.h"
#include "HighScores.h"
#include "joystick.h"
#include "keyboard.h"
#include "loudness.h"
#include "loudness.h"
#include "lvlmast.h"
#include "mainint.h"
#include "menus.h"
#include "newshape.h"
#include "nortsong.h"
#include "params.h"
#include "pcxload.h"
#include "picload.h"
#include "setup.h"
#include "sndmast.h"
#include "superpixel.h"
#include "varz.h"
#include "vga256d.h"
#include "Filesystem.h"
#include "network.h"
#include "itemscreen.h"

#include <cctype>
#include "boost/lexical_cast.hpp"
#include "boost/format.hpp"

using boost::lexical_cast;

JE_word statDmg[2]; /* [1..2] */

/* Level Event Data */
bool quit;
bool jumpSection;

static bool loadLevelOk;

static const int EVENT_MAXIMUM = 2500;
static JE_EventRecType eventRec[EVENT_MAXIMUM]; /* [1..eventMaximum] */
static JE_word levelEnemyMax;
static JE_word levelEnemyFrequency;
static JE_word levelEnemy[40]; /* [1..40] */

static bool musicFade;

static const char shapeFile[34] = /* [1..34] */
{
	'2', '4', '7', '8', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', '5', '#', 'V', '0', '@', '3', '^', '5', '9'
};

void JE_makeEnemy(JE_SingleEnemyType *enemy);
void JE_loadMap();
void JE_eventSystem();
void JE_eventJump(JE_word jump);
void JE_doStatBar();
void JE_titleScreen(bool animate);
void JE_openingAnim();
void JE_displayText();
void JE_readTextSync();
void JE_whoa();

void JE_starShowVGA( void )
{
	Uint8 *src;
	Uint8 *s = NULL; /* screen pointer, 8-bit specific */

	int x, y, lightx, lighty, lightdist;
	
	if (!playerEndLevel && !skipStarShowVGA)
	{

		s = VGAScreenSeg;

		src = game_screen;
		src += 24;

		if (fastPlay != 2 /*&& thisPlayerNum != 2*/)
		{
			int delaycount_temp;
			if ((delaycount_temp = target - SDL_GetTicks()) > 0)
				SDL_Delay(delaycount_temp);
			setjasondelay(frameCountMax);
		}

		if (starShowVGASpecialCode == 1)
		{
			src += scr_width * 183;
			for (y = 0; y < 184; y++)
			{
				memmove(s, src, 264);
				s += scr_width;
				src -= scr_width;
			}
		} else if (starShowVGASpecialCode == 2 && CVars::r_headlight) {
			lighty = 172 - PY;
			lightx = 281 - PX;
			
			for (y = 184; y; y--)
			{
				if (lighty > y)
				{
					for (x = 320 - 56; x; x--)
					{
						*s = (*src & 0xf0) | ((*src >> 2) & 0x03);
						s++;
						src++;
					}
				} else {
					for (x = 320 - 56; x; x--)
					{
						lightdist = abs(lightx - x) + lighty;
						if (lightdist < y)
						{
							*s = *src;
						} else if (lightdist - y <= 5) {
							*s = (*src & 0xf0) | (((*src & 0x0f) + (3 * (5 - (lightdist - y)))) / 4);
						} else {
							*s = (*src & 0xf0) | ((*src & 0x0f) >> 2);
						}
						s++;
						src++;
					}
				}
				s += 56 + scr_width - 320;
				src += 56 + scr_width - 320;
			}
		} else {
			for (y = 0; y < 184; y++)
			{
				memmove(s, src, 264);
				s += scr_width;
				src += scr_width;
			}
		}
		JE_showVGA();
	}

	quitRequested = false;
	skipStarShowVGA = false;
}

void JE_newEnemy( int enemyOffset )
{
	int i;

	b = 0;

	for(i = enemyOffset; i < enemyOffset + 25; i++)
	{
		if (enemyAvail[i] == 1)
		{
			b = i + 1;
			break;
		}
	}

	if (b == 0)
	{
		return;
	}

	JE_makeEnemy(&enemy[b-1]);
	enemyAvail[b-1] = a;
}

void JE_drawEnemy( int enemyOffset )
{
	int i, j;

	Uint8 *p; /* shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	PX -= 25;

	for (i = enemyOffset - 25; i < enemyOffset; i++)
	{
		if (enemyAvail[i] != 1)
		{
			enemy[i].mapoffset = tempMapXOfs;

			if (enemy[i].xaccel && enemy[i].xaccel - 89 > (signed)(mt::rand() % 11))
			{
				if (PX > enemy[i].ex)
				{
					if (enemy[i].exc < enemy[i].xaccel - 89)
					{
						enemy[i].exc++;
					}
				} else {
					if (enemy[i].exc >= 0 || -enemy[i].exc < enemy[i].xaccel - 89)
					{
						enemy[i].exc--;
					}
				}
			}

			if (enemy[i].yaccel && enemy[i].yaccel - 89 > (signed)(mt::rand() % 11))
			{
				if (PY > enemy[i].ey)
				{
					if (enemy[i].eyc < enemy[i].yaccel - 89)
					{
						enemy[i].eyc++;
					}
				} else {
					if (enemy[i].eyc >= 0 || -enemy[i].eyc < enemy[i].yaccel - 89)
					{
						enemy[i].eyc--;
					}
				}
			}

 			if (enemy[i].ex + tempMapXOfs > -29 && enemy[i].ex + tempMapXOfs < 300)
			{
				if (enemy[i].aniactive == 1)
				{
					enemy[i].enemycycle++;

					if (enemy[i].enemycycle == enemy[i].animax)
					{
						enemy[i].aniactive = enemy[i].aniwhenfire;
					} else {
						if (enemy[i].enemycycle > enemy[i].ani)
						{
							enemy[i].enemycycle = enemy[i].animin;
						}
					}
				}

				if (enemy[i].egr[enemy[i].enemycycle - 1] == 999)
				{
					goto enemy_gone;
				}

				if (enemy[i].size == 1)
				{
					/* -------------------------  LARGE ENEMY -- Draw a 2x2 enemy ---------- */

					if (enemy[i].ey > -13)
					{
						s = (Uint8 *)VGAScreen;
						s += (enemy[i].ey - 7) * scr_width + (enemy[i].ex - 6) + tempMapXOfs;

						s_limit = (Uint8 *)VGAScreen;
						s_limit += scr_height * scr_width;

						p = (Uint8 *)enemy[i].shapeseg;
						p += SDL_SwapLE16(((JE_word *)p)[enemy[i].egr[enemy[i].enemycycle - 1] - 1]);

						while (*p != 0x0f)
						{
							s += *p & 0x0f;
							j = (*p & 0xf0) >> 4;
							if (j)
							{
								while (j--)
								{
									p++;
									if (s >= s_limit)
										goto enemy_draw_overflow;
									if ((void *)s >= VGAScreen)
										*s = (enemy[i].filter == 0) ? *p : (*p & 0x0f) | enemy[i].filter;
									s++;
								}
							} else {
								s -= 12;
								s += scr_width;
							}
							p++;
						}

						s = (Uint8 *)VGAScreen;
						s += (enemy[i].ey - 7) * scr_width + (enemy[i].ex + 6) + tempMapXOfs;

						p = (Uint8 *)enemy[i].shapeseg;
						p += SDL_SwapLE16(((JE_word *)p)[enemy[i].egr[enemy[i].enemycycle - 1]]);

						while (*p != 0x0f)
						{
							s += *p & 0x0f;
							j = (*p & 0xf0) >> 4;
							if (j)
							{
								while (j--)
								{
									p++;
									if (s >= s_limit)
										goto enemy_draw_overflow;
									if ((void *)s >= VGAScreen)
										*s = (enemy[i].filter == 0) ? *p : (*p & 0x0f) | enemy[i].filter;
									s++;
								}
							} else {
								s -= 12;
								s += scr_width;
							}
							p++;
						}
					}

					if (enemy[i].ey > -26 && enemy[i].ey < 182)
					{
						s = (Uint8 *)VGAScreen;
						s += (enemy[i].ey + 7) * scr_width + (enemy[i].ex - 6) + tempMapXOfs;

						s_limit = (Uint8 *)VGAScreen;
						s_limit += scr_height * scr_width;

						p = (Uint8 *)enemy[i].shapeseg;
						p += SDL_SwapLE16(((JE_word *)p)[enemy[i].egr[enemy[i].enemycycle - 1] + 18]);

						while (*p != 0x0f)
						{
							s += *p & 0x0f;
							j = (*p & 0xf0) >> 4;
							if (j)
							{
								while (j--)
								{
									p++;
									if (s >= s_limit)
										goto enemy_draw_overflow;
									if ((void *)s >= VGAScreen)
										*s = (enemy[i].filter == 0) ? *p : (*p & 0x0f) | enemy[i].filter;
									s++;
								}
							} else {
								s -= 12;
								s += scr_width;
							}
							p++;
						}

						s = (Uint8 *)VGAScreen;
						s += (enemy[i].ey + 7) * scr_width + (enemy[i].ex + 6) + tempMapXOfs;

						p = (Uint8 *)enemy[i].shapeseg;
						p += SDL_SwapLE16(((JE_word *)p)[enemy[i].egr[enemy[i].enemycycle - 1] + 19]);

						while (*p != 0x0f)
						{
							s += *p & 0x0f;
							j = (*p & 0xf0) >> 4;
							if (j)
							{
								while (j--)
								{
									p++;
									if (s >= s_limit)
										goto enemy_draw_overflow;
									if ((void *)s >= VGAScreen)
										*s = (enemy[i].filter == 0) ? *p : (*p & 0x0f) | enemy[i].filter;
									s++;
								}
							} else {
								s -= 12;
								s += scr_width;
							}
							p++;
						}
					}

				} else {
					if (enemy[i].ey > -13)
					{
						s = (Uint8 *)VGAScreen;
						s += enemy[i].ey * scr_width + enemy[i].ex + tempMapXOfs;

						s_limit = (Uint8 *)VGAScreen;
						s_limit += scr_height * scr_width;

						p = (Uint8 *)enemy[i].shapeseg;
						p += SDL_SwapLE16(((JE_word *)p)[enemy[i].egr[enemy[i].enemycycle - 1] - 1]);

						while (*p != 0x0f)
						{
							s += *p & 0x0f;
							j = (*p & 0xf0) >> 4;
							if (j)
							{
								while (j--)
								{
									p++;
									if (s >= s_limit)
										goto enemy_draw_overflow;
									if ((void *)s >= VGAScreen)
										*s = (enemy[i].filter == 0) ? *p : (*p & 0x0f) | enemy[i].filter;
									s++;
								}
							} else {
								s -= 12;
								s += scr_width;
							}
							p++;
						}
					}
				}
enemy_draw_overflow:
				enemy[i].filter = 0;
			}

			if (enemy[i].excc)
			{
				if (--enemy[i].exccw <= 0)
				{
					if (enemy[i].exc == enemy[i].exrev)
					{
						enemy[i].excc = -enemy[i].excc;
						enemy[i].exrev = -enemy[i].exrev;
						enemy[i].exccadd = -enemy[i].exccadd;
					} else {
						enemy[i].exc += enemy[i].exccadd;
						enemy[i].exccw = enemy[i].exccwmax;
						if (enemy[i].exc == enemy[i].exrev)
						{
							enemy[i].excc = -enemy[i].excc;
							enemy[i].exrev = -enemy[i].exrev;
							enemy[i].exccadd = -enemy[i].exccadd;
						}
					}
				}
			}

			if (enemy[i].eycc)
			{
				if (--enemy[i].eyccw <= 0)
				{
					if (enemy[i].eyc == enemy[i].eyrev)
					{
						enemy[i].eycc = -enemy[i].eycc;
						enemy[i].eyrev = -enemy[i].eyrev;
						enemy[i].eyccadd = -enemy[i].eyccadd;
					} else {
						enemy[i].eyc += enemy[i].eyccadd;
						enemy[i].eyccw = enemy[i].eyccwmax;
						if (enemy[i].eyc == enemy[i].eyrev)
						{
							enemy[i].eycc = -enemy[i].eycc;
							enemy[i].eyrev = -enemy[i].eyrev;
							enemy[i].eyccadd = -enemy[i].eyccadd;
						}
					}
				}
			}

			enemy[i].ey += enemy[i].fixedmovey;

			enemy[i].ex += enemy[i].exc;
			if (enemy[i].ex < -80 || enemy[i].ex > 340)
			{
				goto enemy_gone;
			}

			enemy[i].ey += enemy[i].eyc;
			if (enemy[i].ey < -112 || enemy[i].ey > 190)
			{
				goto enemy_gone;
			}

			goto enemy_still_exists;

enemy_gone:
			/* enemy[i].egr[10] &= 0x00ff; <MXD> madness? */
			enemyAvail[i] = 1;
			goto draw_enemy_end;

enemy_still_exists:

			/*X bounce*/
			if (enemy[i].ex <= enemy[i].xminbounce || enemy[i].ex >= enemy[i].xmaxbounce)
			{
				enemy[i].exc = -enemy[i].exc;
			}

			/*Y bounce*/
			if (enemy[i].ey <= enemy[i].yminbounce || enemy[i].ey >= enemy[i].ymaxbounce)
			{
				enemy[i].eyc = -enemy[i].eyc;
			}

			/* Evalue != 0 - score item at boundary */
			if (enemy[i].scoreitem)
			{
				if (enemy[i].ex < -5)
				{
					enemy[i].ex++;
				}
				if (enemy[i].ex > 245)
				{
					enemy[i].ex--;
				}
			}

			enemy[i].ey += tempBackMove;

			if (enemy[i].ex <= -24 || enemy[i].ex >= 296)
				goto draw_enemy_end;

			tempX = enemy[i].ex;
			tempY = enemy[i].ey;

			temp = enemy[i].enemytype;

			/* Enemy Shots */
			if (enemy[i].edamaged == 1)
				goto draw_enemy_end;

			enemyOnScreen++;

			if (enemy[i].iced)
			{
				enemy[i].iced--;
				if (enemy[i].enemyground != 0)
				{
					enemy[i].filter = 9;
				}
				goto draw_enemy_end;
			}

			for (j = 3; j > 0; j--)
			{
				if (enemy[i].freq[j-1])
				{
					temp3 = enemy[i].tur[j-1];

					if (--enemy[i].eshotwait[j-1] == 0 && temp3)
					{
						enemy[i].eshotwait[j-1] = enemy[i].freq[j-1];
						if (difficultyLevel > 2)
						{
							enemy[i].eshotwait[j-1] = (enemy[i].eshotwait[j-1] / 2) + 1;
							if (difficultyLevel > 7)
							{
								enemy[i].eshotwait[j-1] = (enemy[i].eshotwait[j-1] / 2) + 1;
							}
						}

						if (galagaMode && (enemy[i].eyc == 0 || (mt::rand() % 400) >= galagaShotFreq))
							goto draw_enemy_end;

						switch (temp3)
						{
						case 252: // Savara Boss DualMissile
							if (enemy[i].ey > 20)
							{
								JE_setupExplosion(tempX-8+tempMapXOfs, tempY-20 - backMove*8, 6, -2);
								JE_setupExplosion(tempX+4+tempMapXOfs, tempY-20 - backMove*8, 6, -2);
							}
							break;
						case 251: // Suck-O-Magnet
							tempI4 = 4 - (abs(PX - tempX) + abs(PY - tempY)) / 100;
							if (PX > tempX)
							{
								lastTurn2 -= tempI4;
							} else {
								lastTurn2 += tempI4;
							}
							break;
						case 253: // Left ShortRange Magnet
							if (abs(PX + 25 - 14 - tempX) < 24 && abs(PY - tempY) < 28)
							{
								lastTurn2 += 2;
							}
							if (twoPlayerMode &&
							   (abs(PXB - 14 - tempX) < 24 && abs(PYB - tempY) < 28))
							{
								lastTurn2B += 2;
							}
							break;
						case 254: // Left ShortRange Magnet
							if (abs(PX + 25 - 14 - tempX) < 24 && abs(PY - tempY) < 28)
							{
								lastTurn2 -= 2;
							}
							if (twoPlayerMode &&
							   (abs(PXB - 14 - tempX) < 24 && abs(PYB - tempY) < 28))
							{
								lastTurn2B -= 2;
							}
							break;
						case 255: // Magneto RePulse!!
							if (difficultyLevel != 1) /*DIF*/
							{
								if (j == 3)
								{
									enemy[i].filter = 112;
								} else {
									tempI4 = 4 - (abs(PX - tempX) + abs(PY - tempY)) / 20;
									if (tempI4 > 0)
									{
										if (PX > tempX)
										{
											lastTurn2 += tempI4;
										} else {
											lastTurn2 -= tempI4;
										}
									}
								}
							}
							break;
						default: // Rot
							for (tempCount = weapons[temp3].multi; tempCount > 0; tempCount--)
							{
								for (b = 0; b < ENEMY_SHOT_MAX; b++)
								{
									if (enemyShotAvail[b] == 1)
									{
										break;
									}
								}
								if (b == ENEMY_SHOT_MAX)
								{
									goto draw_enemy_end;
								}

								enemyShotAvail[b] = false;

								if (weapons[temp3].sound > 0) {
									do {
										temp = mt::rand() % 8;
									} while (temp == 3);
									soundQueue[temp] = weapons[temp3].sound;
								}

								tempPos = weapons[temp3].max;

								if (enemy[i].aniactive == 2)
								{
									enemy[i].aniactive = 1;
								}

								if (++enemy[i].eshotmultipos[j-1] > tempPos)
								{
									enemy[i].eshotmultipos[j-1] = 1;
								}
								tempPos = enemy[i].eshotmultipos[j-1];

								if (j == 1)
								{
									temp2 = 4;
								}

								enemyShot[b].sx = tempX + weapons[temp3].bx[tempPos-1] + tempMapXOfs;
								enemyShot[b].sy = tempY + weapons[temp3].by[tempPos-1];
								enemyShot[b].sdmg = weapons[temp3].attack[tempPos-1];
								enemyShot[b].tx = weapons[temp3].tx;
								enemyShot[b].ty = weapons[temp3].ty;
								enemyShot[b].duration = weapons[temp3].del[tempPos-1];
								enemyShot[b].animate = 0;
								enemyShot[b].animax = weapons[temp3].weapani;

								enemyShot[b].sgr = weapons[temp3].sg[tempPos-1];
								switch (j)
								{
									case 1:
										enemyShot[b].syc = weapons[temp3].acceleration;
										enemyShot[b].sxc = weapons[temp3].accelerationx;

										enemyShot[b].sxm = weapons[temp3].sx[tempPos-1];
										enemyShot[b].sym = weapons[temp3].sy[tempPos-1];
										break;
									case 2:
										enemyShot[b].sxc = weapons[temp3].acceleration;
										enemyShot[b].syc = -weapons[temp3].acceleration;

										enemyShot[b].sxm = weapons[temp3].sy[tempPos-1];
										enemyShot[b].sym = -weapons[temp3].sx[tempPos-1];
										break;
									case 3:
										enemyShot[b].sxc = -weapons[temp3].acceleration;
										enemyShot[b].syc = weapons[temp3].accelerationx;

										enemyShot[b].sxm = -weapons[temp3].sy[tempPos-1];
										enemyShot[b].sym = -weapons[temp3].sx[tempPos-1];
										break;
								}

								if (weapons[temp3].aim > 0)
								{
									temp4 = weapons[temp3].aim;

									/*DIF*/
									if (difficultyLevel > 2)
									{
										temp4 += difficultyLevel - 2;
									}

									tempX2 = PX;
									tempY2 = PY;

									if (twoPlayerMode)
									{
										if (playerAliveB && !playerAlive)
										{
											temp = 1;
										} else if (playerAlive && !playerAliveB) {
											temp = 0;
										} else {
											temp = mt::rand() % 2;
										}

										if (temp == 1)
										{
											tempX2 = PXB - 25;
											tempY2 = PYB;
										}
									}

									tempI = (tempX2 + 25) - tempX - tempMapXOfs - 4;
									if (tempI == 0)
									{
										tempI++;
									}
									tempI2 = tempY2 - tempY;
									if (tempI2 == 0)
									{
										tempI2++;
									}
									if (abs(tempI) > abs(tempI2))
									{
										tempI3 = abs(tempI);
									} else {
										tempI3 = abs(tempI2);
									}
									enemyShot[b].sxm = ot_round(((float)tempI / tempI3) * temp4);
									enemyShot[b].sym = ot_round(((float)tempI2 / tempI3) * temp4);
								}
							}
							break;
						}
					}
				}
			}

			/* Enemy Launch Routine */
			if (enemy[i].launchfreq)
			{
				if (--enemy[i].launchwait == 0)
				{
					enemy[i].launchwait = enemy[i].launchfreq;

					if (enemy[i].launchspecial != 0)
					{
						/*Type  1 : Must be inline with player*/
						if (abs(enemy[i].ey - PY) > 5)
							goto draw_enemy_end;
					}

					if (enemy[i].aniactive == 2)
					{
						enemy[i].aniactive = 1;
					}

					if (enemy[i].launchtype == 0)
						goto draw_enemy_end;

					tempW = enemy[i].launchtype;
					JE_newEnemy(enemyOffset == 50 ? 75 : enemyOffset - 25);

					/*Launch Enemy Placement*/
					if (b > 0)
					{
						tempI = tempX;
						tempI2 = tempY + enemyDat[enemy[b-1].enemytype].startyc;
						if (enemy[b-1].size == 0)
						{
							tempI -= 0;
							tempI2 -= 7;
						}
						if (enemy[b-1].launchtype > 0 && enemy[b-1].launchfreq == 0)
						{

							if (enemy[b-1].launchtype > 90)
							{
								tempI += mt::rand() % ((enemy[b-1].launchtype - 90) * 4) - (enemy[b-1].launchtype - 90) * 2;
							} else {
								tempI4 = (PX + 25) - tempX - tempMapXOfs - 4;
								if (tempI4 == 0)
								{
									tempI4++;
								}
								tempI5 = PY - tempY;
								if (tempI5 == 0)
								{
									tempI5++;
								}
								if (abs(tempI4) > abs(tempI5))
								{
									tempI3 = abs(tempI4);
								} else {
									tempI3 = abs(tempI5);
								}
								enemy[b-1].exc = ot_round(((float)tempI4 / tempI3) * enemy[b-1].launchtype);
								enemy[b-1].eyc = ot_round(((float)tempI5 / tempI3) * enemy[b-1].launchtype);
							}
						}

						do {
							temp = mt::rand() % 8;
						} while (temp == 3);
						soundQueue[temp] = randomEnemyLaunchSounds[(mt::rand() % 3)];

						if (enemy[i].launchspecial == 1
							&& enemy[i].linknum < 100)
						{
							enemy[b-1].linknum = enemy[i].linknum;
						}

						enemy[b-1].ex = tempI;
						enemy[b-1].ey = tempI2;
					}
				}
			}
		}
draw_enemy_end:
		;
	}

	PX += 25;
}

void JE_main( void )
{
	int i, j, l;
	unsigned char **bp;

	Uint8 *p; /* source/shape pointer */
	Uint8 *s; /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	int lastEnemyOnScreen;

	loadTitleScreen = true;

	/* Setup Player Items/General Data */
	for (int z = 0; z < 12; z++)
	{
		pItems[z] = 0;
	}
	shieldSet = 5;

	/* Setup Graphics */
	JE_updateColorsFast(black);

	/*debuginfo('Initiating Configuration');*/


	/* Setup Input Device */
	/*JConfigure:=false;*/

	debug = false;

	/* NOTE: BEGIN MAIN PROGRAM HERE AFTER LOADING A GAME OR STARTING A NEW ONE */

	/* ----------- GAME ROUTINES ------------------------------------- */
	/* We need to jump to the beginning to make space for the routines */
	/* --------------------------------------------------------------- */
	goto start_level_first;

	/*------------------------------GAME LOOP-----------------------------------*/


	/* Startlevel is called after a previous level is over.  If the first level
	   is started for a gaming session, startlevelfirst is called instead and
	   this code is skipped.  The code here finishes the level and prepares for
	   the loadmap function. */

start_level:

	if (galagaMode)
	{
		twoPlayerMode = false;
	}

	if (playDemo)
	{
		JE_selectSong(0);
	}

	useButtonAssign = true; /*Joystick button remapping*/

	JE_clearKeyboard();

	if (eShapes1 != NULL)
	{
		free(eShapes1);
		eShapes1 = NULL;
	}
	if (eShapes2 != NULL)
	{
		free(eShapes2);
		eShapes2 = NULL;
	}
	if (eShapes3 != NULL)
	{
		free(eShapes3);
		eShapes3 = NULL;
	}
	if (eShapes4 != NULL)
	{
		free(eShapes4);
		eShapes4 = NULL;
	}

	/* Normal speed */
	if (fastPlay != 0)
	{
		speed = 0x4300;
		JE_resetTimerInt();
		JE_setTimerInt();
	}

	if (CVars::record_demo || playDemo)
	{
		recordFile.close();
		if (playDemo)
		{
			JE_fadeBlack(10);
			/* JE_wipekey();*/
			wait_noinput(true, true, true);
		}
	}

	if (isNetworkGame)
	{
		netQuit = false;
	}

	difficultyLevel = oldDifficultyLevel;   /*Return difficulty to normal*/

	if (!playDemo)
	{
		if (((playerAlive || (twoPlayerMode && playerAliveB))
		   || normalBonusLevelCurrent || bonusLevelCurrent)
		   && !playerEndLevel)
		{
			mainLevel = nextLevel;
			JE_endLevelAni();
			JE_selectSong(0xC001);  /*Fade song out*/
		}
		else
		{
			JE_selectSong(0xC001);  /*Fade song out*/

			JE_fadeBlack(10);
			if (twoPlayerMode)
			{
				temp = 22;
			} else {
				temp = 11;
			}
			JE_loadGame(temp);
			if (doNotSaveBackup)
			{
				superTyrian = false;
				onePlayerAction = false;
				pItems[PITEM_SUPER_ARCADE_MODE] = 0;
			}
			if (bonusLevelCurrent && !playerEndLevel)
			{
				mainLevel = nextLevel;
			}
		}
	}
	doNotSaveBackup = false;

start_level_first:

	/*stopsequence;*/
	/*debuginfo('Setting Master Sound Volume');*/

	JE_loadCompShapes(&shapes6, &shapes6Size, '1');  /* Items */

	endLevel = false;
	reallyEndLevel = false;
	playerEndLevel = false;
	extraGame = false;

	/*debuginfo('Loading LEVELS.DAT');*/

	doNotSaveBackup = false;
	JE_loadMap();
	JE_selectSong(0xC001); /*Fade song out*/

	playerAlive = true;
	playerAliveB = true;
	oldDifficultyLevel = difficultyLevel;
	if (episodeNum == 4)
	{
		difficultyLevel--;
	}
	if (difficultyLevel < 1)
	{
		difficultyLevel = 1;
	}

	if (loadDestruct)
	{
		destruct::JE_destructGame();
		loadDestruct = false;
		loadTitleScreen = true;
		goto start_level_first;
	}

	PX = 100;
	PY = 180;

	PXB = 190;
	PYB = 180;

	playerHNotReady = true;

	lastPXShotMove = PX;
	lastPYShotMove = PY;

	if (twoPlayerMode)
	{
		JE_loadPic(6, false);
	} else {
		JE_loadPic(3, false);
	}

	tempScreenSeg = VGAScreen;
	JE_drawOptions();

	if (twoPlayerMode)
	{
		temp = 76;
	} else {
		temp = 118;
	}
	JE_outText(268, temp, levelName, 12, 4);

	JE_showVGA();
	JE_gammaCorrect(colors, gammaCorrection);
	JE_fadeColor(50);

	JE_loadCompShapes(&shapes6, &shapes6Size, '6'); /* Explosions */

	/* MAPX will already be set correctly */
	mapY = 300 - 8;
	mapY2 = 600 - 8;
	mapY3 = 600 - 8;
	mapYPos = &megaData1->mainmap[mapY][0] - 1;
	mapY2Pos = &megaData2->mainmap[mapY2][0] - 1;
	mapY3Pos = &megaData3->mainmap[mapY3][0] - 1;
	mapXPos = 0;
	mapXOfs = 0;
	mapX2Pos = 0;
	mapX3Pos = 0;
	mapX3Ofs = 0;
	mapXbpPos = 0;
	mapX2bpPos = 0;
	mapX3bpPos = 0;

	map1YDelay = 1;
	map1YDelayMax = 1;
	map2YDelay = 1;
	map2YDelayMax = 1;

	musicFade = false;

	backPos = 0;
	backPos2 = 0;
	backPos3 = 0;
	power = 0;
	starY = scr_width;

	// Setup maximum player speed
	// ==== Mouse Input ====
	baseSpeed = 6;
	baseSpeedKeyH = (baseSpeed / 4) + 1;
	baseSpeedKeyV = (baseSpeed / 4) + 1;

	baseSpeedOld = baseSpeed;
	baseSpeedOld2 = (int)(baseSpeed * 0.7f) + 1;
	baseSpeed2  = 100 - (((baseSpeed + 1) / 4) + 1);
	baseSpeed2B = 100 + 100 - baseSpeed2;
	baseSpeed   = 100 - (((baseSpeed + 1) / 4) + 1);
	baseSpeedB  = 100 + 100 - baseSpeed;
	shadowyDist = 10;

	/* Setup player ship graphics */
	JE_getShipInfo();
	tempI = (((PX - mouseX) / (100 - baseSpeed)) * 2) * 168;
	lastTurn = 0;
	lastTurnB = 0;
	lastTurn2 = 0;

	playerInvulnerable1 = 100;
	playerInvulnerable2 = 100;

	newkey = false;

	/* Initialize Level Data and Debug Mode */
	levelEnd = 255;
	levelEndWarp = -4;
	levelEndFxWait = 0;
	warningCol = 120;
	warningColChange = 1;
	warningSoundDelay = 0;
	armorShipDelay = 50;

	bonusLevel = false;
	readyToEndLevel = false;
	firstGameOver = true;
	eventLoc = 1;
	curLoc = 0;
	backMove = 1;
	backMove2 = 2;
	backMove3 = 3;
	explodeMove = 2;
	enemiesActive = true;
	for(temp = 0; temp < 3; temp++)
	{
		button[temp] = false;
	}
	stopBackgrounds = false;
	stopBackgroundNum = 0;
	background3x1   = false;
	background3x1b  = false;
	background3over = 0;
	background2over = 1;
	topEnemyOver = false;
	skyEnemyOverAll = false;
	smallEnemyAdjust = false;
	starActive = true;
	enemyContinualDamage = false;
	levelEnemyFrequency = 96;
	quitRequested = false;

	memset(statBar, 0, sizeof(statBar));

	forceEvents = false;  /*Force events to continue if background movement = 0*/

	uniqueEnemy = false;  /*Look in MakeEnemy under shape bank*/

	superEnemy254Jump = 0;   /*When Enemy with PL 254 dies*/

	/* Filter Status */
	filterActive = true;
	filterFade = true;
	filterFadeStart = false;
	levelFilter = -99;
	levelBrightness = -14;
	levelBrightnessChg = 1;

	background2notTransparent = false;

	/* Initially erase power bars */
	lastPower = power / 10;

	/* Initial Text */
	JE_drawTextWindow(miscText[20]);

	/* Setup Armor/Shield Data */
	shieldWait = 1;
	shield     = shields[pItems[PITEM_SHIELD]].mpwr;
	shieldT    = shields[pItems[PITEM_SHIELD]].tpwr * 20;
	shieldMax  = shield * 2;
	shield2    = shields[pItemsPlayer2[PITEM_SHIELD]].mpwr;
	shieldMax2 = shield * 2;
	JE_drawShield();
	JE_drawArmor();

	superBomb[0] = 0;
	superBomb[1] = 0;

	/* Set cubes to 0 */
	cubeMax = 0;

	lastPortPower[0] = 0;
	lastPortPower[1] = 0;
	lastPortPower[2] = 0;
	lastPortPower[3] = 0;

	/* Secret Level Display */
	flash = 0;
	flashChange = 1;
	displayTime = 0;

	JE_playSong(levelSong);

	/*if not JConfigure and (InputDevice=1) then CalibrateJoy;*/

	JE_drawPortConfigButtons();

	/* --- MAIN LOOP --- */

	newkey = false;

	if (isNetworkGame)
	{
		JE_clearSpecialRequests();
		mt::seed(32402394);
	}

	JE_setupStars();

	JE_setNewGameSpeed();

	/*Save backup game*/
	if (!playDemo && !doNotSaveBackup)
	{
		temp = twoPlayerMode ? 22 : 11;
		JE_saveGame(temp, "LAST LEVEL    ");
	}

	memset(lastKey, 0, sizeof(lastKey));
	if (CVars::record_demo && !playDemo)
	{
		do
		{
			if (Filesystem::get().fileExists((boost::format("demorec.%1%") % recordFileNum).str()))
			{
				recordFileNum++;
			}
		} while (tempb);

		recordFile.open((Filesystem::getHomeDir()+(boost::format("demorec.%1%") % recordFileNum).str()).c_str(), std::ios_base::out | std::ios_base::binary);
		if (!recordFile.good())
		{
			exit(1);
		}

		OBinaryStream bs(recordFile);

		bs.put8(episodeNum);
		bs.put(levelName, 10);
		bs.put8(lvlFileNum);
		bs.put(pItems, pItems+12);
		bs.put(portPower, portPower+5);
		bs.put8(levelSong);

		lastMoveWait = 0;
	}

	twoPlayerLinked = false;
	linkGunDirec = M_PI;

	JE_calcPurpleBall(1);
	JE_calcPurpleBall(2);

	damageRate = 2;  /*Normal Rate for Collision Damage*/

	chargeWait   = 5;
	chargeLevel  = 0;
	chargeMax    = 5;
	chargeGr     = 0;
	chargeGrWait = 3;

	portConfigChange = false;

	makeMouseDelay = false;

	/*Destruction Ratio*/
	totalEnemy = 0;
	enemyKilled = 0;

	/*InputDevices*/
	if (twoPlayerMode && !isNetworkGame)
	{
		playerDevice1 = inputDevice1;
		playerDevice2 = inputDevice2;
	} else {
		playerDevice1 = 0;
		playerDevice2 = 0;
	}

	astralDuration = 0;

	superArcadePowerUp = 1;

	yourInGameMenuRequest = false;

	constantLastX = -1;

	playerStillExploding = 0;
	playerStillExploding2 = 0;

	if (isNetworkGame)
	{
		JE_loadItemDat();
	}

	std::fill_n(enemyAvail, 100, 1);
	std::fill_n(enemyShotAvail, ENEMY_SHOT_MAX, true);

	/*Initialize Shots*/
	memset(playerShotData,   0, sizeof(playerShotData));
	memset(shotAvail,        0, sizeof(shotAvail));
	memset(shotMultiPos,     0, sizeof(shotMultiPos));
	std::fill(shotRepeat, shotRepeat+COUNTOF(shotRepeat), 0);

	memset(button,           0, sizeof(button));

	memset(globalFlags,      0, sizeof(globalFlags));

	init_explosions();

	/* --- Clear Sound Queue --- */
	memset(soundQueue,       0, sizeof(soundQueue));
	soundQueue[3] = V_GOOD_LUCK;

	memset(enemyShapeTables, 0, sizeof(enemyShapeTables));
	memset(enemy,            0, sizeof(enemy));

	memset(SFCurrentCode,    0, sizeof(SFCurrentCode));
	memset(SFExecuted,       0, sizeof(SFExecuted));

	zinglonDuration = 0;
	specialWait = 0;
	nextSpecialWait = 0;
	optionAttachmentMove  = 0;    /*Launch the Attachments!*/
	optionAttachmentLinked = true;

	editShip1 = false;
	editShip2 = false;

	memset(smoothies, 0, sizeof(smoothies));

	levelTimer = false;
	randomExplosions = false;

	init_superpixels();

	returnActive = false;

	galagaShotFreq = 0;

	if (galagaMode)
	{
		difficultyLevel = 2;
	}
	galagaLife = 10000;

	JE_drawOptionLevel();

	BKwrap1 = &megaData1->mainmap[1][0];
	BKwrap1to = &megaData1->mainmap[1][0];
	BKwrap2 = &megaData2->mainmap[1][0];
	BKwrap2to = &megaData2->mainmap[1][0];
	BKwrap3 = &megaData3->mainmap[1][0];
	BKwrap3to = &megaData3->mainmap[1][0];

level_loop:

	tempScreenSeg = game_screen; /* side-effect of game_screen */

	if (isNetworkGame)
	{
		smoothies[9-1] = false;
		smoothies[6-1] = false;
	} else {
		starShowVGASpecialCode = smoothies[9-1] + (smoothies[6-1] << 1);
	}

	/*Background Wrapping*/
	if (mapYPos <= BKwrap1)
	{
		mapYPos = BKwrap1to;
	}
	if (mapY2Pos <= BKwrap2)
	{
		mapY2Pos = BKwrap2to;
	}
	if (mapY3Pos <= BKwrap3)
	{
		mapY3Pos = BKwrap3to;
	}


	allPlayersGone = !playerAlive &&
	                 (!playerAliveB || !twoPlayerMode) &&
	                 ((portPower[0] == 1 && playerStillExploding == 0) || (!onePlayerAction && !twoPlayerMode)) &&
	                 ((portPower[1] == 1 && playerStillExploding2 == 0) || !twoPlayerMode);


	/*-----MUSIC FADE------*/
	if (musicFade)
	{
		if (tempVolume > 0.f)
		{
			tempVolume -= .01f;
			music_vol_multiplier = tempVolume;
		} else {
			musicFade = false;
		}
	}

	if (!allPlayersGone && levelEnd > 0 && endLevel)
	{
		JE_playSong(10);
		musicFade = false;
	} else {
		if (!playing && CVars::snd_enabled && firstGameOver)
		{
			JE_playSong(levelSong);
			playing = true;
		}
	}


	/* use game_screen for all the generic drawing functions */
	VGAScreen = game_screen;

	if (!endLevel)
	{    /*MAIN DRAWING IS STOPPED STARTING HERE*/
		
		/*-----------------------Message Bar------------------------*/
		if (textErase > 0)
		{
			if (--textErase == 0)
			{
				tempScreenSeg = VGAScreenSeg; /* <MXD> SEGa000 */
				JE_newDrawCShapeNum(OPTION_SHAPES, 37, 16, 189);
			}
		}

		VGAScreen = VGAScreenSeg; /* side-effect of game_screen */
		
		/*------------------------Shield Gen-------------------------*/
		if (galagaMode)
		{
			shield = 0;
			shield2 = 0;
			if (portPower[2-1] == 0 || armorLevel2 == 0)
			{
				twoPlayerMode = false;
			}
			if (score >= galagaLife)
			{
				soundQueue[6] = 11;
				soundQueue[7] = 21;
				if (portPower[1-1] < 11)
				{
					portPower[1-1]++;
				} else {
					score += 1000;
				}
				if (galagaLife == 10000)
				{
					galagaLife = 20000;
				} else {
					galagaLife += 25000;
				}
			}
		} else { /*GALAGA mode*/
			if (twoPlayerMode)
			{
				if (--shieldWait == 0)
				{
					shieldWait = 20 - shieldSet;
					if (shield < shieldMax && playerAlive)
					{
						shield++;
					}
					if (shield2 < shieldMax2 && playerAliveB)
					{
						shield2++;
					}
					JE_drawShield();
				}
			} else {
				if (playerAlive && shield < shieldMax && power > shieldT)
				{
					if (--shieldWait == 0)
					{
						shieldWait = 20 - shieldSet;
						power -= shieldT;
						shield++;
						if (shield2 < shieldMax)
						{
							shield2++;
						}
						JE_drawShield();
					}
				}
			}
		}
		
		/*---------------------Weapon Display-------------------------*/
		if (lastPortPower[1-1] != portPower[1-1])
		{
			lastPortPower[1-1] = portPower[1-1];
			
			if (twoPlayerMode)
			{
				tempW2 = 6;
				tempW = 286;
			} else {
				tempW2 = 17;
				tempW = 289;
			}
			
			JE_c_bar(tempW, tempW2, tempW + 1 + 10 * 2, tempW2 + 2, 0);
			
			for (temp = 1; temp <= portPower[1-1]; temp++)
			{
				JE_rectangle(tempW, tempW2, tempW + 1, tempW2 + 2, 115 + temp); /* <MXD> SEGa000 */
				tempW += 2;
			}
		}
		
		if (lastPortPower[2-1] != portPower[2-1])
		{
			lastPortPower[2-1] = portPower[2-1];
			
			if (twoPlayerMode)
			{
				tempW2 = 100;
				tempW = 286;
			} else {
				tempW2 = 38;
				tempW = 289;
			}
			
			JE_c_bar(tempW, tempW2, tempW + 1 + 10 * 2, tempW2 + 2, 0);
			
			for (temp = 1; temp <= portPower[2-1]; temp++)
			{
				JE_rectangle(tempW, tempW2, tempW + 1, tempW2 + 2, 115 + temp); /* <MXD> SEGa000 */
				tempW += 2;
			}
		}
		
		/*------------------------Power Bar-------------------------*/
		if (twoPlayerMode || onePlayerAction)
		{
			power = 900;
		} else {
			power = power + powerAdd;
			if (power > 900)
			{
				power = 900;
			}
			
			temp = power / 10;
			
			if (temp != lastPower)
			{
				if (temp > lastPower)
				{
					JE_c_bar(269, 113 - 11 - temp, 276, 114 - 11 - lastPower, 113 + temp / 7);
				} else {
					JE_c_bar(269, 113 - 11 - lastPower, 276, 114 - 11 - temp, 0);
				}
			}
			
			lastPower = temp;
		}

		oldMapX3Ofs = mapX3Ofs;

		enemyOnScreen = 0;
		
		VGAScreen = game_screen; /* side-effect of game_screen */

	}    /*MAIN DRAWING IS STOPPED ENDING   HERE*/

	/*---------------------------EVENTS-------------------------*/
	while (eventRec[eventLoc-1].eventtime <= curLoc && eventLoc <= maxEvent)
	{
		JE_eventSystem();
	}

	if (isNetworkGame && reallyEndLevel)
	{
		goto start_level;
	}


	/* SMOOTHIES! */
	JE_checkSmoothies();
	if (anySmoothies)
	{
		VGAScreen = smoothiesScreen;
	}

	/* --- BACKGROUNDS --- */
	/* --- BACKGROUND 1 --- */

	if (forceEvents && !backMove)
	{
		curLoc++;
	}

	if (map1YDelayMax > 1 && backMove < 2)
	{
		if (map1YDelay == 1)
		{
			backMove = 1;
		} else {
			backMove = 0;
		}
	}

	/*Draw background*/
	if (astralDuration == 0)
	{
		/* BP is used by all backgrounds */

		s = (Uint8 *)VGAScreen;

		/* Offset for top */
		s += 11 * 24;
		s += mapXPos;

		/* Map location number in BP */
		bp = mapYPos;
		bp += mapXbpPos;

		if (backPos)
		{
			/* --- BACKGROUND 1 TOP --- */
			for (i = 12; i; i--)
			{

				/* move to previous map X location */
				bp--;

				p = *bp;
				p += (28 - backPos) * 24;

				for (j = backPos; j; j--)
				{
					memcpy(s, p, 24);

					s += scr_width;
					p += 24;
				}

				s -= backPos * scr_width + 24;
			}

			s += 24 * 11;
			s += backPos * scr_width + 24;

			/* Increment Map Location for next line */
			bp += 14 - 2;   /* (Map Width) */

		}
		bp += 14;   /* (Map Width) */

		/* --- BACKGROUND 1 CENTER --- */

		/* Outer loop - Screen 6 lines high */
		for (i = 6; i; i--)
		{
			for (j = 12; j; j--)
			{
				/* move to previous map X location */
				bp--;
				p = *bp;

				for (l = 0; l < 28; l++)
				{
					memcpy(s, p, 24);

					s += scr_width;
					p += 24;
				}

				s -= scr_width * 28 + 24;
			}

			/* Increment Map Location for next line */
			bp += 14 + 14 - 2;  /* (Map Width) */

			s += scr_width * 28 + 24 * 12;
		}

		if (backPos <= 15)
		{
			/* --- BACKGROUND 1 BOTTOM --- */
			for (i = 12; i; i--)
			{
				/* move to previous map X location */
				bp--;
				p = *bp;

				for (j = 15 - backPos + 1; j; j--)
				{
					memcpy(s, p, 24);

					s += scr_width;
					p += 24;
				}

				s -= (15 - backPos + 1) * scr_width + 24;
			}
		}
	} else {
		JE_clr256();
	}

	/*Set Movement of background 1*/
	if (--map1YDelay == 0)
	{
		map1YDelay = map1YDelayMax;

		curLoc += backMove;

		backPos += backMove;

		if (backPos > 27)
		{
			backPos -= 28;
			mapY--;
			mapYPos -= 14;  /*Map Width*/
		}
	}

	/*---------------------------STARS--------------------------*/
	/* DRAWSTARS */
	if (starActive || astralDuration > 0)
	{
		s = (Uint8 *)VGAScreen;
		
		for (i = MAX_STARS; i--; )
		{
			starDat[i].sLoc += starDat[i].sMov + starY;
			if (starDat[i].sLoc < 177 * scr_width)
			{
				if (*(s + starDat[i].sLoc) == 0)
				{
					*(s + starDat[i].sLoc) = starDat[i].sC;
				}
				if (starDat[i].sC - 4 >= 9 * 16)
				{
					if (*(s + starDat[i].sLoc + 1) == 0)
					{
						*(s + starDat[i].sLoc + 1) = starDat[i].sC - 4;
					}
					if (starDat[i].sLoc > 0 && *(s + starDat[i].sLoc - 1) == 0)
					{
						*(s + starDat[i].sLoc - 1) = starDat[i].sC - 4;
					}
					if (*(s + starDat[i].sLoc + scr_width) == 0)
					{
						*(s + starDat[i].sLoc + scr_width) = starDat[i].sC - 4;
					}
					if (starDat[i].sLoc >= scr_width && *(s + starDat[i].sLoc - scr_width) == 0)
					{
						*(s + starDat[i].sLoc - scr_width) = starDat[i].sC - 4;
					}
				}
			}
		}
	}

	if (CVars::r_smoothies_detail > 0 && smoothies[4])
	{
		JE_smoothies3();
	}

	/*-----------------------BACKGROUNDS------------------------*/
	/*-----------------------BACKGROUND 2------------------------*/
	if (background2over == 3)
	{
		JE_drawBackground2();
	}

	if (background2over == 0 && CVars::r_background2)
	{
		//if (!(smoothies[1] && CVars::detail_level < 3) && !(smoothies[0] && CVars::detail_level == 1))
		//{
			if (CVars::r_background2_blend && !background2notTransparent) {
				JE_superBackground2();
			} else {
				JE_drawBackground2();
			}
		//}
	}

	if (smoothies[0] && CVars::r_smoothies_detail > 0 && SDAT[0] == 0)
	{
		JE_smoothies1();
	}
	if (smoothies[1] && CVars::r_smoothies_detail > 0)
	{
		JE_smoothies2();
	}

	/*-----------------------Ground Enemy------------------------*/
	lastEnemyOnScreen = enemyOnScreen;

	tempMapXOfs = mapXOfs;
	tempBackMove = backMove;
	JE_drawEnemy(50);
	JE_drawEnemy(100);

	if (enemyOnScreen == 0 || enemyOnScreen == lastEnemyOnScreen)
	{
		if (stopBackgroundNum == 1)
		{
			stopBackgroundNum = 9;
		}
	}

	if (smoothies[0] && CVars::r_smoothies_detail > 0 && SDAT[0] > 0)
	{
		JE_smoothies1();
	}

	if (CVars::r_wild)
	{
		neat += 3;
		JE_darkenBackground(neat);
	}

	/*-----------------------BACKGROUNDS------------------------*/
	/*-----------------------BACKGROUND 2------------------------*/
	//if (!(smoothies[1] && CVars::detail_level < 3) &&
	//    !(smoothies[0] && CVars::detail_level == 2))
	//{
		if (background2over == 1 && CVars::r_background2)
		{
			if (CVars::r_background2_blend && !background2notTransparent) {
				JE_superBackground2();
			} else {
				JE_drawBackground2();
			}
		}
	//}

	if (CVars::r_wild)
	{
		neat++;
		JE_darkenBackground(neat);
	}

	if (background3over == 2)
	{
		JE_drawBackground3();
	}

	/* New Enemy */
	if (enemiesActive && mt::rand() % 100 > levelEnemyFrequency)
	{
		tempW = levelEnemy[mt::rand() % levelEnemyMax];
		if (tempW == 2)
		{
			soundQueue[3] = 7;
		}
		JE_newEnemy(0);
	}

	if (CVars::r_smoothies_detail > 0 && smoothies[2])
	{
		JE_smoothies3();
	}
	if (CVars::r_smoothies_detail > 0 && smoothies[3])
	{
		JE_smoothies4();
	}

	/* Draw Sky Enemy */
	if (!skyEnemyOverAll)
	{
		lastEnemyOnScreen = enemyOnScreen;

		tempMapXOfs = mapX2Ofs;
		tempBackMove = 0;
		JE_drawEnemy(25);

		if (enemyOnScreen == lastEnemyOnScreen)
		{
			if (stopBackgroundNum == 2)
			{
				stopBackgroundNum = 9;
			}
		}
	}

	if (background3over == 0)
	{
		JE_drawBackground3();
	}

	/* Draw Top Enemy */
	if (!topEnemyOver)
	{
		tempMapXOfs = (background3x1 == 0) ? oldMapX3Ofs : mapXOfs;
		tempBackMove = backMove3;
		JE_drawEnemy(75);
	}

	/* Player Shot Images */
	for (int z = 0; z < MAX_PWEAPON; z++)
	{
		if (shotAvail[z] != 0)
		{
			shotAvail[z]--;
			if (z != MAX_PWEAPON - 1)
			{

				playerShotData[z].shotXM += playerShotData[z].shotXC;
				playerShotData[z].shotX += playerShotData[z].shotXM;
				tempI4 = playerShotData[z].shotXM;

				if (playerShotData[z].shotXM > 100)
				{
					if (playerShotData[z].shotXM == 101)
					{
						playerShotData[z].shotX -= 101;
						playerShotData[z].shotX += PXChange;
						playerShotData[z].shotY += PYChange;
					} else {
						playerShotData[z].shotX -= 120;
						playerShotData[z].shotX += PXChange;
					}
				}

				playerShotData[z].shotYM += playerShotData[z].shotYC;
				playerShotData[z].shotY += playerShotData[z].shotYM;

				if (playerShotData[z].shotYM > 100)
				{
					playerShotData[z].shotY -= 120;
					playerShotData[z].shotY += PYChange;
				}

				if (playerShotData[z].shotComplicated != 0)
				{
					playerShotData[z].shotDevX += playerShotData[z].shotDirX;
					playerShotData[z].shotX += playerShotData[z].shotDevX;

					if (abs(playerShotData[z].shotDevX) == playerShotData[z].shotCirSizeX)
					{
						playerShotData[z].shotDirX = -playerShotData[z].shotDirX;
					}

					playerShotData[z].shotDevY += playerShotData[z].shotDirY;
					playerShotData[z].shotY += playerShotData[z].shotDevY;

					if (abs(playerShotData[z].shotDevY) == playerShotData[z].shotCirSizeY)
					{
						playerShotData[z].shotDirY = -playerShotData[z].shotDirY;
					}
					/*Double Speed Circle Shots - add a second copy of above loop*/
				}

				tempShotX = playerShotData[z].shotX;
				tempShotY = playerShotData[z].shotY;

				if (playerShotData[z].shotX < -34 || playerShotData[z].shotX > 290 ||
				    playerShotData[z].shotY < -15 || playerShotData[z].shotY > 190)
				{
					shotAvail[z] = 0;
					goto draw_player_shot_loop_end;
				}

				if (playerShotData[z].shotTrail != 255)
				{
					if (playerShotData[z].shotTrail == 98)
					{
						JE_setupExplosion(playerShotData[z].shotX - playerShotData[z].shotXM, playerShotData[z].shotY - playerShotData[z].shotYM, playerShotData[z].shotTrail);
					} else {
						JE_setupExplosion(playerShotData[z].shotX, playerShotData[z].shotY, playerShotData[z].shotTrail);
					}
				}

				if (playerShotData[z].aimAtEnemy != 0)
				{
					if (--playerShotData[z].aimDelay == 0) {
						playerShotData[z].aimDelay = playerShotData[z].aimDelayMax;

						if (enemyAvail[playerShotData[z].aimAtEnemy] != 1)
						{
							if (playerShotData[z].shotX < enemy[playerShotData[z].aimAtEnemy].ex)
							{
								playerShotData[z].shotXM++;
							} else {
								playerShotData[z].shotXM--;
							}
							if (playerShotData[z].shotY < enemy[playerShotData[z].aimAtEnemy].ey)
							{
								playerShotData[z].shotYM++;
							} else {
								playerShotData[z].shotYM--;
							}
						} else {
							if (playerShotData[z].shotXM > 0)
							{
								playerShotData[z].shotXM++;
							} else {
								playerShotData[z].shotXM--;
							}
						}
					}
				}

				tempW = playerShotData[z].shotGr + playerShotData[z].shotAni;
				if (++playerShotData[z].shotAni == playerShotData[z].shotAniMax)
				{
					playerShotData[z].shotAni = 0;
				}

				tempI2 = playerShotData[z].shotDmg;
				temp2 = playerShotData[z].shotBlastFilter;
				chain = playerShotData[z].chainReaction;
				playerNum = playerShotData[z].playerNumber;

				tempSpecial = tempW > 60000;

				if (tempSpecial)
				{
					JE_newDrawCShapeTrickNum(OPTION_SHAPES, tempW - 60000, tempShotX+1, tempShotY);
					tempX2 = shapeX[OPTION_SHAPES][tempW - 60000 - 1] >> 1;
					tempY2 = shapeY[OPTION_SHAPES][tempW - 60000 - 1] >> 1;
				} else {
					if (tempW > 1000)
					{
						create_superpixels(tempShotX+1 + 6, tempShotY + 6, 5, 3, (tempW / 1000) << 4);
						tempW = tempW % 1000;
					}
					if (tempW > 500)
					{
						if ((CVars::r_background2 || background2over == 3) && tempShotY + shadowyDist < 190 && tempI4 < 100)
						{
							JE_drawShape2Shadow(tempShotX+1, tempShotY + shadowyDist, tempW - 500, shapesW2);
						}
						JE_drawShape2(tempShotX+1, tempShotY, tempW - 500, shapesW2);
					} else {
						if ((CVars::r_background2 || background2over == 3) && tempShotY + shadowyDist < 190 && tempI4 < 100)
						{
							JE_drawShape2Shadow(tempShotX+1, tempShotY + shadowyDist, tempW, shapesC1);
						}
						JE_drawShape2(tempShotX+1, tempShotY, tempW, shapesC1);
					}
				}

			}

			for (b = 0; b < 100; b++)
			{
				if (enemyAvail[b] == 0)
				{
					if (z == MAX_PWEAPON - 1)
					{
						temp = 25 - abs(zinglonDuration - 25);
						tempB = abs(enemy[b].ex + enemy[b].mapoffset - (PX + 7)) < temp;
						temp2 = 9;
						chain = 0;
						tempI2 = 10;
					} else if (tempSpecial) {
						tempB = ((enemy[b].enemycycle == 0) &&
						        (abs(enemy[b].ex + enemy[b].mapoffset - tempShotX - tempX2) < (25 + tempX2)) &&
						        (abs(enemy[b].ey - tempShotY - 12 - tempY2)                 < (29 + tempY2))) ||
						        ((enemy[b].enemycycle > 0) &&
						        (abs(enemy[b].ex + enemy[b].mapoffset - tempShotX - tempX2) < (13 + tempX2)) &&
						        (abs(enemy[b].ey - tempShotY - 6 - tempY2)                  < (15 + tempY2)));
					} else {
						tempB = ((enemy[b].enemycycle == 0) &&
						        (abs(enemy[b].ex + enemy[b].mapoffset - tempShotX) < 25) && (abs(enemy[b].ey - tempShotY - 12) < 29)) ||
						        ((enemy[b].enemycycle > 0) &&
						        (abs(enemy[b].ex + enemy[b].mapoffset - tempShotX) < 13) && (abs(enemy[b].ey - tempShotY - 6) < 15));
					}
					
					if (tempB)
					{
						if (chain > 0)
						{
							shotMultiPos[5-1] = 0;
							JE_initPlayerShot(0, 5, tempShotX, tempShotY, mouseX, mouseY, chain, playerNum);
							shotAvail[z] = 0;
							goto draw_player_shot_loop_end;
						}
						
						infiniteShot = false;
						
						if (tempI2 == 99)
						{
							tempI2 = 0;
							doIced = 40;
							enemy[b].iced = 40;
						} else {
							doIced = 0;
							if (tempI2 >= 250) {
								tempI2 = tempI2 - 250;
								infiniteShot = true;
							}
						}
						
						tempI = enemy[b].armorleft;
						
						temp = enemy[b].linknum;
						if (temp == 0)
						{
							temp = 255;
						}
						
						if (enemy[b].armorleft < 255)
						{
							if (temp == statBar[1-1])
							{
								statCol[1-1] = 6;
							}
							if (temp == statBar[2-1])
							{
								statCol[2-1] = 6;
							}
							if (enemy[b].enemyground)
							{
								enemy[b].filter = temp2;
							}
							for (i = 0; i < 100; i++)
							{
								if (enemy[i].linknum == temp &&
								    enemyAvail[i] != 1 &&
								    enemy[i].enemyground != 0)
								{
									if (doIced)
									{
										enemy[i].iced = doIced;
									}
									enemy[i].filter = temp2;
								}
							}
						}
						
						if (tempI > tempI2)
						{
							if (z != MAX_PWEAPON - 1)
							{
									if (enemy[b].armorleft != 255)
									{
										enemy[b].armorleft -= tempI2;
										JE_setupExplosion(tempShotX, tempShotY, 0);
									} else {
									create_superpixels(tempShotX + 6, tempShotY + 6, tempI2 / 2 + 3, tempI2 / 4 + 2, temp2);
								}
							}
							
							soundQueue[5] = 3;
							
							if ((tempI - tempI2 <= enemy[b].edlevel) &&
							    ((!enemy[b].edamaged) ^ (enemy[b].edani < 0)))
							{
								
								for (temp3 = 0; temp3 < 100; temp3++)
								{
									if (enemyAvail[temp3] != 1)
									{
										temp4 = enemy[temp3].linknum;
										if ((temp3 == b) ||
										    ((temp != 255) &&
										    (((enemy[temp3].edlevel > 0) && (temp4 == temp)) ||
										    ((enemyContinualDamage && (temp - 100 == temp4)) ||
										    ((temp4 > 40) && (temp4 / 20 == temp / 20) && (temp4 <= temp))))))
										{
										
											enemy[temp3].enemycycle = 1;
											
											enemy[temp3].edamaged = !enemy[temp3].edamaged;
											
											if (enemy[temp3].edani != 0)
											{
												enemy[temp3].ani = abs(enemy[temp3].edani);
												enemy[temp3].aniactive = 1;
												enemy[temp3].animax = 0;
												enemy[temp3].animin = enemy[temp3].edgr;
												enemy[temp3].enemycycle = enemy[temp3].animin - 1;
												
											} else if (enemy[temp3].edgr > 0)
											{
												enemy[temp3].egr[1-1] = enemy[temp3].edgr;
												enemy[temp3].ani = 1;
												enemy[temp3].aniactive = 0;
												enemy[temp3].animax = 0;
												enemy[temp3].animin = 1;
											}
											else
											{
												enemyAvail[temp3] = 1;
												enemyKilled++;
											}
											
											enemy[temp3].aniwhenfire = 0;
											
											if (enemy[temp3].armorleft > enemy[temp3].edlevel)
											{
												enemy[temp3].armorleft = enemy[temp3].edlevel;
											}
											
											tempX = enemy[temp3].ex + enemy[temp3].mapoffset;
											tempY = enemy[temp3].ey;
											
											if (enemyDat[enemy[temp3].enemytype].esize != 1)
											{
												JE_setupExplosion(tempX, tempY - 6, 1);
											} else {
												JE_setupExplosionLarge(enemy[temp3].enemyground, enemy[temp3].explonum / 2, tempX, tempY);
											}
										}
									}
								}
							}
						} else {
							
							if ((temp == 254) && (superEnemy254Jump > 0))
							{
								JE_eventJump(superEnemy254Jump);
							}
						
							for (temp2 = 0; temp2 < 100; temp2++)
							{
								if (enemyAvail[temp2] != 1)
								{
									temp3 = enemy[temp2].linknum;
									if ((temp2 == b) || (temp == 254) ||
									    ((temp != 255) && ((temp == temp3) || (temp - 100 == temp3)
									    || ((temp3 > 40) && (temp3 / 20 == temp / 20) && (temp3 <= temp)))))
									{
										
										tempI3 = enemy[temp2].ex + enemy[temp2].mapoffset;
										
										if (enemy[temp2].special)
										{
											globalFlags[enemy[temp2].flagnum] = enemy[temp2].setto;
										}
										
										if ((enemy[temp2].enemydie > 0) &&
											!((superArcadeMode > 0) &&
											(enemyDat[enemy[temp2].enemydie].value == 30000)))
										{
											zz = b;
											tempW = enemy[temp2].enemydie;
											tempW2 = temp2 - (temp2 % 25);
											if (enemyDat[tempW].value > 30000)
											{
												tempW2 = 0;
											}
											JE_newEnemy(tempW2);
											if (b != 0)
											{
												if ((superArcadeMode > 0) && (enemy[b-1].evalue > 30000))
												{
													superArcadePowerUp++;
													if (superArcadePowerUp > 5)
													{
														superArcadePowerUp = 1;
													}
													enemy[b-1].egr[1-1] = 5 + superArcadePowerUp * 2;
													enemy[b-1].evalue = 30000 + superArcadePowerUp;
												}
												
												if (enemy[b-1].evalue != 0)
												{
													enemy[b-1].scoreitem = true;
												} else {
													enemy[b-1].scoreitem = false;
												}
												enemy[b-1].ex = enemy[temp2].ex;
												enemy[b-1].ey = enemy[temp2].ey;
											}
											b = zz;
										}
										
										if ((enemy[temp2].evalue > 0) && (enemy[temp2].evalue < 10000))
										{
											if (enemy[temp2].evalue == 1)
											{
												cubeMax++;
											} else {
												if ((playerNum < 2) || galagaMode)
												{
													score += enemy[temp2].evalue;
												} else {
													score2 += enemy[temp2].evalue;
												}
											}
										}
										
										if ((enemy[temp2].edlevel == -1) && (temp == temp3))
										{
											enemy[temp2].edlevel = 0;
											enemyAvail[temp2] = 2;
											enemy[temp2].egr[1-1] = enemy[temp2].edgr;
											enemy[temp2].ani = 1;
											enemy[temp2].aniactive = 0;
											enemy[temp2].animax = 0;
											enemy[temp2].animin = 1;
											enemy[temp2].edamaged = true;
											enemy[temp2].enemycycle = 1;
										} else {
											enemyAvail[temp2] = 1;
											enemyKilled++;
										}
										
										if (enemyDat[enemy[temp2].enemytype].esize == 1)
										{
											JE_setupExplosionLarge(enemy[temp2].enemyground, enemy[temp2].explonum, tempI3, enemy[temp2].ey);
											soundQueue[6] = 9;
										} else {
											JE_setupExplosion(tempI3, enemy[temp2].ey, 1);
											soundQueue[6] = 8;
										}
									}
								}
							}
						}
						
						if (infiniteShot)
						{
							tempI2 += 250;
						} else {
							if (z != MAX_PWEAPON - 1)
							{
								if (tempI2 <= tempI)
								{
									shotAvail[z] = 0;
									goto draw_player_shot_loop_end;
								} else {
									playerShotData[z].shotDmg -= tempI;
								}
							}
						}
					}
				}
			}

draw_player_shot_loop_end:
			;
		}
	}

	/* Player movement indicators for shots that track your ship */
	lastPXShotMove = PX;
	lastPYShotMove = PY;

	/*=================================*/
	/*=======Collisions Detection======*/
	/*=================================*/

	if (playerAlive && !endLevel)
	{
		JE_playerCollide(&PX, &PY, &lastTurn, &lastTurn2, &score, &armorLevel, &shield, &playerAlive,
		                 &playerStillExploding, 1, playerInvulnerable1);
	}

	if (twoPlayerMode && playerAliveB && !endLevel)
		JE_playerCollide(&PXB, &PYB, &lastTurnB, &lastTurn2B, &score2, &armorLevel2, &shield2, &playerAliveB,
		                 &playerStillExploding2, 2, playerInvulnerable2);

	if (firstGameOver)
	{
		JE_mainGamePlayerFunctions();      /*--------PLAYER DRAW+MOVEMENT---------*/
	}

	if (!endLevel)
	{    /*MAIN DRAWING IS STOPPED STARTING HERE*/

		/* Draw Enemy Shots */
		for (int z = 0; z < ENEMY_SHOT_MAX; z++)
		{
			if (enemyShotAvail[z] == 0)
			{
				enemyShot[z].sxm += enemyShot[z].sxc;
				enemyShot[z].sx += enemyShot[z].sxm;

				if (enemyShot[z].tx != 0)
				{
					if (enemyShot[z].sx > PX)
					{
						if (enemyShot[z].sxm > -enemyShot[z].tx)
						{
							enemyShot[z].sxm--;
						}
					} else {
						if (enemyShot[z].sxm < enemyShot[z].tx)
						{
							enemyShot[z].sxm++;
						}
					}
				}

				enemyShot[z].sym += enemyShot[z].syc;
				enemyShot[z].sy += enemyShot[z].sym;

				if (enemyShot[z].ty != 0)
				{
					if (enemyShot[z].sy > PY)
					{
						if (enemyShot[z].sym > -enemyShot[z].ty)
						{
							enemyShot[z].sym--;
						}
					} else {
						if (enemyShot[z].sym < enemyShot[z].ty)
						{
							enemyShot[z].sym++;
						}
					}
				}

				if (enemyShot[z].duration-- == 0 || enemyShot[z].sy > 190 || enemyShot[z].sy <= -14 || enemyShot[z].sx > 275 || enemyShot[z].sx <= 0)
				{
					enemyShotAvail[z] = 1;
				} else {
					bool yes = false;

					if (playerAlive != 0
					    && enemyShot[z].sx - PX > sAniXNeg && enemyShot[z].sx - PX < sAniX
					    && enemyShot[z].sy - PY > sAniYNeg && enemyShot[z].sy - PY < sAniY)
					{
						temp3 = 1;
						yes = true;
					}
					if (twoPlayerMode != 0
					    && playerAliveB != 0
					    && enemyShot[z].sx - PXB > sAniXNeg && enemyShot[z].sx - PXB < sAniX
					    && enemyShot[z].sy - PYB > sAniYNeg && enemyShot[z].sy - PYB < sAniY)
					{
						temp3 = 2;
						yes = true;
					}

					if (yes)
					{
						tempX = enemyShot[z].sx;
						tempY = enemyShot[z].sy;
						temp = enemyShot[z].sdmg;
						
						enemyShotAvail[z] = 1;
						
						JE_setupExplosion(tempX, tempY, 0);
						
						switch (temp3)
						{
							case 1:
								if (playerInvulnerable1 == 0)
								{
									if ((temp = JE_playerDamage(tempX, tempY, temp, &PX, &PY, &playerAlive, &playerStillExploding, &armorLevel, &shield, 1)) > 0)
									{
										lastTurn2 += (enemyShot[z].sxm * temp) / 2;
										lastTurn  += (enemyShot[z].sym * temp) / 2;
									}
								}
								break;
							case 2:
								if (playerInvulnerable2 == 0)
								{
									if ((temp = JE_playerDamage(tempX, tempY, temp, &PXB, &PYB, &playerAliveB, &playerStillExploding2, &armorLevel2, &shield2, 2)) > 0)
									{
										lastTurn2B += (enemyShot[z].sxm * temp) / 2;
										lastTurnB  += (enemyShot[z].sym * temp) / 2;
									}
								}
								break;
						}
					} else {
						s = (Uint8 *)VGAScreen;
						s += enemyShot[z].sy * scr_width + enemyShot[z].sx;

						s_limit = (Uint8 *)VGAScreen;
						s_limit += scr_height * scr_width;

						if (enemyShot[z].animax != 0)
						{
							if (++enemyShot[z].animate >= enemyShot[z].animax)
							{
								enemyShot[z].animate = 0;
							}
						}

						if (enemyShot[z].sgr >= 500)
						{
							p = shapesW2;
							p += SDL_SwapLE16(((JE_word *)p)[enemyShot[z].sgr + enemyShot[z].animate - 500 - 1]);
						} else {
							p = shapesC1;
							p += SDL_SwapLE16(((JE_word *)p)[enemyShot[z].sgr + enemyShot[z].animate - 1]);
						}

						while (*p != 0x0f)
						{
							s += *p & 0x0f;
							i = (*p & 0xf0) >> 4;
							if (i)
							{
								while (i--)
								{
									p++;
									if (s >= s_limit)
										goto enemy_shot_draw_overflow;
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

				}

enemy_shot_draw_overflow:
				;
			}
		}
	}

	if (background3over == 1)
	{
		JE_drawBackground3();
	}

	/* Draw Top Enemy */
	if (topEnemyOver)
	{
		tempMapXOfs = (background3x1 == 0) ? oldMapX3Ofs : oldMapXOfs;
		tempBackMove = backMove3;
		JE_drawEnemy(75);
	}

	/* Draw Sky Enemy */
	if (skyEnemyOverAll)
	{
		lastEnemyOnScreen = enemyOnScreen;

		tempMapXOfs = mapX2Ofs;
		tempBackMove = 0;
		JE_drawEnemy(25);

		if (enemyOnScreen == lastEnemyOnScreen)
		{
			if (stopBackgroundNum == 2)
			{
				stopBackgroundNum = 9;
			}
		}
	}

	draw_explosions();

	if (!portConfigChange)
	{
		portConfigDone = true;
	}


	/*-----------------------BACKGROUNDS------------------------*/
	/*-----------------------BACKGROUND 2------------------------*/
	//if (!(smoothies[1] && CVars::detail_level < 3) &&
	//    !(smoothies[0] && CVars::detail_level == 2))
	//{
		if (background2over == 2 && CVars::r_background2)
		{
			if (CVars::r_background2_blend && !background2notTransparent) {
				JE_superBackground2();
			} else {
				JE_drawBackground2();
			}
		}
	//}

	/*-------------------------Warning---------------------------*/
	if ((playerAlive && armorLevel < 6) ||
	    (twoPlayerMode && !galagaMode && playerAliveB && armorLevel2 < 6))
	{
		if (playerAlive && armorLevel < 6)
		{
			tempW2 = armorLevel;
		} else {
			tempW2 = armorLevel2;
		}
		
		if (armorShipDelay > 0)
		{
			armorShipDelay--;
		} else {
			tempW = 560;
			JE_newEnemy(50);
			if (b > 0)
			{
				enemy[b-1].enemydie = 560 + (mt::rand() % 3) + 1;
				enemy[b-1].eyc -= backMove3;
				enemy[b-1].armorleft = 4;
			}
			armorShipDelay = 500;
		}
		
		if ((playerAlive && armorLevel < 6 && (!isNetworkGame || thisPlayerNum == 1))
		    || (twoPlayerMode && playerAliveB && armorLevel2 < 6 && (!isNetworkGame || thisPlayerNum == 2)))
		{
				
			tempW = tempW2 * 4 + 8;
			if (warningSoundDelay > tempW)
			{
				warningSoundDelay = tempW;
			}
			
			if (warningSoundDelay > 1)
			{
				warningSoundDelay--;
			} else {
				soundQueue[7] = 17;
				warningSoundDelay = tempW;
			}
			
			warningCol += warningColChange;
			if (warningCol > 113 + (14 - (tempW2 * 2)))
			{
				warningColChange = -warningColChange;
				warningCol = 113 + (14 - (tempW2 * 2));
			} else if (warningCol < 113) {
				warningColChange = -warningColChange;
			}
			JE_bar(24, 181, 138, 183, warningCol);
			JE_bar(175, 181, 287, 183, warningCol);
			JE_bar(24, 0, 287, 3, warningCol);
			
			JE_outText(140, 178, "WARNING", 7, (warningCol % 16) / 2);
			
		}
	}

	/*------- Random Explosions --------*/
	if (randomExplosions)
	{
		if (mt::rand() % 10 == 1)
		{
			JE_setupExplosionLarge(false, 20, mt::rand() % 280, rand() % 180);
		}
	}


	/*=================================*/
	/*=======The Sound Routine=========*/
	/*=================================*/
	if (CVars::snd_enabled && firstGameOver)
	{
		temp = 0;
		for (temp2 = 0; temp2 < SFX_CHANNELS; temp2++)
		{
			if (soundQueue[temp2] > 0)
			{
				float fx_vol;
				temp = soundQueue[temp2];
				if (temp2 == 3)
				{
					fx_vol = 1.f;
				} else {
					if (temp == 15)
					{
						fx_vol = .25f;
					} else {   /*Lightning*/
						fx_vol = .5f;
					}
				}
				JE_multiSamplePlay(digiFx[temp-1], fxSize[temp-1], temp2, fx_vol);
				soundQueue[temp2] = 0;
			}
		}
	}

	if (returnActive && enemyOnScreen == 0)
	{
		JE_eventJump(65535);
		returnActive = false;
	}

	/*-------      DEbug      ---------*/
	debugTime = SDL_GetTicks();
	tempW = lastmouse_but;
	tempX = mouse_x;
	tempY = mouse_y;

	if (debug)
	{
		std::ostringstream smoothiesStr;
		for (int i = 0; i < 9; ++i)
		{
			smoothiesStr << (smoothies[i] ? '1' : '0');
		}
		JE_outText(30, 70, (boost::format("SM = %1%") % smoothiesStr).str(), 4, 0);

		JE_outText(30, 80, (boost::format("Memory left = %d") % -1).str(), 4, 0);
		JE_outText(30, 90, (boost::format("Enemies onscreen = %d") % enemyOnScreen).str(), 6, 0);

		debugHist = debugHist + ot_abs((float)(debugTime - lastDebugTime));
		debugHistCount++;

		const float fps = 1000.0f / ot_round(debugHist / debugHistCount);
		const std::string debug_str = (boost::format("X:%d Y:%-5d  %2.3f FPS  %d %d %d %d") % ((mapX-1)*12+PX) % curLoc % fps % lastTurn2 % lastTurn % PX % PY).str();
		JE_outText(45, 175, debug_str, 15, 3);
		lastDebugTime = debugTime;
	}

	if (displayTime > 0)
	{
		displayTime--;
		JE_outTextAndDarken(90, 10, miscText[59], 15, flash - 8, FONT_SHAPES);
		flash += flashChange;
		if (flash > 4 || flash == 0)
		{
			flashChange = -flashChange;
		}
	}

	/*Pentium Speed Mode?*/
	if (pentiumMode)
	{
		if (frameCountMax == 2)
		{
			frameCountMax = 3;
		} else {
			frameCountMax = 2;
		}
	}

	/*--------  Level Timer    ---------*/
	if (levelTimer)
	{
		if (levelTimerCountdown > 0)
		{
			levelTimerCountdown--;
			if (levelTimerCountdown == 0)
			{
				JE_eventJump(levelTimerJumpTo);
			}

			if (levelTimerCountdown > 200)
			{
				if (levelTimerCountdown % 100 == 0)
				{
					soundQueue[7] = 17;
				}

				if (levelTimerCountdown % 10 == 0)
				{
					soundQueue[6] = 24;  /*28 or 24*/
				}
			} else {
				if (levelTimerCountdown % 20 == 0)
				{
					soundQueue[7] = 17;
				}
			}

			JE_textShade (140, 6, miscText[66], 7, (levelTimerCountdown % 20) / 3, FULL_SHADE);
			JE_dString (100, 2, (boost::format("%.1f") % (levelTimerCountdown / 100.0f)).str(), SMALL_FONT_SHAPES);
		}
	}

	/*GAME OVER*/
	if (!CVars::ch_constant_play && !CVars::ch_constant_death)
	{
		if (allPlayersGone)
		{
			if (!(playerStillExploding == 0 && playerStillExploding2 == 0))
			{
				if (galagaMode)
				{
					playerStillExploding2 = 0;
				}
				musicFade = true;
			} else {
				if (playDemo || normalBonusLevelCurrent || bonusLevelCurrent)
				{
					reallyEndLevel = true;
				} else {
					JE_dString(120, 60, miscText[21], FONT_SHAPES);
				}
				JE_setMousePosition(159, 100);
				if (firstGameOver)
				{
					if (!playDemo)
					{
						JE_playSong(11);
					}
					firstGameOver = false;
				}

				if (!playDemo)
				{
					service_SDL_events(true);
					JE_joystick2();
					if (playDemo ||
					   (newkey || button[0] || button[1] || button[2]) ||
					   (newmouse))
					{
						reallyEndLevel = true;
					}
				}

				if (isNetworkGame)
				{
					reallyEndLevel = true;
				}

			}
		}
	}

	/* Call Keyboard input handler */
	service_SDL_events(false);
	if (playDemo)
	{
		if (newkey || JE_anyButton() || button[0])
		{
			reallyEndLevel = true;
			stoppedDemo = true;
		}
	} else {
		if (newkey)
		{
			skipStarShowVGA = false;
			JE_mainKeyboardInput();
			newkey = false;
			if (skipStarShowVGA)
			{
				goto level_loop;
			}
		}
	}

	/*Network Update*/
	if (isNetworkGame)
	{
		if (!reallyEndLevel)
		{
			Uint16 requests = pauseRequest << 0 |
				inGameMenuRequest << 1 |
				skipLevelRequest << 2 |
				nortShipRequest << 3;
			SDLNet_Write16(requests, network::packet_state_out[0]->data+14);

			SDLNet_Write16(difficultyLevel, network::packet_state_out[0]->data+16);
			SDLNet_Write16(PX, network::packet_state_out[0]->data+18);
			SDLNet_Write16(PXB, network::packet_state_out[0]->data+20);
			SDLNet_Write16(PY, network::packet_state_out[0]->data+22);
			SDLNet_Write16(PYB, network::packet_state_out[0]->data+24);
			SDLNet_Write16(curLoc, network::packet_state_out[0]->data+26);

			network::state_send();

			if (network::state_update())
			{
				assert(SDLNet_Read16(network::packet_state_in[0]->data+26) == SDLNet_Read16(network::packet_state_out[network::delay]->data+26));

				requests = SDLNet_Read16(network::packet_state_in[0]->data+14) ^ SDLNet_Read16(network::packet_state_out[network::delay]->data+14);
				if (requests & 1)
				{
					JE_pauseGame();
				}
				if (requests & 2)
				{
					yourInGameMenuRequest = (SDLNet_Read16(network::packet_state_out[network::delay]->data+14) & 2) != 0;
					JE_doInGameSetup();
					yourInGameMenuRequest = false;
					if (haltGame)
						reallyEndLevel = true;
				}
				if (requests & 4)
				{
					levelTimer = true;
					levelTimerCountdown = 0;
					endLevel = true;
					levelEnd = 40;
				}
				if (requests & 8)
				{
					pItems[11] = 12;
					pItems[10] = 13;
					pItems[0] = 36;
					pItems[1] = 37;
					shipGr = 1;
				}

				for (int i = 0; i < 2; ++i)
				{
					if (SDLNet_Read16(network::packet_state_in[0]->data+(18+i*2)) != SDLNet_Read16(network::packet_state_out[network::delay]->data+(18+i*2)))
					{
						tempScreenSeg = game_screen;
						JE_textShade(40, 110+i*10, (boost::format("Player %1% is unsynchronized!") % (i+1)).str(), 9, 2, FULL_SHADE);
						tempScreenSeg = VGAScreen;
					}
				}
			}
		}

		JE_clearSpecialRequests();
	}

	draw_superpixels();

	/*Filtration*/
	if (filterActive)
	{
		JE_filterScreen(levelFilter, levelBrightness);
	}

	/* Statbar */
	if (statBar[1-1] > 0 || statBar[2-1] > 0)
	{
		JE_doStatBar();
	}

	JE_inGameDisplays();

	VGAScreen = VGAScreenSeg; /* side-effect of game_screen */

	JE_starShowVGA();

	/*??*/
	if (repause && superPause)
	{
		repause = false;
		JE_pauseGame();
		keysactive[SDLK_p] = false;
	}

	/*Start backgrounds if no enemies on screen
	  End level if number of enemies left to kill equals 0.*/
	if (stopBackgroundNum == 9 && backMove == 0 && !enemyStillExploding)
	{
		backMove = 1;
		backMove2 = 2;
		backMove3 = 3;
		explodeMove = 2;
		stopBackgroundNum = 0;
		stopBackgrounds = false;
		if (waitToEndLevel)
		{
			endLevel = true;
			levelEnd = 40;
		}
		if (allPlayersGone)
		{
			reallyEndLevel = true;
		}
	}

	if (!endLevel && enemyOnScreen == 0)
	{
		if (readyToEndLevel && !enemyStillExploding)
		{
			if (levelTimerCountdown > 0)
			{
				levelTimer = false;
			}
			readyToEndLevel = false;
			endLevel = true;
			levelEnd = 40;
			if (allPlayersGone)
			{
				reallyEndLevel = true;
			}
		}
		if (stopBackgrounds)
		{
			stopBackgrounds = false;
			backMove = 1;
			backMove2 = 2;
			backMove3 = 3;
			explodeMove = 2;
		}
	}

	// Other Network Functions
	JE_handleChat();

	if (reallyEndLevel)
	{
		goto start_level;
	}
	goto level_loop;
}

/* --- Load Level/Map Data --- */
void JE_loadMap( void )
{

	std::fstream shpFile;
	std::fstream lvlFile;
/*	FILE *tempFile;*/ /*Extract map file from LVL file*/


	char char_mapFile, char_shapeFile;

	JE_DanCShape shape;

	JE_word x, y;
	int yy, z;
	JE_word mapSh[3][128]; /* [1..3, 0..127] */
	Uint8 *ref[3][128]; /* [1..3, 0..127] */
	std::string s;


	Uint8 mapBuf[15 * 600]; /* [1..15 * 600] */
	JE_word bufLoc;

	int i;
	Uint8 pic_buffer[320*200]; /* screen buffer, 8-bit specific */
	Uint8 *vga, *pic, *vga2; /* screen pointer, 8-bit specific */

	lastCubeMax = cubeMax;

	/*Defaults*/
	songBuy = 3;  /*Item Screen default song*/

	if (loadTitleScreen || playDemo)
	{
#ifdef NDEBUG
		JE_openingAnim();
#endif
		JE_titleScreen(true);
		loadTitleScreen = false;
	}

	/* Load LEVELS.DAT - Section = MAINLEVEL */
	saveLevel = mainLevel;

new_game:
	galagaMode  = false;
	useLastBank = false;
	extraGame   = false;
	haltGame = false;
	if (loadTitleScreen)
	{
		JE_titleScreen(true);
		loadTitleScreen = false;
	}

	gameLoaded = false;

	if (!playDemo && !loadDestruct)
	{
		do
		{
			Filesystem::get().openDatafileFail(lvlFile, macroFile);

			x = 0;
			jumpSection = false;
			loadLevelOk = false;

			/* Seek Section # Mainlevel */
			while (x < mainLevel)
			{
				s = JE_readCryptLn(lvlFile);
				if (s[0] == '*')
				{
					x++;
					s[0] = ' ';
				}
			}

			ESCPressed = false;

			do
			{

				if (gameLoaded)
				{
					if (mainLevel == 0)
					{
						loadTitleScreen = true;
					}
					lvlFile.close();
					goto new_game;
				}

				s = JE_readCryptLn(lvlFile);

				switch (s[0])
				{
					case ']':
						switch (s[1])
						{
							case 'A':
								JE_playAnim("tyrend.anm", 1, true, 7);
								break;

							case 'G':
								mapOrigin = lexical_cast<JE_word>(s.substr(4, 2));
								mapPNum = lexical_cast<JE_word>(s.substr(7, 1));
								for (i = 0; i < mapPNum; i++)
								{
									mapPlanet[i] = lexical_cast<int>(s.substr(1+ (i+1)*8, 2));
									mapSection[i] = lexical_cast<int>(s.substr(4+ (i+1)*8, 3));
								}
								break;
							case '?': {
								unsigned int temp = lexical_cast<unsigned int>(s.substr(4, 2));

								for (unsigned int i = 0; i < temp; i++)
								{
									cubeList[i] = lexical_cast<JE_word>(s.substr(3+(i+1)*4, 3));
								}
								if (cubeMax > temp)
								{
									cubeMax = temp;
								}
								break; }
							case '!':
								// Auto set CubeMax
								cubeMax = lexical_cast<unsigned int>(s.substr(4, 2));
								break;
							case '+':
								// PORT-CHANGE: 
								//temp = lexical_cast<int>(s.substr(4, 2));
								//cubeMax += temp;
								cubeMax += lexical_cast<int>(s.substr(4, 2));
								if (cubeMax > 4)
								{
									cubeMax = 4;
								}
								break;
							case 'g':
								galagaMode = true;   /*GALAGA mode*/
								memcpy(&pItemsPlayer2, &pItems, sizeof(pItemsPlayer2));
								pItemsPlayer2[PITEM_REAR_WEAPON] = 15; /*Player 2 starts with 15 - MultiCannon and 2 single shot options*/
								pItemsPlayer2[PITEM_LEFT_SIDEKICK] = 0;
								pItemsPlayer2[PITEM_RIGHT_SIDEKICK] = 0;
								break;

							case 'x':
								extraGame = true;
								break;

							case 'e': /*ENGAGE mode*/
								doNotSaveBackup = true;
								onePlayerAction = true;
								superTyrian = true;
								twoPlayerMode = false;

								score = 0;

								portPower[0] = 3;
								portPower[1] = 0;
								pItems[PITEM_SHIP] = 13;
								pItems[PITEM_FRONT_WEAPON] = 39;
								pItems[PITEM_SUPER_ARCADE_MODE] = 255;

								pItems[PITEM_REAR_WEAPON] = 0; /*Normally 0 - Rear Weapon*/
								pItems[PITEM_LEFT_SIDEKICK] = 0;
								pItems[PITEM_RIGHT_SIDEKICK] = 0;
								pItems[PITEM_GENERATOR] = 2;
								pItems[PITEM_P2_SIDEKICK_UPGRADE] = 2; // Unused in P1
								pItems[PITEM_P2_SIDEKICK] = 1; // Unused in P1
								pItems[PITEM_SHIELD] = 4;
								pItems[PITEM_SPECIAL] = 0; /*Secret Weapons*/
								break;

							case 'J':
								// PORT-CHANGE:
								//temp = atoi(strnztcpy(buffer, s + 3, 3));
								//mainLevel = temp;
								mainLevel = lexical_cast<unsigned int>(s.substr(3, 3));
								jumpSection = true;
								break;
							case '2':
								// PORT-CHANGE:
								//temp = atoi(strnztcpy(buffer, s + 3, 3));
								if (twoPlayerMode || onePlayerAction)
								{
									//mainLevel = temp;
									mainLevel = lexical_cast<unsigned int>(s.substr(3, 3));
									jumpSection = true;
								}
								break;
							case 'w':
								// Allowed to go to Time War?
								// PORT-CHANGE:
								//temp = atoi(strnztcpy(buffer, s + 3, 3));
								if (pItems[PITEM_SHIP] == 13)
								{
									//mainLevel = temp;
									mainLevel = lexical_cast<unsigned int>(s.substr(3, 3));
									jumpSection = true;
								}
								break;
							case 't':
								// PORT-CHANGE:
								//temp = atoi(strnztcpy(buffer, s + 3, 3));
								if (levelTimer && levelTimerCountdown == 0)
								{
									//mainLevel = temp;
									mainLevel = lexical_cast<unsigned int>(s.substr(3, 3));
									jumpSection = true;
								}
								break;
							case 'l':
								// PORT-CHANGE:
								//temp = atoi(strnztcpy(buffer, s + 3, 3));
								if (!playerAlive || (twoPlayerMode && !playerAliveB))
								{
									//mainLevel = temp;
									mainLevel = lexical_cast<unsigned int>(s.substr(3, 3));
									jumpSection = true;
								}
								break;
							case 's':
								saveLevel = mainLevel;
								break; /*store savepoint*/
							case 'b':
								if (twoPlayerMode)
								{
									temp = 22;
								} else {
									temp = 11;
								}
								JE_saveGame(11, "LAST LEVEL    ");
								break;

							case 'i':
								// PORT-CHANGE:
								songBuy = lexical_cast<int>(s.substr(3, 3));
								break;
							case 'I':
								// Load Items Available Information

								memset(&itemAvail, 0, sizeof(itemAvail));

								for (temp = 0; temp < 9; temp++)
								{
									std::string line = JE_readCryptLn(lvlFile);
									std::stringstream sstr;

									sstr << (line.length() > 8 ? line.substr(8) : "") << ' ';

									unsigned int items_count = 0;
									while (sstr >> itemAvail[temp][items_count])
									{
										items_count++;
									}

									itemAvailMax[temp] = items_count;
								}

								JE_itemScreen();
								break;

							case 'L':
								nextLevel = lexical_cast<unsigned int>(s.substr(9, 3));
								levelName = s.substr(13, 9);
								levelSong = lexical_cast<int>(s.substr(22, 2));
								if (nextLevel == 0)
								{
									nextLevel = mainLevel + 1;
								}
								lvlFileNum = lexical_cast<int>(s.substr(25, 2));
								loadLevelOk = true;
								bonusLevelCurrent = (s.length() > 28) && (s[28] == '$');
								normalBonusLevelCurrent = (s.length() > 27) && (s[27] == '$');
								gameJustLoaded = false;
								break;

							case '@':
								useLastBank = !useLastBank;
								break;

							case 'Q':
								ESCPressed = false;
								temp = secretHint + (mt::rand() % 3) * 3;

								if (twoPlayerMode)
								{
									levelWarningText[0] = std::string(miscText[40]) + ' ' + lexical_cast<std::string>(score);
									levelWarningText[1] = std::string(miscText[41]) + ' ' + lexical_cast<std::string>(score2);
									levelWarningText[2] = "";
									levelWarningLines = 3;
								} else {
									levelWarningText[0] = std::string(miscText[37]) + ' ' + lexical_cast<std::string>(JE_totalScore(score, pItems));
									levelWarningText[1] = "";
									levelWarningLines = 2;
								}

								for (x = 0; x < temp - 1; x++)
								{
									std::string s;
									do
									{
										s = JE_readCryptLn(lvlFile);
									} while (s[0] != '#');
								}

								{
									std::string s;
									do
									{
										s = JE_readCryptLn(lvlFile);
										levelWarningText[levelWarningLines] = s;
										levelWarningLines++;
									} while (s[0] != '#');
									levelWarningLines--;
								}

								frameCountMax = 4;
								if (!CVars::ch_constant_play)
								{
									JE_displayText();
								}

								JE_fadeBlack(15);

								tempb = JE_nextEpisode();

								if (jumpBackToEpisode1 && !twoPlayerMode)
								{

									JE_loadPic(1, false);
									JE_clr256();

									if (superTyrian)
									{
										if (initialDifficulty == 8)
										{
											superArcadeMode = SA + 1;
										} else {
											superArcadeMode = 1;
										}

										jumpSection = true;
										loadTitleScreen = true;
									}

									if (superArcadeMode < SA + 2)
									{
										if (SANextShip[superArcadeMode] == 9)
										{
											JE_dString(80, 180, "Or play... "+specialName[7], SMALL_FONT_SHAPES);
										}

										if (SANextShip[superArcadeMode] != 9)
										{
											JE_dString(JE_fontCenter(superShips[0], FONT_SHAPES), 30, superShips[0], FONT_SHAPES);
											JE_dString(JE_fontCenter(superShips[SANextShip[superArcadeMode]], SMALL_FONT_SHAPES), 100, superShips[SANextShip[superArcadeMode]], SMALL_FONT_SHAPES);
										} else {
											const std::string temp = (boost::format("%1% %2%") % miscTextB[4] % pName[0]).str();
											JE_dString(JE_fontCenter(temp, FONT_SHAPES), 100, temp, FONT_SHAPES);
										}

										if (SANextShip[superArcadeMode] < 7)
										{
											JE_drawShape2x2(148, 70, ships[SAShip[SANextShip[superArcadeMode]-1]].shipgraphic, shapes9);
										} else {
											if (SANextShip[superArcadeMode] == 7)
											{
												trentWin = true;
											}
										}

										const std::string temp = (boost::format("Type %1% at Title") % specialName[SANextShip[superArcadeMode]-1]).str();
										JE_dString(JE_fontCenter(temp, SMALL_FONT_SHAPES), 160, temp, SMALL_FONT_SHAPES);
										JE_showVGA();

										JE_fadeColor(50);
										if (!CVars::ch_constant_play)
										{
											while (!JE_anyButton());
										}
									}

									jumpSection = true;
									if (isNetworkGame)
									{
										JE_readTextSync();
									}
									if (superTyrian)
									{
										JE_fadeBlack(10);
									}
								}
								break;

							case 'P':
								if (!CVars::ch_constant_play)
								{
									tempX = lexical_cast<JE_word>(s.substr(3, 3));
									if (tempX > 900)
									{
										load_pcx_palette(tempX-900-1, false);
										JE_clr256();
										JE_showVGA();
										JE_fadeColor(1);
									} else {
										if (tempX == 0)
										{
											JE_loadPCX("tshp2.pcx");
										} else {
											JE_loadPic(tempX, false);
										}
										JE_showVGA();
										JE_fadeColor(10);
									}
								}
								break;

							case 'U':
								if (!CVars::ch_constant_play)
								{
									// TODO: NETWORK
									memcpy(VGAScreen2, VGAScreen, scr_width * scr_height);

									tempX = lexical_cast<JE_word>(s.substr(3, 3));
									JE_loadPic(tempX, false);
									memcpy(pic_buffer, VGAScreen, sizeof(pic_buffer));

									service_SDL_events(true);
									for (z = 0; z <= 199; z++)
									{
										service_SDL_events(false);
										if (!newkey && !ESCPressed)
										{
											vga = (Uint8 *)VGAScreen;
											vga2 = (Uint8 *)VGAScreen2;
											pic = pic_buffer + (199 - z) * 320;

											setjasondelay(1); /* attempting to emulate JE_waitRetrace();*/
											for (y = 0; y < 199; y++)
											{
												if (y <= z)
												{
													memcpy(vga, pic, 320);
													pic += 320;
												} else {
													memcpy(vga, vga2, scr_width);
													vga2 += scr_width;
												}
												vga += scr_width;
											}
											JE_showVGA();
											wait_delay();

											if (isNetworkGame)
											{
												// TODO: NETWORK
											}
										}
									}
									memcpy(VGAScreen, pic_buffer, sizeof(pic_buffer));
								}
								break;

							case 'V':
								if (!CVars::ch_constant_play)
								{
									// TODO: NETWORK
									memcpy(VGAScreen2, VGAScreen, scr_width * scr_height);

									tempX = lexical_cast<JE_word>(s.substr(3, 3));
									JE_loadPic(tempX, false);
									memcpy(pic_buffer, VGAScreen, sizeof(pic_buffer));

									service_SDL_events(true);
									for (z = 0; z <= 199; z++)
									{
										service_SDL_events(false);
										if (!newkey && !ESCPressed)
										{
											vga = (Uint8 *)VGAScreen;
											vga2 = (Uint8 *)VGAScreen2;
											pic = pic_buffer;

											setjasondelay(1); /* attempting to emulate JE_waitRetrace();*/
											for (y = 0; y < 199; y++)
											{
												if (y <= 199 - z)
												{
													memcpy(vga, vga2, scr_width);
													vga2 += scr_width;
												} else {
													memcpy(vga, pic, 320);
													pic += 320;
												}
												vga += scr_width;
											}
											JE_showVGA();
											wait_delay();

											if (isNetworkGame)
											{
												// TODO: NETWORK
											}
										}
									}
									memcpy(VGAScreen, pic_buffer, sizeof(pic_buffer));
								}
								break;

							case 'R':
								if (!CVars::ch_constant_play)
								{
									// TODO: NETWORK
									memcpy(VGAScreen2, VGAScreen, scr_width * scr_height);

									tempX = lexical_cast<JE_word>(s.substr(3, 3));
									JE_loadPic(tempX, false);
									memcpy(pic_buffer, VGAScreen, sizeof(pic_buffer));

									service_SDL_events(true);
									for (z = 0; z <= 318; z++)
									{
										service_SDL_events(false);
										if (!newkey && !ESCPressed)
										{
											vga = (Uint8 *)VGAScreen;
											vga2 = (Uint8 *)VGAScreen2;
											pic = pic_buffer;

											setjasondelay(1); /* attempting to emulate JE_waitRetrace();*/
											for(y = 0; y < 200; y++)
											{
												memcpy(vga, vga2 + z, 319 - z);
												vga += 320 - z;
												vga2 += scr_width;
												memcpy(vga, pic, z + 1);
												vga += z;
												pic += 320;
											}
											JE_showVGA();
											wait_delay();

											if (isNetworkGame)
											{
												// TODO: NETWORK
											}
										}
									}
									memcpy(VGAScreen, pic_buffer, sizeof(pic_buffer));
								}
								break;

							case 'C':
								if (!isNetworkGame)
								{
									JE_fadeBlack(10);
								}
								JE_clr256();
								JE_showVGA();
								load_palette(7, true);
								break;

							case 'B':
								if (!isNetworkGame)
								{
									JE_fadeBlack(10);
								}
								break;
							case 'F':
								if (!isNetworkGame)
								{
									JE_fadeWhite(100);
									JE_fadeBlack(30);
								}
								JE_clr256();
								JE_showVGA();
								break;

							case 'W':
								if (!CVars::ch_constant_play)
								{
									if (!ESCPressed)
									{
										warningCol = 14 * 16 + 5;
										warningColChange = 1;
										warningSoundDelay = 0;
										levelWarningDisplay = (s[2] == 'y');
										levelWarningLines = 0;
										frameCountMax = lexical_cast<JE_word>(s.substr(4, 2));
										setjasondelay2(6);
										warningRed = (frameCountMax / 10) != 0;
										frameCountMax = frameCountMax % 10;

										std::string s;
										do
										{
											s = JE_readCryptLn(lvlFile);

											if (s[0] != '#')
											{
												levelWarningText[levelWarningLines] = s;
												levelWarningLines++;
											}
										} while (s[0] != '#');

										JE_displayText();
										newkey = false;
									}
								}
								break;

							case 'H':
								if (initialDifficulty < 3)
								{
									mainLevel = lexical_cast<unsigned int>(s.substr(4, 3));
									jumpSection = true;
								}
								break;

							case 'h':
								if (initialDifficulty > 2)
								{
									JE_readCryptLn(lvlFile);
								}
								break;

							case 'S':
								if (isNetworkGame)
								{
									JE_readTextSync();
								}
								break;

							case 'n':
								ESCPressed = false;
								break;

							/* TODO */

							case 'M':
								// PORT-CHANGE:
								//temp = atoi(strnztcpy(buffer, s + 3, 3));
								//JE_playSong(temp);
								JE_playSong(lexical_cast<JE_word>(s.substr(3, 3)));
								break;

							/* TODO */
						}
					break;
				}


			} while (!(loadLevelOk || jumpSection));


			lvlFile.close();

		} while (!loadLevelOk);
	}

	if (!loadDestruct)
	{

		if (playDemo)
		{

			difficultyLevel = 2;
			Filesystem::get().openDatafileFail(recordFile, "demo."+lexical_cast<std::string>(playDemoNum));

			bonusLevelCurrent = false;

			IBinaryStream bs(recordFile);

			temp = bs.get8();
			JE_initEpisode(temp);
			levelName = bs.getStr(10);
			lvlFileNum = bs.get8();
			for (int i = 0; i < 12; i++)
				pItems[i] = bs.get8();
			for (int i = 0; i < 5; i++)
				portPower[i] = bs.get8();
			levelSong = bs.get8();

			temp = bs.get8();
			temp2 = bs.get8();
			lastMoveWait = (temp << 8) | temp2;
			nextDemoOperation = bs.get8();

			firstEvent = true;

			/*debuginfo('Demo loaded.');*/
		} else {
			JE_fadeBlack(50);
		}


		Filesystem::get().openDatafileFail(lvlFile, levelFile);
		lvlFile.seekg(lvlPos[(lvlFileNum-1) * 2]);

		IBinaryStream bsLvl(lvlFile);

		char_mapFile = bsLvl.get8();
		char_shapeFile = bsLvl.get8();
		mapX = bsLvl.get16();
		mapX2 = bsLvl.get16();
		mapX3 = bsLvl.get16();

		levelEnemyMax = bsLvl.get16();
		for (unsigned int i = 0; i < levelEnemyMax; ++i)
		{
			levelEnemy[i] = bsLvl.get16();
		}

		maxEvent = bsLvl.get16();
		for (unsigned int i = 0; i < maxEvent; ++i)
		{
			eventRec[i].eventtime = bsLvl.get16();
			eventRec[i].eventtype = bsLvl.get8();
			eventRec[i].eventdat = bsLvl.getS16();
			eventRec[i].eventdat2 = bsLvl.getS16();
			eventRec[i].eventdat3 = bsLvl.getS8();
			eventRec[i].eventdat5 = bsLvl.getS8();
			eventRec[i].eventdat6 = bsLvl.getS8();
			eventRec[i].eventdat4 = bsLvl.get8();
		}
		eventRec[maxEvent].eventtime = 65500;  /*Not needed but just in case*/

		/*debuginfo('Level loaded.');*/

		/*debuginfo('Loading Map');*/

		/* MAP SHAPE LOOKUP TABLE - Each map is directly after level */
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 128; ++j)
			{
				// For some reason this data is stored with the wrong endianess.
				mapSh[i][j] = SDL_Swap16(bsLvl.get16());
			}
		}

		// Read Shapes.DAT
		Filesystem::get().openDatafileFail(shpFile, (boost::format("shapes%c.dat") % char(std::tolower(char_shapeFile))).str());
		IBinaryStream bsShp(shpFile);

		for (z = 0; z < 600; z++)
		{
			bool shapeBlank = bsShp.get8() != 0;
			if (shapeBlank)
			{
				std::fill_n(shape, COUNTOF(shape), 0);
			} else {
				bsShp.getIter16(shape, shape+COUNTOF(shape));
			}

			/* Match 1 */
			for (x = 0; x <= 71; x++)
			{
				if (mapSh[0][x] == z+1)
				{
					std::copy(shape, shape+COUNTOF(shape), megaData1->shapes[x].sh);

					ref[0][x] = (Uint8 *)megaData1->shapes[x].sh;
				}
			}

			/* Match 2 */
			for (x = 0; x <= 71; x++)
			{
				if (mapSh[1][x] == z+1)
				{
					if (x != 71 && !shapeBlank)
					{
						std::copy(shape, shape+COUNTOF(shape), megaData2->shapes[x].sh);

						y = 1;
						for (yy = 0; yy < (24 * 28) >> 1; yy++)
						{
							if (shape[yy] == 0)
							{
								y = 0;
							}
						}

						ref[1][x] = (Uint8 *)megaData2->shapes[x].sh;
					} else {
						ref[1][x] = NULL;
					}
				}
			}

			/*Match 3*/
			for (x = 0; x <= 71; x++)
			{
				if (mapSh[2][x] == z+1)
				{
					if (x < 70 && !shapeBlank)
					{
						std::copy(shape, shape+COUNTOF(shape), megaData3->shapes[x].sh);

						y = 1;
						for (yy = 0; yy < (24 * 28) >> 1; yy++)
						{
							if (shape[yy] == 0)
							{
								y = 0;
							}
						}

						ref[2][x] = (Uint8 *)megaData3->shapes[x].sh;
					} else {
						ref[2][x] = NULL;
					}
				}
			}
		}

		shpFile.close();

		for (int i = 0; i < 14*300; ++i)
		{
			mapBuf[i] = bsLvl.get8(); // TODO: Improve efficiency?
		}
		bufLoc = 0;              /* MAP NUMBER 1 */
		for (y = 0; y < 300; y++)
		{
			for (x = 0; x < 14; x++)
			{
				megaData1->mainmap[y][x] = ref[0][mapBuf[bufLoc]];
				bufLoc++;
			}
		}

		for (int i = 0; i < 14*600; ++i)
		{
			mapBuf[i] = bsLvl.get8();
		}
		bufLoc = 0;              /* MAP NUMBER 2 */
		for (y = 0; y < 600; y++)
		{
			for (x = 0; x < 14; x++)
			{
				megaData2->mainmap[y][x] = ref[1][mapBuf[bufLoc]];
				bufLoc++;
			}
		}

		for (int i = 0; i < 15*600; ++i)
		{
			mapBuf[i] = bsLvl.get8();
		}
		bufLoc = 0;              /* MAP NUMBER 3 */
		for (y = 0; y < 600; y++)
		{
			for (x = 0; x < 15; x++)
			{
				megaData3->mainmap[y][x] = ref[2][mapBuf[bufLoc]];
				bufLoc++;
			}
		}

		lvlFile.close();

		/* Note: The map data is automatically calculated with the correct mapsh
		value and then the pointer is calculated using the formula (MAPSH-1)*168.
		Then, we'll automatically add S2Ofs to get the exact offset location into
		the shape table! This makes it VERY FAST! */

		/*debuginfo('Map file done.');*/
		/* End of find loop for LEVEL??.DAT */
	} /*LoadDestruct?*/

}

void JE_titleScreen( bool animate )
{
	bool quit = 0;

	const int menunum = 7;
	unsigned char nameGo[SA + 2] = {0}; /* [1..SA+2] */
	JE_word waitForDemo;
	int menu = 0;
	bool redraw = true, fadeIn = false, first = true;
	JE_word z;

	JE_word temp;

	JE_initPlayerData();

	/*PlayCredits;*/

	highScores.sort();

	if (haltGame)
	{
		JE_tyrianHalt(0);
	}

	tempScreenSeg = VGAScreen;

	playDemo = false;

	stoppedDemo = false;

	first  = true;
	redraw = true;
	fadeIn = false;

	gameLoaded = false;
	jumpSection = false;

	if (isNetworkGame)
	{
		JE_loadPic(2, false);
		std::copy(VGAScreen, VGAScreen+scr_width*scr_height, VGAScreen2);
		JE_dString(JE_fontCenter("Waiting for other player...", SMALL_FONT_SHAPES), 140, "Waiting for other player...", SMALL_FONT_SHAPES);
		JE_showVGA();
		JE_fadeColor(10);

		network::connect();

		twoPlayerMode = true;
		if (thisPlayerNum == 1)
		{
			JE_fadeBlack(10);

			if (select_episode() && select_difficulty())
			{
				initialDifficulty = difficultyLevel;
				
				// Make it one step harder for 2-player mode
				difficultyLevel++;

				network::prepare(network::PACKET_DETAILS);
				SDLNet_Write16(episodeNum, network::packet_out_temp->data+4);
				SDLNet_Write16(difficultyLevel, network::packet_out_temp->data+6);
				network::send(8);
			}
			else
			{
				network::prepare(network::PACKET_QUIT);
				network::send(4);

				network::tyrian_halt(0, true);
			}
		}
		else
		{
			std::copy(VGAScreen2, VGAScreen2+scr_width*scr_height, VGAScreen);
			JE_dString(JE_fontCenter(networkText[3], SMALL_FONT_SHAPES), 140, networkText[3], SMALL_FONT_SHAPES);
			JE_showVGA();

			// Until opponent sends details packet
			//while (SDLNet_Read16(network::packet_in->data+0) != network::PACKET_DETAILS)
			for (;;)
			{
				service_SDL_events(false);
				JE_showVGA();

				if (network::packet_in[0] && SDLNet_Read16(network::packet_in[0]->data+0) == network::PACKET_DETAILS)
					break;

				network::update();
				network::check();

				SDL_Delay(16);
			}

			JE_initEpisode(SDLNet_Read16(network::packet_in[0]->data+4));
			difficultyLevel = SDLNet_Read16(network::packet_in[0]->data+6);
			initialDifficulty = difficultyLevel - 1;
			JE_fadeBlack(10);

			network::update();
		}

		score = 0;
		score2 = 0;

		pItems[11] = 11;

		while (!network::is_sync())
		{
			service_SDL_events(false);
			JE_showVGA();

			network::check();
			SDL_Delay(16);
		}
	}
	else
	{
		do
		{
			/* Animate instead of quickly fading in */
			if (redraw)
			{
				if (currentSong != 30) JE_playSong(30);
	
				menu = 0;
				redraw = false;
				if (animate)
				{
					if (fadeIn)
					{
						JE_fadeBlack(10);
					}
					JE_loadPic(4, false);

					JE_textShade(2, 192, "OpenTyrian Enhanced " + get_opentyrian_version(), 15, 0, PART_SHADE);
	
					memcpy(VGAScreen2, VGAScreen, scr_width * scr_height);
	
					if (moveTyrianLogoUp)
					{
						temp = 62;
					} else {
						temp = 4;
					}
	
					JE_newDrawCShapeNum(PLANET_SHAPES, 147, 11, temp);
	
	
					memcpy(colors2, colors, sizeof(colors));
					for (temp = 256-16; temp < 256; temp++)
					{
						colors[temp].r = 0;
						colors[temp].g = 0;
						colors[temp].b = 0;
					}
					colors2[temp-1].r = 0;
	
					JE_showVGA();
					JE_fadeColor(10);
	
					fadeIn = false;
	
					if (moveTyrianLogoUp)
					{
						for (temp = 61; temp >= 4; temp -= 2)
						{
							setjasondelay(2);
							memcpy(VGAScreen, VGAScreen2, scr_width * scr_height);
	
							JE_newDrawCShapeNum(PLANET_SHAPES, 147, 11, temp);
	
							JE_showVGA();
							wait_delay();
						}
					}
					moveTyrianLogoUp = false;
	
					/* Draw Menu Text on Screen */
					for (temp = 0; temp < menunum; temp++)
					{
						tempX = 104+(temp)*13;
						tempY = JE_fontCenter(menuText[temp],SMALL_FONT_SHAPES);
	
						JE_outTextAdjust(tempY-1,tempX-1,menuText[temp],15,-10,SMALL_FONT_SHAPES,false);
						JE_outTextAdjust(tempY+1,tempX+1,menuText[temp],15,-10,SMALL_FONT_SHAPES,false);
						JE_outTextAdjust(tempY+1,tempX-1,menuText[temp],15,-10,SMALL_FONT_SHAPES,false);
						JE_outTextAdjust(tempY-1,tempX+1,menuText[temp],15,-10,SMALL_FONT_SHAPES,false);
						JE_outTextAdjust(tempY,tempX,menuText[temp],15,-3,SMALL_FONT_SHAPES,false);
					}
					JE_showVGA();
	
					JE_fadeColors(colors, colors2, 0, 255, 20);
	
					memcpy(VGAScreen2, VGAScreen, scr_width * scr_height);
				}
			}
	
			memcpy(VGAScreen, VGAScreen2, scr_width * scr_height);
	
			for (temp = 0; temp < menunum; temp++)
			{
				JE_outTextAdjust(JE_fontCenter(menuText[temp], SMALL_FONT_SHAPES), 104+temp*13,
					menuText[temp], 15, -3+((temp == menu) * 2), SMALL_FONT_SHAPES, false);
			}
	
			JE_showVGA();
	
			first = false;
	
			if (trentWin)
			{
				quit = true;
				goto trentWinsGame;
			}
	
			waitForDemo = 2000;
			JE_textMenuWait(&waitForDemo, false);
	
			if (waitForDemo == 1)
			{
				playDemo = true;
				playDemoNum++;
				if (playDemoNum > 5)
					playDemoNum = 1;
			}
	
			if (keysactive[SDLK_LALT] && keysactive[SDLK_x])
			{
				quit = true;
			}
	
			if (newkey)
			{
				switch (lastkey_sym)
				{
					case SDLK_UP:
						if (menu == 0)
						{
							menu = menunum-1;
						} else {
							menu--;
						}
						JE_playSampleNum(CURSOR_MOVE);
						break;
					case SDLK_DOWN:
						if (menu == menunum-1)
						{
							menu = 0;
						} else {
							menu++;
						}
						JE_playSampleNum(CURSOR_MOVE);
						break;
					default:
						break;
				}
			}
	
			for (z = 0; z < SA+2; z++)
			{
				if (specialName[z][nameGo[z]] == toupper(lastkey_char))
				{
					nameGo[z]++;
					if (specialName[z].length() == nameGo[z])
					{
						if (z == SA)
						{
							loadDestruct = true;
						} else if (z == SA+1) {
							/* SuperTyrian */
	
							JE_playSampleNum(37);
							JE_whoa();
							JE_clr256();
							JE_outText(10, 10, "Cheat codes have been disabled.", 15, 4);
	
							if (keysactive[SDLK_SCROLLOCK])
							{
								initialDifficulty = 6;
							} else {
								initialDifficulty = 8;
							}
	
							if (initialDifficulty == 8)
							{
								JE_outText(10, 20, "Difficulty level has been set to Lord of Game.", 15, 4);
							} else {
								JE_outText(10, 20, "Difficulty level has been set to Suicide.", 15, 4);
							}
							JE_outText(10, 30, "It is imperitive that you discover the special codes.", 15, 4);
							if (initialDifficulty == 8)
							{
								JE_outText(10, 40, "(Next time, for an easier challenge hold down SCROLL LOCK.)", 15, 4);
							}
							JE_outText(10, 60, "Prepare to play...", 15, 4);
	
							std::string buf = std::string(miscTextB[4]) + " " + pName[0];
							JE_dString(JE_fontCenter(buf.c_str(), FONT_SHAPES), 110, buf.c_str(), FONT_SHAPES);
							JE_playSong(17);
							JE_playSampleNum(35);
							JE_showVGA();
	
							wait_input(true, true, true);
	
							JE_initEpisode(1);
							superTyrian = true;
							onePlayerAction = true;
							pItems[PITEM_SHIP] = 13;
							pItems[PITEM_FRONT_WEAPON] = 39;
							pItems[PITEM_SUPER_ARCADE_MODE] = 254;
							gameLoaded = true;
							difficultyLevel = initialDifficulty;
							score = 0;
						} else {
							pItems[PITEM_SUPER_ARCADE_MODE] = z+1;
							pItems[PITEM_SHIP] = SAShip[z];
							JE_fadeBlack(10);
							if (select_episode() && select_difficulty())
							{
								/* Start special mode! */
								JE_fadeBlack(10);
								JE_loadPic(1, false);
								JE_clr256();
								JE_dString(JE_fontCenter(superShips[0], FONT_SHAPES), 30, superShips[0], FONT_SHAPES);
								JE_dString(JE_fontCenter(superShips[z+1], SMALL_FONT_SHAPES), 100, superShips[z+1], SMALL_FONT_SHAPES);
								tempW = ships[pItems[PITEM_SHIP]].shipgraphic;
								if (tempW != 1)
								{
									JE_drawShape2x2(148, 70, tempW, shapes9);
								}
	
								JE_showVGA();
								JE_fadeColor(50);
	
								wait_input(true, true, true);
	
								twoPlayerMode = false;
								onePlayerAction = true;
								superArcadeMode = z+1;
								gameLoaded = true;
								score = 0;
								pItems[PITEM_FRONT_WEAPON] = SAWeapon[z][0];
								pItems[PITEM_SPECIAL] = SASpecialWeapon[z];
								if (z+1 == SA)
								{
									pItems[PITEM_LEFT_SIDEKICK] = 24;
									pItems[PITEM_RIGHT_SIDEKICK] = 24;
								}
								difficultyLevel++;
								initialDifficulty = difficultyLevel;
							} else {
								redraw = true;
								fadeIn = true;
							}
						}
						newkey = false;
					}
				} else {
					nameGo[z] = 0;
				}
			}
			lastkey_char = '\0';
	
			if (newkey)
			{
				switch (lastkey_sym)
				{
					case SDLK_ESCAPE:
						quit = true;
						break;
					case SDLK_RETURN:
						JE_playSampleNum(SELECT);
						switch (menu)
						{
							case 0: /* New game */
								JE_fadeBlack(10);
								if (select_gameplay())
								{
									if (netQuit)
									{
										JE_tyrianHalt(9);
									}
	
									if (select_episode() && select_difficulty())
									{
										gameLoaded = true;
									} else {
										redraw = true;
										fadeIn = true;
									}
	
									initialDifficulty = difficultyLevel;
	
									if (onePlayerAction)
									{
										score = 0;
										pItems[PITEM_SHIP] = 8;
									} else {
										if (twoPlayerMode)
										{
											score = 0;
											score2 = 0;
											pItems[PITEM_SHIP] = 11;
											difficultyLevel++;
											inputDevice1 = 1;
											inputDevice2 = 2;
										} else {
											if (CVars::ch_loot)
											{
												score = 1000000;
											} else {
												switch (episodeNum)
												{
													case 1:
														score = 10000;
														break;
													case 2:
														score = 15000;
														break;
													case 3:
														score = 20000;
														break;
													case 4:
														score = 30000;
														break;
												}
											}
										}
									}
								}
								fadeIn = true;
								break;
							case 1: /* Load game */
								JE_loadScreen();
								if (!gameLoaded)
								{
									redraw = true;
								}
								fadeIn = true;
								break;
							case 2: /* High scores */
								JE_highScoreScreen();
								fadeIn = true;
								break;
							case 3: /* Instructions */
								JE_helpSystem(1);
								redraw = true;
								fadeIn = true;
								break;
							case 4: /* Ordering info, now OpenTyrian menu*/
								opentyrian_menu();
								redraw = true;
								fadeIn = true;
								break;
							case 5: /* Demo */
								JE_initPlayerData();
								playDemo = true;
								if (playDemoNum++ > 4)
								{
									playDemoNum = 1;
								}
								break;
							case 6: /* Quit */
								quit = true;
								break;
						}
						redraw = true;
						break;
					default:
						break;
				}
			}
		} while (!(quit || gameLoaded || jumpSection || playDemo || loadDestruct));
	
	trentWinsGame:
		JE_fadeBlack(15);
		if (quit)
		{
			JE_tyrianHalt(0);
		}

	}
}

void JE_openingAnim( void )
{
	JE_clr256();

	moveTyrianLogoUp = true;

	if (!isNetworkGame && !stoppedDemo)
	{
		static const SDL_Color white_col = {255,255,255};
		static const SDL_Color black_col = {0,0,0};
		std::fill_n(black, 256, white_col);
		JE_fadeColors(colors, black, 0, 255, 50);

		JE_loadPic(10, false);
		JE_showVGA();

		JE_fadeColors(black, colors, 0, 255, 50);
		std::fill_n(black, 256, black_col);

		setjasondelay(200);
		while (!JE_anyButton() && (signed int)(target - SDL_GetTicks()) > 0)
			SDL_Delay(16);

		JE_fadeBlack(15);

		JE_loadPic(12, false);
		JE_showVGA();

		JE_fadeColor(10);

		setjasondelay(200);
		while (!JE_anyButton() && (signed int)(target - SDL_GetTicks()) > 0)
			SDL_Delay(16);

		JE_fadeBlack(10);
	}
}

void JE_readTextSync( void )
{
	// This function seems to be unecessary
	return;

#if 0
	JE_clr256();
	JE_showVGA();
	JE_loadPic(1, true);

	JE_barShade(3, 3, 316, 196);
	JE_barShade(1, 1, 318, 198);
	JE_dString(10, 160, "Waiting for other player.", SMALL_FONT_SHAPES);
	JE_showVGA();

	// TODO: NETWORK

	do
	{
		setjasondelay(2);

		// TODO: NETWORK

		int delaycount_temp;
		if ((delaycount_temp = target - SDL_GetTicks()) > 0)
			SDL_Delay(delaycount_temp);

	} while (0 /* TODO: NETWORK */);
#endif
}

void JE_displayText( void )
{
	/* Display Warning Text */
	tempY = 55;
	if (warningRed)
	{
		tempY = 2;
	}
	for (temp = 0; temp < levelWarningLines; temp++)
	{
		if (!ESCPressed)
		{
			JE_outCharGlow(10, tempY, levelWarningText[temp].c_str());

			if (haltGame)
			{
				JE_tyrianHalt(5);
			}

			tempY += 10;
		}
	}
	if (frameCountMax != 0)
	{
		frameCountMax = 6;
		temp = 1;
	} else {
		temp = 0;
	}
	textGlowFont = TINY_FONT;
	tempW = 184;
	if (warningRed)
	{
		tempW = 7 * 16 + 6;
	}

	JE_outCharGlow(JE_fontCenter(miscText[4], TINY_FONT), tempW, miscText[4]);

	do
	{
		if (levelWarningDisplay)
		{
			JE_updateWarning();
		}

		setjasondelay(1);

		network::keep_alive();

		int delaycount_temp;
		if ((delaycount_temp = target - SDL_GetTicks()) > 0)
			SDL_Delay(delaycount_temp);

	} while (!(JE_anyButton() || (frameCountMax == 0 && temp == 1) || ESCPressed));
	levelWarningDisplay = false;
}


void JE_makeEnemy( JE_SingleEnemyType *enemy )
{
	int temp;
	int t = 0;

	if (superArcadeMode > 0 && tempW == 534)
	{
		tempW = 533;
	}

	enemyShapeTables[5-1] = 21;   /*Coins&Gems*/
	enemyShapeTables[6-1] = 26;   /*Two-Player Stuff*/

	if (uniqueEnemy)
	{
		temp = tempI2;
		uniqueEnemy = false;
	} else {
		temp = enemyDat[tempW].shapebank;
	}

	for (a = 0; a < 6; a++)
	{
		if (temp == enemyShapeTables[a])
		{
			switch (a)
			{
				case 0:
					enemy->shapeseg = eShapes1;
					break;
				case 1:
					enemy->shapeseg = eShapes2;
					break;
				case 2:
					enemy->shapeseg = eShapes3;
					break;
				case 3:
					enemy->shapeseg = eShapes4;
					break;
				case 4:
					enemy->shapeseg = eShapes5;
					break;
				case 5:
					enemy->shapeseg = eShapes6;
					break;
			}
		}
	}

	enemy->enemydatofs = &enemyDat[tempW];

	enemy->mapoffset = 0;

	for (a = 0; a < 3; a++)
	{
		enemy->eshotmultipos[a] = 0;
	}

	temp4 = enemyDat[tempW].explosiontype;
	enemy->enemyground = ((temp4 & 0x01) == 0);
	enemy->explonum = temp4 / 2;

	enemy->launchfreq = enemyDat[tempW].elaunchfreq;
	enemy->launchwait = enemyDat[tempW].elaunchfreq;
	enemy->launchtype = enemyDat[tempW].elaunchtype % 1000;
	enemy->launchspecial = enemyDat[tempW].elaunchtype / 1000;

	enemy->xaccel = enemyDat[tempW].xaccel;
	enemy->yaccel = enemyDat[tempW].yaccel;

	enemy->xminbounce = -10000;
	enemy->xmaxbounce = 10000;
	enemy->yminbounce = -10000;
	enemy->ymaxbounce = 10000;
	/*Far enough away to be impossible to reach*/

	for (a = 0; a < 3; a++)
	{
		enemy->tur[a] = enemyDat[tempW].tur[a];
	}

	enemy->ani = enemyDat[tempW].ani;
	enemy->animin = 1;

	switch (enemyDat[tempW].animate)
	{
		case 0:
			enemy->enemycycle = 1;
			enemy->aniactive = 0;
			enemy->animax = 0;
			enemy->aniwhenfire = 0;
			break;
		case 1:
			enemy->enemycycle = 0;
			enemy->aniactive = 1;
			enemy->animax = 0;
			enemy->aniwhenfire = 0;
			break;
		case 2:
			enemy->enemycycle = 1;
			enemy->aniactive = 2;
			enemy->animax = enemy->ani;
			enemy->aniwhenfire = 2;
			break;
	}

	if (enemyDat[tempW].startxc != 0)
	{
		enemy->ex = enemyDat[tempW].startx + (mt::rand() % (enemyDat[tempW].startxc * 2)) - enemyDat[tempW].startxc + 1;
	} else {
		enemy->ex = enemyDat[tempW].startx + 1;
	}

	if (enemyDat[tempW].startyc != 0)
	{
		enemy->ey = enemyDat[tempW].starty + (mt::rand() % (enemyDat[tempW].startyc * 2)) - enemyDat[tempW].startyc + 1;
	} else {
		enemy->ey = enemyDat[tempW].starty + 1;
	}

	enemy->exc = enemyDat[tempW].xmove;
	enemy->eyc = enemyDat[tempW].ymove;
	enemy->excc = enemyDat[tempW].xcaccel;
	enemy->eycc = enemyDat[tempW].ycaccel;
	enemy->exccw = abs(enemy->excc);
	enemy->exccwmax = enemy->exccw;
	enemy->eyccw = abs(enemy->eycc);
	enemy->eyccwmax = enemy->eyccw;
	if (enemy->excc > 0)
	{
		enemy->exccadd = 1;
	} else {
		enemy->exccadd = -1;
	}
	if (enemy->eycc > 0)
	{
		enemy->eyccadd = 1;
	} else {
		enemy->eyccadd = -1;
	}

	enemy->special = false;
	enemy->iced = 0;

	if (enemyDat[tempW].xrev == 0)
	{
		enemy->exrev = 100;
	} else {
		if (enemyDat[tempW].xrev == -99)
		{
			enemy->exrev = 0;
		} else {
			enemy->exrev = enemyDat[tempW].xrev;
		}
	}
	if (enemyDat[tempW].yrev == 0)
	{
		enemy->eyrev = 100;
	} else {
		if (enemyDat[tempW].yrev == -99)
		{
			enemy->eyrev = 0;
		} else {
			enemy->eyrev = enemyDat[tempW].yrev;
		}
	}
	if (enemy->xaccel > 0)
	{
		enemy->exca = 1;
	} else {
		enemy->exca = -1;
	}
	if (enemy->yaccel > 0)
	{
		enemy->eyca = 1;
	} else {
		enemy->eyca = - 1;
	}

	enemy->enemytype = tempW;

	for (a = 0; a < 3; a++)
	{
		if (enemy->tur[a] == 252)
		{
			enemy->eshotwait[a] = 1;
		} else {
			if (enemy->tur[a] > 0)
			{
				enemy->eshotwait[a] = 20;
			} else {
				enemy->eshotwait[a] = 255;
			}
		}
	}
	for (a = 0; a < 20; a++)
	{
		enemy->egr[a] = enemyDat[tempW].egraphic[a];
	}
	enemy->size = enemyDat[tempW].esize;
	enemy->linknum = 0;
	if (enemyDat[tempW].dani < 0)
	{
		enemy->edamaged = true;
	} else {
		enemy->edamaged = false;
	}
	enemy->enemydie = enemyDat[tempW].eenemydie;

	enemy->freq[1-1] = enemyDat[tempW].freq[1-1];
	enemy->freq[2-1] = enemyDat[tempW].freq[2-1];
	enemy->freq[3-1] = enemyDat[tempW].freq[3-1];

	enemy->edani   = enemyDat[tempW].dani;
	enemy->edgr    = enemyDat[tempW].dgr;
	enemy->edlevel = enemyDat[tempW].dlevel;

	enemy->fixedmovey = 0;

	enemy->filter = 0x00;

	if (enemyDat[tempW].value > 1 && enemyDat[tempW].value < 10000)
	{
		switch (difficultyLevel)
		{
			case -1:
			case 0:
				t = (int)(enemyDat[tempW].value * 0.75f);
				break;
			case 1:
			case 2:
				t = enemyDat[tempW].value;
				break;
			case 3:
				t = (int)(enemyDat[tempW].value * 1.125f);
				break;
			case 4:
				t = (int)(enemyDat[tempW].value * 1.5f);
				break;
			case 5:
				t = enemyDat[tempW].value * 2;
				break;
			case 6:
				t = (int)(enemyDat[tempW].value * 2.5f);
				break;
			case 7:
			case 8:
				t = enemyDat[tempW].value * 4;
				break;
			case 9:
			case 10:
				t = enemyDat[tempW].value * 8;
				break;
		}
		if (t > 10000)
		{
			t = 10000;
		}
		enemy->evalue = t;
	} else {
		enemy->evalue = enemyDat[tempW].value;
	}

	t = 1;
	if (enemyDat[tempW].armor > 0)
	{

		if (enemyDat[tempW].armor != 255)
		{

			switch (difficultyLevel)
			{
				case -1:
				case 0:
					t = (int)(enemyDat[tempW].armor * 0.5f) + 1;
					break;
				case 1:
					t = (int)(enemyDat[tempW].armor * 0.75f) + 1;
					break;
				case 2:
					t = enemyDat[tempW].armor;
					break;
				case 3:
					t = (int)(enemyDat[tempW].armor * 1.2f);
					break;
				case 4:
					t = (int)(enemyDat[tempW].armor * 1.5f);
					break;
				case 5:
					t = (int)(enemyDat[tempW].armor * 1.8f);
					break;
				case 6:
					t = enemyDat[tempW].armor * 2;
					break;
				case 7:
					t = enemyDat[tempW].armor * 3;
					break;
				case 8:
					t = enemyDat[tempW].armor * 4;
					break;
				case 9:
				case 10:
					t = enemyDat[tempW].armor * 8;
					break;
			}

			if (t > 254)
			{
				t = 254;
			}

		} else {
			t = 255;
		}

		enemy->armorleft = t;
		
		a = 0;
		enemy->scoreitem = false;
	} else {
		a = 2;
		enemy->armorleft = 255;
		if (enemy->evalue != 0)
		{
			enemy->scoreitem = true;
		}
	}
	/*The returning A value indicates what to set ENEMYAVAIL to */

	if (!enemy->scoreitem)
	{
		totalEnemy++;  /*Destruction ratio*/
	}
}

void JE_createNewEventEnemy( int enemyTypeOfs, JE_word enemyOffset )
{
	int i;

	b = 0;

	for(i = enemyOffset; i < enemyOffset + 25; i++)
	{
		if (enemyAvail[i] == 1)
		{
			b = i + 1;
			break;
		}
	}

	if (b == 0)
	{
		return;
	}

	tempW = eventRec[eventLoc-1].eventdat + enemyTypeOfs;

	JE_makeEnemy(&enemy[b-1]);

	enemyAvail[b-1] = a;

	if (eventRec[eventLoc-1].eventdat2 != -99)
	{
		switch (enemyOffset)
		{
			case 0:
				enemy[b-1].ex = eventRec[eventLoc-1].eventdat2 - (mapX - 1) * 24;
				enemy[b-1].ey -= backMove2;
				break;
			case 25:
			case 75:
				enemy[b-1].ex = eventRec[eventLoc-1].eventdat2 - (mapX - 1) * 24 - 12;
				enemy[b-1].ey -= backMove;
				break;
			case 50:
				if (background3x1)
				{
					enemy[b-1].ex = eventRec[eventLoc-1].eventdat2 - (mapX - 1) * 24 - 12;
				} else {
					enemy[b-1].ex = eventRec[eventLoc-1].eventdat2 - mapX3 * 24 - 24 * 2 + 6;
				}
				enemy[b-1].ey -= backMove3;

				if (background3x1b)
				{
					enemy[b-1].ex -= 6;
				}
				break;
		}
		enemy[b-1].ey = -28;
		if (background3x1b && enemyOffset == 50)
		{
			enemy[b-1].ey += 4;
		}
	}

	if (smallEnemyAdjust && enemy[b-1].size == 0)
	{
		enemy[b-1].ex -= 10;
		enemy[b-1].ey -= 7;
	}

	enemy[b-1].ey += eventRec[eventLoc-1].eventdat5;
	enemy[b-1].eyc += eventRec[eventLoc-1].eventdat3;
	enemy[b-1].linknum = eventRec[eventLoc-1].eventdat4;
	enemy[b-1].fixedmovey = eventRec[eventLoc-1].eventdat6;
}

void JE_eventJump( JE_word jump )
{
	JE_word tempW;

	if (jump == 65535)
	{
		curLoc = returnLoc;
	} else {
		returnLoc = curLoc + 1;
		curLoc = jump;
	}
	tempW = 0;
	do {
		tempW++;
	} while (!(eventRec[tempW-1].eventtime >= curLoc));
	eventLoc = tempW - 1;
}

bool JE_searchFor/*enemy*/( int PLType )
{
	bool tempb = false;
	int temp;

	for (temp = 0; temp < 100; temp++)
	{
		if (enemyAvail[temp] == 0 && enemy[temp].linknum == PLType)
		{
			temp5 = temp + 1;
			if (galagaMode)
			{
				enemy[temp].evalue += enemy[temp].evalue;
				if (enemy[temp].evalue > 32767) enemy[temp].evalue = enemy[temp].evalue - 65536;
			}
			tempb = true;
		}
	}
	return tempb;
}

void JE_eventSystem( void )
{
	switch (eventRec[eventLoc-1].eventtype)
	{
		case 1:
			starY = eventRec[eventLoc-1].eventdat * scr_width;
			break;
		case 2:
			map1YDelay = 1;
			map1YDelayMax = 1;
			map2YDelay = 1;
			map2YDelayMax = 1;

			backMove = eventRec[eventLoc-1].eventdat;
			backMove2 = eventRec[eventLoc-1].eventdat2;
			if (backMove2 > 0)
			{
				explodeMove = backMove2;
			} else {
				explodeMove = backMove;
			}
			backMove3 = eventRec[eventLoc-1].eventdat3;

			if (backMove > 0)
			{
				stopBackgroundNum = 0;
			}
			break;
		case 3:
			backMove = 1;
			map1YDelay = 3;
			map1YDelayMax = 3;
			backMove2 = 1;
			map2YDelay = 2;
			map2YDelayMax = 2;
			backMove3 = 1;
			break;
		case 4:
			stopBackgrounds = true;
			switch (eventRec[eventLoc-1].eventdat)
			{
				case 0:
				case 1:
					stopBackgroundNum = 1;
					break;
				case 2:
					stopBackgroundNum = 2;
					break;
				case 3:
					stopBackgroundNum = 3;
					break;
			}
			break;
		case 5:
			if (enemyShapeTables[1-1] != eventRec[eventLoc-1].eventdat)
			{
				if (eventRec[eventLoc-1].eventdat > 0)
				{
					JE_loadCompShapes(&eShapes1, &eShapes1Size, shapeFile[eventRec[eventLoc-1].eventdat -1]);      /* Enemy Bank 1 */
					enemyShapeTables[1-1] = eventRec[eventLoc-1].eventdat;
				} else if (eShapes1 != NULL) {
					free(eShapes1);
					eShapes1 = NULL;
					enemyShapeTables[1-1] = 0;
				}
			}
			if (enemyShapeTables[2-1] != eventRec[eventLoc-1].eventdat2)
			{
				if (eventRec[eventLoc-1].eventdat2 > 0)
				{
					JE_loadCompShapes(&eShapes2, &eShapes2Size, shapeFile[eventRec[eventLoc-1].eventdat2-1]);      /* Enemy Bank 2 */
					enemyShapeTables[2-1] = eventRec[eventLoc-1].eventdat2;
				} else if (eShapes2 != NULL) {
					free(eShapes2);
					eShapes2 = NULL;
					enemyShapeTables[2-1] = 0;
				}
			}
			if (enemyShapeTables[3-1] != eventRec[eventLoc-1].eventdat3)
			{
				if (eventRec[eventLoc-1].eventdat3 > 0)
				{
					JE_loadCompShapes(&eShapes3, &eShapes3Size, shapeFile[eventRec[eventLoc-1].eventdat3-1]);      /* Enemy Bank 3 */
					enemyShapeTables[3-1] = eventRec[eventLoc-1].eventdat3;
				} else if (eShapes3 != NULL) {
					free(eShapes3);
					eShapes3 = NULL;
					enemyShapeTables[3-1] = 0;
				}
			}
			if (enemyShapeTables[4-1] != eventRec[eventLoc-1].eventdat4)
			{
				if (eventRec[eventLoc-1].eventdat4 > 0)
				{
					JE_loadCompShapes(&eShapes4, &eShapes4Size, shapeFile[eventRec[eventLoc-1].eventdat4-1]);      /* Enemy Bank 4 */
					enemyShapeTables[4-1] = eventRec[eventLoc-1].eventdat4;
					enemyShapeTables[5-1] = 21;
				} else if (eShapes4 != NULL) {
					free(eShapes4);
					eShapes4 = NULL;
					enemyShapeTables[4-1] = 0;
				}
			}
			break;
		case 6: /* Ground Enemy */
			JE_createNewEventEnemy(0, 25);
			break;
		case 7: /* Top Enemy */
			JE_createNewEventEnemy(0, 50);
			break;
		case 8:
			starActive = false;
			break;
		case 9:
			starActive = true;
			break;
		case 10: /* Ground Enemy 2 */
			JE_createNewEventEnemy(0, 75);
			break;
		case 11:
			if (allPlayersGone || eventRec[eventLoc-1].eventdat == 1)
				reallyEndLevel = true;
			else
				if (!endLevel)
				{
					readyToEndLevel = false;
					endLevel = true;
					levelEnd = 40;
				}
			break;
		case 12: /* Custom 4x4 Ground Enemy */
			switch (eventRec[eventLoc-1].eventdat6)
			{
				case 0:
				case 1:
					tempW4 = 25;
					break;
				case 2:
					tempW4 = 0;
					break;
				case 3:
					tempW4 = 50;
					break;
				case 4:
					tempW4 = 75;
					break;
			}
			eventRec[eventLoc-1].eventdat6 = 0;   /* We use EVENTDAT6 for the background */
			JE_createNewEventEnemy(0, tempW4);
			JE_createNewEventEnemy(1, tempW4);
			enemy[b-1].ex += 24;
			JE_createNewEventEnemy(2, tempW4);
			enemy[b-1].ey -= 28;
			JE_createNewEventEnemy(3, tempW4);
			enemy[b-1].ex += 24;
			enemy[b-1].ey -= 28;
			break;
		case 13:
			enemiesActive = false;
			break;
		case 14:
			enemiesActive = true;
			break;
		case 15: /* Sky Enemy */
			JE_createNewEventEnemy(0, 0);
			break;
		case 16:
			if (eventRec[eventLoc-1].eventdat > 9)
			{
				Console::get() << "error: event 16: bad event data" << std::endl;
			} else {
				JE_drawTextWindow(outputs[eventRec[eventLoc-1].eventdat-1]);
				soundQueue[3] = windowTextSamples[eventRec[eventLoc-1].eventdat-1];
			}
			break;
		case 17: /* Ground Bottom */
			JE_createNewEventEnemy(0, 25);
			if (b > 0)
			{
				enemy[b-1].ey = 190 + eventRec[eventLoc-1].eventdat5;
			}
			break;

		case 18: /* Sky Enemy on Bottom */
			JE_createNewEventEnemy(0, 0);
			if (b > 0)
			{
				enemy[b-1].ey = 190 + eventRec[eventLoc-1].eventdat5;
			}
			break;

		case 19: /* Enemy Global Move */
			if (eventRec[eventLoc-1].eventdat3 > 79 && eventRec[eventLoc-1].eventdat3 < 90)
			{
				temp2 = 1;
				temp3 = 100;
				temp4 = 0;
				eventRec[eventLoc-1].eventdat4 = newPL[eventRec[eventLoc-1].eventdat3 - 80];
			} else {
				switch (eventRec[eventLoc-1].eventdat3)
				{
					case 0:
						temp2 = 1;
						temp3 = 100;
						temp4 = 0;
						break;
					case 2:
						temp2 = 1;
						temp3 = 25;
						temp4 = 1;
						break;
					case 1:
						temp2 = 26;
						temp3 = 50;
						temp4 = 1;
						break;
					case 3:
						temp2 = 51;
						temp3 = 75;
						temp4 = 1;
						break;
					case 99:
						temp2 = 1;
						temp3 = 100;
						temp4 = 1;
						break;
				}
			}

			for (temp = temp2-1; temp < temp3; temp++)
			{
				if (temp4 == 1 || enemy[temp].linknum == eventRec[eventLoc-1].eventdat4)
				{
					if (eventRec[eventLoc-1].eventdat != -99)
					{
						enemy[temp].exc = eventRec[eventLoc-1].eventdat;
					}
					if (eventRec[eventLoc-1].eventdat2 != -99)
					{
						enemy[temp].eyc = eventRec[eventLoc-1].eventdat2;
					}
					if (eventRec[eventLoc-1].eventdat6 != 0)
					{
						enemy[temp].fixedmovey = eventRec[eventLoc-1].eventdat6;
					}
					if (eventRec[eventLoc-1].eventdat6 == -99)
					{
						enemy[temp].fixedmovey = 0;
					}
					if (eventRec[eventLoc-1].eventdat5 > 0)
					{
						enemy[temp].enemycycle = eventRec[eventLoc-1].eventdat5;
					}
				}
			}
			break;
		case 20: /* Enemy Global Accel */
			if (eventRec[eventLoc-1].eventdat3 > 79 && eventRec[eventLoc-1].eventdat3 < 90)
			{
				eventRec[eventLoc-1].eventdat4 = newPL[eventRec[eventLoc-1].eventdat3 - 80];
			}
			for (temp = 0; temp < 100; temp++)
			{
				if (enemyAvail[temp] != 1
				    && (enemy[temp].linknum == eventRec[eventLoc-1].eventdat4 || eventRec[eventLoc-1].eventdat4 == 0))
				{
					if (eventRec[eventLoc-1].eventdat != -99)
					{
						enemy[temp].excc = eventRec[eventLoc-1].eventdat;
						enemy[temp].exccw = abs(eventRec[eventLoc-1].eventdat);
						enemy[temp].exccwmax = abs(eventRec[eventLoc-1].eventdat);
						if (eventRec[eventLoc-1].eventdat > 0)
						{
							enemy[temp].exccadd = 1;
						} else {
							enemy[temp].exccadd = -1;
						}
					}

					if (eventRec[eventLoc-1].eventdat2 != -99)
					{
						enemy[temp].eycc = eventRec[eventLoc-1].eventdat2;
						enemy[temp].eyccw = abs(eventRec[eventLoc-1].eventdat2);
						enemy[temp].eyccwmax = abs(eventRec[eventLoc-1].eventdat2);
						if (eventRec[eventLoc-1].eventdat2 > 0)
						{
							enemy[temp].eyccadd = 1;
						} else {
							enemy[temp].eyccadd = -1;
						}
					}

					if (eventRec[eventLoc-1].eventdat5 > 0)
					{
						enemy[temp].enemycycle = eventRec[eventLoc-1].eventdat5;
					}
					if (eventRec[eventLoc-1].eventdat6 > 0)
					{
						enemy[temp].ani = eventRec[eventLoc-1].eventdat6;
						enemy[temp].animin = eventRec[eventLoc-1].eventdat5;
						enemy[temp].animax = 0;
						enemy[temp].aniactive = 1;
					}
				}
			}
			break;
		case 21:
			background3over = 1;
			break;
		case 22:
			background3over = 0;
			break;
		case 23: /* Sky Enemy on Bottom */
			JE_createNewEventEnemy(0, 50);
			if (b > 0)
			{
				enemy[b-1].ey = 180 + eventRec[eventLoc-1].eventdat5;
			}
			break;

		case 24: /* Enemy Global Animate */
			for (temp = 0; temp < 100; temp++)
			{
				if (enemy[temp].linknum == eventRec[eventLoc-1].eventdat4)
				{
					enemy[temp].aniactive = 1;
					enemy[temp].aniwhenfire = 0;
					if (eventRec[eventLoc-1].eventdat2 > 0)
					{
						enemy[temp].enemycycle = eventRec[eventLoc-1].eventdat2;
						enemy[temp].animin = enemy[temp].enemycycle;
					} else {
						enemy[temp].enemycycle = 0;
					}
					if (eventRec[eventLoc-1].eventdat > 0)
					{
						enemy[temp].ani = eventRec[eventLoc-1].eventdat;
					}
					if (eventRec[eventLoc-1].eventdat3 == 1)
					{
						enemy[temp].animax = enemy[temp].ani;
					} else {
						if (eventRec[eventLoc-1].eventdat3 == 2)
						{
							enemy[temp].aniactive = 2;
							enemy[temp].animax = enemy[temp].ani;
							enemy[temp].aniwhenfire = 2;
						}
					}
				}
			}
			break;
		case 25: /* Enemy Global Damage change */
			for (temp = 0; temp < 100; temp++)
			{
				if (eventRec[eventLoc-1].eventdat4 == 0 || enemy[temp].linknum == eventRec[eventLoc-1].eventdat4)
				{
					enemy[temp].armorleft = eventRec[eventLoc-1].eventdat;
					if (galagaMode)
					{
						enemy[temp].armorleft = ot_round(eventRec[eventLoc-1].eventdat * (difficultyLevel / 2.f));
					}
				}
			}
			break;
		case 26:
			smallEnemyAdjust = (eventRec[eventLoc-1].eventdat != 0);
			break;
		case 27: /* Enemy Global AccelRev */
			if (eventRec[eventLoc-1].eventdat3 > 79 && eventRec[eventLoc-1].eventdat3 < 90)
			{
				eventRec[eventLoc-1].eventdat4 = newPL[eventRec[eventLoc-1].eventdat3 - 80];
			}
			for (temp = 0; temp < 100; temp++)
			{
				if (eventRec[eventLoc-1].eventdat4 == 0 || enemy[temp].linknum == eventRec[eventLoc-1].eventdat4)
				{
					if (eventRec[eventLoc-1].eventdat != -99)
					{
						enemy[temp].exrev = eventRec[eventLoc-1].eventdat;
					}
					if (eventRec[eventLoc-1].eventdat2 != -99)
					{
						enemy[temp].eyrev = eventRec[eventLoc-1].eventdat2;
					}
					if (eventRec[eventLoc-1].eventdat3 != 0 && eventRec[eventLoc-1].eventdat3 < 17)
					{
						enemy[temp].filter = eventRec[eventLoc-1].eventdat3;
					}
				}
			}
			break;
		case 28:
			topEnemyOver = false;
			break;
		case 29:
			topEnemyOver = true;
			break;
		case 30:
			map1YDelay = 1;
			map1YDelayMax = 1;
			map2YDelay = 1;
			map2YDelayMax = 1;

			backMove = eventRec[eventLoc-1].eventdat;
			backMove2 = eventRec[eventLoc-1].eventdat2;
			explodeMove = backMove2;
			backMove3 = eventRec[eventLoc-1].eventdat3;
			break;
		case 31: /* Enemy Fire Override */
			for (temp = 0; temp < 100; temp++)
			{
				if (eventRec[eventLoc-1].eventdat4 == 99 || enemy[temp].linknum == eventRec[eventLoc-1].eventdat4)
				{
					enemy[temp].freq[1-1] = eventRec[eventLoc-1].eventdat ;
					enemy[temp].freq[2-1] = eventRec[eventLoc-1].eventdat2;
					enemy[temp].freq[3-1] = eventRec[eventLoc-1].eventdat3;
					for (temp2 = 0; temp2 < 3; temp2++)
					{
						enemy[temp].eshotwait[temp2] = 1;
					}
					if (enemy[temp].launchtype > 0)
					{
						enemy[temp].launchfreq = eventRec[eventLoc-1].eventdat5;
						enemy[temp].launchwait = 1;
					}
				}
			}
			break;
		case 32:
			JE_createNewEventEnemy(0, 50);
			if (b > 0)
			{
				enemy[b-1].ey = 190;
			}
			break;
		case 33: /* Enemy From other Enemies */
			if (!((eventRec[eventLoc-1].eventdat == 512 || eventRec[eventLoc-1].eventdat == 513) && (twoPlayerMode || onePlayerAction || superTyrian)))
			{
				if (superArcadeMode > 0)
				{
					if (eventRec[eventLoc-1].eventdat == 534)
					{
						eventRec[eventLoc-1].eventdat = 827;
					}
				} else {
					if (eventRec[eventLoc-1].eventdat == 533
					    && (portPower[1-1] == 11 || (signed)(mt::rand() % 15) < portPower[1-1])
					    && !superTyrian)
					{
						eventRec[eventLoc-1].eventdat = 829 + (mt::rand() % 6);
					}
				}
				if (eventRec[eventLoc-1].eventdat == 534 && superTyrian)
				{
					eventRec[eventLoc-1].eventdat = 828 + superTyrianSpecials[mt::rand() % 4];
				}

				for (temp = 0; temp < 100; temp++)
				{
					if (enemy[temp].linknum == eventRec[eventLoc-1].eventdat4)
					{
						enemy[temp].enemydie = eventRec[eventLoc-1].eventdat;
					}
				}
			}
			break;
		case 34: /* Start Music Fade */
			if (firstGameOver)
			{
				musicFade = true;
				tempVolume = CVars::snd_music_vol;
			}
			break;
		case 35: /* Play new song */
			if (firstGameOver)
			{
				JE_playSong(eventRec[eventLoc-1].eventdat);
				if (CVars::snd_enabled)
				{
					JE_selectSong(0);
				}
			}
			musicFade = false;
			break;
		case 36:
			readyToEndLevel = true;
			break;
		case 37:
			levelEnemyFrequency = eventRec[eventLoc-1].eventdat;
			break;
		case 38:
			curLoc = eventRec[eventLoc-1].eventdat;
			tempW2 = 1;
			for (tempW = 0; tempW < maxEvent; tempW++)
			{
				if (eventRec[tempW].eventtime <= curLoc)
				{
					tempW2 = tempW+1 - 1;
				}
			}
			eventLoc = tempW2;
			break;
		case 39: /* Enemy Global Linknum Change */
			for (temp = 0; temp < 100; temp++)
			{
				if (enemy[temp].linknum == eventRec[eventLoc-1].eventdat)
				{
					enemy[temp].linknum = eventRec[eventLoc-1].eventdat2;
				}
			}
			break;
		case 40: /* Enemy Continual Damage */
			enemyContinualDamage = true;
			break;
		case 41:
			if (eventRec[eventLoc-1].eventdat == 0)
			{
				std::fill(enemyAvail, enemyAvail+COUNTOF(enemyAvail), 1);
			} else {
				for (JE_word x = 0; x <= 24; x++)
				{
					enemyAvail[x] = 1;
				}
			}
			break;
		case 42:
			background3over = 2;
			break;
		case 43:
			background2over = eventRec[eventLoc-1].eventdat;
			break;
		case 44:
			filterActive       = (eventRec[eventLoc-1].eventdat > 0);
			filterFade         = (eventRec[eventLoc-1].eventdat == 2);
			levelFilter        = eventRec[eventLoc-1].eventdat2;
			levelBrightness    = eventRec[eventLoc-1].eventdat3;
			levelFilterNew     = eventRec[eventLoc-1].eventdat4;
			levelBrightnessChg = eventRec[eventLoc-1].eventdat5;
			filterFadeStart    = (eventRec[eventLoc-1].eventdat6 == 0);
			break;
		case 45: /* Two Player Enemy from other Enemies */
			if (!superTyrian)
			{
				if (eventRec[eventLoc-1].eventdat == 533
				    && (portPower[1-1] == 11 || (signed)(mt::rand() % 15) < portPower[1-1]))
				{
					eventRec[eventLoc-1].eventdat = 829 + (mt::rand() % 6);
				}
				if (twoPlayerMode || onePlayerAction)
				{
					for (temp = 0; temp < 100; temp++)
					{
						if (enemy[temp].linknum == eventRec[eventLoc-1].eventdat4)
						{
							enemy[temp].enemydie = eventRec[eventLoc-1].eventdat;
						}
					}
				}
			}
			break;
		case 46:
			if (eventRec[eventLoc-1].eventdat3 != 0)
			{
				damageRate = eventRec[eventLoc-1].eventdat3;
			}
			if (eventRec[eventLoc-1].eventdat2 == 0 || twoPlayerMode || onePlayerAction)
			{
				difficultyLevel += eventRec[eventLoc-1].eventdat;
				if (difficultyLevel < 1)
				{
					difficultyLevel = 1;
				}
				if (difficultyLevel > 10)
				{
					difficultyLevel = 10;
				}
			}
			break;
		case 47: /* Enemy Global AccelRev */
			for (temp = 0; temp < 100; temp++)
			{
				if (eventRec[eventLoc-1].eventdat4 == 0 || enemy[temp].linknum == eventRec[eventLoc-1].eventdat4)
				{
					enemy[temp].armorleft = eventRec[eventLoc-1].eventdat;
				}
			}
			break;
		case 48: /* Background 2 Cannot be Transparent */
			background2notTransparent = true;
			break;
		case 49:
		case 50:
		case 51:
		case 52:
			tempDat2 = eventRec[eventLoc-1].eventdat;
			eventRec[eventLoc-1].eventdat = 0;
			tempDat = eventRec[eventLoc-1].eventdat3;
			eventRec[eventLoc-1].eventdat3 = 0;
			tempDat3 = eventRec[eventLoc-1].eventdat6;
			eventRec[eventLoc-1].eventdat6 = 0;
			tempI2 = tempDat;
			enemyDat[0].armor = (Uint8)tempDat3;
			enemyDat[0].egraphic[1-1] = tempDat2;
			switch (eventRec[eventLoc-1].eventtype - 48)
			{
				case 1:
					temp = 25;
					break;
				case 2:
					temp = 0;
					break;
				case 3:
					temp = 50;
					break;
				case 4:
					temp = 75;
					break;
			}
			uniqueEnemy = true;
			JE_createNewEventEnemy(0, temp);
			uniqueEnemy = false;
			eventRec[eventLoc-1].eventdat = tempDat2;
			eventRec[eventLoc-1].eventdat3 = tempDat;
			eventRec[eventLoc-1].eventdat6 = tempDat3;
			break;

		case 53:
			forceEvents = (eventRec[eventLoc-1].eventdat != 99);
			break;
		case 54:
			JE_eventJump(eventRec[eventLoc-1].eventdat);
			break;
		case 55: /* Enemy Global AccelRev */
			if (eventRec[eventLoc-1].eventdat3 > 79 && eventRec[eventLoc-1].eventdat3 < 90)
			{
				eventRec[eventLoc-1].eventdat4 = newPL[eventRec[eventLoc-1].eventdat3 - 80];
			}
			for (temp = 0; temp < 100; temp++)
			{
				if (eventRec[eventLoc-1].eventdat4 == 0 || enemy[temp].linknum == eventRec[eventLoc-1].eventdat4)
				{
					if (eventRec[eventLoc-1].eventdat != -99)
					{
						enemy[temp].xaccel = eventRec[eventLoc-1].eventdat;
					}
					if (eventRec[eventLoc-1].eventdat2 != -99)
					{
						enemy[temp].yaccel = eventRec[eventLoc-1].eventdat2;
					}
				}
			}
			break;
		case 56: /* Ground2 Bottom */
			JE_createNewEventEnemy(0, 75);
			if (b > 0)
			{
				enemy[b-1].ey = 190;
			}
			break;
		case 57:
			superEnemy254Jump = eventRec[eventLoc-1].eventdat;
			break;

		case 60: /*Assign Special Enemy*/
			for (temp = 0; temp < 100; temp++)
			{
				if (enemy[temp].linknum == eventRec[eventLoc-1].eventdat4)
				{
					enemy[temp].special = true;
					enemy[temp].flagnum = eventRec[eventLoc-1].eventdat;
					enemy[temp].setto  = (eventRec[eventLoc-1].eventdat2 == 1);
				}
			}
			break;

		case 61: /*If Flag then...*/
			if (globalFlags[eventRec[eventLoc-1].eventdat] == (eventRec[eventLoc-1].eventdat2 != 0))
			{
				eventLoc += eventRec[eventLoc-1].eventdat3;
			}
			break;
		case 62: /*Play sound effect*/
			soundQueue[3] = eventRec[eventLoc-1].eventdat;
			break;

		case 63: /*Skip X events if not in 2-player mode*/
			if (!twoPlayerMode && !onePlayerAction)
			{
				eventLoc += eventRec[eventLoc-1].eventdat;
			}
			break;

		case 64:
			if (!(eventRec[eventLoc-1].eventdat == 6 && twoPlayerMode && difficultyLevel > 2))
			{
				smoothies[eventRec[eventLoc-1].eventdat-1] = (eventRec[eventLoc-1].eventdat2 != 0);
				temp = eventRec[eventLoc-1].eventdat;
				if (temp == 5)
					temp = 3;
				SDAT[temp-1] = eventRec[eventLoc-1].eventdat3;
			}
			break;

		case 65:
			background3x1 = (eventRec[eventLoc-1].eventdat == 0);
			break;
		case 66: /*If not on this difficulty level or higher then...*/
			if (initialDifficulty <= static_cast<unsigned int>(eventRec[eventLoc-1].eventdat))
				eventLoc += eventRec[eventLoc-1].eventdat2;
			break;
		case 67:
			levelTimer = (eventRec[eventLoc-1].eventdat == 1);
			levelTimerCountdown = eventRec[eventLoc-1].eventdat3 * 100;
			levelTimerJumpTo   = eventRec[eventLoc-1].eventdat2;
			break;
		case 68:
			randomExplosions = (eventRec[eventLoc-1].eventdat == 1);
			break;
		case 69:
			playerInvulnerable1 = eventRec[eventLoc-1].eventdat;
			playerInvulnerable2 = eventRec[eventLoc-1].eventdat;
			break;

		case 70:
			if (eventRec[eventLoc-1].eventdat2 == 0)
			{  /*1-10*/
				tempB = false;
				for (temp = 1; temp <= 19; temp++)
				{
					tempB = tempB | JE_searchFor(temp);
				}
				if (!tempB)
				{
					JE_eventJump(eventRec[eventLoc-1].eventdat);
				}
			} else {
				if (!JE_searchFor(eventRec[eventLoc-1].eventdat2)
				    && (eventRec[eventLoc-1].eventdat3 == 0 || !JE_searchFor(eventRec[eventLoc-1].eventdat3))
				    && (eventRec[eventLoc-1].eventdat4 == 0 || !JE_searchFor(eventRec[eventLoc-1].eventdat4)))
					JE_eventJump(eventRec[eventLoc-1].eventdat);
			}
			break;

		case 71:
			Console::get() << "warning: event 71: possibly bad map repositioning" << std::endl;
			if (((((intptr_t)mapYPos - (intptr_t)&megaData1->mainmap) / sizeof(Uint8 *)) * 2) <= (unsigned int)eventRec[eventLoc-1].eventdat2) /* <MXD> ported correctly? */
			{
				JE_eventJump(eventRec[eventLoc-1].eventdat);
			}
			break;

		case 72:
			background3x1b = (eventRec[eventLoc-1].eventdat == 1);
			break;

		case 73:
			skyEnemyOverAll = (eventRec[eventLoc-1].eventdat == 1);
			break;

		case 74: /* Enemy Global BounceParams */
			for (temp = 0; temp < 100; temp++)
			{
				if (eventRec[eventLoc-1].eventdat4 == 0 || enemy[temp].linknum == eventRec[eventLoc-1].eventdat4)
				{
					if (eventRec[eventLoc-1].eventdat5 != -99)
					{
						enemy[temp].xminbounce = eventRec[eventLoc-1].eventdat5;
					}
					if (eventRec[eventLoc-1].eventdat6 != -99)
					{
						enemy[temp].yminbounce = eventRec[eventLoc-1].eventdat6;
					}
					if (eventRec[eventLoc-1].eventdat != -99)
					{
						enemy[temp].xmaxbounce = eventRec[eventLoc-1].eventdat ;
					}
					if (eventRec[eventLoc-1].eventdat2 != -99)
					{
						enemy[temp].ymaxbounce = eventRec[eventLoc-1].eventdat2;
					}
				}
			}
			break;

		case 75:

			tempB = false;
			for (temp = 0; temp < 100; temp++)
			{
				if (enemyAvail[temp] == 0
				    && enemy[temp].eyc == 0
				    && enemy[temp].linknum >= eventRec[eventLoc-1].eventdat
				    && enemy[temp].linknum <= eventRec[eventLoc-1].eventdat2)
				{
					tempB = true;
				}
			}

			if (tempB)
			{
				do {
					temp = (mt::rand() % (eventRec[eventLoc-1].eventdat2 + 1 - eventRec[eventLoc-1].eventdat)) + eventRec[eventLoc-1].eventdat;
				} while (!(JE_searchFor(temp) && enemy[temp5-1].eyc == 0));

				newPL[eventRec[eventLoc-1].eventdat3 - 80] = temp;
			} else {
				newPL[eventRec[eventLoc-1].eventdat3 - 80] = 255;
				if (eventRec[eventLoc-1].eventdat4 > 0)
				{ /*Skip*/
					curLoc = eventRec[eventLoc-1 + eventRec[eventLoc-1].eventdat4].eventtime - 1;
					eventLoc += eventRec[eventLoc-1].eventdat4 - 1;
				}
			}

			break;

		case 76:
			returnActive = true;
			break;

		case 77:
			Console::get() << "warning: event 77: possibly bad map repositioning" << std::endl;
			mapYPos = &megaData1->mainmap[0][0];
			mapYPos += eventRec[eventLoc-1].eventdat / 2;
			if (eventRec[eventLoc-1].eventdat2 > 0)
			{
				mapY2Pos = &megaData2->mainmap[0][0];
				mapY2Pos += eventRec[eventLoc-1].eventdat2 / 2;
			} else {
				mapY2Pos = &megaData2->mainmap[0][0];
				mapY2Pos += eventRec[eventLoc-1].eventdat / 2;
			}
			break;

		case 78:
			if (galagaShotFreq < 10)
			{
				galagaShotFreq++;
			}
			break;

		case 79:
			statBar[1-1] = eventRec[eventLoc-1].eventdat;
			statBar[2-1] = eventRec[eventLoc-1].eventdat2;
			break;

		case 80: /*Skip X events if not in 2-player mode*/
			if (twoPlayerMode)
			{
				eventLoc += eventRec[eventLoc-1].eventdat;
			}
			break;

		case 81: /*WRAP2*/
			Console::get() << "warning: event 81: possibly bad map repositioning" << std::endl;
			BKwrap2   = &megaData2->mainmap[0][0];
			BKwrap2   += eventRec[eventLoc-1].eventdat / 2;
			BKwrap2to = &megaData2->mainmap[0][0];
			BKwrap2to += eventRec[eventLoc-1].eventdat2 / 2;
			break;

		case 82: /*Give SPECIAL WEAPON*/
			pItems[PITEM_SPECIAL] = eventRec[eventLoc-1].eventdat;
			shotMultiPos[9-1] = 0;
			shotRepeat[9-1] = 0;
			shotMultiPos[11-1] = 0;
			shotRepeat[11-1] = 0;
			break;
		default:
			Console::get() << "warning: event " << eventRec[eventLoc-1].eventtype << ": unhandled event" << std::endl;
			break;
	}

	eventLoc++;
}


void JE_whoa( void )
{
	memcpy(VGAScreen2, VGAScreen, scr_height * scr_width);
	memset(VGAScreen, 0, scr_height * scr_width);

	tempW3 = 300;

	do
	{
		setjasondelay(1);

		Uint16 di = 640; // pixel pointer

		Uint8 *vga2pixels = (Uint8 *)VGAScreen2;
		for (Uint16 dx = 64000 - 1280; dx != 0; dx--)
		{
			Uint16 si = di + (Uint8)((Uint8)(dx >> 8) >> 5) - 4;

			Uint16 ax = vga2pixels[si] * 12;
			ax += vga2pixels[si-320];
			ax += vga2pixels[si-1];
			ax += vga2pixels[si+1];
			ax += vga2pixels[si+320];
			ax >>= 4;

			vga2pixels[di] = (Uint8)ax;

			di++;
		}

		di = 320 * 4;
		for (Uint16 cx = 64000 - 320*7; cx != 0; cx--)
		{
			((Uint8 *)VGAScreen)[di] = vga2pixels[di];
			di++;
		}

		tempW3--;

		wait_delay();
		JE_showVGA();
	} while (!(tempW3 == 0 || JE_anyButton()));

	levelWarningLines = 4;
}

void JE_barX( JE_word x1, JE_word y1, JE_word x2, JE_word y2, Uint8 col )
{
	JE_bar(x1, y1,     x2, y1,     col + 1);
	JE_bar(x1, y1 + 1, x2, y2 - 1, col    );
	JE_bar(x1, y2,     x2, y2,     col - 1);
}

void JE_doStatBar( void )
{
	for (temp2 = 0; temp2 < 2; temp2++)
	{
		if (statBar[temp2] > 0)
		{
			statDmg[temp2] = 256;  /*Higher than maximum*/
			for (temp = 0; temp < 100; temp++)
			{
				if (enemy[temp].linknum == statBar[temp2] && enemyAvail[temp] != 1)
				{
					if (enemy[temp].armorleft < statDmg[temp2])
					{
						statDmg[temp2] = enemy[temp].armorleft;
					}
				}
			}
			if (statDmg[temp2] == 256 || statDmg[temp2] == 0)
			{
				statBar[temp2] = 0;
			}
		}
	}
	
	if (statBar[1-1] == 0)
	{
		statBar[1-1] = statBar[2-1];
		statDmg[1-1] = statDmg[2-1];
		statBar[2-1] = 0;
	}
	
	if (statBar[2-1] > 0)
	{  /*2 bars*/
		JE_barX(100, 7, 150, 12, 115);
		JE_barX(125 - (statDmg[1-1] / 10), 7, 125 + (statDmg[1-1] + 5) / 10, 12, 118 + statCol[1-1]);
		JE_barX(160, 7, 210, 12, 115);
		JE_barX(185 - (statDmg[2-1] / 10), 7, 185 + (statDmg[2-1] + 5) / 10, 12, 118 + statCol[2-1]);
	} else if (statBar[1-1] > 0) {  /*1 bar*/
		tempW = 155;
		if (levelTimer)
		{
			tempW = 250;
		}
		JE_barX(tempW - 26, 7, tempW + 26, 12, 115);
		JE_barX(tempW - (statDmg[1-1] / 10), 7, tempW + (statDmg[1-1] + 5) / 10, 12, 118 + statCol[1-1]);
	}
	
	if (statCol[1-1] > 0)
	{
		statCol[1-1]--;
	}
	if (statCol[2-1] > 0)
	{
		statCol[2-1]--;
	}
}
