/***************************************************************************
 *            dbMysql.h
 *
 *  Fri Jan  13 19:25:19 2006
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

#ifndef _POSTGRESQL_H_
#define _POSTGRESQL_H_

#include <libdodo/directives.h>

#ifdef POSTGRESQL_EXT

#include <postgresql/libpq-fe.h>

#include <libdodo/tools.h>
#include <libdodo/dbPostgresqlEx.h>
#include <libdodo/dbSqlBase.h>
#include <libdodo/xexec.h>

namespace dodo
{

	/**
	 * @enum dbPostgresqlOperTypeEnum defines type of operation for hook
	 */
	enum dbPostgresqlOperTypeEnum
	{
		DBPOSTGRESQL_OPERATION_CONNECT,
		DBPOSTGRESQL_OPERATION_EXEC,
		DBPOSTGRESQL_OPERATION_DISCONNECT,
		DBPOSTGRESQL_OPERATION_FETCHROW,
		DBPOSTGRESQL_OPERATION_FETCHFIELD,
	};
	
	/**
	 * @enum dbPostgresqlHintEnum define DB hints
	 */
	enum dbPostgresqlHintEnum
	{
		DBPOSTGRESQL_HINT_NONE = 0,
		DBPOSTGRESQL_HINT_BLOB = 2,///< insert|update using BLOB values
	};
	
	/**
	 * @class dbPostgresql provides an interface to postgresql db
	 */
	class dbPostgresql : public dbSqlBase
	{
		private:

			/**
			 * constructor
			 * prevent copying
			 */
			dbPostgresql(dbPostgresql &a_pgpp);

		public:

			/**
			 * constructor
			 */
			dbPostgresql();

			/**
			 * destructor
			 */
			virtual ~dbPostgresql();

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
			 * 		define hint class attribute as DBPOSTGRESQL_HINT_BLOB[you have to define it each time before this call]
			 * 		YOU MUST
			 * 				define preventFraming and preventEscaping as true
			 * 				by yourself escape[using dbSqlBase::escapeFields] and frame with '' non-blob text data before inserting/updating
			 * 				by yourself escape[using dbSqlBase::unescapeFields] non-blob text data after selecting
			 * to select BLOB values use hint:
			 * 		define hint class attribute as DBPOSTGRESQL_HINT_BLOB[you have to define it each time before this call]
			 */
			virtual void exec(const dodoString &query = __dodostring__, bool result = false);
			
			unsigned short hint;///< DB hint[see dbPostgresqlHintEnum]

			/**
			 * set BLOB data for the request
			 * @param values defines blob-type values that will be applied for dodo:hint:db:blob instead of identificators
			 */
			virtual void setBLOBValues(const dodoStringArray &values);

#ifndef DBPOSTGRESQL_WO_XEXEC

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

			/**
			 * set sessions charset
			 * @param charset defines charset
			 */
			virtual void setCharset(const dodoString &charset);

			/**
			 * @return current session charset
			 * @note return value can be
			 * PG_SQL_ASCII[SQL/ASCII]
			 * PG_EUC_JP[EUC for Japanese]
			 * PG_EUC_CN[EUC for Chinese]
			 * PG_EUC_KR[EUC for Korean]
			 * PG_EUC_TW[EUC for Taiwan]
			 * PG_JOHAB[EUC for Korean JOHAB]
			 * PG_UTF8[Unicode UTF8]
			 * PG_LATIN1[ISO-8859-1 Latin 1]
			 * PG_LATIN2[ISO-8859-2 Latin 2]
			 * PG_LATIN3[ISO-8859-3 Latin 3]
			 * PG_LATIN4[ISO-8859-4 Latin 4]
			 * PG_LATIN5[ISO-8859-5 Latin 5]
			 * PG_LATIN6[ISO-8859-10 Latin 6]
			 * PG_LATIN7[ISO-8859-13 Latin 7]
			 * PG_LATIN8[ISO-8859-14 Latin 8]
			 * PG_LATIN9[ISO-8859-15 Latin 9]
			 * PG_LATIN10[ISO-8859-16 Latin 10]
			 * PG_WIN1256[windows-1256]
			 * PG_WIN1258[windows-1258]
			 * PG_WIN866[CP866]
			 * PG_WIN874[windows-874]
			 * PG_WIN1251[windows-1251]
			 * PG_WIN1252[windows-1252]
			 * PG_WIN1250[windows-1250]
			 * PG_WIN1253[windows-1253]
			 * PG_WIN1254[windows-1254]
			 * PG_WIN1255[windows-1255]
			 * PG_WIN1257[windows-1257]
			 * PG_KOI8R[KOI8-R]
			 * PG_ISO_8859_5[ISO-8859-5]
			 * PG_ISO_8859_6[ISO-8859-6]
			 * PG_ISO_8859_7[ISO-8859-7]
			 * PG_ISO_8859_8[ISO-8859-8]
			 * PG_SJIS[Shift JIS (Windows-932)]
			 * PG_BIG5[Big5 (Windows-950)]
			 * PG_GBK[GBK (Windows-936)]
			 * PG_UHC[UHC (Windows-949)]
			 * PG_GB18030[GB18030]
			 */
			virtual int getCharset() const;

		protected:

			/**
			 * @return SQL compliant data type
			 * @param type defines data type
			 */
			virtual dodoString sqlDataType(int type);
			
			/**
			 * construct `rename database` statement
			 */
			virtual void renameDbCollect();

			/**
			 * construct `alter table` statement
			 */
			virtual void renameFieldCollect();

			/**
			 * execute request
			 * @param query defines query; you may define it if you don't use db methods like select, update
			 * @param result defines type of result; if true query return the result
			 * @note to insert|update using BLOB values use hint:
			 * 		make standart method calls to collect query, but instead of blob-values place $1 .. $n [identificators]
			 * 		call setBLOBValues method to set blob values according to id
			 * 		define hint class attribute as DBPOSTGRESQL_HINT_BLOB[you have to define it each time before this call]
			 * 		YOU MUST
			 * 				define preventFraming and preventEscaping as true
			 * 				by yourself escape[using dbSqlBase::escapeFields] and frame with '' non-blob text data before inserting/updating
			 * 				by yourself escape[using dbSqlBase::unescapeFields] non-blob text data after selecting
			 * to select BLOB values use hint:
			 * 		define hint class attribute as DBPOSTGRESQL_HINT_BLOB[you have to define it each time before this call]
			 */
			virtual void _exec(const dodoString &query, bool result);

		private:

			bool empty;                 ///< true id pgResult is empty

			PGconn *conn;               ///< DB handle
			PGresult *pgResult;         ///< handlde to result

			dodoStringArray blobs;      ///< blob data
	};

};

#endif

#endif