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
#include "nortvars.h"
#include "BinaryStream.h"
#include <fstream>
#include "Filesystem.h"

#include "editship.h"

static const int SAS = sizeof(JE_ShipsType)-4;

const Uint8 extraCryptKey[10] = {58, 23, 16, 192, 254, 82, 113, 147, 62, 99};

bool extraAvail;
JE_ShipsType extraShips;
Uint8* extraShapes;
unsigned long extraShapeSize;

void JE_decryptShips()
{
	JE_ShipsType s2;

	for (int i = SAS-1; i >= 0; --i) {
		s2[i] = extraShips[i] ^	extraCryptKey[(i+1)%10];
		if (i > 0) s2[i] ^= extraShips[i-1];
	} // <= Key Decryption Test (Reversed Key)

	bool correct = true;

	Uint8 sum = 0;
	for (int i = 0; i < SAS; ++i) {
		sum += s2[i]; // <overflow>
	}
	if (sum != extraShips[SAS+0]) correct = false;

	sum = 0;
	for (int i = 0; i < SAS; ++i) {
		sum -= s2[i];
	}
	if (sum != extraShips[SAS+1]) correct = false;

	sum = 1;
	for (int i = 0; i < SAS; ++i) {
		sum = (sum*s2[i])+1; // <overflow>
	}
	if (sum != extraShips[SAS+2]) correct = false;

	sum = 0;
	for (int i = 0; i < SAS; ++i) {
		sum ^= s2[i];
	}
	if (sum != extraShips[SAS+3]) correct = false;

	if (!correct) exit(255); // TODO: Add better diagnostic and error handling

	std::copy(s2, s2+COUNTOF(s2), extraShips);
}

void JE_encryptShips()
{
	STUB();
}

void JE_compressShapeFile()
{
	STUB();
}

void JE_buildRec()
{
	STUB();
}

void JE_startNewShape()
{
	STUB();
}

void JE_add( Uint8 nextbyte )
{
	STUB();
}

void JE_endShape()
{
	STUB();
}

void JE_loadExtraShapes()
{
	std::fstream file;
	try {
		file.open(Filesystem::get().findFile("newsh$.shp").c_str(), std::ios_base::in | std::ios_base::binary);
	} catch (Filesystem::FileOpenErrorException&) {
		Console::get() << "Shipedit data not found." << std::endl;
		return;
	}

	IBinaryStream s(file);

	extraAvail = true;
	extraShapeSize = s.getSize() - sizeof(extraShips);
	extraShapes = new Uint8[extraShapeSize];

	const std::vector<Uint8>& vec1 = s.getArray(extraShapeSize);
	std::copy(vec1.begin(), vec1.end(), extraShapes);

	const std::vector<Uint8>& vec2 = s.getArray(sizeof(extraShips));
	std::copy(vec2.begin(), vec2.end(), extraShips);

	// TODO: Maybe decrypt ships can directly take the vectors so a std::copy can be avoided
	JE_decryptShips();
}
