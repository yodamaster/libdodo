/***************************************************************************
 *            rpcXmlCgiServer.cc
 *
 *  Sat Apr 12 23:06:55 2008
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

#include <libdodo/rpcXmlCgiServer.h>

using namespace dodo;

rpcXmlCgiServer::rpcXmlCgiServer(cgi &a_provider) : provider(a_provider)
{
	provider.HEADERS.insert(make_pair(CGI_RESPONSEHEADER_CONTENTTYPE, dodoString("text/xml")));
	
	provider.printHeaders();
}

//-------------------------------------------------------------------

rpcXmlCgiServer::~rpcXmlCgiServer()
{	
}

//-------------------------------------------------------------------

void 
rpcXmlCgiServer::sendTextRequest(const dodoString &response)
{
	provider.print(response);
}

//-------------------------------------------------------------------

dodoString 
rpcXmlCgiServer::receiveTextResponse()
{
	return provider.content;
}

//-------------------------------------------------------------------