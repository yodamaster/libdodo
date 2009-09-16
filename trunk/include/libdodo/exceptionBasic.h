/***************************************************************************
 *			  exceptionBasic.h
 *
 *	Mon Feb	 7 20:02:06 2005
 *	Copyright  2005	 Ni@m
 *	niam.niam@gmail.com
 ****************************************************************************/

/*
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *	the Free Software Foundation;
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU Library General Public License for more details.
 *
 *	You should have received a copy of the GNU Lesser General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _EXCEPTIONBASIC_H_
#define _EXCEPTIONBASIC_H_ 1

#include <libdodo/directives.h>

#include <exception>

#include <libdodo/types.h>

namespace dodo {
	namespace exception {
		/**
		 * @enum errnoEnum defines sources, where the code of the error has been got
		 */
		enum errnoEnum {
			ERRNO_LIBDODO,
			ERRNO_ERRNO,
			ERRNO_MYSQL,
			ERRNO_H_ERRNO,
			ERRNO_DYNLOAD,
			ERRNO_LIBXML2,
			ERRNO_ZLIB,
			ERRNO_SQLITE,
			ERRNO_BZIP,
			ERRNO_IMAGEMAGICK,
			ERRNO_POSIXREGEX,
			ERRNO_PCRE,
			ERRNO_BFD,
			ERRNO_OPENSSL,
		};

		/**
		 * @enum moduleEnum defines modules where exception might be thrown
		 */
		enum moduleEnum {
			MODULE_DATABASEMYSQL = 0,
			MODULE_DATABASEPOSTGRESQL,
			MODULE_DATABASESQLCONSTRUCTOR,
			MODULE_DATABASESQLITE,
			MODULE_DATAFORMATXMLPROCESSOR,
			MODULE_DATAFORMATJSONNODE,
			MODULE_DATAFORMATJSONPROCESSOR,
			MODULE_DATATPLPROCESSOR,
			MODULE_DATAMEMORYSHARED,
			MODULE_XEXEC,
			MODULE_TOOLSFILESYSTEM,
			MODULE_TOOLSOS,
			MODULE_TOOLSOSSYNCTHREADSECTION,
			MODULE_TOOLSTIME,
			MODULE_TOOLSMISC,
			MODULE_TOOLSCODE,
			MODULE_TOOLSNETWORK,
			MODULE_TOOLSLIBRARY,
			MODULE_TOOLSREGEXP,
			MODULE_IOFILEREGULAR,
			MODULE_IOFILETEMP,
			MODULE_IOFILEFIFO,
			MODULE_IOPIPE,
			MODULE_IOSTDIO,
			MODULE_IOMEMORY,
			MODULE_IONETWORKCONNECTION,
			MODULE_IONETWORKCLIENT,
			MODULE_IONETWORKSERVER,
			MODULE_IONETWORKSSLCLIENT,
			MODULE_IONETWORKSSLSERVER,
			MODULE_IONETWORKEXCHANGE,
			MODULE_IONETWORKSSLEXCHANGE,
			MODULE_IONETWORKHTTP,
			MODULE_IOEVENTMANAGER,
			MODULE_PCJOBPROCESSMANAGER,
			MODULE_PCJOBTHREADMANAGER,
			MODULE_PCSYNCTHREADDATASINGLE,
			MODULE_PCSYNCPROCESSDATASINGLE,
			MODULE_PCSYNCTHREADDATACOLLECTION,
			MODULE_PCSYNCPROCESSDATACOLLECTION,
			MODULE_PCSYNCTHREADSECTION,
			MODULE_PCSYNCPROCESSSECTION,
			MODULE_GRAPHICSIMAGE,
			MODULE_GRAPHICSTRANSFORM,
			MODULE_GRAPHICSDRAW,
			MODULE_CGIFASTSERVER,
			MODULE_CGIFASTEXCHANGE,
			MODULE_CGIDIALOGUE,
			MODULE_RPCVALUE,
			MODULE_RPCRESPONSE,
			MODULE_RPCJSONMETHOD,
			MODULE_RPCJSONRESPONSE,

			MODULE_ENUMSIZE
		};

#ifdef CALLSTACK_EX
		/**
		 * @struct __call__
		 * @brief describes function call in call stack
		 */
		struct __call__ {
			dodoString object;      ///< name of the object where call was found
			dodoString symbol;      ///< name of the call
			void       *address;    ///< address of the call
		};
