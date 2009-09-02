/***************************************************************************
 *            ioFileFifo.cc
 *
 *  Wed Oct 8 01:44:18 2005
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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include "ioFile.inline"

#include <libdodo/ioFileFifo.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioFileFifoEx.h>
#include <libdodo/types.h>
#include <libdodo/ioChannel.h>
#include <libdodo/pcSyncProtector.h>

using namespace dodo::io::file;

fifo::fifo(short protection) : stream::channel(protection),
							   inBuffer(IOFILEFIFO_INSIZE),
							   outBuffer(IOFILEFIFO_OUTSIZE),
							   handle(new io::__file__),
							   blocked(true)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IOFILEFIFO);
#endif
}

//-------------------------------------------------------------------

fifo::fifo(const dodoString &path,
		   short            mode,
		   short            protection) : stream::channel(protection),
										  inBuffer(IOFILEFIFO_INSIZE),
										  outBuffer(IOFILEFIFO_OUTSIZE),
										  path(path),
										  mode(mode),
										  handle(new io::__file__),
										  blocked(true)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IOFILEFIFO);
#endif

	if (path.size() != 0) {
		bool exists(false);
		struct stat st;

		if (::lstat(path.data(), &st) == -1) {
			if (errno != ENOENT) {
				delete handle;

				throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
		} else
			exists = true;

		if (exists && !S_ISFIFO(st.st_mode)) {
			delete handle;

			throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_LIBDODO, FIFOEX_WRONGFILENAME, IOFILEFIFOEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
		}

		if (!exists) {
			try {
				tools::filesystem::mkfifo(path, DEFAULT_FILE_PERM);
			} catch (...) {
				delete handle;

				throw;
			}
		}

		switch (mode) {
			case OPEN_MODE_WRITE:

				handle->file = fopen(path.data(), "w");

				break;

			case OPEN_MODE_READ_OPENNONBLOCK:
			{
#ifdef O_LARGEFILE
				int fd = ::open(path.data(), O_NONBLOCK | O_RDONLY | O_LARGEFILE);
#else
				int fd = ::open(path.data(), O_NONBLOCK | O_RDONLY);
#endif

				int blockFlag = fcntl(fd, F_GETFL);
				if (blockFlag == -1) {
					delete handle;

					throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				blockFlag &= ~O_NONBLOCK;

				if (fcntl(fd, F_SETFL, blockFlag) == -1) {
					delete handle;

					throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				handle->file = fdopen(fd, "r");

				break;
			}

			case OPEN_MODE_READ:
			default:

				handle->file = fopen(path.data(), "r");
		}
	}

	if (handle->file == NULL) {
		delete handle;

		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
}

//-------------------------------------------------------------------

fifo::fifo(const fifo &fd) : stream::channel(fd.protection),
							 inBuffer(fd.inBuffer),
							 outBuffer(fd.outBuffer),
							 path(fd.path),
							 mode(fd.mode),
							 handle(new __file__),
							 blocked(fd.blocked)
{
#ifndef IO_WO_XEXEC
	collectedData.setExecObject(xexec::OBJECT_IOFILEFIFO);
#endif

	blockSize = fd.blockSize;
	blockSize = fd.blockSize;

	if (fd.handle->file != NULL) {
		int oldDesc, newDesc;

		oldDesc = fileno(fd.handle->file);
		if (oldDesc == -1) {
			delete handle;

			throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		newDesc = dup(oldDesc);
		if (newDesc == -1) {
			delete handle;

			throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		switch (mode) {
			case OPEN_MODE_WRITE:

				handle->file = fdopen(newDesc, "w");

				break;

			case OPEN_MODE_READ:
			default:

				handle->file = fdopen(newDesc, "r");
		}

		if (handle->file == NULL) {
			delete handle;

			throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_FIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}
}

//-------------------------------------------------------------------

fifo::~fifo()
{
	if (handle->file != NULL)
		fclose(handle->file);

	delete handle;
}

//-------------------------------------------------------------------

int
fifo::inDescriptor() const
{
	pc::sync::protector pg(keeper);

	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_INDESCRIPTOR, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);

	return fileno(handle->file);
}

//-------------------------------------------------------------------

int
fifo::outDescriptor() const
{
	pc::sync::protector pg(keeper);

	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_OUTDESCRIPTOR, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);

	return fileno(handle->file);
}

//-------------------------------------------------------------------

void
fifo::clone(const fifo &fd)
{
	pc::sync::protector pg(keeper);

	if (handle->file != NULL) {
		if (fclose(handle->file) != 0)
			throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		handle->file = NULL;
	}

	inBuffer = fd.inBuffer;
	outBuffer = fd.outBuffer;
	blocked = fd.blocked;
	path = fd.path;
	mode = fd.mode;
	blockSize = fd.blockSize;
	blockSize = fd.blockSize;

	if (fd.handle->file != NULL) {
		int oldDesc, newDesc;

		oldDesc = fileno(fd.handle->file);
		if (oldDesc == -1)
			throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		newDesc = dup(oldDesc);
		if (newDesc == -1)
			throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		switch (mode) {
			case OPEN_MODE_WRITE:

				handle->file = fdopen(newDesc, "w");

				break;

			case OPEN_MODE_READ:
			default:

				handle->file = fdopen(newDesc, "r");
		}

		if (handle->file == NULL)
			throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_CLONE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
fifo::close()
{
	pc::sync::protector pg(keeper);

#ifndef IO_WO_XEXEC
	operType = OPERATION_CLOSE;
	performPreExec();
#endif

	if (handle->file != NULL) {
		if (fclose(handle->file) != 0)
			throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		handle->file = NULL;
	}

#ifndef IO_WO_XEXEC
	performPostExec();
#endif
}

//-------------------------------------------------------------------

void
fifo::open(const dodoString &a_path,
		   short            a_mode)
{
	pc::sync::protector pg(keeper);

#ifndef IO_WO_XEXEC
	operType = OPERATION_OPEN;
	performPreExec();
#endif

	path = a_path;
	mode = a_mode;

	if (handle->file != NULL) {
		if (fclose(handle->file) != 0)
			throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		handle->file = NULL;
	}

	if (path.size() == 0)
		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_LIBDODO, FIFOEX_WRONGFILENAME, IOFILEFIFOEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
	else {
		bool exists(false);
		struct stat st;

		if (::lstat(path.data(), &st) == -1) {
			if (errno != ENOENT)
				throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		} else
			exists = true;

		if (exists && !S_ISFIFO(st.st_mode))
			throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_LIBDODO, FIFOEX_WRONGFILENAME, IOFILEFIFOEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);

		if (!exists)
			tools::filesystem::mkfifo(path, DEFAULT_FILE_PERM);

		switch (mode) {
			case OPEN_MODE_WRITE:

				handle->file = fopen(path.data(), "w");

				break;

			case OPEN_MODE_READ_OPENNONBLOCK:
			{
#ifdef O_LARGEFILE
				int fd = ::open(path.data(), O_NONBLOCK | O_RDONLY | O_LARGEFILE);
#else
				int fd = ::open(path.data(), O_NONBLOCK | O_RDONLY);
#endif

				int blockFlag = fcntl(fd, F_GETFL);
				if (blockFlag == -1)
					throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				blockFlag &= ~O_NONBLOCK;

				if (fcntl(fd, F_SETFL, blockFlag) == -1)
					throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				handle->file = fdopen(fd, "r");

				break;
			}

			case OPEN_MODE_READ:
			default:

				handle->file = fopen(path.data(), "r");
		}
	}

	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

#ifndef IO_WO_XEXEC
	performPostExec();
#endif
}

//-------------------------------------------------------------------

bool
fifo::isBlocked()
{
	pc::sync::protector pg(keeper);

	return blocked;
}

//-------------------------------------------------------------------

void
fifo::block(bool flag)
{
	pc::sync::protector pg(keeper);

	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_BLOCK, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__);

	if (blocked == flag)
		return;

	int blockFlag;

	int desc = fileno(handle->file);
	if (desc == -1)
		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	blockFlag = fcntl(desc, F_GETFL);
	if (blockFlag == -1)
		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (flag)
		blockFlag &= ~O_NONBLOCK;
	else
		blockFlag |= O_NONBLOCK;

	if (fcntl(desc, F_SETFL, blockFlag) == -1)
		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	blocked = flag;
}

//-------------------------------------------------------------------

void
fifo::_read(char * const a_data) const
{
	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX__READ, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);

	char *data = a_data;

	unsigned long iter = blockSize / inBuffer;
	unsigned long rest = blockSize % inBuffer;

	unsigned long batch, n;

	for (unsigned long i = 0; i < iter; ++i) {
		batch = inBuffer;
		while (batch > 0) {
			while (true) {
				if ((n = fread(data, 1, batch, handle->file)) == 0) {
					if (feof(handle->file) != 0 || errno == EAGAIN)
						break;

					if (errno == EINTR)
						continue;

					if (ferror(handle->file) != 0)
						throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
				if ((n = fread(data, 1, batch, handle->file)) == 0) {
					if (feof(handle->file) != 0 || errno == EAGAIN)
						break;

					if (errno == EINTR)
						continue;

					if (ferror(handle->file) != 0)
						throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX__READ, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
fifo::_write(const char *const a_data) const
{
	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX__WRITE, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);

	const char *data = a_data;

	unsigned long iter = blockSize / outBuffer;
	unsigned long rest = blockSize % outBuffer;

	unsigned long batch, n;

	for (unsigned long i = 0; i < iter; ++i) {
		batch = outBuffer;
		while (batch > 0) {
			while (true) {
				if ((n = fwrite(data, 1, batch, handle->file)) == 0) {
					if (errno == EINTR)
						continue;

					if (errno == EAGAIN)
						break;

					if (ferror(handle->file) != 0)
						throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
				if ((n = fwrite(data, 1, batch, handle->file)) == 0) {
					if (errno == EINTR)
						continue;

					if (errno == EAGAIN)
						break;

					if (ferror(handle->file) != 0)
						throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX__WRITE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
fifo::flush() const
{
	pc::sync::protector pg(keeper);

	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_FLUSH, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);

	if (fflush(handle->file) != 0)
		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX_FLUSH, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

unsigned long
fifo::_readString(char * const a_data) const
{
	if (handle->file == NULL)
		throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX__READSTREAM, exception::ERRNO_LIBDODO, FIFOEX_NOTOPENED, IOFILEFIFOEX_NOTOPENED_STR, __LINE__, __FILE__, path);

	unsigned long readSize = blockSize + 1;

	memset(a_data, '\0', readSize);

	while (true) {
		if (fgets(a_data, readSize, handle->file) == NULL) {
			if (errno == EINTR)
				continue;

			if (errno == EAGAIN)
				break;

			if (ferror(handle->file) != 0)
				throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX__READSTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	return strlen(a_data);
}

//-------------------------------------------------------------------

void
fifo::_writeString(const char *const a_data) const
{
	unsigned long _blockSize = blockSize;

	try {
		unsigned int bufSize = strlen(a_data);

		if (bufSize < blockSize)
			blockSize = bufSize;

		_write(a_data);

		while (true) {
			if (fputc('\n', handle->file) == EOF) {
				if (errno == EINTR)
					continue;

				if (errno == EAGAIN)
					break;

				if (ferror(handle->file) != 0)
					throw exception::basic(exception::MODULE_IOFILEFIFO, FIFOEX__WRITESTREAM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}

			break;
		}

		blockSize = _blockSize;
	} catch (...) {
		blockSize = _blockSize;

		throw;
	}
}

//-------------------------------------------------------------------
