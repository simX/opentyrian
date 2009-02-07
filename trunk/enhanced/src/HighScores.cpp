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
#include "HighScores.h"

#include <algorithm>
#include <functional>

const unsigned long HighScore::defaultScores[3] = {5000, 2500, 1000};

const char *HighScore::defaultHighScoreNames[] =
{/*1P*/
/*TYR*/   "The Prime Chair", /*13*/
          "Transon Lohk",
          "Javi Onukala",
          "Mantori",
          "Nortaneous",
          "Dougan",
          "Reid",
          "General Zinglon",
          "Late Gyges Phildren",
          "Vykromod",
          "Beppo",
          "Borogar",
          "ShipMaster Carlos",

/*OTHER*/ "Jill", /*5*/
          "Darcy",
          "Jake Stone",
          "Malvineous Havershim",
          "Marta Louise Velasquez",

/*JAZZ*/  "Jazz Jackrabbit", /*3*/
          "Eva Earlong",
          "Devan Shell",

/*OMF*/   "Crystal Devroe", /*11*/
          "Steffan Tommas",
          "Milano Angston",
          "Christian",
          "Shirro",
          "Jean-Paul",
          "Ibrahim Hothe",
          "Angel",
          "Cossette Akira",
          "Raven",
          "Hans Kreissack",

/*DARE*/  "Tyler", /*2*/
          "Rennis the Rat Guard"
};

const char *HighScore::defaultTeamNames[] =
{
	"Jackrabbits",
	"Team Tyrian",
	"The Elam Brothers",
	"Dare to Dream Team",
	"Pinball Freaks",
	"Extreme Pinball Freaks",
	"Team Vykromod",
	"Epic All-Stars",
	"Hans Keissack's WARriors",
	"Team Overkill",
	"Pied Pipers",
	"Gencore Growlers",
	"Microsol Masters",
	"Beta Warriors",
	"Team Loco",
	"The Shellians",
	"Jungle Jills",
	"Murderous Malvineous",
	"The Traffic Department",
	"Clan Mikal",
	"Clan Patrok",
	"Carlos' Crawlers"
};

void HighScore::serialize( OBinaryStream& f ) const
{
	f.put32(mScore);
	f.put(mName);
	f.put8(mDifficulty);
}

HighScores::HighScores( )
{
	for (int i = 0; i < 4; i++) // Episode
	{
		for (int j = 0; j < 2; j++) // One or two players
		{
			for (int k = 0; k < 3; k++) // Place
			{
				mScores[i][j][k] = HighScore(i, j == 1, k);
			}
		}
	}
}

void HighScores::serialize( OBinaryStream& f ) const
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				mScores[i][j][k].serialize(f);
			}
		}
	}
}

void HighScores::unserialize( IBinaryStream& f )
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				mScores[i][j][k] = HighScore(f);
			}
		}
	}
}

void HighScores::sort( )
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			std::sort(mScores[i][j], mScores[i][j]+3, std::greater<HighScore>());
		}
	}
}

int HighScores::insertScore( const int episode, const int players, const HighScore& score, bool dry_run )
{
	sort();
	
	HighScore(& arr)[3] = mScores[episode][players];

	for (int i = 0; i < 3; i++)
	{
		if (arr[i] < score)
		{
			if (!dry_run)
			{
				arr[i] = score;
			}
			return i;
		}
	}

	return -1;
}

HighScores highScores;

