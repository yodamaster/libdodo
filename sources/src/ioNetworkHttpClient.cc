/***************************************************************************
 *            ioNetworkHttpClient.cc
 *
 *  Wed Oct 8 2005
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ioSsl.inline"

#include <libdodo/ioNetworkHttpClient.h>
#include <libdodo/toolsCode.h>
#include <libdodo/toolsString.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioNetworkClient.h>
#include <libdodo/ioNetworkSslClient.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/ioNetworkHttpClientEx.h>
#include <libdodo/ioNetworkClientEx.h>
#include <libdodo/ioNetworkSslClientEx.h>
#include <libdodo/ioNetworkExchange.h>
#include <libdodo/ioNetworkExchangeEx.h>
#include <libdodo/ioNetworkSslExchangeEx.h>
#include <libdodo/ioNetworkSslExchange.h>
#include <libdodo/types.h>
#include <libdodo/cgi.h>
#include <libdodo/toolsMisc.h>

using namespace dodo::io::network::http;

const dodoString client::requestHeaderStatements[] = {
    "Accept",
    "Accept-Charset",
    "Accept-Encoding",
    "Accept-Language",
    "Accept-Ranges",
    "Authorization",
    "Proxy-Authorization",
    "Connection",
    "Date",
    "If-Modified-Since",
    "User-Agent",
    "Cookie",
};

//-------------------------------------------------------------------

const dodoString client::responseHeaderStatements[] = {
    "Accept-Ranges",
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
    "Transfer-Encoding",
    "WWW-Authenticate",
    "Proxy-Authenticate",
    "X-Powered-By",
};

//-------------------------------------------------------------------

response::response() : code(0),
                       redirected(false)
{
}

//-------------------------------------------------------------------

file::file(const dodoString path,
           const dodoString mime) : path(path),
                                    mime(mime)
{
}

//-------------------------------------------------------------------

file::file()
{
}

//-------------------------------------------------------------------

client::client() : followRedirection(true),
                   cacheAuthentification(true),
                   authTries(0),
                   scheme(SCHEME_HTTP)
#ifdef OPENSSL_EXT
                   ,
                   certsSet(false)
#endif
{
    requestHeaders[REQUEST_HEADER_USERAGENT] = PACKAGE_NAME "/" PACKAGE_VERSION;
    requestHeaders[REQUEST_HEADER_ACCEPT] = "*/*";
    requestHeaders[REQUEST_HEADER_CONNECTION] = "Close";
}

//-------------------------------------------------------------------

client::client(const dodoString &url) : followRedirection(true),
                                        cacheAuthentification(true),
                                        authTries(0),
                                        scheme(SCHEME_HTTP)
#ifdef OPENSSL_EXT
                                        ,
                                        certsSet(false)
#endif
{
    requestHeaders[REQUEST_HEADER_USERAGENT] = PACKAGE_NAME "/" PACKAGE_VERSION;
    requestHeaders[REQUEST_HEADER_ACCEPT] = "*/*";
    requestHeaders[REQUEST_HEADER_CONNECTION] = "Close";

    setUrl(url);
}

//-------------------------------------------------------------------

client::client(client &)
{
}

//-------------------------------------------------------------------

client::~client()
{
}

//-------------------------------------------------------------------

short
client::getStatusCode(const dodoString &header) const
{
    if (header.size() < 12)
        return 0;

    short code = 0;

    code += ((short)header[9] - 0x30) * 100;
    code += ((short)header[10] - 0x30) * 10;
    code += (short)header[11] - 0x30;

    return code;
}

//-------------------------------------------------------------------

#ifdef OPENSSL_EXT
void
client::setSertificates(const io::ssl::__certificates__ &a_certs) const
{
    certs = a_certs;
    if ((certs.key.size() +
         certs.keyPassword.size() +
         certs.cert.size() +
         certs.ca.size() +
         certs.caPath.size() +
         certs.cipher.size()) > 0)
        certsSet = true;
    else
        certsSet = false;
}
#endif

//-------------------------------------------------------------------

void
client::setUrl(const dodoString &a_url) const
{
    urlComponents = tools::code::parseUrl(a_url);

    if (urlComponents.protocol.size() == 0 || tools::string::iequal(urlComponents.protocol, "http")) {
        urlComponents.protocol = "http";
        scheme = SCHEME_HTTP;
    } else {
#ifdef OPENSSL_EXT
        if (tools::string::iequal(urlComponents.protocol, "https"))
            scheme = SCHEME_HTTPS;
        else
#endif

        throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_SETURL, exception::ERRNO_LIBDODO, CLIENTEX_UNSUPPORTEDSURICHEME, IONETWORKHTTPCLIENTEX_UNSUPPORTEDSURICHEME_STR, __LINE__, __FILE__);
    }

    unsigned long portSize = urlComponents.port.size();

    if (portSize == 0) {
        if (scheme == SCHEME_HTTP)
            urlComponents.port = "80";

#ifdef OPENSSL_EXT
        else if (scheme == SCHEME_HTTPS)
            urlComponents.port = "443";

#endif
    }

    urlBasePath.clear();
    urlQuery.clear();

    urlBasePath.append(urlComponents.protocol);
    urlBasePath.append("://" );
    urlBasePath.append(urlComponents.host);
    if (portSize > 0) {
        urlBasePath.append(":");
        urlBasePath.append(urlComponents.port);
    }
    urlBasePath.append("/");
    urlBasePath.append(urlComponents.path);

    if (urlComponents.request.size() > 0) {
        urlQuery.append("?");
        urlQuery.append(tools::code::encodeUrl(urlComponents.request));
    }
}

