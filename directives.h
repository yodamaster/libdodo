/***************************************************************************
 *            directives.h
 *
 *  Fri Jul 15 22:19:48 2005
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
 
#ifndef _DIRECTIVES_H_
#define _DIRECTIVES_H_

#include <stdio.h>

#define LIBDODO_VERSION "libdodo/0.0.7"

/**
 * !!!!note!!!!!
 * please read notes before uncommentin'!!!
 */


/** 
 * compile or use without exceptions. usage of exception depends of library compiled options; 
 * if u compiled library with exeptions u have to work with 'em;
 * if u compiled library without exeptions u have to work without 'em;
 */
//#define NO_EX

/**
 * removes some checks
 * 1) check for request completeness(sqlBase)
 * 2) some checks that now is not needed, but in future releases will be neded, but set for now for more compability with newly writed code
 */
#define FAST

//#define UNICODE

//#define FLUSH_DISK_WO_XEXEC = faster, if u don't want to use it!
//#define MYSQLPP_WO_XEXEC
//#define FLUSH_STD_WO_XEXEC
//#define FLUSH_SOCKET_WO_XEXEC

///socket default timeouts
#define RECIEVE_TIMEOUT 1000
#define SEND_TIMEOUT 1000

///how many modules can init per object
#define XEXEC_MAXMODULES 10

///for flushDisk::getFileContentArr
#define DISK_MAXLINELEN 10240

///default values of buffer for io operations(flush*)
#define INSIZE 1024
#define OUTSIZE 1024	

///default values of socket buffer for io operations(flushSocket)
#define SOCKET_INSIZE 10240
#define SOCKET_OUTSIZE 10240

///default values of stdin/out buffer for io operations(flushSTD)
#define STD_INSIZE 1024000
#define STD_OUTSIZE 1024000

///amount of seconds for linger
#define SOCKET_LINGER_PERIOD 1
#define SOCKET_LINGER_OPTION SOCKET_WAIT_CLOSE

///max length of the file path
#define MAXPATHLEN FILENAME_MAX

#endif /* _DIRECTIVES_H */
