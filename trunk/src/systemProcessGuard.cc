/***************************************************************************
 *            systemProcessGuard.cc
 *
 *  Tue Jul  10 22:10:57 2007
 *  Copyright  2007  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <libdodo/systemProcessGuard.h>

using namespace dodo;

systemProcessGuardHolder::systemProcessGuardHolder()
{
	misc::random(key, 31);
	key[31] = '\0';

	mutex = new systemAtomicSemaphore(1, key);
}

//-------------------------------------------------------------------

systemProcessGuardHolder::~systemProcessGuardHolder()
{
	delete mutex;
}

//-------------------------------------------------------------------

