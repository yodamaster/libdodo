/***************************************************************************
 *            cgiFastExchange.cc
 *
 *  Sat Aug  5 03:37:19 2006
 *  Copyright  2006  Ni@m
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

#include <libdodo/cgiFast.h>

#ifdef FCGI_EXT

using namespace dodo;

cgiFastExchange::cgiFastExchange(cgiFastExchange &cf)
{
}

//-------------------------------------------------------------------

cgiFastExchange::cgiFastExchange(FCGX_Request *a_request) : request(a_request)
{
}

//-------------------------------------------------------------------

cgiFastExchange::~cgiFastExchange()
{
}

//-------------------------------------------------------------------

void
cgiFastExchange::flush()
{
	if (FCGX_FFlush(request->out) == -1)
		throw baseEx(ERRMODULE_CGIFAST, IOCGIFASTEXCHANGEEX_FLUSH, ERR_LIBDODO, IOCGIFASTEXCHANGEEX_FAILEDTOFLUSH, IOCGIFASTEXCHANGEEX_FAILEDTOFLUSH_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

char *
cgiFastExchange::getenv(const char *buf)
{
	return FCGX_GetParam(buf, request->envp);
}

//-------------------------------------------------------------------

int
cgiFastExchange::getInDescriptor() const
{
	return -1;
}

//-------------------------------------------------------------------

int
cgiFastExchange::getOutDescriptor() const
{
	return -1;
}

//-------------------------------------------------------------------

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC

int
cgiFastExchange::addPostExec(inExec func,
				   void   *data)
{
	return _addPostExec(func, (void *)&collectedData, XEXEC_OBJECT_IOCGIFASTEXCHANGE, data);
}

//-------------------------------------------------------------------

int
cgiFastExchange::addPreExec(inExec func,
				  void   *data)
{
	return _addPreExec(func, (void *)&collectedData, XEXEC_OBJECT_IOCGIFASTEXCHANGE, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

int
cgiFastExchange::addPostExec(const dodoString &module,
				   void             *data,
				   void             *toInit)
{
	return _addPostExec(module, (void *)&collectedData, XEXEC_OBJECT_IOCGIFASTEXCHANGE, data, toInit);
}

//-------------------------------------------------------------------

__xexecCounts
cgiFastExchange::addExec(const dodoString &module,
			   void             *data,
			   void             *toInit)
{
	return _addExec(module, (void *)&collectedData, XEXEC_OBJECT_IOCGIFASTEXCHANGE, data, toInit);
}

//-------------------------------------------------------------------

int
cgiFastExchange::addPreExec(const dodoString &module,
				  void             *data,
				  void             *toInit)
{
	return _addPreExec(module, (void *)&collectedData, XEXEC_OBJECT_IOCGIFASTEXCHANGE, data, toInit);
}

#endif

#endif

//-------------------------------------------------------------------

void
cgiFastExchange::_read(char * const a_void)
{
	memset(a_void, '\0', inSize);

	FCGX_GetStr(a_void, inSize, request->in);
}

//-------------------------------------------------------------------

void
cgiFastExchange::read(char * const a_void)
{
	systemRaceHazardGuard pg(this);

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	operType = IOCGIFASTEXCHANGE_OPERATION_READ;
	performXExec(preExec);

	buffer.reserve(inSize);
#endif

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	try
	{
		_read(a_void);
	}
	catch (...)
	{
		buffer.clear();

		throw;
	}
#else
	_read(a_void);
#endif

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	buffer.assign(a_void, inSize);

	performXExec(postExec);

	strncpy(a_void, buffer.c_str(), buffer.size() > inSize ? inSize : buffer.size());
	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
cgiFastExchange::readString(dodoString &a_str)
{
	systemRaceHazardGuard pg(this);

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	operType = IOCGIFASTEXCHANGE_OPERATION_READSTRING;
	performXExec(preExec);

	buffer.reserve(inSize);
#endif

	char *data = new char[inSize];

	try
	{
		_read(data);
	}
	catch (...)
	{
		delete [] data;

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
		buffer.clear();
#endif

		throw;
	}

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	buffer.assign(data, inSize);
	delete [] data;

	performXExec(postExec);

	a_str = buffer;
	buffer.clear();
#else
	a_str.assign(data, inSize);
	delete [] data;
#endif
}

//-------------------------------------------------------------------

void
cgiFastExchange::writeString(const dodoString &a_buf)
{
	systemRaceHazardGuard pg(this);

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	buffer = a_buf;

	operType = IOCGIFASTEXCHANGE_OPERATION_WRITESTRING;
	performXExec(preExec);

	try
	{
		_write(buffer.c_str());
	}
	catch (...)
	{
		buffer.clear();

		throw;
	}
#else
	_write(a_buf.c_str());
#endif


#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
cgiFastExchange::write(const char *const a_buf)
{
	systemRaceHazardGuard pg(this);

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	buffer.assign(a_buf, outSize);

	operType = IOCGIFASTEXCHANGE_OPERATION_WRITE;
	performXExec(preExec);

	try
	{
		_write(buffer.c_str());
	}
	catch (...)
	{
		buffer.clear();

		throw;
	}
#else
	_write(a_buf);
#endif


#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
cgiFastExchange::_write(const char *const buf)
{
	if (FCGX_PutStr(buf, outSize, request->out) == -1)
		throw baseEx(ERRMODULE_CGIFAST, IOCGIFASTEXCHANGEEX__WRITE, ERR_LIBDODO, IOCGIFASTEXCHANGEEX_FAILEDTOPRINTSTRING, IOCGIFASTEXCHANGEEX_FAILEDTOPRINTSTRING_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
cgiFastExchange::readStream(char * const a_void)
{
	systemRaceHazardGuard pg(this);

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	operType = IOCGIFASTEXCHANGE_OPERATION_READSTREAM;
	performXExec(preExec);
#endif

	_read(a_void);

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	buffer = a_void;

	performXExec(postExec);

	if (buffer.size() > inSize)
		buffer.resize(inSize);
	strcpy(a_void, buffer.c_str());
	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
cgiFastExchange::readStreamString(dodoString &a_str)
{
	systemRaceHazardGuard pg(this);

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	operType = IOCGIFASTEXCHANGE_OPERATION_READSTREAMSTRING;
	performXExec(preExec);
#endif

	char *data = new char[inSize];

	try
	{
		_read(data);
	}
	catch (...)
	{
		delete [] data;

		throw;
	}

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	buffer = data;
	delete [] data;

	performXExec(postExec);

	a_str = buffer;
	buffer.clear();
#else
	a_str = data;
	delete [] data;
#endif
}

//-------------------------------------------------------------------

void
cgiFastExchange::writeStreamString(const dodoString &a_buf)
{
	systemRaceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	buffer = a_buf;

	operType = IOCGIFASTEXCHANGE_OPERATION_WRITESTREAMSTRING;
	performXExec(preExec);

	try
	{
		outSize = buffer.size();

		_write(buffer.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		buffer.clear();

		throw;
	}
#else
	try
	{
		outSize = a_buf.size();

		_write(a_buf.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		throw;
	}
#endif

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

//-------------------------------------------------------------------

void
cgiFastExchange::writeStream(const char *const a_buf)
{
	systemRaceHazardGuard pg(this);

	unsigned long _outSize = outSize;

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	buffer = a_buf;

	operType = IOCGIFASTEXCHANGE_OPERATION_WRITESTREAM;
	performXExec(preExec);

	try
	{
		outSize = buffer.size();

		_write(buffer.c_str());

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		buffer.clear();

		throw;
	}
#else
	try
	{
		outSize = strlen(a_buf);

		_write(a_buf);

		outSize = _outSize;
	}
	catch (...)
	{
		outSize = _outSize;

		throw;
	}
#endif

#ifndef IOCGIFASTEXCHANGE_WO_XEXEC
	performXExec(postExec);

	buffer.clear();
#endif
}

#endif

//-------------------------------------------------------------------