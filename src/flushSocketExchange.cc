/***************************************************************************
 *            flushSocketExchange.cc
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
 
#include <flushSocketExchange.h>

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

flushSocketExchange::flushSocketExchange(flushSocketExchange &fse)
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

flushSocketExchange::flushSocketExchange()
{
}

//-------------------------------------------------------------------

flushSocketExchange::flushSocketExchange(__initialAccept &a_init)
{
	init(a_init.socket,a_init.blockInherited);
}

//-------------------------------------------------------------------

flushSocketExchange::~flushSocketExchange()
{
	if (opened)
	{
		::shutdown(socket,SHUT_RDWR);		
		
		::close(socket);
	}
}

//-------------------------------------------------------------------

void
flushSocketExchange::init(__initialAccept &a_init)
{
	family = a_init.family;
	type = a_init.type;
	
	blocked = a_init.blocked;
	
	init(a_init.socket, a_init.blockInherited);
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::close()
{		
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKETEXCHANGE_OPER_CLOSE;
		performXExec(preExec);
	#endif
	
	if (!opened)
		#ifndef NO_EX
			return ;
		#else
			return true;
		#endif
	
	#ifdef NO_EX
		register bool result = 
	#endif
	
	flushSocketOptions::_close(socket);
	
	socket = -1;
	
	opened = false;
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
	#endif
			
	#ifdef NO_EX
		return result;
	#endif			
}

//-------------------------------------------------------------------

void 
flushSocketExchange::init(int a_socket, 
						bool blockInherited)
{
	close();

	socket = a_socket;

	setInBufferSize(inSocketBuffer);
	setOutBufferSize(outSocketBuffer);
	
	setInTimeout(inTimeout);
	setOutTimeout(outTimeout);
	
	setLingerSockOption(lingerOpts,lingerSeconds);	
	
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
flushSocketExchange::alive()
{
	return opened;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::write(const char * const data)
{
	buffer.assign(data,outSize);
				
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKETEXCHANGE_OPER_SEND;
		performXExec(preExec);
	#endif	
	
	register unsigned long iter = outSize/outSocketBuffer;
	register unsigned long rest = outSize%outSocketBuffer;

	register unsigned long sent_received = 0;
		
	register unsigned long batch, n;	
		
	for (register unsigned long i=0;i<iter;++i)
	{
		batch = 0;
		while (batch < outSocketBuffer)
		{
			while (true)
			{
				n = ::send(socket,buffer.c_str()+sent_received,outSocketBuffer,0);
				if (n == -1)
				{
					if (errno == EINTR)
						continue;
						
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHSOCKETEXCHANGE,FLUSHSOCKETEXCHANGE_SEND,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;	
					#endif
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
				n = ::send(socket,buffer.c_str()+sent_received,rest,0);
				if (n == -1)
				{
					if (errno == EINTR)
						continue;
										
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHSOCKETEXCHANGE,FLUSHSOCKETEXCHANGE_SEND,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;	
					#endif
				}
				
				break;
			}
			
			batch += n;	
			sent_received += n;
		}		
	}
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
	#endif
		
	#ifdef NO_EX
		return true;
	#endif		
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::writeString(const dodoString &data)
{
	return this->write(data.c_str());
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::read(char * const data)
{
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKETEXCHANGE_OPER_RECEIVE;
		performXExec(preExec);
	#endif	
	
	memset(data,'\0',inSize);
				
	register unsigned long iter = inSize/inSocketBuffer;
	register unsigned long rest = inSize%inSocketBuffer;

	register unsigned long sent_received = 0;
		
	register unsigned long batch, n; 
			
	for (register unsigned long i=0;i<iter;++i)
	{
		batch = 0;
		while (batch < inSocketBuffer)
		{
			while (true)
			{
				n = ::recv(socket,data+sent_received,inSocketBuffer,0);
				if (n == -1)
				{
					if (errno == EINTR)
						continue;
						
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHSOCKETEXCHANGE,FLUSHSOCKETEXCHANGE_RECEIVE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;	
					#endif
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
				n = ::recv(socket,data+sent_received,rest,0);
				if (n == -1)
				{
					if (errno == EINTR)
						continue;				
					
					#ifndef NO_EX
					{
						if (errno == EINVAL)
							return ;
						else
							throw baseEx(ERRMODULE_FLUSHSOCKETEXCHANGE,FLUSHSOCKETEXCHANGE_RECEIVE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					}
					#else
						return false;	
					#endif
				}
				
				break;
			}
			
			batch += n;
			sent_received += n;
		}
	}
	
	buffer.assign(data,inSize);
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
	#endif
	
	#ifdef NO_EX
		return true;
	#endif		
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::readString(dodoString &data)
{	
	register char *t_data = new char[inSize+1];

	#ifdef NO_EX
		register bool result = 
	#endif
	
	#ifndef NO_EX
		try
		{
	#endif
	
	this->read(t_data);
			
	#ifndef NO_EX
		}
		catch(...)
		{
			data.assign(t_data,inSize);
			delete [] t_data;
			
			throw baseEx(ERRMODULE_FLUSHSOCKETEXCHANGE,FLUSHSOCKETEXCHANGE_RECEIVESTRING,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		}
	#endif
	
	data.assign(t_data,inSize);
	delete [] t_data;
	
	#ifdef NO_EX	
		return result;
	#endif
}

//-------------------------------------------------------------------

#ifndef FLUSH_SOCKETEXCHANGE_WO_XEXEC

	//-------------------------------------------------------------------

	int 
	flushSocketExchange::addPostExec(inExec func, 
						void *data)
	{
		return _addPostExec(func, (void *)this, XEXECOBJ_FLUSHSOCKETEXCHANGE, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	flushSocketExchange::addPreExec(inExec func, 
						void *data)
	{
		return _addPreExec(func, (void *)this, XEXECOBJ_FLUSHSOCKETEXCHANGE, data);
	}
	
	//-------------------------------------------------------------------
	
	#ifdef DL_EXT
	
		int 
		flushSocketExchange::addPostExec(const dodoString &module, 
							void *data,
							void *toInit)
		{
			return _addPostExec(module, (void *)this, XEXECOBJ_FLUSHSOCKETEXCHANGE, data, toInit);
		}
		
		//-------------------------------------------------------------------
		
		int 
		flushSocketExchange::addPreExec(const dodoString &module, 
							void *data,
							void *toInit)
		{
			return _addPreExec(module, (void *)this, XEXECOBJ_FLUSHSOCKETEXCHANGE, data, toInit);
		}
		
		//-------------------------------------------------------------------
		
		xexecCounts 
		flushSocketExchange::addExec(const dodoString &module, 
							void *data,
							void *toInit)
		{
			return _addExec(module, (void *)this, XEXECOBJ_FLUSHSOCKETEXCHANGE, data, toInit);
		}
	
	#endif

	//-------------------------------------------------------------------

#endif

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::writeStream(const char * const data)
{
	buffer.assign(data);
				
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKETEXCHANGE_OPER_SENDSTREAM;
		performXExec(preExec);
	#endif	
	
	buffer.append(1,'\n');
	
	register unsigned long outSize = buffer.size();
	
	register unsigned long iter = outSize/outSocketBuffer;
	register unsigned long rest = outSize%outSocketBuffer;

	register unsigned long sent_received = 0;
	
	register unsigned long batch, n;
		
	for (register unsigned long i=0;i<iter;++i)
	{
		batch = 0;
		while (batch < outSocketBuffer)
		{
			while (true)
			{
				n = ::send(socket,buffer.c_str()+sent_received,outSocketBuffer,0);
				if (n == -1)
				{
					if (errno == EINTR)
						continue;
						
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHSOCKETEXCHANGE,FLUSHSOCKETEXCHANGE_SENDSTREAM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;	
					#endif
				}
				
				break;
			}
			
			batch += n;	
			sent_received += n;
		}
	}
	
	if (rest>0)
	{
		batch = 0;
		while (batch < rest)
		{
			while (true)
			{
				n = ::send(socket,buffer.c_str()+sent_received,rest,0);
				if (n == -1)
				{
					if (errno == EINTR)
						continue;
										
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHSOCKETEXCHANGE,FLUSHSOCKETEXCHANGE_SENDSTREAM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;	
					#endif
				}
				
				break;
			}
			
			batch += n;	
			sent_received += n;
		}		
	}
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
	#endif
		
	#ifdef NO_EX
		return true;
	#endif		
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::writeStreamString(const dodoString &data)
{
	return this->writeStream(data.c_str());
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::readStream(char * const data)
{
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKETEXCHANGE_OPER_RECEIVESTREAM;
		performXExec(preExec);
	#endif	
	
	memset(data,'\0',inSocketBuffer);
	
	register unsigned long n;
	
	while (true)
	{
		if ((n = ::recv(socket,data,inSocketBuffer,0)) == -1)
		{
			if (errno == EINTR)
				continue;
					
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKETEXCHANGE,FLUSHSOCKETEXCHANGE_RECEIVESTREAM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;	
			#endif
		}
		
		break;	
	}
	
	data[n] = '\0';
		
	buffer.assign(data,n);
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
	#endif
	
	#ifdef NO_EX
		return true;
	#endif		
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::readStreamString(dodoString &data)
{
	register char *t_data = new char[inSocketBuffer+1];

	#ifdef NO_EX
		register bool result =
	#endif

	
	#ifndef NO_EX
		try
		{
	#endif
	
	this->readStream(t_data);
		
	#ifndef NO_EX
		}
		catch(...)
		{
			data.assign(t_data);
			delete [] t_data;
			
			throw baseEx(ERRMODULE_FLUSHSOCKETEXCHANGE,FLUSHSOCKETEXCHANGE_RECEIVESTRING,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		}
	#endif
	  
	data.assign(t_data);
	delete [] t_data;

	#ifdef NO_EX
		return result;
	#endif
}

//-------------------------------------------------------------------

flushSocketExchange *
flushSocketExchange::createCopy()
{
	flushSocketExchange *copy = new flushSocketExchange;
	
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
flushSocketExchange::deleteCopy(flushSocketExchange *copy)
{
	delete copy;
}

//-------------------------------------------------------------------

