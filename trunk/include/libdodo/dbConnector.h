/***************************************************************************
 *            dbConnector.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _DBCONNECTOR_H_
#define _DBCONNECTOR_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/xexec.h>

namespace dodo
{
	namespace db
	{
#define CONNECTOR_JOINTYPESTSTATEMENTS 6

		/**
		 * @enum connectorJoinTypeEnum defines join types
		 */
		enum connectorJoinTypeEnum
		{
			CONNECTOR_JOINTYPE_JOIN,
			CONNECTOR_JOINTYPE_LEFTOUTER,
			CONNECTOR_JOINTYPE_RIGHTOUTER,
			CONNECTOR_JOINTYPE_FULLOUTER,
			CONNECTOR_JOINTYPE_INNER,
			CONNECTOR_JOINTYPE_CROSS,
		};

#define CONNECTOR_SUBREQUESTSTATEMENTS 4

		/**
		 * @enum connectorSubrequestEnum defines type of sub request
		 */
		enum connectorSubrequestEnum
		{
			CONNECTOR_SUBREQUEST_UNION = 1,
			CONNECTOR_SUBREQUEST_UNION_ALL,
			CONNECTOR_SUBREQUEST_MINUS,
			CONNECTOR_SUBREQUEST_INTERSECT,
		};

		/**
		 * @struct __connectorStorage defines fetched data from db
		 */
		struct __connectorStorage
		{
			/**
			 * constructor
			 * @param rows defines rows of data
			 * @param fields defines names of fields
			 */
			__connectorStorage(dodoArray<dodoStringArray> rows, dodoStringArray fields);

			/**
			 * constructor
			 */
			__connectorStorage();

			dodoArray<dodoStringArray> rows;                    ///< rows of data
			dodoStringArray fields;                             ///< names of fields
		};

		/**
		 * @struct __sqlInfo defines connection options for the server
		 */
		struct __connectorInfo
		{

			/**
			 * constructor
			 */
			__connectorInfo();

			/**
			 * constructor
			 * @param db defines name of db
			 * @param host defines host
			 * @param user defines user
			 * @param password defines password
			 * @param path defines path to db or unix socket
			 * @param port defines port
			 */
			__connectorInfo(const dodoString &db, const dodoString &host, const dodoString &user, const dodoString &password, const dodoString &path, int port);

			dodoString db;                      ///< database name
			dodoString host;                    ///< hostname
			dodoString user;                    ///< username
			dodoString password;                ///< password
			dodoString path;                    ///< path to db or unix socket
			unsigned int port;                  ///< port
		};

		/**
		 * @enum dbOperationTypeEnum defines type of operation for hook
		 */
		enum dbOperationTypeEnum
		{
			DB_OPERATION_CONNECT,
			DB_OPERATION_EXEC,
			DB_OPERATION_DISCONNECT,
			DB_OPERATION_FETCHROW,
			DB_OPERATION_FETCHFIELD,
		};

		/**
		 * @class connector implements an interface to db through sql and database independent interfaces
		 */
		class connector
#ifndef DB_WO_XEXEC
						: public xexec
#endif
		{
			public:

				/**
				 *constructor
				 */
				connector();

				/**
				 * destructor
				 */
				virtual ~connector() = 0;

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
				 * @param dbInfo defines connection information
				 */
				virtual void setDbInfo(const __connectorInfo &dbInfo) = 0;

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
				 * @param type defines type of combining subqueries[see connectorSubrequestEnum]
				 */
				virtual void subquery(const dodoStringArray &subqueries, int type = CONNECTOR_SUBREQUEST_UNION) = 0;

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
				 * @param type defines join type[see connectorJoinTypeEnum]
				 */
				virtual void join(const dodoString &table, int type, const dodoString &condition) = 0;

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
				virtual dodoArray<dodoStringArray> fetchRows() const = 0;

				/**
				 * @return fields got from the request
				 */
				virtual dodoStringArray fetchFields() const = 0;

				/**
				 * @return result got from the request
				 */
				virtual __connectorStorage fetch() const = 0;

				/**
				 * @return set of hashes of field=>value
				 */
				virtual dodoStringMapArray fetchFieldsToRows() const = 0;

				/**
				 * execute collected request
				 * @param query contains query for DB
				 * @param result defines type of result(true for quering data, false for data operation)
				 * @note if query is empty request will be constructed from the data that was defined using object's methods
				 */
				virtual void exec(const dodoString &query = __dodostring__, bool result = false) = 0;

				bool reconnect;                 ///< if true tries to reconect in case when `exec` failed with connection error[true by default]

			protected:

				bool connected;                 ///< true if connected to the db
		};
	};
};

#endif
