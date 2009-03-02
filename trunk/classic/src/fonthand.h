/* 
 * OpenTyrian Classic: A modern cross-platform port of Tyrian
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
#ifndef FONTHAND_H
#define FONTHAND_H

#include "opentyr.h"


#define PART_SHADE 0
#define FULL_SHADE 1
#define DARKEN     2
#define TRICK      3
#define NO_SHADE 255

extern const JE_byte fontMap[136];
extern JE_integer defaultBrightness;
extern JE_byte textGlowFont, textGlowBrightness;
extern JE_boolean levelWarningDisplay;
extern JE_byte levelWarningLines;
extern char levelWarningText[10][61];
extern JE_boolean warningRed;
extern JE_byte warningSoundDelay;
extern JE_word armorShipDelay;
extern JE_byte warningCol;
extern JE_shortint warningColChange;

void JE_dString( JE_word x, JE_word y, const char *s, JE_byte font );

JE_word JE_fontCenter( const char *s, JE_byte font );
JE_word JE_textWidth( const char *s, JE_byte font );
void JE_textShade( JE_word x, JE_word y, const char *s, JE_byte colorbank, JE_shortint brightness, JE_byte shadetype );
void JE_outText( JE_word x, JE_word y, const char *s, JE_byte colorbank, JE_shortint brightness );
void JE_outTextModify( JE_word x, JE_word y, const char *s, JE_byte filter, JE_byte brightness, JE_byte font );
void JE_outTextShade( JE_word x, JE_word y, const char *s, JE_byte font );
void JE_outTextAdjust( JE_word x, JE_word y, const char *s, JE_byte filter, JE_shortint brightness, JE_byte font, JE_boolean shadow );
void JE_outTextAndDarken( JE_word x, JE_word y, const char *s, JE_byte colorbank, JE_byte brightness, JE_byte font );

void JE_updateWarning( void );
void JE_outTextGlow( JE_word x, JE_word y, const char *s );

#endif /* FONTHAND_H */

// kate: tab-width 4; vim: set noet:
