/***************************************************************************
 *            pcSyncThreadDataSingle.cc
 *
 *  Wed Nov 30 22:02:16 2005
 *  Copyright  2005  Ni@m
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/pcSyncThreadDataSingle.h>

using namespace dodo::pc::sync::thread::data;

single::single(single &sts)
{
}

//-------------------------------------------------------------------

single::single() : data(NULL)
{
#ifdef PTHREAD_EXT

	pthread_mutexattr_t attr;
	errno = pthread_mutexattr_init(&attr);
	if (errno != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_SIGNLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	errno = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	if (errno != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_SIGNLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	errno = pthread_mutex_init(&mutex, &attr);
	if (errno != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_SIGNLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	errno = pthread_mutexattr_destroy(&attr);
	if (errno != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_SIGNLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif

	timeout.tv_nsec = 1000;
	timeout.tv_sec = 0;
}

//-------------------------------------------------------------------

single::~single()
{
#ifdef PTHREAD_EXT

	if (pthread_mutex_trylock(&mutex) == 0)
		pthread_mutex_unlock(&mutex);

	pthread_mutex_destroy(&mutex);

#endif
}

//-------------------------------------------------------------------

void
single::set(void *a_data)
{
#ifdef PTHREAD_EXT

	errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_SET, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	data = a_data;

	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_SET, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
}

//-------------------------------------------------------------------

void
single::del()
{
#ifdef PTHREAD_EXT

	errno = pthread_mutex_lock(&mutex);
	if (errno != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_DEL, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	data = NULL;

	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_DEL, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
}

//-------------------------------------------------------------------

void *
single::acquire(unsigned long microseconds)
{
#ifdef PTHREAD_EXT

	if (microseconds == 0)
	{
		errno = pthread_mutex_lock(&mutex);
		if (errno != 0)
			throw exception::basic(exception::ERRMODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_LOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
	else
	{
		bool locked = true;
		unsigned long slept = 0;

		while (locked)
		{
			errno = pthread_mutex_trylock(&mutex);
			if (errno != 0)
			{
				if (errno != EBUSY)
					throw exception::basic(exception::ERRMODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_LOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				if (nanosleep(&timeout, NULL) == -1)
					throw exception::basic(exception::ERRMODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_LOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				slept += 1;

				if (slept > microseconds)
					throw exception::basic(exception::ERRMODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_LOCK, exception::ERRNO_ERRNO, SINGLEEX_CANNOTLOCK, PCSYNCTHREADDATASINGLEEX_CANNOTLOCK_STR, __LINE__, __FILE__);
			}
			else
				locked = false;
		}
	}

#endif

	return data;
}

//-------------------------------------------------------------------

void
single::release()
{
#ifdef PTHREAD_EXT

	errno = pthread_mutex_unlock(&mutex);
	if (errno != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCTHREADDATASINGLE, SINGLEEX_UNLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
}

//-------------------------------------------------------------------

