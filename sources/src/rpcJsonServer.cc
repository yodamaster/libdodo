/***************************************************************************
 *            rpcJsonServer.cc
 *
 *  Mon Jul 07 2008
 *  Copyright  2008  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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

#include <libdodo/directives.h>

#include <libdodo/rpcJsonServer.h>
#include <libdodo/types.h>
#include <libdodo/dataFormatJsonProcessor.h>
#include <libdodo/dataFormatJsonNode.h>
#include <libdodo/ioChannel.h>
#include <libdodo/rpcServer.h>
#include <libdodo/rpcJsonMethod.h>
#include <libdodo/rpcMethod.h>
#include <libdodo/rpcJsonResponse.h>
#include <libdodo/rpcResponse.h>
#include <libdodo/exceptionBasic.h>

using namespace dodo::rpc::json;

__additional__::__additional__(dodo::string &version,
                               long       &id) : version(version),
                                                 id(id)
{
}

//-------------------------------------------------------------------

server::server(io::channel &io) : rpc::server(io),
                                  rpVersion("1.1"),
                                  rqId(0),
                                  rpId(0)
{
}

//-------------------------------------------------------------------

server::~server()
{
}

//-------------------------------------------------------------------

dodo::rpc::method
server::processCallRequest()
{
    dodo::data::format::json::processor jsonValue;

    dodo::data::format::json::node node = jsonValue.process(io);

    return method::jsonToMethod(node, rqVersion, rqId);
}

//-------------------------------------------------------------------

void
server::processCallResult(const rpc::response &resp)
{
    dodo::data::format::json::processor jsonValue;

    jsonValue.make(response::responseToJson(resp, rpVersion, rpId), io);

    io.flush();
}

//-------------------------------------------------------------------

void
server::setResponseVersion(const dodo::string &version)
{
    rpVersion = version;
}

//-------------------------------------------------------------------

void
server::serve()
{
    dodo_try {
        rpc::method meth = processCallRequest();

        dodo::string version = rqVersion;

        __additional__ idata(rqVersion, rqId);

        rpId = rqId;
        __additional__ odata(rpVersion, rpId);

        dodoMap<dodo::string, handler, dodoMapStringCompare>::iterator handler = handlers.find(meth.name);

        if (handler == handlers.end())
            processCallResult(defaultHandler(meth.name, meth.arguments, &idata, &odata));
        else
            processCallResult(handler->second(meth.name, meth.arguments, &idata, &odata));

        rpVersion = version;
    } dodo_catch (exception::basic *e UNUSED) {
        rpc::response response;
        response.fault(e->errStr);

        rpId = rqId;

        processCallResult(response);
    }
}

//-------------------------------------------------------------------

