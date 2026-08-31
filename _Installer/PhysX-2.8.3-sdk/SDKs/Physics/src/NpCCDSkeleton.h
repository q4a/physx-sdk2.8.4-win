#ifndef NP_CCDSKELETON
#define NP_CCDSKELETON

#include "Allocateable.h"
#include "RefCountable.h"
#include "NvPhysicsSDK.h"
#include "NxCCDSkeleton.h"


class NpCCDSkeleton : public NxCCDSkeleton, public RefCountable, public NxFoundation::NxAllocateable
	{
	public:
	NpCCDSkeleton(NvCCDSkeleton * m) : mSkeleton(m)		{ m->setNxCCDSkeleton(this);	}
	~NpCCDSkeleton()									{ mSkeleton->release(); }
	NX_INLINE NvCCDSkeleton * getNvSkeleton()			{ return mSkeleton;		}

	virtual NxU32 save(void * destBuffer, NxU32 bufferSize)	{ return mSkeleton->save(destBuffer, bufferSize); }
	virtual NxU32 getDataSize()	{ return mSkeleton->getSize(); }
	virtual	NxU32 getReferenceCount() { return getNumRefs(); }
	virtual NxU32 saveToDesc(NxSimpleTriangleMesh &mesh) { return mSkeleton->saveToDesc(mesh); }
	private:
	
	
	NvCCDSkeleton * mSkeleton;	
	};

#endif
