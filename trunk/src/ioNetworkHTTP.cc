/***************************************************************************
 *            ioNetworkHTTP.cc
 *
 *  Wed Oct 8 01:44:18 2005
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

#include <libdodo/ioNetworkHTTP.h>

using namespace dodo;

const dodoString ioNetworkHTTP::requestHeaderStatements[] = { "Accept",
		"Accept-Charset",
		"Accept-Encoding",
		"Accept-Language",
		"Accept-Ranges",
		"Authorization",
		"Connection",
		"Date",
		"If-Modified-Since",
		"User-Agent",
		"Cookie",
};

//-------------------------------------------------------------------

const dodoString ioNetworkHTTP::responseHeaderStatements[] = { "Accept-Ranges",
		"Age",
		"Allow",
		"Cache-Control",
		"Content-Encoding",
		"Content-Language",
		"Content-Length",
		"Content-Location",
		"Content-Disposition",
		"Content-MD5",
		"Content-Range",
		"Content-Type",
		"Date",
		"Last-Modified",
		"Location",
		"Server",
		"WWW-Authenticate",
};

//-------------------------------------------------------------------

const char ioNetworkHTTP::trimSymbols[] = {' ',
		'\r'
};

//-------------------------------------------------------------------

__httpResponse::__httpResponse() : code(0),
								redirected(false)
{
}

//-------------------------------------------------------------------

ioNetworkHTTP::ioNetworkHTTP() : httpStatusRE("^HTTP/[0-9].[0-9]\\s([0-9]+)\\s.*$"),
								followRedirection(true),
								authTries(0)
{	
	requestHeaders[IONETWORKHTTP_REQUESTHEADER_USERAGENT] = PACKAGE_NAME "/" PACKAGE_VERSION;
	requestHeaders[IONETWORKHTTP_REQUESTHEADER_ACCEPT] = "*/*";
	requestHeaders[IONETWORKHTTP_REQUESTHEADER_CONNECTION] = "Keep-Alive";
}

//-------------------------------------------------------------------

ioNetworkHTTP::ioNetworkHTTP(ioNetworkHTTP &fd)
{
}

//-------------------------------------------------------------------

ioNetworkHTTP::~ioNetworkHTTP()
{
}

//-------------------------------------------------------------------

__httpResponse 
ioNetworkHTTP::getResponse()
{
	return response;
}

//-------------------------------------------------------------------

void 
ioNetworkHTTP::setUrl(const dodoString &a_url)
{
	url = tools::parseURL(a_url);
}

//-------------------------------------------------------------------

void 
ioNetworkHTTP::setCookies(const dodoStringMap &cookies)
{
	dodoString data;
	
	dodoStringMap::const_iterator i = cookies.begin(), j = cookies.end();
	--j;
	
	for (;i!=j;++i)
	{
		data.append(i->first);
		data.append("=");
		data.append(tools::encodeURL(i->second));
		data.append("; ");
	}
	data.append(i->first);
	data.append("=");
	data.append(tools::encodeURL(i->second));
	
	requestHeaders[IONETWORKHTTP_REQUESTHEADER_COOKIE] = data;
}

//-------------------------------------------------------------------

