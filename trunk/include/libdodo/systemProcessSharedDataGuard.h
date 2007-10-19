/***************************************************************************
 *            systemProcessSharedDataGuard.h
 *
 *  Sun Jul 22 23:14:14 2007
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

#ifndef _SYSTEMPROCESSSHAREDDATAGUARD_H_
#define _SYSTEMPROCESSSHAREDDATAGUARD_H_

#include <semaphore.h>
#include <time.h>

#include <libdodo/directives.h>

#include <libdodo/systemProcessSharedDataGuardEx.h>
#include <libdodo/types.h>
#include <libdodo/tools.h>

namespace dodo
{
	/**
	 * @class systemProcessSharedDataGuard is to manage data between processes(based on POSIX semaphores)
	 * @note you should use systemShareDate
	 */
	class systemProcessSharedDataGuard
	{
		private:

		/**
		 * copy constructor
		 * to prevent copying
		 */
		systemProcessSharedDataGuard(systemProcessSharedDataGuard &sts);

		public:

		/**
		 * constructor
		 * @param value sets initial value of semaphore
		 * @param key is given to set semaphores's key
		 * @note if key is NULL - name is autogenerated
		 * if you want to share semaphore between different instances of process - set own key,
		 * otherwise(like fork) - can be generated
		 */
		systemProcessSharedDataGuard(unsigned int value = 1, const char *key = NULL);

		/**
		 * destructor
		 */
		virtual ~systemProcessSharedDataGuard();

		/**
		 * sets data to became a shared
		 * @param data describes data to be shared
		 */
		virtual void set(void *data);

		/**
		 * sets shared data to NULL
		 */
		virtual void del();

		/**
		 * lock and return shared data
		 * @return data points on shared data or NULL in error case
		 * @param microseconds indicates how many time to wait for locking; if time expired and can't unlock - error =(; only if realization of pthreads supports it!
		 * @note if microseconds==0 - infinite sleep
		 */
		virtual void *lock(unsigned long microseconds = 0);


		/**
		 * lock and return shared data
		 */
		virtual void unlock();

		protected:

		void *data;             ///< data that will be shared
		sem_t *semaphore;       ///< semaphore

		timespec timeout;       ///< timeout to lock mutex check

		char *key;              ///< key for the semaphore
	};

};

#endif

