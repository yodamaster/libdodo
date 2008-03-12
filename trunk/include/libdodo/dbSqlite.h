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

#include <libdodo/directives.h>

#ifdef SQLITE_EXT

#include <sqlite3.h>

#include <libdodo/tools.h>
#include <libdodo/dbSqliteEx.h>
#include <libdodo/dbSqlBase.h>
#include <libdodo/xexec.h>

namespace dodo
{

	/**
	 * @enum dbSqliteOperTypeEnum defines type of operation for hook
	 */
	enum dbSqliteOperTypeEnum
	{
		DBSQLITE_OPERATION_CONNECT,
		DBSQLITE_OPERATION_EXEC,
		DBSQLITE_OPERATION_DISCONNECT,
		DBSQLITE_OPERATION_FETCHROW,
		DBSQLITE_OPERATION_FETCHFIELD,
	};
	
	/**
	 * @enum dbSqliteHintEnum define DB hints
	 */
	enum dbSqliteHintEnum
	{
		DBSQLITE_HINT_NONE = 0,
		DBSQLITE_HINT_BLOB = 2,///< insert|update using BLOB values
	};

	class dbSqlite;

	/**
	 * @class dbSqlite provides an interface to sqlite db
	 * @note for autoFraming sqlite must be compiled with defined SQLITE_ENABLE_COLUMN_METADATA directive
	 */
	class dbSqlite : public dbSqlBase
	{
		private:

			/**
			 * constructor
			 * prevent copying
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
			 * connect to the database
			 */
			virtual void connect();

			/**
			 * disconnect from the database
			 */
			virtual void disconnect();

			/**
			 * @return amount of affected rows from the evaluated request
			 */
			virtual unsigned int affectedRowsCount() const;

			/**
			 * @return amount of received rows from the evaluated request
			 */
			virtual unsigned int rowsCount() const;

			/**
			 * @return amount of received fields from the evaluated request
			 */
			virtual unsigned int fieldsCount() const;

			/**
			 * @return received rows from the evaluated request
			 */
			virtual dodoArray<dodoStringArray> fetchRow() const;

			/**
			 * @return received fields from the evaluated request
			 */
			virtual dodoStringArray fetchField() const;

			/**
			 * @return structure received rows and fields from the evaluated request
			 */
			virtual __dbStorage fetch() const;

			/**
			 * @return received rows and fields from the evaluated request using hash `key`=>`value`
			 */
			virtual dodoStringMapArray fetchAssoc() const;

			/**
			 * execute request
			 * @param query defines query; you may define it if you don't use db methods like select, update
			 * @param result defines type of result; if true query return the result
			 * @note to insert|update using BLOB values use hint:
			 * 		make standart method calls to collect query, but instead of blob-values place $1 .. $n [identificators]
			 * 		call setBLOBValues method to set blob values according to id
			 * 		define hint class attribute as DBSQLITE_HINT_BLOB[you have to define it each time before this call]
			 * 		YOU MUST
			 * 				define preventFraming and preventEscaping as true
			 * 				by yourself escape[using dbSqlBase::escapeFields] and frame with '' non-blob text data before inserting/updating
			 * 				by yourself escape[using dbSqlBase::unescapeFields] non-blob text data after selecting
			 */
			virtual void exec(const dodoString &query = __dodostring__, bool result = false);

			unsigned short hint;///< DB hint[see dbSqliteHintEnum]
			
			/**
			 * set BLOB data for the request
			 * @param values defines blob-type values that will be applied for dodo:hint:db:blob instead of identificators
			 */
			virtual void setBLOBValues(const dodoStringArray &values);

#ifndef DBSQLITE_WO_XEXEC

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param func defines hook function
			 * @param data defines data that will be passed to hook function
			 */
			virtual int addPostExec(inExec func, void *data);

			/**
			 * add hook before the operation
			 * @return id of the hook method
			 * @param func defines hook function
			 * @param data defines data that will be passed to hook function
			 */
			virtual int addPreExec(inExec func, void *data);

#ifdef DL_EXT

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPostExec(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * add hook after the operation
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 */
			virtual int addPreExec(const dodoString &path, void *data, void *toInit = NULL);

			/**
			 * set hook from the library that will be executed before/after the operation
			 * @return number in list where function is set
			 * @return id of the hook method
			 * @param path defines path to the library[if not in ldconfig db] or library name
			 * @param data defines data that will be passed to hook function
			 * @param toInit defines data that will be passed to the init function
			 * @note type of hook[pre/post] is defined in the library
			 */
			virtual __xexecCounts addExec(const dodoString &path, void *data, void *toInit = NULL);

#endif

#endif

		protected:

			/**
			 * construct adaptive field info for statements
			 */
			virtual dodoString fieldCollect(const __fieldInfo &row);

			/**
			 * execute request
			 * @param query defines query; you may define it if you don't use db methods like select, update
			 * @param result defines type of result; if true query return the result
			 * @note to insert|update using BLOB values use hint:
			 * 		make standart method calls to collect query, but instead of blob-values place $1 .. $n [identificators]
			 * 		call setBLOBValues method to set blob values according to id
			 * 		define hint class attribute as DBSQLITE_HINT_BLOB[you have to define it each time before this call]
			 * 		YOU MUST
			 * 				define preventFraming and preventEscaping as true
			 * 				by yourself escape[using dbSqlBase::escapeFields] and frame with '' non-blob text data before inserting/updating
			 * 				by yourself escape[using dbSqlBase::unescapeFields] non-blob text data after selecting
			 */
			virtual void _exec(const dodoString &query, bool result);

		private:

			sqlite3 *lite;                  ///< DB handle
			sqlite3_stmt *liteStmt;         ///< handlde to result

			bool empty;                     ///< true if liteStmt is empty

			dodoStringArray blobs;          ///< blob data
	};

};

#endif

#endif
