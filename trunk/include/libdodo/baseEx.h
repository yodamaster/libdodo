/***************************************************************************
 *            baseEx.h
 *
 *  Mon Feb  7 20:02:06 2005
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

#ifndef _BASEEX_H_
#define _BASEEX_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>

#ifdef DL_EXT

	#include <dlfcn.h>

#endif

namespace dodo
{
	/**
	 * @enum errnoSourceEnum describes names of sources, where code of error got
	 */
	enum errnoSourceEnum
	{
		ERR_LIBDODO,
		ERR_ERRNO,
		ERR_MYSQL,
		ERR_H_ERRNO,
		ERR_DYNLOAD,
		ERR_LIBXML2,
		ERR_ZLIB,
		ERR_SQLITE,
		ERR_BZIP,
		ERR_IMAGEMAGICK
	};

	/**
	 * @enum errorModuleEnum describes names of modules where exception was thrown
	 */
	enum errorModuleEnum
	{
		ERRMODULE_IOSOCKET = 0,
		ERRMODULE_IODISK,
		ERRMODULE_IODISKTOOLS,
		ERRMODULE_DBMYSQL,
		ERRMODULE_REGEXP,
		ERRMODULE_DBSQLBASE,
		ERRMODULE_IOSTD,
		ERRMODULE_XEXEC,
		ERRMODULE_SYSTEMTOOLS,
		ERRMODULE_TIMETOOLS,
		ERRMODULE_LIBXML2,
		ERRMODULE_TOOLS,
		ERRMODULE_DBSQLITE,
		ERRMODULE_IOSOCKETOPTIONS,
		ERRMODULE_IOSOCKETEXCHANGE,
		ERRMODULE_IOSOCKETTOOLS,
		ERRMODULE_SYSTEMPROCESSES,
		ERRMODULE_SYSTEMTHREADS,
		ERRMODULE_SYSTEMSHAREDDATA,
		ERRMODULE_SYSTEMTHREADSHAREDDATAGUARD,
		ERRMODULE_SYSTEMPROCESSSHAREDDATAGUARD,
		ERRMODULE_SYSTEMPROCESSSHAREDDATACOLLECTIONGUARD,
		ERRMODULE_SYSTEMTHREADSHAREDDATACOLLECTIONGUARD,
		ERRMODULE_DBPOSTGRESQL,
		ERRMODULE_CGIPREPROCESSOR,
		ERRMODULE_CGIPROCESSOR,
		ERRMODULE_IONBA,
		ERRMODULE_ATOMICMUTEX,
		ERRMODULE_ATOMICSEMAPHORE,
		ERRMODULE_IMAGE,
	};

		#define AM_MODULES    30

		#ifdef DL_EXT

	/**
	 * @struct exMod must be returned from initExModule in the module
	 */
	struct exMod
	{
		char name[64];                  ///< name of module
		char discription[256];          ///< discription of module
		char hook[64];                  ///< name of function in module that will be a hook
		errorModuleEnum module;         ///< for what module set handler
	};

	/**
	 * @typedef describes function in module that must return info for the hook
	 */
	typedef exMod (*initExModule)(void *);

	/**
	 * @typedef describes function in module that will be called during module unloading
	 */
	typedef void (*deinitExModule)();

		#endif

	class baseEx;    ///< to make typedef before class declaration

	/**
	 * @typedef describes function that can handle error
	 */
	typedef void (*errorHandler)(errorModuleEnum, baseEx *, void *);

	/**
	 * @class baseEx
	 * describes exception that was thrown
	 */
	class baseEx
	{
		public:

			/**
			 * constructor
			 * @param errModule in what module was thrown
			 * @param functionID in what function was thrown[see *Ex.h headers for IDs]
			 * @param errnoSource where error code and string was taken
			 * @param baseErrno error code
			 * @param baseErrstr describes error code
			 * @param line line, where was thrown
			 * @param file file, where was thrown
			 */
			baseEx(errorModuleEnum errModule, unsigned long functionID, errnoSourceEnum errnoSource, int baseErrno, const dodoString &baseErrstr, unsigned long line, const dodoString &file, const dodoString &message = __dodostring__);

			/**
			 * destructor
			 */
			~baseEx();

			/**
			 * @return string that describes error
			 * 	catch(baseEx ex)
			 *	{
			 *		cout << ex << endl;
			 *	}
			 */
			operator const dodoString &();

			errorModuleEnum errModule;      ///< in what module was thrown
			unsigned long funcID;           ///< in what function was thrown[see *Ex.h headers for IDs]
			errnoSourceEnum errnoSource;    ///< 	where error code and string was taken

			int baseErrno;                  ///< error code
			dodoString baseErrstr;          ///< describes error code

			unsigned long line;             ///< line, where was thrown
			dodoString file;                ///< file, where was thrown

			dodoString message;             ///< custom message

			/**
			 * set handler on error for specific module
			 * @param module indicates for what module to set handler
			 * @param handler is function that will be called when error occured[in catch]
			 * @param data is data that will be passed to handler
			 */
			static void setErrorHandler(errorModuleEnum module, errorHandler handler, void *data);

			/**
			 * set handlers on error for all modules
			 * @param handler is function that will be called when error occured[in catch]
			 * @param data is data that will be passed to handler
			 */
			static void setErrorHandlers(errorHandler handler, void *data);

			/**
			 * remove handler on error for specific module
			 * @param module indicates for what module to set handler
			 */
			static void unsetErrorHandler(errorModuleEnum module);

			/**
			 * unset handlers on error for all modules
			 */
			static void unsetErrorHandlers();

				#ifdef DL_EXT

			/**
			 * @return info about module
			 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
			 * @param toInit indicates data that will path to initialize function
			 */
			static exMod getModuleInfo(const dodoString &module, void *toInit = NULL);

			/**
			 * set handler on error for specific module
			 * @return false on error
			 * @param module indicates for what module to set handler
			 * @param path is path to module from what function will be called when error occured[in catch]
			 * @param data is data that will be passed to handler
			 * @param toInit indicates data that will path to initialize function
			 */
			static bool setErrorHandler(errorModuleEnum module, const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * set handler on error for specific module
			 * @return false on error
			 * @param path is path to module from what function will be called when error occured[in catch]
			 * @param data is data that will be passed to handler
			 * @param toInit indicates data that will path to initialize function
			 */
			static bool setErrorHandler(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * set handler on error for all modules
			 * @return false on error
			 * @param handler is path to module from what function will be called when error occured[in catch]
			 * @param data is data that will be passed to handler
			 * @param toInit indicates data that will path to initialize function
			 */
			static bool setErrorHandlers(const dodoString &path, void *data, void *toInit = NULL);

				#endif

		protected:

			static errorHandler handlersEx[AM_MODULES];     ///< handlers

			static bool handlerSetEx[AM_MODULES];           ///< indicates whether handler was set

			static void *handlerDataEx[AM_MODULES];         ///< data that will be passed to handler

				#ifdef DL_EXT

			static void *handlesEx[AM_MODULES];             ///< handles to modules

			static bool handlesOpenedEx[AM_MODULES];        //< map of opened modules

				#endif
	};

};

#endif