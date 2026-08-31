#ifndef PXS_SOLVERCONSTRAINT_H
#define PXS_SOLVERCONSTRAINT_H

#include "px_config.h"
#include "PxcVector.h"
#include "PxcQuaternion.h"
#include "PxcHelperFunction.h"
#include "PxcThreadCoherantCache.h"
#include "PxcFastMemory.h"
#include "PxsSolverAtom.h"

class PxsSolverBodyAtom;
class PxsSolverConstraint;

//Callback that can be used by other parts of the code to get calls per constraint from solver
class PxsSolverConstraintCallback
{
public:
	virtual void lastIterationCallback(const PxsSolverConstraint& sc) = 0;
	virtual void lastIterationCallback(PxFloat vForce, PxFloat force) = 0;
	virtual void frictionBrokenCallback() = 0;
};



PXD_INLINE void PxsImplicitSpringHelper(PxFloat realSCoeff, PxFloat realDCoeff, PxFloat dt,
										PxFloat& implicitSCoeff, PxFloat& implicitDCoeff)
{
	realDCoeff = PxcMax(realDCoeff, 0.00001f);

	implicitSCoeff = realSCoeff*dt + realDCoeff;
	implicitDCoeff = dt * realSCoeff / implicitSCoeff;
	implicitSCoeff = 1.0f/(dt*implicitSCoeff);
}

class PxsSolverConstraint
{
public:
	enum SolverConstraintType
	{
		PXS_SC_TYPE_NONE = 0,
		PXS_SC_TYPE_CONTACT,
		PXS_SC_TYPE_CONTACT_QUAT,
		PXS_SC_TYPE_CONTACT_QUAT_ANIS, //same as _QUAT but with anisotropic friction
		PXS_SC_TYPE_LINEAR,
		PXS_SC_TYPE_ANGULAR,
		PXS_SC_TYPE_LINEAR_FRICTION,
		PXS_SC_TYPE_COMPRESSED_CONTACT,
	};

	enum SolverConstraintFlags
	{
		PXS_SC_FLAG_NOT_FINALIZED		= (1<<0),
		PXS_SC_FLAG_DONT_ZERO_POSITIVE	= (1<<1),
		PXS_SC_FLAG_BROKEN				= (1<<2),
		PXS_SC_FLAG_UNBREAKABLE			= (1<<3),
#define PXS_SC_0_PUSHABLE_SHIFT			4				//AM: this is a bit creative but I need to know which bit it is.
		PXS_SC_FLAG_0_IS_PUSHABLE		= (1<<PXS_SC_0_PUSHABLE_SHIFT),
#define PXS_SC_1_PUSHABLE_SHIFT			5
		PXS_SC_FLAG_1_IS_PUSHABLE		= (1<<PXS_SC_1_PUSHABLE_SHIFT)
	};

	PxsSolverConstraint()
	{
		mFlags.mType = PXS_SC_TYPE_NONE;		
		mFlags.mFlags = PXS_SC_FLAG_NOT_FINALIZED;
	}

	//! Initialize for PXS_SC_TYPE_CONTACT
	PXD_FORCE_INLINE PxsSolverConstraint* initContact(PxsSolverBodyAtom* atom0, PxsSolverBodyAtom* atom1,
		const PxcVector& r0, const PxcVector& r1, const PxcVector& normal, PxFloat bias, void* patch,
		PxFloat minImpulse, PxFloat maxImpulse, PxsSolverBodyAtom* staticAtom, PxU8 domFlags);

	//! Initialize for PXS_SC_TYPE_CONTACT_QUAT
	PXD_FORCE_INLINE PxsSolverConstraint* initContactQuat(PxsSolverBodyAtom* atom0, PxsSolverBodyAtom* atom1,
		const PxcVector& r0, const PxcVector& r1, const PxcVector& geometricError, const PxcQuaternion& quat,
		PxFloat staticFriction0, PxFloat dynamicFriction0, PxFloat minImpulse, PxFloat maxImpulse,
		PxsSolverBodyAtom* staticAtom, PxU8 domFlags);

	//! Initialize for PXS_SC_TYPE_CONTACT_QUAT_ANIS
	PXD_FORCE_INLINE PxsSolverConstraint* initContactQuatAnis(PxsSolverBodyAtom* atom0, PxsSolverBodyAtom* atom1,
		const PxcVector& r0, const PxcVector& r1, const PxcVector& geometricError, const PxcQuaternion& quat,
		PxFloat staticFriction0, PxFloat staticFriction1, PxFloat dynamicFriction0, PxFloat dynamicFriction1,
		PxFloat minImpulse, PxFloat maxImpulse,
		PxsSolverBodyAtom* staticAtom, PxU8 domFlags);

	//! Initialize for PXS_SC_TYPE_LINEAR
	PXD_FORCE_INLINE PxsSolverConstraint* initLinear(PxsSolverBodyAtom* atom0, PxsSolverBodyAtom* atom1,
		const PxcVector& r0, const PxcVector& r1, const PxcVector &axis0, const PxcVector &axis1,
		PxFloat targetVelocity, PxFloat bias, PxFloat minImpulse, PxFloat maxImpulse,
		PxFloat solverExtrapolation, PxsSolverBodyAtom* staticAtom, PxU8 domFlags);

	//! Initialize for PXS_SC_TYPE_ANGULAR
	PXD_FORCE_INLINE PxsSolverConstraint* initAngular(PxsSolverBodyAtom* atom0, PxsSolverBodyAtom* atom1,
		const PxcVector& axis0, const PxcVector& axis1, PxFloat targetVelocity, PxFloat bias,
		PxFloat minImpulse, PxFloat maxImpulse, PxFloat solverExtrapolation,
		PxsSolverBodyAtom* staticAtom, PxU8 domFlags);