#endif

		/**
		 * @class basic
		 * @brief describes exception that has been thrown
		 */
		class basic : public std::exception,
					  public singleton<basic>{
			friend class singleton<basic>;

		  private:

			/**
			 * constructor
			 */
			basic() throw ();

		  public:

#ifdef DL_EXT
			/**
			 * @struct __module__
			 * @brief is returned from initModule in the library
			 */
			struct __module__ {
				char  name[64];         ///< name of the library
				char  discription[256]; ///< discription of the library
				char  hook[64];         ///< name of the function in module that will be as a hook
				bool modules[MODULE_ENUMSIZE];           ///< for what modules handler should be set, @see exception::moduleEnum
			};

			/**
			 * @typedef initModule
			 * @brief defines type of init function for library
			 * @param data defines user data
			 * @note name in the library must be initExceptionBasicModule
			 */
			typedef __module__ (*initModule)(void *data);

			/**
			 * @typedef deinitModule
			 * @brief defines type of deinit function for library
			 * @note name in the library must be deinitExceptionBasicModule
			 */
			typedef void (*deinitModule)();
#endif

			/**
			 * @typedef handler
			 * @brief defines type of hook function
			 * @param module defines module where exception occured, @see exception::moduleEnum
			 * @param ex defines pointer to basic object with exception information
			 * @param data defines user data
			 */
			typedef void (*handler)(int module, basic *ex, void *data);

			/**
			 * constructor
			 * @param module defines module where exception has been thrown
			 * @param function defines function where exception has been thrown, @see *Ex.h headers for IDs
			 * @param errnoSource defines source of the error code and of the error string, @see exception::errnoEnum
			 * @param errNo defines error code
			 * @param errStr defines error string
			 * @param line defines line where exception has been thrown
			 * @param file defines file where exception has been thrown
			 * @param message defines custom message that might clarify the exception
			 */
			basic(int              module,
				  int              function,
				  int              errnoSource,
				  int              errNo,
				  const dodoString &errStr,
				  unsigned long    line,
				  const dodoString &file,
				  const dodoString &message = __dodostring__) throw ();

			/**
			 * destructor
			 */
			~basic() throw ();

			/**
			 * @return error string
			 */
			operator const dodoString &();

			/**
			 * return error string
			 */
			virtual const char *what() const throw ();

#ifdef CALLSTACK_EX
			/**
			 * @return call stack to the exception point
			 */
			virtual dodoString backtrace();
#endif

			int source;                      ///< module where exception has been thrown
			int function;                         ///< function where exception has been thrown, @see *Ex.h headers for IDs
			int errnoSource;                    ///< the source of the error code and of the error string
			int errNo;                      ///< error code
			dodoString errStr;              ///< error string
			unsigned long line;                 ///< line where exception has been thrown
			dodoString file;                    ///< file where exception has been thrown
			dodoString message;                 ///< custom message that might clarify the exception

#ifdef CALLSTACK_EX
			dodoArray<__call__> callStack;      ///< call stack of the raised exception
#endif

			/**
			 * set handler for exceptions for specific module
			 * @param module defines for what module to set handler
			 * @param handler defines function that will be called when exception is thrown
			 * @param data decribes data that will be passed to the handler
			 */
			static void setHandler(moduleEnum module,
										handler    handler,
										void            *data);

			/**
			 * set handlers for exceptions for all modules
			 * @param handler defines function that will be called when exception is thrown
			 * @param data decribes data that will be passed to the handler
			 */
			static void setHandler(handler handler,
										 void         *data);

			/**
			 * remove handler set for exceptions for specific module
			 * @param module defines for what module to remove handler
			 */
			static void removeHandler(moduleEnum module);

			/**
			 * remove handlers set for exceptions for all modules
			 */
			static void removeHandlers();

#ifdef DL_EXT
			/**
			 * @return information about module
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param toInit defines data that will be passed to the init function
			 */
			static __module__ module(const dodoString &path,
											  void             *toInit = NULL);

			/**
			 * set handler for exceptions for specific module
			 * @return false on error
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data decribes data that will be passed to the handler
			 * @param toInit defines data that will be passed to the init function
			 * @note module for what to set handler is taken from the library information, @see exception::basic::__module__
			 */
			static void setHandler(const dodoString &path,
										void             *data,
										void             *toInit = NULL);
#endif

		  protected:

			static handler handlers[MODULE_ENUMSIZE]; ///< exception handlers

			static bool handlerMap[MODULE_ENUMSIZE];       ///< map of set handlers

			static void *handlerData[MODULE_ENUMSIZE];     ///< data that will be passed to the handler

#ifdef DL_EXT
			static void *handles[MODULE_ENUMSIZE];         ///< handles to the libraries

			static bool handlesOpened[MODULE_ENUMSIZE];    ///< map of the opened libraries
#endif

			/**
			 * @class syncThreadSection
			 * @brief performs atomic locks using mutexes
			 */
			class syncThreadSection {
			  public:

				/**
				 * consructor
				 */
				syncThreadSection();

				/**
				 * destructor
				 */
				virtual ~syncThreadSection();

				/**
				 * lock critical section
				 */
				virtual void acquire();

				/**
				 * unlock critical section
				 */
				virtual void release();

			  protected:

#ifdef PTHREAD_EXT
				static pthread_mutex_t keeper;  ///< mutex
#endif
			};

			static syncThreadSection keeper;    ///< lock

			/**
			 * @class syncThreadStack
			 * @brief provides thread safe behaviour
			 * @note it locks in constructor and unlocks in destructor
			 */
			class syncThreadStack {
			  public:

				/**
				 * contructor
				 */
				syncThreadStack();

				/**
				 * destructor
				 */
				virtual ~syncThreadStack();
			};

			static unsigned long instances;
		};
	};
};
#endif
