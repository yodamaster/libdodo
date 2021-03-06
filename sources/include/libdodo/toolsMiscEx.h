/***************************************************************************
 *            toolsMiscEx.h
 *
 *  Wed Oct 5 2005
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

#ifndef _TOOLSMISCEX_H_
#define _TOOLSMISCEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    namespace tools {
        /**
         * libdodo defined errors
         */
        enum miscExR {
            MISCEX_WRONGSTRENGTH,
            MISCEX_EMPTYARRAY,
        };

        /**
         * explanations for libdodo defined errors
         */
#define TOOLSMISCEX_WRONGSTRENGTH_STR  "Wrong strength argument"
#define TOOLSMISCEX_EMPTYARRAY_STR     "Array is empty"

        /**
         * IDs of functions where exception might be thrown
         */
        enum miscFunctionsID {
            MISCEX_RANDOM,
            MISCEX_JOIN,
        };
    };
};
#endif