void
ioNetworkHTTP::GET()
{	
	response = __httpResponse();
	
	ioNetworkExchange ex;
	ioNetwork net(false, IONETWORKOPTIONS_PROTO_FAMILY_IPV4, IONETWORKOPTIONS_TRANSFER_TYPE_STREAM);
	
	__hostInfo host = ioNetworkTools::getHostInfo(url.host);
	
	dodoString protocol = url.protocol; 
	if (protocol.size() == 0)
		protocol = "http";
	
	int port = stringTools::stringToI(url.port);
	if (port == 0)
	{
		if (stringTools::iequal(protocol, "http"))
			port = 80;
	}
	
	dodoStringArray::iterator o = host.addresses.begin(), p = host.addresses.end();
	for (;o!=p;++o)
		try
		{
			net.connect(*o, port, ex);
			break;
		}
		catch (baseEx &ex)
		{
			if (ex.funcID == IONETWORKEX_CONNECT)
			{
				if (*o == *p)
					throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GET, ERR_LIBDODO, IONETWORKHTTPEX_CANNOTCONNECT, IONETWORKHTTPEX_CANNOTCONNECT_STR, __LINE__, __FILE__);
				else
					continue;
			}
			
			throw;
		}
	
	dodoString data;
	
	data.append("GET /");
	data.append(url.path);
	data.append(url.request);
	data.append(" HTTP/1.0\r\n");
	dodoMap<short, dodoString>::iterator i(requestHeaders.begin()), j(requestHeaders.end());
	for (;i!=j;++i)
	{
		data.append(requestHeaderStatements[i->first]);
		data.append(": ");
		data.append(i->second);
		data.append("\r\n");
	}
	data.append("Host: ");
	data.append(url.host);
	
	data.append("\r\n\r\n");
	
	ex.writeStreamString(data);
	
	switch (getContent(data, ex))
	{
		case GETCONTENTSTATUS_NORMAL:
			
			break;
			
		case GETCONTENTSTATUS_REDIRECT:
			
			url = tools::parseURL(response.headers[IONETWORKHTTP_RESPONSEHEADER_LOCATION]);
			
			GET();
			
			break;
			
		case GETCONTENTSTATUS_BASICAUTH:
			
			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_POST, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}
			
			requestHeaders[IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION] = "Basic " + tools::encodeBase64(url.login + ":" + url.password);
			
			GET();
			
			break;
			
		case GETCONTENTSTATUS_DIGESTAUTH:
			
			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_POST, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}
			
			dodoString nonce, opaque, realm;
			
			dodoStringArray parts = tools::explode(response.headers[IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE].substr(6), &trim, ",");

			dodoString HA1;

			unsigned char HA[16];			
			tools::MD5_CTX context;
			
			dodoStringArray tuple;
			
			dodoStringArray::iterator i = parts.begin(), j = parts.end();
			for (;i!=j;++i)
			{
				tuple = tools::explode(*i, "=");
				if (tuple.size() != 2)
					continue;
				
				if (stringTools::iequal(tuple[0], "realm"))
				{
					realm = stringTools::trim(tuple[1], '"');
					
					tools::MD5Init(&context);
					tools::MD5Update(&context, (unsigned char *)url.login.c_str(), url.login.size());
					tools::MD5Update(&context, (unsigned char *)":", 1);
					tools::MD5Update(&context, (unsigned char *)realm.c_str(), realm.size());
					tools::MD5Update(&context, (unsigned char *)":", 1);
					tools::MD5Update(&context, (unsigned char *)url.password.c_str(), url.password.size());
					tools::MD5Final(HA, &context);
					
					HA1 = tools::binToHex(dodoString((char *)&HA, 16));
				}
				else
				{
					if (stringTools::iequal(tuple[0], "nonce"))
						nonce = stringTools::trim(tuple[1], '"');
					else
					{
						if (stringTools::iequal(tuple[0], "opaque"))
							opaque = stringTools::trim(tuple[1], '"');
					}
				}
			}
			
			dodoString uri = "/" + url.path + url.request;
			dodoString cnonce = tools::MD5Hex(tools::stringRandom(5));

			tools::MD5Init(&context);
			tools::MD5Update(&context, (unsigned char *)"GET:", 4);
			tools::MD5Update(&context, (unsigned char *)uri.c_str(), uri.size());
			tools::MD5Final(HA, &context);


			dodoString HA2 = tools::binToHex(dodoString((char *)&HA, 16));
			
			tools::MD5Init(&context);
			tools::MD5Update(&context, (unsigned char *)HA1.c_str(), HA1.size());
			tools::MD5Update(&context, (unsigned char *)":", 1);
			tools::MD5Update(&context, (unsigned char *)nonce.c_str(), nonce.size());
			tools::MD5Update(&context, (unsigned char *)":00000001:", 10);
			tools::MD5Update(&context, (unsigned char *)cnonce.c_str(), cnonce.size());
			tools::MD5Update(&context, (unsigned char *)":auth:", 6);
			tools::MD5Update(&context, (unsigned char *)HA2.c_str(), HA2.size());
			tools::MD5Final(HA, &context);

			dodoString response = tools::binToHex(dodoString((char *)&HA, 16));

			requestHeaders[IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION] = "Digest username=\"" + url.login + 
																		"\", realm=\"" + realm + 
																		 + "\", nonce=\"" + nonce + 
																		 + "\", uri=\"" + uri + 
																		 + "\", qop=auth" + 
																		 + ", nc=00000001" + 
																		 + ", cnonce=\"" + cnonce + 
																		 + "\", response=\"" + response + 
																		 + "\", opaque=\"" + opaque + "\"";
			
			GET();
			
			break;
	}
}

