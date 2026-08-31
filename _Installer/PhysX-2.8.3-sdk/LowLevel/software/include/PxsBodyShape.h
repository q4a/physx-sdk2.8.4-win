#ifndef PXS_BODYSHAPE_H
#define PXS_BODYSHAPE_H

#include "PxnBodyShape.h"

#include "PxcTransform.h"
#include "PxcSmallArray.h"

// For the inlining
#include "PxcBox.h"
#include "PxsShape.h"
#include "PxsBodyAtom.h"

class PxsContext;

class PxsBodyShape : public PxnBodyShape
{
public:
											PxsBodyShape(PxsContext* context, PxU32 index);
	virtual									~PxsBodyShape();

			void							init(const PxdShapeDesc& desc);

	// Implements PxnBodyShape
	virtual void							setTransformV(const PxdTransform& v);
	virtual const PxdTransform&				getTransformV()						const;

	virtual void							setBodyAtomV(PxnBodyAtom* b);
	virtual PxnBodyAtom*					getBodyAtomV()						const;

	virtual PxnShape*						getShapeV()							const;

	virtual void							setUserDataV(void* data);
	virtual void*							getUserDataV()						const;

	virtual PxdHandle						getHandleV()						const;
	virtual void							destroyV();
	//~Implements PxnBodyShape

	PXD_FORCE_INLINE	PxU32				getIndex()							const	{ return mIndex;			}
	PXD_FORCE_INLINE	PxsBodyAtom*		getBodyAtom()								{ return mBodyAtom;			}
	PXD_FORCE_INLINE	const PxsBodyAtom*	getBodyAtom()						const	{ return mBodyAtom;			}
	PXD_FORCE_INLINE	PxsShape*			getShape()									{ return mShape;			}
	PXD_FORCE_INLINE	const PxsShape*		getShape()							const	{ return mShape;			}
	PXD_FORCE_INLINE	const PxcTransform&	getTransform()						const	{ return mTransform;		}
	PXD_FORCE_INLINE	PxU32				getChangeTimestamp()				const	{ return mChangeTimestamp;	}

	PXD_INLINE			void				computeBounds(PxcVector& origin, PxcVector& extent)
											{
												PxcTransform transform;
												if(mBodyAtom)
													transform = mBodyAtom->getPose().transform(mTransform);
												else
													transform = mTransform;

												mShape->computeBounds(transform, origin, extent);
											}
	
						void				addDependantVolume(PxU16 volumeIndex);
						void				removeDependantVolume(PxU16 volumeIndex);

private:

						void				addDependantVolumes();
						void				removeDependantVolumes();
						void				markDependantVolumes();

						PxsContext*			mContext;
						PxU32				mIndex;

						PxsBodyAtom*		mBodyAtom;
						PxsShape*			mShape;

						void*				mUserData;
						PxU32				mChangeTimestamp;

						PxcTransform		mTransform;

						PxcSmallIndexArray	mDependantVolumes;
};

#endif
