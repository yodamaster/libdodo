/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>
#include <stdlib.h>

using namespace dodo;
using namespace std;

#ifdef OPENSSL_EXT

using namespace io::network::ssl;

#ifndef IO_WO_XEXEC

void
hook(__xexecCollectedData__ *odata,
	 short int type,
	 void *udata)
{
	using io::__xexecIoChannelCollectedData__;

	__xexecIoChannelCollectedData__ *st = (__xexecIoChannelCollectedData__ *)odata;

	cout << st->buffer << endl;
	cout << dynamic_cast<exchange *>(st->executor)->isAlive() << endl;
}

#endif

void
process(exchange fse)
{
#ifndef IO_WO_XEXEC
	fse.addXExec(XEXEC_ACTION_PREEXEC, ::hook, NULL);
	fse.addXExec(XEXEC_ACTION_POSTEXEC, ::hook, NULL);
#endif

	fse.inSize = 4;
	fse.setInBufferSize(1);
	fse.setOutBufferSize(1);

	fse.outSize = 7;
	fse.writeStream("session");

	if (fse.isAlive())
		fse.writeStream("Alive!");

	dodoString rec = "";
	try
	{
		rec = fse.read();

		cout << rec << rec.size() << endl;
		cout.flush();
		if (rec.compare("exit") == 0)
		{
			fse.close();

			exit(0);
		}
	}
	catch (dodo::exception::basic ex)
	{
		cout << "Smth happened!" << (dodoString)ex << endl;
		cout.flush();
	}
	catch (...)
	{
		cout << "Smth happened!" << endl;
		cout.flush();
	}
}

#endif

int main(int argc, char **argv)
{
	try
	{
#ifdef OPENSSL_EXT

		server sock(io::network::CONNECTION_PROTO_FAMILY_IPV4, io::network::CONNECTION_TRANSFER_TYPE_STREAM);

		io::network::__peerInfo__ info;
		__initialAccept__ fake;

		sock.serve("127.0.0.1",7778,3);

		io::ssl::__certificates__ certs;
		certs.ca = "host.pem";
		certs.cipher = "RC4-MD5";

		sock.setSertificates(certs);

		exchange conn;

		while(true)
		{
			if (sock.accept(fake,info))
			{
					if (sock.isBlocked())
					{
							cout << "PARENT BLOCKED\n";
							cout.flush();
					}

					conn.init(fake);

					process(conn);
			}
		}
#endif
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
