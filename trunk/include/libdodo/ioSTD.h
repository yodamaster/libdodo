/***************************************************************************
 *            ioSTD.h
 *
 *  Tue Nov 15 21:19:57 2005
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

#ifndef _IOSTD_H_
#define _IOSTD_H_

#include <fcntl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <libdodo/directives.h>

#include <libdodo/tools.h>
#include <libdodo/ioSTDEx.h>
#include <libdodo/types.h>
#include <libdodo/io.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/threadGuard.h>

namespace dodo
{
	/**
	 * @enum ioSTDOperationTypeEnum defines type of operation for hook
	 */
	enum ioSTDOperationTypeEnum
	{
		IOSTD_OPERATION_READ,
		IOSTD_OPERATION_READSTRING,
		IOSTD_OPERATION_READSTREAM,
		IOSTD_OPERATION_READSTREAMSTRING,
		IOSTD_OPERATION_WRITE,
		IOSTD_OPERATION_WRITESTRING,
		IOSTD_OPERATION_WRITESTREAM,
		IOSTD_OPERATION_WRITESTREAMSTRING,
		IOSTD_OPERATION_OPEN,
		IOSTD_OPERATION_CLOSE
	};

	/**
	 * @class ioSTD provides interface for stdin/stdout/stderr I/O operations
	 */

	class ioSTD : public io,
				  virtual public threadGuardHolder
	{
		private:

			/**
			 * constructor
			 * to prevent from copying
			 */
			ioSTD(ioSTD &fd);

		public:

			/**
			 * constructor
			 */
			ioSTD();

			/**
			 * destructor
			 */
			virtual ~ioSTD();

			/**
			 * @return info about source of inputting
			 * @note it can be used to get info foreign 'inputter' if you ar using inetd
			 */
			__connInfo inputterInfo();

#ifndef IOSTD_WO_XEXEC

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param func defines hook function
			 * @param data defines data that will be passed to hook function
			 */
			virtual int addPostExec(inExec func, void *data);

			/**
			 * add hook before the operation
			 * @return id of the hook method
			 * @param func defines hook function
			 * @param data defines data that will be passed to hook function
			 */
			virtual int addPreExec(inExec func, void *data);

#ifdef DL_EXT

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPostExec(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPreExec(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * set hook from the library that will be executed before/after the operation
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 * @note type of hook[pre/post] is defined in the library
			 */
			virtual __xexecCounts addExec(const dodoString &path, void *data, void *toInit = NULL);

#endif

#endif

			/**
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void readString(dodoString &data);

			/**
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void read(char * const data);

			/**
			 * @param data defines data that will be written
			 */
			virtual void writeString(const dodoString &data);

			/**
			 * @param data defines data that will be written
			 */
			virtual void write(const char * const data);

			/**
			 * read from stream - '\0' or '\n' - terminated string
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void readStreamString(dodoString &data);

			/**
			 * read from stream - '\0' or '\n' - terminated string
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void readStream(char * const data);

			/**
			 * write to stream - '\0' - terminated string
			 * @param data defines data that will be written
			 */
			virtual void writeStreamString(const dodoString &data);

			/**
			 * write to stream - '\0' - terminated string
			 * @param data defines data that will be written
			 */
			virtual void writeStream(const char * const data);

			/**
			 * flush output
			 */
			virtual void flush();

			/**
			 * @return true if stream is blocked
			 */
			virtual bool isBlocked();

			/**
			 * blocks/unblocks stream
			 * @param flag indicates whether to block or unblock stream
			 */
			virtual void block(bool flag);

			/**
			 * redirect output stream to stderr or stdout
			 * @param toSTDErr defines to redirect stream to stderr if true
			 */
			virtual void redirectToSTDErr(bool toSTDErr);

			/**
			 * @return true if output stream is redirected to stderr
			 */
			virtual bool isRedirectedToSTDErr();

			int inSTDBuffer;    ///< input buffer
			int outSTDBuffer;   ///< output buffer

		protected:

			/**
			 * @return descriptor of the input stream
			 */
			virtual int getInDescriptor() const;

			/**
			 * @return descriptor of the output stream
			 */
			virtual int getOutDescriptor() const;

			/**
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void _read(char * const data);

			/**
			 * read from stream - '\0' or '\n' - terminated string
			 * @param data defines buffer that will be filled
			 * @note not more then inSize(including '\0')
			 */
			virtual void _readStream(char * const data);

			/**
			 * @param data defines data that will be written
			 */
			virtual void _write(const char * const data);

		private:

			FILE *desc;         ///< stream descriptor

			bool blocked;       ///< true if stream is blocked

			bool err;           ///< true if output stream is redirected to stderr
	};

};

#endif