//-------------------------------------------------------------------


__httpResponse
ioNetworkHTTP::GET(const dodoString &a_url)
{
	url = tools::parseURL(a_url);
	
	GET();
	
	return response;
}

//-------------------------------------------------------------------

__httpResponse 
ioNetworkHTTP::POST(const dodoString &a_url, 
							const dodoStringMap &arguments, 
							const dodoStringMap &files)
{
	url = tools::parseURL(a_url);
	
	POST(arguments, files);
	
	return response;
}

//-------------------------------------------------------------------

void
ioNetworkHTTP::POST(const dodoStringMap &arguments, 
							const dodoStringMap &files)
{
	dodoString boundary = "---------------------------" + stringTools::ulToString(tools::ulRandom()) + stringTools::ulToString(tools::ulRandom());
	dodoString type = "multipart/form-data; boundary=" + boundary;
	boundary.insert(0, "--");
	
	dodoString data;
	
	dodoStringMap::const_iterator i = files.begin(), j = files.end();
	
	for (;i!=j;++i)
	{
		data.append(boundary);
		data.append("\r\nContent-Disposition: form-data; name=\"");
		data.append(i->first);
		data.append("\"; filename=\"");
		data.append(ioDiskTools::lastname(i->second));
		data.append("\"\r\n");
		
		data.append("Content-Type: application/octet-stream\r\n\r\n");
		
		data.append(ioDiskTools::getFileContents(i->second));
		data.append("\r\n");
	}

	i = arguments.begin();
	j = arguments.end();
	for (;i!=j;++i)
	{
		data.append(boundary);
		data.append("\r\nContent-Disposition: form-data; name=\"");
		data.append(i->first);
		data.append("\"\r\n\r\n");
		
		data.append(i->second);
		data.append("\r\n");
	}
	data.append(boundary);
	data.append("--");
	
	POST(data, type);
}

//-------------------------------------------------------------------

__httpResponse 
ioNetworkHTTP::POST(const dodoString &a_url, 
					const dodoStringMap &arguments)
{
	url = tools::parseURL(a_url);
	
	POST(arguments);
	
	return response;
}

//-------------------------------------------------------------------

void 
ioNetworkHTTP::POST(const dodoStringMap &arguments)
{
	dodoString data;
	
	dodoStringMap::const_iterator i(arguments.begin()), j(arguments.end());
	--j;
	
	for (;i!=j;++i)
	{
		data.append(tools::encodeURL(i->first));
		data.append("=");
		data.append(tools::encodeURL(i->second));
		data.append("&");
	}
	data.append(tools::encodeURL(i->first));
	data.append("=");
	data.append(tools::encodeURL(i->second));
	
	POST(data, "application/x-www-form-urlencoded");
}

//-------------------------------------------------------------------

