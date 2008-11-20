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
#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include "BinaryStream.h"
#include "mtrand.h"

#include <string>

extern const char defaultHighScoreNames[34][23];
extern const char defaultTeamNames[22][25];

class HighScore
{
private:
	static const unsigned long defaultScores[3];
	static const char *defaultHighScoreNames[34];
	static const char *defaultTeamNames[22];

	unsigned long mScore;
	std::string mName;
	unsigned char mDifficulty;
public:
	HighScore( const unsigned long score = 0, const std::string& name = "", const unsigned char difficulty = 0 )
		: mScore(score), mName(name), mDifficulty(difficulty)
	{}

	HighScore( const int episode, const bool two_player, const int place )
		: mScore(!two_player ? defaultScores[place]*(episode+1) : defaultScores[place]*(episode+1)/2),
		  mName(!two_player ? defaultHighScoreNames[mt::rand() % 34] : defaultTeamNames[rand() % 22]),
		  mDifficulty(0)
	{}

	HighScore( IBinaryStream& f )
		: mScore(f.get32()), mName(f.getStr()), mDifficulty(f.get8())
	{}

	void serialize( OBinaryStream& f ) const;

	bool operator<( const HighScore& o ) const
	{
		return getScore() < o.getScore();
	}
	bool operator>( const HighScore& o ) const
	{
		return getScore() > o.getScore();
	}


	unsigned long getScore() const { return mScore; }
	std::string getName() const { return mName; }
	unsigned char getDifficulty() const { return mDifficulty; }

	void setScore( unsigned long score ) { mScore = score; }
	void setName( const std::string& name ) { mName = name; }
	void setDifficulty( unsigned char difficulty ) { mDifficulty = difficulty; }
};

class HighScores
{
private:
	HighScore mScores[4][2][3];
public:
	HighScores( );
	HighScores( IBinaryStream& f )
	{
		unserialize(f);
	}
	void serialize( OBinaryStream& f ) const;
	void unserialize( IBinaryStream& f );

	void sort( );

	int insertScore( const int episode, const int players, const HighScore& score, bool dry_run = false );

	HighScore& getScore( const int episode, const int players, const int place ) { return mScores[episode][players][place]; }
};

extern HighScores highScores;

#endif // HIGHSCORES_H