	//! Initialize for PXS_SC_TYPE_LINEAR_FRICTION
	PXD_FORCE_INLINE PxsSolverConstraint* initLinearFriction(PxsSolverBodyAtom* atom0, PxsSolverBodyAtom* atom1,
		const PxcVector& r0, const PxcVector& r1, const PxcVector& axis0, const PxcVector& axis1, 
		PxFloat bias, PxFloat staticFriction, PxFloat dynamicFriction, void* patch,
		PxsSolverBodyAtom* staticAtom, PxU8 domFlags);

	//! Finalize constraint pre-computations. Must be called before solving with this particular constraint.
	// finalization of linear friction is done inside the solver(as it is performance critical).
	void finalizeQuat();
	PXD_FORCE_INLINE void finalizeQuatAnis()
	{
		finalizeQuat();
	}

	//! Setup for implicit spring-damper, call only once and after init
	void setImplicitSpringDamper(PxFloat implicitSpring, PxFloat implicitDamper, bool useImprovedSpringSolver,
								bool isAccelerationSpring);
	
	//! Zero bias

	//Used by SolverCoreGeneral (TODO: use same method as SolverCoreSIMD)
	PXD_INLINE void setZeroBias()
	{
		if(mFlags.mType == PXS_SC_TYPE_CONTACT_QUAT ||
		   mFlags.mType == PXS_SC_TYPE_CONTACT_QUAT_ANIS)
		{
			mBiasC.x = PxcMax(mBiasC.x, 0.0f);
			mBiasC.y = 0.0f;
			mBiasC.z = 0.0f;
		}
		else if(mFlags.mType == PXS_SC_TYPE_CONTACT ||
				 ((mFlags.mType == PXS_SC_TYPE_LINEAR || 
				  mFlags.mType == PXS_SC_TYPE_ANGULAR) &&
				  (mFlags.mFlags & PXS_SC_FLAG_DONT_ZERO_POSITIVE)) ) 
		{
			mBias = PxcMax(mBias, 0.0f);
		}
		else
		{
			mBias = 0.0f;
		}
	}

	//Used by SolverCoreSIMD
	PXD_FORCE_INLINE void setZeroBiasContact()
	{
		mBias = PxcMax(mBias, 0.0f);
	}

	PXD_FORCE_INLINE void setZeroBiasQuat()
	{
		mBiasC.x = PxcMax(mBiasC.x, 0.0f);
		mBiasC.y = 0.0f;
		mBiasC.z = 0.0f;
	}

	PXD_FORCE_INLINE void setZeroBiasQuatAnis()
	{
		mBiasC.x = PxcMax(mBiasC.x, 0.0f);
		mBiasC.y = 0.0f;
		mBiasC.z = 0.0f;
	}

	PXD_FORCE_INLINE void setZeroBiasLinear()
	{
		if(mFlags.mFlags & PXS_SC_FLAG_DONT_ZERO_POSITIVE)
		{
			mBias = PxcMax(mBias, 0.0f);
		}
		else
		{
			mBias = 0.0f;
		}
	}

	PXD_FORCE_INLINE void setZeroBiasAngular()
	{
		if(mFlags.mFlags & PXS_SC_FLAG_DONT_ZERO_POSITIVE)
		{
			mBias = PxcMax(mBias, 0.0f);
		}
		else
		{
			mBias = 0.0f;
		}
	}

	PXD_FORCE_INLINE void setZeroBiasLinearFriction()
	{
		mBias = 0.0f;
	}


	// Compute normal-axis relative velocity
	PxFloat computeRelativeNormalVelocity() const;
	PXD_FORCE_INLINE PxFloat computeRelativeNormalVelocityContact() const;

	//Note: mType must be first as the solver expects to find the type in the first 8bits of a constraint.

	// Data
	struct
	{
		PxU8	mType;			//enum SolverConstraintType
		// number of constraints which are of the same type and share the same bodies
		// which follow, not including the current.
		PxU8	mNumIdentical; 
		PxU8	mFlags;			//enum SolverConstraintFlags
		PxU8	mPad;			//was mIterations, but we now use the max iterations across the island.
	} mFlags;											//4

	// Common data
	PxsSolverBodyAtom* mBodyAtom0;						//8
	PxsSolverBodyAtom* mBodyAtom1;						//12
	
	PxsSolverConstraintCallback*	mCallback;			//16

	// Type-specific data
	// NOTE: Vector fields of this structure should be 16byte aligned,
	// this implies that the size of this structure should be a multiple of 16bytes.

	// On WiiU anonymous unions doesn't compile correctly
#if !defined(PXD_PLATFORM_WIIU)
	union 
	{
		struct // PXS_SC_TYPE_CONTACT_QUAT & PXS_SC_TYPE_CONTACT_QUAT_ANIS
		{
#endif
			//! Relative vector from object center to contact point, body 0
			PxcVector		mRelative0C;				
			//! Static friction coefficient, axis 0 (y) Always same or bigger than axis 1
			PxFloat			mStaticFriction0C;			//32

			//! Relative vector from object center to contact point, body 1
			PxcVector		mRelative1C;				
			//! Static friction coefficient, axis 1 (z)
			PxFloat			mStaticFriction1C;			//48

			//! Applied friction force in contact space
			PxcVector		mAppliedFrictionForceC;		
			//! Dynamic friction coefficient, axis 0 (y) Always same or bigger than axis 1
			PxFloat			mDynamicFriction0C;			//64

			//! Contact base quaternion x=normal, y/z = friction
			PxcQuaternion	mContactBaseC;				//80

			//! Unit impulse velocity response
			PxcVector		mVelMultiplierC;			
			//! Dynamic friction coefficient, axis 1 (z)
			PxFloat			mDynamicFriction1C;			//96

