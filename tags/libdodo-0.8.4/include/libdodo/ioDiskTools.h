/***************************************************************************
 *            ioDiskTools.h
 *
 *  Tue Oct 8 08:19:57 2005
 *  Copyright  2005  Ni@m
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

#ifndef _IODISKTOOLS_H_
#define _IODISKTOOLS_H_

#include <libdodo/directives.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <utime.h>
#include <time.h>
#include <libgen.h>
#include <unistd.h>

#include <libdodo/tools.h>
#include <libdodo/ioDiskToolsEx.h>
#include <libdodo/types.h>

namespace dodo
{
	/**
	 * @enum ioDiskToolsFileTypeEnum defines file type
	 */
	enum ioDiskToolsFileTypeEnum
	{
		IODISKTOOLS_FILETYPE_REGULAR_FILE,
		IODISKTOOLS_FILETYPE_LOCAL_SOCKET,
		IODISKTOOLS_FILETYPE_SYMBOLIC_LINK,
		IODISKTOOLS_FILETYPE_BLOCK_DEVICE,
		IODISKTOOLS_FILETYPE_DIRECTORY,
		IODISKTOOLS_FILETYPE_CHARACTER_DEVICE,
		IODISKTOOLS_FILETYPE_FIFO
	};

	/**
	 * @enum ioDiskPermissionModesEnum defines file permissions
	 */
	enum ioDiskPermissionModesEnum
	{
		IODISKTOOLS_PERM_NONE,

		IODISKTOOLS_PERM_OWNER_READ_ACCESS = 1 << 1,
		IODISKTOOLS_PERM_GROUP_READ_ACCESS = 1 << 2,
		IODISKTOOLS_PERM_OTHER_READ_ACCESS = 1 << 3,

		IODISKTOOLS_PERM_OWNER_WRITE_ACCESS = 1 << 4,
		IODISKTOOLS_PERM_GROUP_WRITE_ACCESS = 1 << 5,
		IODISKTOOLS_PERM_OTHER_WRITE_ACCESS = 1 << 6,

		IODISKTOOLS_PERM_OWNER_EXECUTE_ACCESS = 1 << 7,
		IODISKTOOLS_PERM_GROUP_EXECUTE_ACCESS = 1 << 8,
		IODISKTOOLS_PERM_OTHER_EXECUTE_ACCESS = 1 << 9,

		IODISKTOOLS_PERM_STICKY_ACCESS = 1 << 10,

		IODISKTOOLS_PERM_SUID_ACCESS = 1 << 11,
		IODISKTOOLS_PERM_SGID_ACCESS = 1 << 12,

		IODISKTOOLS_PERM_OWNER_ALL_ACCESS = IODISKTOOLS_PERM_OWNER_READ_ACCESS | IODISKTOOLS_PERM_OWNER_WRITE_ACCESS | IODISKTOOLS_PERM_OWNER_EXECUTE_ACCESS,
		IODISKTOOLS_PERM_GROUP_ALL_ACCESS = IODISKTOOLS_PERM_GROUP_READ_ACCESS | IODISKTOOLS_PERM_GROUP_WRITE_ACCESS | IODISKTOOLS_PERM_GROUP_EXECUTE_ACCESS,
		IODISKTOOLS_PERM_OTHER_ALL_ACCESS = IODISKTOOLS_PERM_OTHER_READ_ACCESS | IODISKTOOLS_PERM_OTHER_WRITE_ACCESS | IODISKTOOLS_PERM_OTHER_EXECUTE_ACCESS,

		IODISKTOOLS_PERM_ALL_ALL_ACCESS = IODISKTOOLS_PERM_OWNER_ALL_ACCESS | IODISKTOOLS_PERM_GROUP_ALL_ACCESS | IODISKTOOLS_PERM_OTHER_ALL_ACCESS,

	};

	/**
	 * @struct __fileInfo defines file info
	 */
	struct __fileInfo
	{
		dodoString name;    ///< file name
		int perm;           ///< file permissions[see ioDiskPermissionModesEnum]
		int type;           ///< file type[see ioDiskToolsFileTypeEnum]
		long size;          ///< file size
		long modTime;       ///< modyfication time
		long accTime;       ///< access time
		int gid;            ///< group id of the file
		int uid;            ///< user id of the file
	};

	/**
	 * @class ioDiskTools provides disk I/O manipulations
	 */
	class ioDiskTools
	{

			friend class ioSocket;

		public:

			/**
			 * copy file/empty dir/...
			 * @param from defines source file/empty directory/...
			 * @param to defines destination file/empty directory/...
			 * @param force defines overwrite condition
			 * @note if destination has trailing slash it will treat destination as a diroctory and copy into it
			 */
			static void copy(const dodoString &from, const dodoString &to, bool force = false);

			/**
			 * copy file/dir
			 * @param from defines source file/directory/...
			 * @param to defines destination file/directory/...
			 * @param force defines overwrite condition
			 * @note if destination has trailing slash it will treat destination as a diroctory and copy into it
			 */
			static void copyDir(const dodoString &from, const dodoString &to, bool force = false);

			/**
			 * append string to file
			 * @param path defines path to file
			 * @param content defines string that will be appended to the file
			 */
			static void appendToFile(const dodoString &path, const dodoString &content);

			/**
			 * append array of strings to file
			 * @param path defines path to file
			 * @param content defines array of strings that will be appended to the file
			 */
			static void appendToFile(const dodoString &path, const dodoStringArray &content);
            
			/**
			 * write string to file
			 * @param path defines path to file
			 * @param content defines string that will be written to the file
			 */                     
			static void writeToFile(const dodoString &path, const dodoString &content);
			
			/**
			 * write array of strings to file
			 * @param path defines path to file
			 * @param content defines array of strings that will be written to the file
			 */                     
			static void writeToFile(const dodoString &path, const dodoStringArray &content);
            
			/**
			 * @return basename of path string
			 * @param path defines path string
			 */
			static dodoString lastname(const dodoString &path);

			/**
			 * @return dirname of path string
			 * @param path defines path string
			 */
			static dodoString dirname(const dodoString &path);

			/**
			 * @return file content
			 * @param path defines path to file
			 */
			static dodoString getFileContents(const dodoString &path);

			/**
			 * @return file content
			 * @param path defines path to file
			 */
			static dodoStringArray getFileContentsArr(const dodoString &path);

			/**
			 * delete file/empty directory/...
			 * @param path defines path to file/empty directory/...
			 * @param force defines unlink condition[if it is true and nothing already exists do not say anything]
			 */
			static void unlink(const dodoString &path, bool force = true);

			/**
			 * rename file
			 * @param oldPath defines source file
			 * @param newPath defines destination file
			 */
			static void rename(const dodoString &oldPath, const dodoString &newPath);

			/**
			 * set access and modyfication time of file
			 * @param path defines path to file
			 * @param time defines timestams
			 * @note if time is defined as (-1) - as by default timestamp of current timepoint is used
			 */
			static void touch(const dodoString &path, int time = -1);

			/**
			 * make fifo(pipe)
			 * @param path defines path to file
			 * @param permissions defines fifo permissions[see ioDiskPermissionModesEnum]
			 */
			static void mkfifo(const dodoString &path, int permissions = IODISKTOOLS_PERM_OWNER_ALL_ACCESS);

			/**
			 * make directory
			 * @param path defines path to directory
			 * @param permissions defines directory permissions[see ioDiskPermissionModesEnum]
			 * @param force defines overwrite condition[if it is true and directory already exists do not say anything]
			 */
			static void mkdir(const dodoString &path, int permissions = IODISKTOOLS_PERM_OWNER_ALL_ACCESS, bool force = true);

			/**
			 * make directory recursively
			 * @param path defines path to directory
			 * @param permissions defines directory permissions[see ioDiskPermissionModesEnum]
			 */
			static void mkdirRecursive(const dodoString &path, int permissions = IODISKTOOLS_PERM_OWNER_ALL_ACCESS);

			/**
			 * delete files, non empty directory
			 * @param path defines path to file/directory/...
			 * @param force defines unlink condition[if it is true and nothing already exists do not say anything]
			 */
			static void rm(const dodoString &path, bool force = true);

			/**
			 * @return type of file
			 * @param path defines path to file/directory/...
			 */
			static int getFileType(const dodoString &path);

			/**
			 * change permissions
			 * @param path defines path to file/directory/...
			 * @param permissions defines file/directory/... permissions[see ioDiskPermissionModesEnum]
			 */
			static void chmod(const dodoString &path, int permissions);

			/**
			 * @return file/directory/... permissions
			 * @param path defines path to file/directory/...
			 */
			static int getPermissions(const dodoString &path);

			/**
			 * @return file size
			 * @param path indicates path what to describe
			 */
			static long getSize(const dodoString &path); ///< in bytes

			/**
			 * @return file access time
			 * @param path defines path to file
			 */
			static long getAccTime(const dodoString &path);

			/**
			 * @return file modification time
			 * @param path defines path to file
			 */
			static long getModTime(const dodoString &path);

			/**
			 * get original path of the file
			 * @return original path
			 * @param path defines path to symbolic link
			 */
			static dodoString followSymlink(const dodoString &path);

			/**
			 * create symbolic link
			 * @param oldPath defines path to file
			 * @param newPath defines path to symlink
			 * @param force defines overwrite condition[if it is true and link already exists do not say anything and replace]
			 */
			static void symlink(const dodoString &oldPath, const dodoString &newPath, bool force = true);

			/**
			 * create hard link
			 * @param oldPath defines path to file
			 * @param newPath defines path to link
			 */
			static void link(const dodoString &oldPath, const dodoString &newPath);

			/**
			 * change owner of the file/directory/...
			 * @param path defines path to file/directory/...
			 * @param uid defines user id
			 */
			static void chown(const dodoString &path, int uid);

			/**
			 * change group owner of the file/directory/...
			 * @param path defines path to file/directory/...
			 * @param gid defines group id
			 */
			static void chgrp(const dodoString &path, int gid);

			/**
			 * @return true if file/directory/... exists
			 * @param path defines path to file/directory/...
			 */
			static bool exists(const dodoString &path);

			/**
			 * @return user id of the file/directory/...
			 * @param path defines path to file/directory/...
			 */
			static int getUserOwner(const dodoString &path);

			/**
			 * @return group id of the file/directory/...
			 * @param path defines path to file/directory/...
			 */
			static int getGroupOwner(const dodoString &path);

			/**
			 * @return file info[see __fileInfo]
			 * @param path defines path to file
			 */
			static __fileInfo getFileInfo(const dodoString &path);

			/**
			 * @return info of files in directory[see __fileInfo]
			 * @param path defines path to file
			 */
			static dodoArray<__fileInfo> getDirInfo(const dodoString &path);

		protected:

			/**
			 * @return system understandable permissions
			 * @param permission defines user[libdodo] understandable permissions
			 */
			static int toRealPermission(int permission);
			
			/**
			 * write string to file
			 * @param path defines path to file
			 * @param content defines string that will be written to the file
			 * @param mode defines mode to open file
			 */                     
			static void _writeToFile(const dodoString &path, const dodoString &content, const char *mode);

			/**
			 * writes array of strings to file
			 * @param path defines path to file
			 * @param content defines array of strings that will be written to the file
			 * @param mode defines mode to open file
			 */                     
			static void _writeToFile(const dodoString &path, const dodoStringArray &content, const char *mode);
	};

};

#endif