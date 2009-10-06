/***************************************************************************
 *            ioNetworkExchange.cc
 *
 *  Thu Sep 20 2005
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

#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <errno.h>
#include <string.h>

#include <libdodo/ioNetworkExchange.h>
#include <libdodo/ioNetworkConnection.h>
#include <libdodo/ioChannel.h>
#include <libdodo/ioNetworkExchangeEx.h>
#include <libdodo/types.h>
#include <libdodo/pcSyncProtector.h>

using namespace dodo::io::network;

exchange::__init__::__init__() : socket(-1)
{
}

//-------------------------------------------------------------------

exchange::__init__::__init__(__init__ &init) : socket(init.socket)
{
	init.socket = -1;
}

//-------------------------------------------------------------------

exchange::__init__::~__init__()
{
}

//-------------------------------------------------------------------

exchange::exchange(exchange &fse) : stream::channel(fse.protection)
{
}

//-------------------------------------------------------------------

exchange::exchange(short protection) : stream::channel(protection)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IONETWORKEXCHANGE);
#endif
}

//-------------------------------------------------------------------

exchange::exchange(__init__ &a_init,
				   short             protection) : stream::channel(protection)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IONETWORKEXCHANGE);
#endif

	init(a_init.socket, a_init.blocked, a_init.blockInherited);

	a_init.socket = -1;
}

//-------------------------------------------------------------------

exchange::~exchange()
{
	if (socket != -1) {
		::shutdown(socket, SHUT_RDWR);

		::close(socket);
	}
}

//-------------------------------------------------------------------

void
exchange::close()
{
	pc::sync::protector pg(keeper);

#ifndef IO_WO_XEXEC
	performPreExec(OPERATION_CLOSE);
#endif

	if (socket != -1) {
		_close(socket);

		socket = -1;
	}

#ifndef IO_WO_XEXEC
	performPostExec(OPERATION_CLOSE);
#endif
}

//-------------------------------------------------------------------

void
exchange::init(int  a_socket,
			   bool a_blocked,
			   bool blockInherited)
{
	pc::sync::protector pg(keeper);

	if (socket != -1) {
		_close(socket);

		socket = -1;
	}

	blocked = a_blocked;
	socket = a_socket;

	setInBufferSize(inSocketBufferSize);
	setOutBufferSize(outSocketBufferSize);

	setInTimeout(inSocketTimeout);
	setOutTimeout(outSocketTimeout);

	setLingerOption(lingerOpts, lingerSeconds);

	if (!blocked) {
		if (blockInherited)
			block(false);
		else
			block(true);
	} else
		block(true);
}

//-------------------------------------------------------------------

bool
exchange::isAlive()
{
	pc::sync::protector pg(keeper);

	if (socket == -1)
		return false;

	pollfd fd;
	fd.fd = socket;
	fd.events = POLLOUT;

	if (poll(&fd, 1, -1) > 0)
		if (isSetFlag(fd.revents, POLLOUT))
			return true;

	_close(socket);

	socket = -1;

	return false;
}

//-------------------------------------------------------------------

void
exchange::_write(const char * const a_data) const
{
	if (socket == -1)
		throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);

	unsigned long iter = blockSize / outSocketBufferSize;
	unsigned long rest = blockSize % outSocketBufferSize;

	const char *data = a_data;

	long batch, n;

	for (unsigned long i = 0; i < iter; ++i) {
		batch = outSocketBufferSize;
		while (batch > 0) {
			while (true) {
				if ((n = ::send(socket, data, batch, 0)) == -1) {
					if (errno == EINTR)
						continue;

					if (errno == EAGAIN)
						break;

					throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			batch -= n;
			data += n;
		}
	}

	if (rest > 0) {
		batch = rest;
		while (batch > 0) {
			while (true) {
				if ((n = ::send(socket, data, batch, 0)) == -1) {
					if (errno == EINTR)
						continue;

					if (errno == EAGAIN)
						break;

					throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			batch -= n;
			data += n;
		}
	}
}

//-------------------------------------------------------------------

void
exchange::_read(char * const a_data) const
{
	if (socket == -1)
		throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);

	unsigned long iter = blockSize / inSocketBufferSize;
	unsigned long rest = blockSize % inSocketBufferSize;

	char *data = a_data;

	long batch, n;

	for (unsigned long i = 0; i < iter; ++i) {
		batch = inSocketBufferSize;
		while (batch > 0) {
			while (true) {
				if ((n = ::recv(socket, data, batch, 0)) == -1) {
					if (errno == EINTR)
						continue;

					if (errno == EAGAIN)
						break;

					throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			if (n == 0)
				break;

			batch -= n;
			data += n;
		}
	}

	if (rest > 0) {
		batch = rest;
		while (batch > 0) {
			while (true) {
				if ((n = ::recv(socket, data, batch, 0)) == -1) {
					if (errno == EINTR)
						continue;

					if (errno == EAGAIN)
						break;

					throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			if (n == 0)
				break;

			batch -= n;
			data += n;
		}
	}
}

//-------------------------------------------------------------------

void
exchange::_writeString(const char * const data) const
{
	unsigned long _blockSize = blockSize;

	try {
		blockSize = strnlen(data, blockSize) + 1;

		_write(data);

		blockSize = _blockSize;
	} catch (...) {
		blockSize = _blockSize;

		throw;
	}
}

//-------------------------------------------------------------------

unsigned long
exchange::_readString(char * const data) const
{
	if (socket == -1)
		throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__READSTREAM, exception::ERRNO_LIBDODO, EXCHANGEEX_NOCONNECTION, IONETWORKEXCHANGEEX_NOCONNECTION_STR, __LINE__, __FILE__);

	long n = 0;

	while (true) {
		if ((n = ::recv(socket, data, blockSize, 0)) == -1) {
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			throw exception::basic(exception::MODULE_IONETWORKEXCHANGE, EXCHANGEEX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	return n;
}

//-------------------------------------------------------------------

void
exchange::flush() const
{
}

//-------------------------------------------------------------------

int
exchange::inDescriptor() const
{
	pc::sync::protector pg(keeper);

	return socket;
}

//-------------------------------------------------------------------

int
exchange::outDescriptor() const
{
	pc::sync::protector pg(keeper);

	return socket;
}

//-------------------------------------------------------------------

