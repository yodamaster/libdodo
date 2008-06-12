/***************************************************************************
 *            cgiFastExchangeEx.h
 *
 *  Mon Dec 31 01:51:55 2007
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

#ifndef _CGIFASTEXCHANGEEX_H_
#define _CGIFASTEXCHANGEEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	namespace cgi
	{
		namespace fast
		{
			/**
			 * libdodo defined errors
			 */
			enum exchangeExR
			{
				FASTEXCHANGEEX_FAILEDTOFLUSH,
				FASTEXCHANGEEX_FAILEDTOPRINTSTRING,
			};

			/**
			 * explanations for libdodo defined errors
			 */
#define CGIFASTEXCHANGEEX_FAILEDTOFLUSH_STR       "Failed to flush."
#define CGIFASTEXCHANGEEX_FAILEDTOPRINTSTRING_STR "Failed to print string."

			/**
			 * IDs of functions where exception might be thrown
			 */
			enum exchangeFunctionsID
			{
				FASTEXCHANGEEX_FLUSH,
				FASTEXCHANGEEX__WRITE,
			};
		};
	};
};

#endif