//-------------------------------------------------------------------

void
client::setCookies(const dodoStringMap &cookies) const
{
    dodoString data;

    dodoStringMap::const_iterator i = cookies.begin(), j = cookies.end();
    --j;

    for (; i != j; ++i) {
        data.append(i->first);
        data.append("=");
        data.append(tools::code::encodeUrl(i->second));
        data.append("; ");
    }
    data.append(i->first);
    data.append("=");
    data.append(tools::code::encodeUrl(i->second));

    requestHeaders[REQUEST_HEADER_COOKIE] = data;
}

//-------------------------------------------------------------------

response
client::GET() const
{
    response response;

    exchange *ex = NULL;
    network::client *net = NULL;

    dodoString data;

    if (scheme == SCHEME_HTTP) {
        net = new network::client(connection::PROTOCOL_FAMILY_IPV4, connection::TRANSFER_STREAM);
        ex = new exchange;
    }

#ifdef OPENSSL_EXT
    else {
        net = new ssl::client(connection::PROTOCOL_FAMILY_IPV4, connection::TRANSFER_STREAM);
        ex = new ssl::exchange;
    }
#endif

    if (proxyAuthInfo.host.size() > 0) {
        if (scheme == SCHEME_HTTP)
            net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *ex);

#ifdef OPENSSL_EXT
        else {
            net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *ex);
            data.append("CONNECT ");
            data.append(urlComponents.host);
            data.append(":");
            data.append(urlComponents.port);
            data.append(" HTTP/1.1\r\n");
            if (requestHeaders.find(REQUEST_HEADER_PROXYAUTHORIZATION) != requestHeaders.end()) {
                data.append(requestHeaderStatements[REQUEST_HEADER_PROXYAUTHORIZATION]);
                data.append(": ");
                data.append(requestHeaders[REQUEST_HEADER_PROXYAUTHORIZATION]);
                data.append("\r\n");
            }
            data.append("\r\n");

            unsigned long bs = ex->bs;
            ex->bs = data.size();
            ex->exchange::_write(data.data());
            ex->bs = bs;

            try {
                switch (getProxyConnectResponse(ex, response)) {
                    case GETCONTENTSTATUS_NORMAL:

                        break;

                    case GETCONTENTSTATUS_PROXYBASICAUTH:

                        if (authTries > 2) {
                            authTries = 0;

                            throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                        }

                        makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

                        delete ex;
                        ex = NULL;
                        delete net;
                        net = NULL;

                        return GET();

                    case GETCONTENTSTATUS_PROXYDIGESTAUTH:

                        if (authTries > 2) {
                            authTries = 0;

                            throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                        }

                        makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password, response);

                        delete ex;
                        ex = NULL;
                        delete net;
                        net = NULL;

                        return GET();
                }
            } catch (...) {
                delete ex;
                delete net;

                clear();

                throw;
            }

            if (certsSet)
                ((ssl::client *)net)->setSertificates(certs);
            else
                ((ssl::client *)net)->initSsl();

            net->socket = ex->socket;
            ((ssl::client *)net)->connectSsl();

            ((ssl::exchange *)ex)->handle->handle = ((ssl::client *)net)->handle->handle;

            net->socket = -1;
            ((ssl::client *)net)->handle->handle = NULL;
        }
