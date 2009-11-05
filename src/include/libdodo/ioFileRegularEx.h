/***************************************************************************
 *            ioFileRegularEx.h
 *
 *  Mon Feb 21 2005
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

#ifndef _IOFILEREGULAREX_H_
#define _IOFILEREGULAREX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
    namespace io {
        namespace file {
            /**
             * libdodo defined errors
             */
            enum regularExR {
                REGULAREX_WRONGFILENAME,
                REGULAREX_NOTOPENED
            };

            /**
             * explanations for libdodo defined errors
             */
#define IOFILEREGULAREX_WRONGFILENAME_STR                  "Wrong filename"
#define IOFILEREGULAREX_NOTOPENED_STR                       "File not opened"

            /**
             * IDs of functions where exception might be thrown
             */
            enum regularFunctionsID {
                REGULAREX_REGULAR,
                REGULAREX_CLOSE,
                REGULAREX_CLONE,
                REGULAREX_OPEN,
                REGULAREX__READ,
                REGULAREX__WRITE,
                REGULAREX__READSTRING,
                REGULAREX__WRITESTRING,
                REGULAREX_FLUSH,
                REGULAREX_INDESCRIPTOR,
                REGULAREX_OUTDESCRIPTOR,
            };
        };
    };
};
#endif
