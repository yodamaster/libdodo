	/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/baseEx.h>
#include <libdodo/dbConnector.h>
#include <libdodo/dbSqlConstructor.h>
#include <libdodo/dbPostgresql.h>
#include <libdodo/dbMysql.h>
#include <libdodo/dbSqlite.h>
#include <libdodo/toolsFilesystem.h>

#include <iostream>

using namespace dodo;
using namespace db;

using namespace std;

#ifndef DB_WO_XEXEC

/**
 * db hook
 */
void
hook(void *odata,
	 short int type,
	 void *udata)
{
	__xexecDbAccumulatorCollectedData *sql = (__xexecDbAccumulatorCollectedData *)odata;

	if (sql->operType == DB_OPERATION_EXEC)
	{
		///print the resulting query
		cout << endl << endl << "request: " << ((sqlConstructor *)(sql->executor))->queryCollect() << endl << endl;
	}
}

#endif

int main(int argc, char **argv)
{
	connector *pp;

	///parse command line arguments to figure out what db to use
	if (argc == 2)
	{
		if (strcasecmp(argv[1], "postgresql") == 0)
#ifdef POSTGRESQL_EXT
			pp = new postgresql;
#else
			return 1;
#endif

		else if (strcasecmp(argv[1], "mysql") == 0)
#ifdef MYSQL_EXT
			pp = new mysql;
#else
			return 1;
#endif

		else if (strcasecmp(argv[1], "sqlite") == 0)
#ifdef SQLITE_EXT
			pp = new sqlite;
#else
			return 1;
#endif

		else
			return 1;
	}
	else
		return 1;

	try
	{

#ifndef DB_WO_XEXEC

		///add db hook
		int pos = pp->addPreExec(hook, (void *)"id");

#endif

		__connectorInfo info;
		info.db = "test";
		if (strcasecmp(argv[1], "postgresql") == 0)
		{
			info.host = "127.0.0.1";
			info.port = 5432;
			info.user = "postgres";
		}
		else if (strcasecmp(argv[1], "mysql") == 0)
		{
			info.path = "/var/run/mysqld/mysqld.sock";
			info.user = "root";
			info.password = "password";
		}
		else if (strcasecmp(argv[1], "sqlite") == 0)
		{
			info.path = "test.lite";
		}
		pp->setDbInfo(info);
		pp->connect();

		///define session charset
		if (strcasecmp(argv[1], "postgresql") == 0)
		{
			#ifdef POSTGRESQL_EXT

				((postgresql *)pp)->setCharset("UTF-8");

				cout << "Encoding: " << ((postgresql *)pp)->getCharset() << endl;

			#endif
		}
		else if (strcasecmp(argv[1], "mysql") == 0)
		{
			#ifdef MYSQL_EXT

				((mysql *)pp)->setCharset("UTF-8");

				cout << "Encoding: " << ((mysql *)pp)->getCharset() << endl;

			#endif
		}

		pp->deleteTable("test");
		try
		{
			pp->exec();
		}
		catch (...)
		{
		}

		__connectorField fi;

		__connectorTable ti;
		ti.name = "test";

		fi.name = "date";
		fi.type = CONNECTOR_FIELDTYPE_TEXT;
		ti.fields.push_back(fi);

		fi.name = "operation";
		fi.type = CONNECTOR_FIELDTYPE_TEXT;
		ti.fields.push_back(fi);

		fi.name = "id";
		fi.type = CONNECTOR_FIELDTYPE_INTEGER;
		fi.flag = CONNECTOR_FIELDFLAG_NULL;
		ti.fields.push_back(fi);

		fi.name = "d";
		fi.type = CONNECTOR_FIELDTYPE_INTEGER;
		fi.flag = CONNECTOR_FIELDFLAG_NULL;
		ti.fields.push_back(fi);

		fi.name = "b";
		fi.type = CONNECTOR_FIELDTYPE_LONGBLOB;
		fi.flag = CONNECTOR_FIELDFLAG_NULL;
		ti.fields.push_back(fi);

		pp->createTable(ti);
		pp->exec();

#ifdef ENABLE_SQL_AUTOFRAMING

#ifndef SQLITE_ENABLE_COLUMN_METADATA

#ifdef SQLITE_EXT

		///manually define which fields to escape if db is sqlite and it was compiled w/o SQLITE_ENABLE_COLUMN_METADATA and autoFraming is turned on
		if (strcasecmp(argv[1], "sqlite") == 0)
		{
			dodoStringArray columns;
			columns.push_back("operation");
			columns.push_back("b");
			columns.push_back("date");

			((sqlConstructor *)pp)->autoFraming = true;
			((sqlConstructor *)pp)->manualAutoFraming = true;

			((sqlConstructor *)pp)->setAutoFramingRule(info.db, ti.name, columns);
		}
#endif

#endif

		cout << ((((sqlConstructor *)pp)->autoFraming) ? "Automatic framing turned on." : "Automatic framing turned off.") << endl;

#endif

		dodoStringMap arr;
		arr["date"] = "2005-07-08";
		arr["operation"] = "mu";
		arr["d"] = "1";

		dodoArray<dodoString> select;
		select.push_back("date");
		select.push_back("operation");
		select.push_back("b");

		__connectorStorage store;

		for (int i = 0; i < 10; i++)
		{
			pp->select("test", select, "id<20 or operation='um'");
			pp->exec();

			store = pp->fetch();

			pp->insert("test", arr);
			pp->exec();

			arr["d"] = "d+1";
			arr["operation"] = "um";
			pp->update("test", arr);
			pp->exec();
			
			arr["operation"] = "mu";
			arr["d"] = "1";
		}

		pp->select("test", select, "operation='um'");
		pp->exec();

		store = pp->fetch();

		cout << store.rows.size() << endl;

		dodoArray<dodoStringArray>::iterator i(store.rows.begin()), j(store.rows.end());
		dodoStringArray::iterator m, n;
		for (; i != j; i++)
		{
			m = i->begin();
			n = i->end();
			for (; m != n; m++)
				cout << *m << "\t";
			cout << endl;
		}

#ifndef DB_WO_XEXEC

		///disable hook
		pp->disablePreExec(pos);

#endif

		tools::filesystem::unlink("test.db");
		dodoString dt = tools::filesystem::getFileContents("test");

		arr.clear();

		///define links to field values for sqlite or postgresql or put binary data for others
		if (strcasecmp(argv[1], "sqlite") == 0 || strcasecmp(argv[1], "postgresql") == 0)
			arr["b"] = "$1";
		else
			arr["b"] = dt;

		///define BLOB values for sqlite or postgresql to satisfy fields links
		if (strcasecmp(argv[1], "sqlite") == 0 || strcasecmp(argv[1], "postgresql") == 0)
		{
			dodoStringArray blobs;
			blobs.push_back(dt);

			if (strcasecmp(argv[1], "sqlite") == 0)
#ifdef SQLITE_EXT
				((sqlite *)pp)->setBLOBValues(blobs);
#else
				;
#endif
			else
			{
				if (strcasecmp(argv[1], "postgresql") == 0)
#ifdef POSTGRESQL_EXT
					((postgresql *)pp)->setBLOBValues(blobs);
#else
					;
#endif
			}

			///disable escaping and framing
			((sqlConstructor *)pp)->preventFraming = true;
			((sqlConstructor *)pp)->preventEscaping = true;

			arr["date"] = "'2005-07-08'";
			arr["operation"] = "'ma'";
		}
		else
		{
			arr["date"] = "2005-07-08";
			arr["operation"] = "ma";
		}

		///use a hint to notify sqlite or postgresql to use field links
		if (strcasecmp(argv[1], "sqlite") == 0)
#ifdef SQLITE_EXT
			addFlag(((sqlite *)pp)->hint, SQLITE_HINT_BLOB);
#else
			;
#endif
		else
		if (strcasecmp(argv[1], "postgresql") == 0)
#ifdef POSTGRESQL_EXT
			addFlag(((postgresql *)pp)->hint, POSTGRESQL_HINT_BLOB);
#else
			;
#endif

		///insert binary data
		pp->insert("test", arr);
		pp->exec();

		///use a hint to notify sqlite or postgresql to use field links
		if (strcasecmp(argv[1], "sqlite") == 0)
#ifdef SQLITE_EXT
			addFlag(((sqlite *)pp)->hint, SQLITE_HINT_BLOB);
#else
			;
#endif
		else
		if (strcasecmp(argv[1], "postgresql") == 0)
#ifdef POSTGRESQL_EXT
			addFlag(((postgresql *)pp)->hint, POSTGRESQL_HINT_BLOB);
#else
			;
#endif
		
		pp->select("test", select, "operation='ma'");
		pp->exec();

		store = pp->fetch();

		///put fetched binary data to file
		if (store.fields.size() == 3 && store.rows.size() > 0)
			tools::filesystem::writeToFile("test.db", (*store.rows.begin())[2]);
	}
	catch (baseEx ex)
	{
		cout << (string)ex << endl << ex.message << endl << ex.line << endl << endl;
	}

	delete pp;

	return 0;
}
