/***************************************************************************
 *            dataBasePostgresql.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#ifdef POSTGRESQL_EXT
#include <libpq-fe.h>
#include <stdlib.h>

#include <libdodo/dataBasePostgresql.h>
#include <libdodo/toolsString.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/dataBasePostgresqlEx.h>
#include <libdodo/xexec.h>

namespace dodo
{
	namespace data
	{
		namespace base
		{
			/**
			 * @struct __postgresql__
			 * @brief defines internal handlers for MySQL DBMS interaction
			 */
			struct __postgresql__
			{
				/**
				 * constructor
				 */
				__postgresql__() : handle(NULL),
								 result(NULL)
				{
				}

				PGconn *handle; ///< DB handle
				PGresult *result; ///< handlde to result
			};
		};
	};
};

using namespace dodo::data::base;

#ifdef POSTGRESQL_NO_ENCODINGTOCHAR
const dodoString postgresql::encodingStatements[] =
{
	"SQL_ASCII",
	"EUC_JP",
	"EUC_CN",
	"EUC_KR",
	"EUC_TW",
	"JOHAB",
	"UTF8",
	"MULE_INTERNAL",
	"LATIN1",
	"LATIN2",
	"LATIN3",
	"LATIN4",
	"LATIN5",
	"LATIN6",
	"LATIN7",
	"LATIN8",
	"LATIN9",
	"LATIN10",
	"WIN1256",
	"WIN1258",
	"WIN866",
	"WIN874",
	"KOI8",
	"WIN1251",
	"WIN1252",
	"ISO_8859_5",
	"ISO_8859_6",
	"ISO_8859_7",
	"ISO_8859_8",
	"WIN1250",
	"WIN1253",
	"WIN1254",
	"WIN1255",
	"WIN1257",
	"SJIS",
	"BIG5",
	"GBK",
	"UHC",
	"GB18030"
};
#endif

//-------------------------------------------------------------------

postgresql::postgresql() : empty(true),
						   handle(new __postgresql__)
{
#ifndef DATABASE_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_DATABASEPOSTGRESQL);
#endif
}

//-------------------------------------------------------------------

