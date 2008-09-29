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

#ifdef _MSC_VER
#pragma once
#pragma warning( push, 0 )
#endif // _MSC_VER

// C++ Standard headers
#include <algorithm>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// C++ C Standard headers
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstring>
#include <ctime>

// C Standard headers
#include <assert.h>
#include <ctype.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Boost headers
// Works around bug on GCC < 4.2
#if (!defined(__GNUC__) || (__GNUC__ >= 4 && __GNUC_MINOR__ >= 2))
#include "boost/bimap.hpp"
#include "boost/bind.hpp"
#endif

#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/function.hpp"

// SDL header
#include "SDL.h"
#include "SDL_endian.h"
#include "SDL_types.h"