#endif
    } else {
        tools::network::__host__ host = tools::network::host(urlComponents.host);

        dodoStringArray::iterator o = host.addresses.begin(), p = host.addresses.end();
        for (; o != p; ++o) {
            try {
                if (scheme == SCHEME_HTTP) {
                    net->connect(*o, tools::string::stringToI(urlComponents.port), *ex);
                    ex->setInBufferSize(512);
                    ex->bs = 512;
                }

#ifdef OPENSSL_EXT
                else {
                    if (certsSet)
                        ((ssl::client *)net)->setSertificates(certs);

                    net->connect(*o, tools::string::stringToI(urlComponents.port), *ex);
                    ex->setInBufferSize(512);
                    ex->bs = 512;
                }
#endif

                break;
            } catch (exception::basic &exp) {
#ifdef OPENSSL_EXT
                if (exp.function == CLIENTEX_CONNECT || exp.function == ssl::CLIENTEX_CONNECT)

#else
                if (exp.function == CLIENTEX_CONNECT)
#endif
                {
                    if ((o + 1) == p) {
                        delete net;
                        delete ex;

                        throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_CANNOTCONNECT, IONETWORKHTTPCLIENTEX_CANNOTCONNECT_STR, __LINE__, __FILE__);
                    } else
                        continue;
                }

                delete net;
                delete ex;

                throw;
            }
        }
    }

    delete net;

    data.clear();

    data.append("GET ");
    data.append(urlBasePath);
    data.append(urlQuery);
    data.append(" HTTP/1.1\r\n");

    if (cacheAuthentification) {
        dodoStringMap::iterator header = httpAuth.find(urlBasePath);
        if (header != httpAuth.end())
            requestHeaders[REQUEST_HEADER_AUTHORIZATION] = header->second;
    }

    dodoMap<short, dodoString>::iterator i(requestHeaders.begin()), j(requestHeaders.end());
    for (; i != j; ++i) {
#ifdef OPENSSL_EXT
        if (proxyAuthInfo.host.size() > 0 && scheme == SCHEME_HTTPS && i->first == REQUEST_HEADER_PROXYAUTHORIZATION)
            continue;

#endif

        data.append(requestHeaderStatements[i->first]);
        data.append(": ");
        data.append(i->second);
        data.append("\r\n");
    }
    data.append("Host: ");
    data.append(urlComponents.host);

    data.append("\r\n\r\n");

    unsigned long bs = ex->bs;

    ex->bs = data.size();
    ex->write(data);

    ex->bs = bs;

    try {
        switch (getContent(ex, response)) {
            case GETCONTENTSTATUS_NORMAL:

                break;

            case GETCONTENTSTATUS_REDIRECT:

                setUrl(response.headers[RESPONSE_HEADER_LOCATION]);

                delete ex;
                ex = NULL;

                return GET();

            case GETCONTENTSTATUS_PROXYBASICAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

                delete ex;
                ex = NULL;

                return GET();

            case GETCONTENTSTATUS_WWWBASICAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeBasicAuth(REQUEST_HEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

                delete ex;
                ex = NULL;

                return GET();

            case GETCONTENTSTATUS_PROXYDIGESTAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password, response);

                delete ex;
                ex = NULL;

                return GET();

            case GETCONTENTSTATUS_WWWDIGESTAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeDigestAuth(RESPONSE_HEADER_WWWAUTHENTICATE, REQUEST_HEADER_AUTHORIZATION, "GET", urlComponents.login, urlComponents.password, response);

                delete ex;
                ex = NULL;

                return GET();

            case GETCONTENTSTATUS_WWWPROXYBASICAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeBasicAuth(REQUEST_HEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

                if (proxyAuthInfo.authType == PROXY_AUTH_BASIC)
                    makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
                else
                    makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password, response);

                delete ex;
                ex = NULL;

                return GET();

            case GETCONTENTSTATUS_WWWPROXYDIGESTAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GET, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeDigestAuth(RESPONSE_HEADER_WWWAUTHENTICATE, REQUEST_HEADER_AUTHORIZATION, "GET", urlComponents.login, urlComponents.password, response);

                if (proxyAuthInfo.authType == PROXY_AUTH_BASIC)
                    makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
                else
                    makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "GET", proxyAuthInfo.user, proxyAuthInfo.password, response);

                delete ex;
                ex = NULL;

                return GET();
        }
    } catch (...) {
        delete ex;

        clear();

        throw;
    }

    delete ex;

    clear();

    return response;
}

//-------------------------------------------------------------------

response
client::GET(const dodoString &a_url) const
{
    setUrl(a_url);

    return GET();
}

//-------------------------------------------------------------------

response
client::POST(const dodoString &a_url,
             const dodoStringMap &arguments,
             const dodoMap<dodoString, file> &files) const
{
    setUrl(a_url);

    return POST(arguments, files);
}

//-------------------------------------------------------------------

response
client::POST(const dodoStringMap &arguments,
             const dodoMap<dodoString, file> &files) const
{
    dodoString boundary = "---------------------------" + tools::string::ulToString(tools::misc::ulRandom()) + tools::string::ulToString(tools::misc::ulRandom());
    dodoString type = "multipart/form-data; boundary=" + boundary;
    boundary.insert(0, "--");

    dodoString data;

    dodoMap<dodoString, file>::const_iterator i = files.begin(), j = files.end();
    for (; i != j; ++i) {
        data.append(boundary);
        data.append("\r\nContent-Disposition: form-data; name=\"");
        data.append(i->first);
        data.append("\"; filename=\"");
        data.append(tools::filesystem::basename(i->second.path));
        data.append("\"\r\n");

        data.append("Content-Type: ");
        data.append(i->second.mime);
        data.append("\r\n\r\n");

        data.append(tools::filesystem::fileContents(i->second.path));
        data.append("\r\n");
    }

    dodoStringMap::const_iterator o = arguments.begin(), p = arguments.end();
    for (; o != p; ++o) {
        data.append(boundary);
        data.append("\r\nContent-Disposition: form-data; name=\"");
        data.append(o->first);
        data.append("\"\r\n\r\n");

        data.append(o->second);
        data.append("\r\n");
    }
    data.append(boundary);
    data.append("--");

    return POST(data, type);
}