postgresql::postgresql(const __connectionInfo__ &info) : empty(true),
													   handle(new __postgresql__)
{
#ifndef DATABASE_WO_XEXEC
	collectedData.setExecObject(XEXEC_OBJECT_DATABASEPOSTGRESQL);
#endif

	collectedData.dbInfo = info;

	handle->handle = PQsetdbLogin(
		collectedData.dbInfo.host.size() == 0 ? NULL : collectedData.dbInfo.host.c_str(),
		tools::string::uiToString(collectedData.dbInfo.port).c_str(),
		NULL,
		NULL,
		collectedData.dbInfo.db.size() == 0 ? NULL : collectedData.dbInfo.db.c_str(),
		collectedData.dbInfo.user.size() == 0 ? NULL : collectedData.dbInfo.user.c_str(),
		collectedData.dbInfo.password.size() == 0 ? NULL : collectedData.dbInfo.password.c_str());

	int status = PQstatus(handle->handle);

	if (status != CONNECTION_OK)
	{
		delete handle;

		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_POSTGRESQL, exception::ERRNO_MYSQL, status, PQerrorMessage(handle->handle), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

postgresql::postgresql(postgresql &a_mypp)
{
}

//-------------------------------------------------------------------

postgresql::~postgresql()
{
	if (handle->handle != NULL)
	{
		if (!empty)
		{
			PQclear(handle->result);
		}

		PQfinish(handle->handle);
	}

	delete handle;
}

//-------------------------------------------------------------------

void
postgresql::connect(const __connectionInfo__ &info)
{
	collectedData.dbInfo = info;

#ifndef DATABASE_WO_XEXEC
	operType = DATABASE_OPERATION_CONNECT;
	performXExec(preExec);
#endif

	if (handle->handle != NULL)
	{
		if (!empty)
		{
			PQclear(handle->result);
			empty = true;
		}

		PQfinish(handle->handle);

		handle->handle = NULL;
	}

	handle->handle = PQsetdbLogin(
		collectedData.dbInfo.host.size() == 0 ? NULL : collectedData.dbInfo.host.c_str(),
		tools::string::uiToString(collectedData.dbInfo.port).c_str(),
		NULL,
		NULL,
		collectedData.dbInfo.db.size() == 0 ? NULL : collectedData.dbInfo.db.c_str(),
		collectedData.dbInfo.user.size() == 0 ? NULL : collectedData.dbInfo.user.c_str(),
		collectedData.dbInfo.password.size() == 0 ? NULL : collectedData.dbInfo.password.c_str());

	int status = PQstatus(handle->handle);

	if (status != CONNECTION_OK)
	{
		handle->handle = NULL;

		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_CONNECT, exception::ERRNO_MYSQL, status, PQerrorMessage(handle->handle), __LINE__, __FILE__);
	}

#ifndef DATABASE_WO_XEXEC
	performXExec(postExec);
#endif
}

//-------------------------------------------------------------------

void
postgresql::disconnect()
{
	if (handle->handle != NULL)
	{
#ifndef DATABASE_WO_XEXEC
		operType = DATABASE_OPERATION_DISCONNECT;
		performXExec(preExec);
#endif

		if (!empty)
		{
			PQclear(handle->result);
			empty = true;
		}

		PQfinish(handle->handle);

#ifndef DATABASE_WO_XEXEC
		performXExec(postExec);
#endif

		handle->handle = NULL;
	}
}

//-------------------------------------------------------------------

dodoArray<dodo::dodoStringArray>
postgresql::fetchRows() const
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

#ifndef DATABASE_WO_XEXEC
	operType = DATABASE_OPERATION_FETCHROW;
	performXExec(preExec);
#endif

	dodoArray<dodoStringArray> rows;

	if (empty || !show)
	{
		return rows;
	}

	int rowsNum = PQntuples(handle->result);
	int fieldsNum = PQnfields(handle->result);

#ifndef USE_DEQUE
	rows.reserve(rowsNum);
#endif

	dodoStringArray rowsPart;

#ifndef USE_DEQUE
	rowsPart.reserve(fieldsNum);
#endif

	int j;
	for (int i(0); i < rowsNum; ++i)
	{
		rowsPart.clear();

		for (j = 0; j < fieldsNum; ++j)
		{
			if (PQgetisnull(handle->result, i, j) == 1)
			{
				rowsPart.push_back(statements[SQLCONSTRUCTOR_STATEMENT_NULL]);
			}
			else
			{
				rowsPart.push_back(dodoString(PQgetvalue(handle->result, i, j), PQgetlength(handle->result, i, j)));
			}
		}

		rows.push_back(rowsPart);
	}

#ifndef DATABASE_WO_XEXEC
	performXExec(postExec);
#endif

	return rows;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
postgresql::fetchFields() const
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

#ifndef DATABASE_WO_XEXEC
	operType = DATABASE_OPERATION_FETCHFIELD;
	performXExec(preExec);
#endif

	dodoStringArray fields;

	if (empty || !show)
	{
		return fields;
	}

	int fieldsNum = PQnfields(handle->result);

#ifndef USE_DEQUE
	fields.reserve(fieldsNum);
#endif

	for (int i(0); i < fieldsNum; ++i)
	{
		fields.push_back(PQfname(handle->result, i));
	}

#ifndef DATABASE_WO_XEXEC
	performXExec(postExec);
#endif

	return fields;
}

//-------------------------------------------------------------------

__tuples__
postgresql::fetch() const
{
	return __tuples__(fetchRows(), fetchFields());
}

//-------------------------------------------------------------------

unsigned int
postgresql::rowsCount() const
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	if (empty || !show)
	{
		return 0;
	}
	else
	{
		return PQntuples(handle->result);
	}
}

//-------------------------------------------------------------------

unsigned int
postgresql::fieldsCount() const
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	if (empty || !show)
	{
		return 0;
	}
	else
	{
		return PQnfields(handle->result);
	}
}

//-------------------------------------------------------------------

unsigned int
postgresql::affectedRowsCount() const
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	if (empty || show)
	{
		return 0;
	}
	else
	{
		return atoi(PQcmdTuples(handle->result));
	}
}

//-------------------------------------------------------------------

