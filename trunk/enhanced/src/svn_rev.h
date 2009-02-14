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
#ifndef SVN_REV_H
#define SVN_REV_H
#include "svn_rev_def.h"

#include "boost/lexical_cast.hpp"
#include <string>
#include <sstream>

#ifdef HAVE_SVN_REV
namespace {
	inline unsigned int get_svn_rev_internal(unsigned int rev)
	{
		return rev;
	}

	inline unsigned int get_svn_rev_internal(const char* rev)
	{
		std::istringstream s(rev);
		unsigned int tmp = 0;
		s >> tmp;
		return tmp;
	}
}

inline unsigned int get_svn_rev_int()
{
	return get_svn_rev_internal(SVN_REV);
}

inline std::string get_svn_rev_str()
{
	return boost::lexical_cast<std::string>(get_svn_rev_internal(SVN_REV));
}
#else
inline unsigned int get_svn_rev_int()
{
	return 0;
}

inline std::string get_svn_rev_str()
{
	return "0";
}
#endif

#undef SVN_REV

#endif // SVN_REV_H
