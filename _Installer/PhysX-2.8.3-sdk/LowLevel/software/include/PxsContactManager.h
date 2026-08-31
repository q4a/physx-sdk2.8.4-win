#ifndef PXS_CONTACTMANAGER_H
#define PXS_CONTACTMANAGER_H

#include "px_config.h"

//#include "PxsContext.h"

#include "PxnContactManager.h"

#include "PxsBodyShape.h"
//#include "PxsContactCallback.h"
#include "PxsContactCallbackPatch.h"
#include "PxsContactCallbackQuat.h"

#include "PxcBitField.h"
#include "PxcContactCache.h"
#include "PxcContactMethod.h"
#include "PxcListNode.h"

#include "PxsSolverConstraint.h"
#include "PxnCustomConstraint.h"

//class PxsContext;
//class PxnCustomConstraint;
//class PxsContactCacheObject;
//class PxsContactCallback;


class PxsContactManager : public PxnContactManager
{
public:
											PxsContactManager(PxsContext* context, PxU32 index);
	virtual									~PxsContactManager();

						void				init(const PxdManagerDesc& desc);

	// Implements PxnContactManager
	virtual				void				setDynamicFrictionV(PxFloat v);
	virtual				PxFloat				getDynamicFrictionV()								const;

	virtual				void				setStaticFrictionV(PxFloat v);
	virtual				PxFloat				getStaticFrictionV()								const;

	virtual				void				setRestitutionV(PxFloat v);
	virtual				PxFloat				getRestitutionV()									const;

	virtual				bool				getInfoReportingV()									const;
	virtual				bool				getImpulseReportingV()								const;
	virtual				bool				getChangeableContactsV()							const;

	virtual				void				setMaxConstraintCountV(PxU32 v);
	virtual				PxU32				getMaxConstraintCountV()							const;

	virtual				bool				getTouchStatusV()									const;

	virtual				PxU32				getConstraintCountV()								const;
	virtual				void				getConstraintsV(PxdConstraint* clist)				const;

	virtual				void				getShapesV(PxnBodyShape*& s0, PxnBodyShape*& s1)	const;

	virtual				PxFloat				getSkinWidthV()										const;
	virtual				void				setSkinWidthV(PxFloat v);

	virtual				PxFloat				getCorrelationDistanceV()							const;
	virtual				void				setCorrelationDistanceV(PxFloat v);

	virtual				PxInt				mapPageV(PxdShape s)								{	return 0; }
	virtual				void				unmapPageV(PxdShape s)								{}

	virtual				void				activateV();
	virtual				void				deactivateV();
	virtual				bool				isActiveV()											const;

	virtual				void*				getUserDataV()										const;		
	virtual				void				setUserDataV(void* data);

	virtual				PxdHandle			getHandleV()										const;
	virtual				void				destroyV();

	virtual				bool				getDisableResponseV()								const;

	virtual				PxFloat				getDominance0V()									const;
	virtual				void				setDominance0V(PxFloat);

	virtual				PxFloat				getDominance1V()									const;
	virtual				void				setDominance1V(PxFloat);
	//~Implements PxnContactManager


	PXD_FORCE_INLINE	PxU32				getIndex()											const
											{
												return mIndex;
											}

	// Software-specific routines
	PXD_FORCE_INLINE	PxsBodyAtom*		getBodyAtom0()										const
											{
												return mBodyShape0 ? mBodyShape0->getBodyAtom() : NULL;
											}

	PXD_FORCE_INLINE	PxsBodyAtom*		getBodyAtom1()										const
											{
												return mBodyShape1 ? mBodyShape1->getBodyAtom() : NULL;
											}

	PXD_FORCE_INLINE	PxsBodyShape*		getBodyShape0()										const
											{
												return mBodyShape0;
											}

	PXD_FORCE_INLINE	PxsBodyShape*		getBodyShape1()										const
											{
												return mBodyShape1;
											}

	PXD_FORCE_INLINE	PxIBool				getTouchStatus()									const
											{
												return mFlags & PXS_CM_TOUCH_STATUS;
											}

	PXD_FORCE_INLINE	PxIBool				getDisableResponse()								const
											{
												return mFlags & PXS_CM_DISABLE_RESPONSE;
											}

	// Update contact manager status
	PXD_FORCE_INLINE	void				update(PxFloat dt, PxsContactCacheObject& cacheObject, bool& oldTouch, bool& newTouch, PxU32 cmTimestamp, PxU32 dynamicTimestamp);
	// Setup solver-constraints
						PxU32				setupSolverConstraints(PxsSolverConstraintPool& scpool, PxFloat dt, PxsContactCacheObject& cacheObject, PxsBodyAtom* prefetchAtom0, PxsBodyAtom* prefetchAtom1);

