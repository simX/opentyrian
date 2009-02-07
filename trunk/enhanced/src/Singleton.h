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
#ifndef SINGLETON_H
#define SINGLETON_H

#include <cassert>

template<class T> class Singleton
{
private:
	Singleton( const Singleton& );
	Singleton& operator=( const Singleton& );
	static T* sInstance;
public:
	Singleton( ) { }

	static void initialize( )
	{
		if (sInstance != 0) return;
		sInstance = new T();
	}

	static T& get( )
	{
		if (sInstance == 0) initialize();
		return *sInstance;
	}

	static void deinitialize( )
	{
		if (sInstance == 0) return;
		delete sInstance;
		sInstance = 0;
	}
};

template<class T> T* Singleton<T>::sInstance = 0;

#endif // SINGLETON_H
