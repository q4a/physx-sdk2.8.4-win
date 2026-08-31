#ifndef PXS_FLUIDBODY_CONTACTMANAGER_H
#define PXS_FLUIDBODY_CONTACTMANAGER_H

#include "PxsFluidConfig.h"
#ifdef PXS_ENABLE_FLUIDS

#include "PxnFluidBodyContactManager.h"

#include "PxsFluidShape.h"
#include "PxsBodyShape.h"

#include "PxcBitField.h"
#include "PxcContactCache.h"
#include "PxcContactMethod.h"
#include "PxcListNode.h"

#include "PxsSolverConstraint.h"
#include "PxnCustomConstraint.h"


class PxsFluidBodyContactManager : public PxnFluidBodyContactManager
{
public:
											PxsFluidBodyContactManager(PxsContext* context, PxU32 index);
	virtual									~PxsFluidBodyContactManager();

						void				init(const PxdManagerDescFluidBody& desc);

	// Implements PxnFluidBodyContactManager
	virtual				void				activateV();
	virtual				void				deactivateV();
	virtual				bool				isActiveV()											const;

	virtual				void*				getUserDataV()										const;
	virtual				void				setUserDataV(void* data);

	virtual				PxdHandle			getHandleV()										const;
	virtual				void				destroyV();

	virtual				void				getShapesV(PxnFluidShape*& s0, PxnBodyShape*& s1)	const;

	virtual				bool				getIsDrainV()										const;
	virtual				void				setIsDrainV(PxInt isDrain);
	//~Implements PxnFluidBodyContactManager


	PXD_FORCE_INLINE	PxU32				getIndex()											const
											{
												return mIndex;
											}

	// Software-specific routines
	PXD_FORCE_INLINE	PxsFluid*			getFluid()											const
											{
												return mFluidShape ? mFluidShape->getFluid() : NULL;
											}

	PXD_FORCE_INLINE	PxsBodyAtom*		getBodyAtom()										const
											{
												return mBodyShape ? mBodyShape->getBodyAtom() : NULL;
											}

	PXD_FORCE_INLINE	PxsFluidShape*		getFluidShape()										const
											{
												return mFluidShape;
											}

	PXD_FORCE_INLINE	PxsBodyShape*		getBodyShape()										const
											{
												return mBodyShape;
											}

	PXD_FORCE_INLINE	bool				hasStaticShape()									const
											{
												return ((mFlags & PXS_FB_CM_STATIC_SHAPE) != 0);
											}

	PXD_FORCE_INLINE	bool				isActive()											const
											{
												return ((mFlags & PXS_FB_CM_ACTIVE) != 0);
											}

private:
						PxsContext*			mContext;
						PxU32				mIndex;

						PxsFluidShape*		mFluidShape;
						PxsBodyShape*		mBodyShape;

	PxcListNode<PxsFluidBodyContactManager>	mContactManagerListNode;

	// Contact manager settings
	enum
	{
		PXS_FB_CM_ACTIVE				= (1<<0),
		PXS_FB_CM_IS_DRAIN				= (1<<1),	// The interacting body shape is a fluid particle drain
		PXS_FB_CM_STATIC_SHAPE			= (1<<2),	// The interacting body shape is a static shape
	};

	//Dont use a bit field as that introduces many variable shifts(slow microcoded instruction on 360)
						PxU32			mFlags;

						void*			mUserData;
};


#endif	// PXS_ENABLE_FLUIDS

#endif
