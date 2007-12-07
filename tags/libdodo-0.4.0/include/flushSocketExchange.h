/***************************************************************************
 *            flushSocket.h
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
 
#ifndef _FLUSHSOCKETEXCHANGE_H_
#define _FLUSHSOCKETEXCHANGE_H_

#include <directives.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <flushSocketOptions.h>
#include <flushSocketExchangeEx.h>
#include <types.h>
#include <xexec.h>

namespace dodo
{	
	
	/**
	 * @enum flushSocketOperationTypeEnum describes type of operation for hook
	 */
	enum flushSocketExchangeOperationTypeEnum
	{
		FLUSHSOCKETEXCHANGE_OPER_RECEIVE,
		FLUSHSOCKETEXCHANGE_OPER_SEND,
		FLUSHSOCKETEXCHANGE_OPER_RECEIVESTREAM,
		FLUSHSOCKETEXCHANGE_OPER_SENDSTREAM,
		FLUSHSOCKETEXCHANGE_OPER_CLOSE,
	};	
	
	/**
	 * @class __initialAccept holds info that passes to accept call, and then inits flushSocketExchange;
	 */
	class __initialAccept
	{
		friend class flushSocketExchange;
		friend class flushSocket;
		
		public:			
		
			/**
			 * constructor
			 */
			__initialAccept();
			
			/**
			 * copy constructor
			 * @note if you want to copy it, the object, from what was copy is not be able to init new session => you have to reinit it with accept method!
			 */
			__initialAccept(__initialAccept &init);
		
		
		private:
				
			int socket;///< id of socket
			
			short family;///< socket family[see socketProtoFamilyEnum]
			short type;///< socket type[see socketTransferTypeEnum]
			
			bool blocked;///< if blocked
			bool blockInherited;///< if block inherited	
	};
		
	/**
	 * @class flushSocketExchange used for communication[send/receive data]
	 * you may use it's functions only after passing it to connect(accept)
	 * otherwise you'll receive exeptions about socket(or false) from all of this' class' methods
	 * if you'll init this class again with another connection = previous will be closed
	 */
	class flushSocketExchange : public flushSocketOptions	
	
	#ifndef FLUSH_SOCKETEXCHANGE_WO_XEXEC
								, public xexec
	#endif
	
	 
	 {
	 	
	 	friend class flushSocket;
	 	
		public:
											
			/**
			 * constructor
			 */	
			flushSocketExchange();
			
			/**
			 * copy constructor
			 * @note object that inited new object of this class you can use for future connections; 
			 * you can safely pass it to the functions;
			 */	
			flushSocketExchange(flushSocketExchange &fse);

			/**
			 * constructor
			 * @param init is initial data[got from accept method]
			 * @note object that inited new object of this class you can use for future connections; 
			 */	
			flushSocketExchange(__initialAccept &init);
		
			/**
			 * destructor
			 */
			virtual ~flushSocketExchange();

			/**
			 * @return copy of object
			 */
			virtual flushSocketExchange *createCopy();
					
			/**
			 * deletes copy of object
			 * @param copy is copy of object to delete
			 */		
			static void deleteCopy(flushSocketExchange *copy);
			
			#ifndef FLUSH_SOCKETEXCHANGE_WO_XEXEC
					
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
					virtual xexecCounts addExec(const dodoString &module, void *data, void *toInit = NULL);
							
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 * @param toInit indicates data that will path to initialize function
					 */
					virtual int addPostExec(const dodoString &module, void *data, void *toInit = NULL);
					
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 * @param toInit indicates data that will path to initialize function
					 */
					virtual int addPreExec(const dodoString &module, void *data, void *toInit = NULL);
					
				#endif	
			
			#endif
				
			/**
			 * init oject with given data
			 * @param init is initial data[got from accept method]
			 */
			virtual void init(__initialAccept &init);
		
			/**
			 * @return true if connection is alive
			 */
			virtual bool alive();
			
			/**
			 * send
			 * @param data is data that would be sent
			 * @param urgent -> send out-of-band data
			 * @note sends no longer than outSize
			 * if outSize bigger than socket buffer size - sends with few iterations
			 * signal safe
			 */			
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							send(const char * const data, bool urgent = false);
			
			/**
			 * send
			 * @param data is string that would be sent
			 * @param urgent -> send out-of-band data
			 * @note sends no longer than outSize
			 * if outSize bigger than socket buffer size - sends with few iterations
			 * signal safe
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif				
							sendString(const dodoString &data, bool urgent = false);
			
			/**
			 * receive
			 * @param data is data that would be received
			 * @param urgent -> receives out-of-band data
			 * @note receives no longer than inSize
			 * if inSize bigger than socket buffer size - receives with few iterations
			 * signal safe
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool
			#endif
							receive(char * const data, bool urgent = false);
							 
			/**
			 * receive
			 * @param data is string that would be received
			 * @param urgent -> receives out-of-band data
			 * @note receives no longer than inSize
			 * if inSize bigger than socket buffer size - receives with few iterations
			 * signal safe
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif				
							receiveString(dodoString &data, bool urgent = false);///urgent = true -> Receipt  of out-of-band data

			/**
			 * send - null-terminated string
			 * @param data is data that would be sent
			 * @param urgent -> send out-of-band data
			 * @note sends no longer than outSize
			 * if outSize bigger than socket buffer size - sends with few iterations
			 * max data size is outSocketBuffer
			 * @note - appends '\n'
			 * signal safe
			 */			
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							sendStream(const char * const data, bool urgent = false);
			
			/**
			 * send - null-terminated string
			 * @param data is string that would be sent
			 * @param urgent -> send out-of-band data
			 * @note sends no longer than outSize
			 * if outSize bigger than socket buffer size - sends with few iterations
			 * max data size is outSocketBuffer
			 * @note - appends '\n'
			 * signal safe
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif				
							sendStreamString(const dodoString &data, bool urgent = false);
			
			/**
			 * receive - null-terminated string
			 * @param data is data that would be received
			 * @param urgent -> receives out-of-band data
			 * @note receives no longer than inSize
			 * if inSize bigger than socket buffer size - receives with few iterations
			 * max data size is inSocketBuffer
			 * signal safe
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							receiveStream(char * const data, bool urgent = false);
			
			/**
			 * receive - null-terminated string
			 * @param data is string that would be received
			 * @param urgent -> receives out-of-band data
			 * @note receives no longer than inSize
			 * if inSize bigger than socket buffer size - receives with few iterations
			 * max data size is inSocketBuffer
			 * signal safe
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif				
							receiveStreamString(dodoString &data, bool urgent = false);///urgent = true -> Receipt  of out-of-band data
						
			/**
			 * closes this socket
			 */			
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							close();
			
		protected:	
					
			/**
			 * inits this class' data
			 * @param socket is id of socket
			 * @param blockInherited indicates whether to inherit block of parent
			 */		
			virtual void init(int socket, bool blockInherited);	
	 };

};

#endif