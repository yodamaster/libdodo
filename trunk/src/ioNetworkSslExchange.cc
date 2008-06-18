/***************************************************************************
 *            ioNetworkSslExchange.cc
 *
 *  Tue Jun 10 17:00:47 2008
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

#include <libdodo/ioNetworkSslExchange.h>

#ifdef OPENSSL_EXT

using namespace dodo::io::network::ssl;

__initialAccept::__initialAccept() : socket(-1),
									 sslHandle(NULL)
{
}

//-------------------------------------------------------------------

__initialAccept::__initialAccept(__initialAccept &init) : socket(init.socket),
														  sslHandle(init.sslHandle)
{
	init.socket = -1;
	init.sslHandle = NULL;
}

//-------------------------------------------------------------------

exchange::exchange(exchange &fse) : network::exchange(fse)
{
	sslHandle = fse.sslHandle;

	fse.sslHandle = NULL;
}

//-------------------------------------------------------------------

exchange::exchange() : sslHandle(NULL)
{
}

//-------------------------------------------------------------------

exchange::exchange(__initialAccept &a_init)
{
	init(a_init);
}

//-------------------------------------------------------------------

exchange::~exchange()
{
	if (sslHandle != NULL)
	{
		if (SSL_shutdown(sslHandle) == 0)
			SSL_shutdown(sslHandle);

		SSL_free(sslHandle);
	}
}

//-------------------------------------------------------------------

void
exchange::init(__initialAccept &a_init)
{
	init(a_init.socket, a_init.sslHandle, a_init.blocked, a_init.blockInherited);

	a_init.socket = -1;
	a_init.sslHandle = NULL;
}

//-------------------------------------------------------------------

void
exchange::_close(int socket,
				 SSL *sslHandle)
{
	int err = SSL_shutdown(sslHandle);
	if (err < 0)
	{
		unsigned long nerr = ERR_get_error();
		throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__CLOSE, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
	}
	if (err == 0)
	{
		err = SSL_shutdown(sslHandle);
		if (err < 0)
		{
			unsigned long nerr = ERR_get_error();
			throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__CLOSE, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
		}
	}
	
	options::_close(socket);
}

//-------------------------------------------------------------------

void
exchange::close()
{
	raceHazardGuard pg(this);

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	operType = EXCHANGE_OPERATION_CLOSE;
	performXExec(preExec);
#endif

	if (!opened)
		return ;

	_close(socket, sslHandle);

	socket = -1;
	sslHandle = NULL;

	opened = false;

#ifndef IONETWORKEXCHANGE_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
exchange::init(int a_socket,
			   SSL *a_sslHandle,
			   bool a_blocked,
			   bool blockInherited)
{
	raceHazardGuard pg(this);

	if (opened)
	{
		_close(socket, sslHandle);

		socket = -1;
		sslHandle = NULL;

		opened = false;
	}

	blocked = a_blocked;
	socket = a_socket;
	sslHandle = a_sslHandle;

	setInBufferSize(inSocketBuffer);
	setOutBufferSize(outSocketBuffer);

	setInTimeout(inTimeout);
	setOutTimeout(outTimeout);

	setLingerOption(lingerOpts, lingerSeconds);

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
exchange::isAlive()
{
	raceHazardGuard pg(this);

	if (network::exchange::isAlive())
		return true;

	sslHandle = NULL;

	return false;
}

//-------------------------------------------------------------------

void
exchange::_write(const char * const data)
{
	unsigned long iter = outSize / outSocketBuffer;
	unsigned long rest = outSize % outSocketBuffer;

	unsigned long sent_received = 0;

	unsigned long batch;
	long n;

	for (unsigned long i = 0; i < iter; ++i)
	{
		batch = 0;
		while (batch < outSocketBuffer)
		{
			while (true)
			{
				if ((n = SSL_write(sslHandle, data + sent_received, outSocketBuffer)) <= 0)
				{
					switch (SSL_get_error(sslHandle, n))
					{
						case SSL_ERROR_WANT_READ:
						case SSL_ERROR_WANT_WRITE:
						case SSL_ERROR_WANT_X509_LOOKUP:

							n = 0;

							break;

						default:
						{
							unsigned long nerr = ERR_get_error();
							throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__WRITE, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
						}

					}
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
				if ((n = SSL_write(sslHandle, data + sent_received, rest)) <= 0)
				{
					switch (SSL_get_error(sslHandle, n))
					{
						case SSL_ERROR_WANT_READ:
						case SSL_ERROR_WANT_WRITE:
						case SSL_ERROR_WANT_X509_LOOKUP:

							n = 0;

							break;

						default:
						{
							unsigned long nerr = ERR_get_error();
							throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__WRITE, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
						}

					}
				}

				break;
			}

			batch += n;
			sent_received += n;
		}
	}
}

//-------------------------------------------------------------------

void
exchange::_read(char * const data)
{
	memset(data, '\0', inSize);

	unsigned long iter = inSize / inSocketBuffer;
	unsigned long rest = inSize % inSocketBuffer;

	unsigned long sent_received = 0;

	unsigned long batch;
	long n;

	for (unsigned long i = 0; i < iter; ++i)
	{
		batch = 0;
		while (batch < inSocketBuffer)
		{
			while (true)
			{
				if ((n = SSL_read(sslHandle, data + sent_received, inSocketBuffer)) <= 0)
				{
					switch (SSL_get_error(sslHandle, n))
					{
						case SSL_ERROR_WANT_READ:
						case SSL_ERROR_WANT_WRITE:
						case SSL_ERROR_WANT_X509_LOOKUP:

							n = 0;

							break;

						default:
						{
							unsigned long nerr = ERR_get_error();
							throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READ, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
						}

					}
				}

				break;
			}

			if (n == 0)
				break;

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
				if ((n = SSL_read(sslHandle, data + sent_received, rest)) <= 0)
				{
					switch (SSL_get_error(sslHandle, n))
					{
						case SSL_ERROR_WANT_READ:
						case SSL_ERROR_WANT_WRITE:
						case SSL_ERROR_WANT_X509_LOOKUP:

							n = 0;

							break;

						default:
						{
							unsigned long nerr = ERR_get_error();
							throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READ, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
						}

					}
				}

				break;
			}

			if (n == 0)
				break;

			batch += n;
			sent_received += n;
		}
	}
}

//-------------------------------------------------------------------

unsigned long
exchange::_readStream(char * const data)
{
	memset(data, '\0', inSize);

	long n = 0;

	while (true)
	{
		if ((n = SSL_read(sslHandle, data, inSize)) <= 0)
		{
			switch (SSL_get_error(sslHandle, n))
			{
				case SSL_ERROR_WANT_READ:
				case SSL_ERROR_WANT_WRITE:
				case SSL_ERROR_WANT_X509_LOOKUP:
				case SSL_ERROR_ZERO_RETURN:

					n = 0;

					break;

				default:
				{
					unsigned long nerr = ERR_get_error();
					throw baseEx(ERRMODULE_IONETWORKSSLEXCHANGE, EXCHANGEEX__READSTREAM, ERR_OPENSSL, nerr, ERR_error_string(nerr, NULL), __LINE__, __FILE__);
				}
			}
		}

		break;
	}

	return (unsigned long)n;
}

//-------------------------------------------------------------------

exchange *
exchange::createCopy()
{
	raceHazardGuard pg(this);

	exchange *copy = new exchange;

	copy->socket = socket;
	copy->opened = opened;
	copy->sslHandle = sslHandle;

	opened = false;
	socket = -1;
	sslHandle = NULL;

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

#endif

