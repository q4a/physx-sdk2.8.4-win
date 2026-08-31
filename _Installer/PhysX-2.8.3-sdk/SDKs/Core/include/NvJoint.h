#ifndef NX_PHYSICS_NV_JOINT
#define NX_PHYSICS_NV_JOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "NvQueuedEvent.h"
#include "NxJointLimitSoftPairDesc.h"
#include "NxJointLimitPairDesc.h"
#include "NxSpringDesc.h"
#include "NxMotorDesc.h"

class NvJoint;
class NvScene;
class NpJoint;
class NvBody;

//notify is implemented in Utilities.h!  TODO: move somewhere else??
class NvJointBreakEvent : public NvQueuedEvent
	{
	public:
	NvJoint & joint;
	NxReal largeForce;
	NvJointBreakEvent(NxReal f, NvJoint & j) : NvQueuedEvent(NX_QE_JOINT_BREAK), joint(j), largeForce(f) {}
	//virtual void notify();
	};

class NvJointDesc
	{
	public:
	NxQuat					frameQ[2];
	NxVec3					frameP[2];
	NvBody *				bodies[2];
	NpJoint*				npJoint;
	NxReal					linBreakForce;
	NxReal					angBreakForce;
	NxReal					solverExtrapolationFactor;
	bool					useAccelerationSpring;

	NxU32					publicFlags;
	NxJointType				type;
	};

class NvJointPrismaticDesc
	{
	};
	
class NvJointCylindricalDesc
	{
	};

class NvJointPointOnLineDesc
	{
	};

class NvJointPointOnPlaneDesc
	{
	};

class NvJointFixedDesc
	{
	};

class NvJointRevoluteDesc
	{
	public:
	NxJointLimitPairDesc		limit;
	NxMotorDesc					motor;
	NxSpringDesc				spring;
	NxJointProjectionMode		projectionMode;
	NxReal						linearProjection;
	NxReal						angularProjection;	
	NxU32						flags;
	};

class NvJointSphereicalDesc
	{
	public:
	NxJointLimitDesc			swingLimit;
	NxJointLimitPairDesc		twistLimit;
	NxSpringDesc				twistSpring;
	NxSpringDesc				swingSpring;
	NxSpringDesc				jointSpring;
	NxVec3						swingAxis;
	NxJointProjectionMode		projectionMode;
	NxReal						linearProjection;
	NxU32						flags;
	};

class NvJointDistanceDesc
	{
	public:
	NxReal						min;
	NxReal						max;
	NxSpringDesc				spring;
	NxU32						flags;
	};

class NvJointPulleyDesc
	{
	public:
	NxVec3						suspension[2];
	NxReal						targetDistance;
	NxReal						stiffness;
	NxReal						ratio;
	NxMotorDesc					motor;
	NxU32						flags;
	};

class NvJointD6Desc
	{
	public:
	NxD6JointMotion				linMotion[3];
	NxD6JointMotion				angMotion[3];
	NxJointLimitSoftDesc		linLimit;
	NxJointLimitSoftDesc		swing1Limit;
	NxJointLimitSoftDesc		swing2Limit;
	NxJointLimitSoftPairDesc	twistLimit;
	NxJointDriveDesc			linDrive[3];
	NxJointDriveDesc			twistDrive;
	NxJointDriveDesc			swingDrive;
	NxJointDriveDesc			slerpDrive;
	NxVec3						driveP;
	NxQuat						driveQ;
	NxVec3						driveLinearVelocity;
	NxVec3						driveAngularVelocity;
	NxJointProjectionMode		projectionMode;
	NxReal						linearProjection;
	NxReal						angularProjection;	
	NxReal						gearRatio;
	NxU32						flags;
	};


