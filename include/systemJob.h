/***************************************************************************
 *            systemJob.h
 *
 *  Mon Mar 05 13:01:55 2007
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
 
#ifndef _SYSTEMJOB_H_
#define _SYSTEMJOB_H_

#include <directives.h>
#include <types.h>
	
namespace dodo
{
	/**
	 * @typedef describes function to be passed as job
	 */
	typedef void *(*jobFunc)(void *);
		
	/**
	 * @class systemJob is to manage jobs
	 */
	class systemJob
	{
		public:
			
			/**
			 * adds function to became a job[not executing]
			 * @return position of jobFunc in queue
			 * @param func indicates function to be executed
			 * @param data describes data to be passed to func
			 */
			virtual unsigned long add(jobFunc func, void *data) = 0;	
				
			/**
			 * adds function to became a job[executing]
			 * @return position of job in queue
			 * @param func indicates function to be executed
			 * @param data describes data to be passed to func
			 */
			virtual unsigned long addNRun(jobFunc func, void *data) = 0;
				
			/**
			 * removes registered job
			 * @param position indicates on job to remove
			 * @param force if is set to true stops execution if this job is running
			 * @note - exception if it's currently running
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 
							del(unsigned long position, bool force=false) = 0;
			
			/**
			 * executes job
			 * @param position indicates what job to run
			 * @param force if is set to true permits execution even if this job is running
			 * @note - exception if it's currently running
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 
							run(unsigned long position, bool force=false) = 0;

			/**
			 * stops job
			 * @param position indicates what job to stop
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 		
							stop(unsigned long position) = 0;

			/**
			 * stops all registered jobs
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 		
							stop() = 0;
										
			/**
			 * waits for job's termination
			 * @param position indicates for what job to wait
			 * @param data points on data that job returned
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 	
							wait(unsigned long position, void **data=NULL) = 0;
										
			/**
			 * @return true if job is running
			 * @param position indicates for what job to indicate
			 */
			virtual bool isRunning(unsigned long position) const = 0;
			
			/**
			 * @return amount of running jobs
			 */
			virtual unsigned long running() const = 0;
							
			/**
			 * waits for all registered jobs' termination
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif						 	
							wait() = 0;			
			
			/**
			 * sweep jobs if their time are already passed
			 */
			virtual void sweepTrash() = 0;
			
			/**
			 * set maximum execution time
			 * @param position indicates for what job to set limit
			 * @param limit indicates the job's limit on executions
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif			
							setExecutionLimit(unsigned long position, unsigned long limit=1) = 0; 

			
			#ifdef DL_EXT
			
				/**
				 * adds function to became a job[not executing] from module
				 * @return position of job in queue
				 * @param module indicates mudule where is function to be executed
				 * @param data describes data to be passed to func
				 * @param toInit indicates data that will path to initialize function
				 */
				virtual unsigned long add(const dodoString &module, void *data, void *toInit = NULL) = 0;
			
			#endif
	};
};

#endif
