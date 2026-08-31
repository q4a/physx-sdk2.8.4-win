#ifndef PXS_CONTACTCACHE_H
#define PXS_CONTACTCACHE_H

#include "PxsBodyAtom.h"
#include "PxsBodyShape.h"
#include "PxsSolverAtom.h"
#include "PxsSolverConstraint.h"
#include "PxcStreamedThresholdTable.h"

#include "PxcContactCache.h"
#include "PxcCache.h"
#include "PxcTransform.h"
#include "PxcMatrix34.h"
#include "PxcCachedTransforms.h"
#include "PxcInterlocked.h"
#include "PxcThreadCoherantCache.h"
#include "PxcBitMap.h"

#include "PxnError.h"



struct PxsBodyShapeHasher
{
	static PXD_FORCE_INLINE PxUInt hash(PxsBodyShape* ptr)
	{
		return ptr->getIndex()+1;
	}
	static PXD_FORCE_INLINE bool equal(PxsBodyShape* a, PxsBodyShape* b)
	{
		return a==b;
	}

	static PXD_FORCE_INLINE PxsBodyShape* invalidKey()
	{
		return NULL;
	}
};

/*!
Cache information specific to the software implimentation(non common).

See PxcContactCacheObject.

Not thread-safe, so remember to have one object per thread!

TODO! refactor this and rename(it is a general per thread cache). Move transform cache into its own class.
*/
class PxsContactCacheObject : 
	public PxcThreadCoherantCache<PxsContactCacheObject>::EntryBase, public PxcThreadContactCache
{
public:

	//TODO: tune cache size based on number of active objects.
	PxsContactCacheObject() : transformCache(1024),
#ifdef _DEBUG
							mTransformLock(false),
#endif
							mLocalNewTouchCount(0), mLocalLostTouchCount(0){}

	/*
	This is a little ugly, but copying the transforms is a performance hit.
	So we keep the transforms in place and return pointers. If there happens to be a collision
	we keep the transform in an auxilary buffer.

	Getting a pair of transform also helps the compiler to schedule code.

	WARNING: do not call this again until you are finished with the transforms(we enforce this in
	debug builds with mTransformLock).
	*/
	PXD_FORCE_INLINE void getCachedTransforms(PxsBodyShape* bodyShape0, PxsBodyShape* bodyShape1, 
		const PxcCachedTransforms*& T0, const PxcCachedTransforms*& T1)
	{
		PXN_ASSERT(!mTransformLock);
#ifdef _DEBUG
		mTransformLock = true;
#endif
		PxUInt index0 = transformCache.getIndex(bodyShape0);
		PxUInt index1 = transformCache.getIndex(bodyShape1);

		PxcCachedTransforms* trans0 = transformCache.getPtr(index0);
		T0 = trans0;

		if(!transformCache.isValid(index0, bodyShape0))
		{
			setupTransforms(trans0, bodyShape0);
			transformCache.validate(index0, bodyShape0);
		}

		if(index0 == index1) 
		{// We have a collision :-(
			T1 = &auxTransforms;
			setupTransforms(&auxTransforms, bodyShape1);
		}
		else
		{
			PxcCachedTransforms* trans1 = transformCache.getPtr(index1);
			T1 = trans1;

			if(!transformCache.isValid(index1, bodyShape1))
			{
				setupTransforms(trans1, bodyShape1);
				transformCache.validate(index1, bodyShape1);
			}
		}

	}

	PXD_FORCE_INLINE void putCachedTransforms(const PxcCachedTransforms* T0, const PxcCachedTransforms* T1)
	{
		PXN_ASSERT(mTransformLock);
#ifdef _DEBUG
		mTransformLock = false;
#endif
	}

	PXD_INLINE void invalidateTransforms()
	{
		PXN_ASSERT(!mTransformLock);
		transformCache.invalidate();

		mLocalChangeTouch.reset();
		mLocalNewTouchCount = 0;
		mLocalLostTouchCount = 0;

		mLocalChangedVolumes.reset();
	}

	PXD_FORCE_INLINE PxsSolverBodyAtomPool& getSolverBodyAtomPool()
	{
		return mSolverBodyAtomPoolCache;
	}
	
	PXD_FORCE_INLINE PxsSolverConstraintPool& getSolverConstraintPool()
	{
		return mSolverConstraintPoolCache;
	}

	PXD_FORCE_INLINE void addLocalNewTouchCount(PxU32 newTouchCMCount)
	{
		mLocalNewTouchCount += newTouchCMCount;
	}

	PXD_FORCE_INLINE void addLocalLostTouchCount(PxU32 lostTouchCMCount)
	{
		mLocalLostTouchCount += lostTouchCMCount;
	}

	PXD_FORCE_INLINE PxU32 getLocalNewTouchCount()		const
	{
		return mLocalNewTouchCount;
	}

	PXD_FORCE_INLINE PxU32 getLocalLostTouchCount()		const
	{
		return mLocalLostTouchCount;
	}

	PXD_FORCE_INLINE PxcBitMap& getLocalChangeTouch()
	{
		return mLocalChangeTouch;
	}

	PXD_FORCE_INLINE PxcBitMap& getLocalChangedVolumes()
	{
		return mLocalChangedVolumes;
	}


	PXD_FORCE_INLINE PxcStreamedThresholdTable<PxsBodyAtom *>& getStreamedThresholdTable()
	{
		return mStreamedThresholdTable;
	}

private:
	
	// PT: why is that a function of this class?
	static
	PXD_FORCE_INLINE void setupTransforms(PxcCachedTransforms* transforms, PxsBodyShape* bodyShape)
	{
		PxsBodyAtom* atom = bodyShape->getBodyAtom();
			
		if(atom != NULL)
		{
			transforms->set(bodyShape->getTransform(), atom->getPose());
		}
		else
		{
			transforms->set(bodyShape->getTransform());
		}
	}

	// This is used to store the transforms if there is a collision(hopefully rarely).
	// Otherwise the transforms are returned in place.
	PxcCachedTransforms auxTransforms;

	PxcCache<PxsBodyShape*, PxcCachedTransforms, PxsBodyShapeHasher > transformCache;

	// Solver caches
	PxsSolverBodyAtomPool mSolverBodyAtomPoolCache;
	PxsSolverConstraintPool mSolverConstraintPoolCache;

	// threshold-based reporting
	PxcStreamedThresholdTable<PxsBodyAtom *> mStreamedThresholdTable;

	// change touch handling.
	PxcBitMap mLocalChangeTouch;
	PxU32 mLocalNewTouchCount;
	PxU32 mLocalLostTouchCount;

	//Volume changed handling(keeps track of which volumes changed within a particular thread).
	PxcBitMap mLocalChangedVolumes;

#ifdef _DEBUG
	bool mTransformLock;
#endif
};

#endif
