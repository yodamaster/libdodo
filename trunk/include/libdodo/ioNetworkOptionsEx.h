/***************************************************************************
 *            ioNetworkOptionsEx.h
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

#ifndef _IONETWORKOPTIONSEX_H_
#define _IONETWORKOPTIONSEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	/**
	 * libdodo defined errors
	 */
	enum ioNetworkOptionsExR
	{
		IONETWORKOPTIONSEX_NOSOCKETCREATED,
		IONETWORKOPTIONSEX_WRONGPARAMETER,
	};

	/**
	 * explanations for libdodo defined errors
	 */
#define IONETWORKOPTIONSEX_NOSOCKETCREATED_STR "No socket was created =(."
#define IONETWORKOPTIONSEX_WRONGPARAMETER_STR "Wrong paramather passed to function."

	/**
	 * IDs of functions where exception might be thrown
	 */
	enum ioNetworkOptionsFunctionsID
	{
		IONETWORKOPTIONSEX_SETINBUFFERSIZE,
		IONETWORKOPTIONSEX_SETOUTBUFFERSIZE,
		IONETWORKOPTIONSEX_SETINTIMEOUT,
		IONETWORKOPTIONSEX_SETOUTTIMEOUT,
		IONETWORKOPTIONSEX_SETSOCKOPT,
		IONETWORKOPTIONSEX_SETLINGERSOCKOPT,
		IONETWORKOPTIONSEX_BLOCK,
		IONETWORKOPTIONSEX__CLOSE,
	};

};

#endif