			//! Geometric error (bias)
			PxcVector		mBiasC;						
			//! Applied normal force
			PxFloat			mAppliedForceC;				//112

			//! Anisotropicity / excentricy of friction ellipse mStaticFriction0C/mStaticFriction1C
			PxFloat			mAnisotropicityC;			

			//! Upper bound on impulse magnitude
			PxFloat			mMaxImpulseC;				
			//! Lower bound on impulse magnitude
			PxFloat			mMinImpulseC;				
#if !defined(PXD_PLATFORM_WIIU)
		};												//124
		struct// PXS_SC_TYPE_CONTACT & PXS_SC_TYPE_LINEAR & PXS_SC_TYPE_ANGULAR & PXS_SC_TYPE_LINEAR_FRICTION
		{
#endif
			//! Axis in for body 0, global space
			PxcVector		mAxis0;						
			//! Velocity bias
			PxFloat			mBias;						//32

			//! Axis in for body 1, global space
			PxcVector		mAxis1;						
			//! Velocity unit response
			PxFloat			mVelMultiplier;				//48

			//! Relative0 x Axis0
			PxcVector		mR0XAxis0;					
			
			union
			{
				//! Position bias unit response
				PxFloat			mPosMultiplier;			
				//! Static friction limit
				PxFloat			mStaticFriction;		//64
			};

			//! Relative1 x Axis1
			PxcVector		mR1XAxis1;
			// MS: The following is not very nice and could be done with unions. But unions seem to be slow on PS3
			//     and since we do not want to make their integration work unnecessary painful we stick to this approach.
			//     And yes, cleaning this whole mess up is on a todo list...
			//! Impulse unit response (PXS_SC_TYPE_LINEAR & PXS_SC_TYPE_ANGULAR) or dynamic friction limit (PXS_SC_TYPE_CONTACT & PXS_SC_TYPE_LINEAR_FRICTION)
			PxFloat			mImpulseMultiplier_Or_DynamicFriction;
														//80

			//! body0.mIInv * mR0XAxis0
			PxcVector		mmIInvR0XAxis0;
			//! Solver extrapolation factor (PXS_SC_TYPE_LINEAR & PXS_SC_TYPE_ANGULAR) or patch identifier (PXS_SC_TYPE_CONTACT & PXS_SC_TYPE_LINEAR_FRICTION)
			PxFloat			mSolverExtrapolation_Or_PatchID;
														//96

			//! body1.mIInv * mR1XAxis1
			PxcVector		mmIInvR1XAxis1;
			//! Applied force to correct velocity
			PxFloat			mAppliedVForce;				//112

			//! Applied force to correct velocity+bias
			PxFloat			mAppliedForce;

			//! Upper bound on impulse magnitude
			PxFloat			mMaxImpulse;
			//! Lower bound on impulse magnitude
			PxFloat			mMinImpulse;
#if !defined(PXD_PLATFORM_WIIU)
		};												//124
	};													
#endif
	//! Normal-axis target velocity
	PxFloat			mTargetVelocity;					//128

	
};

//Compressed constraint components:
// NOTE: Vector fields of this structure should be 16byte aligned,
// this implies that the size of this structure should be a multiple of 16bytes.
struct PxsCompressedConstrHdr
{
	//Note: mType should be first as the solver expects a type in the first byte.
	PxU8 mType, mFlags;		//mFlags was mPad.  Now it may be  SolverConstraintFlags::PXS_SC_FLAG_0_IS_PUSHABLE and/or PXS_SC_FLAG_1_IS_PUSHABLE
	PxU8 mNumNormalConstr, mNumFrictionConstr;

	PxsSolverBodyAtom* mSolverAtom0;
	PxsSolverBodyAtom* mSolverAtom1;

	void *mPatch;

	PxFloat mStaticFriction;
	PxFloat mDynamicFriction;

	PxsSolverBodyAtom* mPrefetchAtom0;
	PxsSolverBodyAtom* mPrefetchAtom1;
}; //32bytes

// NOTE: Vector fields of this structure should be 16byte aligned,
// this implies that the size of this structure should be a multiple of 16bytes.
struct PxsCompressedConstrPt
{
	PxcVector mNormal; PxFloat mBiasPosMultiplier; //mBias * mPosMultiplier
	
	PxcVector mR0XAxis0; PxFloat mAppliedForce;
	PxcVector mR1XAxis1; PxFloat mAppliedVForce;

	PxcVector mmIInvR0XAxis0; PxFloat mTargetVelocity;
	PxcVector mmIInvR1XAxis1; PxFloat mVelMultiplier;

	PxsSolverConstraintCallback* mCallback;
	PxU32 mPad1, mPad2, mPad3; //TODO: This extra padding sucks:-( We could refactor the callback mechanism...
}; //96 bytes

// NOTE: Vector fields of this structure should be 16byte aligned,
// this implies that the size of this structure should be a multiple of 16bytes.
struct PxsCompressedConstrFric
{
	PxcVector mAxis; PxFloat mBias;
	
	PxcVector mR0XAxis0; PxFloat mAppliedForce;
	PxcVector mR1XAxis1; PxsSolverConstraintCallback* mFrictionCallback;;

	PxcVector mmIInvR0XAxis0; PxU16 mFinalized, mBroken;
	PxcVector mmIInvR1XAxis1; PxFloat mVelMultiplier;		
}; //80bytes

#ifdef PXS_USE_COMBINED_FRICTION_AXIS_CONSTRAINT
// NOTE: Vector fields of this structure should be 16byte aligned,
// this implies that the size of this structure should be a multiple of 16bytes.
struct PxsCompressedConstrFricCombi
{
	PxcVector mAxisY; PxFloat mBiasY;				//16
	PxcVector mAxisZ; PxFloat mBiasZ;				//32
	
