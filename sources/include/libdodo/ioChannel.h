/***************************************************************************
 *            ioChannel.h
 *
 *  Tue Oct 11 2005
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

#ifndef _IOCHANNEL_H_
#define _IOCHANNEL_H_ 1

#include <libdodo/directives.h>

#include <libdodo/xexec.h>
#include <libdodo/types.h>
#include <libdodo/ioEventDescriptor.h>

namespace dodo {
    namespace pc {
        namespace sync {
            class protector;
        };
    };

    namespace io {
        namespace block {
            class channel;
        };

        namespace stream {
            class channel;
        };

        /**
         * @class channel
         * @brief implements an interface for I/O operations
         */
        class channel : virtual public event::descriptor
#ifndef IO_WO_XEXEC
                        ,
                        public xexec
#endif
        {
            friend class block::channel;
            friend class stream::channel;

          private:

            /**
             * constructor
             * @param protection defines type of IO protection, @see channel::io::channel::protectionEnum
             */
            channel(short protection);

            /**
             * destructor
             */
            virtual ~channel();

          public:

            /**
             * @enum protectionEnum defines type of protection for io objects
             * in IO interaction in parallel environment
             */
            enum protectionEnum {
                PROTECTION_NONE,
                PROTECTION_THREAD,
                PROTECTION_PROCESS
            };

            /**
             * @enum operationEnum describes type of operation for xexec
             */
            enum operationEnum {
                OPERATION_READ,
                OPERATION_READSTRING,
                OPERATION_WRITE,
                OPERATION_WRITESTRING,
            };

#ifndef IO_WO_XEXEC
            /**
             * @class __collected_data__
             * @brief defines data that could be retrieved from io::channel(to modificate)
             */
            class __collected_data__ : public xexec::__collected_data__ {
              public:

                /**
                 * constructor
                 * @param executor defines class that executed hook
                 * @param execObject defines type of object that executed a hook, @see xexec::objectEnum
                 */
                __collected_data__(xexec *executor,
                                   short execObject);

                dodo::string buffer; ///< data buffer
            };
#endif

            /**
             * @return read data
             * @note not more than io::channel::bs
             */
            virtual dodo::string read() const = 0;

            /**
             * @return amount in bytes of successfully written data
             * @param data defines data that will be written
             * @note not more than io::channel::bs
             */
            virtual unsigned long write(const dodo::string &data) const = 0;

            /**
             * read null- or newline- terminated string
             * @return read data
             * @note not more than io::channel::bs
             */
            virtual dodo::string readString() const = 0;

            /**
             * write null- terminated string
             * @return amount in bytes of successfully written data
             * @param data defines data that will be written
             * @note not more than io::channel::bs
             */
            virtual unsigned long writeString(const dodo::string &data) const = 0;

            /**
             * flush output
             */
            virtual void flush() const = 0;

            mutable unsigned long bs;   ///< size of data block for read/write operations

          protected:

            /**
             * @return amount in bytes of successfully read data
             * @param data defines buffer that will be filled
             * @note not more than io::channel::bs(including null)
             */
            virtual unsigned long _read(char * const data) const = 0;

            /**
             * read null- or newline- terminated string
             * @return amount in bytes of successfully read data
             * @param data defines buffer that will be filled
             * @note not more than io::channel::bs(including null)
             */
            virtual unsigned long _readString(char * const data) const = 0;

            /**
             * @return amount in bytes of successfully written data
             * @param data defines data that will be written
             * @note not more than io::channel::bs(including null)
             */
            virtual unsigned long _write(const char * const data) const = 0;

            /**
             * write null- terminated string
             * @return amount in bytes of successfully written data
             * @param data defines data that will be written
             * @note not more than io::channel::bs(including null)
             */
            virtual unsigned long _writeString(const char * const data) const = 0;

            pc::sync::protector *keeper;                                    ///< section locker
            short protection;                                               ///< type of IO protection, @see io::channel::protectionEnum

#ifndef IO_WO_XEXEC
            mutable __collected_data__ collectedData;                       ///< data collected for xexec
#endif
        };
    };
};
#endif