__httpResponse
ioNetworkHTTP::POST(const dodoString &a_url, 
					const dodoString &data,
					const dodoString &type)
{
	url = tools::parseURL(a_url);
	
	POST(data, type);
	
	return response;
}

//-------------------------------------------------------------------

void
ioNetworkHTTP::POST(const dodoString &a_data,
					const dodoString &type)
{	
	response = __httpResponse();
	
	ioNetworkExchange ex;
	ioNetwork net(false, IONETWORKOPTIONS_PROTO_FAMILY_IPV4, IONETWORKOPTIONS_TRANSFER_TYPE_STREAM);
	
	__hostInfo host = ioNetworkTools::getHostInfo(url.host);
	
	dodoString protocol = url.protocol; 
	if (protocol.size() == 0)
		protocol = "http";
	
	int port = stringTools::stringToI(url.port);
	if (port == 0)
	{
		if (stringTools::iequal(protocol, "http"))
			port = 80;
	}
	
	dodoStringArray::iterator o = host.addresses.begin(), p = host.addresses.end();
	for (;o!=p;++o)
		try
		{
			net.connect(*o, port, ex);
			break;
		}
		catch (baseEx &ex)
		{
			if (ex.funcID == IONETWORKEX_CONNECT)
			{
				if (*o == *p)
					throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_POST, ERR_LIBDODO, IONETWORKHTTPEX_CANNOTCONNECT, IONETWORKHTTPEX_CANNOTCONNECT_STR, __LINE__, __FILE__);
				else
					continue;
			}
			
			throw;
		}
	
	dodoString data;
	
	data.append("POST /");
	data.append(url.path);
	data.append(url.request);
	data.append(" HTTP/1.0\r\n");
	dodoMap<short, dodoString>::iterator i(requestHeaders.begin()), j(requestHeaders.end());
	for (;i!=j;++i)
	{
		data.append(requestHeaderStatements[i->first]);
		data.append(": ");
		data.append(i->second);
		data.append("\r\n");
	}
	data.append("Host: ");
	data.append(url.host);
	data.append("\r\n");
	
	data.append("Content-length: ");
	data.append(stringTools::ulToString(a_data.size()));
	data.append("\r\n");
	
	data.append("Content-type: ");
	data.append(type);
	data.append("\r\n\r\n");
	
	ex.writeStreamString(data);
	
	ex.outSize = a_data.size();
	ex.writeString(a_data);
	
	switch (getContent(data, ex))
	{
		case GETCONTENTSTATUS_NORMAL:
			
			break;
			
		case GETCONTENTSTATUS_REDIRECT:
			
			url = tools::parseURL(response.headers[IONETWORKHTTP_RESPONSEHEADER_LOCATION]);
			
			POST(data, type);
			
			break;
			
		case GETCONTENTSTATUS_BASICAUTH:
			
			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_POST, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}
			
			requestHeaders[IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION] = "Basic" + tools::encodeBase64(url.login + ":" + url.password);
			
			POST(data, type);
			
			break;
			
		case GETCONTENTSTATUS_DIGESTAUTH:
			
			if (authTries > 2)
			{
				authTries = 0;
				
				throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_POST, ERR_LIBDODO, IONETWORKHTTPEX_NOTAUTHORIZED, IONETWORKHTTPEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
			}
			
			dodoString nonce, opaque, realm;
			
			dodoStringArray parts = tools::explode(response.headers[IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE].substr(6), &trim, ",");

			dodoString HA1;

			unsigned char HA[16];			
			tools::MD5_CTX context;
			
			dodoStringArray tuple;
			
			dodoStringArray::iterator i = parts.begin(), j = parts.end();
			for (;i!=j;++i)
			{
				tuple = tools::explode(*i, "=");
				if (tuple.size() != 2)
					continue;
				
				if (stringTools::iequal(tuple[0], "realm"))
				{
					realm = stringTools::trim(tuple[1], '"');
					
					tools::MD5Init(&context);
					tools::MD5Update(&context, (unsigned char *)url.login.c_str(), url.login.size());
					tools::MD5Update(&context, (unsigned char *)":", 1);
					tools::MD5Update(&context, (unsigned char *)realm.c_str(), realm.size());
					tools::MD5Update(&context, (unsigned char *)":", 1);
					tools::MD5Update(&context, (unsigned char *)url.password.c_str(), url.password.size());
					tools::MD5Final(HA, &context);

					HA1 = tools::binToHex(dodoString((char *)&HA, 16));
				}
				else
				{
					if (stringTools::iequal(tuple[0], "nonce"))
						nonce = stringTools::trim(tuple[1], '"');
					else
					{
						if (stringTools::iequal(tuple[0], "opaque"))
							opaque = stringTools::trim(tuple[1], '"');
					}
				}
			}
			
			dodoString uri = "/" + url.path + url.request;
			dodoString cnonce = tools::MD5Hex(tools::stringRandom(5));
			
			tools::MD5Init(&context);
			tools::MD5Update(&context, (unsigned char *)"POST:", 5);
			tools::MD5Update(&context, (unsigned char *)uri.c_str(), uri.size());
			tools::MD5Final(HA, &context);

			dodoString HA2 = tools::binToHex(dodoString((char *)&HA, 16));
			
			tools::MD5Init(&context);
			tools::MD5Update(&context, (unsigned char *)HA1.c_str(), HA1.size());
			tools::MD5Update(&context, (unsigned char *)":", 1);
			tools::MD5Update(&context, (unsigned char *)nonce.c_str(), nonce.size());
			tools::MD5Update(&context, (unsigned char *)":00000001:", 10);
			tools::MD5Update(&context, (unsigned char *)cnonce.c_str(), cnonce.size());
			tools::MD5Update(&context, (unsigned char *)":auth:", 6);
			tools::MD5Update(&context, (unsigned char *)HA2.c_str(), HA2.size());
			tools::MD5Final(HA, &context);

			dodoString response = tools::binToHex(dodoString((char *)&HA, 16));

			requestHeaders[IONETWORKHTTP_REQUESTHEADER_AUTHORIZATION] = "Digest username=\"" + url.login + 
																		"\", realm=" + realm + 
																		 + ", nonce=" + nonce + 
																		 + ", uri=\"" + uri + 
																		 + "\", qop=auth" + 
																		 + ", nc=00000001" + 
																		 + ", cnonce=\"" + cnonce + 
																		 + "\", response=\"" + response + 
																		 + "\", opaque=\"" + opaque + "\"";
			
			POST(data, type);

			break;
	}
}

