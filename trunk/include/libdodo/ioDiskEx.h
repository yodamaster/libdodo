/***************************************************************************
 *            ioDiskEx.h
 *
 *  Mon Feb 21 03:03:47 2005
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

#ifndef _IODISKEX_H_
#define _IODISKEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{

	/**
	 * libdodo defined errors
	 */
	enum ioDiskExR
	{
		IODISK_WRONG_FILENAME,
		IODISK_CANNOT_OVEWRITE,
		IODISK_FILE_IS_SHORTER_THAN_GIVEN_POSITION,
	};

	/**
	 * libdodo defined errors' explanation
	 */
		#define IODISK_WRONG_FILENAME_STR                         "Probably wrong filename, type of file is wrong or `path` is empty!"
		#define IODISK_CANNOT_OVEWRITE_STR                        "You set option not to overwrite, but try to do that =)"
		#define IODISK_FILE_IS_SHORTER_THAN_GIVEN_POSITION_STR    "Given file couldn't reach given position. File is short."

	/**
	 * ID of function where exception was thrown
	 */
	enum ioDiskFunctionsID
	{
		IODISK_CLOSE,
		IODISK_OPEN,
		IODISK_READ,
		IODISK_WRITE,
		IODISK_READSTREAM,
		IODISK_READSTRING,
		IODISK_READSTREAMSTRING,
		IODISK_WRITESTREAM,
		IODISK_IO,
	};
};

#endif