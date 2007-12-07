/***************************************************************************
 *            imageEx.h
 *
 *  Thu Nov 23 00:19:57 2007
 *  Copyright  2007  Ni@m
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

#ifndef _IMAGEEX_H_
#define _IMAGEEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * next for image
	 */

	/**
	 * libdodo defined errors
	 */
	enum imageFunctionsExR
	{
		IMAGE_BADINFO
	};

	/**
	 * libdodo defined errors' explanation
	 */
	#define IMAGE_BADINFO_STR "Bad information for image was provided."

	/**
	 * ID of function where exception was thrown
	 */
	enum imageFunctionsID
	{
		IMAGE_READ,
		IMAGE_WRITE,
		IMAGE_SETENCODER,
		IMAGE_SCALE,
		IMAGE_ROTATE,
	};

};

#endif