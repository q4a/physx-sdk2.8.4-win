#ifndef PXS_D6JOINT_H
#define PXS_D6JOINT_H

#include "PxnCustomConstraint.h"
#include "PxcTransform.h"
#include "PxcVector.h"
#include "PxcListNode.h"
#include "PxsDynamics.h"
#include "PxsSolverConstraint.h"

class PxsBodyAtom;
class PxsDynamicsContext;
class PxsSolverConstraintPool;

class PxsD6Joint : public PxsConstraintBase, public PxnD6Joint
{
public:
									PxsD6Joint(PxsDynamicsContext* dc, PxU32 index);
									~PxsD6Joint();

			void					init(const PxdD6JointDesc& desc);

	// Implements PxnD6Joint
	virtual	PxdHandle				getHandleV()									const;
	virtual	void					destroyV();

	virtual	void					getAtomsV(PxnBodyAtom*& a0, PxnBodyAtom*& a1)	const;

	virtual	const PxdTransform&		getFrame0V()									const;
	virtual	void					setFrame0V(const PxdTransform& t);

	virtual	const PxdTransform&		getFrame1V()									const;
	virtual	void					setFrame1V(const PxdTransform& t);

	virtual	PxInt					getJointTypeV()									const;
	virtual	void					setJointTypeV(PxInt i);

	virtual	PxInt					getDriveTypeV()									const;
	virtual	void					setDriveTypeV(PxInt i); 

	virtual	PxFloat					getLimitValueV(PxdD6JointSubProperty p)			const;
	virtual	void					setLimitValueV(PxdD6JointSubProperty p, PxFloat f);

	virtual	PxFloat					getLimitSpringV(PxdD6JointSubProperty p)		const;
	virtual	void					setLimitSpringV(PxdD6JointSubProperty p, PxFloat f);

	virtual	PxFloat					getLimitDampingV(PxdD6JointSubProperty p)		const;
	virtual	void					setLimitDampingV(PxdD6JointSubProperty p, PxFloat f);

	virtual	PxFloat					getLimitRestitutionV(PxdD6JointSubProperty p)	const;
	virtual	void					setLimitRestitutionV(PxdD6JointSubProperty p, PxFloat f);

	virtual	PxFloat					getDriveSpringV(PxdD6JointSubProperty p)		const;
	virtual	void					setDriveSpringV(PxdD6JointSubProperty p, PxFloat f);

	virtual	PxFloat					getDriveDampingV(PxdD6JointSubProperty p)		const;
	virtual	void					setDriveDampingV(PxdD6JointSubProperty p, PxFloat f);

	virtual	PxFloat					getDriveLimitV(PxdD6JointSubProperty p)			const;
	virtual	void					setDriveLimitV(PxdD6JointSubProperty p, PxFloat f);

	virtual	PxFloat					getDriveGearV()									const;
	virtual	void					setDriveGearV(PxFloat f);

	virtual	const PxdTransform&		getDrivePositionV()								const;
	virtual	void					setDrivePositionV(const PxdTransform& f);

	virtual	void					getDriveVelocityV(PxdVector& lin, PxdVector& ang) const;
	virtual	void					setDriveVelocityV(const PxdVector& lin, const PxdVector& ang);

	virtual	PxFloat					getMaxLinearImpulseV()							const;
	virtual	void					setMaxLinearImpulseV(PxFloat f);

	virtual	PxFloat					getMaxAngularImpulseV()							const;
	virtual	void					setMaxAngularImpulseV(PxFloat f);

	virtual	void					getImpulseV(PxdVector& lin, PxdVector& ang)		const;

	virtual	bool					getImpulseReportV()								const;
	virtual	void					setImpulseReportV(bool b);

	virtual PxFloat					getSolverExtrapolationFactorV()					const;
	virtual void					setSolverExtrapolationFactorV(PxFloat f);

	virtual bool					getUseAccelerationSpringsV()					const;
	virtual void					setUseAccelerationSpringsV(bool b);

	virtual bool					getConstrainPerpendicularDirsV()				const;
	virtual void					setConstrainPerpendicularDirsV(bool b);

