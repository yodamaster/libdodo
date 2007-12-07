/***************************************************************************
 *            ioSocketExchange.cc
 *
 *  Thu Sep 20 01:43:24 2005
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

#include <libdodo/ioSocketExchange.h>

using namespace dodo;

__initialAccept::__initialAccept() : socket(-1),
									 family(-1),
									 type(-1)
{
}

//-------------------------------------------------------------------

__initialAccept::__initialAccept(__initialAccept &init) : socket(init.socket),
														  family(init.family),
														  type(init.type)
{
	init.socket = -1;
}

//-------------------------------------------------------------------

ioSocketExchange::ioSocketExchange(ioSocketExchange &fse)
{
	socket = fse.socket;
	opened = fse.opened;

	fse.opened = false;
	fse.socket = -1;

	family = fse.family;
	type = fse.type;

	socketOpts = fse.socketOpts;
	inTimeout = fse.inTimeout;
	outTimeout = fse.outTimeout;
	inSocketBuffer = fse.inSocketBuffer;
	outSocketBuffer = fse.outSocketBuffer;
	lingerOpts = fse.lingerOpts;
	lingerSeconds = fse.lingerSeconds;
	blocked = fse.blocked;
}

//-------------------------------------------------------------------

ioSocketExchange::ioSocketExchange()
{
}

//-------------------------------------------------------------------

ioSocketExchange::ioSocketExchange(__initialAccept &a_init)
{
	init(a_init.socket, a_init.blockInherited);
}

//-------------------------------------------------------------------

ioSocketExchange::~ioSocketExchange()
{
	if (opened)
	{
		::shutdown(socket, SHUT_RDWR);

		::close(socket);
	}
}

//-------------------------------------------------------------------

void
ioSocketExchange::init(__initialAccept &a_init)
{
	family = a_init.family;
	type = a_init.type;

	blocked = a_init.blocked;

	init(a_init.socket, a_init.blockInherited);
}

//-------------------------------------------------------------------

void
ioSocketExchange::close()
{
	#ifndef IO_IOSOCKETOPTIONS_SOCKET_WO_XEXEC
	operType = IOSOCKETEXCHANGE_OPER_CLOSE;
	performXExec(preExec);
	#endif

	if (!opened)
		return ;

	ioSocketOptions::_close(socket);

	socket = -1;

	opened = false;

	#ifndef IO_IOSOCKETOPTIONS_SOCKET_WO_XEXEC
	performXExec(postExec);
	#endif
}

//-------------------------------------------------------------------

void
ioSocketExchange::init(int a_socket,
						  bool blockInherited)
{
	close();

	socket = a_socket;

	setInBufferSize(inSocketBuffer);
	setOutBufferSize(outSocketBuffer);

	setInTimeout(inTimeout);
	setOutTimeout(outTimeout);

	setLingerSockOption(lingerOpts, lingerSeconds);

	if (!blocked)
	{
		if (blockInherited)
			block(false);
		else
			block(true);
	}
	else
		block(true);

	opened = true;
}

//-------------------------------------------------------------------

bool
ioSocketExchange::alive()
{
	return opened;
}

//-------------------------------------------------------------------

void
ioSocketExchange::write(const char * const data)
{
	buffer.assign(data, outSize);

	#ifndef IO_IOSOCKETOPTIONS_SOCKET_WO_XEXEC
	operType = IOSOCKETEXCHANGE_OPER_SEND;
	performXExec(preExec);
	#endif

	unsigned long iter = outSize / outSocketBuffer;
	unsigned long rest = outSize % outSocketBuffer;

	unsigned long sent_received = 0;

	unsigned long batch, n;

	for (unsigned long i = 0; i < iter; ++i)
	{
		batch = 0;
		while (batch < outSocketBuffer)
		{
			while (true)
			{
				n = ::send(socket, buffer.c_str() + sent_received, outSocketBuffer, 0);
				if (n == -1)
				{
					if (errno == EINTR)
						continue;

					throw baseEx(ERRMODULE_IOSOCKETEXCHANGE, IOSOCKETEXCHANGE_SEND, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			batch += n;
			sent_received += n;
		}
	}

	if (rest > 0)
	{
		batch = 0;
		while (batch < rest)
		{
			while (true)
			{
				n = ::send(socket, buffer.c_str() + sent_received, rest, 0);
				if (n == -1)
				{
					if (errno == EINTR)
						continue;

					throw baseEx(ERRMODULE_IOSOCKETEXCHANGE, IOSOCKETEXCHANGE_SEND, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			batch += n;
			sent_received += n;
		}
	}

	#ifndef IO_IOSOCKETOPTIONS_SOCKET_WO_XEXEC
	performXExec(postExec);
	#endif
}

//-------------------------------------------------------------------

void
ioSocketExchange::writeString(const dodoString &data)
{
	this->write(data.c_str());
}

//-------------------------------------------------------------------

void
ioSocketExchange::read(char * const data)
{
	#ifndef IO_IOSOCKETOPTIONS_SOCKET_WO_XEXEC
	operType = IOSOCKETEXCHANGE_OPER_RECEIVE;
	performXExec(preExec);
	#endif

	memset(data, '\0', inSize);

	unsigned long iter = inSize / inSocketBuffer;
	unsigned long rest = inSize % inSocketBuffer;

	unsigned long sent_received = 0;

	unsigned long batch, n;

	for (unsigned long i = 0; i < iter; ++i)
	{
		batch = 0;
		while (batch < inSocketBuffer)
		{
			while (true)
			{
				n = ::recv(socket, data + sent_received, inSocketBuffer, 0);
				if (n == -1)
				{
					if (errno == EINTR)
						continue;

					if (errno == EAGAIN)
						break;

					throw baseEx(ERRMODULE_IOSOCKETEXCHANGE, IOSOCKETEXCHANGE_RECEIVE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			batch += n;
			sent_received += n;
		}
	}

	if (rest > 0)
	{
		batch = 0;
		while (batch < rest)
		{
			while (true)
			{
				n = ::recv(socket, data + sent_received, rest, 0);
				if (n == -1)
				{
					if (errno == EINTR)
						continue;

					if (errno == EAGAIN)
						break;

					throw baseEx(ERRMODULE_IOSOCKETEXCHANGE, IOSOCKETEXCHANGE_RECEIVE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			batch += n;
			sent_received += n;
		}
	}

	buffer.assign(data, inSize);

	#ifndef IO_IOSOCKETOPTIONS_SOCKET_WO_XEXEC
	performXExec(postExec);
	#endif
}

//-------------------------------------------------------------------

void
ioSocketExchange::readString(dodoString &data)
{
	char *t_data = new char[inSize + 1];

	try
	{
		this->read(t_data);
	}
	catch (...)
	{
		data.assign(t_data, inSize);
		delete [] t_data;

		throw baseEx(ERRMODULE_IOSOCKETEXCHANGE, IOSOCKETEXCHANGE_RECEIVESTRING, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	data.assign(t_data, inSize);
	delete [] t_data;
}

//-------------------------------------------------------------------

#ifndef IO_SOCKETEXCHANGE_WO_XEXEC

//-------------------------------------------------------------------

int
ioSocketExchange::addPostExec(inExec func,
								 void   *data)
{
	return _addPostExec(func, (void *)this, XEXECOBJ_IOSOCKETEXCHANGE, data);
}

//-------------------------------------------------------------------

int
ioSocketExchange::addPreExec(inExec func,
								void   *data)
{
	return _addPreExec(func, (void *)this, XEXECOBJ_IOSOCKETEXCHANGE, data);
}

//-------------------------------------------------------------------

	#ifdef DL_EXT

int
ioSocketExchange::addPostExec(const dodoString &module,
								 void             *data,
								 void             *toInit)
{
	return _addPostExec(module, (void *)this, XEXECOBJ_IOSOCKETEXCHANGE, data, toInit);
}

//-------------------------------------------------------------------

int
ioSocketExchange::addPreExec(const dodoString &module,
								void             *data,
								void             *toInit)
{
	return _addPreExec(module, (void *)this, XEXECOBJ_IOSOCKETEXCHANGE, data, toInit);
}

//-------------------------------------------------------------------

xexecCounts
ioSocketExchange::addExec(const dodoString &module,
							 void             *data,
							 void             *toInit)
{
	return _addExec(module, (void *)this, XEXECOBJ_IOSOCKETEXCHANGE, data, toInit);
}

	#endif

//-------------------------------------------------------------------

#endif

//-------------------------------------------------------------------

void
ioSocketExchange::writeStream(const char * const data)
{
	buffer.assign(data);

	#ifndef IO_IOSOCKETOPTIONS_SOCKET_WO_XEXEC
	operType = IOSOCKETEXCHANGE_OPER_SENDSTREAM;
	performXExec(preExec);
	#endif

	buffer.append(1, '\n');

	unsigned long outSize = buffer.size();

	unsigned long iter = outSize / outSocketBuffer;
	unsigned long rest = outSize % outSocketBuffer;

	unsigned long sent_received = 0;

	unsigned long batch, n;

	for (unsigned long i = 0; i < iter; ++i)
	{
		batch = 0;
		while (batch < outSocketBuffer)
		{
			while (true)
			{
				n = ::send(socket, buffer.c_str() + sent_received, outSocketBuffer, 0);
				if (n == -1)
				{
					if (errno == EINTR)
						continue;

					throw baseEx(ERRMODULE_IOSOCKETEXCHANGE, IOSOCKETEXCHANGE_SENDSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			batch += n;
			sent_received += n;
		}
	}

	if (rest > 0)
	{
		batch = 0;
		while (batch < rest)
		{
			while (true)
			{
				n = ::send(socket, buffer.c_str() + sent_received, rest, 0);
				if (n == -1)
				{
					if (errno == EINTR)
						continue;

					throw baseEx(ERRMODULE_IOSOCKETEXCHANGE, IOSOCKETEXCHANGE_SENDSTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				break;
			}

			batch += n;
			sent_received += n;
		}
	}

	#ifndef IO_IOSOCKETOPTIONS_SOCKET_WO_XEXEC
	performXExec(postExec);
	#endif
}

//-------------------------------------------------------------------

void
ioSocketExchange::writeStreamString(const dodoString &data)
{
	this->writeStream(data.c_str());
}

//-------------------------------------------------------------------

void
ioSocketExchange::readStream(char * const data)
{
	#ifndef IO_IOSOCKETOPTIONS_SOCKET_WO_XEXEC
	operType = IOSOCKETEXCHANGE_OPER_RECEIVESTREAM;
	performXExec(preExec);
	#endif

	memset(data, '\0', inSocketBuffer);

	unsigned long n;

	while (true)
	{
		if ((n = ::recv(socket, data, inSocketBuffer, 0)) == -1)
		{
			if (errno == EINTR)
				continue;

			throw baseEx(ERRMODULE_IOSOCKETEXCHANGE, IOSOCKETEXCHANGE_RECEIVESTREAM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	data[n] = '\0';

	buffer.assign(data, n);

	#ifndef IO_IOSOCKETOPTIONS_SOCKET_WO_XEXEC
	performXExec(postExec);
	#endif
}

//-------------------------------------------------------------------

void
ioSocketExchange::readStreamString(dodoString &data)
{
	char *t_data = new char[inSocketBuffer + 1];

	try
	{

		this->readStream(t_data);

	}
	catch (...)
	{
		data.assign(t_data);
		delete [] t_data;

		throw baseEx(ERRMODULE_IOSOCKETEXCHANGE, IOSOCKETEXCHANGE_RECEIVESTRING, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	data.assign(t_data);
	delete [] t_data;
}

//-------------------------------------------------------------------

ioSocketExchange *
ioSocketExchange::createCopy()
{
	ioSocketExchange *copy = new ioSocketExchange;

	copy->socket = socket;
	copy->opened = opened;

	opened = false;
	socket = -1;

	copy->family = family;
	copy->type = type;

	copy->socketOpts = socketOpts;
	copy->inTimeout = inTimeout;
	copy->outTimeout = outTimeout;
	copy->inSocketBuffer = inSocketBuffer;
	copy->outSocketBuffer = outSocketBuffer;
	copy->lingerOpts = lingerOpts;
	copy->lingerSeconds = lingerSeconds;
	copy->blocked = blocked;

	return copy;
}

//-------------------------------------------------------------------

void
ioSocketExchange::deleteCopy(ioSocketExchange *copy)
{
	delete copy;
}

//-------------------------------------------------------------------
