/***************************************************************************
 *            toolsFilesystem.h
 *
 *  Tue Oct 8 2005
 *  Copyright  2005  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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

#ifndef _TOOLSFILESYSTEM_H_
#define _TOOLSFILESYSTEM_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace tools {
        /**
         * @class filesystem
         * @brief provides disk I/O manipulations
         */
        class filesystem {
          public:

            /**
             * @enum fileEnum defines file type
             */
            enum fileEnum {
                FILE_REGULAR_FILE,
                FILE_LOCAL_SOCKET,
                FILE_SYMBOLIC_LINK,
                FILE_BLOCK_DEVICE,
                FILE_DIRECTORY,
                FILE_CHARACTER_DEVICE,
                FILE_FIFO
            };

            /**
             * @enum permissionEnum defines file permissions
             */
            enum permissionEnum {
                PERMISSION_NONE,

                PERMISSION_OWNER_READ_ACCESS = 1 << 1,
                PERMISSION_GROUP_READ_ACCESS = 1 << 2,
                PERMISSION_OTHER_READ_ACCESS = 1 << 3,

                PERMISSION_OWNER_WRITE_ACCESS = 1 << 4,
                PERMISSION_GROUP_WRITE_ACCESS = 1 << 5,
                PERMISSION_OTHER_WRITE_ACCESS = 1 << 6,

                PERMISSION_OWNER_EXECUTE_ACCESS = 1 << 7,
                PERMISSION_GROUP_EXECUTE_ACCESS = 1 << 8,
                PERMISSION_OTHER_EXECUTE_ACCESS = 1 << 9,

                PERMISSION_STICKY_ACCESS = 1 << 10,

                PERMISSION_SUID_ACCESS = 1 << 11,
                PERMISSION_SGID_ACCESS = 1 << 12,

                PERMISSION_OWNER_ALL_ACCESS = PERMISSION_OWNER_READ_ACCESS | PERMISSION_OWNER_WRITE_ACCESS | PERMISSION_OWNER_EXECUTE_ACCESS,
                PERMISSION_GROUP_ALL_ACCESS = PERMISSION_GROUP_READ_ACCESS | PERMISSION_GROUP_WRITE_ACCESS | PERMISSION_GROUP_EXECUTE_ACCESS,
                PERMISSION_OTHER_ALL_ACCESS = PERMISSION_OTHER_READ_ACCESS | PERMISSION_OTHER_WRITE_ACCESS | PERMISSION_OTHER_EXECUTE_ACCESS,

                PERMISSION_ALL_ALL_ACCESS = PERMISSION_OWNER_ALL_ACCESS | PERMISSION_GROUP_ALL_ACCESS | PERMISSION_OTHER_ALL_ACCESS,
            };

            /**
             * @struct __file__
             * @brief defines file info
             */
            struct __file__ {
                dodo::string name;    ///< file name
                int        perm;    ///< file permissions, @see filesystem::permissionModesEnum
                int        type;    ///< file type, @see filesystem::fileEnum
                long       size;    ///< file size
                long       modTime; ///< modyfication time
                long       accTime; ///< access time
                int        gid;     ///< group id of the file
                int        uid;     ///< user id of the file
            };

            /**
             * copy file/empty dir/...
             * @param from defines source file/empty directory/...
             * @param to defines destination file/empty directory/...
             * @param force defines overwrite condition
             * @note if destination has trailing slash it will treat destination as a diroctory and copy into it
             */
            static void copy(const dodo::string &from,
                             const dodo::string &to,
                             bool             force = false);

            /**
             * copy file/dir
             * @param from defines source file/directory/...
             * @param to defines destination file/directory/...
             * @param force defines overwrite condition
             * @note if destination has trailing slash it will treat destination as a diroctory and copy into it
             */
            static void copyDir(const dodo::string &from,
                                const dodo::string &to,
                                bool             force = false);

            /**
             * append string to file
             * @param path defines path to file
             * @param content defines string that will be appended to the file
             */
            static void appendToFile(const dodo::string &path,
                                     const dodo::string &content);

            /**
             * append array of strings to file
             * @param path defines path to file
             * @param content defines array of strings that will be appended to the file
             */
            static void appendToFile(const dodo::string      &path,
                                     const dodoStringArray &content);

            /**
             * write string to file
             * @param path defines path to file
             * @param content defines string that will be written to the file
             */
            static void writeToFile(const dodo::string &path,
                                    const dodo::string &content);

            /**
             * write array of strings to file
             * @param path defines path to file
             * @param content defines array of strings that will be written to the file
             */
            static void writeToFile(const dodo::string      &path,
                                    const dodoStringArray &content);

            /**
             * @return basename of path string
             * @param path defines path string
             */
#ifdef basename
#undef basename
#endif
            static dodo::string basename(const dodo::string &path);

            /**
             * @return dirname of path string
             * @param path defines path string
             */
            static dodo::string dirname(const dodo::string &path);

            /**
             * @return file content
             * @param path defines path to file
             */
            static dodo::string fileContents(const dodo::string &path);

            /**
             * @return file content
             * @param path defines path to file
             */
            static dodoStringArray fileContentsInArray(const dodo::string &path);

            /**
             * delete file/empty directory/...
             * @param path defines path to file/empty directory/...
             * @param force defines unlink condition[if it is true and nothing already exists do not say anything]
             */
            static void unlink(const dodo::string &path,
                               bool             force = true);

            /**
             * rename file
             * @param oldPath defines source file
             * @param newPath defines destination file
             */
            static void rename(const dodo::string &oldPath,
                               const dodo::string &newPath);

            /**
             * set access and modyfication time of file
             * @param path defines path to file
             * @param time defines timestams
             * @note if time is defined as (-1) - as by default timestamp of current timepoint is used
             */
            static void touch(const dodo::string &path,
                              int              time = -1);

            /**
             * make fifo(pipe)
             * @param path defines path to file
             * @param permissions defines fifo permissions, @see filesystem::permissionModesEnum
             */
            static void mkfifo(const dodo::string &path,
                               int              permissions = PERMISSION_OWNER_ALL_ACCESS);

            /**
             * make directory recursively
             * @param path defines path to directory
             * @param permissions defines directory permissions, @see filesystem::permissionModesEnum
             */
            static void mkdir(const dodo::string &path,
                              int              permissions = PERMISSION_OWNER_ALL_ACCESS);

            /**
             * delete files, non empty directory
             * @param path defines path to file/directory/...
             * @param force defines unlink condition[if it is true and nothing already exists do not say anything]
             */
            static void rm(const dodo::string &path,
                           bool             force = true);

            /**
             * @return type of file
             * @param path defines path to file/directory/...
             */
            static int fileType(const dodo::string &path);

            /**
             * change permissions
             * @param path defines path to file/directory/...
             * @param permissions defines file/directory/... permissions, @see filesystem::permissionModesEnum
             */
            static void chmod(const dodo::string &path,
                              int              permissions);

            /**
             * @return file/directory/... permissions
             * @param path defines path to file/directory/...
             */
            static int permissions(const dodo::string &path);

            /**
             * @return file size
             * @param path indicates path what to describe
             */
            static long size(const dodo::string &path); ///< in bytes

            /**
             * @return file access time
             * @param path defines path to file
             */
            static long accessTime(const dodo::string &path);

            /**
             * @return file modification time
             * @param path defines path to file
             */
            static long modificationTime(const dodo::string &path);

            /**
             * get original path of the file
             * @return original path
             * @param path defines path to symbolic link
             */
            static dodo::string followSymlink(const dodo::string &path);

            /**
             * create symbolic link
             * @param oldPath defines path to file
             * @param newPath defines path to symlink
             * @param force defines overwrite condition[if it is true and link already exists do not say anything and replace]
             */
            static void symlink(const dodo::string &oldPath,
                                const dodo::string &newPath,
                                bool             force = true);

            /**
             * create hard link
             * @param oldPath defines path to file
             * @param newPath defines path to link
             */
            static void link(const dodo::string &oldPath,
                             const dodo::string &newPath);

            /**
             * change owner of the file/directory/...
             * @param path defines path to file/directory/...
             * @param uid defines user id
             */
            static void chown(const dodo::string &path,
                              int              uid);

            /**
             * change group owner of the file/directory/...
             * @param path defines path to file/directory/...
             * @param gid defines group id
             */
            static void chgrp(const dodo::string &path,
                              int              gid);

            /**
             * @return true if file/directory/... exists
             * @param path defines path to file/directory/...
             */
            static bool exists(const dodo::string &path);

            /**
             * @return user id of the file/directory/...
             * @param path defines path to file/directory/...
             */
            static int userOwner(const dodo::string &path);

            /**
             * @return group id of the file/directory/...
             * @param path defines path to file/directory/...
             */
            static int groupOwner(const dodo::string &path);

            /**
             * @return file info, @see filesystem::__file__
             * @param path defines path to file
             */
            static __file__ file(const dodo::string &path);

            /**
             * @return info of files in directory, @see filesystem::__file__
             * @param path defines path to file
             */
            static dodoArray<__file__> dir(const dodo::string &path);

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
            static void _writeToFile(const dodo::string &path,
                                     const dodo::string &content,
                                     const char       *mode);

            /**
             * writes array of strings to file
             * @param path defines path to file
             * @param content defines array of strings that will be written to the file
             * @param mode defines mode to open file
             */
            static void _writeToFile(const dodo::string      &path,
                                     const dodoStringArray &content,
                                     const char            *mode);
        };
    };
};
#endif
