/* vim: set noet:
 *
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
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
#include "fm_synth.h"

#include "fmopl.h"
#include "loudness.h"


const unsigned short note_table[12] = {363, 385, 408, 432, 458, 485, 514, 544, 577, 611, 647, 686};
const unsigned char op_table[9] = {0x00, 0x01, 0x02, 0x08, 0x09, 0x0a, 0x10, 0x11, 0x12};

int use16bit, stereo;
#define opl 0

void opl_update( Sint16 *buf, int samples )
{
	int i;

	if (use16bit)
	{
		YM3812UpdateOne(opl, buf, samples);
		
		if (stereo)
			for(i = samples - 1; i >= 0; i--) {
				buf[i*2] = buf[i];
				buf[i*2+1] = buf[i];
			}
	} else {
		short *tempbuf = malloc(sizeof(Sint16) * (stereo ? samples * 2 : samples));
		int i;

		YM3812UpdateOne(opl, tempbuf, samples);

		if (stereo)
			for (i = samples - 1; i >= 0; i--) {
				tempbuf[i*2] = tempbuf[i];
				tempbuf[i*2+1] = tempbuf[i];
			}

		for (i = 0; i < (stereo ? samples * 2 : samples); i++)
			((char *)buf)[i] = (tempbuf[i] >> 8) ^ 0x80;

		free(tempbuf);
	}
}

void opl_init( void )
{
	use16bit = (BYTES_PER_SAMPLE == 2);
	stereo = 0;

	YM3812Init(1, 3579545, 11025 * OUTPUT_QUALITY);
}

void opl_deinit( void )
{
	YM3812Shutdown();
}

void opl_reset( void )
{
	YM3812ResetChip(opl);
}

void opl_write(int reg, int val)
{
  YM3812Write(opl, 0, reg);
  YM3812Write(opl, 1, val);
}
