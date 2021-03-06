/***************************************************************************
 *            cgiFastServer.cc
 *
 *  Sat Aug  5 2006
 *  Copyright  2006  Dmytro Milinevskyy
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

#ifdef FASTCGI_EXT
#include <fcgiapp.h>

#include "cgiFastRequest.inline"

#include <libdodo/cgiFastServer.h>
#include <libdodo/types.h>
#include <libdodo/pcExecutionThread.h>
#include <libdodo/pcExecutionManager.h>
#include <libdodo/cgiFastServerEx.h>
#include <libdodo/cgiFastExchange.h>
#include <libdodo/pcSyncThread.h>
#include <libdodo/pcSyncStack.h>

using namespace dodo::cgi::fast;

dodo::pc::sync::thread server::accept;

//-------------------------------------------------------------------

dodo::pc::sync::thread server::request;

//-------------------------------------------------------------------

void
dummyfastCGIThread(dodo::cgi::exchange &)
{
}

//-------------------------------------------------------------------

dodo::cgi::server::handler server::handler = &dummyfastCGIThread;

//-------------------------------------------------------------------

server::server(server &)
{
}

//-------------------------------------------------------------------

server::server(unsigned long a_limit,
               bool          threading,
               unsigned int  threadsNum) : threading(threading),
                                           threadsNum(threadsNum)
{
    limit = a_limit;

    FCGX_Init();
}

//-------------------------------------------------------------------

server::~server()
{
    FCGX_Finish();
}

//-------------------------------------------------------------------

int
server::thread(void *data)
{
    FCGX_Request req;
    __request__ request = &req;

    FCGX_InitRequest(request.request, 0, 0);

    exchange cfSTD(request);

    unsigned long limit = *(unsigned long *)data;
    unsigned long requests = 0;

    int res = 0;

    while (true) {
        if (limit != 0) {
            pc::sync::stack p(&server::request);

            ++requests;

            if (requests >= limit)
                break;
        }

        accept.acquire();
        res = FCGX_Accept_r(request.request);
        accept.release();

        if (res == -1)
            dodo_throw exception::basic(exception::MODULE_CGIFASTSERVER, SERVEREX_STACKTHREAD, exception::ERRNO_LIBDODO, SERVEREX_ACCEPTFAILED, CGIFASTSERVEREX_ACCEPTFAILED_STR, __LINE__, __FILE__);

        handler(cfSTD);

        FCGX_Finish_r(request.request);
    }

    return 0;
}

//-------------------------------------------------------------------

void
server::serve(cgi::server::handler func)
{
    if (!isFastCgi())
        dodo_throw exception::basic(exception::MODULE_CGIFASTSERVER, SERVEREX_LISTEN, exception::ERRNO_LIBDODO, SERVEREX_ISCGI, CGIFASTSERVEREX_ISCGI_STR, __LINE__, __FILE__);

    handler = func;

    if (threading) {
        pc::execution::manager m;

        for (unsigned int i = 0; i < threadsNum; ++i)
            m.run(m.add(pc::execution::thread(server::thread, &limit, pc::execution::ON_DESTRUCTION_STOP, false)));

        m.wait();
    } else {
        unsigned long requests = 0;

        FCGX_Request req;
        __request__ request = &req;

        FCGX_InitRequest(request.request, 0, 0);

        exchange cfSTD(request);

        while (true) {
            if (limit != 0) {
                ++requests;

                if (requests >= limit)
                    break;
            }

            if (FCGX_Accept_r(request.request) == -1)
                dodo_throw exception::basic(exception::MODULE_CGIFASTSERVER, SERVEREX_LISTEN, exception::ERRNO_LIBDODO, SERVEREX_ACCEPTFAILED, CGIFASTSERVEREX_ACCEPTFAILED_STR, __LINE__, __FILE__);

            handler(cfSTD);

            FCGX_Finish_r(request.request);
        }
    }
}

//-------------------------------------------------------------------

bool
server::isFastCgi()
{
    return !FCGX_IsCGI();
}
#endif

//-------------------------------------------------------------------