	PxcVector mAppliedForce;
	PxsSolverConstraintCallback* mFrictionCallback;	//48

	PxcVector mR0;
	PxFloat mVelMultiplierY;						//64
	PxcVector mR1;
	PxFloat mVelMultiplierZ;						//80

	PxU32 mFinalized, mBroken, mPad1, mPad2;		//96
}; //96bytes
#endif


class PxsSolverConstraintPool : public PxcThreadCoherantCache<PxsSolverConstraintPool>::EntryBase
{
public:
	PxsSolverConstraintPool();
	~PxsSolverConstraintPool();

	//! Initialize to storage of at least maxBodyAtoms size
	void init(PxU32 initCapacity, PxsSolverBodyAtom* staticAtom);

	PXD_FORCE_INLINE PxsSolverConstraint& addSolverConstraint()
	{
		if((mSize+sizeof(PxsSolverConstraint)) >= mCapacity)
			grow(sizeof(PxsSolverConstraint));

		PxsSolverConstraint* c = (PxsSolverConstraint *)(mPool + mSize);
		mSize += sizeof(PxsSolverConstraint);
		
#ifdef _DEBUG
		memset(c, 0xcc, sizeof(PxsSolverConstraint)); //Shred the memory to aid debugging.
#endif
		return *c;
	}

	PXD_FORCE_INLINE PxU32 getSize() const
	{
		return mSize;
	}

	PXD_FORCE_INLINE PxU8* getPtr() const
	{
		return mPool;
	}

	PXD_FORCE_INLINE PxsSolverBodyAtom* getStaticAtom()
	{
		return mStaticAtom;
	}

	PXD_FORCE_INLINE void startCompressedContact(
		PxsSolverBodyAtom* solverAtom0, PxsSolverBodyAtom* solverAtom1, 
		PxFloat restitution, PxU8 numTemporalPoints, PxU8 numPatchPoints, void *patch,
		PxFloat staticFriction, PxFloat dynamicFriction,
		PxsSolverBodyAtom* prefetchAtom0, PxsSolverBodyAtom* prefetchAtom1, PxU8 domFlags);


	PXD_FORCE_INLINE void addCompressedNormalConstr(
		PxsSolverBodyAtom* solverAtom0, PxsSolverBodyAtom* solverAtom1,
		PxcVector &r0, PxcVector &r1, PxcVector &normal, 
		PxFloat bias, PxFloat restitution, PxFloat bounceThreshold, 
		PxsSolverConstraintCallback *callback, PxU8 domFlags);

	PXD_FORCE_INLINE void addCompressedFricConstr(PxcVector &ra, PxcVector &rb, 
		PxcVector &axis, PxFloat bias,PxsSolverConstraintCallback* frictionCallback);

#ifdef PXS_USE_COMBINED_FRICTION_AXIS_CONSTRAINT
	PXD_FORCE_INLINE void addCompressedFricConstrCombi(PxcVector &ra, PxcVector &rb, 
		PxcVector &axisY, PxcVector &axisZ, PxFloat biasY, PxFloat biasZ, 
		PxsSolverConstraintCallback* frictionCallback);
#endif

	PXD_FORCE_INLINE void endCompressedContact()
	{
		//TODO: Should we pad to 128 byte boundry, probably not.
	}

private:

	static const PxU32		CONSTRAINT_ALIGNMENT;

	void grow(PxU32 growSize);
	PxU32					mSize;
	PxU32					mCapacity;
	PxsSolverBodyAtom*		mStaticAtom;
	PxU8*					mPool;
};



//Helper class to accumulate vforce/force
class PxsSolverImpulseAccumulator : public PxsSolverConstraintCallback
{
public:
	PxsSolverImpulseAccumulator(bool vforce = true)
		: mAccumulatedImpulse(0.0f), mVForce(vforce)
	{}

	// Implements PxsSolverConstraintCallback
	virtual void lastIterationCallback(const PxsSolverConstraint& sc)
	{
		if(mVForce)
			mAccumulatedImpulse += sc.mAppliedVForce;
		else
			mAccumulatedImpulse += sc.mAppliedForce;
	}

	virtual void lastIterationCallback(PxFloat vForce, PxFloat force)
	{
		if(mVForce)
			mAccumulatedImpulse += vForce;
		else
			mAccumulatedImpulse += force;
	}

	virtual void frictionBrokenCallback() {}

	//~Implements PxsSolverConstraintCallback

	PxFloat mAccumulatedImpulse;
	bool	mVForce;
};

class PxsSolverImpulseAccumulatorQuat : public PxsSolverConstraintCallback
{
public:
	PxsSolverImpulseAccumulatorQuat()
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

	PxcVector	mAccumulatedImpulse;
};

/********************************************************************************************/
#define DOMINANCE_FLOAT_0 ((float)((mFlags.mFlags&PXS_SC_FLAG_0_IS_PUSHABLE)>>PXS_SC_0_PUSHABLE_SHIFT))
#define DOMINANCE_FLOAT_1 ((float)((mFlags.mFlags&PXS_SC_FLAG_1_IS_PUSHABLE)>>PXS_SC_1_PUSHABLE_SHIFT))

#define CREF_DOMINANCE_FLOAT_0(c) ((float)((c.mFlags.mFlags&PxsSolverConstraint::PXS_SC_FLAG_0_IS_PUSHABLE)>>PXS_SC_0_PUSHABLE_SHIFT))
#define CREF_DOMINANCE_FLOAT_1(c) ((float)((c.mFlags.mFlags&PxsSolverConstraint::PXS_SC_FLAG_1_IS_PUSHABLE)>>PXS_SC_1_PUSHABLE_SHIFT))