	virtual	bool					getJointBrokenV()								const;

	virtual	PxFloat					getDominance0V()								const;
	virtual	void					setDominance0V(PxFloat);

	virtual	PxFloat					getDominance1V()								const;
	virtual	void					setDominance1V(PxFloat);

	//~Implements PxnD6Joint

	// Helpers
	PXD_FORCE_INLINE	PxU32		getIndex()										const
	{
		return mIndex;
	}

	// Implements PxsConstraintBase
	virtual	PxU32					setupSolverConstraint(PxsSolverConstraintPool& scpool, PxFloat dt, PxU32& thresholdPairCount);
	//~Implements PxsConstraintBase

private:
			void					updateDerived();

	//Helper methods to code/decode bit-fields
	PXD_INLINE PxU32		getJointTypeBitfield()							const
	{
		//Encode AxisMotions into bitfield
		//Encoded LSB to MSB as groups of two bits per axis. Each two bits is interpreted as 
		//0-free, 1-limited, 2-locked
		PxU32 bits = 0;

		for(PxUInt i = 0; i < 3; i++)
		{
			//Encode linear
			bits |= (((PxUInt)mLinMotion[i])&0x03 << i);
			//Encode angular
			bits |= (((PxUInt)mAngMotion[i])&0x03 << (i+3));
		}

		return bits;
	}

	PXD_INLINE void		setJointTypeBitfield(PxU32 bits)
	{
		//Decode bitfield into AxisMotions
		//Se above for format

		for(PxUInt i = 0; i < 3; i++)
		{
			mLinMotion[i] = (AxisMotionType)(bits & 0x03);
			bits >>= 2;
		}

		for(PxUInt i = 0; i < 3; i++)
		{
			mAngMotion[i] = (AxisMotionType)(bits & 0x03);
			bits >>= 2;
		}
	}

	PXD_INLINE PxU32	getJointDriveBitfield()								const
	{
		//Encode AxisDrives type into a bitfield
		//Encoded LSB to MSB as a group of two bits per axis in order x,y,z,tw,sw,slerp
		//Each two bits represents 00-none, 01-position, 10-velocity, 11-both
		PxU32 bits = 0;

		for(PxUInt i = 0; i < 6; i++)
		{
			bits |= (((PxUInt)mDrive[i].mType)&0x03 << (i));
		}

		return bits;
	}

	PXD_INLINE void		setJointDriveBitfield(PxU32 bits)
	{
		for(PxUInt i = 0; i < 6; i++)
		{
			mDrive[i].mType = (bits & 0x03);
			bits >>= 2;
		}
	}

	//Helper-methods when setting up the solver constraints
	struct AxisLimit;
	PXD_INLINE void				setLinSC(PxsSolverConstraintPool& scpool, bool ineq, const PxcVector& ra,
										const PxcVector& rb, const PxcVector& normal, PxFloat bias, PxFloat maxImpulse,
										AxisLimit* limit = 0, PxFloat dt = 0.01f, PxU8 identicalCount = 0);
	PXD_INLINE void				setAngSC(PxsSolverConstraintPool& scpool, bool ineq, const PxcVector& normal, 
										PxFloat bias, PxFloat maxImpulse, AxisLimit* limit = 0, PxFloat dt = 0.01f, PxU8 identicalCount = 0);

	// Properties
	PxsDynamicsContext*		mDynamicsContext;
	PxU32					mIndex;

	PxcTransform			mFrame0;
	PxcTransform			mFrame1;

	// Flags for motion type in each direction
	enum AxisMotionType
	{
		D6_MOTION_FREE = 0,
		D6_MOTION_LIMITED = 1,
		D6_MOTION_LOCKED = 2
	};
	AxisMotionType			mLinMotion[3];	//x,y,z
	AxisMotionType			mAngMotion[3];	//tw, sw1, sw2

	// Limits
	struct AxisLimit
	{
		PxFloat		mValue;
		PxFloat		mRestitution;
		PxFloat		mSpring;
		PxFloat		mDamping;
	};
	AxisLimit				mLimits[5];		//linear, sw1, sw2, twh, twl