//-------------------------------------------------------------------

response
client::POST(const dodoString    &a_url,
             const dodoStringMap &arguments) const
{
    setUrl(a_url);

    return POST(arguments);
}

//-------------------------------------------------------------------

response
client::POST(const dodoStringMap &arguments) const
{
    dodoString data;

    dodoStringMap::const_iterator i(arguments.begin()), j(arguments.end());
    --j;

    for (; i != j; ++i) {
        data.append(tools::code::encodeUrl(i->first));
        data.append("=");
        data.append(tools::code::encodeUrl(i->second));
        data.append("&");
    }
    data.append(tools::code::encodeUrl(i->first));
    data.append("=");
    data.append(tools::code::encodeUrl(i->second));

    return POST(data, "application/x-www-form-urlencoded");
}

//-------------------------------------------------------------------

response
client::POST(const dodoString &a_url,
             const dodoString &data,
             const dodoString &type) const
{
    setUrl(a_url);

    return POST(data, type);
}

//-------------------------------------------------------------------

response
client::POST(const dodoString &rdata,
             const dodoString &type) const
{
    response response;

    exchange *ex = NULL;
    network::client *net = NULL;

    dodoString data;

    if (scheme == SCHEME_HTTP) {
        net = new network::client(connection::PROTOCOL_FAMILY_IPV4, connection::TRANSFER_STREAM);
        ex = new exchange;
    }

#ifdef OPENSSL_EXT
    else {
        net = new ssl::client(connection::PROTOCOL_FAMILY_IPV4, connection::TRANSFER_STREAM);
        ex = new ssl::exchange;
    }
#endif

    if (proxyAuthInfo.host.size() > 0) {
        if (scheme == SCHEME_HTTP)
            net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *ex);

#ifdef OPENSSL_EXT
        else {
            net->connect(proxyAuthInfo.host, proxyAuthInfo.port, *(exchange *)ex);
            data.append("CONNECT ");
            data.append(urlComponents.host);
            data.append(":");
            data.append(urlComponents.port);
            data.append(" HTTP/1.1\r\n");
            if (requestHeaders.find(REQUEST_HEADER_PROXYAUTHORIZATION) != requestHeaders.end()) {
                data.append(requestHeaderStatements[REQUEST_HEADER_PROXYAUTHORIZATION]);
                data.append(": ");
                data.append(requestHeaders[REQUEST_HEADER_PROXYAUTHORIZATION]);
                data.append("\r\n");
            }
            data.append("\r\n");

            unsigned long bs = ex->bs;
            ex->bs = data.size();
            ex->exchange::_write(data.data());
            ex->bs = bs;

            try {
                switch (getProxyConnectResponse(ex, response)) {
                    case GETCONTENTSTATUS_NORMAL:

                        break;

                    case GETCONTENTSTATUS_PROXYBASICAUTH:

                        if (authTries > 2) {
                            authTries = 0;

                            throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                        }

                        makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

                        delete ex;
                        ex = NULL;
                        delete net;
                        net = NULL;

                        return POST(data, type);

                    case GETCONTENTSTATUS_PROXYDIGESTAUTH:

                        if (authTries > 2) {
                            authTries = 0;

                            throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                        }

                        makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password, response);

                        delete ex;
                        ex = NULL;
                        delete net;
                        net = NULL;

                        return POST(data, type);
                }
            } catch (...) {
                delete ex;
                delete net;

                clear();

                throw;
            }

            if (certsSet)
                ((ssl::client *)net)->setSertificates(certs);
            else
                ((ssl::client *)net)->initSsl();

            net->socket = ex->socket;
            ((ssl::client *)net)->connectSsl();

            ((ssl::exchange *)ex)->handle->handle = ((ssl::client *)net)->handle->handle;

            net->socket = -1;
            ((ssl::client *)net)->handle->handle = NULL;
        }
