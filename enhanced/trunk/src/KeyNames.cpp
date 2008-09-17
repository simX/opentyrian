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
#include "KeyNames.h"

#include "Console.h"
#include <sstream>

#include "SDL.h"

KeyNames::KeyNames( )
{
	DEBUG_MSG("Initializing KeyNames...");

	for (const KeyName* p = key_names; p->name; ++p)
	{
		nameMap.insert(EntryType(p->name, p->key));
	}
}

std::string KeyNames::getNameFromKey( const SDLKey sym )
{
	BimapType::right_map::const_iterator p = nameMap.right.find(sym);
	if (p == nameMap.right.end())
	{
		std::ostringstream s;
		s << sym;
		throw UnknownKeyError(s.str());
	}

	return p->second;
}

SDLKey KeyNames::getKeyFromName( const std::string& name )
{
	BimapType::left_map::const_iterator p = nameMap.left.find(name);
	if (p == nameMap.left.end())
	{
		throw UnknownKeyError(name);
	}

	return p->second;
}

// This list was taken from the SDL sources, then processed and modified. Thanks guys!
const KeyNames::KeyName KeyNames::key_names[] = {
	{"backspace", SDLK_BACKSPACE},
	{"tab", SDLK_TAB},
	{"clear", SDLK_CLEAR},
	{"return", SDLK_RETURN},
	{"pause", SDLK_PAUSE},
	{"escape", SDLK_ESCAPE},
	{"space", SDLK_SPACE},
	{"!", SDLK_EXCLAIM},
	{"\"", SDLK_QUOTEDBL},
	{"#", SDLK_HASH},
	{"$", SDLK_DOLLAR},
	{"&", SDLK_AMPERSAND},
	{"'", SDLK_QUOTE},
	{"(", SDLK_LEFTPAREN},
	{")", SDLK_RIGHTPAREN},
	{"*", SDLK_ASTERISK},
	{"+", SDLK_PLUS},
	{",", SDLK_COMMA},
	{"-", SDLK_MINUS},
	{".", SDLK_PERIOD},
	{"/", SDLK_SLASH},
	{"0", SDLK_0},
	{"1", SDLK_1},
	{"2", SDLK_2},
	{"3", SDLK_3},
	{"4", SDLK_4},
	{"5", SDLK_5},
	{"6", SDLK_6},
	{"7", SDLK_7},
	{"8", SDLK_8},
	{"9", SDLK_9},
	{":", SDLK_COLON},
	{";", SDLK_SEMICOLON},
	{"<", SDLK_LESS},
	{"=", SDLK_EQUALS},
	{">", SDLK_GREATER},
	{"?", SDLK_QUESTION},
	{"@", SDLK_AT},
	{"[", SDLK_LEFTBRACKET},
	{"\\", SDLK_BACKSLASH},
	{"]", SDLK_RIGHTBRACKET},
	{"^", SDLK_CARET},
	{"_", SDLK_UNDERSCORE},
	{"`", SDLK_BACKQUOTE},
	{"a", SDLK_a},
	{"b", SDLK_b},
	{"c", SDLK_c},
	{"d", SDLK_d},
	{"e", SDLK_e},
	{"f", SDLK_f},
	{"g", SDLK_g},
	{"h", SDLK_h},
	{"i", SDLK_i},
	{"j", SDLK_j},
	{"k", SDLK_k},
	{"l", SDLK_l},
	{"m", SDLK_m},
	{"n", SDLK_n},
	{"o", SDLK_o},
	{"p", SDLK_p},
	{"q", SDLK_q},
	{"r", SDLK_r},
	{"s", SDLK_s},
	{"t", SDLK_t},
	{"u", SDLK_u},
	{"v", SDLK_v},
	{"w", SDLK_w},
	{"x", SDLK_x},
	{"y", SDLK_y},
	{"z", SDLK_z},
	{"delete", SDLK_DELETE},

	{"world0", SDLK_WORLD_0},
	{"world1", SDLK_WORLD_1},
	{"world2", SDLK_WORLD_2},
	{"world3", SDLK_WORLD_3},
	{"world4", SDLK_WORLD_4},
	{"world5", SDLK_WORLD_5},
	{"world6", SDLK_WORLD_6},
	{"world7", SDLK_WORLD_7},
	{"world8", SDLK_WORLD_8},
	{"world9", SDLK_WORLD_9},
	{"world10", SDLK_WORLD_10},
	{"world11", SDLK_WORLD_11},
	{"world12", SDLK_WORLD_12},
	{"world13", SDLK_WORLD_13},
	{"world14", SDLK_WORLD_14},
	{"world15", SDLK_WORLD_15},
	{"world16", SDLK_WORLD_16},
	{"world17", SDLK_WORLD_17},
	{"world18", SDLK_WORLD_18},
	{"world19", SDLK_WORLD_19},
	{"world20", SDLK_WORLD_20},
	{"world21", SDLK_WORLD_21},
	{"world22", SDLK_WORLD_22},
	{"world23", SDLK_WORLD_23},
	{"world24", SDLK_WORLD_24},
	{"world25", SDLK_WORLD_25},
	{"world26", SDLK_WORLD_26},
	{"world27", SDLK_WORLD_27},
	{"world28", SDLK_WORLD_28},
	{"world29", SDLK_WORLD_29},
	{"world30", SDLK_WORLD_30},
	{"world31", SDLK_WORLD_31},
	{"world32", SDLK_WORLD_32},
	{"world33", SDLK_WORLD_33},
	{"world34", SDLK_WORLD_34},
	{"world35", SDLK_WORLD_35},
	{"world36", SDLK_WORLD_36},
	{"world37", SDLK_WORLD_37},
	{"world38", SDLK_WORLD_38},
	{"world39", SDLK_WORLD_39},
	{"world40", SDLK_WORLD_40},
	{"world41", SDLK_WORLD_41},
	{"world42", SDLK_WORLD_42},
	{"world43", SDLK_WORLD_43},
	{"world44", SDLK_WORLD_44},
	{"world45", SDLK_WORLD_45},
	{"world46", SDLK_WORLD_46},
	{"world47", SDLK_WORLD_47},
	{"world48", SDLK_WORLD_48},
	{"world49", SDLK_WORLD_49},
	{"world50", SDLK_WORLD_50},
	{"world51", SDLK_WORLD_51},
	{"world52", SDLK_WORLD_52},
	{"world53", SDLK_WORLD_53},
	{"world54", SDLK_WORLD_54},
	{"world55", SDLK_WORLD_55},
	{"world56", SDLK_WORLD_56},
	{"world57", SDLK_WORLD_57},
	{"world58", SDLK_WORLD_58},
	{"world59", SDLK_WORLD_59},
	{"world60", SDLK_WORLD_60},
	{"world61", SDLK_WORLD_61},
	{"world62", SDLK_WORLD_62},
	{"world63", SDLK_WORLD_63},
	{"world64", SDLK_WORLD_64},
	{"world65", SDLK_WORLD_65},
	{"world66", SDLK_WORLD_66},
	{"world67", SDLK_WORLD_67},
	{"world68", SDLK_WORLD_68},
	{"world69", SDLK_WORLD_69},
	{"world70", SDLK_WORLD_70},
	{"world71", SDLK_WORLD_71},
	{"world72", SDLK_WORLD_72},
	{"world73", SDLK_WORLD_73},
	{"world74", SDLK_WORLD_74},
	{"world75", SDLK_WORLD_75},
	{"world76", SDLK_WORLD_76},
	{"world77", SDLK_WORLD_77},
	{"world78", SDLK_WORLD_78},
	{"world79", SDLK_WORLD_79},
	{"world80", SDLK_WORLD_80},
	{"world81", SDLK_WORLD_81},
	{"world82", SDLK_WORLD_82},
	{"world83", SDLK_WORLD_83},
	{"world84", SDLK_WORLD_84},
	{"world85", SDLK_WORLD_85},
	{"world86", SDLK_WORLD_86},
	{"world87", SDLK_WORLD_87},
	{"world88", SDLK_WORLD_88},
	{"world89", SDLK_WORLD_89},
	{"world90", SDLK_WORLD_90},
	{"world91", SDLK_WORLD_91},
	{"world92", SDLK_WORLD_92},
	{"world93", SDLK_WORLD_93},
	{"world94", SDLK_WORLD_94},
	{"world95", SDLK_WORLD_95},

	{"[0]", SDLK_KP0},
	{"[1]", SDLK_KP1},
	{"[2]", SDLK_KP2},
	{"[3]", SDLK_KP3},
	{"[4]", SDLK_KP4},
	{"[5]", SDLK_KP5},
	{"[6]", SDLK_KP6},
	{"[7]", SDLK_KP7},
	{"[8]", SDLK_KP8},
	{"[9]", SDLK_KP9},
	{"[.]", SDLK_KP_PERIOD},
	{"[/]", SDLK_KP_DIVIDE},
	{"[*]", SDLK_KP_MULTIPLY},
	{"[-]", SDLK_KP_MINUS},
	{"[+]", SDLK_KP_PLUS},
	{"enter", SDLK_KP_ENTER},
	{"equals", SDLK_KP_EQUALS},

	{"up", SDLK_UP},
	{"down", SDLK_DOWN},
	{"right", SDLK_RIGHT},
	{"left", SDLK_LEFT},
	{"down", SDLK_DOWN},
	{"insert", SDLK_INSERT},
	{"home", SDLK_HOME},
	{"end", SDLK_END},
	{"pageup", SDLK_PAGEUP},
	{"pagedown", SDLK_PAGEDOWN},

	{"f1", SDLK_F1},
	{"f2", SDLK_F2},
	{"f3", SDLK_F3},
	{"f4", SDLK_F4},
	{"f5", SDLK_F5},
	{"f6", SDLK_F6},
	{"f7", SDLK_F7},
	{"f8", SDLK_F8},
	{"f9", SDLK_F9},
	{"f10", SDLK_F10},
	{"f11", SDLK_F11},
	{"f12", SDLK_F12},
	{"f13", SDLK_F13},
	{"f14", SDLK_F14},
	{"f15", SDLK_F15},

	{"numlock", SDLK_NUMLOCK},
	{"capslock", SDLK_CAPSLOCK},
	{"scrolllock", SDLK_SCROLLOCK},
	{"rshift", SDLK_RSHIFT},
	{"lshift", SDLK_LSHIFT},
	{"rctrl", SDLK_RCTRL},
	{"lctrl", SDLK_LCTRL},
	{"ralt", SDLK_RALT},
	{"lalt", SDLK_LALT},
	{"rmeta", SDLK_RMETA},
	{"lmeta", SDLK_LMETA},
	{"lsuper", SDLK_LSUPER}, // "Windows" keys
	{"rsuper", SDLK_RSUPER},	
	{"altgr", SDLK_MODE},
	{"compose", SDLK_COMPOSE},

	{"help", SDLK_HELP},
	{"printscreen", SDLK_PRINT},
	{"sysreq", SDLK_SYSREQ},
	{"break", SDLK_BREAK},
	{"menu", SDLK_MENU},
	{"power", SDLK_POWER},
	{"euro", SDLK_EURO},
	{"undo", SDLK_UNDO},

	{NULL, SDLK_LAST} // This must be the last element in the array
};

