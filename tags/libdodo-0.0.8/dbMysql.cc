/***************************************************************************
 *            dbMysql.cc
 *
 *  Thu Apr  30 13:45:19 2005
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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  aint with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef MYSQL_EXT

	#include "dbMysql.h"
	
	using namespace dodo;

	dodoBase * const 
	dbMysql::getSelf()
	{
		return dynamic_cast<dodoBase *>(this);
	}
	
	//-------------------------------------------------------------------

	
	dbMysql::dbMysql() : connected(false),
						empty(true)
	{
		mysql = mysql_init(NULL);
		addSQL();
	}
	
	//-------------------------------------------------------------------
	
	dbMysql::dbMysql(dbMysql &a_mypp)
	{
	}
	
	//-------------------------------------------------------------------
	
	dbMysql::~dbMysql()
	{
		if (!empty)
			mysql_free_result(mysqlRes);
		disconnect();
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::addSQL()
	{
		sqlDbDepAddInsArr.push_back(" delayed ");
		sqlDbDepAddInsArr.push_back(" low_priority ");
		sqlDbDepAddInsArr.push_back(" high_priority ");
		
		sqlDbDepAddSelArr.push_back(" straight_join ");
		sqlDbDepAddSelArr.push_back(" sql_small_result ");
		sqlDbDepAddSelArr.push_back(" sql_big_result ");
		
		sqlDbDepAddUpArr.push_back(" low_priority ");
	
		sqlDbDepAddDelArr.push_back(" low_priority ");
		sqlDbDepAddDelArr.push_back(" quick ");	
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::setMyAddInsSt(mysqlAddInsEnum statement)
	{
		/*switch (statement)
		{
			break;
			default:
				break;
			
		}*/
		addF(qDbDepInsShift,1<<statement);
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::setMyAddUpSt(mysqlAddUpEnum statement)
	{
		/*switch (statement)
		{
			default:
				break;
			
		}*/
		addF(qDbDepUpShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::setMyAddSelSt(mysqlAddSelEnum statement)
	{
		switch (statement)
		{
			case SELECT_DISTINCT:
			case SELECT_ALL:
				removeF(qSelShift,2<<(SELECT_ALL-1));
				removeF(qSelShift,2<<(SELECT_DISTINCT-1));
				break;
			default:
				break;
			
		}
		addF(qDbDepSelShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::setMyAddDelSt(mysqlAddDelEnum statement)
	{
		/*switch (statement)
		{
			default:
				break;
			
		}*/
		addF(qDbDepDelShift,1<<statement);
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::unsetMyAddInsSt(mysqlAddInsEnum statement)
	{
		removeF(qDbDepInsShift,1<<statement);
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::unsetMyAddUpSt(mysqlAddUpEnum statement)
	{
		removeF(qDbDepUpShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::unsetMyAddSelSt(mysqlAddSelEnum statement)
	{
		removeF(qDbDepSelShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::unsetMyAddDelSt(mysqlAddDelEnum statement)
	{
		removeF(qDbDepDelShift,1<<statement);	
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbMysql::connect(unsigned long type) const
	{
		if (connected)
			disconnect();
			
		#ifndef DBMYSQL_WO_XEXEC
			operType = DBMYSQL_OPER_CONNECT;
			performXExec(preExec);
		#endif
		
		if (!mysql_real_connect(mysql,
			sqlInfo.host.size()==0?NULL:sqlInfo.host.c_str(),
			sqlInfo.user.size()==0?NULL:sqlInfo.user.c_str(),
			sqlInfo.password.size()==0?NULL:sqlInfo.password.c_str(),
			sqlInfo.db.size()==0?NULL:sqlInfo.db.c_str(),
			sqlInfo.port,
			sqlInfo.socket.size()==0?NULL:sqlInfo.socket.c_str(),
			type))
		{
			connected = false;
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBMYSQL,DBMYSQL_CONNECT,ERR_MYSQL,mysql_errno(mysql),mysql_error(mysql),__LINE__,__FILE__);
			#else
				return false;
			#endif
		}
		
		#ifndef DBMYSQL_WO_XEXEC
			performXExec(postExec);
		#endif
		
		connected = true;
		
		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbMysql::disconnect() const
	{
		if (connected)
		{
			#ifndef DBMYSQL_WO_XEXEC
				operType = DBMYSQL_OPER_DISCONNECT;
				performXExec(preExec);
			#endif
			
	     	mysql_close(mysql);

			#ifndef DBMYSQL_WO_XEXEC
				performXExec(preExec);
			#endif
	     	
		 	connected = false;
		}
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbMysql::_exec() const
	{	
		queryCollect();
		
		if (mysql_real_query(mysql,request.c_str(),request.size()) != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBMYSQL,DBMYSQL_CONNECT,ERR_MYSQL,mysql_errno(mysql),mysql_error(mysql),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		if (!show)	
			#ifndef NO_EX
				return ;
			#else
				return true;
			#endif
		
		if (!empty)
			mysql_free_result(mysqlRes);
		empty = false;
		
		mysqlRes = mysql_store_result(mysql);
		if (mysqlRes == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBMYSQL,DBMYSQL_CONNECT,ERR_MYSQL,mysql_errno(mysql),mysql_error(mysql),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	std::vector<stringArr>
	dbMysql::fetchRow()
	{	
		register unsigned  numFields = mysql_num_fields(mysqlRes);	
		
		std::vector<stringArr> rows;
		rows.reserve(mysql_num_rows(mysqlRes));
		
		stringArr row;
		row.reserve(numFields);
		
		std::string rowPart;
		
		register unsigned long *length, j;
		while ((mysqlRow = mysql_fetch_row(mysqlRes)) != NULL)
		{		
			length = mysql_fetch_lengths(mysqlRes);
			row.clear();
			for (j=0;j<numFields;j++)
			{
				rowPart.assign(mysqlRow[j]?mysqlRow[j]:"NULL",mysqlRow[j]?length[j]:4);
				row.push_back(rowPart);
			}
			rows.push_back(row);
		}
	
		return rows;
	}
	
	//-------------------------------------------------------------------
	
	stringArr
	dbMysql::fetchField()
	{	
		unsigned int numFields = mysql_num_fields(mysqlRes);	
		mysqlFields = mysql_fetch_fields(mysqlRes);
		
		stringArr fields;
		fields.reserve(numFields);
		
		for (register unsigned int i(0);i<numFields;i++)
			fields.push_back(mysqlFields[i].name);
		
		return fields;
	}
	
	//-------------------------------------------------------------------
	
	__sqlStorage 
	dbMysql::fetch()
	{
		return __sqlStorage(fetchRow(), fetchField());
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	dbMysql::rowsCount()
	{
		return mysql_num_rows(mysqlRes);
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	dbMysql::fieldsCount()
	{
		return mysql_num_fields(mysqlRes);
	}
	
	//-------------------------------------------------------------------
	
	unsigned int
	dbMysql::affectedRowsCount()
	{
		return mysql_affected_rows(mysql);
	}
	
	//-------------------------------------------------------------------
	

	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbMysql::exec() const
	{
		#ifndef DBMYSQL_WO_XEXEC
			operType = DBMYSQL_OPER_EXEC;
			performXExec(preExec);
		#endif
		
		#ifdef NO_EX
			bool result = 
		#endif	
			_exec(); 
		
		#ifndef DBMYSQL_WO_XEXEC		
			performXExec(postExec);
		#endif
		
		cleanCollect();
		
		#ifdef NO_EX
			return result;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	int 
	dbMysql::addPostExec(inExec func, 
						void *data) const
	{
		return _addPostExec(func, (dodoBase *)this, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	dbMysql::addPreExec(inExec func, 
						void *data) const
	{
		return _addPreExec(func, (dodoBase *)this, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	dbMysql::addPostExec(const std::string &module, 
						void *data) const
	{
		return _addPostExec(module, (dodoBase *)this, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	dbMysql::addPreExec(const std::string &module, 
						void *data) const
	{
		return _addPreExec(module, (dodoBase *)this, data);
	}

	//-------------------------------------------------------------------
	
	int 
	dbMysql::addExec(const std::string &module, 
						void *data) const
	{
		return _addExec(module, (dodoBase *)this, data);
	}
	
	
	//-------------------------------------------------------------------
	
#endif