#define COMPRESSED_DOMINANCE_FLOAT_0(z) ((float)((z&PxsSolverConstraint::PXS_SC_FLAG_0_IS_PUSHABLE)>>PXS_SC_0_PUSHABLE_SHIFT))
#define COMPRESSED_DOMINANCE_FLOAT_1(z) ((float)((z&PxsSolverConstraint::PXS_SC_FLAG_1_IS_PUSHABLE)>>PXS_SC_1_PUSHABLE_SHIFT))


// PT: TODO: try to move the common init code to a common function

PXD_FORCE_INLINE PxsSolverConstraint* PxsSolverConstraint::initContact(
											   PxsSolverBodyAtom* atom0, PxsSolverBodyAtom* atom1,
											   const PxcVector& r0, const PxcVector& r1, 
											   const PxcVector& normal, PxFloat bias, 
											   void* patch, 
											   PxFloat minImpulse, PxFloat maxImpulse,
											   PxsSolverBodyAtom* staticAtom, PxU8 domFlags)
{
	//PXS_SC_TYPE_CONTACT
	mCallback = 0;
	((void*&)mSolverExtrapolation_Or_PatchID) = patch;
	mFlags.mType = PXS_SC_TYPE_CONTACT;
	mFlags.mFlags = PXS_SC_FLAG_NOT_FINALIZED | domFlags;
	mFlags.mNumIdentical = 0;

	// Save atom-pointers, if any
		PxsSolverBodyAtom* locAtom0 = staticAtom;
		if(atom0)
			locAtom0 = atom0;

		PxsSolverBodyAtom* locAtom1 = staticAtom;
		if(atom1) 
			locAtom1 = atom1;

		mBodyAtom0 = locAtom0;
		mBodyAtom1 = locAtom1;

	mTargetVelocity = 0;

	mBias = bias;

	mAppliedForce = 0;
	mAppliedVForce = 0;

	mAxis0 = normal;
	mAxis1 = normal;

	mMaxImpulse = maxImpulse;
	mMinImpulse = minImpulse;

	PxFloat normalResponse;

	PxFloat normalMagSq = normal.magnitudeSquared();
	/*
	response = (mIInv*(rel X n)) x rel + minv * n
	nresp = n * response = n* ((mIInv*(rel X n)) x rel + minv * n) =
	n*((mIInv*(rel X n)) x rel) + n*n*minv =
	(rel X n) * (mIInv * (rel X n)) + minv*|n|^2
	*/

	{
		const PxcVector R0XAxis0 = r0.cross(normal);
		const PxcVector IInvR0XAxis0 = locAtom0->mInvInertia.transform(R0XAxis0);

		mR0XAxis0 = R0XAxis0;
		mmIInvR0XAxis0 = IInvR0XAxis0;

		normalResponse = (R0XAxis0.dot(IInvR0XAxis0) + locAtom0->mInvMass*normalMagSq) * DOMINANCE_FLOAT_0;
	}

	{
		const PxcVector R1XAxis1 = r1.cross(normal);
		const PxcVector IInvR1XAxis1 = locAtom1->mInvInertia.transform(R1XAxis1);

		mR1XAxis1 = R1XAxis1;
		mmIInvR1XAxis1 = IInvR1XAxis1;

		normalResponse += (R1XAxis1.dot(IInvR1XAxis1) + locAtom1->mInvMass*normalMagSq) * DOMINANCE_FLOAT_1;
	}

	// Compute unit-response

	mVelMultiplier = normalResponse ? 1.0f/normalResponse : 0.0f;

	mPosMultiplier = mVelMultiplier * 0.8f;

	return this;
}

PXD_FORCE_INLINE PxsSolverConstraint* PxsSolverConstraint::initContactQuat(
											   PxsSolverBodyAtom* atom0, PxsSolverBodyAtom* atom1, 
											   const PxcVector& r0, const PxcVector& r1, 
											   const PxcVector& geometricError, const PxcQuaternion& quat, 
											   PxFloat staticFriction0, PxFloat dynamicFriction0,
											   PxFloat minImpulse, PxFloat maxImpulse,
											   PxsSolverBodyAtom* staticAtom, PxU8 domFlags)
{
	//PXS_SC_TYPE_CONTACT_QUAT
	mCallback = 0;
	mFlags.mType = PXS_SC_TYPE_CONTACT_QUAT;
	mFlags.mFlags = PXS_SC_FLAG_NOT_FINALIZED | domFlags;
	mFlags.mNumIdentical = 0;

	// Save atom-pointers, if any
		PxsSolverBodyAtom* locAtom0 = staticAtom;
		if(atom0)
			locAtom0 = atom0;

		PxsSolverBodyAtom* locAtom1 = staticAtom;
		if(atom1) 
			locAtom1 = atom1;

		mBodyAtom0 = locAtom0;
		mBodyAtom1 = locAtom1;

	// Save basic properties
	mRelative0C = r0;
	mRelative1C = r1;

	mStaticFriction0C = staticFriction0;
	mStaticFriction1C = 0;
	mDynamicFriction0C = dynamicFriction0;
	mDynamicFriction1C = 0;

	mContactBaseC = quat;
	mBiasC = geometricError;

	// Reset
	mAppliedFrictionForceC.setZero();
	mAppliedForceC = 0;

	mTargetVelocity = 0.0f;

	mMaxImpulseC = maxImpulse;
	mMinImpulseC = minImpulse;

	return this;
}


