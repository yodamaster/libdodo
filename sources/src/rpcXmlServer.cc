/***************************************************************************
 *            rpcXmlServer.cc
 *
 *  Wed Apr 09 2008
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

#include <libdodo/rpcXmlServer.h>
#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/rpcServer.h>
#include <libdodo/rpcXmlMethod.h>
#include <libdodo/rpcMethod.h>
#include <libdodo/rpcXmlResponse.h>
#include <libdodo/rpcResponse.h>
#include <libdodo/dataFormatXmlProcessor.h>
#include <libdodo/dataFormatXmlNode.h>
#include <libdodo/exceptionBasic.h>

using namespace dodo::rpc::xml;

__additional__::__additional__(dodo::string &encoding) : encoding(encoding)
{
}

//-------------------------------------------------------------------

server::server(io::channel &io) : rpc::server(io),
                                  rpEncoding("UTF-8")
{
}

//-------------------------------------------------------------------

server::~server()
{
}

//-------------------------------------------------------------------

void
server::setResponseEncoding(const dodo::string &a_encoding)
{
    rpEncoding = a_encoding;
}

//-------------------------------------------------------------------

dodo::rpc::method
server::processCallRequest()
{
    dodo::data::format::xml::processor xmlValue;

    dodo::data::format::xml::__definition__ xmlMethodCall;
    xmlMethodCall.name = "methodCall";
    xmlMethodCall.allChildren = true;

    dodo::data::format::xml::node node = xmlValue.process(xmlMethodCall, io);

    rqEncoding = xmlValue.information().encoding;

    return method::xmlToMethod(node);
}

//-------------------------------------------------------------------

void
server::processCallResult(const rpc::response &resp)
{
    dodo::data::format::xml::processor xmlValue;

    xmlValue.make(response::responseToXml(resp), rpEncoding, "1.0", io);

    io.flush();
}

//-------------------------------------------------------------------

void
server::serve()
{
    dodo_try {
        rpc::method meth = processCallRequest();

        dodo::string encoding = rpEncoding;

        __additional__ idata(rqEncoding);

        __additional__ odata(rpEncoding);

        dodoMap<dodo::string, handler, dodoMapStringCompare>::iterator handler = handlers.find(meth.name);

        if (handler == handlers.end())
            processCallResult(defaultHandler(meth.name, meth.arguments, &idata, &odata));
        else
            processCallResult(handler->second(meth.name, meth.arguments, &idata, &odata));

        rpEncoding = encoding;
    } dodo_catch (exception::basic *e UNUSED) {
        rpc::response response;
        response.fault(e->errStr);

        processCallResult(response);
    }
}

//-------------------------------------------------------------------

