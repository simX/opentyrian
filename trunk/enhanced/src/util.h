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
#ifndef UTIL_H
#define UTIL_H
#include "opentyr.h"

#include "boost/iterator.hpp"

template <class Iter, class Container> class CircularIter
	: public boost::iterator_adaptor<
		CircularIter<Iter, Container>, // Derived
		Iter, // Base
		boost::use_default, // Value
		boost::bidirectional_traversal_tag> // CatergoryOrTraversal
{
private:
	friend class boost::iterator_core_access;

	Container *container;

	void increment()
	{
		if (container->empty())
			return;

		++this->base_reference();

		if (this->base_reference() == container->end())
			this->base_reference() = container->begin();
	}

	void decrement()
	{
		if (container->empty())
			return;

		if (this->base_reference() == container->begin())
			this->base_reference() = container->end();

		--this->base_reference();
	}

public:
	explicit CircularIter(Container& container) : CircularIter::iterator_adaptor_(Container::iterator()), container(&container)
	{}

	CircularIter(Iter i, Container& container) : CircularIter::iterator_adaptor_(i), container(&container)
	{}
};

#endif // UTIL_H
