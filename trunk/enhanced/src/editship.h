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
#ifndef EDITSHIP_H
#define EDITSHIP_H

#include "opentyr.h"


typedef Uint8 JE_ShipsType[154];

extern bool extraAvail;
extern JE_ShipsType extraShips;
extern Uint8* extraShapes;
extern unsigned long extraShapeSize;

void JE_decryptShips();
void JE_encryptShips();
void JE_compressShapeFile();
void JE_buildRec();
void JE_startNewShape();
void JE_add( Uint8 nextbyte );
void JE_endShape();
void JE_loadExtraShapes();

#endif /* EDITSHIP_H */
