/***************************************************************************
 *            ioPipe.cc
 *
 *  Tue Jul 1 15:31:57 2008
 *  Copyright  2008  Ni@m
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

#include <libdodo/ioPipe.h>

using namespace dodo;

io::pipe::pipe() : inPipeBuffer(IOPIPE_INSIZE),
			   outPipeBuffer(IOPIPE_OUTSIZE),
			   blocked(true),
			   inHandle(NULL),
			   outHandle(NULL)
{
#ifndef IO_WO_XEXEC

	execObject = XEXEC_OBJECT_IOPIPE;
	execObjectData = (void *)&collectedData;

#endif
}

//-------------------------------------------------------------------

io::pipe::pipe(const pipe &fd) : inPipeBuffer(fd.inPipeBuffer),
							 outPipeBuffer(fd.outPipeBuffer),
							 blocked(fd.blocked),
							 inHandle(NULL),
							 outHandle(NULL)
{
#ifndef IO_WO_XEXEC

	execObject = XEXEC_OBJECT_IOPIPE;
	execObjectData = (void *)&collectedData;

#endif

	inSize = fd.inSize;
	outSize = fd.outSize;

	if (fd.opened)
	{
		int oldDesc, newDesc;

		oldDesc = fileno(fd.inHandle);
		if (oldDesc != -1)
		{
			newDesc = dup(oldDesc);
			if (newDesc != -1)
				inHandle = fdopen(newDesc, "r");
		}

		oldDesc = fileno(fd.outHandle);
		if (oldDesc != -1)
		{
			newDesc = dup(oldDesc);
			if (newDesc != -1)
				outHandle = fdopen(newDesc, "w");
		}

		if (inHandle != NULL && outHandle != NULL)
			opened = true;
	}
}

//-------------------------------------------------------------------

io::pipe::~pipe()
{
	if (opened)
	{
		fclose(inHandle);

		fclose(outHandle);
	}
}

//-------------------------------------------------------------------

void
io::pipe::clone(const pipe &fd)
{
	protector pg(this);

	if (opened)
	{
		if (fclose(inHandle) != 0)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (fclose(outHandle) != 0)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		inHandle = NULL;
		outHandle = NULL;

		opened = false;
	}

	inPipeBuffer = fd.inPipeBuffer;
	outPipeBuffer = fd.outPipeBuffer;
	blocked = fd.blocked;
	inSize = fd.inSize;
	outSize = fd.outSize;

	if (fd.opened)
	{
		int oldDesc, newDesc;

		oldDesc = fileno(fd.inHandle);
		if (oldDesc == -1)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		newDesc = dup(oldDesc);
		if (newDesc == -1)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		inHandle = fdopen(newDesc, "r");
		if (inHandle == NULL)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		oldDesc = fileno(fd.outHandle);
		if (oldDesc == -1)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		newDesc = dup(oldDesc);
		if (newDesc == -1)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		outHandle = fdopen(newDesc, "w");
		if (outHandle == NULL)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		opened = true;
	}
}

//-------------------------------------------------------------------

int
io::pipe::getInDescriptor() const
{
	protector pg(this);

	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILE, PIPEEX_GETINDESCRIPTOR, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_PIPENOTOPENED_STR, __LINE__, __FILE__);

	return fileno(inHandle);
}

//-------------------------------------------------------------------

int
io::pipe::getOutDescriptor() const
{
	protector pg(this);

	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILE, PIPEEX_GETOUTDESCRIPTOR, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_PIPENOTOPENED_STR, __LINE__, __FILE__);


	return fileno(outHandle);
}

//-------------------------------------------------------------------

void
io::pipe::close()
{
	protector pg(this);

#ifndef IO_WO_XEXEC
	operType = PIPE_OPERATION_CLOSE;
	performXExec(preExec);
#endif

	if (opened)
	{
		if (fclose(inHandle) != 0)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (fclose(outHandle) != 0)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		inHandle = NULL;
		outHandle = NULL;

		opened = false;
	}

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
io::pipe::open()
{
	protector pg(this);

#ifndef IO_WO_XEXEC
	operType = PIPE_OPERATION_OPEN;
	performXExec(preExec);
#endif

	if (opened)
	{
		if (fclose(inHandle) != 0)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (fclose(outHandle) != 0)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		inHandle = NULL;
		outHandle = NULL;

		opened = false;
	}

	int pipefd[2];

	if (::pipe(pipefd) != 0)
		throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (blocked)
	{
		int blockFlag;

		blockFlag = fcntl(pipefd[0], F_GETFL);
		if (blockFlag == -1)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		blockFlag |= O_NONBLOCK;

		if (fcntl(pipefd[0], F_SETFL, blockFlag) == -1)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		blockFlag = fcntl(pipefd[1], F_GETFL);
		if (blockFlag == -1)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		blockFlag |= O_NONBLOCK;

		if (fcntl(pipefd[1], F_SETFL, blockFlag) == -1)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	inHandle = fdopen(pipefd[0], "r");
	if (inHandle == NULL)
		throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	outHandle = fdopen(pipefd[1], "w");
	if (outHandle == NULL)
		throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	opened = true;

#ifndef IO_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
io::pipe::_read(char * const a_void)
{
	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILE, PIPEEX__READ, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_PIPENOTOPENED_STR, __LINE__, __FILE__);

	char *data = a_void;

	memset(a_void, '\0', inSize);

	unsigned long iter = inSize / inPipeBuffer;
	unsigned long rest = inSize % inPipeBuffer;

	for (unsigned long i = 0; i < iter; ++i)
	{
		while (true)
		{
			if (fread(data, inPipeBuffer, 1, inHandle) == 0)
			{
				if (feof(inHandle) != 0 || errno == EAGAIN)
					break;

				if (errno == EINTR)
					continue;

				if (ferror(inHandle) != 0)
					throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}

		data += inPipeBuffer;
	}

	if (rest > 0)
	{
		while (true)
		{
			if (fread(data, rest, 1, inHandle) == 0)
			{
				if (feof(inHandle) != 0 || errno == EAGAIN)
					break;

				if (errno == EINTR)
					continue;

				if (ferror(inHandle) != 0)
					throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}
	}
}

//-------------------------------------------------------------------

void
io::pipe::_write(const char *const buf)
{
	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILE, PIPEEX__WRITE, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_PIPENOTOPENED_STR, __LINE__, __FILE__);

	char *data = (char *)buf;

	unsigned long iter = outSize / outPipeBuffer;
	unsigned long rest = outSize % outPipeBuffer;

	for (unsigned long i = 0; i < iter; ++i)
	{
		while (true)
		{
			if (fwrite(data, outPipeBuffer, 1, outHandle) == 0)
			{
				if (errno == EINTR)
					continue;

				if (errno == EAGAIN)
					break;

				if (ferror(outHandle) != 0)
					throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}

		data += outPipeBuffer;
	}

	if (rest > 0)
	{
		while (true)
		{
			if (fwrite(data, 1, rest, outHandle) == 0)
			{
				if (errno == EINTR)
					continue;

				if (errno == EAGAIN)
					break;

				if (ferror(outHandle) != 0)
					throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}
	}
}

//-------------------------------------------------------------------

void
io::pipe::flush()
{
	protector pg(this);

	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILE, PIPEEX_FLUSH, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_PIPENOTOPENED_STR, __LINE__, __FILE__);

	if (fflush(outHandle) != 0)
		throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_FLUSH, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

io::network::__peerInfo
io::pipe::peerInfo()
{
	protector pg(this);

	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILE, PIPEEX_PEERINFO, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_PIPENOTOPENED_STR, __LINE__, __FILE__);

	network::__peerInfo info;

	struct sockaddr sa;

	socklen_t len = sizeof(sockaddr_in6);

	int desc = fileno(inHandle);
	if (desc == -1)
		throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_PEERINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (::getpeername(desc, &sa, &len) == 1)
	{
		if (errno != ENOTSOCK)
			throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_PEERINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		else
			return info;
	}

	switch (len)
	{
		case sizeof(sockaddr_in):

		{
			char temp[15];
			sockaddr_in *sa4;
			sa4 = (sockaddr_in *)&sa;
			if (inet_ntop(AF_INET, &(sa4->sin_addr), temp, 15) != NULL)
				info.host.assign(temp);
			info.port = ntohs(sa4->sin_port);

			return info;
		}

		case sizeof(sockaddr_in6):

		{
			char temp[INET6_ADDRSTRLEN];
			sockaddr_in6 *sa6;
			sa6 = (sockaddr_in6 *)&sa6;
			if (inet_ntop(AF_INET6, &(sa6->sin6_addr), temp, INET6_ADDRSTRLEN) != NULL)
				info.host.assign(temp);
			info.port = ntohs(sa6->sin6_port);

			return info;
		}

		default:

			return info;
	}
}

//-------------------------------------------------------------------

bool
io::pipe::isBlocked()
{
	protector pg(this);

	return blocked;
}


//-------------------------------------------------------------------

void
io::pipe::block(bool flag)
{
	protector pg(this);

	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILE, PIPEEX_BLOCK, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_PIPENOTOPENED_STR, __LINE__, __FILE__);

	if (blocked == flag)
		return;

	int blockFlag;
	int desc;

	desc = fileno(inHandle);
	if (desc == -1)
		throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	blockFlag = fcntl(desc, F_GETFL);
	if (blockFlag == -1)
		throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (flag)
		blockFlag &= ~O_NONBLOCK;
	else
		blockFlag |= O_NONBLOCK;

	if (fcntl(desc, F_SETFL, blockFlag) == -1)
		throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	desc = fileno(outHandle);
	if (desc == -1)
		throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	blockFlag = fcntl(desc, F_GETFL);
	if (blockFlag == -1)
		throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (flag)
		blockFlag &= ~O_NONBLOCK;
	else
		blockFlag |= O_NONBLOCK;

	if (fcntl(desc, F_SETFL, blockFlag) == -1)
		throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	blocked = flag;
}

//-------------------------------------------------------------------

unsigned long
io::pipe::_readStream(char * const a_void)
{
	if (!opened)
		throw exception::basic(exception::ERRMODULE_IOFILE, PIPEEX__READSTREAM, exception::ERRNO_LIBDODO, PIPEEX_PIPENOTOPENED, IOPIPEEX_PIPENOTOPENED_STR, __LINE__, __FILE__);

	memset(a_void, '\0', inSize);

	while (true)
	{
		if (fgets(a_void, inSize, inHandle) == NULL)
		{
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			if (ferror(inHandle) != 0)
				throw exception::basic(exception::ERRMODULE_IOPIPE, PIPEEX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	return strlen(a_void);
}

//-------------------------------------------------------------------

void
io::pipe::_writeStream(const char * const data)
{
	_write(data);
}

//-------------------------------------------------------------------