//-------------------------------------------------------------------

bool
ioNetworkHTTP::extractHeaders(const dodoString &data,
						__httpResponse &response)
{
	unsigned long i(0), j(0);
	unsigned long size = data.size();
	dodoStringArray arr;
	dodoString piece;
	
	short o;
	
	bool statusCode = false;
	
	while (i < size)
	{
		i = data.find("\n", i);
		if (i == dodoString::npos)
		{
			response.data.append(data.substr(j));
			
			return true;
		}
		
		piece = stringTools::trim(data.substr(j, i - j), '\r');
		if (piece.size() == 0)
		{
			response.data.append(data.substr(i + 1));
			
			return true;
		}

		arr = tools::explode(piece, ":", 2);
		if (arr.size() != 2)
		{
			if (!statusCode)
			{ 
				statusCode = true;

				if (httpStatusRE.match(piece, arr))
					response.code = stringTools::stringToS(stringTools::trim(arr[0], trimSymbols, 2));
			}
			else
			{
				response.data.append(data.substr(j));
				
				return true;
			}
		}
		else
		{
			for (o = 0;o<IONETWORKHTTP_RESPONSEHEADERSTATEMENTS;++o)
				if (stringTools::equal(responseHeaderStatements[o], arr[0]))
					response.headers[o] = stringTools::trim(arr[1], trimSymbols, 2);
			
			if (stringTools::equal("Set-Cookie", arr[0]))
				response.cookies.push_back(parseCookie(stringTools::trim(arr[1], trimSymbols, 2)));
		}
		
		i += 1;
		j = i;
	}
	
	response.data.append(data.substr(j));
	
	return false;
}