void
postgresql::getFieldsTypes(const dodoString &table)
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	dodoString temp = collectedData.dbInfo.db + ":" + table;

	dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(temp);

	if (types == fieldTypes.end())
	{
		types = fieldTypes.insert(make_pair(temp, dodoMap<dodoString, short, dodoMapICaseStringCompare>())).first;
	}

	request = "select column_name, data_type from information_schema.columns where table_name='" + table + "'";

	if (!empty)
	{
		PQclear(handle->result);
		empty = true;
	}

	handle->result = PQexecParams(handle->handle, request.c_str(), 0, NULL, NULL, NULL, NULL, 1);
	if (handle->result == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_GETFIELDSTYPES, exception::ERRNO_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(handle->handle), __LINE__, __FILE__, request);
	}

	int status = PQresultStatus(handle->result);

	switch (status)
	{
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:

			throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_GETFIELDSTYPES, exception::ERRNO_MYSQL, status, PQerrorMessage(handle->handle), __LINE__, __FILE__);
	}

	empty = false;

	int rowsNum = PQntuples(handle->result);

	const char *columnType, *columnName;

	dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator field, fieldsEnd = types->second.end();

	dodoStringArray rowsPart;

	for (int i(0); i < rowsNum; ++i)
	{
		columnName = PQgetvalue(handle->result, i, 0);
		columnType = PQgetvalue(handle->result, i, 1);

		field = types->second.find(columnName);

		if (field == fieldsEnd)
		{
			if (strcasestr(columnType, "char") != NULL ||
				strcasestr(columnType, "date") != NULL ||
				strcasestr(columnType, "text") != NULL)
			{
				types->second.insert(make_pair(dodoString(columnName), sql::FIELDTYPE_TEXT));
			}
			else
			{
				if (strcasestr(columnType, "bytea") != NULL ||
					strcasestr(columnType, "array") != NULL ||
					strcasestr(columnType, "cidr") != NULL ||
					strcasestr(columnType, "macaddrcd") != NULL ||
					strcasestr(columnType, "inet") != NULL)
				{
					types->second.insert(make_pair(dodoString(columnName), sql::FIELDTYPE_BINARY));
				}
				else
				{
					types->second.insert(make_pair(dodoString(columnName), sql::FIELDTYPE_NUMERIC));
				}
			}
		}
		else
		{
			if (strcasestr(columnType, "char") != NULL ||
				strcasestr(columnType, "date") != NULL ||
				strcasestr(columnType, "text") != NULL)
			{
				field->second = sql::FIELDTYPE_TEXT;
			}
			else
			{
				if (strcasestr(columnType, "bytea") != NULL ||
					strcasestr(columnType, "array") != NULL ||
					strcasestr(columnType, "cidr") != NULL ||
					strcasestr(columnType, "macaddrcd") != NULL ||
					strcasestr(columnType, "inet") != NULL)
				{
					field->second = sql::FIELDTYPE_BINARY;
				}
				else
				{
					field->second = sql::FIELDTYPE_NUMERIC;
				}
			}
		}
	}

	if (!empty)
	{
		PQclear(handle->result);
		empty = true;
	}
}

//-------------------------------------------------------------------

void
postgresql::exec(const dodoString &query,
				 bool             result)
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

#ifndef DATABASE_WO_XEXEC
	operType = DATABASE_OPERATION_EXEC;
	performXExec(preExec);
#endif

	int status;

	if (query.size() == 0)
	{
		queryCollect();
	}
	else
	{
		request = query;
		show = result;
	}

	if (!empty)
	{
		PQclear(handle->result);
		empty = true;
	}

	unsigned long size = blobs.size();

	if (size > 0)
	{
		char **values = new char*[size];
		int *lengths = new int[size];
		int *formats = new int[size];

		dodoList<__blob__>::iterator i(blobs.begin()), j(blobs.end());
		for (int o = 0; i != j; ++i, ++o)
		{
			values[o] = (char *)i->value->c_str();
			lengths[o] = i->value->size();
			formats[o] = 1;
		}

		handle->result = PQexecParams(handle->handle, request.c_str(), size, NULL, values, lengths, formats, 0);

		blobs.clear();
	}
	else
	{
		handle->result = PQexecParams(handle->handle, request.c_str(), 0, NULL, NULL, NULL, NULL, 1);
	}

	if (handle->result == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_EXEC, exception::ERRNO_MYSQL, PGRES_FATAL_ERROR, PQerrorMessage(handle->handle), __LINE__, __FILE__);
	}

	status = PQresultStatus(handle->result);
	switch (status)
	{
		case PGRES_EMPTY_QUERY:
		case PGRES_BAD_RESPONSE:
		case PGRES_NONFATAL_ERROR:
		case PGRES_FATAL_ERROR:

			throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_EXEC, exception::ERRNO_MYSQL, status, PQerrorMessage(handle->handle), __LINE__, __FILE__);
	}

	empty = false;

#ifndef DATABASE_WO_XEXEC
	performXExec(postExec);
#endif

	cleanCollected();
	request.clear();
}

//-------------------------------------------------------------------