PXD_FORCE_INLINE PxsSolverConstraint* PxsSolverConstraint::initContactQuatAnis(
											   PxsSolverBodyAtom* atom0, PxsSolverBodyAtom* atom1, 
											   const PxcVector& r0, const PxcVector& r1, 
											   const PxcVector& geometricError, const PxcQuaternion& quat, 
											   PxFloat staticFriction0, PxFloat staticFriction1, 
											   PxFloat dynamicFriction0, PxFloat dynamicFriction1,
											   PxFloat minImpulse, PxFloat maxImpulse,
											   PxsSolverBodyAtom* staticAtom, PxU8 domFlags)
{
	//PXS_SC_TYPE_CONTACT_QUAT_ANIS
	mCallback = 0;
	mFlags.mType = PXS_SC_TYPE_CONTACT_QUAT_ANIS;
	mFlags.mFlags = PXS_SC_FLAG_NOT_FINALIZED | domFlags;
	mFlags.mNumIdentical = 0;

	// Save atom-pointers, if any
		PxsSolverBodyAtom* locAtom0 = staticAtom;
		if(atom0)
			locAtom0 = atom0;

		PxsSolverBodyAtom* locAtom1 = staticAtom;
		if(atom1) 
			locAtom1 = atom1;

		mBodyAtom0 = locAtom0;
		mBodyAtom1 = locAtom1;

	// Save basic properties
	mRelative0C = r0;
	mRelative1C = r1;

	mStaticFriction0C = staticFriction0;
	mStaticFriction1C = staticFriction1;
	mDynamicFriction0C = dynamicFriction0;
	mDynamicFriction1C = dynamicFriction1;

	mAnisotropicityC = staticFriction0/staticFriction1; 

	mContactBaseC = quat;
	mBiasC = geometricError;

	// Reset
	mAppliedFrictionForceC.setZero();
	mAppliedForceC = 0;

	mTargetVelocity = 0.0f;

	mMaxImpulseC = maxImpulse;
	mMinImpulseC = minImpulse;

	return this;
}

PXD_FORCE_INLINE PxsSolverConstraint*  PxsSolverConstraint::initLinear(
												PxsSolverBodyAtom* PXD_RESTRICT atom0, PxsSolverBodyAtom* PXD_RESTRICT atom1, 
												const PxcVector& r0, const PxcVector& r1, 
												const PxcVector& axis0, const PxcVector& axis1, 
												PxFloat targetVelocity, PxFloat bias,
												PxFloat minImpulse, PxFloat maxImpulse,
											    PxFloat solverExtrapolation, PxsSolverBodyAtom* staticAtom, PxU8 domFlags)
{
	//PXS_SC_TYPE_LINEAR
	mCallback = 0;
	mFlags.mType = PXS_SC_TYPE_LINEAR;
	mFlags.mFlags = PXS_SC_FLAG_NOT_FINALIZED | domFlags;
	mFlags.mNumIdentical = 0;

	// Save atom-pointers, if any
		PxsSolverBodyAtom* locAtom0 = staticAtom;
		if(atom0)
			locAtom0 = atom0;

		PxsSolverBodyAtom* locAtom1 = staticAtom;
		if(atom1) 
			locAtom1 = atom1;

		mBodyAtom0 = locAtom0;
		mBodyAtom1 = locAtom1;

	mAxis0 = axis0;
	mAxis1 = axis1;

	mSolverExtrapolation_Or_PatchID = solverExtrapolation;

	mTargetVelocity = targetVelocity;
	mBias = bias;

	mMinImpulse = minImpulse;
	mMaxImpulse = maxImpulse;

	mAppliedForce = 0;
	mAppliedVForce = 0;

	PxFloat normalResponse;

	/*
	response = (mIInv*(rel X n)) x rel + minv * n
	nresp = n * response = n* ((mIInv*(rel X n)) x rel + minv * n) =
	n*((mIInv*(rel X n)) x rel) + n*n*minv =
	(rel X n) * (mIInv * (rel X n)) + minv*|n|^2
	*/

	{
		const PxcVector R0XAxis0 = r0.cross(axis0);
		const PxcVector IInvR0XAxis0 = locAtom0->mInvInertia.transform(R0XAxis0);

		normalResponse = (R0XAxis0.dot(IInvR0XAxis0) + locAtom0->mInvMass*axis0.magnitudeSquared()) * DOMINANCE_FLOAT_0;

		mR0XAxis0 = R0XAxis0;
		mmIInvR0XAxis0 = IInvR0XAxis0;
	}

	// Compute unit-response

	{
		const PxcVector R1XAxis1 = r1.cross(axis1);
		const PxcVector IInvR1XAxis1 = locAtom1->mInvInertia.transform(R1XAxis1);

		normalResponse += (R1XAxis1.dot(IInvR1XAxis1) + locAtom1->mInvMass*axis1.magnitudeSquared()) * DOMINANCE_FLOAT_1;

		mR1XAxis1 = R1XAxis1;
		mmIInvR1XAxis1 = IInvR1XAxis1;
	}
	
	mImpulseMultiplier_Or_DynamicFriction = 0.0f;

	mVelMultiplier = normalResponse ? 1.0f/normalResponse : 0.0f;

	mPosMultiplier = mVelMultiplier * 0.7f;

	return this;
}