	PXD_FORCE_INLINE	PxU8				getDominanceByte() const
											{
											return 
												(mDominance0 == 0 ? 0 : PxsSolverConstraint::PXS_SC_FLAG_0_IS_PUSHABLE)
											+	(mDominance1 == 0 ? 0 : PxsSolverConstraint::PXS_SC_FLAG_1_IS_PUSHABLE);
											}

					PxnContactConstraint*	findContactConstraint(PxU32 index);
private:
						PxsContext*			mContext;
						PxU32				mIndex;

						PxsBodyShape*		mBodyShape0;
						PxsBodyShape*		mBodyShape1;

	//Updated-counters for shapes and atoms
						PxU32				mCMTimestamp;
						PxU32				mDynamicsTimestamp;

						PxFloat				mDynamicsFriction;
						PxFloat				mStaticFriction;
						PxFloat				mRestitution;
	
	// Correlation parameters
						PxFloat				mSkinWidth;
						PxFloat				mCorrelationDistance;

	PxcListNode<PxsContactManager>			mContactManagerListNode;

						PxcContactMethod	mContactMethod;		// PT: is it really worth storing here?
						PxsContactCallback* mContactCallback;	// PT: allocated each time? why not unite with contact manager?

	// 04/17/2007 -- Sauce.  These were under "Correlation parameters"
	// but SPU narrowphase needs mContactCallback to be 16 byte aligned and these
	// break that.
						PxFloat				mDominance0;
						PxFloat				mDominance1;
	// Contact manager settings
	enum
	{
		PXS_CM_REPORT_INFO				= 0x1,
		PXS_CM_REPORT_IMPULSE			= 0x2,
		PXS_CM_CHANGEABLE				= 0x4,
		PXS_CM_CALLBACK_TYPE_PATCH		= 0x8,
		PXS_CM_TOUCH_STATUS				= 0x10,
		PXS_CM_OLD_TOUCH_STATUS			= 0x20,
		PXS_CM_ACTIVE					= 0x40,
		PXS_CM_SWAP_BODIES				= 0x80,
		PXS_CM_DISABLE_RESPONSE			= 0x100,
		PXS_CM_HAVE_IMPULSE_INFO		= 0x200,
		PXS_CM_REPORT_AND_HAVE_IMPULSE	= PXS_CM_REPORT_IMPULSE | PXS_CM_HAVE_IMPULSE_INFO
	};

	//Dont use a bit field as that introduces many variable shifts(slow microcoded instruction on 360)
						PxU32			mFlags;

	// Per pair contact generation routine cache
				PxcPairContactCache		mPairCache;	// PT: same as mContactCallback...

						void*			mUserData;

	// PT: TODO: Argh.... remove ugly friends
	friend class PxsContactCallback;
	friend class PxsContactCallbackQuat;
	friend class PxsContactCallbackPatch;
	friend class PxsContactCallbackQuat::ExternalContactProxy;
	friend class PxsContactCallbackPatch::ExternalContactProxy;
};

//Need to inline to avoid LHS.
PXD_FORCE_INLINE void PxsContactManager::update(PxFloat dt, PxsContactCacheObject& threadCache, bool& oldTouch, bool& newTouch, PxU32 cmTimestamp, PxU32 dynamicTimestamp)
{
	PXN_ASSERT(mFlags & PXS_CM_ACTIVE);

	if(mFlags & PXS_CM_TOUCH_STATUS)
	{
		mFlags |= PXS_CM_OLD_TOUCH_STATUS;
		oldTouch = true;
	}
	else
	{
		mFlags &= ~PXS_CM_OLD_TOUCH_STATUS;
		oldTouch = false;
	}	
	
	bool touch;
	bool performCD;

	performCD = (mBodyShape0->getBodyAtom() && mDynamicsTimestamp < mBodyShape0->getBodyAtom()->getMovedTimestamp()) ||
				(mBodyShape1->getBodyAtom() && mDynamicsTimestamp < mBodyShape1->getBodyAtom()->getMovedTimestamp()) ||
				(mCMTimestamp < mBodyShape0->getChangeTimestamp()) ||
				(mCMTimestamp < mBodyShape1->getChangeTimestamp()) ||
				(mCMTimestamp < mBodyShape0->getShape()->getChangeTimestamp()) ||
				(mCMTimestamp < mBodyShape1->getShape()->getChangeTimestamp());

	if (performCD)
	{
		touch = mContactCallback->executeContactMethod(mContactMethod, threadCache, mPairCache);

		//Record "last-update" info
		mCMTimestamp = cmTimestamp;
		mDynamicsTimestamp = dynamicTimestamp;
	}
	else
	{
		mContactCallback->releaseConstraintHandles();
		touch = oldTouch;		
	}


	if(touch)
	{
		mFlags |= PXS_CM_TOUCH_STATUS;
		newTouch = true;
	}
	else
	{
		mFlags &= ~PXS_CM_TOUCH_STATUS;
		newTouch = false;
	}
}
#endif
