/***************************************************************************
 *            flushDisk.h
 *
 *  Tue Oct 8 08:19:57 2005
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
 
#ifndef _FLUSHDISK_H_
#define _FLUSHDISK_H_

#include <unistd.h>

#include <directives.h>

#include <flushDiskEx.h>
#include <types.h>
#include <tools.h>
#include <flushDiskTools.h>
#include <flush.h>

namespace dodo
{
	/**
	 * @enum flushDiskOperationTypeEnum describes type of operation for hook
	 */
	enum flushDiskOperationTypeEnum
	{
		FLUSHDISK_OPER_READ,
		FLUSHDISK_OPER_WRITE,
		FLUSHDISK_OPER_OPEN,
		FLUSHDISK_OPER_CLOSE
	};
	
	/**
	 * @enum flushDiskModesEnum descibes modes to open file
	 */
	enum flushDiskModesEnum
	{
		READ_ONLY,///< error if not exists file
		READ_WRITE,///< creates if not exists
		READ_WRITE_TRUNCATE,///< if exists = truncates
		APPEND///< for readin'; writin' to the end; you may skip parameter `pos` for write method
	};
	
	/**
	 * @enum flushDiskFileToCreateEnum describes file type you can create
	 */
	enum flushDiskFileToCreateEnum
	{
		REG_FILE,///< regular file
		TMP_FILE,///< temporary file. will be deleted after you exit program(or close it)
		FIFO_FILE///< FIFO file
	};
	
	/**
	 * @class flushDisk allows disk I/O manipulations
	 */
	
	class flushDisk : public flush
	
	#ifndef FLUSH_DISK_WO_XEXEC
								, public xexec
	#endif
	
	{
		
		friend class flushSocket;
		
		private:
		
			/**
			 * cosrtructor prevents from copyin'
			 */
			flushDisk(flushDisk &fd);
		
		public:
							
			/**
			 * constructor
			 * @param type describes type of file with what manipulation will be made
			 * @param path is path to the file
			 * 
			 * if type == TMP_FILE, u don't have to specify path
			 */
			flushDisk(flushDiskFileToCreateEnum type, const std::string &path = __string__);
			
			/**
			 * destructor
			 */
			virtual ~flushDisk();
			
			#ifndef FLUSH_DISK_WO_XEXEC
			
				/**
				 * adds hook after the operation by callback
				 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */			
				virtual int addPostExec(inExec func, void *data) const;
				
				/**
				 * adds hook before the operation by callback
				 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */
				virtual int addPreExec(inExec func, void *data) const;
				
				#ifdef DL_EXT
	
					/**
					 * set function from module that will be executed before/after the main action call
					 * the type of hook[pre/post] is defined in module
					 * @return number in list where function is set
					 * @param func is a pointer to function
					 * @param data is pointer to data toy want to pass to hook
					 */			
					virtual xexecCounts addExec(const std::string &module, void *data) const;
								
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 */
					virtual int addPostExec(const std::string &module, void *data) const;
					
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 */
					virtual int addPreExec(const std::string &module, void *data) const;
					
				#endif	
			
			#endif
						
			/**
			 * opens file
			 * @param path describes path to file
			 * closes previous opened file if needed
			 * if u want to create pipe, but not a pipe was created with the same name - false will be returned
			 * if u want to create regular file, but not regular file was created with the same name - false will be returned
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif			 
							open(const std::string &path = __string__) const;///if opened previous file, closes it		
			
			/**
			 * closes file
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							close() const;
							
			/**
			 * read string
			 * @param data will be filled with data
			 * @param pos indicates position in file
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							readString(std::string &data, unsigned long pos = 0) const;
			/**
			 * read data
			 * @param data will be filled with data
			 * @param pos indicates position in file
			 */							
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							read(char * const data, unsigned long pos = 0) const;///reads to void*; return false if eof		
			
			/**
			 * write string
			 * @param data will be written to file
			 * @param pos indicates position in file
			 */	
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							writeString(const std::string &data, unsigned long pos = 0);
										
			/**
			 * write string
			 * @param data will be written to file
			 * @param pos indicates position in file
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							write(const char * const data, unsigned long pos = 0);

			/**
			 * erase node on position
			 * @param pos indicates position in file
			 * @note for xexec  - no call for pre/postExec is performed, no operation type is set, 'cos it's only special type of write!!
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							erase(unsigned long pos);
			/**
			 * flushes to disk
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							flush();
			
			bool over;///< indicates whether overright or not; if tou want to write to nonempty node error will be occured; for files,tmp_files only
			mutable flushDiskModesEnum mode;///< mode to open file; if you change it then you have to reopen!
			
			/**
			 * @return path of the opened file
			 */			
			virtual std::string getPath() const;

			mutable flushDiskFileToCreateEnum fileType;///< type of file; if you change then it you have to reopen!
			
									
		private:
		
			mutable std::string path;///< file name
					
			mutable FILE *file;///< file handler
			
			mutable bool append;///< if true, will append to the end of the file, even pos is set.
	};

};

#endif