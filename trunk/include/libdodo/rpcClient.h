/***************************************************************************
 *            rpcClient.h
 *
 *  Sat Mar 22 00:06:55 2008
 *  Copyright  2008  Ni@m
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

#ifndef _RPCCLIENT_H_
#define _RPCCLIENT_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/stringTools.h>
#include <libdodo/rpcMethod.h>
#include <libdodo/rpcResponse.h>

namespace dodo
{	
	class ioHTTP;
	
	/**
	 * @class rpcClient defines 
	 */
	class rpcClient
	{
		public:

			/**
			 * constructor
			 */
			rpcClient();

			/**
			 * destructor
			 */
			virtual ~rpcClient() = 0;
		
			/**
			 * @param method defines rpc method call
			 */
			virtual void sendRequest(const rpcMethod &method) = 0;
			
			/**
			 * @return rpc response result 
			 */
			virtual rpcResponse receiveResponse() = 0;
			
			/**
			 * set transport layer provider
			 * @param provider defines trasport layer provider
			 */
			virtual void setIOProvider(ioHTTP *provider);
		
		protected:
			
			ioHTTP *ioProvider;
	};
};

#endif
