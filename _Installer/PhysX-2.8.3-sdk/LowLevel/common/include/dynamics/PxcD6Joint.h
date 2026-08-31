#ifndef PXS_D6JOINT_H
#define PXS_D6JOINT_H

#include "PxcTransform.h"
#include "PxcMatrix33.h"

struct ConcreteConstraintGenerator;
struct PxcD6JointState;

class PxcD6Joint
{
public:
	// Flags for motion type in each direction
	enum AxisMotionType
	{
		MOTION_FREE		= 0,
		MOTION_LIMITED	= 1,
		MOTION_LOCKED	= 2
	};
	// Description of drive per axis
	enum AxisDriveType
	{
		DRIVE_POSITION	= 1,
		DRIVE_VELOCITY	= 2
	};

	PxcTransform			mFrame0;
	PxcTransform			mFrame1;

	PxFloat					mLinearBreakImpulse;
	PxFloat					mAngularBreakImpulse;

	PxFloat					mDominance0, mDominance1;


	// Limits
	struct AxisLimit
	{
		PxFloat		mValue;
		PxFloat		mRestitution;
		PxFloat		mSpring;
		PxFloat		mDamping;
		AxisLimit(): mValue(0), mRestitution(0), mSpring(0), mDamping(0) {}
	};
	AxisLimit				mLimits[5];		//linear, sw1, sw2, twh, twl

	// Drive
	struct AxisDrive
	{
		PxU32		mType;					//One or a combination of AxisDriveType
		PxFloat		mSpring;
		PxFloat		mDamping;
		PxFloat		mImpulseLimit;
		AxisDrive(): mType(0), mSpring(0), mDamping(0), mImpulseLimit(0) {}
	};
	AxisDrive				mDrive[6];		//x,y,z, tw, sw, slerp

	PxcTransform			mDrivePosition;
	PxcVector				mDriveLinearVelocity;
	PxcVector				mDriveAngularVelocity;
	PxFloat					mDriveGear;

	// Derived quantities
	PxU8					mLinearLockedCount;
	PxU8					mAngularLockedCount;
	bool					mLinearLimited;
	bool					mAngularLimited;
	bool					mLinearDrive;
	bool					mAngularDrive;

	PxFloat					mTanQuarterSwing1Max;
	PxFloat					mTanQuarterSwing2Max;
	PxFloat					mTanHalfSwing1Max;
	PxFloat					mTanHalfSwing2Max;
	PxFloat					mTanQuarterTwistLow;
	PxFloat					mTanQuarterTwistHigh;

	PxU32					getJointTypeBitfield()				const;
	void					setJointTypeBitfield(PxU32 bits);

	PxU32					getJointDriveBitfield()				const;
	void					setJointDriveBitfield(PxU32 bits);

	void					updateLimitAngles();		// combination of (1<<angular dof)

	const AxisMotionType *	getLinMotion()						const	{	return mLinMotion;	}
	const AxisMotionType *	getAngMotion()						const	{	return mAngMotion;	}


	template<class ConcreteConstraintGenerator>
	void	checkSymmetricLimit(ConcreteConstraintGenerator &ccg,
								const PxcVector & axis, 
								PxFloat angle, PxFloat limitAngle,
								PxFloat padding, const PxcD6Joint::AxisLimit &limit)				const;

	template<class ConcreteConstraintGenerator>
	void	createAngularDriveConstraints(PxcD6JointState &, ConcreteConstraintGenerator &)			const;

	template<class ConcreteConstraintGenerator>
	void	createLinearDriveConstraints(PxcD6JointState &, ConcreteConstraintGenerator &)			const;

	template<class ConcreteConstraintGenerator>
	void	createAngularHardConstraints(PxcD6JointState &, ConcreteConstraintGenerator &)			const;

	template<class ConcreteConstraintGenerator>
	void	createLinearHardConstraints(PxcD6JointState &, ConcreteConstraintGenerator &)			const;

private:
	AxisMotionType			mLinMotion[3];	//x,y,z
	AxisMotionType			mAngMotion[3];	//tw, sw1, sw2


	// math helpers

	PxFloat			interpolateEllipse(PxFloat w1, PxFloat w2, PxFloat v1, PxFloat v2)				const;
	PxcQuaternion	rotationArc(const PxcVector& v0, const PxcVector& v1)							const;
	void			computeJwQ(PxcVector JwQ[4], 
							   const PxcQuaternion& qa, 
							   const PxcQuaternion& qb)												const;
	void			separateSwingTwist(const PxcQuaternion& q, PxcQuaternion& swing, PxcQuaternion& twist) const;

	// constraint generation helpers

	void			angSymmLimit(ConcreteConstraintGenerator &ccg,
								 const PxcVector & axis, 
								 PxFloat angle, PxFloat limitAngle, PxFloat padding,
								 const PxcD6Joint::AxisLimit &limit)								const;

	template<class ConcreteConstraintGenerator>
	void	createSlerpDriveConstraints(PxcD6JointState &, ConcreteConstraintGenerator &)			const;

	template<class ConcreteConstraintGenerator>
	void	createSwingAndTwistDriveConstraints(PxcD6JointState &, ConcreteConstraintGenerator &)	const;

	static const PxFloat LINEAR_PAD;
	static const PxFloat TANHALF_PAD;
	static const PxFloat TANQUARTER_PAD;
};


struct PxcD6JointState 
{
	PxFloat			dt;
	PxFloat			invDt;
	PxcVector		ra;
	PxcVector		rb;
	PxcVector		relAngVel;
	PxcTransform	cA2w;					// constraint frame A to world
	PxcTransform	cB2w;					// constraint frame B to world
	PxcTransform	cB2cA;					// constraint frame B to constraint frame A
	PxcMatrix33		cA2wMat;				// constraint frame A to world
	PxcMatrix33		cB2wMat;				// constraint frame B to world
	PxcQuaternion   d2cA;					// drive quat in frame A, flipped to ensure positive w
	PxcQuaternion   d2w;					// drive quat in world frame

	PxcD6JointState(PxcD6Joint &joint, 
					const PxcTransform &b2wA,
					const PxcTransform &b2wB, 
					const PxcVector &angVelA,
					const PxcVector &angVelB,
					PxFloat stepSize);

	void computeDriveQuaternions(const PxcQuaternion &driveOrientation);
};


#endif
