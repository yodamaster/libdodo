/***************************************************************************
 *            pcSyncThreadSection.cc
 *
 *  Mon Jul 09 11:31:55 2007
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/pcSyncThreadSection.h>

using namespace dodo::pc::sync::thread;

section::section()
{
#ifdef PTHREAD_EXT

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

	pthread_mutex_init(&keeper, &attr);

	pthread_mutexattr_destroy(&attr);

#endif
}

//-------------------------------------------------------------------

section::~section()
{
#ifdef PTHREAD_EXT

	pthread_mutex_destroy(&keeper);

#endif
}

//-------------------------------------------------------------------

void
section::acquire()
{
#ifdef PTHREAD_EXT

	errno = pthread_mutex_lock(&keeper);
	if (errno != 0 && errno != EDEADLK)
		throw exception::basic(exception::ERRMODULE_PCSYNCTHREADSECTION, SECTIONEX_LOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
}

//-------------------------------------------------------------------

void
section::release()
{
#ifdef PTHREAD_EXT

	errno = pthread_mutex_unlock(&keeper);
	if (errno != 0)
		throw exception::basic(exception::ERRMODULE_PCSYNCTHREADSECTION, SECTIONEX_UNLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
}

//-------------------------------------------------------------------
