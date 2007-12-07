/***************************************************************************
 *            cgiPreprocessor.h
 *
 *  Sun Jan 22 19:05:57 2006
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

#ifndef _CGIPREPROCESSOR_H_
#define _CGIPREPROCESSOR_H_

#include <libdodo/directives.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <libdodo/tools.h>
#include <libdodo/cgiPreprocessorEx.h>
#include <libdodo/types.h>

namespace dodo
{

	/**
	 * @class cgiPreprocessor prepares template for future parsings
	 */
	class cgiPreprocessor
	{
		public:

			/**
			 * constructor
			 */
			cgiPreprocessor();

			/**
			 * destructor
			 */
			virtual ~cgiPreprocessor();

			/**
			 * @return preprocessed template from file
			 * @param path indicates path where template stays
			 */
			virtual dodoString preProcess(const dodoString &path);

		protected:

			/**
			 * @return line number in file
			 * @param newLinePos describes array of newlines' positions
			 * @param pos describes pos in file
			 */
			virtual unsigned long getLineNumber(const dodoArray<unsigned long> &newLinePos, unsigned long pos);

			/**
			 * @return file contents
			 * @param path indicates what to read
			 * @param newLinePos describes array of newlines' positions
			 */
			virtual dodoString read(const dodoString &path, dodoArray<unsigned long> &newLinePos);

			dodoArray< dodoArray<unsigned long> > newLinePositions; ///< stack of processed new lines' positions

			/**
			 * @enum processorStatemets describes positions in 'statements' property
			 */
			enum processorStatemets
			{
				DODO = 0,
				OPEN_ST,
				CLOSE_ST,
				OPEN_NP,
				CLOSE_NP,
				OPEN_COMM,
				CLOSE_COMM,
				OPEN_IF,
				ELSE,
				CLOSE_IF,
				OPEN_FOR,
				IN,
				KEY_VALUE,
				CLOSE_FOR,
				PRINT,
				BREAK,
				CONT,
				ASSIGN,
				ASSIGN_OP,
				OPEN_NS,
				CLOSE_NS,
				INCLUDE,
				ITERATOR,
				VERSION,
				DOT,
				COMA,
				DOLLAR,
				FALSE,
				EQ,
				NE,
				LE,
				GE,
				LT,
				GT,
				OPEN_VARPART,
				CLOSE_VARPART,
			};
			static const dodoString statements[36]; ///< dodo statement[for dodo.*, ...]
	};

};

#endif