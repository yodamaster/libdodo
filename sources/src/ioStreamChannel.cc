/***************************************************************************
 *            ioStreamChannel.cc
 *
 *  Sat Jun 13 2009
 *  Copyright  2009  Dmytro Milinevskyy
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

#include <string.h>

#include <libdodo/ioStreamChannel.h>
#include <libdodo/exceptionBasic.h>
#include <libdodo/ioChannel.h>
#include <libdodo/xexec.h>
#include <libdodo/types.h>
#include <libdodo/pcSyncStack.h>

using namespace dodo::io::stream;

channel::channel(short protection) : io::channel(protection)
{
}

//-------------------------------------------------------------------

channel::~channel()
{
}

//-------------------------------------------------------------------

dodo::string
channel::read() const
{
    pc::sync::stack pg(keeper);

    dodo::string data;
    unsigned long n;

#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_READ);

    collectedData.buffer.reserve(bs);
#endif

    data = dodo::string('\0', bs);

    dodo_try {
        n = _read((char *)data.data());
        data.resize(n);
    } dodo_catch (exception::basic *e UNUSED) {
        data.clear();

#ifndef IO_WO_XEXEC
        collectedData.buffer.clear();
#endif

        dodo_rethrow;
    }

#ifndef IO_WO_XEXEC
    collectedData.buffer = data;

    performPostExec(OPERATION_READ);

    data = collectedData.buffer;

    collectedData.buffer.clear();
#else
#endif

    return data;
}

//-------------------------------------------------------------------

dodo::string
channel::readString() const
{
    pc::sync::stack pg(keeper);

    dodo::string data;

#ifndef IO_WO_XEXEC
    performPreExec(OPERATION_READSTRING);
#endif

    data = dodo::string('\0', bs);
    unsigned long n = 0;

    dodo_try {
        n = _readString((char *)data.data());
        data.resize(n);
    } dodo_catch (exception::basic *e UNUSED) {
        data.clear();

        dodo_rethrow;
    }

#ifndef IO_WO_XEXEC
    collectedData.buffer = data;

    performPostExec(OPERATION_READSTRING);

    data = collectedData.buffer;

    collectedData.buffer.clear();
#endif

    return data;
}

//-------------------------------------------------------------------

unsigned long
channel::write(const dodo::string &data) const
{
    pc::sync::stack pg(keeper);

    unsigned long n;

#ifndef IO_WO_XEXEC
    collectedData.buffer = data;

    performPreExec(OPERATION_WRITE);

    dodo_try {
        n = _write(collectedData.buffer.data());
    } dodo_catch (exception::basic *e UNUSED) {
        collectedData.buffer.clear();

        dodo_rethrow;
    }
#else
    n = _write(data.data());
#endif

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_WRITE);

    collectedData.buffer.clear();
#endif

    return n;
}

//-------------------------------------------------------------------

unsigned long
channel::writeString(const dodo::string &data) const
{
    pc::sync::stack pg(keeper);

    unsigned long n;

#ifndef IO_WO_XEXEC
    collectedData.buffer = data;

    performPreExec(OPERATION_WRITESTRING);

    dodo_try {
        n = _writeString(collectedData.buffer.data());
    } dodo_catch (exception::basic *e UNUSED) {
        collectedData.buffer.clear();

        dodo_rethrow;
    }
#else
    n = _writeString(data.data());
#endif

#ifndef IO_WO_XEXEC
    performPostExec(OPERATION_WRITESTRING);

    collectedData.buffer.clear();
#endif

    return n;
}

//-------------------------------------------------------------------

