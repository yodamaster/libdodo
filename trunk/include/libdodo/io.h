/***************************************************************************
 *            io.h
 *
 *  Tue Oct 11 00:19:57 2005
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

#ifndef _IO_H_
#define _IO_H_

#include <libdodo/directives.h>

#include <libdodo/xexec.h>
#include <libdodo/types.h>
#include <libdodo/ioNBAInfo.h>

namespace dodo
{
	/**
	 * @struct __xexexIoCollectedData defines data that could be retrieved from class(to modificate)[contains references]
	 */
	struct __xexexIoCollectedData
	{
		/**
		 * constructor
		 */
		__xexexIoCollectedData(dodoString &buffer,
							   int &operType,
							   void *executor);

		dodoString &buffer;         ///< data buffer

		int &operType;              ///< xexec operation

		void *executor;             ///< class that executed hook
	};

	/**
	 * @class io implements an interface for I/O operations
	 */
	class io : public xexec,
				virtual public ioNBAInfo
	{
		public:

			/**
			 * constructor
			 */
			io();

			/**
			 * destructor
			 */
			virtual ~io() = 0;
			
			/**
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void readString(dodoString &data) = 0;

			/**
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void read(char * const data) = 0;

			/**
			 * @param data defines data that will be written
			 */
			virtual void writeString(const dodoString &data) = 0;

			/**
			 * @param data defines data that will be written
			 */
			virtual void write(const char * const data) = 0;

			/**
			 * read from stream - '\0' or '\n' - terminated string
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void readStreamString(dodoString &data) = 0;

			/**
			 * read from stream - '\0' or '\n' - terminated string
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void readStream(char * const data) = 0;

			/**
			 * write to stream - '\0' - terminated string
			 * @param data defines data that will be written
			 */
			virtual void writeStreamString(const dodoString &data) = 0;

			/**
			 * write to stream - '\0' - terminated string
			 * @param data defines data that will be written
			 */
			virtual void writeStream(const char * const data) = 0;

			/**
			 * flush output
			 */
			virtual void flush() = 0;

			unsigned long inSize;   ///< size of data block for read* operations
			unsigned long outSize;  ///< size of data block for write* operations

		protected:

			bool opened; ///< true if I/O *session* is opened

			dodoString buffer;                      ///< buffer

			__xexexIoCollectedData collectedData;   ///< data collected for xexec
	};

};

#endif
