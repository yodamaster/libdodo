/***************************************************************************
 *            dbSqlite.cc
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

#include <dbSqlite.h>

#ifdef SQLITE_EXT

	#include <tools.h>
	
	using namespace dodo;

	
	dbSqlite::dbSqlite() : empty(true)
	{
		auto_increment = " autoincrement ";
	}
	
	//-------------------------------------------------------------------
	
	dbSqlite::dbSqlite(dbSqlite &a_pp)
	{
	}
	
	//-------------------------------------------------------------------
	
	dbSqlite::~dbSqlite()
	{
		if (connected)
		{
			if (!empty)
				sqlite3_finalize(liteStmt);
				
			sqlite3_close(lite);
		}
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbSqlite::connect() const
	{
		if (connected)
			disconnect();
			
		#ifndef DBSQLITE_WO_XEXEC
			operType = DBSQLITE_OPER_CONNECT;
			performXExec(preExec);
		#endif
		
		if (sqlite3_open(dbInfo.path.c_str(),&lite) != SQLITE_OK)
		{
			sqlite3_close(lite);
			
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBSQLITE,DBSQLITE_CONNECT,ERR_SQLITE,sqlite3_errcode(lite),sqlite3_errmsg(lite),__LINE__,__FILE__);
			#else
				return false;
			#endif		
		}
		
		#ifndef DBSQLITE_WO_XEXEC
			performXExec(postExec);
		#endif
		
		connected = true;
		
		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbSqlite::disconnect() const
	{
		if (connected)
		{
			#ifndef DBSQLITE_WO_XEXEC
				operType = DBSQLITE_OPER_DISCONNECT;
				performXExec(preExec);
			#endif
		
			if (!empty)
			{
				empty = true;
				sqlite3_finalize(liteStmt);
			}
						
			if (sqlite3_close(lite) != SQLITE_OK)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_DBSQLITE,DBSQLITE_DISCONNECT,ERR_SQLITE,sqlite3_errcode(lite),sqlite3_errmsg(lite),__LINE__,__FILE__);
				#else
					return ;
				#endif

			#ifndef DBSQLITE_WO_XEXEC
				performXExec(postExec);
			#endif
	     	
		 	connected = false;
		}
	}
	
	//-------------------------------------------------------------------
	
	void 
	dbSqlite::setBLOBValues(const stringArr &values)
	{
		blobs = values;
	}
	
	//-------------------------------------------------------------------
	
	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbSqlite::_exec(const std::string &query, 
					bool result) const
	{
		if (query.size() == 0)
		{
			queryCollect();
			
			blobHint = false;
		}
		else
		{
			if (strstr(query.c_str(),"dodo:hint:db:blob") != NULL)
			{
				queryCollect();
				
				if (!show)
					blobHint = true;
			}
			else
			{
				request = query;
				show = result;
				
				blobHint = false;
			}
		}

		if (!empty)
		{
			sqlite3_finalize(liteStmt);
			empty = true;
		}			
			
		if (sqlite3_prepare(lite,request.c_str(),request.size(), &liteStmt, NULL) != SQLITE_OK)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBSQLITE,DBSQLITE__EXEC,ERR_SQLITE,sqlite3_errcode(lite),sqlite3_errmsg(lite),__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		if (blobHint)
		{
			switch (qType)
			{
				case DBREQUEST_UPDATE:
				case DBREQUEST_INSERT:
				
					{
						stringArr::iterator i(blobs.begin()), j(blobs.end());
						for (register int o=1;i!=j;++i,++o)
							if (sqlite3_bind_blob(liteStmt, o, i->c_str(), i->size(), SQLITE_TRANSIENT) != SQLITE_OK)
								#ifndef NO_EX
									throw baseEx(ERRMODULE_DBSQLITE,DBSQLITE__EXEC,ERR_SQLITE,sqlite3_errcode(lite),sqlite3_errmsg(lite),__LINE__,__FILE__);
								#else
									return false;
								#endif
					}	
										
					break;
					
				default:
					
					#ifndef NO_EX
						throw baseEx(ERRMODULE_DBSQLITE,DBSQLITE__EXEC,ERR_LIBDODO,DBSQLITE_WRONG_HINT_USAGE,DBSQLITE_WRONG_HINT_USAGE_STR,__LINE__,__FILE__);
					#else
						return false;
					#endif
					
			}
		}	
		
		if (liteStmt == NULL)	
			#ifndef NO_EX
				throw baseEx(ERRMODULE_DBSQLITE,DBSQLITE__EXEC,ERR_SQLITE,sqlite3_errcode(lite),sqlite3_errmsg(lite),__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		empty = false;	
			
		if (!show)
			if (sqlite3_step(liteStmt) != SQLITE_DONE)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_DBSQLITE,DBSQLITE_FETCHROW,ERR_SQLITE,sqlite3_errcode(lite),sqlite3_errmsg(lite),__LINE__,__FILE__);
				#else
					return false;
				#endif	
		
		#ifdef NO_EX
			return true;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	dodoArray<stringArr>
	dbSqlite::fetchRow() const
	{
		#ifndef DBSQLITE_WO_XEXEC
			operType = DBSQLITE_OPER_FETCHROW;
			performXExec(preExec);
		#endif		
			
		if (!show)
			return __stringarrayvector__;

		sqlite3_reset(liteStmt);
		
		numFields = sqlite3_column_count(liteStmt);
		
		register bool iterate = true;
		register unsigned int i = 0;
		
		rows.clear();
		
		#ifndef USE_DEQUE
			rows.reserve(sqlite3_data_count(liteStmt));
		#endif
		
		while (iterate)
		{
			result = sqlite3_step(liteStmt);
			switch (result)
			{
				case SQLITE_BUSY:
				
					continue;
					
				case SQLITE_DONE:
				
					iterate = false;
					
					break;
					
				case SQLITE_ERROR:
				
					#ifndef NO_EX
						throw baseEx(ERRMODULE_DBSQLITE,DBSQLITE_FETCHROW,ERR_SQLITE,sqlite3_errcode(lite),sqlite3_errmsg(lite),__LINE__,__FILE__);
					#else
						return __stringarrayvector__;
					#endif
					
				case SQLITE_ROW:	
						
					rowsPart.clear();
					
					#ifndef USE_DEQUE
						rowsPart.reserve(numFields);
					#endif
					
					for (i=0;i<numFields;++i)
						switch (sqlite3_column_type(liteStmt,i))
						{
							case SQLITE_INTEGER:
								
								rowsPart.push_back(tools::lToString(sqlite3_column_int(liteStmt,i)));
								
								break;
								
							case SQLITE_FLOAT:
								
								rowsPart.push_back(tools::dToString(sqlite3_column_double(liteStmt,i)));
								
								break;
								
							case SQLITE_TEXT:
								
								rowPart = (const char *)sqlite3_column_text(liteStmt,i);
								if (preventEscaping)
									rowsPart.push_back(rowPart);
								else
									rowsPart.push_back(unescapeFields(rowPart));
								
								break;
								
							case SQLITE_BLOB:
								
								rowPart.assign((const char *)sqlite3_column_blob(liteStmt,i),sqlite3_column_bytes(liteStmt,i));
								if (preventEscaping)
									rowsPart.push_back(rowPart);
								else
									rowsPart.push_back(unescapeFields(rowPart));
								
								break;
								
							case SQLITE_NULL:
							default:
							
								rowsPart.push_back("NULL");
								
								break;	
						}
						
						rows.push_back(rowsPart);
						
					break;
			}
		}

		#ifndef DBSQLITE_WO_XEXEC
			performXExec(postExec);
		#endif
				
		return rows;
	}
	
	//-------------------------------------------------------------------
	
	stringArr
	dbSqlite::fetchField() const
	{	
		#ifndef DBSQLITE_WO_XEXEC
			operType = DBSQLITE_OPER_FETCHFIELD;
			performXExec(preExec);
		#endif
		
		if (!show)
			return __stringarray__;
			
		numFields = sqlite3_column_count(liteStmt);
		
		fields.clear();
		
		#ifndef USE_DEQUE
			fields.reserve(numFields);
		#endif
		
		for (register unsigned int i(0);i<numFields;++i)
			fields.push_back(sqlite3_column_name(liteStmt, i));

		#ifndef DBSQLITE_WO_XEXEC
			performXExec(postExec);
		#endif			
					
		return fields;
	}
	
	//-------------------------------------------------------------------
	
	__dbStorage 
	dbSqlite::fetch() const
	{
		return __dbStorage(fetchRow(), fetchField());
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	dbSqlite::rowsCount() const
	{
		if (show)
		{
			sqlite3_reset(liteStmt);
		
			numRows = 0;
			register bool iterate = true;
			
			while (iterate)
			{
				result = sqlite3_step(liteStmt);
				switch (result)
				{
					case SQLITE_BUSY:
					
						continue;
						
					case SQLITE_DONE:
					
						iterate = false;
						
						break;
						
					case SQLITE_ERROR:
					
						#ifndef NO_EX
							throw baseEx(ERRMODULE_DBSQLITE,DBSQLITE_FETCHROW,ERR_SQLITE,sqlite3_errcode(lite),sqlite3_errmsg(lite),__LINE__,__FILE__);
						#else
							return false;
						#endif
						
					case SQLITE_ROW:	
					
						++numRows;
							
						break;
				}
			}	
			
			return numRows;	
		}
		else	
			return 0;
	}
	
	//-------------------------------------------------------------------
	
	unsigned int 
	dbSqlite::fieldsCount() const
	{
		if (show)
			return sqlite3_column_count(liteStmt);
		else	
			return 0;
	}
	
	//-------------------------------------------------------------------
	
	unsigned int
	dbSqlite::affectedRowsCount()
	{
		if (!show)
			return sqlite3_changes(lite);
		else	
			return 0;
	}
	
	//-------------------------------------------------------------------
	

	#ifndef NO_EX
		void 
	#else
		bool
	#endif
	dbSqlite::exec(const std::string &query, 
					bool result) const
	{
		#ifndef DBSQLITE_WO_XEXEC
			operType = DBSQLITE_OPER_EXEC;
			performXExec(preExec);
		#endif
		
		#ifdef NO_EX
			bool _result = 
		#endif	
			_exec(query, result); 
		
		#ifndef DBSQLITE_WO_XEXEC		
			performXExec(postExec);
		#endif
		
		cleanCollect();
		
		#ifdef NO_EX
			return _result;
		#endif
	}
	
	//-------------------------------------------------------------------
	
	#ifndef DBSQLITE_WO_XEXEC
	
		int 
		dbSqlite::addPostExec(inExec func, 
							void *data) const
		{
			return _addPostExec(func, (void *)this, XEXECOBJ_DBSQLITE, data);
		}
		
		//-------------------------------------------------------------------
		
		int 
		dbSqlite::addPreExec(inExec func, 
							void *data) const
		{
			return _addPreExec(func, (void *)this, XEXECOBJ_DBSQLITE, data);
		}
		
		//-------------------------------------------------------------------
		
		#ifdef DL_EXT
		
			int 
			dbSqlite::addPostExec(const std::string &module, 
								void *data,
								void *toInit) const
			{
				return _addPostExec(module, (void *)this, XEXECOBJ_DBSQLITE, data, toInit);
			}
			
			//-------------------------------------------------------------------
			
			int 
			dbSqlite::addPreExec(const std::string &module, 
								void *data,
								void *toInit) const
			{
				return _addPreExec(module, (void *)this, XEXECOBJ_DBSQLITE, data, toInit);
			}
		
			//-------------------------------------------------------------------
			
			xexecCounts 
			dbSqlite::addExec(const std::string &module, 
								void *data,
								void *toInit) const
			{
				return _addExec(module, (void *)this, XEXECOBJ_DBSQLITE, data, toInit);
			}
		
		#endif
	
		//-------------------------------------------------------------------
			
	#endif
	
	//-------------------------------------------------------------------	
	
	dodoStringMapArr 
	dbSqlite::fetchAssoc() const
	{
		if (!show)
			return __dodostringmap__;
		
		sqlite3_reset(liteStmt);
		
		numFields = sqlite3_column_count(liteStmt);
		
		register bool iterate = true;
		register unsigned int i = 0;
		
		rowsFields.clear();
		
		#ifndef USE_DEQUE
			rowsFields.reserve(sqlite3_data_count(liteStmt));
		#endif
		
		while (iterate)
		{
			result = sqlite3_step(liteStmt);
			switch (result)
			{
				case SQLITE_BUSY:
				
					continue;
					
				case SQLITE_DONE:
				
					iterate = false;
					
					break;
					
				case SQLITE_ERROR:
				
					#ifndef NO_EX
						throw baseEx(ERRMODULE_DBSQLITE,DBSQLITE_FETCHASSOC,ERR_SQLITE,sqlite3_errcode(lite),sqlite3_errmsg(lite),__LINE__,__FILE__);
					#else
						return __dodostringmap__;
					#endif
					
				case SQLITE_ROW:	
						
					rowFieldsPart.clear();
					
					for (i=0;i<numFields;++i)
						switch (sqlite3_column_type(liteStmt,i))
						{
							case SQLITE_INTEGER:
								
								rowFieldsPart.realArr[sqlite3_column_name(liteStmt, i)] = tools::lToString(sqlite3_column_int(liteStmt,i));
								
								break;
								
							case SQLITE_FLOAT:
								
								rowFieldsPart.realArr[sqlite3_column_name(liteStmt, i)] = tools::dToString(sqlite3_column_double(liteStmt,i));
								
								break;
								
							case SQLITE_TEXT:
								
								rowPart = (const char *)sqlite3_column_text(liteStmt,i);
								if (preventEscaping)
									rowFieldsPart.realArr[sqlite3_column_name(liteStmt, i)] = rowPart;
								else
									rowFieldsPart.realArr[sqlite3_column_name(liteStmt, i)] = unescapeFields(rowPart);
								
								break;
								
							case SQLITE_BLOB:
								
								rowPart.assign((const char *)sqlite3_column_blob(liteStmt,i),sqlite3_column_bytes(liteStmt,i));
								if (preventEscaping)
									rowFieldsPart.realArr[sqlite3_column_name(liteStmt, i)] = rowPart;
								else
									rowFieldsPart.realArr[sqlite3_column_name(liteStmt, i)] = unescapeFields(rowPart);
								
								break;
								
							case SQLITE_NULL:
							default:
							
								rowFieldsPart.realArr[sqlite3_column_name(liteStmt, i)] = "NULL";
								
								break;	
						}
						
						rowsFields.push_back(rowFieldsPart);
						
					break;			
			}
		}
		
		return rowsFields;	
	}

	//-------------------------------------------------------------------
	
	
#endif