PXD_FORCE_INLINE PxsSolverConstraint*  PxsSolverConstraint::initAngular(
												PxsSolverBodyAtom* atom0, PxsSolverBodyAtom* atom1, 
												const PxcVector& axis0, const PxcVector& axis1, 
												PxFloat targetVelocity, PxFloat bias,
												PxFloat minImpulse, PxFloat maxImpulse,
												PxFloat solverExtrapolation, PxsSolverBodyAtom* staticAtom, PxU8 domFlags)
{
	//PXS_SC_TYPE_ANGULAR
	mCallback = 0;
	mFlags.mType = PXS_SC_TYPE_ANGULAR;
	mFlags.mFlags = PXS_SC_FLAG_NOT_FINALIZED | domFlags;
	mFlags.mNumIdentical = 0;

	// Save atom-pointers, if any
		PxsSolverBodyAtom* locAtom0 = staticAtom;
		if(atom0)
			locAtom0 = atom0;

		PxsSolverBodyAtom* locAtom1 = staticAtom;
		if(atom1) 
			locAtom1 = atom1;

		mBodyAtom0 = locAtom0;
		mBodyAtom1 = locAtom1;

	mSolverExtrapolation_Or_PatchID = solverExtrapolation;

	mTargetVelocity = targetVelocity;
	mBias = bias;

	mAppliedVForce = 0;
	mAppliedForce = 0;

	mMinImpulse = minImpulse;
	mMaxImpulse = maxImpulse;

	mAxis0 = axis0;
	mAxis1 = axis1;

	// Precompute

	// Compute unit-response
	PxFloat normalResponse =  (axis0.dot(locAtom0->mInvInertia.transform(axis0))) * DOMINANCE_FLOAT_0
							+ (axis1.dot(locAtom1->mInvInertia.transform(axis1))) * DOMINANCE_FLOAT_1;

	mImpulseMultiplier_Or_DynamicFriction = 0.0f;

	mVelMultiplier = normalResponse ? 1.0f/normalResponse : 0.0f;

	mPosMultiplier = mVelMultiplier * 0.7f;

	return this;
}

PXD_FORCE_INLINE PxsSolverConstraint* PxsSolverConstraint::initLinearFriction(
											   PxsSolverBodyAtom* atom0, PxsSolverBodyAtom* atom1,
											   const PxcVector& r0, const PxcVector& r1,
											   const PxcVector& axis0, const PxcVector& axis1, 
											   PxFloat bias, PxFloat staticFriction, PxFloat dynamicFriction,
											   void* patch,
											   PxsSolverBodyAtom* staticAtom, PxU8 domFlags)
{
	//PXS_SC_TYPE_LINEAR_FRICTION
	((void*&)mSolverExtrapolation_Or_PatchID) = patch;
	mCallback = 0;
	mFlags.mType = PXS_SC_TYPE_LINEAR_FRICTION;
	mFlags.mFlags = PXS_SC_FLAG_NOT_FINALIZED | domFlags;
	mFlags.mNumIdentical = 0;

	// Save atom-pointers, if any
		PxsSolverBodyAtom* locAtom0 = staticAtom;
		if(atom0)
			locAtom0 = atom0;

		PxsSolverBodyAtom* locAtom1 = staticAtom;
		if(atom1) 
			locAtom1 = atom1;

		mBodyAtom0 = locAtom0;
		mBodyAtom1 = locAtom1;

	mAxis0 = axis0;
	mAxis1 = axis1;

	mTargetVelocity = 0;
	mBias = bias;

	mStaticFriction = staticFriction;
	mImpulseMultiplier_Or_DynamicFriction = dynamicFriction;

	mAppliedForce = 0;
	mAppliedVForce = 0;

	// Temporary storage...
	mR0XAxis0 = r0;
	mR1XAxis1 = r1;

	return this;
}

PXD_FORCE_INLINE PxFloat PxsSolverConstraint::computeRelativeNormalVelocityContact() const
{
	PxFloat normalVel =  (mAxis0.dot(mBodyAtom0->mLinearVelocity) + mR0XAxis0.dot(mBodyAtom0->mAngularVelocity))
						-(mAxis1.dot(mBodyAtom1->mLinearVelocity) + mR1XAxis1.dot(mBodyAtom1->mAngularVelocity));

	return normalVel;

}

/***********************************************************************************************************/
//Compressed constraint inlines.


PXD_FORCE_INLINE void PxsSolverConstraintPool::startCompressedContact(
		PxsSolverBodyAtom* solverAtom0, PxsSolverBodyAtom* solverAtom1, 
		PxFloat restitution, PxU8 numTemporalPoints, PxU8 numPatchPoints, void *patch,
		PxFloat staticFriction, PxFloat dynamicFriction,
		PxsSolverBodyAtom* prefetchAtom0, PxsSolverBodyAtom* prefetchAtom1, PxU8 domFlags)//domFlags of type SolverConstraintFlags, only dom bits used!
{
	if((mSize+sizeof(PxsCompressedConstrHdr)) >= mCapacity)
		grow(sizeof(PxsCompressedConstrHdr));

	PxsCompressedConstrHdr* hdr = (PxsCompressedConstrHdr *)(mPool + mSize);
	mSize += sizeof(PxsCompressedConstrHdr);

	PxsSolverBodyAtom* locAtom0 = mStaticAtom;
	if(solverAtom0 != NULL)
		locAtom0 = solverAtom0;

	PxsSolverBodyAtom* locAtom1 = mStaticAtom;
	if(solverAtom1 != NULL)
		locAtom1 = solverAtom1;

	hdr->mType = PxsSolverConstraint::PXS_SC_TYPE_COMPRESSED_CONTACT;
	hdr->mFlags = domFlags;	//only 2 dominance bits used! 

	hdr->mNumNormalConstr = numTemporalPoints;
	hdr->mNumFrictionConstr = numPatchPoints;
	
	hdr->mSolverAtom0 = locAtom0;
	hdr->mSolverAtom1 = locAtom1;
	hdr->mPatch = patch;

	hdr->mStaticFriction = staticFriction;
	hdr->mDynamicFriction = dynamicFriction;

	hdr->mPrefetchAtom0 = prefetchAtom0;
	hdr->mPrefetchAtom1 = prefetchAtom1;
}



