#ifndef __PX_TASK_H__
#define __PX_TASK_H__

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
 
#include "Px.h"
#include "PxTaskManager.h"
#include "PxAssert.h"

namespace physx
{
using namespace pubfnd2;

namespace pxtask
{

class Task
{
public:
	Task();   
	virtual ~Task() {}

    /* run() functions must be thread safe and stack friendly (no alloca, etc). */
    virtual void        run() = 0;
    virtual const char *getName() const = 0;

	/* User's implementation of release() must call resolveDependencies() */
    virtual void release()
	{
		PX_ASSERT(mTm);

		// If this is the last task to be released, it will trigger notifications
		// during the taskCompleted() call which can result in this task being
		// immediately re-submitted in another thread.  So we clear the mTm pointer
		// before calling taskCompleted() to avoid making any modifications to this
		// afterwards.
		TaskManager *save = mTm;
		mTm = NULL;
		save->taskCompleted( *this );
	}

	void emitStartEvent( PxU16 stat );
	void emitStopEvent( PxU16 stat );

	/* Wraps run() function with profiling events.  Optional entry point for
	 * use by CpuDispatchers.
	 */
	PX_INLINE void runProfiled()
	{
		PX_ASSERT(mTm);
		emitStartEvent( 0 );
		run();
		emitStopEvent( mProfileStat );
	}

	/* If called while the task is executing, the given value will appear
	 * in the task's event bar in the profile viewer
	 */
	PX_INLINE void setProfileStat( PxU16 stat )
	{
		mProfileStat = stat;
	}

    PX_INLINE void finishBefore( TaskID taskID )
	{
		PX_ASSERT(mTm);
		mTm->finishBefore( *this, taskID);
	}
    PX_INLINE void startAfter( TaskID taskID )
	{
		PX_ASSERT(mTm);
		mTm->startAfter( *this, taskID );
	}

    /* Manually increment this task's reference count.  The task will
     * not be allowed to run until you call removeReference().
     */
    PX_INLINE void addReference()
	{
		PX_ASSERT(mTm);
		mTm->addReference( mTaskID );
	}

    /* Manually decrement this task's reference count.  If the reference
     * count reaches zero, the task will be dispatched.
     */
    PX_INLINE void removeReference()
	{
		PX_ASSERT(mTm);
		mTm->decrReference( mTaskID );
	}

	PX_INLINE TaskID	    getTaskID() const		{ return mTaskID; }
	PX_INLINE TaskManager * getTaskManager() const	{ return mTm; }

	/**
	 * \brief Called by TaskManager when this task has been submitted
	 *
	 * Perform simulation step initialization here.
	 */
	virtual void submitted()
	{
		mStreamIndex = 0;
		mPreSyncRequired = false;
		mProfileStat = 0;
	}


protected:
    TaskManager *		mTm;
    TaskID				mTaskID;
	PxU16				mEventID;
    PxU32               mStreamIndex;
    bool				mPreSyncRequired;
	PxU16               mProfileStat;

	friend class TaskMgr;
    friend class GpuWorkerThread;
};

} // end pxtask namespace
} // end physx namespace

#endif