	// Description of drive per axis
	enum AxisDriveType
	{
		D6_DRIVE_NONE = 0,
		D6_DRIVE_POSITION = 1,
		D6_DRIVE_VELOCITY = 2
	};
	struct AxisDrive
	{
		PxU32		mType; //One or a combination of AxisDriveType
		PxFloat		mSpring;
		PxFloat		mDamping;
		PxFloat		mImpulseLimit;
	};
	AxisDrive				mDrive[6];		//x,y,z, tw, sw, slerp

	// Drive targets
	PxcTransform			mDrivePosition;
	PxcVector				mDriveVelocity[2];

	PxFloat					mDriveGear;
	bool					mImpulseReport;

	// Derived quantities
	PxFloat					mTanQuarterSwing1Max;
	PxFloat					mTanQuarterSwing2Max;
	PxFloat					mTanHalfSwing1Max;
	PxFloat					mTanHalfSwing2Max;
	PxFloat					mTanQuarterTwistLow;
	PxFloat					mTanQuarterTwistHigh;
	
	PxU8					mLinearLockedCount;
	PxU8					mAngularLockedCount;

	PxU8					mLinearLimitedCount;
	bool					mAngularLimited;
	bool					mLinearDrive;
	bool					mAngularDrive;
	bool					mDerivedDirty;

	// Setup during SC-setup
	PxsSolverBodyAtom*		mSolverAtom0;
	PxsSolverBodyAtom*		mSolverAtom1;

	PxcListNode<PxsConstraintBase>	mConstraintListNode;

	/// Sauce -- I moved these values to be very close to the SolverCallback structure.
	/// These are the only values needed by the solver during the last iteration
	/// callback.
	PxFloat					mLinBreakImpulse;
	PxFloat					mAngBreakImpulse;

	PxFloat					mSolverExtrapolation;
	bool					mUseAccelerationSprings;
	bool					mConstrainPerpendicularDirs;

	bool					mJointBroken;	//broken last solve-run

	struct SolverCallback : public PxsSolverConstraintCallback
	{
		SolverCallback(PxsD6Joint& owner)
			: /*mAccumulatedImpulse(0.0f), mAccumulatedTorque(0.0f),*/ mOwner(owner)
		{
			mAccumulatedImpulse.setZero();
			mAccumulatedTorque.setZero();
		}

		// Implements PxsSolverConstraintCallback
		virtual void lastIterationCallback(const PxsSolverConstraint& sc)
		{
			if(sc.mFlags.mType == PxsSolverConstraint::PXS_SC_TYPE_LINEAR)
				mAccumulatedImpulse += sc.mAxis0 * sc.mAppliedVForce;
			else if(sc.mFlags.mType == PxsSolverConstraint::PXS_SC_TYPE_ANGULAR)
				mAccumulatedTorque += sc.mAxis0 * sc.mAppliedVForce;

			if(!(sc.mFlags.mFlags & PxsSolverConstraint::PXS_SC_FLAG_UNBREAKABLE))
			{
				if(sc.mFlags.mType == PxsSolverConstraint::PXS_SC_TYPE_LINEAR &&
					PxcAbs(sc.mAppliedVForce) >= mOwner.mLinBreakImpulse)
				{
 					mOwner.mJointBroken = true;
				}
				else if(sc.mFlags.mType == PxsSolverConstraint::PXS_SC_TYPE_ANGULAR &&
						PxcAbs(sc.mAppliedVForce) >= mOwner.mAngBreakImpulse)
				{
					mOwner.mJointBroken = true;
				}
			}
		}
		
		virtual void lastIterationCallback(PxFloat vForce, PxFloat force)
		{
			PXN_ASSERT(0); //Should not be called for a D6 constraint.
		}
		
		virtual void frictionBrokenCallback()
		{
			PXN_ASSERT(0); //Should not be called for a D6 constraint.
		}

		//~Implements PxsSolverConstraintCallback

		PxcVector		mAccumulatedImpulse;
		PxcVector		mAccumulatedTorque;
		PxsD6Joint&		mOwner;
	} mCallback;
};


#endif
