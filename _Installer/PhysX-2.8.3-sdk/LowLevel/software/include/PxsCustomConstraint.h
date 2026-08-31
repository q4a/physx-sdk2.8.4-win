
#ifndef PXS_CUSTOMCONSTRAINT_H
#define PXS_CUSTOMCONSTRAINT_H

#include "PxnCustomConstraint.h"
#include "PxcVector.h"
#include "PxcQuaternion.h"
#include "PxcListNode.h"
#include "PxsDynamics.h"

class PxsBodyAtom;
class PxsDynamicsContext;
class PxsSolverConstraintPool;

class PxsCustomConstraint : public PxsConstraintBase, public PxnCustomConstraint
{
public:
									PxsCustomConstraint(PxsDynamicsContext* dc, PxU32 index);
	virtual							~PxsCustomConstraint();

			void					init(const PxdConstraintDesc& desc);

	// Implements PxnCustomConstraint
	virtual	PxdHandle				getHandleV()									const;
	virtual	void					destroyV();

	virtual	void					getAtomsV(PxnBodyAtom*& a0, PxnBodyAtom*& a1)	const;

	virtual	PxdConstraintType		getTypeV()										const;

	virtual	const PxdVector&		getPoint0V()									const;
	virtual	void					setPoint0V(const PxdVector& v);

	virtual	const PxdVector&		getPoint1V()									const;
	virtual	void					setPoint1V(const PxdVector& v);

	virtual	const PxdVector&		getAxis0V()										const;
	virtual	void					setAxis0V(const PxdVector& v);

	virtual	const PxdVector&		getAxis1V()										const;
	virtual	void					setAxis1V(const PxdVector& v);

	virtual	PxFloat					getErrorV()										const;
	virtual	void					setErrorV(PxFloat f);

	virtual	PxFloat					getTargetVelocityV()							const;
	virtual	void					setTargetVelocityV(PxFloat f);

	virtual	PxFloat					getMinImpulseV()								const;
	virtual	void					setMinImpulseV(PxFloat f);

	virtual	PxFloat					getMaxImpulseV()								const;
	virtual	void					setMaxImpulseV(PxFloat f);

	virtual	PxFloat					getImpulseMagnitudeV()							const;

	virtual	PxFloat					getImplicitSpringV()							const;
	virtual	void					setImplicitSpringV(PxFloat f);

	virtual	PxFloat					getImplicitDamperV()							const;
	virtual	void					setImplicitDamperV(PxFloat f);

	virtual	bool					getImpulseReportV()								const;
	virtual	void					setImpulseReportV(bool b);

	virtual	PxFloat					getDominance0V()								const;
	virtual	void					setDominance0V(PxFloat);

	virtual	PxFloat					getDominance1V()								const;
	virtual	void					setDominance1V(PxFloat);
	//~Implements PxnCustomConstraint

	// Helpers
	PXD_FORCE_INLINE PxU32			getIndex()										const
	{
		return mIndex;
	}

	// Implements PxsConstraintBase
    virtual	PxU32					setupSolverConstraint(PxsSolverConstraintPool& scpool, PxFloat dt, PxU32& thresholdPairCount);
	//~Implements PxsConstraintBase

private:
	PxsDynamicsContext*				mDynamicsContext;
	PxU32							mIndex;
	PxdConstraintType				mType;

	// General data
	PxcVector						mPoint0;
	PxcVector						mPoint1;
	PxcVector						mAxis0;
	PxcVector						mAxis1;
	PxFloat							mTargetVelocity;
	
	PxFloat							mMinMagnitude;
	PxFloat							mMaxMagnitude;
	PxFloat							mError;
	PxFloat							mImplicitSpring;
	PxFloat							mImplicitDamper;

	bool							mImpulseReport;

	PxcListNode<PxsConstraintBase>	mConstraintListNode;

	PxsSolverImpulseAccumulator		mAccumulator;

	friend class PxsDynamicsContext;
};

#endif
