/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/dataBaseSqlite.h>
#include <libdodo/toolsFilesystem.h>

#include <iostream>

using namespace std;

using namespace dodo;

#ifdef SQLITE_EXT

using namespace data::base;

#ifndef DATABASE_WO_XEXEC

void
hook(__xexecCollectedData *odata,
	 short int type,
	 void *udata)
{
	__xexecDataBaseAccumulatorCollectedData *sql = (__xexecDataBaseAccumulatorCollectedData *)odata;

	if (sql->operType == DATABASE_OPERATION_EXEC)
	{
		cout << endl << endl << "request: " << dynamic_cast<sql::constructor *>(sql->executor)->queryCollect() << endl << endl;
	}
}

#endif

#endif

int main(int argc, char **argv)
{
#ifdef SQLITE_EXT

	sqlite pp;
	try
	{
#ifndef DATABASE_WO_XEXEC

		int pos = pp.addPreExec(hook, (void *)"id");

#endif

		tools::filesystem::unlink("test.lite", true);

		__connectionInfo info;
		info.path = "test.lite";

		pp.connect(info);

		try
		{
			pp.exec("DROP TABLE test");
		}
		catch (...)
		{
		}

		pp.exec("CREATE TABLE test (date text NOT NULL, operation text NOT NULL, id int default NULL, d int default NULL, b longblob)");

		dodoStringMap arr;
		arr["date"] = "2005-07-08";
		arr["operation"] = "mu";

		dodoArray<dodoString> select;
		select.push_back("date");
		select.push_back("operation");

		for (int i = 0; i < 10; i++)
		{
			pp.select("test", select, "`id`<20 or `operation`='um'");
			pp.exec();

			cout << "Selected: " << pp.rowsCount() << endl;
			cout << "Selected2: " << pp.fetch().rows.size() << endl;
			cout << "Selected3: " << pp.fetch().rows.size() << endl;
			cout << "Fields: " << pp.fieldsCount() << endl;
			cout << "Fields2: " << pp.fetch().fields.size() << endl;

			pp.insert("test", arr);
			pp.exec();

			arr["operation"] = "um";
			pp.update("test", arr);
			pp.exec();

			cout << "Updated: " << pp.affectedRowsCount() << endl;

			arr["operation"] = "mu";
		}

		pp.select("test", select, "`id`<20 or `operation`='um'");
		pp.exec();

		cout << pp.fetch().rows.size() << endl;

		__tuples store = pp.fetch();

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

	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << endl << endl;
	}

#else

	cout << "No SQLite exteinsion was compiled!";

#endif

	return 0;
}
