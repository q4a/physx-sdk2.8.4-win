#ifndef PXC_CONTACTCACHE_H
#define PXC_CONTACTCACHE_H

#include "px_config.h"

#include "PxcNoCopy.h"
#include "PxcSeparatingAxes.h"

//Need some opcode stuff
#include "PxcIceHelper.h"
#include "PxcGenericCache.h"

#ifdef PXD_TEST_CONVEX_CONVEX_OPTIM_02
struct PxcConvexConvexCache
{
	PxcConvexConvexCache():
		mC0(0xffff),
		mC1(0xffFF),
		mFlag(false)
	{
		mSepAxis.set(0,0,0);
		mTransform.rotation.set(0,0,0,0);
		mTransform.translation.set(0,0,0);
	}

	PxcTransform	mTransform; //prev relative transform.
	PxcVector		mSepAxis; //seperating axis, or zero when no known axis
	PxU16			mC0, mC1; //Face indices when in contact or 0xffFF when seperated/unknown
	bool			mFlag; //Used to say which way round to do contact gen.
};
#endif

/*!
Cache-object used by contact generation routines.
This cache holds "global"/bigger cache objects that can be shared among different shape-pairs.

This cache is not thread-safe, so you must have one object per execution thread
*/
class PxcThreadContactCache : public PxcNoCopy
{
public:
	PxcThreadContactCache();
	~PxcThreadContactCache();

	//! Prepare for tag-stamping
	void setupStamps(PxU32 numStamps);

	//! Get current stamp
	PXD_INLINE PxU32 getCurrentStamp()
	{
		// First valid timestamp is maxNbStamps. We clear the array on wrapping only.

		mCurrentTagStamp++;
		if(!mCurrentTagStamp)
		{
			if(mTagStamps) PxcMemset(mTagStamps, 0, mMaxNumStamps*sizeof(PxU32));
			mCurrentTagStamp = mMaxNumStamps;
		}

		return mCurrentTagStamp;
	}

	//! Get the array of stamps
	PXD_FORCE_INLINE PxU32* getTagStamps()
	{
		return mTagStamps;
	}

	//! General shared container
	Container						mSharedContainer;

	//! Query-object for spheres
	Opcode::HybridSphereCollider	mSphereCollider;
	//! Cache-object for spheres
	Opcode::SphereCache				mSphereCache;

	//! Query-object for capsules
	Opcode::HybridLSSCollider		mCapsuleCollider;
	//! Cache-object for capsules
	Opcode::LSSCache				mCapsuleCache;

	//! Query-object for planes
	Opcode::HybridPlanesCollider	mPlanesCollider;
	//! Cache-object for planes
	Opcode::PlanesCache				mPlanesCache;

	//! Query-object for box
	Opcode::HybridOBBCollider		mBoxCollider;
	//! Cache-obejct for box
	Opcode::OBBCache				mBoxCache;


	// Tag-buffer for hill-climbing etc
	PxU32							mMaxNumStamps;
	PxU32*							mTagStamps;
	PxU32							mCurrentTagStamp;

	// Separating axes buffers for convex-convex
	PxcSeparatingAxes				mSA0, mSA1;

#if defined(PXD_TEST_CONVEX_MESH_OPTIM_01) || defined(PXD_TEST_CONVEX_CONVEX_OPTIM_02)
	PxcGenericCache					mSharedContactCache;
#endif
};


/*!
Cache-object used by contact generation routines.
This cache holds shape-pair specific information, and should be reused on subsequent calls
to contact generation between the same shapes.
*/
class PxcPairContactCache
{
public:
	PxcPairContactCache();
	~PxcPairContactCache();

	PXD_FORCE_INLINE void invalidate()
	{
		mPairData = 0;

#if defined(PXD_TEST_CONVEX_MESH_OPTIM_01) || defined(PXD_TEST_CONVEX_CONVEX_OPTIM_02)
		mCookie.invalidate();
#endif
	}

#if defined(PXD_TEST_CONVEX_MESH_OPTIM_01) || defined(PXD_TEST_CONVEX_CONVEX_OPTIM_02)
	PxcCacheIndex					mCookie;
#endif

	//! General 32-bit data to be used for pair data
	//Convex-Convex - mPairData is true if there was previously a contact
	//Box-Box - prev best axis...
	//sphere-convex - cached polygon index.
	PxU32							mPairData;
};

/*!
Holder-objects for the different caches used by contact routines
*/
class PxcContactCacheObject
{
public:
	PxcContactCacheObject(PxcThreadContactCache& t, PxcPairContactCache& p)
		: mThreadCache(t), mPairCache(p)
	{}

	PxcThreadContactCache&			mThreadCache;
	PxcPairContactCache&			mPairCache;
};

#endif
