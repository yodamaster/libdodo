/***************************************************************************
 *            dbSqlite.h
 *
 *  Sat Dec 10 06:45:19 2005
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
#ifndef _DBSQLITE_H_
#define _DBSQLITE_H_

#include <directives.h>

#ifdef SQLITE_EXT
	
	#include <sqlite3.h>

	#include <tools.h>
	#include <dbSqliteEx.h>	
	#include <dbSqlBase.h>
	#include <xexec.h>
	
	namespace dodo
	{
		
		/**
		 * @enum dbSqliteOperTypeEnum describes type of operation for hook
		 */
		enum dbSqliteOperTypeEnum
		{
			DBSQLITE_OPER_CONNECT,
			DBSQLITE_OPER_EXEC,
			DBSQLITE_OPER_DISCONNECT,
			DBSQLITE_OPER_FETCHROW,
			DBSQLITE_OPER_FETCHFIELD,
		};
		
		class dbSqlite;///< to make struct with this class before class declaration
		
		/**
		 * @struct __sqliteCallbackData passes to callback function
		 */
		struct __sqliteCallbackData
		{
			dbSqlite *data;
			bool first;
		};
		
		/**
	 	 * @class dbSqlite is an interface to sqlite db through sql-,database- independent interfaces
	 	 * @note for autoFraming sqlite must be compiled with SQLITE_ENABLE_COLUMN_METADATA defined
		 */
		class dbSqlite : public dbSqlBase
		
		#ifndef DBSQLITE_WO_XEXEC
										, public xexec
		#endif
		
		{
			private:
			
				/**
				 * constructor
				 * to prevent from copying
				 */
				dbSqlite(dbSqlite &a_pp);
			
			public:
				
				/**
				 * constructor
				 */
				dbSqlite();
				
				/**
				 * destructor
				 */
				virtual ~dbSqlite();	
			
				/**
				 * connect to database
				 */	
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								connect() const;
				
				/**
				 * disconnect from database
				 */
				virtual void disconnect() const;
				
				/**
				 * @return amount of affected rows(update,delete...)
				 */
				virtual unsigned int affectedRowsCount();
				
				/**
				 * @return amount of rows got from request(select ...)
				 */
				virtual unsigned int rowsCount() const;				
				
				/**
				 * @return amount of fields got from request(select ...)
				 */
				virtual unsigned int fieldsCount() const;
				
				/**
				 * @return array of rows got from request
				 */
				virtual dodoArray<dodoStringArr> fetchRow() const;
				
				/**
				 * @return array of fields got from request
				 */
				virtual dodoStringArr fetchField() const;
				
				/**
				 * @return structure that holds array of rows and array of fields got from request
				 */
				virtual __dbStorage fetch() const;
				
				/**
				 * @return array that holds assoc array['fiels'=>'value'] got from request
				 */
				virtual dodoStringMapArr fetchAssoc() const;
							
				/**
				 * executes collected request
				 * @param query contains query for DB. You may pass it if you don't use methods like select, update of libdodo
				 * @param result describes whether request returns result[show, select...] or not[delete, update]
				 * @note to insert|update using BLOB values use hint:
				 * 		make standart method calls to collect query, but instead of blob-values place $1 .. $n [identificators]  
				 * 		call setBLOBValues method to set blob values according to id
				 * 		call exec method with query = "dodo:hint:db:blob"
				 * 		YOU MUST 
				 * 				set preventFraming and preventEscaping to true 
				 * 				by yourself escape[using dbSqlBase::escapeFields] and frame with '' non-blob text data before inserting/updating
				 * 				by yourself escape[using dbSqlBase::unescapeFields] non-blob text data after selecting
				 */				
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								exec(const std::string &query = __string__, bool result = false) const;
				
				/**
				 * @param values defines what blob-type values will be applied for dodo:hint:db:blob instead of identificators
				 */				
				virtual void setBLOBValues(const dodoStringArr &values);	
				
				#ifndef DBSQLITE_WO_XEXEC
				
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
					 	 * @param toInit indicates data that will path to initialize function
						 */			
						virtual xexecCounts addExec(const std::string &module, void *data, void *toInit = NULL) const;
					
						/**
						 * adds hook after the operation by callback
					 	 * @return number in list where function is set
						 * @param module is a path to module, whrere hook exists
						 * @param data is pointer to data toy want to pass to hook
						 * @param toInit indicates data that will path to initialize function
						 */
						virtual int addPostExec(const std::string &module, void *data, void *toInit = NULL) const;
						
						/**
						 * adds hook after the operation by callback
					 	 * @return number in list where function is set
						 * @param module is a path to module, whrere hook exists
						 * @param data is pointer to data toy want to pass to hook
					 	 * @param toInit indicates data that will path to initialize function
						 */
						virtual int addPreExec(const std::string &module, void *data, void *toInit = NULL) const;
					
					#endif
				
				#endif
				 
			protected:
			
				/**
				 * executes request
				 * @param query contains query for DB. You may pass it if you don't use methods like select, update of libdodo
				 * @param result describes whether request returns result[show, select...] or not[delete, update]
				 * @note pure sqlite actions
				 * in function without `_` hooks are calling
				 * to insert|update using BLOB values use hint:
				 * 		make standart method calls to collect query, but instead of blob-values place $1 .. $n [identificators]  
				 * 		call setBLOBValues method to set blob values according to id
				 * 		call exec method with query = "dodo:hint:db:blob"
				 * 		YOU MUST 
				 * 				set preventFraming and preventEscaping to true 
				 * 				by yourself escape[using dbSqlBase::escapeFields] and frame with '' non-blob text data before inserting/updating
				 * 				by yourself escape[using dbSqlBase::unescapeFields] non-blob text data after selecting
				 */
				#ifndef NO_EX
					virtual void 
				#else
					virtual bool 
				#endif
								_exec(const std::string &query, bool result) const;
					
			private:
				
				mutable sqlite3 *lite;///< handle to DB
				mutable sqlite3_stmt *liteStmt;///< handlde to sqlite statement[e.g. result]  

				mutable bool empty;///< for detectin' whether liteStmt is empty or not
				
				mutable int result;///< store result for query
				
				mutable dodoStringArr rowsPart;///< to store rows
				mutable std::string rowPart;///< to set temporary row content
				mutable dodoStringMap rowFieldsPart;///< to store rows with fields' names
				
				mutable unsigned int numFields;///< number of fields
				mutable unsigned int numRows;///< number of rows
				
				mutable bool blobHint;///< if true - enable blob hint

				mutable dodoArray<dodoStringArr> rows;///< to store rows
				mutable dodoStringArr fields;///< to store fields
				mutable dodoStringMapArr rowsFields;///< to store arrau of hashes 'field'=>'row'
				
				mutable dodoStringArr blobs;///< to store blob data			
		};

	};
	
#endif

#endif
