/***************************************************************************
 *            systemAtomicSemaphore.cc
 *
 *  Sat Oct 20 02:00:55 2007
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

#include <libdodo/systemAtomicSemaphore.h>

using namespace dodo;

systemAtomicSemaphore::systemAtomicSemaphore(unsigned int value,
								 const char   *a_key)
{
	key = new char[strlen(a_key) + 1];
	strcpy(key, a_key);

	semaphore = sem_open(key, O_CREAT, 0660, value);
}

//-------------------------------------------------------------------

systemAtomicSemaphore::~systemAtomicSemaphore()
{
	sem_close(semaphore);

	sem_unlink(key);
}

//-------------------------------------------------------------------

void
systemAtomicSemaphore::lock()
{
	if (sem_wait(semaphore) != 0)
		throw baseEx(ERRMODULE_SYSTEMATOMICSEMAPHORE, SYSTEMATOMICSEMAPHOREEX_LOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
systemAtomicSemaphore::unlock()
{
	if (sem_post(semaphore) != 0)
		throw baseEx(ERRMODULE_SYSTEMATOMICSEMAPHORE, SYSTEMATOMICSEMAPHOREEX_UNLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

