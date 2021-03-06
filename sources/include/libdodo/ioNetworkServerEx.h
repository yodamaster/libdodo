/***************************************************************************
 *            ioNetworkServerEx.h
 *
 *  Mon Feb 21 2005
 *  Copyright  2005  Dmytro Milinevskyy
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

#ifndef _IONETWORKSERVEREX_H_
#define _IONETWORKSERVEREX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    namespace io {
        namespace network {
            /**
             * libdodo defined errors
             */
            enum serverExR {
                SERVEREX_WRONGPARAMETER,
                SERVEREX_WRONGFILENAME,
                SERVEREX_LONGPATH,
            };

            /**
             * explanations for libdodo defined errors
             */
#define IONETWORKSERVEREX_WRONGPARAMETER_STR "Wrong paramather passed to function"
#define IONETWORKSERVEREX_WRONGFILENAME_STR  "Wrong filename"
#define IONETWORKSERVEREX_LONGPATH_STR "Path to socket is too long"

            /**
             * IDs of functions where exception might be thrown
             */
            enum serverFunctionsID {
                SERVEREX_MAKESOCKET,
                SERVEREX_BINDNLISTEN,
                SERVEREX_ACCEPT,
            };
        };
    };
};
#endif
