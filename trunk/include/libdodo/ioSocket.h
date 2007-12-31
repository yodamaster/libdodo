/***************************************************************************
 *            ioSocket.h
 *
 *  Thu Oct 04 02:02:24 2005
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

#ifndef _IOSOCKET_H_
#define _IOSOCKET_H_

#include <libdodo/directives.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <libdodo/ioDiskTools.h>
#include <libdodo/tools.h>
#include <libdodo/ioSocketEx.h>
#include <libdodo/types.h>
#include <libdodo/ioSocketOptions.h>
#include <libdodo/ioSocketExchange.h>
#include <libdodo/xexec.h>

namespace dodo
{

	/**
	 * @enum ioSocketOperationTypeEnum describes type of operation for hook
	 */
	enum ioSocketOperationTypeEnum
	{
		IOSOCKET_OPERATION_CONNECT,
		IOSOCKET_OPERATION_CONNECT_UNIX,
		IOSOCKET_OPERATION_BINDNLISTEN,
		IOSOCKET_OPERATION_BINDNLISTEN_UNIX,
		IOSOCKET_OPERATION_ACCEPT,
	};

	/**
	 * @struct __connInfo describes connections info
	 * you may use it for connect() method or accept()[info about remote connected host]
	 */
	struct __connInfo
	{
		dodoString host;
		int port;
	};

	/**
	 * @class ioSocket performs communication actions!!
	 * exchange of data is ioSocketExchange class' task; ou init it with connect or accept methods
	 */
	class ioSocket : public io, public ioSocketOptions
	{
			friend class ioSocketExchange;

		private:

			/**
			 * connstructor
			 * to prevent copying
			 */
			ioSocket(ioSocket &fs);

		public:

			/**
			 * constructors
			 * @param server indicates what type of oject will be
			 * @param family is family of the socket[see socketProtoFamilyEnum]
			 * @param type is type of the socket[see socketTransferTypeEnum]
			 */
			ioSocket(bool server, short family, short type);

			/**
			 * destructor
			 */
			virtual ~ioSocket();

			#ifndef IOSOCKET_WO_XEXEC

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param func is a pointer to function
			 * @param data is pointer to data toy want to pass to hook
			 */
			virtual int addPostExec(inExec func, void *data);

			/**
			 * adds hook before the operation by callback
			 * @return number in list where function is set
			 * @param func is a pointer to function
			 * @param data is pointer to data toy want to pass to hook
			 */
			virtual int addPreExec(inExec func, void *data);

				#ifdef DL_EXT

			/**
			 * set function from module that will be executed before/after the main action call
			 * the type of hook[pre/post] is defined in module
			 * @return number in list where function is set
			 * @param func is a pointer to function
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual __xexecCounts addExec(const dodoString &module, void *data, void *toInit=NULL);

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param module is a path to module, whrere hook exists
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual int addPostExec(const dodoString &module, void *data, void *toInit=NULL);

			/**
			 * adds hook after the operation by callback
			 * @return number in list where function is set
			 * @param module is a path to module, whrere hook exists
			 * @param data is pointer to data toy want to pass to hook
			 * @param toInit indicates data that will path to initialize function
			 */
			virtual int addPreExec(const dodoString &module, void *data, void *toInit=NULL);

				#endif

			#endif

			/**
			 * connect from specific address. for client part
			 * @param host is ip address where to connect
			 * @param port is port where to connect
			 * @param exchange is reference to oject that will perform communication actions
			 */
			virtual void connectFrom(const dodoString &local, const dodoString &host, int port, ioSocketExchange &exchange);

			/**
			 * connect from specific address. for client part
			 * @param destinaton is structure that describes destination
			 * @param exchange is reference to oject that will perform communication actions
			 * the same as previous, but more pretty
			 */
			virtual void connectFrom(const dodoString &local, const __connInfo &destinaton, ioSocketExchange &exchange);

			/**
			 * connect. for client part
			 * @param host is ip address where to connect
			 * @param port is port where to connect
			 * @param exchange is reference to oject that will perform communication actions
			 */
			virtual void connect(const dodoString &host, int port, ioSocketExchange &exchange);

			/**
			 * connect. for client part
			 * @param destinaton is structure that describes destination
			 * @param exchange is reference to oject that will perform communication actions
			 * the same as previous, but more pretty
			 */
			virtual void connect(const __connInfo &destinaton, ioSocketExchange &exchange);

			/**
			 * connect. for client part
			 * @param path is path to unix socket
			 * @param exchange is reference to oject that will perform communication actions
			 */
			virtual void connect(const dodoString &path, ioSocketExchange &exchange);                  ///< if socket is already created - nothin' will be done for creation. if file exists, but not socket - ex will be thrown (or false will be returned)!

			/**
			 * connect. for server part
			 * @param host is ip address that would be listen; can be '*' -> any address
			 * @param port is port where to listen
			 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
			 */
			virtual void bindNListen(const dodoString &host, int port, int numberOfConnections);

			/**
			 * connect. for server part
			 * @param destinaton is structure that describes destination
			 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
			 * the same as previous, but more pretty
			 */
			virtual void bindNListen(const __connInfo &destinaton, int numberOfConnections);

			/**
			 * connect. for server part
			 * @param path is path to unix socket
			 * @param numberOfConnections defines the maximum length the queue of pending connections may grow to
			 * @note if socket is already created and force=true and it's a socket - delete it!!
			 */
			virtual void bindNListen(const dodoString &path, int numberOfConnections, bool force=false);

			/**
			 * accepts incommin' connections(as for server)
			 * @return true on accept; with IOSOCKETOPTIONS_TRANSFER_TYPE_DATAGRAM is always returns true, so u should skip calling this function
			 * @param init will be filled with info that will init ioSocketExchange object
			 * @param info is info about connected host
			 * with IOSOCKETOPTIONS_PROTO_FAMILY_UNIX_SOCKET `info` will be always empty, so you may use second function
			 */
			virtual bool accept(__initialAccept &init, __connInfo &info);

			/**
			 * accepts incommin' connections(as for server)
			 * @return true on accept; with IOSOCKETOPTIONS_TRANSFER_TYPE_DATAGRAM is always returns true, so u should skip calling this function
			 * @param init will be filled with info that will init ioSocketExchange object
			 * if you don't want to know anythin' about remote; not just alias. a little bit faster!
			 */
			virtual bool accept(__initialAccept &init);

			bool blockInherited; ///< if true - children(ioSocketExchange) became unblocked, if parent(ioSocket) in unblocked; false by default

			/**
			 * @return descriptor of input stream
			 */
			virtual int getInDescriptor() const;

			/**
			 * @return descriptor of output stream
			 */
			virtual int getOutDescriptor() const;
			
		protected:

			/**
			 * restores options on connect/bind
			 */
			virtual void restoreOptions();

			/**
			 * creates socket with given data
			 */
			virtual void makeSocket();

			bool server;            ///< indicates whether server object or not

			dodoString unixSock;    ///< to remember, 'cos have to unlink in destructor
	};

};

#endif
