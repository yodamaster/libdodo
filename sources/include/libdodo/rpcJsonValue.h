/***************************************************************************
 *            rpcJsonValue.h
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

#ifndef _RPCJSONVALUE_H_
#define _RPCJSONVALUE_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace data {
        namespace format {
            namespace json {
                class node;
            };
        };
    };

    namespace rpc {
        class value;

        namespace json {
            /**
             * @class value
             * @brief defines RPC value in JSON representation
             */
            class value {
              public:

                /**
                 * @return value parsed from JSON node
                 * @param node defines JSON node
                 */
                static rpc::value jsonToValue(const dodo::data::format::json::node &node);

                /**
                 * @return JSON node from value
                 * @param data defines value structure
                 */
                static dodo::data::format::json::node valueToJson(const rpc::value &data);
            };
        };
    };
};
#endif
