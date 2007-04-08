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

#include "sndmast.h"

const char soundTitle[SoundNum + 9][9] = /* [1..soundnum + 9] of string [8] */
{
    "SCALEDN2",        /*1*/
    "F2",              /*2*/
    "TEMP10",
    "EXPLSM",
    "PASS3",           /*5*/
    "TEMP2",
    "BYPASS1",
    "EXP1RT",
    "EXPLLOW",
    "TEMP13",          /*10*/
    "EXPRETAP",
    "MT2BOOM",
    "TEMP3",
    "LAZB",     /*28K*/
    "LAZGUN2",         /*15*/
    "SPRING",
    "WARNING",
    "ITEM",
    "HIT2",     /*14K*/
    "MACHNGUN",        /*20*/
    "HYPERD2",
    "EXPLHUG",
    "CLINK1",
    "CLICK",
    "SCALEDN1",        /*25*/
    "TEMP11",
    "TEMP16",
    "SMALL1",
    "POWERUP",
    "VOICE1",
    "VOICE2",
    "VOICE3",
    "VOICE4",
    "VOICE5",
    "VOICE6",
    "VOICE7",
    "VOICE8",
    "VOICE9"
};

const JE_byte WindowTextSamples[9] = /* [1..9] */
{
    SoundNum + 6,
    SoundNum + 2,
    SoundNum + 3,
    SoundNum + 1,
    SoundNum + 6,
    SoundNum + 7,
    SoundNum + 9,
    SoundNum + 6,
    SoundNum + 3
};


/*
XCLEARED.SMP      *Cleared Enemy Platforms
SU2ENEMY.SMP      *Large Enemy Ahead
LARGEE.SMP        *Incoming Enemies -> Large Enemy Ahead
SU1LUCK.SMP       *Good Luck
XLEVEL.SMP        *Level Completed
WARNING.SMP       *Warning (Enemies approaching from behind)
SU3SPIKE.SMP      *Warning: Spikes Ahead
DATA1.SMP         *Data Cube collected
SPEEDINC.SMP      *Unexplained Speed Increase
*/
