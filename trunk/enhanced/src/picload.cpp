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
#include "picload.h"

#include "error.h"
#include "nortvars.h"
#include "palette.h"
#include "video.h"
#include "Filesystem.h"
#include "BinaryStream.h"

#include <string.h>

static const unsigned int PCX_NUM = 13;

static long pcxpos[PCX_NUM+1];

static const int pcxpal[PCX_NUM] = {0, 7, 5, 8, 10, 5, 18, 19, 19, 20, 21, 22, 5};

void JE_loadPic( unsigned int PCXnumber, bool storepal )
{
	if (PCXnumber > PCX_NUM)
	{
		Console::get() << "\a7Error:\ax Tried to load non-existent PCX background " << PCXnumber << std::endl;
		return;
	}

	typedef Uint8 JE_buftype[63000]; /* [1..63000] */
	static bool notYetLoadedPCX = true;

	PCXnumber--;

	std::fstream f;
	Filesystem::get().openDatafileFail(f, "tyrian.pic");
	IBinaryStream bs(f);

	/*Same as old AnalyzePic*/
	if (notYetLoadedPCX)
	{
		notYetLoadedPCX = false;
		bs.get16();
		for (unsigned int i = 0; i < PCX_NUM; ++i)
		{
			pcxpos[i] = bs.getS32();
		}
		f.seekg(0, std::ios::end);
		pcxpos[PCX_NUM] = f.tellg();
	}

	JE_buftype buf;

	f.seekg(pcxpos[PCXnumber]);
	bs.getIter(buf, buf+(pcxpos[PCXnumber+1]-pcxpos[PCXnumber]));
	f.close();

	Uint8 *p = buf;
	// screen pointer, 8-bit specific
	Uint8 *s = VGAScreen;

	for (int i = 0; i < 320 * 200; )
	{
		if ((*p & 0xc0) == 0xc0)
		{
			i += (*p & 0x3f);
			memset(s, *(p + 1), (*p & 0x3f));
			s += (*p & 0x3f); p += 2;
		} else {
			i++;
			*s = *p;
			s++; p++;
		}
		if (i && (i % 320 == 0))
		{
			s += scr_width - 320;
		}
	}
	
	load_pcx_palette(PCXnumber, storepal);
}

void load_pcx_palette( unsigned int pcx_num, bool set_colors )
{
	if (pcx_num > PCX_NUM)
	{
		Console::get() << "\a7Error:\ax Tried to load non-existent PCX palette " << pcx_num << std::endl;
		return;
	}
	load_palette(pcxpal[pcx_num], set_colors);
}
