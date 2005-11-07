/***************************************************************************
 *            flush.h
 *
 *  Tue Oct 11 00:19:57 2005
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef _FLUSH_H_
#define _FLUSH_H_

#include "directives.h"
#include "xexec.h"
#include "types.h"

namespace dodo
{

	enum flushOperationTypeEnum///for xExec
	{
		FLUSH_OPER_NONE
	};
	
	/**
	 * preExec - before operation, postExec - after operation and before checks(can be exeptions).
	 * flushOperationTypeEnum - such operations have handlers for xexec. position of preExec - before action, postExec - after action, after check of goodness of operation, so if error occured - postExec won't be called
	 */
	class flush : public xexec, public dodoBase
	{
		public:
		
			/**
			 * constructors, destructors
			 */
			flush();
			virtual ~flush();
			
			mutable unsigned long inSize;///size of data; size of buffer
			mutable unsigned long outSize;///size of data; size of buffer
			mutable std::string buffer;///before readin' or after writin' the storege sets to buffer if next option is set to true(bufferize); usefull for xExec
			mutable bool bufferize;///false by default; set true, if u r usin' xexec(hooks)
			mutable bool normalize;///only for write mode, if string, that is going to write is less than set size, will left space with ' '; it will prevent 'unknowns' in file. true by default
			mutable int operType;///to detect in xexec what type of operation
		protected:
		
			mutable bool opened;///indicates whether file(connection) opened or not
	};
};

#endif /*FLUSH_H_*/
