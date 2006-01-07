#include <flushSocket.h>
#include <flushSocketTools.h>
#include <systemThreads.h>
#include <systemThreadShares.h>

using namespace dodo;

using namespace std;

void *
process(void *data)
{
	flushSocketExchange *t_fse = (flushSocketExchange *)data;
	flushSocketExchange fse(*t_fse);
	
	if (fse.isBlocked())
		std::cout << "CHILD BLOCKED\n";
	
	fse.inSize = 4;
	fse.setInBufferSize(1);
	fse.setOutBufferSize(1);
	
//	fse.outSize = 7;
//	fse.autoOutSize = false;
	fse.sendStreamString("dasdasd");
	
	std::string q = "";
	try
	{
		fse.receiveStreamString(q);
		cout << q << q.size() << endl;
//		if (q.compare("exit")==0)
//			return false;
	}
	catch (baseEx ex)
	{
		cout << "Smth happened!" << ex << endl;
	}
	
//	return true;
}

int main(int argc, char **argv)
{
//#define DATAGRAM	
	try
	{	
		stringArr ifaces = flushSocketTools::getInterfacesNames();
		for (int i(0);i<ifaces.size();i++)
		{
			try
			{
				cout << ifaces[i] << ":\t" << flushSocketTools::getInterfaceInfo(ifaces[i]).broadcast << endl;	
			}
			catch (baseEx ex)
			{
				cout << ifaces[i] << ":\t" << ex << endl;
			}
		}
		
		cout << flushSocketTools::getInterfaceInfo("lo").hwaddr << endl;
		cout << flushSocketTools::getInterfaceInfo("eth0").address << endl;
			
		flushSocket sock(true,PROTO_FAMILY_IPV4/*PROTO_FAMILY_IPV6*//*PROTO_FAMILY_UNIX_SOCKET*/,TRANSFER_TYPE_STREAM);
		
		
/*		sock.connect("127.0.0.1",21,ex1);
		ex1.close();
		
		sock.connect("127.0.0.1",21,ex1);

		flushSocketExchange ex2;
		flushSocketExchange ex2 = ex1;
		sock.connect("127.0.0.1",21,ex2);
*/
		__connInfo info;
		__initialAccept fake;
		sock.setSockOption(SOCKET_REUSE_ADDRESS,true);
		sock.setLingerSockOption(SOCKET_HARD_CLOSE);
		
		//sock.blockInherited = true;
		
		sock.bindNListen("127.0.0.1",7777,3);
		//sock.bindNListen("::",7777);
		//sock.bindNListen("./sock",10,true);
		
		sock.block(true);
		
		flushSocketExchange conn1;
		int i = 0;

		systemThreads th;
		systemThreadShares sh;
		
		int shPos = sh.add((void *)&conn1);
		
		int pos = th.add(process,(void *)&conn1);
		
		while(true)
		{
			if (sock.accept(fake,info))
			{
				if (sock.isBlocked())
					std::cout << "PARENT BLOCKED\n";
					
				conn1.init(fake);
				cout << info.port << endl;
				th.run(pos,true);
				//if (!process(conn1))
				//	break;
			}
		}
		
		
		//flushSocketTools::setLocalName("BUBU");
		
		/*cout << flushSocketTools::getLocalName() << endl;
		cout << flushSocketTools::getHostInfo("192.168.0.1").addresses[0] << endl;
		
		flushSocketTools *pointer[10];
		pointer[0] = &sock;
		cout << pointer[0]->getHostInfo("elessar.mu").addresses[0] << endl;*/
		
	}
	catch(baseEx ex)
	{
		cout << ex << "\t" << ex.line << endl;
	}
	
	return 0;
}