#endif
    } else {
        tools::network::__host__ host = tools::network::host(urlComponents.host);

        dodoStringArray::iterator o = host.addresses.begin(), p = host.addresses.end();
        for (; o != p; ++o) {
            try {
                if (scheme == SCHEME_HTTP) {
                    net->connect(*o, tools::string::stringToI(urlComponents.port), *ex);
                    ex->setInBufferSize(512);
                    ex->bs = 512;
                }

#ifdef OPENSSL_EXT
                else {
                    if (certsSet)
                        ((ssl::client *)net)->setSertificates(certs);

                    net->connect(*o, tools::string::stringToI(urlComponents.port), *ex);
                    ex->setInBufferSize(512);
                    ex->bs = 512;
                }
#endif

                break;
            } catch (exception::basic &exp) {
#ifdef OPENSSL_EXT
                if (exp.function == CLIENTEX_CONNECT || exp.function == ssl::CLIENTEX_CONNECT)
#else
                if (exp.function == CLIENTEX_CONNECT)
#endif
                {
                    if ((o + 1) == p) {
                        delete net;
                        delete ex;

                        throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_CANNOTCONNECT, IONETWORKHTTPCLIENTEX_CANNOTCONNECT_STR, __LINE__, __FILE__);
                    } else
                        continue;
                }

                delete net;
                delete ex;

                throw;
            }
        }
    }

    delete net;

    data.append("POST ");
    data.append(urlBasePath);
    data.append(urlQuery);
    data.append(" HTTP/1.1\r\n");

    if (cacheAuthentification) {
        dodoStringMap::iterator header = httpAuth.find(urlBasePath);
        if (header != httpAuth.end())
            requestHeaders[REQUEST_HEADER_AUTHORIZATION] = header->second;
    }

    dodoMap<short, dodoString>::iterator i(requestHeaders.begin()), j(requestHeaders.end());
    for (; i != j; ++i) {
#ifdef OPENSSL_EXT
        if (proxyAuthInfo.host.size() > 0 && scheme == SCHEME_HTTPS && i->first == REQUEST_HEADER_PROXYAUTHORIZATION)
            continue;

#endif

        data.append(requestHeaderStatements[i->first]);
        data.append(": ");
        data.append(i->second);
        data.append("\r\n");
    }
    data.append("Host: ");
    data.append(urlComponents.host);
    data.append("\r\n");

    data.append("Content-length: ");
    data.append(tools::string::ulToString(rdata.size()));
    data.append("\r\n");

    data.append("Content-type: ");
    data.append(type);
    data.append("\r\n\r\n");

    unsigned long bs = ex->bs;

    ex->bs = data.size();
    ex->write(data);

    ex->bs = rdata.size();
    ex->write(rdata);

    ex->bs = bs;

    try {
        switch (getContent(ex, response)) {
            case GETCONTENTSTATUS_NORMAL:

                break;

            case GETCONTENTSTATUS_REDIRECT:

                setUrl(response.headers[RESPONSE_HEADER_LOCATION]);

                return POST(rdata, type);

            case GETCONTENTSTATUS_PROXYBASICAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);

                return POST(rdata, type);

            case GETCONTENTSTATUS_WWWBASICAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeBasicAuth(REQUEST_HEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

                return POST(rdata, type);

            case GETCONTENTSTATUS_PROXYDIGESTAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password, response);

                return POST(rdata, type);

            case GETCONTENTSTATUS_WWWDIGESTAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeDigestAuth(RESPONSE_HEADER_WWWAUTHENTICATE, REQUEST_HEADER_AUTHORIZATION, "POST", urlComponents.login, urlComponents.password, response);

                return POST(rdata, type);

            case GETCONTENTSTATUS_WWWPROXYBASICAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeBasicAuth(REQUEST_HEADER_AUTHORIZATION, urlComponents.login, urlComponents.password);

                if (proxyAuthInfo.authType == PROXY_AUTH_BASIC)
                    makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
                else
                    makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password, response);

                return POST(rdata, type);

            case GETCONTENTSTATUS_WWWPROXYDIGESTAUTH:

                if (authTries > 2) {
                    authTries = 0;

                    throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_POST, exception::ERRNO_LIBDODO, CLIENTEX_NOTAUTHORIZED, IONETWORKHTTPCLIENTEX_NOTAUTHORIZED_STR, __LINE__, __FILE__);
                }

                makeDigestAuth(RESPONSE_HEADER_WWWAUTHENTICATE, REQUEST_HEADER_AUTHORIZATION, "POST", urlComponents.login, urlComponents.password, response);

                if (proxyAuthInfo.authType == PROXY_AUTH_BASIC)
                    makeBasicAuth(REQUEST_HEADER_PROXYAUTHORIZATION, proxyAuthInfo.user, proxyAuthInfo.password);
                else
                    makeDigestAuth(RESPONSE_HEADER_PROXYAUTHENTICATE, REQUEST_HEADER_PROXYAUTHORIZATION, "POST", proxyAuthInfo.user, proxyAuthInfo.password, response);

                return POST(rdata, type);
        }
    } catch (...) {
        delete ex;

        clear();

        throw;
    }

    delete ex;

    clear();

    return response;
}

//-------------------------------------------------------------------

void
client::setProxy(const dodoString &host,
                 unsigned int     port,
                 const dodoString &user,
                 const dodoString &password) const
{
    proxyAuthInfo.host = host;
    proxyAuthInfo.port = port;
    proxyAuthInfo.user = user;
    proxyAuthInfo.password = password;
    proxyAuthInfo.authType = PROXY_AUTH_NONE;

    requestHeaders.erase(REQUEST_HEADER_PROXYAUTHORIZATION);
}

//-------------------------------------------------------------------

