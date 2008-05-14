#include <libdodo/baseEx.h>
#include <libdodo/cgiServer.h>
#include <libdodo/cgiProcessor.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/cgiFastServer.h>
#include <libdodo/ipcThreadSharedDataGuard.h>

#include <iostream>

using namespace std;

#ifdef FASTCGI_EXT

using namespace dodo;
using cgi::fast::serverExchange;
using dodo::ipc::thread::shared::dataGuard;

	dataGuard sh;

	void 
	cgif(serverExchange *fcgi)
	{
		using namespace cgi;

		server cgit(fcgi, true);
		cgit.setCookie("test","Ni@m");
		cgit.printHeaders();
			
		int *inc = (int *)sh.acquire();
		(*inc)++;
		sh.release();
		
		fcgi->writeStreamString("!" + tools::string::iToString(*inc) + "!<br>");
		fcgi->writeStreamString("!" + cgit.GET["a"] + "!<br>");
		fcgi->writeStreamString("!" + cgit.POST["hidden"] + "!<br>");
		fcgi->writeStreamString("!" + cgit.POST["test"] + "!<br>");
		fcgi->writeStreamString("!" + cgit.ENVIRONMENT[SERVER_ENVIRONMENT_QUERYSTRING] + "<br>");
		fcgi->writeStreamString("!" + cgit.COOKIES["test"] + "<br>");
		fcgi->writeStreamString("!" + tools::string::iToString(cgit.FILES["file"].size) + "<br>");
		
		fcgi->writeStreamString("!!!\n\n\n");
		
		try
		{
			processor cgip(cgit);
			cgip.assign("test","hoho");
			cgip.assign("show","That's works!");
			
			dodoStringArray arr;
			arr.push_back("one");
			arr.push_back("two");
			arr.push_back("three");
			cgip.assign("arr",arr);
			
			dodoStringMap arr1;
			arr1["one"] = "one";
			arr1["two"] = "two";
			arr1["three"] = "three";
			cgip.assign("arr1",arr1);
			
			dodoArray<dodoStringMap> arr2;
			arr2.push_back(arr1);
			arr1["one"] = "three";
			arr2.push_back(arr1);
			cgip.assign("arr2",arr2);
		
			cgip.assign("one","one");
		
			cgip.display("test.tpl");
		}
		catch(baseEx ex)
		{
			fcgi->writeStreamString(ex.baseErrstr + " " + tools::string::lToString(ex.line));
		}	
		
		fcgi->writeStreamString("\n\n\n!!!");
	}

#endif

int main(int argc, char **argv)
{	
#ifdef FASTCGI_EXT
	
	try
	{
		int *shared = new int(1);
		sh.set((void *)shared);
		
		using namespace cgi::fast;

		server cf;
		if (!cf.isFastCgi())
		{
			cout << "Not a fastCGI.";
			cout.flush();
		}
		
		cf.setHandler(&cgif);
	
		cf.listen();

		delete shared;
	}
	catch (baseEx &ex)
	{
		cout << endl << ex.baseErrstr << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
	}

#else
	
		cout << "No fastCGI extension was compiled!";
		
#endif
		
	return 0;
}
