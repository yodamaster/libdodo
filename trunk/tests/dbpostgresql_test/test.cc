#include <libdodo/baseEx.h>
#include <libdodo/dbPostgresql.h>

#include <iostream>

using namespace dodo;

using namespace std;

void
hook(void *object, short int type, void *data)
{
	cout << "executed" << endl;
}

int main(int argc, char **argv)
{
#ifdef POSTGRESQL_EXT
	
		dbPostgresql pp;	
		
		try
		{
			pp.addPreExec(&hook,NULL);
			
			__dbInfo info;
			
			info.db = "test";
			info.host = "localhost";
			info.port = 5432;
			info.user = "postgres";
			
			pp.setDbInfo(info);
			pp.connect();	
	
			pp.deleteTable("test");
			pp.exec();
	
			__fieldInfo fi;
			fi.name = "date";
			fi.type = DBBASE_FIELDTYPE_TEXT;
			
			__tableInfo ti;
			ti.name = "leg";
			ti.fields.push_back(fi);
			
			fi.name = "operation";
			fi.type = DBBASE_FIELDTYPE_TEXT;		
			ti.fields.push_back(fi);
			
			fi.name = "id";
			fi.type = DBBASE_FIELDTYPE_INTEGER;
			fi.flag = DBBASE_FIELDFLAG_NULL;
			ti.fields.push_back(fi);		
			
			pp.createTable(ti);
			cout << pp.queryCollect() << endl;
			pp.exec();
			
			dodoStringMap arr;
			arr["date"] = "2005-07-08";
			arr["operation"] = "mu";
			
			dodoStringArray select;
			select.push_back("date");
			select.push_back("operation");
			
			for (int i=0;i<10;i++)
			{
				pp.select("test",select,"id<20 or operation='um'");
				pp.exec();
				
				pp.fetch();
				
				pp.insert("test",arr);
				pp.exec();
				
				arr["operation"] = "um";
				pp.update("test",arr);
				arr["operation"] = "mu";
				pp.exec();
			}
			
			pp.select("test",select,"operation='um'");cout << pp.queryCollect() << endl;
			pp.exec();
			
			cout << pp.fetch().rows.size() << endl;
			
			__dbStorage store = pp.fetch();
			
			dodoArray<dodoStringArray>::iterator i(store.rows.begin()), j(store.rows.end());
			
			dodoStringArray::iterator m, n;
			
			for (;i!=j;i++)
			{
				m = i->begin();
				n = i->end();
				for (;m!=n;m++)
					cout << *m << "\t";
				cout << endl;	
			}
			
		}
		catch(baseEx ex)
		{
			cout << (string)ex << ex.line;
		}	
	
#else
	
		cout << "No Postresql extension was compiled";
		
#endif
	
	return 0;
}