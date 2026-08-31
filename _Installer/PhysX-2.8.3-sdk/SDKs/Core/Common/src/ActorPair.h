#ifndef NX_COLLISION_ACTORPAIR
#define NX_COLLISION_ACTORPAIR
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "RbActor.h"
#include "ContactStream.h"
#include "NPhaseCore.h"

class ActorPair;

class ActorPairContactReportData
{
public:
	ActorPairContactReportData() : 
		mForceStamp(0xffffffff), 
		mStrmResetStamp(0xffffffff), 
		mVisualizationStamp(0xffffffff),
		mSumFrictionForce(0, 0, 0), 
		mSumNormalForce(0, 0, 0), 
		mContactCallbackNext(NULL),
		mThisFrameForceThresholdExceeded(false),
		mLastFrameForceThresholdExceeded(false)
		{}
	ContactStream mContactStream;
	NxU32 mStrmResetStamp;
	NxU32 mVisualizationStamp;
	NxVec3 mSumNormalForce;
	NxVec3 mSumFrictionForce;
	NxU32 mForceStamp;
	ActorPair* mContactCallbackNext;
	bool mThisFrameForceThresholdExceeded;
	bool mLastFrameForceThresholdExceeded;
};

// Class shared by all SIPs or SIP markers for a pair of actors
class ActorPair
{
public:
	NX_INLINE ActorPair(RbActor& shape0, RbActor& shape1);
	NX_INLINE ~ActorPair();
	NX_INLINE ContactStream& getContactStream() { return getContactReportData()->mContactStream;	}
	NX_INLINE void incRefCount() { ++mRefCount; NX_ASSERT(mRefCount>0); }
	NX_INLINE NxU32 decRefCount() { NX_ASSERT(mRefCount>0); return --mRefCount; }
	NX_INLINE NxU32 getRefCount() const { return mRefCount; }
	NX_INLINE RbActor& getActorA() const { return mActorA; }
	NX_INLINE RbActor& getActorB() const { return mActorB; }
	NX_INLINE Scene* getScene() const { return getActorA().getScene(); }
	NX_INLINE bool streamResetStamp(NxU32 cmpStamp);
	// Contact notification is only set on actor pairs so far. Not shape pairs.
	NxU32 getContactNotifyFlags() const;
	NX_INLINE ActorPair* getContactCallbackNext() const { return getContactReportData()->mContactCallbackNext; }
	NX_INLINE void setContactCallbackNext(ActorPair* aPair) { getContactReportData()->mContactCallbackNext = aPair; }
	NX_INLINE void setForceThresholdExceeded(bool exceeded);
	NX_INLINE void swapForceThresholdExceeded();
	NX_INLINE bool lastFrameForceThresholdExceeded() const;
	NX_INLINE bool thisFrameForceThresholdExceeded() const;
	const NxVec3 getSumNormalForce() const; 
	const NxVec3 getSumFrictionForce() const; 
	void addNormalForce(const NxVec3& force);
	void addFrictionForce(const NxVec3& force);
	bool lastFrameHadContacts() const; 
	NX_INLINE bool thisFrameHaveContacts() const;
	NX_INLINE void incTouchCount(NxU32 timeStamp);
	NX_INLINE void decTouchCount(NxU32 timeStamp);
	NX_INLINE NxU32 getTouchCount() const;
	void visualize(NxFoundation::DebugRenderable &d);
private:
	NX_INLINE ActorPairContactReportData* getContactReportData() const;
	void createContactReportData() const;
	void releaseContactReportData() const;
	RbActor& mActorA;
	RbActor& mActorB;
	NxU32 mLostTouchStamp;
	NxU32 mNewTouchStamp;
	NxU16 mTouchCount;
	NxU16 mRefCount;
	mutable ActorPairContactReportData* mReportData;
};

//////////////////////////////////////////////////////////////////////////
ActorPair::ActorPair(RbActor& actor0, RbActor& actor1) : 
mActorA(actor0), 
mActorB(actor1), 
mRefCount(0), 
mTouchCount(0), 
mNewTouchStamp(0xffffffff), 
mLostTouchStamp(0xffffffff),
mReportData(NULL)
{ 
}

ActorPair::~ActorPair()
{
	if (mReportData != NULL) 
		releaseContactReportData();
}

NX_INLINE bool ActorPair::streamResetStamp(NxU32 cmpStamp) 
{ 
	bool ret = (cmpStamp != getContactReportData()->mStrmResetStamp); 
	getContactReportData()->mStrmResetStamp = cmpStamp; 
	return ret; 
}

NX_INLINE void ActorPair::incTouchCount(NxU32 timeStamp)
{
	if (mTouchCount == 0)
		mNewTouchStamp = timeStamp;
	++mTouchCount;
	NX_ASSERT(mTouchCount>0);
}

NX_INLINE void ActorPair::decTouchCount(NxU32 timeStamp)
{
	NX_ASSERT(mTouchCount>0);
	--mTouchCount;
	if (mTouchCount == 0)
		mLostTouchStamp = timeStamp;
}

NX_INLINE void ActorPair::setForceThresholdExceeded(bool exceeded)
{
	getContactReportData()->mThisFrameForceThresholdExceeded = exceeded;
}

NX_INLINE void ActorPair::swapForceThresholdExceeded()
{
	getContactReportData()->mLastFrameForceThresholdExceeded = getContactReportData()->mThisFrameForceThresholdExceeded;
}

NX_INLINE bool ActorPair::lastFrameForceThresholdExceeded() const
{
	return getContactReportData()->mLastFrameForceThresholdExceeded;
}

NX_INLINE bool ActorPair::thisFrameForceThresholdExceeded() const
{
	return getContactReportData()->mThisFrameForceThresholdExceeded;
}

NX_INLINE bool ActorPair::thisFrameHaveContacts() const
{
	return mTouchCount>0;
}

NX_INLINE NxU32 ActorPair::getTouchCount() const
{
	return mTouchCount;
}

NX_INLINE ActorPairContactReportData* ActorPair::getContactReportData() const 
{ 
	// Lazy create report data
	if (!mReportData) 
		createContactReportData();
	return mReportData; 
}

#endif
