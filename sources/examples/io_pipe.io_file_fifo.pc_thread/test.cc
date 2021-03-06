/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace dodo::pc::execution;

using namespace std;

int
threadRead(void *data)
{
    cout << "READER->\n";
    cout.flush();

    dodo_try {
        dodo::string str;

        io::channel *pipe = (io::channel *)data;

        str = pipe->readString();
        cout << "%" << str << "%\n";
        cout.flush();

        str = pipe->readString();
        cout << "%" << str << "%\n";
        cout.flush();

        pipe->bs = tools::string::stringToUL(str);
        str = pipe->read();
        cout << "%MD5: " << tools::code::MD5Hex(str) << "%\n";
        cout.flush();
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    cout << "<-READER\n";
    cout.flush();

    return 0;
}

int
threadWrite(void *data)
{
    cout << "WRITER->\n";
    cout.flush();

    dodo_try {
        dodo::string str;

        io::channel *pipe = (io::channel *)data;

        pipe->writeString(tools::time::byFormat("%H:%M:%S", tools::time::now()) + "\n");
        pipe->flush();

        str = tools::filesystem::fileContents("test.cc");

        pipe->writeString(tools::string::ulToString(str.size()));
        pipe->flush();

        pipe->bs = str.size();
        pipe->write(str);
        pipe->flush();
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    cout << "<-WRITER\n";
    cout.flush();

    return 0;
}

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
        manager threads;

        cout << "\n~~using one pipe for the thread~~\n";

        io::pipe pipe1;
        pipe1.open();

            ///< write first to avoid deadlock due to io::pipe is threadsafe and here one object is used
        threads.run(threads.add(thread(&threadWrite,
                                       (void *)dynamic_cast<io::channel *>(&pipe1),
                                       ON_DESTRUCTION_STOP)));
        tools::os::sleep(1);
        threads.run(threads.add(thread(&threadRead,
                                       (void *)dynamic_cast<io::channel *>(&pipe1),
                                       ON_DESTRUCTION_STOP)));

        threads.wait();

        cout << "\n~~using original cloned pipe for one thread and copy for the second~~\n";
        pipe1.bs = 4096;

        io::pipe pipe2 = pipe1;
        //or clone
        //pipe2.clone(pipe1);

            ///< use a copy, so no need to keep an order
        threads.run(threads.add(thread(&threadRead,
                                       (void *)dynamic_cast<io::channel *>(&pipe1),
                                       ON_DESTRUCTION_STOP)));
        threads.run(threads.add(thread(&threadWrite,
                                       (void *)dynamic_cast<io::channel *>(&pipe2),
                                       ON_DESTRUCTION_STOP)));

        threads.wait();

        cout << "\n~~using FIFO file, opened to read for one thread and to write for the second~~\n";

        io::file::fifo fifo1;
        fifo1.open("fifo.file", io::file::fifo::OPEN_MODE_READ_OPENNONBLOCK);
        io::file::fifo fifo2;
        fifo2.open("fifo.file", io::file::fifo::OPEN_MODE_WRITE);

        threads.run(threads.add(thread(&threadWrite,
                                       (void *)dynamic_cast<io::channel *>(&fifo2),
                                       ON_DESTRUCTION_STOP)));
        threads.run(threads.add(thread(&threadRead,
                                       (void *)dynamic_cast<io::channel *>(&fifo1),
                                       ON_DESTRUCTION_STOP)));

        threads.wait();
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    return 0;
}
