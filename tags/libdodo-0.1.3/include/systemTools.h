/***************************************************************************
 *            system.h
 *
 *  Sat Nov 19 15:19:57 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <iostream>

#include <directives.h>

#include <systemToolsEx.h>
#include <types.h>

namespace dodo
{

	/**
	 * @struct __usage contains info about process
	 */
	struct __usage
	{
		long time;///<  amount of processor time of execution in miliseconds
		long mem;///< amount of memmory usage in bytes
	};

	/**
	 * @struct __limits describes system limits
	 */
	struct __limits
	{
		unsigned long current;///< can use current process
		unsigned long max;///< max amount
	};

	/**
	 * @enum systemToolsLimitEnum describes what type of limits you can get
	 */
	enum systemToolsLimitEnum
	{
		SYSTEM_CPUTIME,
		SYSTEM_MAXFILESIZE,
		SYSTEM_MAXMEMUSAGE,
		SYSTEM_MAXSTACK,
		SYSTEM_MAXPROC,
		SYSTEM_MAXOPENFILES
	}; 
	
	/**
	 * @typedef describes handler function on signal
	 */
	typedef void (*signalHandler)(int, siginfo_t *, void *);

	/**
	 * @enum systemSygnalsEnum describes system signals
	 */
	enum systemSignalsEnum
	{
		SIGNAL_HANGUP = 0,
		SIGNAL_INTERRUPT,
		SIGNAL_QUIT,
		SIGNAL_ILLEGAL_INSTRUCTION,
		SIGNAL_TRACE_TRAP,
		SIGNAL_ABORT,
		SIGNAL_BUS_FAULT,
		SIGNAL_FLOATINGPOINT_FAULT,
		SIGNAL_USER_DEFINED1,
		SIGNAL_SEGMENTATION_FAULT,
		SIGNAL_USER_DEFINED2,
		SIGNAL_PIPE_FAULT,
		SIGNAL_ALARM,
		SIGNAL_TERMINATION,
		SIGNAL_STACK_FAULT,
		SIGNAL_CHILD_CHANGED,
		SIGNAL_CONTINUE,
		SIGNAL_KEYBOARD_STOP,
		SIGNAL_CPULIMIT_EXCEEDED,
		SIGNAL_FILESIZE_EXCEEDED,
		SIGNAL_BAD_SYSCALL
	};

	/**
	 * @enum uidTypeEnum describes type of UID
	 * @note real - what ID started process
	 * effective - what ID's permissions process has
	 */
	enum uidTypeEnum
	{
		SYSTEM_UID,
		SYSTEM_EUID
	};
	
	/**
	 * @struct	__userInfo contains user info
	 */
	struct	__userInfo
	{
		std::string name;///< user's name 
		std::string pass;///< user's password
		int uid;///< user's id
		int gid;///< user's group
		std::string realName;///< user's real name
		std::string home;///< user's home directory
		std::string shell;///< user's default shell
	};
	
	/**
	 * @struct __groupInfo contains group info
	 */
	struct __groupInfo
	{
		std::string name;///< name of the group
		int gid;///< group id
		stringArr members;///< list of group members
	};

	#ifdef DL_EXT
	
		/**
		 * @struct sigMod must be returned from initSigModule in the module
		 */
		struct sigMod
		{
			char name[20];///< name of module
			char discription[40];///< discription of module
			char hook[20];///< name of function in module that will be a hook
			systemSignalsEnum signal;///< on what signal to set handler
		};
		
		/**
		 * @typedef describes function in module that must return info for the hook
		 */
		typedef sigMod (*initSigModule)();

		/**
		 * @typedef describes function in module that will be called during module unloading
		 */
		typedef void (*deinitSigModule)();
	
	#endif


	/**
	 * @class systemTools provides misc system operations, gets diff info about system
	 */
	class systemTools
	{
					
		public:			
		
			/**
			 * registers functions that will be called on normal program exit
			 * @note you can register more than one
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							atExit(void (*func)());
		
			/**
			 * suspend for given microseconds
			 * @param period is time in microseconds
			 */
			static void microSleep(unsigned long period);
		
			/**
			 * suspend for given seconds
			 * @param period is time in seconds
			 */
			static void sleep(unsigned long period);
					
			/**
			 * prints message to stderr end exits from program
			 * @param message is message to print
			 * @param status indicate with what status to exit
			 */
			static void die(const std::string &message, int status = 1);
			
			/**
			 * changes root(/) to new
			 * @param path indicates where to change root(/) directory
			 * @note you will appear in the root(/)
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							changeRoot(const std::string &path);		
			/**
			 * @return current working directory
			 */
			static std::string getWorkingDir();
			
			/**
			 * set current working directory (cd path) 
			 * @param path is path where to go
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setWorkingDir(const std::string &path);
				
			/**
			 * get system usage with current process
			 * @param info will be filled with system usage info
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getUsageInfo(__usage &info);
			
			/**
			 * get limits from systemToolsLimitEnum
			 * @param type is type of info to get
			 * @param lim will be filled with requested values
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getLimit(systemToolsLimitEnum type,  __limits &lim);
							
			/**
			 * set limits from systemToolsLimitEnum
			 * @param type is type of info to set
			 * @param lim will fill with requested values
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setLimit(systemToolsLimitEnum type, const __limits &lim);
			
			/**
			 * @return priority of current process for uidTypeEnum
			 * @param type is type of UID to use
			 * @note if error occured and if NO_EX set -> -1 will return
			 */		
			static int getPriority(uidTypeEnum type);

			/**
			 * sets priority of current process (nice)
			 * @param type is type of UID to use
			 * @param prio is value of priority
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setPriority(uidTypeEnum type, int prio);
							
			/**
			 * @return user id of the current process
			 * @param type is type of UID to use
			 * @note if error occured and if NO_EX set -> -1 will return
			 */		
			static int getUID(uidTypeEnum type);
			
			/**
			 * set user id of the current process
			 * @param type is type of UID to use
			 * @param uid is user's id
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setUID(uidTypeEnum type, int uid);	
							
			/**
			 * get group id of the current process
			 * @param type is type of UID to use
			 * @note if error occured and if NO_EX set -> -1 will return
			 */		
			static int getGID(uidTypeEnum type);
			
			/**
			 * sets group id of the current process
			 * @param type is type of UID to use
			 * @param gid is group id
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setGID(uidTypeEnum type, int gid);		
			
			/**
			 * gets user info
			 * @param info will be filled with user's info
			 * @param uid is user's id about what to get info
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getUserInfo(__userInfo &info, int uid);	
										
			/**
			 * gets user info
			 * @param info will be filled with user's info
			 * @param name is user's login name about what to get info
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getUserInfo(__userInfo &info, const std::string &uid);			
							
			/**
			 * gets users of the system
			 * @param info will be filled with array of users' info
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getUsers(std::vector<__userInfo> &info);
			
			/**
			 * gets group info
			 * @param info will be filled with group's info
			 * @param name is group's id name about what to get info
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getGroupInfo(__groupInfo &info, int gid);	
										
			/**
			 * gets group info
			 * @param info will be filled with user's info
			 * @param name is group's name about what to get info
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getGroupInfo(__groupInfo &info, const std::string &gid);			
							
			/**
			 * gets group of the system
			 * @param info will be filled with array of group' info
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							getGroups(std::vector<__groupInfo> &info);								
			/**
			 * @return PID of current process
			 */
			static int getPID();
			
			/**
			 * @return parent PID of current process
			 */
			static int getParentPID();

			/**
			 * @return group PID of current process
			 */
			static int getGroupPID();
			
			/**
			 * @return group PID of given PID
			 * @param pid is process id from what to get group PID
			 */
			static int getGroupPID(int pid);
			
			/**
			 * sets group PID of current process
			 * @param pgid specifies the group PID where to move current process
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setGroupPID(int gpid);
			
			/**
			 * sets group PID of given process
			 * @param pid specifies what pid to move
			 * @param pgid specifies the group PID where to move current process
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							setGroupPID(int pid, int gpid);
			
			/**
			 * set signal handler
			 * @param signal is signal on what set handler
			 * @param handler is function that will be called
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							setSignalHandler(systemSignalsEnum signal, signalHandler handler);
			
			/**
			 * determines whether handler was set on signal
			 * @param is signal is on what set handler
			 */
			static bool isSignalHandled(systemSignalsEnum signal);
			
			/**
			 * removes signal handler
			 * @param is signal is from what unset handler
			 */			
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif			
							unsetSignalHandler(systemSignalsEnum signal);
							
			#ifdef DL_EXT
			
				/**
				 * @return info about module
				 * @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
				 */
				static sigMod getModuleInfo(const std::string &module);
				
				/**
				 * set handler on signal from specific module
				 * @param signal indicates for what signal to set handler
				 * @param @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
				 */
				#ifndef NO_EX
					static void 
				#else
					static bool 
				#endif				 
								setSignalHandler(systemSignalsEnum signal, const std::string &module);

				/**
				 * set handler on signal from specific module
				 * @param signal indicates for what signal to set handler
				 * @param @param module is path[if not in ldconfig db] to module or module name [if in ldconfig db] where function that will be called as a hook
				 */
				#ifndef NO_EX
					static void 
				#else
					static bool 
				#endif				 
								setSignalHandler(const std::string &module);

			#endif											
				
		protected:
			
			static int toRealSignal(systemSignalsEnum signal);
			
			/**
			 * fills __userInfo with values from passwd structure
			 */
			static __userInfo &fillUserInfo(__userInfo &info, passwd *pw);
			
			/**
			 * fills __groupInfo with values from group structure
			 */
			static __groupInfo &fillGroupInfo(__groupInfo &info, group *pw);
																						
	};

};

#endif