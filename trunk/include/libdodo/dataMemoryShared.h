/***************************************************************************
 *            dataMemoryShared.h
 *
 *  Wed Nov 21 2007
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

#ifndef _DATAMEMORYSHARED_H_
#define _DATAMEMORYSHARED_H_ 1

#include <libdodo/directives.h>

namespace dodo {
	namespace io {
		class memory;
	};

	namespace data {
		namespace memory {
			struct __key__;

			/**
			 * @class shared
			 * @brief provides shared data functionality
			 * @note if the key was not autogenerated the semaphore is not removed from the system
			 * you should call remove to remove it from the system
			 */
			class shared {
				friend class io::memory;

			  private:

				/**
				 * copy constructor
				 * @note to prevent copying
				 */
				shared(shared &);

			  public:

				/**
				 * constructor
				 * @param key defines shared memory key
				 * @note if key is 0 - key will be generated
				 * if you want to share data between different instances of process - set own key,
				 * otherwise(like fork) - it may be generated
				 */
				shared(const int key);

				/**
				 * constructor
				 */
				shared();

				/**
				 * destructor
				 */
				~shared();

				/**
				 * open or create shared memory object
				 * @param key defines shared memory key
				 * @note if key is 0 - key will be generated
				 * if you want to share data between different instances of process - set own key,
				 * otherwise(like fork) - it may be generated
				 */
				void open(const int key) const;

				/**
				 * close shared memory object
				 */
				void close() const;

				/**
				 * map shared data
				 * @return shared data
				 * @param size defines size of shared data
				 * @note if map is called twice - old data will be unmapped
				 * is size is 0 it will be detected automatically
				 */
				void *map(unsigned long size = 0) const;

				/**
				 * unmap shared data
				 */
				void unmap() const;

				/**
				 * @return shared data
				 */
				void *data() const;

				/**
				 * @return size of shared data
				 * @note this should be called before map to obtain the size of the already allocated shared memory
				 */
				unsigned long size() const;

				/**
				 * remove the shared data from the system
				 * @param key defines the key of the shared data
				 */
				static void remove(int key);

			  protected:

				mutable int handle;                ///< shared data descriptor

				/**
				 * @struct __shared__
				 * @brief defines shared data
				 */
				struct __shared__ {
					/**
					 * constructor
					 * @param data defines pointer to the shared data
					 */
					__shared__(void *data);

					/**
					 * constructor
					 */
					__shared__();


					mutable void *data;           ///< shared data
#ifndef XSI_IPC
					mutable unsigned long size;   ///< size of shared data
#endif
				};

				struct __shared__ sh; ///< shared data

				__key__ *key;                   ///< key for the shared data

				mutable bool autogenerated;     ///< true if key was autogenerated
			};
		};
	};
};
#endif

