#ifndef NX_PHYSICS_D6Joint
#define NX_PHYSICS_D6Joint
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Joint.h"
#include "NxJointLimitSoftDesc.h"
#include "NxJointLimitSoftPairDesc.h"
#include "NxMotorDesc.h"

#define D6JOINT_DIRECT_SOLVE 0

class D6Joint : public Joint
{
public:
	virtual void release();
	virtual		void						loadFromDesc(const NvJointDesc &, const NvJointD6Desc &);
 
	virtual		void						setSolverExtrapolationFactor(NxReal sef);
	virtual		void						setUseAccelerationSpring(bool b);

	void setDriveP(const NxVec3 &position);
	void setDriveQ(const NxQuat &orientation); 

	void setDriveLinearVelocity(const NxVec3 &linVel);

	void setDriveAngularVelocity(const NxVec3 &angVel);


	virtual		operator					Joint&();
	virtual		Joint&						getJoint();
	virtual		D6Joint*					isD6Joint();

	//group access:
											D6Joint(const NvJointDesc & desc, const NvJointD6Desc &sdesc);

											~D6Joint();

	virtual		void						processToAxisConstraints(NxReal dt);
	virtual		void						visualize(NxFoundation::DebugRenderable&);

	virtual		void						projectPose(Body * childBody);

				void						comShiftUpdate();

	NX_INLINE	void						setLinMotion(NxU32 index, const NxD6JointMotion & value);
	NX_INLINE	NxD6JointMotion 			getLinMotion(NxU32 index) const { return linMotion[index]; }
	NX_INLINE	void						setAngMotion(NxU32 index, const NxD6JointMotion & value);
	NX_INLINE	NxD6JointMotion 			getAngMotion(NxU32 index) const { return angMotion[index]; }

	NX_INLINE	void						setLinearLimit(const NxJointLimitSoftDesc & l);
	NX_INLINE	void						setSwing1Limit(const NxJointLimitSoftDesc & l);
	NX_INLINE	void						setSwing2Limit(const NxJointLimitSoftDesc & l);
	NX_INLINE	void						setTwistLimit(const NxJointLimitSoftPairDesc & l);

	NX_INLINE	NxJointLimitSoftDesc 		getLinearLimit() const { return linearLimit; }
	NX_INLINE	NxJointLimitSoftDesc 		getSwing1Limit() const { return swing1Limit; }
	NX_INLINE	NxJointLimitSoftDesc 		getSwing2Limit() const { return swing2Limit; }
	NX_INLINE	NxJointLimitSoftPairDesc	getTwistLimit()  const { return twistLimit;  }

	NX_INLINE	void						setLinDrive(NxU32 index, const NxJointDriveDesc & value);
	NX_INLINE	NxJointDriveDesc			getLinDrive(NxU32 index) const { return linearDrive[index]; }

	NX_INLINE	void						setSwingDrive(const NxJointDriveDesc & value);
	NX_INLINE	NxJointDriveDesc			getSwingDrive() const { return swingDrive; }
	NX_INLINE	void						setTwistDrive(const NxJointDriveDesc & value);
	NX_INLINE	NxJointDriveDesc			getTwistDrive() const { return twistDrive; }
	NX_INLINE	void						setSlerpDrive(const NxJointDriveDesc & value);
	NX_INLINE	NxJointDriveDesc			getSlerpDrive() const { return slerpDrive; }

	NX_INLINE	void						setFlags(NxU32 f);
	NX_INLINE	NxU32						getFlags() const { return d6flags; }

	NX_INLINE	void						setDrivePQ(const NxVec3 & dp, const NxQuat & dq);
	NX_INLINE	void						getDrivePQ(NxVec3 & dp, NxQuat & dq) const { dp = drivePosition; dq = driveOrientation; }

	NX_INLINE	void						setDriveVel(const NxVec3 & lv, const NxVec3 & av);
	NX_INLINE	void						getDriveVel(NxVec3 & lv, NxVec3 & av) const			{ lv = driveLinearVelocity; av = driveAngularVelocity; }

	NX_INLINE	void						setProjectionParams(NxReal dist, NxReal angle)		{	projectionDistance = dist; projectionAngle = angle; }
	NX_INLINE	void						getProjectionParams(NxReal & dist, NxReal & angle) const {	dist = projectionDistance; angle = projectionAngle; }

	NX_INLINE	void						setGearRatio(const NxReal ratio);
	NX_INLINE	NxReal						getGearRatio() const { return gearRatio; }


	virtual void disableAxCons();
private:

	typedef enum
	{
		Eq,
		Ineq
	} EqType;

	void setLinAC(EqType type,
				  const NxVec3 &ra,
				  const NxVec3 &rb, 
                  const NxVec3 &normal, 
                  NxReal bias, 
                  NxReal maxForce,
    			  NxJointLimitSoftDesc *limit = NULL, NxReal dt = 0.01f);

	void setAngAC(EqType type,
				  const NxVec3 &normal, 
				  NxReal bias, 
				  NxReal maxForce,
    			  NxJointLimitSoftDesc *limit = NULL, NxReal dt = 0.01f);

	NxD6JointMotion			linMotion[3], angMotion[3];

	NxJointLimitSoftDesc		linearLimit;
	NxJointLimitSoftDesc		swing1Limit;
	NxJointLimitSoftDesc		swing2Limit;
	NxJointLimitSoftPairDesc	twistLimit;

	/* drive */
	NxJointDriveDesc		linearDrive[3];

	// two different angular drive models are provided: swing & twist, or spherical.
	//AM: TODO: remove one of these and just use the data field of sphericalDrive!
	NxJointDriveDesc		swingDrive, twistDrive;
	NxJointDriveDesc		slerpDrive;			

	NxVec3					drivePosition;
	NxQuat					driveOrientation;

	NxVec3					driveLinearVelocity;
	NxVec3					driveAngularVelocity;

	NxReal					projectionDistance;
	NxReal					projectionAngle;	

	NxReal					gearRatio;

	// precomputed auxiliaries
	NxReal					tanQuarterSwing1Max;
	NxReal					tanQuarterSwing2Max;
	NxReal					tanHalfSwing1Max;
	NxReal					tanHalfSwing2Max;
	NxReal					tanQuarterTwistLow;
	NxReal					tanQuarterTwistHigh;

	NxReal					tanQuarterProjectionAngle;
	NxU32					d6flags;
	bool					angularLimited, linearLimited;

	bool					useCorrectedAnchor;
	NxVec3					correctedAnchor;

	NxVec3                  solve_gear_axis[2];
	
	PxdD6Joint				d6Constraint;

};

#endif
