#ifndef PXS_SOLVERATOM_H
#define PXS_SOLVERATOM_H

#include "PxcVector.h"
#include "PxcMatrix33.h"
#include "PxcThreadCoherantCache.h"

class PxsBodyAtom;

/*!
Solver local body atom definition
*/
class PxsSolverBodyAtom
{
public:
	//! Default constructor
	PxsSolverBodyAtom();

	//! Copy from a BodyAtom
	void copyFrom(PxsBodyAtom* bodyAtom);

	//! Copy back results to the BodyAtom
	void writeBack() const;

	//! Save velocity into motion velocity
	void saveVelocity();

	//! Setup as static
	void setupStatic();

	// Data, think about alignment a bit at least before changing this..

	//! Post-solver linear velocity
	PxcVector		mLinearVelocity;				//12
	//! Pad, don't remove, want to be able to write freely
	PxFloat			mPad1;							//16

	//! Post-solver angular velocity
	PxcVector		mAngularVelocity;				//28
	//! Pad, don't remove, want to be able to write freely
	PxFloat			mPad2;							//32

	//! Velocity used for integration
	PxcVector		mMotionVelocity;				//44
	//! Inverse mass
	PxFloat			mInvMass;						//48

	//! Angular velocity used for integration
	PxcVector		mMotionAngularVelocity;			//60

	// Inv inertia tensor
	PxcMatrix33		mInvInertia;					//96

	//! PxsBodyAtom created from
	PxsBodyAtom*	mOriginalBodyAtom;				//100

	//! Max iterations for this body
	PxU32			mSolverIterationCount;			//104

	PxFloat			mThreshold;						//108
	//! Pad to sizeof(this)%16 == 0
	PxFloat			mPad3[1];						//112
};

/*!
Pool of solver bodies
*/
class PxsSolverBodyAtomPool : public PxcThreadCoherantCache<PxsSolverBodyAtomPool>::EntryBase
{
public:
	PxsSolverBodyAtomPool();
	~PxsSolverBodyAtomPool();

	//! Initialize to storage of at least maxBodyAtoms size
	void init(PxU32 maxBodyAtoms);

	PXD_FORCE_INLINE	PxsSolverBodyAtom& addSolverAtom()
	{
		return mPool[mSize++];
	}

	PXD_FORCE_INLINE	PxU32 getSize() const
	{
		return mSize;
	}

	PXD_FORCE_INLINE	PxsSolverBodyAtom* getPtr() const
	{
		return mPool;
	}

private:
	PxU32				mSize;
	PxU32				mCapacity;

	PxsSolverBodyAtom*	mPool;
};

#endif
