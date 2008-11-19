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
#include "pcxload.h"

#include "picload.h"
#include "BinaryStream.h"
#include "video.h"
#include "Filesystem.h"

#include <fstream>

void JE_loadPCX( const std::string& file )
{
	if (file != "tshp2.pcx")
	{
		Console::get() << "\a7Warning:\ax JE_loadPCX used with file other than tshp2.pcx" << std::endl;
	}

	std::fstream fstr;
	using std::ios;
	fstr.exceptions(ios::badbit | ios::failbit | ios::eofbit);
	Filesystem::get().openDatafileFail(fstr, file);
	IBinaryStream f(fstr);

	try
	{
		if (f.get8() != 10 || // PCX magic number
			f.get8() != 5 || // Version 5
			f.get8() != 1 || // RLE encoding
			f.get8() != 8 || // 8 bpp
			f.get16() != 0 || f.get16() != 0 || f.get16() != 319 || f.get16() != 199) // 320x200 image
		{
			throw ios::failure("PCX error");
		} else {
			fstr.seekg(64); // Skip HDpi, VDpi and colormap
			if (f.get8() != 0 || // Reserved
				f.get8() != 1 || // 1 plane
				f.get16() != 320) // 320 bytes per line
			{
				throw ios::failure("PCX error");
			}
		}

		fstr.seekg(128);

		// Decode image
		Uint8* s = VGAScreen;
		for (int line = 0; line < 200; ++line)
		{
			int x = 0;
			while (x < 320)
			{
				Uint8 byte = f.get8();
				if ((byte & 0xc0) == 0xc0)
				{
					// Run
					int count = byte & ~0xc0;
					Uint8 color = f.get8();
					for (int i = 0; i < count; ++i)
					{
						s[x+i] = color;
					}
					x += count;
				} else {
					// Single byte
					s[x] = byte;
					++x;
				}
			}
			s += 320;
		}

		// Read palette
		if (f.get8() != 12) // Palette magic number
		{
			throw ios::failure("PCX error"); // I'm hijacking ios::failure for my own evil deeds here. Mwahwahwahwa!
		}

		for (int col = 0; col < 256; ++col)
		{
			colors[col].r = f.get8();
			colors[col].g = f.get8();
			colors[col].b = f.get8();
		}
	} catch (ios::failure& e) {
		Console::get() << "\a7Error:\ax Failed to load PCX file " << file << ". JE_loadPCX can only load a specific PCX format, see pcxload.cpp for more info. (Reason: " << e.what() << ")" << std::endl;

		// This is probably a non-fatal error, so lets just load another pic instead and continue
		JE_loadPic(2, false);
		return;
	}
}
