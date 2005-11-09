/***************************************************************************
 *            flushSocket.cc
 *
 *  Thu Sep 20 01:43:24 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include "flushSocket.h"

using namespace dodo;

//-------------------------------------------------------------------

flushSocket::flushSocket(unsigned long a_numberOfConn, 
						socketProtoFamilyEnum a_family, 
						socketTransferTypeEnum a_type, 
						unsigned int a_protocol) : numberOfConn(a_numberOfConn),
						family(a_family),
						type(a_type),
						protocol(a_protocol),
						socket(-1)
{
	aliveConnections = new bool [numberOfConn];
	#ifndef NO_EX
		if (aliveConnections == NULL)
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_LIBDODO,FLUSHSOCKET_MEMORY_OVER,FLUSHSOCKET_MEMORY_OVER_STR,__LINE__,__FILE__);
	#endif
	for (register int i=0;i<numberOfConn;++i)
		aliveConnections[i] = false;
}

//-------------------------------------------------------------------

flushSocket::flushSocket(socketProtoFamilyEnum a_family, 
						socketTransferTypeEnum a_type, 
						unsigned int a_protocol) : numberOfConn(-1),
						family(a_family),
						type(a_type),
						protocol(a_protocol),
						socket(-1)
						
{
	aliveConnections = new bool [0];
}

//-------------------------------------------------------------------

flushSocket::~flushSocket()
{
	close();
	delete [] aliveConnections;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::makeSocket(socketProtoFamilyEnum domain, 
						socketTransferTypeEnum type,
						unsigned int protocol)
{
	int real_domain, real_type;
	
	switch (domain)
	{
		case PROTO_FAMILY_IPV4:
			real_domain = PF_INET;
			break;
		case PROTO_FAMILY_IPV6:
			real_domain = PF_INET6;
			break;
	#ifndef WIN
		case PROTO_FAMILY_UNIX_SOCKET:
			real_domain = PF_UNIX;
			break;
	#endif
	}
	switch (type)
	{
		case TRANSFER_TYPE_STREAM:
			real_type = SOCK_STREAM;
			break;
		case TRANSFER_TYPE_DATAGRAM:
			real_type = SOCK_DGRAM;
			break;
		case TRANSFER_TYPE_RAW:
			real_type = SOCK_RAW;
			break;
	}	
	
	socket = ::socket(real_domain,real_type,protocol);
	if (socket == -1)
	#ifndef NO_EX
		throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_MAKESOCKET,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
	#else
		return false;			
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
flushSocket::connect(const std::string &host, 
					unsigned int port, 
					flushSocketExchange *exchange, 
					bool immortal)
{
	if (opened)
		#ifndef NO_EX
			return ;
		#else
			return false;
		#endif
			
	if (numberOfConn != -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_LIBDODO,FLUSHSOCKET_CANNOT_CONNECT,FLUSHSOCKET_CANNOT_CONNECT_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
		
	makeSocket(family,type,protocol);	
			
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif
	
	struct sockaddr_in sa;
	
	if (family == PROTO_FAMILY_IPV6)
		sa.sin_family = AF_INET6;
	else
		sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	inet_aton(host.c_str(),&sa.sin_addr);
	
	if (::connect(socket,(struct sockaddr *)&sa,sizeof(sa))==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else			
			return false;		
		#endif
	
	exchange = new flushSocketExchange(this);
	
	opened = !immortal;
		
	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

#ifndef WIN

	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	flushSocket::makeUnixSocket(const std::string &path)
	{
		struct stat st;
		if (::lstat(path.c_str(),&st) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_MAKEUNIXSOCKET,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
			#else
				return false;		
			#endif
		if (errno != ENOENT)
		{
			if (!S_ISSOCK(st.st_mode))
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_MAKEUNIXSOCKET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
		}
		else
			if (mknod(path.c_str(),flushDisk::getPermission(UNIX_SOCKET_PERM),S_IFSOCK)==-1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_MAKEUNIXSOCKET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
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
	flushSocket::connect(const std::string &path, 
					flushSocketExchange *exchange, 
					bool immortal)
	{
		if (opened)
			#ifndef NO_EX
				return ;
			#else
				return false;
			#endif
			
		if (numberOfConn != -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_LIBDODO,FLUSHSOCKET_CANNOT_CONNECT,FLUSHSOCKET_CANNOT_CONNECT_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		makeSocket(family,type,protocol);	
		
		if (socket == -1)
			#ifdef NO_EX
				return false;
			#else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
			#endif
	
		#ifdef NO_EX
			if (!makeUnixSocket(path))
				return false;
		#else
			makeUnixSocket(path);
		#endif
		
		struct sockaddr_un sa;
		
		strcpy(sa.sun_path,path.c_str());
		sa.sun_family = AF_UNIX;
	
		if (::connect(socket,(struct sockaddr *)&sa,path.size()+sizeof(sa.sun_family))==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_CONNECT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else			
				return false;		
			#endif	
		
		exchange = new flushSocketExchange(this);
		
		opened = !immortal;
		
		#ifdef NO_EX
			return true;
		#endif	
	}

#endif

//-------------------------------------------------------------------

//-------------------------------------------------------------------

__hostInfo 
flushSocket::getHostInfo(const std::string &host)
{
	hostent *ent = gethostbyname(host.c_str());
	
	__hostInfo info;
	
	if (ent == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_GETHOSTINFO,ERR_H_ERRNO,h_errno,hstrerror(h_errno),__LINE__,__FILE__);
		#else
			return info;		
		#endif		
	
	info.name = ent->h_name;

	register int i(0);

	while (ent->h_aliases[i] != NULL)	
		info.aliases.push_back(ent->h_aliases[i++]);
	
	i = 0;	
	while (ent->h_addr_list[i] != NULL)	
		info.addresses.push_back(inet_ntoa(*( (struct in_addr *)ent->h_addr_list[i++]) ));
	
	return info;	
}

//-------------------------------------------------------------------

__servInfo 
flushSocket::getServiceInfo(const std::string &host, 
						const std::string &protocol)
{
	servent *ent = getservbyname(host.c_str(), protocol.c_str());
	
	__servInfo info;
	
	if (ent == NULL)
		return info;
	
	info.name = ent->s_name;
	info.port = ent->s_port;
		
		register int i(0);

	while (ent->s_aliases[i] != NULL)	
		info.aliases.push_back(ent->s_aliases[i++]);
		
	return info;	
}

//-------------------------------------------------------------------

__servInfo 
flushSocket::getServiceInfo(int port, 
						const std::string &protocol)
{
	servent *ent = getservbyport(port, protocol.c_str());
	
	__servInfo info;
	
	if (ent == NULL)
		return info;
	
	info.name = ent->s_name;
	info.port = ent->s_port;
		
		register int i(0);

	while (ent->s_aliases[i] != NULL)	
		info.aliases.push_back(ent->s_aliases[i++]);
		
	return info;	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::bindNListen(const std::string &host, unsigned int port)
{
	if (opened)
		#ifndef NO_EX
			return ;
		#else
			return false;
		#endif
			
	if (numberOfConn == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_LIBDODO,FLUSHSOCKET_CANNOT_BIND,FLUSHSOCKET_CANNOT_BIND_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
		
	makeSocket(family,type,protocol);	
	
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif
	
	struct sockaddr_in sa;
	
	if (family == PROTO_FAMILY_IPV6)
		sa.sin_family = AF_INET6;
	else
		sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	if (strcmp(host.c_str(),"*") == 0)
		sa.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		inet_aton(host.c_str(),&sa.sin_addr);
	
	if (::bind(socket,(struct sockaddr *)&sa,sizeof(sa))==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	if (::listen(socket,numberOfConn)==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif	
	
	opened = true;
		
	#ifdef NO_EX
		return true;
	#endif	

}

//-------------------------------------------------------------------

#ifndef WIN
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	flushSocket::bindNListen(const std::string &path)
	{
		if (opened)
			#ifndef NO_EX
				return ;
			#else
				return false;
			#endif
		
		if (numberOfConn == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_LIBDODO,FLUSHSOCKET_CANNOT_BIND,FLUSHSOCKET_CANNOT_BIND_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		makeSocket(family,type,protocol);	
		
		if (socket == -1)
			#ifdef NO_EX
				return false;
			#else
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
			#endif

			#ifdef NO_EX
				if (!makeUnixSocket(path))
					return false;
			#else
				makeUnixSocket(path);
			#endif
			
			struct sockaddr_un sa;
			
			strcpy(sa.sun_path,path.c_str());
			sa.sun_family = AF_UNIX;	
			
		if (::bind(socket,(struct sockaddr *)&sa,path.size()+sizeof(sa.sun_family))==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
	
		if (::listen(socket,numberOfConn)==-1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_BINDNLISTEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif	
		
		opened = true;
			
		#ifdef NO_EX
			return true;
		#endif		
	}
	
#endif	

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocket::_close(int socket)
{
	
}

//-------------------------------------------------------------------

void 
flushSocket::close()
{
	if (numberOfConn == -1)
	{
		if (!opened)
			_close(socket);
	}
	else
	{
		for (register int i=0;i<numberOfConn;++i)
			if (aliveConnections[i])
			{
				delete accepted;
				aliveConnections[i] = false;
			}
		if (!opened)	
			_close(socket);
	}
	opened = false;
}

//-------------------------------------------------------------------

flushSocketExchange::flushSocketExchange(flushSocket *connector): inTimeout(RECIEVE_TIMEOUT),
																outTimeout(SEND_TIMEOUT),
																inSocketBuffer(SOCKET_INSIZE),
																outSocketBuffer(SOCKET_OUTSIZE)
{
	setInBufferSize(SOCKET_INSIZE);
	setOutBufferSize(SOCKET_OUTSIZE);
	setInTimeout(RECIEVE_TIMEOUT);
	setOutTimeout(SEND_TIMEOUT);
}

//-------------------------------------------------------------------

flushSocketExchange::~flushSocketExchange()
{
	close();
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::setInBufferSize(int bytes)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETINBUFFERSIZE,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif
	
	inSocketBuffer = bytes;
	
	if (setsockopt(socket,SOL_SOCKET,SO_RCVBUF,&inSocketBuffer,size_of_long)==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETINBUFFERSIZE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif			
}

//-------------------------------------------------------------------

int
flushSocketExchange::getInBufferSize()
{
	return inSocketBuffer;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::setOutBufferSize(int bytes)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETOUTBUFFERSIZE,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif
	
	outSocketBuffer = bytes;
	
	if (setsockopt(socket,SOL_SOCKET,SO_SNDBUF,&outSocketBuffer,size_of_long)==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETOUTBUFFERSIZE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif				
}

//-------------------------------------------------------------------

int
flushSocketExchange::getOutBufferSize()
{
	return outSocketBuffer;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushSocketExchange::setInTimeout(unsigned long microseconds)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETINTIMEOUT,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif

	inTimeout = microseconds;

	timeval val;
	val.tv_sec = inTimeout/100;
	val.tv_usec = inTimeout%100;
	
	if (setsockopt(socket,SOL_SOCKET,SO_RCVTIMEO,&val,sizeof(val))==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETINTIMEOUT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;	
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif	
}


//-------------------------------------------------------------------

unsigned long 
flushSocketExchange::getInTimeout()
{
	return inTimeout;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::setOutTimeout(unsigned long microseconds)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETOUTTIMEOUT,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif

	outTimeout = microseconds;

	timeval val;
	val.tv_sec = outTimeout/100;
	val.tv_usec = outTimeout%100;
	
	if (setsockopt(socket,SOL_SOCKET,SO_SNDTIMEO,&val,sizeof(val))==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETOUTTIMEOUT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif	
}


//-------------------------------------------------------------------

unsigned long 
flushSocketExchange::getOutTimeout()
{
	return outTimeout;
}

//-------------------------------------------------------------------

bool
flushSocketExchange::getSocketOpts(int option)
{
	if  ( (option&socketOpts) == option)
		return true;
	return false;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::setSockOption(socketOptionsEnum option, 
							bool flag)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETSOCKOPT,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif
	
	register int sockFlag(1);
	
	if (!flag)
		sockFlag = 0;
	
	register int real_option;
	
	switch (option)
	{
		case SOCKET_KEEP_ALIVE:
			real_option = SO_KEEPALIVE;
			break;
		case SOCKET_REUSE_ADDRESS:
			real_option = SO_REUSEADDR;
			break;
		case SOCKET_DONOT_USE_GATEWAY:
			real_option = SO_DONTROUTE;
			break;
		case SOCKET_BROADCAST:
			real_option = SO_BROADCAST;
			break;
		case SOCKET_OOB_INLINE:
			real_option = SO_OOBINLINE;
			break;
		#ifdef SO_REUSEPORT	
			case SOCKET_REUSE_PORT:
				real_option = SO_REUSEPORT;
				break;
		#endif
	}
	
		
	if (setsockopt(socket,SOL_SOCKET,real_option,&sockFlag,size_of_int)==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETSOCKOPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;	
		#endif	
	
	if (!flag)
		removeF(socketOpts,option);
	else
		addF(socketOpts,option);	
		
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
flushSocketExchange::setLingerSockOption(socketLingerOption option,
									int seconds)
{	
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETLINGERSOCKOPT,ERR_LIBDODO,FLUSHSOCKET_NO_SOCKET_CREATED,FLUSHSOCKET_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif
		
	linger lin;
	
	switch (option)
	{
		case SOCKET_GRACEFUL_CLOSE:
			lin.l_onoff = 0;
			break;
		case SOCKET_HARD_CLOSE:
			lin.l_onoff = 1;
			lin.l_linger = 0;
			break;
		case SOCKET_WAIT_CLOSE:
			lin.l_onoff = 1;
			lin.l_linger = seconds;
			break;
	}

	if (setsockopt(socket,SOL_SOCKET,SO_LINGER,&lin,sizeof(linger))==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKET,FLUSHSOCKET_SETLINGERSOCKOPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;	
		#endif
	
	removeF(socketOpts,SOCKET_GRACEFUL_CLOSE);
	removeF(socketOpts,SOCKET_HARD_CLOSE);
	removeF(socketOpts,SOCKET_WAIT_CLOSE);
	
	addF(socketOpts,option);	
		
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

void 
flushSocketExchange::close()
{
	if (!opened)
		flushSocket::_close(socket);
	opened = false;
}

//-------------------------------------------------------------------

