/***************************************************************************
 *            rpcJsonServer.cc
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

#include <libdodo/rpcJsonServer.h>

using namespace dodo::rpc::json;

server::server()  : rpVersion("1.1")
{
}

//-------------------------------------------------------------------

server::~server()
{
}

//-------------------------------------------------------------------

dodo::rpc::method
server::processCall(const dodoString &data)
{
	dodo::json::processor jsonValue;

	dodo::json::node node = jsonValue.process(data);

	return method::jsonToMethod(node, rqVersion);
}

//-------------------------------------------------------------------

dodoString
server::processCallResult(const rpc::response &resp)
{
	dodo::json::processor jsonValue;

	return jsonValue.make(response::responseToJson(resp, rpVersion));
}

//-------------------------------------------------------------------

void 
server::setResponseVersion(const dodoString &version)
{
	rpVersion = version;
}

//-------------------------------------------------------------------

dodoString 
server::getRequestVersion()
{
	return rqVersion;
}

//-------------------------------------------------------------------