/***************************************************************************
 *            pcSyncProcessDataSingle.cc
 *
 *  Sun Jul 22 23:14:14 2007
 *  Copyright  2007  Ni@m
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

#ifdef XSI_IPC
#include <sys/ipc.h>
#include <sys/sem.h>
#else /*POSIX_SEMAPHORES*/
#include <fcntl.h>
#include <semaphore.h>
#endif
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "pcSyncProcessLock.inline"

#include <libdodo/pcSyncProcessDataSingle.h>
#include <libdodo/pcSyncDataSingle.h>
#include <libdodo/pcSyncProcessDataSingleEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/toolsCode.h>

using namespace dodo::pc::sync::process::data;

single::single(single &sts)
{
}

//-------------------------------------------------------------------

single::single(int a_key) : data(NULL),
							semaphore(new pc::sync::process::__lock__),
							autogenerated(false),
							acquired(false)
{
#ifdef XSI_IPC
	if (a_key == 0) {
		semaphore->key = tools::misc::iRandom();

		autogenerated = true;
	} else
		semaphore->key = a_key;

	semaphore->operations[0].sem_num = 0;
	semaphore->operations[0].sem_flg = 0;

	semaphore->keeper = semget(semaphore->key, 1, S_IRUSR | S_IWUSR | IPC_CREAT);
	if (semaphore->keeper == -1) {
		delete semaphore;

		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_SINGLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	semaphore->control.val = 1;

	if (semctl(semaphore->keeper, 0, SETVAL, semaphore->control) == -1) {
		delete semaphore;

		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_SINGLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#else
	semaphore->key = '/';

	if (a_key == 0) {
		char _key[SH_KEY_SIZE / 2 - 1];
		tools::misc::random(_key, SH_KEY_SIZE / 2 - 1);

		semaphore->key.append(tools::code::binToHex(dodoString(_key, SH_KEY_SIZE / 2 - 1)));

		autogenerated = true;
	} else {
		char _key[4];
		memcpy(_key, &a_key, 4);

		semaphore->key.append(tools::code::binToHex(dodoString(_key, 4)));
	}

	semaphore->keeper = sem_open(semaphore->key.data(), O_CREAT, S_IRUSR | S_IWUSR, 1);
	if (semaphore->keeper == SEM_FAILED) {
		delete semaphore;

		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_SINGLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
#endif
}

//-------------------------------------------------------------------

single::single() : data(NULL),
				   semaphore(new pc::sync::process::__lock__),
				   autogenerated(false),
				   acquired(false)
{
}

//-------------------------------------------------------------------

single::~single()
{
#ifdef XSI_IPC
	if (semaphore->keeper != -1) {
		if (autogenerated) {
			semctl(semaphore->keeper, 0, IPC_RMID);
		} else {
			if (acquired) {
				semaphore->operations[0].sem_op = 1;
				semop(semaphore->keeper, semaphore->operations, 1);
			}
		}
	}
#else
	if (semaphore->keeper != NULL) {
		if (acquired)
			sem_post(semaphore->keeper);

		sem_close(semaphore->keeper);

		if (autogenerated)
			sem_unlink(semaphore->key.data());
	}
#endif

	delete semaphore;
}

//-------------------------------------------------------------------

void
single::close()
{
#ifdef XSI_IPC
	if (semaphore->keeper != -1) {
		if (autogenerated) {
			if (semctl(semaphore->keeper, 0, IPC_RMID) == -1)
				throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		} else if (acquired) {
			semaphore->operations[0].sem_op = 1;
			if (semop(semaphore->keeper, semaphore->operations, 1) == -1)
				throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
	}

	semaphore->keeper = -1;
#else
	if (semaphore->keeper != NULL) {
		if (acquired)
			if (sem_post(semaphore->keeper) == -1)
				throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (sem_close(semaphore->keeper) == -1)
			throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

		if (autogenerated)
			if (sem_unlink(semaphore->key.data()))
				throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	semaphore->keeper = NULL;
#endif
}

//-------------------------------------------------------------------

void
single::open(int a_key)
{
	close();

#ifdef XSI_IPC
	if (a_key == 0) {
		semaphore->key = tools::misc::iRandom();

		autogenerated = true;
	} else {
		semaphore->key = a_key;
	}

	semaphore->operations[0].sem_num = 0;
	semaphore->operations[0].sem_flg = 0;
	semaphore->keeper = semget(semaphore->key, 1, S_IRUSR | S_IWUSR | IPC_CREAT);
	if (semaphore->keeper == -1)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	semaphore->control.val = 1;
	if (semctl(semaphore->keeper, 0, SETVAL, semaphore->control) == -1)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#else
	semaphore->key = '/';

	if (a_key == 0) {
		char _key[SH_KEY_SIZE / 2 - 1];
		tools::misc::random(_key, SH_KEY_SIZE / 2 - 1);

		semaphore->key.append(tools::code::binToHex(dodoString(_key, SH_KEY_SIZE / 2 - 1)));

		autogenerated = true;
	} else {
		char _key[4];
		memcpy(_key, &a_key, 4);

		semaphore->key.append(tools::code::binToHex(dodoString(_key, 4)));
	}

	semaphore->keeper = sem_open(semaphore->key.data(), O_CREAT, S_IRUSR | S_IWUSR, 1);
	if (semaphore->keeper == SEM_FAILED)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_OPEN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
}

//-------------------------------------------------------------------

void
single::remove(int a_key)
{
#ifdef XSI_IPC
	int keeper = semget(a_key, 1, S_IRUSR | S_IWUSR);
	if (keeper == -1)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_REMOVE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (semctl(keeper, 0, IPC_RMID) == -1)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_REMOVE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
#else
	dodoString key = "/";

	char _key[4];
	memcpy(_key, &a_key, 4);

	key.append(tools::code::binToHex(dodoString(_key, 4)));

	if (sem_unlink(key.data()) == -1)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_REMOVE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#endif
}

//-------------------------------------------------------------------

void
single::set(void *a_data)
{
#ifdef XSI_IPC
	if (semaphore->keeper == -1)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_SET, exception::ERRNO_LIBDODO, SINGLEEX_NOTOPENED, PCSYNCPROCESSDATASINGLEEX_NOTOPENED_STR, __LINE__, __FILE__);

	semaphore->operations[0].sem_op = -1;
	if (semop(semaphore->keeper, semaphore->operations, 1) != 0)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_SET, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	data = a_data;

	semaphore->operations[0].sem_op = 1;
	if (semop(semaphore->keeper, semaphore->operations, 1) != 0)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_SET, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
#else
	if (semaphore->keeper == NULL)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_SET, exception::ERRNO_LIBDODO, SINGLEEX_NOTOPENED, PCSYNCPROCESSDATASINGLEEX_NOTOPENED_STR, __LINE__, __FILE__);

	if (sem_wait(semaphore->keeper) != 0)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_SET, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	data = a_data;

	if (sem_post(semaphore->keeper) != 0)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_SET, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
#endif
}

//-------------------------------------------------------------------

void
single::remove()
{
	set(NULL);
}

//-------------------------------------------------------------------

void *
single::acquire(unsigned long microseconds)
{
#ifdef XSI_IPC
	if (semaphore->keeper == -1)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_LIBDODO, SINGLEEX_NOTOPENED, PCSYNCPROCESSDATASINGLEEX_NOTOPENED_STR, __LINE__, __FILE__);

	if (microseconds == 0) {
		semaphore->operations[0].sem_op = -1;

		if (semop(semaphore->keeper, semaphore->operations, 1) != 0)
			throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	} else {
		bool locked = true;
		unsigned long slept = 0;
		timespec timeout = {
			0, 10
		};

		semaphore->operations[0].sem_op = -1;
		semaphore->operations[0].sem_flg = IPC_NOWAIT;

		while (locked) {
			if (semop(semaphore->keeper, semaphore->operations, 1) != 0) {
				if (errno != EAGAIN) {
					semaphore->operations[0].sem_flg = 0;

					throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				if (nanosleep(&timeout, NULL) == -1) {
					semaphore->operations[0].sem_flg = 0;

					throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
				}

				slept += 1;

				if (slept > microseconds) {
					semaphore->operations[0].sem_flg = 0;

					throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_LIBDODO, SINGLEEX_CANNOTLOCK, PCSYNCPROCESSDATASINGLEEX_CANNOTLOCK_STR, __LINE__, __FILE__);
				}
			} else
				locked = false;
		}
	}
#else
	if (semaphore->keeper == NULL)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_LIBDODO, SINGLEEX_NOTOPENED, PCSYNCPROCESSDATASINGLEEX_NOTOPENED_STR, __LINE__, __FILE__);

	if (microseconds == 0) {
		if (sem_wait(semaphore->keeper) != 0)
			throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	} else {
		bool locked = true;
		unsigned long slept = 0;
		timespec timeout = {
			0, 10
		};

		while (locked) {
			if (sem_trywait(semaphore->keeper) != 0) {
				if (errno != EAGAIN)
					throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				if (nanosleep(&timeout, NULL) == -1)
					throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

				slept += 1;

				if (slept > microseconds)
					throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_ACQUIRE, exception::ERRNO_LIBDODO, SINGLEEX_CANNOTLOCK, PCSYNCPROCESSDATASINGLEEX_CANNOTLOCK_STR, __LINE__, __FILE__);
			} else
				locked = false;
		}
	}
#endif

	acquired = true;

	return data;
}

//-------------------------------------------------------------------

void
single::release()
{
#ifdef XSI_IPC
	if (semaphore->keeper == -1)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_RELEASE, exception::ERRNO_LIBDODO, SINGLEEX_NOTOPENED, PCSYNCPROCESSDATASINGLEEX_NOTOPENED_STR, __LINE__, __FILE__);

	semaphore->operations[0].sem_op = 1;

	if (semop(semaphore->keeper, semaphore->operations, 1) != 0)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_RELEASE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
#else
	if (semaphore->keeper == NULL)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_RELEASE, exception::ERRNO_LIBDODO, SINGLEEX_NOTOPENED, PCSYNCPROCESSDATASINGLEEX_NOTOPENED_STR, __LINE__, __FILE__);

	if (sem_post(semaphore->keeper) != 0)
		throw exception::basic(exception::MODULE_PCSYNCPROCESSDATASINGLE, SINGLEEX_RELEASE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
#endif

	acquired = false;
}

//-------------------------------------------------------------------

