#include <tools.h>
#include <flushDisk.h>

using namespace dodo;
using namespace std;

void
baseHandler(errorModuleEnum module, baseEx *ex, void *data)
{
	cout << "baseHandler: " << ex->baseErrstr << endl;
}

int main(int argc, char **argv)
{	

	try
	{
		//tools tl;
		
		/*baseEx::setErrorHandler(ERRMODULE_FLUSHDISK,&baseHandler,NULL);
		if(!baseEx::setErrorHandler(ERRMODULE_FLUSHDISK,"./module",NULL))
			cout << "WTF";*/
		
		//cout << flushDiskTools::getFileContent("test.cc");
		
		//cout << "Что происходит?" << endl;
		
		/*for (register int i(0);i<10000;++i)
			tl.codesetConversion(flushDiskTools::getFileContent("test.cc"),"cp1251","utf-8");*/
		
		/*tl.codesetConversion(flushDiskTools::getFileContent("test.cc"),"cp1251","utf-8");
		for (register int i(0);i<9999;++i)
			tl.reCodesetConversion(flushDiskTools::getFileContent("test.cc"));*/
	
		//cout << flushDiskTools::getFileContent("Makefile").size() << endl;
		//cout << tl.zCompress(flushDisk::getFileContent("Makefile"),9).size() << endl;
	
		//cout << tl.zDecompress(tl.zCompress(flushDiskTools::getFileContent("test.cc")));
		
		/*cout << tools::decodeURL(tools::encodeURL("@!()HEY, that's working!")) << endl;
		cout << tools::encodeURL("@!()HEY, that's working!") << endl;
		
		cout << "real size of test is : " << flushDiskTools::getSize("test") << endl;
		
		cout << tools::encodeASCII85("HEY, that's working!") << endl;
		cout << tools::decodeASCII85(tools::encodeASCII85("HEY, that's working!")) << endl;
		cout << "size of test ASCII85-encoded : " <<  tools::encodeASCII85(flushDiskTools::getFileContent("test")).size() << endl;
		
		cout << tools::encodeBase64("HEY, that's working!") << endl;
		cout << tools::decodeBase64(tools::encodeBase64("HEY, that's working!")) << endl;
		cout << "size of test base64-encoded : " <<  tools::encodeBase64(flushDiskTools::getFileContent("test")).size() << endl;*/
		
		//cout << tools::parseURL("http://hoho:hihi@niam").password << endl;
		//cout << tools::parseURL("http://hoho:hihi@niam").login << endl;
		//cout << tools::parseURL("hoho@niam").login << endl;
		//cout << tools::parseURL("ftp://hoho:hihi@niam").protocol << endl;
		
//		cout << tools::parseURL("ftp://hoho:hihi@niam").host << endl;
cout << tools::parseURL("ftp://:hihi@niam").login << endl;
cout << tools::parseURL("ftp://:hihi@niam").password << endl;
		//cout << tools::parseURL("ftp://niam").host << endl;
			
	}
    catch(baseEx ex)
    {
    }
    catch(std::exception ex)
    {
    	cout << endl << ex.what() << endl;
    }
    
	return 0;
}