void
client::getHeaders(const dodoString &headers,
                   response         &response) const
{
    unsigned long i(0), j(0);
    unsigned long size = headers.size();

    dodoStringArray arr;
    dodoString piece;

    short o;

    bool statusCode = false;

    while (i < size) {
        i = headers.find("\n", i);
        if (i == dodoString::npos)
            return;

        piece = tools::string::trim(dodoString(headers.data() + j, i - j), '\r');

        arr = tools::misc::split(piece, ":", 2);
        if (arr.size() != 2) {
            if (!statusCode) {
                statusCode = true;

                response.code = getStatusCode(piece);
            }
        } else {
            for (o = 0; o < RESPONSE_HEADER_ENUMSIZE; ++o)
                if (tools::string::equal(responseHeaderStatements[o], arr[0]))
                    response.headers[o] = tools::string::lTrim(arr[1], ' ');

            if (tools::string::equal("Set-Cookie", arr[0]))
                response.cookies.push_back(parseCookie(tools::string::lTrim(arr[1], ' ')));
        }

        ++i;
        j = i;
    }
}

//-------------------------------------------------------------------

unsigned int
client::extractHeaders(const dodoString &data,
                       dodoString       &headers) const
{
    headers.append(data);

    unsigned long i = headers.find("\r\n\r\n");
    if (i == dodoString::npos) {
        i = headers.find("\n\n");
        if (i == dodoString::npos)
            return 0;
        else {
            headers.erase(i + 1);

            return i + 2;
        }
    } else {
        headers.erase(i + 2);

        return i + 4;
    }

    return 0;
}

//-------------------------------------------------------------------

short
client::getProxyConnectResponse(exchange *ex,
                                response &response) const
{
    unsigned long size = 0, i;
    bool endOfHeaders = false;

    dodoString headers;

    char data[512];
    ex->setInBufferSize(512);
    ex->bs = 512;

    while (true) {
        try {
            size = ex->exchange::_readString(data);

            if (size == 0)
                break;

            headers.append(data);

            i = headers.find("\r\n\r\n");
            if (i == dodoString::npos) {
                i = headers.find("\n\n");
                if (i != dodoString::npos) {
                    headers.erase(i + 1);

                    endOfHeaders = true;
                }
            } else {
                headers.erase(i + 2);

                endOfHeaders = true;
            }

            if (endOfHeaders) {
                getHeaders(headers, response);

                if (response.code == 407) {
                    ++authTries;

                    if (tools::string::contains(response.headers[RESPONSE_HEADER_PROXYAUTHENTICATE], "Basic")) {
                        proxyAuthInfo.authType = PROXY_AUTH_BASIC;

                        return GETCONTENTSTATUS_PROXYBASICAUTH;
                    } else {
                        if (tools::string::contains(response.headers[RESPONSE_HEADER_PROXYAUTHENTICATE], "Digest")) {
                            proxyAuthInfo.authType = PROXY_AUTH_DIGEST;

                            return GETCONTENTSTATUS_PROXYDIGESTAUTH;
                        } else
                            throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GETPROXYCONNECTRESPONSE, exception::ERRNO_LIBDODO, CLIENTEX_UNKNOWNPROXYAUTHTYPE, IONETWORKHTTPCLIENTEX_UNKNOWNPROXYAUTH_STR, __LINE__, __FILE__);
                    }
                }

                break;
            }
        } catch (exception::basic &ex) {
            if (ex.function == EXCHANGEEX__READSTRING)
                break;
            else
                throw;
        }
    }

    authTries = 0;

    return GETCONTENTSTATUS_NORMAL;
}

//-------------------------------------------------------------------

