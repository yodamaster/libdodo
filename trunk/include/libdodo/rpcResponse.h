/***************************************************************************
 *            rpcResponse.h
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

#ifndef _RPCRESPONSE_H_
#define _RPCRESPONSE_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/stringTools.h>
#include <libdodo/rpcValue.h>

namespace dodo
{	
	/**
	 * @class rpcResponse defines RPC response reprasentation
	 */
	class rpcResponse
	{
		friend class rpcClient;
		friend class rpcServer;
		friend class rpcXmlResponse;
		
		public:

			/**
			 * constructor
			 */
			rpcResponse();

			/**
			 * destructor
			 */
			virtual ~rpcResponse();

			/**
			 * @return response value
			 * @param position defines index of returned value
			 */
			virtual rpcValue getValue(unsigned long position = 0);

			/**
			 * @return response values
			 */
			virtual dodoArray<rpcValue> getValues();

			/**
			 * @return response values count
			 */
			virtual unsigned long getValuesCount();
			
			/**
			 * @return true if request has  
			 */
			virtual bool isSuccessful();
			
			/**
			 * set argument for fault response
			 * @param argument defines response argument 
			 */
			virtual void fault(const rpcValue &argument);
			
			/**
			 * add argument for successful response
			 * @param argument defines response argument 
			 */
			virtual void addArgument(const rpcValue &argument);
			
			/**
			 * @return rpcValue for type casting  
			 * @param position defines index of returned value
			 */
			virtual rpcValue operator[](unsigned long position); 
			
		private:
			
			dodoArray<rpcValue> values;///< response values
			bool succ;///< true if server returned non-fail response
	};
};

#endif