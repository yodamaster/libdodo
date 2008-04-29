/***************************************************************************
 *            db.h
 *
 *  Sun Jan  15 19:45:19 2005
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

#ifndef _DBINTERFACE_H_
#define _DBINTERFACE_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/xexec.h>

namespace dodo
{
	/**
	 * @enum dbEmptyEnum defines empty state of the db object
	 */
	enum dbEmptyEnum
	{
		DB_EMPTY = 0
	};

	/**
	 * @enum dbBaseAddEnum defines additional properties for the request
	 */
	enum dbBaseAddEnum
	{
		DBBASE_ADDREQUEST_WHERE = 1,
		DBBASE_ADDREQUEST_HAVING,
		DBBASE_ADDREQUEST_GROUPBY,
		DBBASE_ADDREQUEST_ORDERBY,
		DBBASE_ADDREQUEST_LIMIT,
		DBBASE_ADDREQUEST_OFFSET,
		DBBASE_ADDREQUEST_AS,
		DBBASE_ADDREQUEST_JOIN,
	};
	
	/**
	 * @enum dbBaseRequestEnum defines type of request
	 */
	enum dbBaseRequestEnum
	{
		DBBASE_REQUEST_UNION = 1,
		DBBASE_REQUEST_UNION_ALL,
		DBBASE_REQUEST_MINUS,
		DBBASE_REQUEST_INTERSECT,
	
		DBBASE_REQUEST_SELECT,
		DBBASE_REQUEST_INSERT,
		DBBASE_REQUEST_INSERT_SELECT,
		DBBASE_REQUEST_UPDATE,
		DBBASE_REQUEST_DELETE,
	
		DBBASE_REQUEST_TRUNCATE,
	
		DBBASE_REQUEST_RENAME_DB,
		DBBASE_REQUEST_RENAME_TABLE,
		DBBASE_REQUEST_RENAME_FIELD,
	
		DBBASE_REQUEST_DELETE_DB,
		DBBASE_REQUEST_DELETE_TABLE,
		DBBASE_REQUEST_DELETE_FIELD,
	
		DBBASE_REQUEST_CREATE_DB,
		DBBASE_REQUEST_CREATE_TABLE,
		DBBASE_REQUEST_CREATE_FIELD,
	
		DBBASE_REQUEST_CREATE_INDEX,
		DBBASE_REQUEST_DELETE_INDEX,
	
		DBBASE_REQUEST_CALL_FUNCTION,
		DBBASE_REQUEST_CALL_PROCEDURE,
	};
	
	/**
	 * @enum dbBaseAddSelEnum defines additional properties for the `select` request
	 */
	enum dbBaseAddSelEnum
	{
		DBBASE_ADDREQUEST_SELECT_DISTINCT = 1,
		DBBASE_ADDREQUEST_SELECT_ALL
	};
	
	/**
	 * @enum dbBaseAddDelEnum defines additional properties for the `delete` request
	 */
	enum dbBaseAddDelEnum
	{
		DBBASE_ADDREQUEST_DELETE_IGNORE = 1,
	};
	
	/**
	 * @enum dbBaseAddUpEnum defines additional properties for the `update` request
	 */
	enum dbBaseAddUpEnum
	{
		DBBASE_ADDREQUEST_UPDATE_IGNORE = 1,
	};
	
	/**
	 * @enum dbBaseAddInsEnum defines additional properties for the `insert` request
	 */
	enum dbBaseAddInsEnum
	{
		DBBASE_ADDREQUEST_INSERT_IGNORE = 1,
	};
	
	/**
	 * @enum dbBaseFieldTypeEnum defines data types
	 * @note with data marked with '**' may have length
	 * with data marked with '**' needs to specify length
	 */
	enum dbBaseFieldTypeEnum
	{
		DBBASE_FIELDTYPE_TINYINT,                           ///< *The signed range is -128 to 127. The unsigned range is 0 to 255
		DBBASE_FIELDTYPE_SMALLINT,                          ///< *The signed range is -32768 to 32767. The unsigned range is 0 to 65535
		DBBASE_FIELDTYPE_MEDIUMINT,                         ///< *The signed range is -8388608 to 8388607. The unsigned range is 0 to 16777215
		DBBASE_FIELDTYPE_INTEGER, DBBASE_FIELDTYPE_INT,     ///< *The signed range is -2147483648 to 2147483647. The unsigned range is 0 to 4294967295
		DBBASE_FIELDTYPE_BIGINT,                            ///< *The signed range is -9223372036854775808 to 9223372036854775807. The unsigned range is 0 to 18446744073709551615
		DBBASE_FIELDTYPE_FLOAT,                             ///< *Allowable values are -3.402823466E+38 to -1.175494351E-38, 0, and 1.175494351E-38 to 3.402823466E+38
		DBBASE_FIELDTYPE_REAL, DBBASE_FIELDTYPE_DOUBLE,     ///< *[DOUBLE in some systems] Allowable values are -1.7976931348623157E+308 to -2.2250738585072014E-308, 0, and 2.2250738585072014E-308 to 1.7976931348623157E+308
		DBBASE_FIELDTYPE_DECIMAL,                           ///< **An unpacked(the number is stored as a string) fixed-point number
		DBBASE_FIELDTYPE_DATE,                              ///< The supported range is '1000-01-01' to '9999-12-31'
		DBBASE_FIELDTYPE_TIME,                              ///< The range is '-838:59:59' to '838:59:59'
		DBBASE_FIELDTYPE_TIMESTAMP,                         ///< *The range is '1970-01-01 00:00:00' to partway through the year 2037. The first TIMESTAMP column in a table is automatically set to the date and time of the most recent operation if you don't assign it a value yourself
		/**
		 * TIMESTAMP(14) 	YYYYMMDDHHMMSS
		 * TIMESTAMP(12) 	YYMMDDHHMMSS
		 * TIMESTAMP(10) 	YYMMDDHHMM
		 * TIMESTAMP(8) 		YYYYMMDD
		 * TIMESTAMP(6) 		YYMMDD
		 * TIMESTAMP(4) 		YYMM
		 * TIMESTAMP(2) 		YY
		 */
		DBBASE_FIELDTYPE_CHAR,                              ///< **The range of M is 0 to 255 characters; A fixed-length string that is always right-padded with spaces to the specified length when stored
		DBBASE_FIELDTYPE_VARCHAR,                           ///< **The range of M is 0 to 255 characters. A variable-length string. Range represents the maximum column length
		DBBASE_FIELDTYPE_TINYBLOB,                          ///< A column with a maximum length of 255 (2^8 - 1) characters
		DBBASE_FIELDTYPE_BLOB,                              ///< A column with a maximum length of 65, 535 (2^16 -1) characters
		DBBASE_FIELDTYPE_MEDIUMBLOB,                        ///< A column with a maximum length of 16, 777, 215 (2^24 - 1) characters
		DBBASE_FIELDTYPE_LONGBLOB,                          ///< A column with a maximum length of 4, 294, 967, 295 or 4GB (2^32 - 1) characters
		DBBASE_FIELDTYPE_TINYTEXT,                          ///< A column with a maximum length of 255 (2^8 - 1) characters
		DBBASE_FIELDTYPE_TEXT,                              ///< A column with a maximum length of 65, 535 (2^16 -1) characters
		DBBASE_FIELDTYPE_MEDIUMTEXT,                        ///< A column with a maximum length of 16, 777, 215 (2^24 - 1) characters
		DBBASE_FIELDTYPE_LONGTEXT,                          ///< A column with a maximum length of 4, 294, 967, 295 or 4GB (2^32 - 1) characters
		DBBASE_FIELDTYPE_ENUM,                              ///< An enumeration. A string object that can have only one value, chosen from the list of values 'value1', 'value2', ..., NULL or the special '' error value. An column can have a maximum of 65, 535 distinct values
		DBBASE_FIELDTYPE_SET,                               ///< A string object that can have zero or more values, each of which must be chosen from the list of values 'value1', 'value2', ... A column can have a maximum of 64 members
	};
	
	/**
	 *  @enum dbBaseFieldFlagEnum defines additional field properties
	 */
	enum dbBaseFieldFlagEnum
	{
		DBBASE_FIELDFLAG_NULL = 2,              ///< may be NULL
		DBBASE_FIELDFLAG_AUTO_INCREMENT = 4,    ///< if is not set by request, will be incremented relatevly to previous
	};
	
	/**
	 * @enum dbBaseReferenceEnum defines reference type assigned to the field
	 */
	enum dbBaseReferenceEnum
	{
		DBBASE_REFERENCE_RESTRICT = 1,  ///< does not allow the action of any of those parent rows
		DBBASE_REFERENCE_CASCADE,       ///< a row in the parent table is deleted, automatically deletes also all those rows in the child table whose foreign key values are equal to the referenced key value in the parent row
		DBBASE_REFERENCE_SET_NULL,      ///< sets NULL on the action of any of those parent rows indicates on set action
		DBBASE_REFERENCE_NO_ACTION,     ///< noacton on the action of any of those parent rows indicates on set action
		DBBASE_REFERENCE_SET_DEFAULT    ///< sets default on the action of any of those parent rows indicates on set action
	};
	
	/**
	 * @struct __fieldInfo defines data for field creation
	 */
	struct __fieldInfo
	{
		/**
		 * constructor
		 */
		__fieldInfo();
	
		/**
		 * overloaded operator '='
		 * @param from defines object where get properties
		 */
		const __fieldInfo &operator=(const __fieldInfo &from);
	
		dodoString name;    ///< name of the field
		int type;           ///< type of field[see dbBaseFieldTypeEnum]
		int length;         ///< length of the field[see dbBaseFieldTypeEnum notes]
		int flag;           ///< flags of the field; may be combined with '|'[see dbBaseFieldFlagEnum][DBBASE_FIELDFLAG_NULL by default]
	
		dodoString refTable;            ///< table where reference points
		dodoStringArray refFields;      ///< fields where reference points
		int onDelete;                   ///< reference action on delete operation[see dbReferenceEnum]
		int onUpdate;                   ///< reference action on update operation[see dbReferenceEnum]
	
		dodoString defaultVal;          ///< default value of the field
		dodoStringArray set_enum;       ///< statements for SET or ENUM types
	
		dodoString charset;             ///< collate charset of the field
	};
	
	/*
	 * @struct __tableInfo defines data for table creation
	 */
	struct __tableInfo
	{
		/**
		 * constructor
		 */
		__tableInfo();
	
		/**
		 * overloaded operator '='
		 * @param from defines object where get properties
		 */
		const __tableInfo &operator=(const __tableInfo &from);
	
		dodoString name;                    ///< name of the table
		dodoArray<__fieldInfo> fields;      ///< fields[see __fieldInfo]
	
		dodoStringArray primKeys;           ///< primary keys of the table
		dodoStringArray uniq;               ///< fields of the table that may contain only unique values
	
		bool ifNotExists;                   ///< do not throw exception if table already exists[false by default]
	};

	/**
	 * @struct __dbStorage defines fetched data from db
	 */
	struct __dbStorage
	{
		/**
		 * constructor
		 * @param rows defines rows of data
		 * @param fields defines names of fields
		 */
		__dbStorage(dodoArray<dodoStringArray> rows, dodoStringArray fields);
		
		/**
		 * constructor
		 */
		__dbStorage();

		dodoArray<dodoStringArray> rows;    ///< rows of data
		dodoStringArray fields;             ///< names of fields
	};

	/**
	 * @struct __sqlInfo defines connection options for the server
	 */
	struct __dbInfo
	{

		/**
		 * constructor
		 */
		__dbInfo();

		/**
		 * constructor
		 * @param db defines name of db
		 * @param host defines host
		 * @param user defines user
		 * @param password defines password
		 * @param path defines path to db or unix socket
		 * @param port defines port
		 */
		__dbInfo(const dodoString &db, const dodoString &host, const dodoString &user, const dodoString &password, const dodoString &path, int port);

		dodoString db;          ///< database name
		dodoString host;        ///< hostname
		dodoString user;        ///< username
		dodoString password;    ///< password
		dodoString path;        ///< path to db or unix socket
		unsigned int port;      ///< port
	};

	
	/**
	 * @enum dbJoinTypeEnum defines join types
	 */
	enum dbJoinTypeEnum
	{
		DB_JOINTYPE_JOIN,
		DB_JOINTYPE_LEFTOUTER,
		DB_JOINTYPE_RIGHTOUTER,
		DB_JOINTYPE_FULLOUTER,
		DB_JOINTYPE_INNER,
		DB_JOINTYPE_CROSS,
	};

	/**
	 * @class db implements an interface to db through sql and database independent interfaces
	 */
	class db : public xexec
	{
		public:

			/**
			 *constructor
			 */
			db();

			/**
			 * destructor
			 */
			virtual ~db() = 0;

			/**
			 * connect to the database
			 */
			virtual void connect() = 0;

			/**
			 * disconnect from the database
			 */
			virtual void disconnect() = 0;
			
			/**
			 * set connection information for database
			 * @param db defines name of db
			 * @param host defines host
			 * @param user defines user
			 * @param password defines password
			 * @param path defines path to db or unix socket
			 * @param port defines port
			 */
			virtual void setDbInfo(const dodoString &db, const dodoString &host, unsigned int port, const dodoString &user, const dodoString &password, const dodoString &path = __dodostring__) = 0;

			/**
			 * set connection information for database
			 * @param dbInfo defines connection information
			 */
			virtual void setDbInfo(const __dbInfo &dbInfo) = 0;

			/**
			 * call stored function
			 * @param name defines function name
			 * @param arguments defines arguments
			 * @param as defines name of the result row
			 */
			virtual void callFunction(const dodoString &name, const dodoStringArray &arguments, const dodoString &as = __dodostring__) = 0;

			/**
			 * call stored procedure
			 * @param name is procedure name
			 * @param arguments is array of arguments
			 */
			virtual void callProcedure(const dodoString &name, const dodoStringArray &arguments) = 0;
			
			/**
			 * @param table defines table name
			 * @param fields defines names of fields
			 * @param where defines `where` statement
			 * @note if (length(table) == 0) => 'from `table`' is not used
			 */
			virtual void select(const dodoString &table, const dodoStringArray &fields, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table defines table name
			 * @param where defines `where` statement
			 */
			virtual void selectAll(const dodoString &table, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table defines table name
			 * @param fields defines hash of field=>value
			 */
			virtual void insert(const dodoString &table, const dodoStringMap &fields) = 0;

			/**
			 * @param table defines table name
			 * @param fields defines set of hashes of field=>value
			 */
			virtual void insert(const dodoString &table, const dodoArray<dodoStringMap> &fields) = 0;

			/**
			 * @param table defines table name
			 * @param values defines values
			 * @param fields defines names of fields
			 */
			virtual void insert(const dodoString &table, const dodoStringArray &values, const dodoStringArray &fields = __dodostringarray__) = 0;

			/**
			 * @param table defines table name
			 * @param values defines values
			 * @param fields defines names of fields
			 */
			virtual void insert(const dodoString &table, const dodoArray<dodoStringArray> &values, const dodoStringArray &fields = __dodostringarray__) = 0;

			/**
			 * @param tableTo defines table where data will be stored
			 * @param tableFrom defines table where data will be fetched
			 * @param fieldsTo defines fields where data will be stored
			 * @param fieldsFrom defines fields that will be fetched
			 * @param where defines `where` statement
			 */
			virtual void insertSelect(const dodoString &tableTo, const dodoString &tableFrom, const dodoStringArray &fieldsTo, const dodoStringArray &fieldsFrom = __dodostringarray__, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table defines table name
			 * @param fields defines hash of field=>value
			 * @param where defines `where` statement
			 */
			virtual void update(const dodoString &table, const dodoStringMap &fields, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table defines table name
			 * @param values defines values
			 * @param fields defines names of fields
			 * @param where defines `where` statement
			 */
			virtual void update(const dodoString &table, const dodoStringArray &values, const dodoStringArray &fields, const dodoString &where = __dodostring__) = 0;

			/**
			 * @param table defines table name
			 * @param where defines `where` statement
			 */
			virtual void del(const dodoString &table, const dodoString &where = __dodostring__) = 0;

			/**
			 * store query, made from subquery with requested method
			 * @param subqueries defines subqueries
			 * @param type defines type of combining subqueries[see dbBaseRequestEnum]
			 */
			virtual void subquery(const dodoStringArray &subqueries, int type = DBBASE_REQUEST_UNION) = 0;

			/**
			 * create index in the table
			 * @param table defines table where create index
			 * @param field defines field that would be index
			 * @param name defines name of the index
			 */
			virtual void createIndex(const dodoString &table, const dodoString &field, const dodoString &name) = 0;

			/**
			 * create index in the table
			 * @param table defines table where create index
			 * @param fields defines fields that would be index
			 * @param name defines name of the index
			 */
			virtual void createIndex(const dodoString &table, const dodoStringArray &fields, const dodoString &name) = 0;

			/**
			 * delete index in the table
			 * @param table defines table where delete index
			 * @param name defines name of the index
			 */
			virtual void deleteIndex(const dodoString &table, const dodoString &name) = 0;

			/**
			 * rename database
			 * @param db defines current name of the database
			 * @param to_db defines new name of the database
			 */
			virtual void renameDb(const dodoString &db, const dodoString &to_db) = 0;

			/**
			 * rename table
			 * @param table defines current name of the table
			 * @param to_table defines new name of the table
			 */
			virtual void renameTable(const dodoString &table, const dodoString &to_table) = 0;

			/**
			 * rename field
			 * @param field defines current name of the field
			 * @param to_field defines new name of the field
			 * @param table defines table that contains the field
			 */
			virtual void renameField(const dodoString &field, const dodoString &to_field, const dodoString &table) = 0;

			/**
			 * delete database
			 * @param db defines the database
			 */
			virtual void deleteDb(const dodoString &db) = 0;

			/**
			 * delete table
			 * @param table defines the table
			 */
			virtual void deleteTable(const dodoString &table) = 0;

			/**
			 * delete field
			 * @param field defines field that will be deleted
			 * @param table defines table that contains the field
			 */
			virtual void deleteField(const dodoString &field, const dodoString &table) = 0;

			/**
			 * create database
			 * @param db the name of the new database
			 * @param charset defines charset of the database
			 */
			virtual void createDb(const dodoString &db, const dodoString &charset = __dodostring__) = 0;

			/**
			 * create table
			 * @param tableInfo defines table definition[see __tableInfo]
			 */
			virtual void createTable(const __tableInfo &tableInfo) = 0;

			/**
			 * create field
			 * @param fieldInfo defines field definition[see __fieldInfo]
			 * @param table defines table that will contain the field
			 */
			virtual void createField(const __fieldInfo &fieldInfo, const dodoString &table) = 0;

			/**
			 * truncate table
			 * @param table is name name of table to truncate
			 */
			virtual void truncate(const dodoString &table) = 0;

			/**
			 * set `where` statement
			 * @param where defines `where` statement
			 * @note overwrites previous definition[from methods that may define it]
			 */
			virtual void where(const dodoString &where) = 0;

			/**
			 * set `limit` property
			 * @param number defines `limit` value
			 */
			virtual void limit(unsigned int number) = 0;

			/**
			 * set `offset` property
			 * @param number defines `offset` value
			 */
			virtual void offset(unsigned int number) = 0;

			/**
			 * set `order` property
			 * @param order defines `order` value
			 */
			virtual void order(const dodoString &order) = 0;

			/**
			 * set `group` property
			 * @param group defines `group` value
			 */
			virtual void group(const dodoString &group) = 0;

			/**
			 * set having property
			 * @param having defines having value
			 */
			virtual void having(const dodoString &having) = 0;

			/**
			 * append join statement
			 * @param table defines table to join
			 * @param condition defines condition for joining
			 * @param type defines join type[see dbBaseJoinTypeEnum]
			 */
			virtual void join(const dodoString &table, int type, const dodoString &condition) = 0;

			/**
			 * remove `where` statement
			 */
			virtual void unwhere() = 0;

			/**
			 * remove `limit` property
			 */
			virtual void unlimit() = 0;

			/**
			 * remove `offset` property
			 */
			virtual void unoffset() = 0;

			/**
			 * remove `order` property
			 */
			virtual void unorder() = 0;

			/**
			 * remove `group` property
			 */
			virtual void ungroup() = 0;

			/**
			 * remove `having` property
			 */
			virtual void unhaving() = 0;

			/**
			 * @return amount of affected rows
			 */
			virtual unsigned int affectedRowsCount() const = 0;

			/**
			 * @return amount of rows got from the request
			 */
			virtual unsigned int rowsCount() const = 0;

			/**
			 * @return amount of fields got from the request
			 */
			virtual unsigned int fieldsCount() const = 0;

			/**
			 * @return rows got from the request
			 */
			virtual dodoArray<dodoStringArray> fetchRow() const = 0;

			/**
			 * @return fields got from the request
			 */
			virtual dodoStringArray fetchField() const = 0;

			/**
			 * @return result got from the request
			 */
			virtual __dbStorage fetch() const = 0;

			/**
			 * @return set of hashes of field=>value
			 */
			virtual dodoStringMapArray fetchAssoc() const = 0;

			/**
			 * execute collected request
			 * @param query contains query for DB
			 * @param result defines type of result(true for quering data, false for data operation)
			 * @note if query is empty request will be constructed from the data that was defined using object's methods 
			 */
			virtual void exec(const dodoString &query = __dodostring__, bool result = false) = 0;

			bool reconnect; ///< if true tries to reconect in case when `exec` failed with connection error[true by default]

		protected:

			bool connected; ///< true if connected to the db

	};

};

#endif