class NvJoint
	{
	public:
	enum Flags 
		{
		//012:
		JF_IS_IN_A_SCENE			= 1,		//indicates that this body has been added to a scene. Used to catch user bugs.
		JF_LIMIT_POINT_ON_BODY1		= 1 << 1,
		//345: NxJoint::State bits	//see shifts below!
		//67: NxJoint::Method bits	//this is the prefered method and should not be changed by the actual method, or fix prefersReducedMode().
		JF_COLLISION_ENABLED		= 1 << 8,
		JF_VISUALIZATION			= 1 << 9,   /* COMMENT(djs): visualization should be at a higher level */
		};

	//virtual		void						loadFromDesc(const NvJointDesc &) = 0;	//necessitated by the D6 joint because it has a lot of properties that can't be set individually due to a complex updateDerived().
	
	virtual void							loadFromDesc(const NvJointDesc &, const NvJointPrismaticDesc &)    { assert(0); } // If you assert you called the wrong loadFromDesc.
	virtual void							loadFromDesc(const NvJointDesc &, const NvJointCylindricalDesc &)  { assert(0); }
	virtual void							loadFromDesc(const NvJointDesc &, const NvJointPointOnLineDesc &)  { assert(0); }
	virtual void							loadFromDesc(const NvJointDesc &, const NvJointPointOnPlaneDesc &) { assert(0); }
	virtual void							loadFromDesc(const NvJointDesc &, const NvJointFixedDesc &)        { assert(0); }
	virtual void							loadFromDesc(const NvJointDesc &, const NvJointRevoluteDesc &)     { assert(0); }
	virtual void							loadFromDesc(const NvJointDesc &, const NvJointSphereicalDesc &)   { assert(0); }
	virtual void							loadFromDesc(const NvJointDesc &, const NvJointDistanceDesc &)     { assert(0); }
	virtual void							loadFromDesc(const NvJointDesc &, const NvJointPulleyDesc &)       { assert(0); }
	virtual void							loadFromDesc(const NvJointDesc &, const NvJointD6Desc &)           { assert(0); }
	
	virtual		void						release() = 0;

//@	virtual		void						setScene(NvScene*) = 0;
	virtual		NvScene*					getScene() = 0;

	virtual		void						setNpJoint(NpJoint* npJ) = 0;
	virtual		NpJoint*					getNpJoint() = 0;

	/* QUERY(djs): can't we just use positions and quats here */
	/*
	virtual		const NxVec3&				getNormalAS(NxU32 i) const = 0;
	virtual		const NxVec3&				getBinormalAS(NxU32 i) const = 0;
	virtual		const NxVec3&				getAxisAS(NxU32 i) const = 0;

	virtual		void						setNormalAS(NxU32 i, const NxVec3 &v) = 0;
	virtual		void						setBinormalAS(NxU32 i, const NxVec3 &v) = 0;
	virtual		void						setAxisAS(NxU32 i, const NxVec3 &v) = 0;

	virtual		void						updateBodySpaceFrame(unsigned b) const = 0;

	virtual		const NxVec3&				getAnchorPointAS(NxU32 i) const = 0;
	virtual		void						setAnchorPointAS(NxU32 i, const NxVec3 &v) = 0;

	virtual		const NxQuat&				getQuatAS(NxU32 i) const = 0;
	virtual		void						setQuatAS(NxU32 i, const NxQuat &v) = 0;
	*/
	//actor space joint frames of the joint's actor 0 or 1.
	virtual		NxVec3						getFrameP(NxU32 i)				const = 0;
	virtual		NxQuat						getFrameQ(NxU32 i)				const = 0;
	virtual		void						setFrameP(NxU32 i, const NxVec3 &)	  = 0;
	virtual		void						setFrameQ(NxU32 i, const NxQuat &)	  = 0;
	virtual		void						setFramePQ(NxU32 i, const NxVec3 &, const NxQuat &)	  = 0;


	/* COMMENT(djs): since breakage is written by the dynamics update, should it have a isBroken_API method? */
	virtual		void						getBreakForce(NxReal & linearForce, NxReal & angularForce) const = 0;
	virtual		void						setBreakForce(NxReal linearForce, NxReal angularForce) = 0;
//@	virtual		void						setBreakable(NxReal maxLForce, NxReal maxAForce) = 0;
//@	virtual		void						getBreakable(NxReal & l, NxReal & a) = 0;
	virtual		void						breakJoint() = 0;
//@	virtual		bool						isBroken()	= 0;
	virtual		NxJointState				getState() = 0;          /* QUERY(djs) : part of the brokenness API? */

	virtual		NxReal						getSolverExtrapolationFactor() const = 0;
	virtual		void						setSolverExtrapolationFactor(NxReal solverExtrapolationFactor) = 0;

	virtual		bool						getUseAccelerationSpring() const = 0;
	virtual		void						setUseAccelerationSpring(bool b) = 0;

//@	virtual		void						setBodies(NvBody * b1, NvBody * b2) = 0;
	virtual		NvBody *					getBody(NxU32 i) = 0;

	virtual		NxJointType					getType()		const = 0;

	virtual		void						setPublicFlags(NxU32 f)	= 0;
	virtual		NxU32						getPublicFlags() const = 0;

	virtual		void						setProjectionMode(NxJointProjectionMode pm) = 0;
	virtual		NxJointProjectionMode		getProjectionMode() const = 0;

	/* QUERY(djs): what do we need this for?*/
	virtual		NvJoint*					getNextInScene() = 0;				//scene's joint linked list
//@	virtual		void						clearFlag(NxU32 flag) = 0;			/* QUERY(djs): what flag if not a public flag? Also clashes with D6's 'setFlags' */


	virtual		void						setLimitPoint(const NxVec3 & point, bool pointIsOnBody2 = true) = 0;
	virtual		bool						getLimitPoint(NxVec3 & worldLimitPoint) = 0;
	virtual		bool						addLimitPlane(const NxVec3 & normal, const NxVec3 & pointInPlane, NxReal restitution) = 0;
	virtual		void						purgeLimitPlanes() = 0;
	virtual		void						resetLimitPlaneIterator() = 0;
	virtual		bool						hasMoreLimitPlanes() = 0;
	virtual		bool						getNextLimitPlane(NxVec3 & planeNormal, NxReal & planeD, NxReal * restitution) = 0;

	//methods for specific joint types:
	/* COMMENT(djs): it would be better to prefix all these methods with the joint name, i.e. DistanceSetMax, PulleySetRatio etc */

	//methods for Distance joint only:		//also implements get/setSpring() listed under Revolute.
	virtual		void						setMaxDistance(NxReal md) = 0;
	virtual		void						setMinDistance(NxReal md) = 0;
	
	virtual		void						setDistanceFlags(NxU32 df) = 0;

	virtual		NxReal						getMaxDistance() const = 0;
	virtual		NxReal						getMinDistance() const = 0;
	
	virtual		NxU32						getDistanceFlags() const = 0;

	//methods for Pulley joint only:	//also implements get/setMotor of revolute:
	virtual		void						setPulley(NxU32 i, const NxVec3 & p)	 = 0;
	virtual		void						setTargetDistance(NxReal td)			 = 0;
	virtual		void						setStiffness(NxReal st)					 = 0;
	virtual		void						setRatio(NxReal r)						 = 0;

	virtual		NxVec3 						getPulley(NxU32 i) const				 = 0;//{ NX_ASSERT(0); return NxVec3(); }
	virtual		NxReal						getTargetDistance() const				 = 0;//{ NX_ASSERT(0); return 0; }
	virtual		NxReal						getStiffness() const					 = 0;
	virtual		NxReal						getRatio() const						 = 0;

	//methods for revolute only:	(could now be subsumed by D6 Joint!)
	virtual		void						setLimits(const NxJointLimitPairDesc &)  = 0;
	virtual		bool						getLimits(NxJointLimitPairDesc &)		 = 0;
	virtual		void						setMotor(const NxMotorDesc &)			 = 0;
	virtual		bool						getMotor(NxMotorDesc &) const			 = 0;
	virtual		void						setSpring(const NxSpringDesc &)			 = 0;
	virtual		bool						getSpring(NxSpringDesc &) const			 = 0;
	virtual		void						setProjection(NxReal dist, NxReal angle) = 0;
	virtual		void						getProjection(NxReal & dist, NxReal & angle) const  = 0;
	virtual		NxReal						getAngleExternal() const  = 0;		//TODO: try to pull this out into the NP classes!!  See implementation.

	//methods for sphere joint only:	(could now be subsumed by D6 Joint!)
	virtual		void						setSphereTwistLimit(const NxJointLimitPairDesc & l)  = 0;
	virtual		NxJointLimitPairDesc		getSphereTwistLimit() const   = 0;//{ NX_ASSERT(0); return NxJointLimitPairDesc(); };
	virtual		void						setSwingLimit(const NxJointLimitDesc & l)  = 0;
	virtual		NxJointLimitDesc	 		getSwingLimit() const   = 0;//{ NX_ASSERT(0); return NxJointLimitDesc(); }
	virtual		void						setFlags(NxU32 f)  = 0;	//also used by pulley and revolute.
	virtual		NxU32						getFlags() const  = 0;

	virtual		void						setTwistSpring(const NxSpringDesc & s)  = 0;
	virtual		void						setSwingSpring(const NxSpringDesc & s)  = 0;
	virtual		void						setJointSpring(const NxSpringDesc & s)  = 0;
	virtual		NxSpringDesc				getTwistSpring() const  = 0;
	virtual		NxSpringDesc  				getSwingSpring() const  = 0;
	virtual		NxSpringDesc  				getJointSpring() const  = 0;
	virtual		void				 		setSwingAxis(const NxVec3 & s)  = 0;
	virtual		NxVec3 		 				getSwingAxis() const   = 0;

	virtual		void						setProjectionDistance(NxReal dist)  = 0;
	virtual		NxReal						getProjectionDistance() const  = 0;


	//methods for D6 Joint only:
//@	virtual		void						updateDerived()	 = 0;
// According to Matthias The D6 joint should always use loadFromDesc as individual sets are inefficient due to the heavy weight updateDerived().
//@	virtual		void						setLinMotion(NxU32 index, const NxD6JointMotion & value)  = 0;
	virtual		NxD6JointMotion 			getLinMotion(NxU32 index) const  = 0;
//@	virtual		void						setAngMotion(NxU32 index, const NxD6JointMotion & value)  = 0;
	virtual		NxD6JointMotion 			getAngMotion(NxU32 index) const  = 0;

//@	virtual		void						setLinearLimit(const NxJointLimitSoftDesc & l)  = 0;
//@	virtual		void						setSwing1Limit(const NxJointLimitSoftDesc & l)  = 0;
//@	virtual		void						setSwing2Limit(const NxJointLimitSoftDesc & l)  = 0;
//@	virtual		void						setTwistLimit(const NxJointLimitSoftPairDesc & l)  = 0;

	virtual		NxJointLimitSoftDesc 		getLinearLimit() const  = 0;
	virtual		NxJointLimitSoftDesc 		getSwing1Limit() const  = 0;
	virtual		NxJointLimitSoftDesc 		getSwing2Limit() const  = 0;
	virtual		NxJointLimitSoftPairDesc	getTwistLimit()  const  = 0;

//@	virtual		void						setLinDrive(NxU32 index, const NxJointDriveDesc & value)  = 0;
	virtual		NxJointDriveDesc			getLinDrive(NxU32 index) const  = 0;

//@	virtual		void						setSwingDrive(const NxJointDriveDesc & value)  = 0;
	virtual		NxJointDriveDesc			getSwingDrive() const  = 0;
//@	virtual		void						setTwistDrive(const NxJointDriveDesc & value)  = 0;
	virtual		NxJointDriveDesc			getTwistDrive() const  = 0;
//@	virtual		void						setSlerpDrive(const NxJointDriveDesc & value)  = 0;
	virtual		NxJointDriveDesc			getSlerpDrive() const  = 0;


//@	virtual		void						setDrivePQ(const NxVec3 & dp, const NxQuat & dq)  = 0;
	virtual		void						getDrivePQ(NxVec3 & dp, NxQuat & dq) const  = 0;

//@	virtual		void						setDriveVel(const NxVec3 & lv, const NxVec3 & av)  = 0;
	virtual		void						getDriveVel(NxVec3 & lv, NxVec3 & av) const	  = 0;

//@	virtual		void						setProjectionParams(NxReal dist, NxReal angle)	  = 0;
	virtual		void						getProjectionParams(NxReal & dist, NxReal & angle) const   = 0;
	virtual		void						setDriveP(const NxVec3 &position)  = 0;
	virtual		void						setDriveQ(const NxQuat &orientation)  = 0; 
	virtual		void						setDriveLinearVelocity(const NxVec3 &linVel)  = 0;
	virtual		void						setDriveAngularVelocity(const NxVec3 &angVel)  = 0;
//@	virtual		void						setGearRatio(const NxReal ratio)  = 0;
	virtual		NxReal						getGearRatio() const  = 0;

	protected:
		~NvJoint() {}				//please use release() instead!
	};

#endif
