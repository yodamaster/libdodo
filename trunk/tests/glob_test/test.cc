#include <libdodo/tools.h>
#include <libdodo/ioDisk.h>
#include <libdodo/baseEx.h>
#include <libdodo/ioDiskTools.h>
#include <libdodo/ioSocket.h>

#include <iostream>

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
		cout << tools::decodeBase64("yujl4j3P7uvy4OLg") << endl;
		
		cout << "Что происходит?" << endl;
		cout << tools::codesetConversion("Что происходит?","cp1251","utf-8") << endl;
		
		cout << "MD5 of \"\"" << endl;
		dodoString md5 = tools::MD5("");

		for (int i = 0; i < 16; i++)
 			printf ("%02x", (unsigned char)md5[i]);
 			
		cout << endl;

		//tools::mail("127.0.0.1",IOSOCKETOPTIONS_PROTO_FAMILY_IPV4,25,"niam.niam@gmail.com","root@winerrorfixer.com", "test", "test");
		
		baseEx::setErrorHandler(ERRMODULE_IODISK,&baseHandler,NULL);
		if(!baseEx::setErrorHandler(ERRMODULE_IODISK,"./module",NULL))
			cout << "WTF";
		
		cout << ioDiskTools::getFileContent("test.cc");
		
		
		for (register int i(0);i<10000;++i)
			tools::codesetConversion(ioDiskTools::getFileContent("test.cc"),"cp1251","utf-8");
		
		cout << ioDiskTools::getFileContent("Makefile").size() << endl;
		cout << tools::zCompress(ioDiskTools::getFileContent("Makefile"),9).size() << endl;
	
		cout << tools::zDecompress(tools::zCompress(ioDiskTools::getFileContent("test.cc")));
		
		cout << tools::decodeURL(tools::encodeURL("@!()HEY, that's working!")) << endl;
		cout << tools::encodeURL("@!()HEY, that's working!") << endl;
		
		cout << "real size of test is : " << ioDiskTools::getSize("test") << endl;
		
		cout << tools::encodeASCII85("HEY, that's working!") << endl;
		cout << tools::decodeASCII85(tools::encodeASCII85("HEY, that's working!")) << endl;
		cout << "size of test ASCII85-encoded : " <<  tools::encodeASCII85(ioDiskTools::getFileContent("test")).size() << endl;
		
		cout << tools::encodeBase64("HEY, that's working!") << endl;
		cout << tools::decodeBase64(tools::encodeBase64("HEY, that's working!")) << endl;
		cout << tools::decodeBase64("PDQxMjg3NjA3ODEuMTMzODQ4NjJAd2luZXJyb3JmaXhlci5jb20+") << endl;
		cout << "size of test base64-encoded : " <<  tools::encodeBase64(ioDiskTools::getFileContent("test")).size() << endl;
		
		dodoString tt = "ftp://hihi:hoho@niam.mu:32/init.cgi?gonivo=true&work=true";

		cout << tools::parseURL(tt).host << endl;
		cout << tools::parseURL(tt).login << endl;
		cout << tools::parseURL(tt).password << endl;
		cout << tools::parseURL(tt).path << endl;
		cout << tools::parseURL(tt).port << endl;
		cout << tools::parseURL(tt).protocol << endl;
		cout << tools::parseURL(tt).request << endl;

		cout << ioDiskTools::getFileContent("Makefile").size() << endl;
		cout << tools::bzCompress(ioDiskTools::getFileContent("Makefile"),9).size() << endl;
		for (int i(0);i<10000;++i)
			tools::bzDecompress(tools::bzCompress(ioDiskTools::getFileContent("Makefile"),9));
			
		dodoString t = "abcddF";
		stringTools::replace("cd","WW",t);
		cout << t << endl;	
	}
    catch(baseEx ex)
    {
    	cout << endl << ex.line << "!!" << ex.baseErrno << "!!" << endl;
    }
    catch(std::exception ex)
    {
    	cout << endl << ex.what() << endl;
    }
    
	return 0;
}