short
client::getContent(exchange *ex,
                   response &response) const
{
    dodoString data;

    unsigned long contentSize = 0;

    unsigned long chunkSize = 0;
    dodoString chunkSizeHex;

    unsigned long eoc;

    unsigned int endOfHeaders = 0;
    bool chunked = false;

    dodoString headers;

    while (true) {
        try {
            if (chunked) {
                if (chunkSize > 0) {
                    if (chunkSize > data.size()) {
                        ex->bs = chunkSize - data.size();
                        data.append(ex->read());
                    }

                    response.data.append(data, 0, chunkSize);

                    data.erase(0, chunkSize);
                    chunkSize = 0;
                }

                if (data.size() == 0) {
                    ex->bs = 512;
                    data = ex->readString();

                    if (data.size() == 0)
                        break;
                }

                eoc = data.find("\r\n");
                if (eoc == dodoString::npos) {
                    eoc = data.find('\n');
                    if (eoc != dodoString::npos)
                        ++eoc;
                } else {
                    eoc += 2;
                }

                if (eoc != dodoString::npos) {
                    chunkSizeHex.clear();

                    for (unsigned long i = 0; i < eoc; ++i) {
                        if (data[i] == '\r' || data[i] == ';' || data[i] == '\n')
                            break;

                        chunkSizeHex.append(1, data[i]);
                    }
                    data.erase(0, eoc);

                    if (chunkSizeHex.size() == 0) {
                        chunkSize = 0;

                        continue;
                    }

                    chunkSize = tools::code::hexToLong(chunkSizeHex);
                    if (chunkSize == 0)
                        break;
                } else {
                    ex->bs = 512;
                    data.append(ex->readString());
                }
            } else {
                data = ex->readString();

                if (data.size() == 0 && contentSize <= 0) {
                    if (endOfHeaders == 0 && headers.size() > 0)
                        response.data.assign(headers);

                    break;
                }

                if (endOfHeaders != 0) {
                    response.data.append(data);
                } else {
                    endOfHeaders = extractHeaders(data, headers);

                    if (endOfHeaders > 0) {
                        getHeaders(headers, response);
                        headers.clear();

                        if (followRedirection && (response.code / 100) == 3 && response.code != 304) {
                            response.redirected = true;

                            return GETCONTENTSTATUS_REDIRECT;
                        }

                        if (response.code == 401) {
                            ++authTries;

                            if (proxyAuthInfo.authType != PROXY_AUTH_NONE) {
                                if (tools::string::contains(response.headers[RESPONSE_HEADER_WWWAUTHENTICATE], "Basic")) {
                                    return GETCONTENTSTATUS_WWWPROXYBASICAUTH;
                                } else {
                                    if (tools::string::contains(response.headers[RESPONSE_HEADER_WWWAUTHENTICATE], "Digest"))
                                        return GETCONTENTSTATUS_WWWPROXYDIGESTAUTH;
                                    else
                                        throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GETCONTENT, exception::ERRNO_LIBDODO, CLIENTEX_UNKNOWNWWWAUTHTYPE, IONETWORKHTTPCLIENTEX_UNKNOWNWWWAUTH_STR, __LINE__, __FILE__);
                                }
                            } else {
                                if (tools::string::contains(response.headers[RESPONSE_HEADER_WWWAUTHENTICATE], "Basic")) {
                                    return GETCONTENTSTATUS_WWWBASICAUTH;
                                } else {
                                    if (tools::string::contains(response.headers[RESPONSE_HEADER_WWWAUTHENTICATE], "Digest"))
                                        return GETCONTENTSTATUS_WWWDIGESTAUTH;
                                    else
                                        throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GETCONTENT, exception::ERRNO_LIBDODO, CLIENTEX_UNKNOWNWWWAUTHTYPE, IONETWORKHTTPCLIENTEX_UNKNOWNWWWAUTH_STR, __LINE__, __FILE__);
                                }
                            }
                        }

                        if (response.code == 407) {
                            ++authTries;

                            if (tools::string::contains(response.headers[RESPONSE_HEADER_PROXYAUTHENTICATE], "Basic")) {
                                proxyAuthInfo.authType = PROXY_AUTH_BASIC;

                                return GETCONTENTSTATUS_PROXYBASICAUTH;
                            } else {
                                if (tools::string::contains(response.headers[RESPONSE_HEADER_PROXYAUTHENTICATE], "Digest")) {
                                    proxyAuthInfo.authType = PROXY_AUTH_DIGEST;

                                    return GETCONTENTSTATUS_PROXYDIGESTAUTH;
                                } else {
                                    throw exception::basic(exception::MODULE_IONETWORKHTTP, CLIENTEX_GETCONTENT, exception::ERRNO_LIBDODO, CLIENTEX_UNKNOWNPROXYAUTHTYPE, IONETWORKHTTPCLIENTEX_UNKNOWNPROXYAUTH_STR, __LINE__, __FILE__);
                                }
                            }
                        }

                        chunked = tools::string::equal(response.headers[RESPONSE_HEADER_TRANSFERENCODING], "chunked");

                        if (chunked) {
                            data.erase(0, endOfHeaders);
                        } else {
                            response.data = data.substr(endOfHeaders);

                            contentSize = tools::string::stringToUL(response.headers[RESPONSE_HEADER_CONTENTLENGTH]);

                            ex->bs = 16384;
                        }

                        ex->setInBufferSize(16384);
                    }
                }

                if (contentSize > 0 && response.data.size() == contentSize)
                    break;
            }
        } catch (exception::basic &ex) {
#ifdef OPENSSL_EXT
            if (ex.function == EXCHANGEEX__READSTRING || ex.function == ssl::EXCHANGEEX__READSTRING)
#else
            if (ex.function == EXCHANGEEX__READSTRING)
#endif
            {
                if (!endOfHeaders && headers.size() > 0)
                    response.data.assign(headers);

                break;
            } else {
                throw;
            }
        }
    }

    authTries = 0;

    return GETCONTENTSTATUS_NORMAL;
}

//-------------------------------------------------------------------