void
postgresql::updateCollect()
{
	request = statements[SQLCONSTRUCTOR_STATEMENT_UPDATE];
	request.append(collectedData.table);
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_SET]);

	dodoArray<dodoStringArray>::iterator v = collectedData.values.begin();
	if (v != collectedData.values.end())
	{
		unsigned int fn(collectedData.fields.size()), fv(v->size());

		unsigned int o(fn <= fv ? fn : fv);

		dodoStringArray::const_iterator i(collectedData.fields.begin()), j(v->begin());
		if (i != j)
		{
			dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.dbInfo.db + statements[SQLCONSTRUCTOR_STATEMENT_COLON] + collectedData.table);
			if (types != fieldTypes.end())
			{
				dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator type;
				dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

				__blob__ blob;

				unsigned int k = 1;
				for (; k < o; ++i, ++k, ++j)
				{
					request.append(*i);

					type = types->second.find(*i);
					if (type != typesEnd)
					{
						if (type->second == sql::FIELDTYPE_TEXT)
						{
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
							request.append(escapeFields(*j));
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
						}
						else
						{
							if (type->second == sql::FIELDTYPE_BINARY)
							{
								request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
								request.append("$" + tools::string::uiToString(k));
								request.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);

								blob.reference = k;
								blob.value = &(*j);

								blobs.push_back(blob);
							}
							else
							{
								request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
								request.append(*j);
								request.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
							}
						}
					}
					else
					{
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
						request.append(escapeFields(*j));
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
					}
				}
				request.append(*i);

				type = types->second.find(*i);
				if (type != typesEnd)
				{
					if (type->second == sql::FIELDTYPE_TEXT)
					{
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
						request.append(escapeFields(*j));
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
					}
					else
					{
						if (type->second == sql::FIELDTYPE_BINARY)
						{
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
							request.append("$" + tools::string::uiToString(k));
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);

							blob.reference = k;
							blob.value = &(*j);

							blobs.push_back(blob);
						}
						else
						{
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUAL]);
							request.append(*j);
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
						}
					}
				}
				else
				{
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
					request.append(escapeFields(*j));
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
				}
			}
			else
			{
				for (unsigned int k(1); k < o; ++i, ++k, ++j)
				{
					request.append(*i);
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
					request.append(escapeFields(*j));
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
				}
				request.append(*i);
				request.append(statements[SQLCONSTRUCTOR_STATEMENT_EQUALAPOSTROPHE]);
				request.append(escapeFields(*j));
				request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
			}
		}
	}
}

//-------------------------------------------------------------------