//-------------------------------------------------------------------

short 
ioNetworkHTTP::getContent(dodoString &data, 
						ioNetworkExchange &ex)
{
	ex.setInBufferSize(2048);
	ex.inSize = 2048;
	
	unsigned long contentSize = 0;
	bool endOfHeaders = false;
	
	while (true)
	{
		try
		{
			ex.readStreamString(data);
			
			if (data.size() == 0 && contentSize <= 0)
				break;
			
			if (endOfHeaders)
				response.data.append(data);
			else
			{
				endOfHeaders = extractHeaders(data, response);
				
				if (endOfHeaders)
				{
					contentSize = stringTools::stringToUL(response.headers[IONETWORKHTTP_RESPONSEHEADER_CONTENTLENGTH]);

					if (followRedirection && (response.code / 100) == 3 && response.code != 304)
					{
						response.redirected = true;
					
						return GETCONTENTSTATUS_REDIRECT;
					}
					
					if (response.code == 401)
					{
						++authTries;
						
						if (stringTools::contains(response.headers[IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE], "Basic"))
							return GETCONTENTSTATUS_BASICAUTH;
						else 
						{
							if (stringTools::contains(response.headers[IONETWORKHTTP_RESPONSEHEADER_WWWAUTHENTICATE], "Digest"))
								return GETCONTENTSTATUS_DIGESTAUTH;
							else
								throw baseEx(ERRMODULE_IONETWORKHTTP, IONETWORKHTTPEX_GETCONTENT, ERR_LIBDODO, IONETWORKHTTPEX_UNKNOWNAUTHTYPE, IONETWORKHTTPEX_UNKNOWNAUTHTYPE_STR, __LINE__, __FILE__);
						}
					}
					
					ex.setInBufferSize(16384);
					ex.inSize = 16384;
				}
			}
			
			if (contentSize > 0 && response.data.size() == contentSize)
				break;
		}
		catch (baseEx &ex)
		{
			if (ex.funcID == IONETWORKEXCHANGEEX__READSTREAM)
				break;
			else
				throw;
		}
	}
	
	authTries = 0;
	
	return GETCONTENTSTATUS_NORMAL;
}

//-------------------------------------------------------------------

dodoString 
ioNetworkHTTP::trim(const dodoString &data)
{
	return stringTools::trim(data, ' ');
}

//-------------------------------------------------------------------

__cookie 
ioNetworkHTTP::parseCookie(const dodoString &header)
{
	dodoStringArray parts = tools::explode(header, &trim, ";");
	dodoStringArray tuple;
	
	dodoStringArray::iterator i = parts.begin(), j = parts.end();
	
	tuple = tools::explode(*i, "=", 2);
	if (tuple.size() != 2)
		return __cookie();
		
	__cookie cookie;
	cookie.name = tuple[0];
	cookie.value = tools::decodeURL(tuple[1]);
	
	++i;
	
	for (;i!=j;++i)
	{
		tuple = tools::explode(*i, "=");
		
		if (stringTools::iequal(tuple[0], "path"))
			cookie.path = tuple[1];
		else
		{
			if (stringTools::iequal(tuple[0], "expires"))
				cookie.expires = tuple[1];
			else
			{
				if (stringTools::iequal(tuple[0], "domain"))
					cookie.domain = tuple[1];
				else
				{
					if (stringTools::iequal(tuple[0], "secure"))
						cookie.secure = true;
				}
			}
		}
	}
	
	return cookie;
}

//-------------------------------------------------------------------