void
client::makeDigestAuth(short            requestHeader,
                       short            responseHeader,
                       const dodoString &method,
                       const dodoString &user,
                       const dodoString &password,
                       response         &response) const
{
    dodoString nonce, opaque, realm;

    dodoString &rh = response.headers[requestHeader];
    dodoStringArray parts = tools::misc::split(dodoString(rh.data() + 7, rh.size() - 7), ",");

    dodoString HA1;

    unsigned char HA[16];
    tools::code::__MD5Context__ context;

    dodoStringArray tuple;

    dodoStringArray::iterator i = parts.begin(), j = parts.end();
    for (; i != j; ++i) {
        tuple = tools::misc::split(tools::string::trim(*i, ' '), "=");
        if (tuple.size() != 2)
            continue;

        if (tools::string::iequal(tuple[0], "realm")) {
            realm = tools::string::trim(tuple[1], '"');

            tools::code::MD5Init(&context);
            tools::code::MD5Update(&context, (unsigned char *)user.data(), user.size());
            tools::code::MD5Update(&context, (unsigned char *)":", 1);
            tools::code::MD5Update(&context, (unsigned char *)realm.data(), realm.size());
            tools::code::MD5Update(&context, (unsigned char *)":", 1);
            tools::code::MD5Update(&context, (unsigned char *)password.data(), password.size());
            tools::code::MD5Final(HA, &context);

            HA1 = tools::code::binToHex(dodoString((char *)&HA, 16));
        } else {
            if (tools::string::iequal(tuple[0], "nonce"))
                nonce = tools::string::trim(tuple[1], '"');
            else if (tools::string::iequal(tuple[0], "opaque"))
                opaque = tools::string::trim(tuple[1], '"');
        }
    }

    dodoString cnonce = tools::code::MD5Hex(tools::misc::stringRandom(5));

    dodoString methodForAuth = method + ":";

    dodoString url = urlBasePath;
    url.append(urlQuery);

    tools::code::MD5Init(&context);
    tools::code::MD5Update(&context, (unsigned char *)methodForAuth.data(), methodForAuth.size());
    tools::code::MD5Update(&context, (unsigned char *)url.data(), url.size());
    tools::code::MD5Final(HA, &context);

    dodoString HA2 = tools::code::binToHex(dodoString((char *)&HA, 16));

    tools::code::MD5Init(&context);
    tools::code::MD5Update(&context, (unsigned char *)HA1.data(), HA1.size());
    tools::code::MD5Update(&context, (unsigned char *)":", 1);
    tools::code::MD5Update(&context, (unsigned char *)nonce.data(), nonce.size());
    tools::code::MD5Update(&context, (unsigned char *)":00000001:", 10);
    tools::code::MD5Update(&context, (unsigned char *)cnonce.data(), cnonce.size());
    tools::code::MD5Update(&context, (unsigned char *)":auth:", 6);
    tools::code::MD5Update(&context, (unsigned char *)HA2.data(), HA2.size());
    tools::code::MD5Final(HA, &context);

    {
        dodoString response = tools::code::binToHex(dodoString((char *)&HA, 16));

        requestHeaders[responseHeader] = "Digest username=\"" + user +
                                         "\", realm=\"" + realm +
                                         + "\", nonce=\"" + nonce +
                                         + "\", uri=\"" + url +
                                         + "\", qop=\"auth\", nc=00000001" +
                                         + ", cnonce=\"" + cnonce +
                                         + "\", response=\"" + response +
                                         + "\", opaque=\"" + opaque + "\"";
    }
}

//-------------------------------------------------------------------

void
client::makeBasicAuth(short            responseHeader,
                      const dodoString &user,
                      const dodoString &password) const
{
    requestHeaders[responseHeader] = "Basic " + tools::code::encodeBase64(user + ":" + password);
}

//-------------------------------------------------------------------

void
client::clear() const
{
    requestHeaders.erase(REQUEST_HEADER_COOKIE);

    if (!cacheAuthentification) {
        requestHeaders.erase(REQUEST_HEADER_PROXYAUTHORIZATION);
        proxyAuthInfo.authType = PROXY_AUTH_NONE;
    } else {
        dodoMap<short, dodoString>::iterator header = requestHeaders.find(REQUEST_HEADER_AUTHORIZATION);
        if (header != requestHeaders.end())
            httpAuth[urlBasePath] = header->second;
    }

    requestHeaders.erase(REQUEST_HEADER_AUTHORIZATION);
}

//-------------------------------------------------------------------

void
client::setAuth(const dodoString &user,
                const dodoString &password) const
{
    urlComponents.login = user;
    urlComponents.password = password;
}

//-------------------------------------------------------------------

dodo::cgi::cookie
client::parseCookie(const dodoString &header) const
{
    dodoStringArray parts = tools::misc::split(header, ";");

    dodoStringArray tuple;

    dodoStringArray::iterator i = parts.begin(), j = parts.end();

    tuple = tools::misc::split(*i, "=", 2);
    if (tuple.size() != 2)
        return cgi::cookie();

    cgi::cookie cookie;
    cookie.name = tuple[0];
    cookie.value = tools::code::decodeUrl(tuple[1]);

    ++i;

    for (; i != j; ++i) {
        tuple = tools::misc::split(tools::string::trim(*i, ' '), "=");

        if (tools::string::iequal(tuple[0], "path"))
            cookie.path = tuple[1];
        else {
            if (tools::string::iequal(tuple[0], "expires"))
                cookie.expires = tuple[1];
            else {
                if (tools::string::iequal(tuple[0], "domain"))
                    cookie.domain = tuple[1];
                else if (tools::string::iequal(tuple[0], "secure"))
                    cookie.secure = true;
            }
        }
    }

    return cookie;
}

//-------------------------------------------------------------------