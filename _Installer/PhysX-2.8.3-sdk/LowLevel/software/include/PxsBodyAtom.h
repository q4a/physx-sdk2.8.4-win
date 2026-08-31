#ifndef PXS_BODYATOM_H
#define PXS_BODYATOM_H

#include "PxnError.h"
#include "PxnBodyAtom.h"
#include "PxcVector.h"
#include "PxcTransform.h"
#include "PxcListNode.h"
#include "PxcSmallArray.h"

class PxsConstraintBase;
class PxsContactManager;
class PxsDynamicsContext;
class PxsSolverBodyAtom;
class PxsSolverBodyAtom;


class PxsBodyAtom: public PxnBodyAtom
{
	public:
											PxsBodyAtom(PxsDynamicsContext* context, PxU32 index);
	virtual									~PxsBodyAtom();

			void							init(const PxdAtomDesc& desc);

	// Implements PxnBodyAtom
	virtual	const PxdTransform&				getPoseV()										const;
	virtual	void	 						setPoseV(const PxdTransform& v);

	virtual const PxdSpatialVector&			getVelocityV()									const;
	virtual void	 						setVelocityV(const PxdSpatialVector& v);

	virtual const PxdSpatialVector&			getAccelerationV()								const;
	virtual void	 						setAccelerationV(const PxdSpatialVector& v);

	virtual void							getDampingV(PxFloat& lin, PxFloat& ang)			const;
	virtual void							setDampingV(PxFloat lin, PxFloat ang);

	virtual void							getVelocityCapV(PxFloat& lin, PxFloat& ang)		const;
	virtual void							setVelocityCapV(PxFloat lin, PxFloat ang);

	virtual void							getInverseMassV(PxdVector& i, PxFloat& m)		const;
	virtual void							setInverseMassV(const PxdVector& i, PxFloat m);

	virtual bool							getRestingV()									const;
	virtual void							setRestingV(bool resting);

	virtual void							activateV();
	virtual void							deactivateV();
	virtual bool							isActiveV()										const;

	virtual void							setIterationCountV(PxInt i);
	virtual PxInt							getIterationCountV()							const;

	virtual void							setHalfPiClampV(bool clamp);
	virtual bool							getHalfPiClampV()								const;

	virtual PxFloat							getReportThresholdV()							const;
	virtual void							setReportThresholdV(PxFloat threshold);

	virtual	void							destroyV();
	virtual	PxdHandle						getHandleV()									const;


	//~Implements PxnBodyAtom

	PXD_FORCE_INLINE	PxU32				getIndex()										const	{ return mIndex;			}
	PXD_FORCE_INLINE	const PxcTransform&	getPose()										const	{ return mTransform;		}
	PXD_FORCE_INLINE	PxsSolverBodyAtom*	getSolverAtom()									const	{ return mSolverAtom;		}
	PXD_FORCE_INLINE	PxU32				getMovedTimestamp()								const	{ return mMovedTimestamp;	}
	PXD_FORCE_INLINE	PxFloat				getReportThreshold()							const	{ return mReportThreshold;	}

	PXD_INLINE			void				addToContactManagerList(PxcListNode<PxsContactManager>& cm)
											{
												cm.insertAfter(&mContactManagerList);
											}


	PXD_INLINE			void				addToConstraintList(PxcListNode<PxsConstraintBase>& c)
											{
												c.insertAfter(&mConstraintList);
											}

						void				addDependantVolume(PxU16 volumeIndex);
						void				addDependantVolumes(const PxcSmallIndexArray &array);
						void				removeDependantVolume(PxU16 volumeIndex);
						void				removeDependantVolumes(const PxcSmallIndexArray &array);
						void				removeAllDependantVolumes();
						void				markDependantVolumes();

	private:
						PxsDynamicsContext*	mContext;
						PxU32				mIndex;

	// Data used during group-building for solver
						PxsBodyAtom*		mNextInGroup;
						PxsBodyAtom*		mGroupTail;
						PxU32				mGroupSize;
						PxsSolverBodyAtom*	mSolverAtom;

						PxcSpatialVector 	mAcceleration;
						PxcSpatialVector	mVelocity;
						PxcSpatialVector	mMotionVelocity;

						PxcTransform		mTransform;

						PxFloat				mLinDamping, mAngDamping;
						PxFloat				mLinMaxVelSq, mAngMaxVelSq;

						PxFloat				mInvMass;
						PxcVector			mInvInertia;
	// VangelisK : Moved those back here as they were in 2.5
						bool				mIsActive;
						bool				mResting;
						bool				mClampHalfPi;
						PxU32				mMovedTimestamp; // timestamp to indicate that the atom moved this tick

						PxInt				mIterationCount;

						PxFloat				mReportThreshold;
						PxU32				mGroupThresholdCount;
	
	// Pointer to first holder of custom constraints
		PxcListNode<PxsConstraintBase>		mConstraintList;
	// Pointer to first contact manager
		PxcListNode<PxsContactManager>		mContactManagerList;

	//List of broadphase volumes(indices) which depend on this atom
						PxcSmallIndexArray	mDependantVolumes;

	// PT: TODO: remove ugly friends
	friend class PxsDynamicsContext;
	friend class PxsSolverBodyAtom;
};

PXD_FORCE_INLINE PxsBodyAtom* SelectLinkBodyAtom(PxsBodyAtom* atom0, PxsBodyAtom* atom1)
{
	PxsBodyAtom* linkAtom = NULL;
	if(!atom0)
	{
		linkAtom = atom1;
	}
	else if(!atom1)
	{
		linkAtom = atom0;
	}
	else
	{
		// both set, use the one with lowest atom-id
		if(atom0->getIndex() < atom1->getIndex())
		{
			linkAtom = atom0;
		}
		else
		{
			linkAtom = atom1;
		}
	}
	PXN_ASSERT(linkAtom);
	return linkAtom;
}

class PxsBodyPair
{
public:
	PxsBodyAtom *atom0, *atom1;
};

#endif