PXD_FORCE_INLINE void PxsSolverConstraintPool::addCompressedNormalConstr(
	PxsSolverBodyAtom* solverAtom0, PxsSolverBodyAtom* solverAtom1,
	PxcVector &r0, PxcVector &r1, PxcVector &normal, 
	PxFloat bias, PxFloat restitution, PxFloat bounceThreshold, 
	PxsSolverConstraintCallback *callback, 
	PxU8 domFlags)//domFlags of type SolverConstraintFlags, only dom bits used!
{
	if((mSize+sizeof(PxsCompressedConstrPt)) >= mCapacity)
		grow(sizeof(PxsCompressedConstrPt));

	PxsCompressedConstrPt* ptConstr = (PxsCompressedConstrPt *)(mPool + mSize);
	mSize += sizeof(PxsCompressedConstrPt);

	PxsSolverBodyAtom* locAtom0 = mStaticAtom;
	if(solverAtom0 != NULL)
		locAtom0 = solverAtom0;

	PxsSolverBodyAtom* locAtom1 = mStaticAtom;
	if(solverAtom1 != NULL)
		locAtom1 = solverAtom1;

	ptConstr->mAppliedForce = 0.0f;
	ptConstr->mAppliedVForce = 0.0f;

	ptConstr->mNormal = normal;

	PxFloat normalResponse;
	PxFloat normalMagSq = normal.magnitudeSquared();
	/*
		response = (mIInv*(rel X n)) x rel + minv * n
		nresp = n * response = n* ((mIInv*(rel X n)) x rel + minv * n) =
		n*((mIInv*(rel X n)) x rel) + n*n*minv =
		(rel X n) * (mIInv * (rel X n)) + minv*|n|^2
		*/

	const PxcVector R0XAxis0 = r0.cross(normal);
	{
		
		const PxcVector IInvR0XAxis0 = locAtom0->mInvInertia.transform(R0XAxis0);

		ptConstr->mR0XAxis0 = R0XAxis0;
		ptConstr->mmIInvR0XAxis0 = IInvR0XAxis0;

		normalResponse = (R0XAxis0.dot(IInvR0XAxis0) + locAtom0->mInvMass*normalMagSq) * COMPRESSED_DOMINANCE_FLOAT_0(domFlags);
	}

	const PxcVector R1XAxis1 = r1.cross(normal);
	{
		
		const PxcVector IInvR1XAxis1 = locAtom1->mInvInertia.transform(R1XAxis1);

		ptConstr->mR1XAxis1 = R1XAxis1;
		ptConstr->mmIInvR1XAxis1 = IInvR1XAxis1;

		normalResponse += (R1XAxis1.dot(IInvR1XAxis1) + locAtom1->mInvMass*normalMagSq) * COMPRESSED_DOMINANCE_FLOAT_1(domFlags);
	}

	// Compute unit-response

	ptConstr->mVelMultiplier = normalResponse ? 1.0f/normalResponse : 0.0f;

	ptConstr->mBiasPosMultiplier = bias * (ptConstr->mVelMultiplier * 0.8f);

	ptConstr->mTargetVelocity = 0.0f;
	if(restitution > 0.0f)
	{
		//compute relative normal velocity
		PxFloat vrel =  
			(normal.dot(locAtom0->mLinearVelocity) + R0XAxis0.dot(locAtom0->mAngularVelocity))
			-(normal.dot(locAtom1->mLinearVelocity) + R1XAxis1.dot(locAtom1->mAngularVelocity));

		if(vrel < bounceThreshold)
		{
			ptConstr->mBiasPosMultiplier = 0.0f;
			ptConstr->mTargetVelocity = -vrel * restitution;
		}
	}

	ptConstr->mCallback = callback;
}

PXD_FORCE_INLINE void PxsSolverConstraintPool::addCompressedFricConstr(
	PxcVector &ra, PxcVector &rb, 
	PxcVector &axis, PxFloat bias,
	PxsSolverConstraintCallback* frictionCallback)
{
	if((mSize+sizeof(PxsCompressedConstrFric)) >= mCapacity)
		grow(sizeof(PxsCompressedConstrFric));

	PxsCompressedConstrFric* frConstr = (PxsCompressedConstrFric *)(mPool + mSize);
	mSize += sizeof(PxsCompressedConstrFric);


	frConstr->mAxis = axis;
	frConstr->mBias = bias;

	frConstr->mAppliedForce = 0.0f;

	frConstr->mR0XAxis0 = ra;
	frConstr->mR1XAxis1 = rb;

	frConstr->mFinalized = 0;
	frConstr->mBroken = 0;
	frConstr->mFrictionCallback = frictionCallback;

	//write to all value to be more cache friendly.
	frConstr->mmIInvR0XAxis0.setZero();
	frConstr->mmIInvR1XAxis1.setZero();
}

#ifdef PXS_USE_COMBINED_FRICTION_AXIS_CONSTRAINT
PXD_FORCE_INLINE void PxsSolverConstraintPool::addCompressedFricConstrCombi(
	PxcVector &ra, PxcVector &rb, 
	PxcVector &axisY, PxcVector &axisZ, PxFloat biasY, PxFloat biasZ, 
	PxsSolverConstraintCallback* frictionCallback)
{
	if((mSize+sizeof(PxsCompressedConstrFricCombi)) >= mCapacity)
		grow(sizeof(PxsCompressedConstrFricCombi));

	PxsCompressedConstrFricCombi* frConstr = (PxsCompressedConstrFricCombi *)(mPool + mSize);
	mSize += sizeof(PxsCompressedConstrFricCombi);


	frConstr->mAxisY = axisY;
	frConstr->mBiasY = biasY;

	frConstr->mAxisZ = axisZ;
	frConstr->mBiasZ = biasZ;

	frConstr->mAppliedForce.setZero();

	frConstr->mR0 = ra;
	frConstr->mR1 = rb;

	frConstr->mFinalized = 0;
	frConstr->mBroken = 0;
	frConstr->mFrictionCallback = frictionCallback;
}
#endif	// PXS_USE_COMBINED_FRICTION_AXIS_CONSTRAINT

#endif