void
postgresql::insertCollect()
{
	request = statements[SQLCONSTRUCTOR_STATEMENT_INSERT];
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_INTO]);
	request.append(collectedData.table);
	if (collectedData.fields.size() != 0)
	{
		request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
		request.append(tools::misc::join(collectedData.fields, statements[SQLCONSTRUCTOR_STATEMENT_COMA]));
		request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
	}
	request.append(statements[SQLCONSTRUCTOR_STATEMENT_VALUES]);

	dodoArray<dodoStringArray>::iterator k(collectedData.values.begin()), l(collectedData.values.end());
	if (k != l)
	{
		dodoMap<dodoString, dodoMap<dodoString, short, dodoMapICaseStringCompare>, dodoMapICaseStringCompare>::iterator types = fieldTypes.find(collectedData.dbInfo.db + statements[SQLCONSTRUCTOR_STATEMENT_COLON] + collectedData.table);
		if (types != fieldTypes.end())
		{
			dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator type;
			dodoMap<dodoString, short, dodoMapICaseStringCompare>::iterator typesEnd = types->second.end();

			dodoStringArray::iterator t;

			__blob__ blob;

			unsigned int o = 0;

			--l;
			for (; k != l; ++k)
			{
				request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);

				t = collectedData.fields.begin();

				dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
				for (; i != j; ++i, ++t)
				{
					type = types->second.find(*t);
					if (type != typesEnd)
					{
						if (type->second == sql::FIELDTYPE_TEXT)
						{
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
						}
						else
						{
							if (type->second == sql::FIELDTYPE_BINARY)
							{
								++o;

								request.append("$" + tools::string::uiToString(o));
								request.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);

								blob.reference = o;
								blob.value = &(*i);

								blobs.push_back(blob);
							}
							else
							{
								request.append(*i + statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
							}
						}
					}
					else
					{
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
					}
				}
				type = types->second.find(*t);
				if (type != typesEnd)
				{
					if (type->second == sql::FIELDTYPE_TEXT)
					{
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
					}
					else
					{
						if (type->second == sql::FIELDTYPE_BINARY)
						{
							++o;

							request.append("$" + tools::string::uiToString(o));

							blob.reference = o;
							blob.value = &(*i);

							blobs.push_back(blob);
						}
						else
						{
							request.append(*i);
						}
					}
				}
				else
				{
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
				}

				request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKETCOMA]);
			}
			request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);

			t = collectedData.fields.begin();

			dodoStringArray::const_iterator i(k->begin()), j(k->end() - 1);
			for (; i != j; ++i, ++t)
			{
				type = types->second.find(*t);
				if (type != typesEnd)
				{
					if (type->second == sql::FIELDTYPE_TEXT)
					{
						request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
					}
					else
					{
						if (type->second == sql::FIELDTYPE_BINARY)
						{
							++o;

							request.append("$" + tools::string::uiToString(o));
							request.append(statements[SQLCONSTRUCTOR_STATEMENT_COMA]);

							blob.reference = o;
							blob.value = &(*i);

							blobs.push_back(blob);
						}
						else
						{
							request.append(*i + statements[SQLCONSTRUCTOR_STATEMENT_COMA]);
						}
					}
				}
				else
				{
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHECOMA]);
				}
			}
			type = types->second.find(*t);
			if (type != typesEnd)
			{
				if (type->second == sql::FIELDTYPE_TEXT)
				{
					request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
				}
				else
				{
					if (type->second == sql::FIELDTYPE_BINARY)
					{
						++o;

						request.append("$" + tools::string::uiToString(o));

						blob.reference = o;
						blob.value = &(*i);

						blobs.push_back(blob);
					}
					else
					{
						request.append(*i);
					}
				}
			}
			else
			{
				request.append(statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE] + escapeFields(*i) + statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]);
			}

			request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
		}
		else
		{
			--l;
			for (; k != l; ++k)
			{
				request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
				request.append(joinFields(*k, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
				request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKETCOMA]);
			}
			request.append(statements[SQLCONSTRUCTOR_STATEMENT_LEFTBRACKET]);
			request.append(joinFields(*k, statements[SQLCONSTRUCTOR_STATEMENT_COMA], statements[SQLCONSTRUCTOR_STATEMENT_APOSTROPHE]));
			request.append(statements[SQLCONSTRUCTOR_STATEMENT_RIGHTBRACKET]);
		}
	}
}

//-------------------------------------------------------------------

dodo::dodoStringMapArray
postgresql::fetchFieldsToRows() const
{
	dodoStringMapArray rowsFields;

	if (empty || !show)
	{
		return rowsFields;
	}

	int rowsNum = PQntuples(handle->result);
	int fieldsNum = PQnfields(handle->result);

#ifndef USE_DEQUE
	rowsFields.reserve(rowsNum);
#endif

	dodoStringMap rowFieldsPart;

	int j;
	for (int i(0); i < rowsNum; ++i)
	{
		rowFieldsPart.clear();

		for (j = 0; j < fieldsNum; ++j)
		{
			if (PQgetisnull(handle->result, i, j) == 1)
			{
				rowFieldsPart.insert(make_pair(PQfname(handle->result, j), statements[SQLCONSTRUCTOR_STATEMENT_NULL]));
			}
			else
			{
				rowFieldsPart.insert(make_pair(PQfname(handle->result, j), dodoString(PQgetvalue(handle->result, i, j), PQgetlength(handle->result, i, j))));
			}
		}

		rowsFields.push_back(rowFieldsPart);
	}

	return rowsFields;
}

//-------------------------------------------------------------------

void
postgresql::setCharset(const dodoString &charset)
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

	int status = PQsetClientEncoding(handle->handle, charset.c_str());
	if (status == -1)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_SETCHARSET, exception::ERRNO_MYSQL, status, PQerrorMessage(handle->handle), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

dodoString
postgresql::getCharset() const
{
	if (handle->handle == NULL)
	{
		throw exception::basic(exception::ERRMODULE_DATABASEPOSTGRESQL, POSTGRESQLEX_GETFIELDSTYPES, exception::ERRNO_LIBDODO, POSTGRESQLEX_NOTOPENED, DATABASEPOSTGRESQLEX_NOTOPENED_STR, __LINE__, __FILE__);
	}

#ifdef POSTGRESQL_NO_ENCODINGTOCHAR
	int encoding = PQclientEncoding(handle->handle);

	if (encoding >= 0 && encoding < _PG_LAST_ENCODING_)
	{
		return encodingStatements[encoding];
	}

	return __dodostring____;
#else
	return pg_encoding_to_char(PQclientEncoding(handle->handle));
#endif
}
#endif

//-------------------------------------------------------------------
