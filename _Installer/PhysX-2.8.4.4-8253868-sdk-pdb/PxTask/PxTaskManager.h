#ifndef __PX_TASK_MANAGER_H__
#define __PX_TASK_MANAGER_H__

/*
 * Copyright 2009-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
 
namespace physx
{
namespace pxtask
{

class Task;
typedef unsigned long TaskID;

struct TaskType
{
   enum Enum
   {
       TT_CPU,
       TT_GPU,
	   TT_NOT_PRESENT,
       TT_COMPLETED
   };
};

class NotificationInterface
{
public:
    // Called when all submitted tasks have executed.
    virtual void simulationComplete() = 0;
	// Called when the task manager is destroyed
	virtual void release() = 0;
};

class CpuDispatcher
{
public:
    // Upon receiving a task, the dispatcher should schedule the task
    // the run when resource is available.  After the task has been run,
    // it should call the release() method and discard it's pointer.
    virtual void submitTask( Task& task ) = 0;

	virtual ~CpuDispatcher() 
	{
	}
};

class GpuDispatcher;

class TaskManager
{
public:
    virtual void     setCpuDispatcher( CpuDispatcher& ref ) = 0;
    virtual void     setGpuDispatcher( GpuDispatcher& ref ) = 0;
	virtual void     setNotification( NotificationInterface& notify ) = 0;

    virtual CpuDispatcher*			getCpuDispatcher() const = 0;
	virtual GpuDispatcher*			getGpuDispatcher() const = 0;
	virtual NotificationInterface*	getNotification() const = 0;

    virtual void	resetDependencies() = 0;
    virtual void	startSimulation() = 0;
    virtual void	taskCompleted( Task& task ) = 0;

    virtual TaskID  getNamedTask( const char *name ) = 0;
	virtual TaskID  submitNamedTask( Task *task, const char *name, TaskType::Enum type = TaskType::TT_CPU ) = 0;
	virtual TaskID  submitUnnamedTask( Task& task, TaskType::Enum type = TaskType::TT_CPU ) = 0;
	virtual Task*   getTaskFromID( TaskID ) = 0;

	virtual void        release() = 0;
	static TaskManager* createTaskManager( CpuDispatcher* = 0, GpuDispatcher* = 0 );

protected:
	virtual ~TaskManager() {}

    virtual void finishBefore( Task& task, TaskID taskID ) = 0;
    virtual void startAfter( Task& task, TaskID taskID ) = 0;

    virtual void addReference( TaskID taskID ) = 0;
    virtual void decrReference( TaskID taskID ) = 0;

    friend class Task;
	friend class GpuWorkerThread;
};

CpuDispatcher *createDefaultThreadPool( unsigned int numThreads );

} // end pxtask namespace
} // end physx namespace

#endif
