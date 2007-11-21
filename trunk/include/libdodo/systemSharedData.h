/***************************************************************************
 *            systemSharedData.h
 *
 *  Wed Nov 21 00:39:14 2007
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

#ifndef _SYSTEMSHAREDDATA_H_
#define _SYSTEMSHAREDDATA_H_

#include <libdodo/directives.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <libdodo/systemSharedDataEx.h>
#include <libdodo/tools.h>

#include <iostream>

namespace dodo
{
	/**
	 * @class systemSharedData is to manage data
	 */
	class systemSharedData
	{
		private:

		/**
		 * copy constructor
		 * to prevent copying
		 */
		systemSharedData(systemSharedData &sts);

		public:

		/**
		 * constructor
		 * @param key is given to set shared memory's key
		 * @note if key is NULL - name is autogenerated
		 * if you want to share semaphore between different instances of process - set own key,
		 * otherwise(like fork) - can be generated
		 */
		systemSharedData(const char *key = NULL);

		/**
		 * destructor
		 */
		virtual ~systemSharedData();

		/**
		 * maps shared data
		 * @return pointer on shared data
		 * @param size describes amount of data to be shared
		 * @param offset describes the offset of shared data
		 * @note offset must be a multiple of the page size as returned by systemSharedData::pageSize
		 * if map is called twice - old data would be unmapped
		 */
		virtual void *map(unsigned long size, unsigned long offset=0);

		/**
		 * unmaps shared data
		 */
		virtual void unmap();
		
		/**
		 * @return pointer on shared data
		 */
		virtual void *getMapped();
		
		/**
		 * @return size shared data
		 */
		
		virtual unsigned long getSize();
		/**
		 * @return page size
		 */
		static long pageSize();

		protected:
			
		int shm;///< shared data descriptor
		void *data;///< shared data
		unsigned long size;///< size of shared data

		char *key;              ///< key for the semaphore
	};
};

#endif

