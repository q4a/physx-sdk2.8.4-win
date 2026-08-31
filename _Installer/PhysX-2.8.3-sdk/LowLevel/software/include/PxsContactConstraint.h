#ifndef PXS_CONTACTCONSTRAINT_H
#define PXS_CONTACTCONSTRAINT_H

#include "PxnCustomConstraint.h"


#include "PxnCustomConstraint.h"
#include "PxcVector.h"
#include "PxcTransform.h"
#include "PxcListNode.h"
#include "PxsDynamics.h"

class PxsContactConstraint : public PxsConstraintBase, public PxnContactConstraint 
{
public:
										PxsContactConstraint(PxsDynamicsContext* dc, PxU32 index);
	virtual								~PxsContactConstraint();

						void			init(const PxdContactConstraintDesc& desc);

	// Implements PxnContactConstraint
	virtual				PxdHandle		getHandleV()									const;
	virtual				void			destroyV();

	virtual				void			getAtomsV(PxnBodyAtom*& a0, PxnBodyAtom*& a1)	const;

	virtual				PxdTransform	getFrame0V()									const;
	virtual				void			setFrame0V(const PxdTransform& t);

	virtual				PxdTransform	getFrame1V()									const;
	virtual				void			setFrame1V(const PxdTransform& t);

	virtual				PxdVector		getErrorV()										const;
	virtual				void			setErrorV(const PxdVector& v);

	virtual				PxdVector		getTargetVelocityV()							const;
	virtual				void			setTargetVelocityV(const PxdVector& v);

	virtual				PxdVector		getImpulseV()									const;

	virtual				PxFloat			getMinImpulseV()								const;
	virtual				void			setMinImpulseV(PxFloat f);

	virtual				PxFloat			getMaxImpulseV()								const;
	virtual				void			setMaxImpulseV(PxFloat f);

	virtual				PxFloat			getStaticFriction0V()							const;
	virtual				void			setStaticFriction0V(PxFloat f);

	virtual				PxFloat			getStaticFriction1V()							const;
	virtual				void			setStaticFriction1V(PxFloat f);

	virtual				PxFloat			getDynamicFriction0V()							const;
	virtual				void			setDynamicFriction0V(PxFloat f);

	virtual				PxFloat			getDynamicFriction1V()							const;
	virtual				void			setDynamicFriction1V(PxFloat f);

	virtual				PxFloat			getRestitutionV()								const;
	virtual				void			setRestitutionV(PxFloat f);

	virtual				bool			getAnisotropicV()								const;
	virtual				void			setAnisotropicV(bool b);

	virtual				bool			getImpulseReportV()								const;
	virtual				void			setImpulseReportV(bool b);

	virtual				PxInt			getFeature0V()									const	{ return 0;			}
	virtual				PxInt			getFeature1V()									const	{ return 0;			}

	virtual				PxFloat			getDominance0V()								const;
	virtual				void			setDominance0V(PxFloat);

	virtual				PxFloat			getDominance1V()								const;
	virtual				void			setDominance1V(PxFloat);

	//~Implements PxnContactConstraint

	PXD_FORCE_INLINE	PxU32			getIndex()										const	{ return mIndex;	}

	// Implements PxsConstraintBase
	virtual				PxU32			setupSolverConstraint(PxsSolverConstraintPool& scpool, PxFloat dt, PxU32& thresholdPairCount);
	//~Implements PxsConstraintBase

private:
				PxsDynamicsContext*		mDynamicsContext;
						PxU32			mIndex;
	
						PxcTransform	mFrame0;
						PxcTransform	mFrame1;

						PxcVector		mError;
						PxcVector		mTargetVelocity;
						PxFloat			mMinMagnitude;
						PxFloat			mMaxMagnitude;

						PxFloat			mStaticFriction0;
						PxFloat			mStaticFriction1;
						PxFloat			mDynamicFriction0;
						PxFloat			mDynamicFriction1;
						PxFloat			mRestitution;

						bool			mAnisotropic;
						bool			mImpulseReport;

	PxcListNode<PxsConstraintBase>		mConstraintListNode;

	friend class PxsDynamicsContext;

	//Helper to accumulate impulses for report
	struct ImpulseAccumulator : public PxsSolverConstraintCallback
	{
		ImpulseAccumulator()
			//: mAccumulatedImpulse(0.0f)
		{
			mAccumulatedImpulse.setZero();
		}

		// Implements PxsSolverConstraintCallback
		virtual void lastIterationCallback(const PxsSolverConstraint& sc)
		{
			mAccumulatedImpulse.x += sc.mAppliedForceC;
			mAccumulatedImpulse.y += (sc.mAppliedFrictionForceC.dot(sc.mContactBaseC.getBasisVector1()));
			mAccumulatedImpulse.z += (sc.mAppliedFrictionForceC.dot(sc.mContactBaseC.getBasisVector2()));
		}

		virtual void lastIterationCallback(PxFloat vForce, PxFloat force)
		{
			PXN_ASSERT(0); //Should not be called for a quat constraint.
		}
		virtual void frictionBrokenCallback()
		{
			PXN_ASSERT(0); //Should not be called for a quat constraint.
		}

		//~Implements PxsSolverConstraintCallback

		PxcVector mAccumulatedImpulse;
	} mAccumulator;
};

#endif
