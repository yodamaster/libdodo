/***************************************************************************
 *            rpcJsonCgiServer.h
 *
 *  Mon Jul 07 11:29:55 2008
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _RPCJSONCGISERVER_H_
#define _RPCJSONCGISERVER_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/cgiServer.h>
#include <libdodo/rpcJsonServer.h>

namespace dodo
{
	namespace rpc
	{
		namespace json
		{
			/**
			 * @class server defines server-side RPC instrument
			 */
			class cgiServer : public server
			{
				public:

					/**
					 * constructor
					 */
					cgiServer(cgi::server &provider);

					/**
					 * destructor
					 */
					virtual ~cgiServer();

				protected:

					/**
					 * send request
					 * @param response defines rpc method call
					 */
					virtual void sendTextRequest(const dodoString &response);

					/**
					 * get response
					 * @return rpc response result
					 */
					virtual dodoString receiveTextResponse();

					cgi::server &provider;
			};
		};
	};
};

#endif