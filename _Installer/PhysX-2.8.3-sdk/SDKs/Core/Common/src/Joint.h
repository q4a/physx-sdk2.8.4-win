#ifndef NX_PHYSICS_JOINT
#define NX_PHYSICS_JOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "NvJoint.h"
#include "NxBitField.h"
#include "NxUserNotify.h"
#include "Body.h"
//#define SERIOUS_SOLVE
#ifdef SERIOUS_SOLVE
#include "tnt/tnt.h"
#include "tnt/vec.h"
#include "tnt/cmat.h"
#endif

//#define USE_PROXY
#ifdef USE_PROXY
#define SIMULT_SOLVE_BALL
#endif

class Scene;
class NpJoint;
class Joint;

class JointInteraction;
class RevoluteJoint;
class PointInPlaneJoint;
class PointOnLineJoint;
class PrismaticJoint;
class CylindricalJoint;
class SphericalJoint;
class D6Joint;
class NvScene;

//namespace NxRigidBody
//	{


class Joint : public NxFoundation::NxAllocateable, public NvJoint /*: public Constraint*//*, public NxJoint*/ // : public NxConstraint
	{
	public:
	//actor space joint frames of the joint's actor 0 or 1.
	virtual		NxVec3						getFrameP(NxU32 i)				const;
	virtual		NxQuat						getFrameQ(NxU32 i)				const;
	virtual		void						setFrameP(NxU32 i, const NxVec3 &)	 ;
	virtual		void						setFrameQ(NxU32 i, const NxQuat &)	 ;
	virtual		void						setFramePQ(NxU32 i, const NxVec3 &, const NxQuat &);

	virtual		void						loadFromDesc(const NvJointDesc &);

	//default implementations of methods for specific joint types:
	//methods for Distance joint only:		//also implements get/setSpring() listed under Revolute.
	virtual		void						setMaxDistance(NxReal md) { NX_ASSERT(0); }
	virtual		void						setMinDistance(NxReal md) { NX_ASSERT(0); }
	
	virtual		void						setDistanceFlags(NxU32 df) { NX_ASSERT(0); }

	virtual		NxReal						getMaxDistance() const { NX_ASSERT(0); return 0; }
	virtual		NxReal						getMinDistance() const { NX_ASSERT(0); return 0; }
	
	virtual		NxU32						getDistanceFlags() const { NX_ASSERT(0); return 0; }
	virtual		void						getBreakForce(NxReal & linearForce, NxReal & angularForce) const;
	virtual		void						setBreakForce(NxReal linearForce, NxReal angularForce);

	virtual		NxReal						getSolverExtrapolationFactor() const;
	virtual		void						setSolverExtrapolationFactor(NxReal sef);

	virtual		bool						getUseAccelerationSpring() const;
	virtual		void						setUseAccelerationSpring(bool b);

	//methods for Pulley joint only:	//also implements get/setMotor of revolute:
	virtual		void						setPulley(NxU32 i, const NxVec3 & p)	 { NX_ASSERT(0); }
	virtual		void						setTargetDistance(NxReal td)			 { NX_ASSERT(0); }
	virtual		void						setStiffness(NxReal st)					 { NX_ASSERT(0); }
	virtual		void						setRatio(NxReal r)						 { NX_ASSERT(0); }

	virtual		NxVec3 						getPulley(NxU32 i) const				 { NX_ASSERT(0); return NxVec3(); }
	virtual		NxReal						getTargetDistance() const				 { NX_ASSERT(0); return 0; }
	virtual		NxReal						getStiffness() const					 { NX_ASSERT(0); return 0; }
	virtual		NxReal						getRatio() const						 { NX_ASSERT(0); return 0; }

	//methods for revolute only:	(could now be subsumed by D6 Joint!)
	virtual		void						setLimits(const NxJointLimitPairDesc &)  { NX_ASSERT(0); }
	virtual		bool						getLimits(NxJointLimitPairDesc &)		 { NX_ASSERT(0); return false; }
	virtual		void						setMotor(const NxMotorDesc &)			 { NX_ASSERT(0); }
	virtual		bool						getMotor(NxMotorDesc &) const			 { NX_ASSERT(0); return false; }
	virtual		void						setSpring(const NxSpringDesc &)			 { NX_ASSERT(0); }
	virtual		bool						getSpring(NxSpringDesc &) const			 { NX_ASSERT(0); return false; }
	virtual		void						setProjection(NxReal dist, NxReal angle) { NX_ASSERT(0); }
	virtual		void						getProjection(NxReal & dist, NxReal & angle) const  { NX_ASSERT(0); }
	virtual		NxReal						getAngleExternal() const  { NX_ASSERT(0); return 0; }		//TODO: try to pull this out into the NP classes!!  See implementation.

	//methods for sphere joint only:	(could now be subsumed by D6 Joint!)
	virtual		void						setSphereTwistLimit(const NxJointLimitPairDesc & l)  { NX_ASSERT(0); }
	virtual		NxJointLimitPairDesc		getSphereTwistLimit() const   { NX_ASSERT(0); return NxJointLimitPairDesc(); };
	virtual		void						setSwingLimit(const NxJointLimitDesc & l)  { NX_ASSERT(0); }
	virtual		NxJointLimitDesc	 		getSwingLimit() const   { NX_ASSERT(0); return NxJointLimitDesc(); }

	virtual		void						setTwistSpring(const NxSpringDesc & s)  { NX_ASSERT(0); }
	virtual		void						setSwingSpring(const NxSpringDesc & s)  { NX_ASSERT(0); }
	virtual		void						setJointSpring(const NxSpringDesc & s)  { NX_ASSERT(0); }
	virtual		NxSpringDesc				getTwistSpring() const  { NX_ASSERT(0); return NxSpringDesc(); }
	virtual		NxSpringDesc  				getSwingSpring() const  { NX_ASSERT(0); return NxSpringDesc(); }
	virtual		NxSpringDesc  				getJointSpring() const  { NX_ASSERT(0); return NxSpringDesc(); }
	virtual		void				 		setSwingAxis(const NxVec3 & s)  { NX_ASSERT(0); }
	virtual		NxVec3 		 				getSwingAxis() const   { NX_ASSERT(0); return NxVec3(); }

	virtual		void						setProjectionDistance(NxReal dist)  { NX_ASSERT(0); }
	virtual		NxReal						getProjectionDistance() const  { NX_ASSERT(0); return 0; }


	//methods for D6 Joint only:
	virtual		void						setLinMotion(NxU32 index, const NxD6JointMotion & value)  { NX_ASSERT(0); }
	virtual		NxD6JointMotion 			getLinMotion(NxU32 index) const  { NX_ASSERT(0); return NxD6JointMotion();  }
	virtual		void						setAngMotion(NxU32 index, const NxD6JointMotion & value)  { NX_ASSERT(0); }
	virtual		NxD6JointMotion 			getAngMotion(NxU32 index) const  { NX_ASSERT(0); return NxD6JointMotion(); }

	virtual		void						setLinearLimit(const NxJointLimitSoftDesc & l)  { NX_ASSERT(0); }
	virtual		void						setSwing1Limit(const NxJointLimitSoftDesc & l)  { NX_ASSERT(0); }
	virtual		void						setSwing2Limit(const NxJointLimitSoftDesc & l)  { NX_ASSERT(0); }
	virtual		void						setTwistLimit(const NxJointLimitSoftPairDesc & l)  { NX_ASSERT(0); }

	virtual		NxJointLimitSoftDesc 		getLinearLimit() const  { NX_ASSERT(0); return NxJointLimitSoftDesc(); }
	virtual		NxJointLimitSoftDesc 		getSwing1Limit() const  { NX_ASSERT(0); return NxJointLimitSoftDesc(); }
	virtual		NxJointLimitSoftDesc 		getSwing2Limit() const  { NX_ASSERT(0); return NxJointLimitSoftDesc(); }
	virtual		NxJointLimitSoftPairDesc	getTwistLimit()  const  { NX_ASSERT(0); return NxJointLimitSoftPairDesc(); }

	virtual		void						setLinDrive(NxU32 index, const NxJointDriveDesc & value)  { NX_ASSERT(0); }
	virtual		NxJointDriveDesc			getLinDrive(NxU32 index) const  { NX_ASSERT(0); return NxJointDriveDesc(); }

	virtual		void						setSwingDrive(const NxJointDriveDesc & value)  { NX_ASSERT(0); }
	virtual		NxJointDriveDesc			getSwingDrive() const  { NX_ASSERT(0); return NxJointDriveDesc(); }
	virtual		void						setTwistDrive(const NxJointDriveDesc & value)  { NX_ASSERT(0); }
	virtual		NxJointDriveDesc			getTwistDrive() const  { NX_ASSERT(0); return NxJointDriveDesc(); }
	virtual		void						setSlerpDrive(const NxJointDriveDesc & value)  { NX_ASSERT(0); }
	virtual		NxJointDriveDesc			getSlerpDrive() const  { NX_ASSERT(0); return NxJointDriveDesc(); }

	virtual		void						setFlags(NxU32 f)  { NX_ASSERT(0); }
	virtual		NxU32						getFlags() const  { NX_ASSERT(0); return 0; }

	virtual		void						setDrivePQ(const NxVec3 & dp, const NxQuat & dq)  { NX_ASSERT(0); }
	virtual		void						getDrivePQ(NxVec3 & dp, NxQuat & dq) const  { NX_ASSERT(0); }

	virtual		void						setDriveVel(const NxVec3 & lv, const NxVec3 & av)  { NX_ASSERT(0); }
	virtual		void						getDriveVel(NxVec3 & lv, NxVec3 & av) const	  { NX_ASSERT(0); }

	virtual		void						setProjectionParams(NxReal dist, NxReal angle)	  { NX_ASSERT(0); }
	virtual		void						getProjectionParams(NxReal & dist, NxReal & angle) const   { NX_ASSERT(0); }
	virtual		void						setDriveP(const NxVec3 &position)  { NX_ASSERT(0); }
	virtual		void						setDriveQ(const NxQuat &orientation)  { NX_ASSERT(0); } 
	virtual		void						setDriveLinearVelocity(const NxVec3 &linVel)  { NX_ASSERT(0); }
	virtual		void						setDriveAngularVelocity(const NxVec3 &angVel)  { NX_ASSERT(0); }
	virtual		void						setGearRatio(const NxReal ratio)  { NX_ASSERT(0); }
	virtual		NxReal						getGearRatio() const  { NX_ASSERT(0); return 0; }




				void			getGlobalAnchor(NxVec3&) const;
				void			getGlobalAxis(NxVec3&) const;

				NxJointState	getState();

				void			setLimitPoint(const NxVec3 & point, bool pointIsOnBody2 = true);
				bool			getLimitPoint(NxVec3 & worldLimitPoint);
				bool			addLimitPlane(const NxVec3 & normal, const NxVec3 & pointInPlane, NxReal restitution);
				void			purgeLimitPlanes();
				void			resetLimitPlaneIterator();
				bool			hasMoreLimitPlanes();
				bool			getNextLimitPlane(NxVec3 & planeNormal, NxReal & planeD, NxReal * restitution);
				NxJointType				getType()		const;

				NvBody *		getBody(NxU32 i);
				Body *			getBodyFast(NxU32 i) const { return bodies[i]; }
	NX_INLINE	void			getRotatedPointsDistance(const NxVec3& ra, const NxVec3& rb, NxVec3& out)  const;
				void			getLocalPointsDistance(const NxVec3 & la, const NxVec3 & lb, NxVec3 & out) const;



				void*					is(NxJointType)	/*const*/;
	NX_INLINE	NvJoint*		getNextInScene()		{ return next; }

	/**
	attempts to perform an downcast to the type returned. Returns 0 if this object is not of the appropriate type.
	*/
	NX_INLINE RevoluteJoint*		isRevoluteJoint()		{ return (RevoluteJoint*)		is(NX_JOINT_REVOLUTE);		}

	/**
	attempts to perform an downcast to the type returned. Returns 0 if this object is not of the appropriate type.
	*/
	NX_INLINE PointInPlaneJoint*	isPointInPlaneJoint()	{ return (PointInPlaneJoint*)	is(NX_JOINT_POINT_IN_PLANE);}

	/**
	attempts to perform an downcast to the type returned. Returns 0 if this object is not of the appropriate type.
	*/
	NX_INLINE PointOnLineJoint*		isPointOnLineJoint()	{ return (PointOnLineJoint*)	is(NX_JOINT_POINT_ON_LINE);	}

	/**
	attempts to perform an downcast to the type returned. Returns 0 if this object is not of the appropriate type.
	*/
	NX_INLINE PrismaticJoint*		isPrismaticJoint()		{ return (PrismaticJoint*)		is(NX_JOINT_PRISMATIC);		}

	/**
	attempts to perform an downcast to the type returned. Returns 0 if this object is not of the appropriate type.
	*/
	NX_INLINE CylindricalJoint*		isCylindricalJoint()	{ return (CylindricalJoint*)	is(NX_JOINT_CYLINDRICAL);	}

	/**
	attempts to perform an downcast to the type returned. Returns 0 if this object is not of the appropriate type.
	*/
	NX_INLINE SphericalJoint*		isSphericalJoint()		{ return (SphericalJoint*)		is(NX_JOINT_SPHERICAL);		}

	/**
	attempts to perform an downcast to the type returned. Returns 0 if this object is not of the appropriate type.
	*/
	NX_INLINE D6Joint*				isD6Joint()				{ return (D6Joint*)				is(NX_JOINT_D6);		}

//internal:

//	virtual		void					event(NxU32 e, NxFoundation::Observable & o);
	//  OBS! The user is now reported the appliedVForce not largeForce (potentialSumVF in solver)
	void					checkMaxForceExceeded();
	void					updateRelatedSIPs();

	//group access:
	enum Shifts		//the first bit of various things in the flag word.
		{
		JS_STATE = 3,
		JS_METHOD = 6
		};

	enum Masks
		{
		JM_STATE = (NX_JS_UNBOUND | NX_JS_SIMULATING | NX_JS_BROKEN) << JS_STATE,
		};



	struct LimitPlane : public NxFoundation::NxAllocateable
		{
		NxVec3 normal;
		NxReal d;			// the 'd' coeff of the plane equation.
		NxReal restitution;
		PxdConstraint constraint;
		LimitPlane * next;
		};

										Joint(const NvJointDesc & desc);
				virtual					~Joint();
				void					setBodies(NvBody * b1, NvBody * b2);

#ifdef SERIOUS_SOLVE
	virtual void solve(NxReal dt);							//called by proxy axis constraint for special constraints that can't be expressed as a set of axis constraints.
#endif
	virtual		void					processToAxisConstraints(NxReal dt);
	virtual		void					visualize(NxFoundation::DebugRenderable &);
	virtual		void					processLimitsToAxisConstraints(NxReal dt);
	virtual		void					projectPose(Body * childBody);
	NX_INLINE	virtual		void		comShiftUpdate()	{ disableAxCons(); }			//called when a COM shift is detected so joints can do custom logic in response.

	virtual		void					disableAxCons();

				NxReal					isPlaneSatisfied(LimitPlane & p, NxVec3 & worldPoint, NxVec3 & destWorldNormal, NxReal  & destWorldPlaneD);
				void					bodyReleasedEvent(Body & b);
				void					breakJoint();
	NX_INLINE	bool					prefersReducedMode();

	NX_INLINE	bool					getFlag(unsigned flag);
	NX_INLINE	void					setFlag(unsigned flag);
	NX_INLINE	void					clearFlag(unsigned flag);
	NX_INLINE	bool					isBroken()		{ return (flags.getField(JS_STATE,  JM_STATE) == NX_JS_BROKEN); }
	NX_INLINE	void					setPublicFlags(NxU32 f)	
														{
														//convert public flags to internal flags: AM: TODO: clean this up!
														flags.setFlagMask(JF_COLLISION_ENABLED, f & NX_JF_COLLISION_ENABLED);
														flags.setFlagMask(JF_VISUALIZATION, f & NX_JF_VISUALIZATION);
														}
	NX_INLINE	NxU32					getPublicFlags() const
														{
														//convert internal flags to public flags: AM: TODO: clean this up!
														NxU32 out = 0;
														if (flags.getFlagMask(JF_COLLISION_ENABLED))
															out |= NX_JF_COLLISION_ENABLED;
														else
															out &= ~NX_JF_COLLISION_ENABLED;
														if (flags.getFlagMask(JF_VISUALIZATION))
															out |= NX_JF_VISUALIZATION;
														else
															out &= ~NX_JF_VISUALIZATION;
														return out;
														}


	NX_INLINE	bool					checkBodySpaceFrames() const;			//returns true if no update was needed.	//AM: TODO: remove this!
				void					updateBodySpaceFrame(unsigned b) const;
	NX_INLINE	const NxVec3 &			getLocalNormalFast(unsigned b) const;
	NX_INLINE	const NxVec3 &			getLocalBinormalFast(unsigned b) const;
	NX_INLINE	const NxVec3 &			getLocalAxisFast(unsigned b) const;
	NX_INLINE	const NxVec3 &			getLocalAnchorFast(unsigned b) const;
	NX_INLINE	const NxQuat &			getLocalQuatFast(unsigned b) const;

	//NX_INLINE	void					setScene(Scene*);
	//new:
				void					setScene(NvScene*);
				NvScene*				getScene();

	NX_INLINE	void					setNpJoint(NpJoint* npJ){ mNpJoint = npJ; }
	NX_INLINE	NpJoint*				getNpJoint() { return mNpJoint; }

	NX_INLINE	bool					isUnsolvable();

	NX_INLINE	void					setProjectionMode(NxJointProjectionMode pm);
	NX_INLINE	NxJointProjectionMode	getProjectionMode() const { return projectionMode; }

	NX_INLINE	const NxVec3&			getAnchorPointAS(NxU32 i) const {return anchorPointAS[i]; }
	NX_INLINE	const NxQuat&			getQuatAS(NxU32 i) const { return quatAS[i]; }

	// Small helper...
	NX_INLINE	NxQuat					getLocalD6Quat(int i);

/*
	NX_INLINE	const NxVec3&			getNormalAS(NxU32 i) const				{ return normalAS[i]; }
	NX_INLINE	const NxVec3&			getBinormalAS(NxU32 i) const			{return binormalAS[i]; }
	NX_INLINE	const NxVec3&			getAxisAS(NxU32 i) const			{return axisAS[i];}

				void					setNormalAS(NxU32 i, const NxVec3 &v);
				void					setBinormalAS(NxU32 i, const NxVec3 &v);
				void					setAxisAS(NxU32 i, const NxVec3 &v);
				void					setAnchorPointAS(NxU32 i, const NxVec3 &v);
				void					setQuatAS(NxU32 i, const NxQuat &v);
*/
				Joint*					next;		//next pointer for joint linked list in Scene
				NxVec3					limitPoint;	//in space of the body its attached to.
				LimitPlane*				limitPlanes;

	//note: it may come to pass that both bodies are zero in case they are released by the user.
	//in this case this joint will still exist in the scene, but should not do anything.
				Body*					pointBody;	//temp rename so we can filter xform usage.
				Body*					planeBody;

				NxBitField32			flags;
//				NxReal					damping;
	static		LimitPlane*				userLimitPlaneIterator;
				Scene*					scene;

	/*-------------------------------*\
	| New with group storage:
	| Each Constraint is stored in a linked 
	| list (Body::contactList) rooted 
	| in the body with the smaller groupGetGuid().
	\*-------------------------------*/
				Joint*					nextInBodyJointList;
				Joint*					nextInBodyReverseJointList;
	protected:
				NxReal					maxLinBreakForce;
				NxReal					maxAngBreakForce;
				NxJointProjectionMode	projectionMode;		//this used to be part of both SphericalJoint and RevoluteJoint, but its in the process of being moved down here for all joints.
				NxReal					solverExtrapolationFactor;
				bool					useAccelerationSpring;
				// proxy
				NpJoint*				mNpJoint;
				NxU32					nbConstraints;		// These are set by the respective subclasses so that joint can traverse them without virtual calls
				NxU32					nbD6Constraints;	// Number of constraints that are D6 joint constraints. These are at the beginning of the constraint array
				PxdHandle*				firstConstraint;	// PxdHandle is used instead of PxdConstraint because of the D6 joint using the PxdD6Joint handle instead of PxdHandle


				//basis in body[i]'s space (cols of joint2body[i]):
	private:	//temp

				//now that API is quat based the below 3 vars can be removed:
/*
				NxVec3					normalAS[2];		//X a direction orthogonal to axis
				NxVec3					binormalAS[2];		//Y normal1^axis1
				NxVec3					axisAS[2];			//Z joint axis in actor[0]'s space
*/
				NxVec3					anchorPointAS[2];	//W in actor[0]'s space.
				NxQuat					quatAS[2];			//quat of actor space constraint frame.
//				Actor *					actor[2];			//the actor ptr, useful if the body ptr is null .... at least one should be non-null unless broken. We choose to support NULL actors too cause its convenient.

		//body space variables (mutable because this is a lazy update cache of i.e. bodies[i]->getBody2Actor().M % normalAS[i]:
				mutable NxVec3			normal[2];			//X a direction orthogonal to axis
				mutable NxVec3			binormal[2];		//Y normal1^axis1
				mutable NxVec3			axis[2];			//Z joint axis in bodies[0]'s space
				mutable NxVec3			anchorPoint[2];		//W in bodies[0]'s space.
				mutable NxQuat			quat[2];
				mutable unsigned		comShiftCounters[2];//last seen com shift counters for the two actors. If this gets mismatched, the body space vectors need to be updated.
		//actor space variables:

				NxVec3					appliedForce;	//becomes valid after a dynamics step

private:

				NxJointType				externalType;		//we also have a different type enum in our baseclass that includes contacts.


				JointInteraction	   *interaction;
public:
	NX_INLINE	JointInteraction	   &getPxInteraction() { return *interaction; }
				CoreActor			   &getCoreActor(int i);

	NX_INLINE	Body*					anybody();	//may return NULL, see above!
	NX_INLINE	Body*					otherBody(Body *);
	NX_INLINE	bool					involvesBody(Body *);
	NX_INLINE	NxReal					getRelativeAngularVelocity(const NxVec3& axis);

				Body*					bodies[2];

#ifdef SERIOUS_SOLVE
	protected:
	void massSetup();
	//all of these vars are only used during the solver.
	TNT::Matrix<NxReal> solve_inverseResponseLU;//up to 6x6
	TNT::Vector<TNT::Subscript> solve_inverseResponseIdx;//up to 6x1

	TNT::Vector<NxReal> solve_bias;//up to 6x1
	TNT::Matrix<NxReal> solve_jacobian;//up to 12x6
#endif
	};



NX_INLINE	bool	Joint::checkBodySpaceFrames() const
	{
	NxU32 updates = 0;
	for (unsigned i=0; i<2; i++)
		{
		if (bodies[i])
			{
			if (bodies[i]->getComShiftCounter() != comShiftCounters[i])
				{
				updateBodySpaceFrame(i);
				updates++;
				}
			}
		}
	if (updates)
		{
		//stupid const propagation has to be restrained else we have to make all members mutable.
		const_cast<Joint*>(this)->comShiftUpdate();
		return false;
		}
	else
		return true;
	}

NX_INLINE	const NxVec3 &Joint::getLocalNormalFast(unsigned b) const
	{
	NX_ASSERT(checkBodySpaceFrames());	//if this fails, just call checkBodySpaceFrames() in the caller without an assert.
	return normal[b];
	}

NX_INLINE	const NxVec3 &Joint::getLocalBinormalFast(unsigned b) const
	{
	NX_ASSERT(checkBodySpaceFrames());	//if this fails, just call checkBodySpaceFrames() in the caller without an assert.
	return binormal[b];
	}

NX_INLINE	const NxVec3 &Joint::getLocalAxisFast(unsigned b) const
	{
	NX_ASSERT(checkBodySpaceFrames());	//if this fails, just call checkBodySpaceFrames() in the caller without an assert.
	return axis[b];
	}

NX_INLINE	const NxVec3 &Joint::getLocalAnchorFast(unsigned b) const
	{
	NX_ASSERT(checkBodySpaceFrames());	//if this fails, just call checkBodySpaceFrames() in the caller without an assert.
	return anchorPoint[b];
	}

NX_INLINE	const NxQuat &Joint::getLocalQuatFast(unsigned b) const
	{
	NX_ASSERT(checkBodySpaceFrames());	//if this fails, just call checkBodySpaceFrames() in the caller without an assert.
	return quat[b];
	}


NX_INLINE bool Joint::getFlag(unsigned flag)
	{
	//return (flags & flag) != 0;
	return flags.getFlagMask(flag) != 0;
	}

NX_INLINE void Joint::setFlag(unsigned flag)
	{
	//flags |= flag;
	flags.raiseFlagMask(flag);
	}

NX_INLINE void Joint::clearFlag(unsigned flag)
	{
	//flags &= ~flag;
	flags.lowerFlagMask(flag);
	}
/*
NX_INLINE void Joint::setScene(Scene* s)
	{
	scene = s;
	}
*/
#ifdef CORELIB	//temp during refactor
NX_INLINE bool Joint::isUnsolvable()
	{
	return (!bodies[0] || (bodies[0]->getPublicFlagsFast() & NX_BF_KINEMATIC))	&&	(!bodies[1] || (bodies[1]->getPublicFlags() & NX_BF_KINEMATIC));
	}
#endif
//void planeNormal2Tangents(const NxVec3 & n, NxVec3 & t1, NxVec3 & t2);	//messy global, put in some math lib!

NX_INLINE void Joint::setProjectionMode(NxJointProjectionMode pm)
{
	if (projectionMode == NX_JPM_NONE && pm != NX_JPM_NONE)
	{
		// check if this joint has already gone through Scene::prepareJoints()
		// if the lists do not exist, then all of the code below will be executed
		// in prepareJoints at the beginning of the next substep.
		Body* root = anybody()->jointGroupGetRoot();
		if (root->hasJointLists())
		{
			root->raiseFlag(Body::BF_NEED_JOINT_ARTICULATION);
			Articulation::processJointGroup(root);
		}
	}
	projectionMode = pm;
}

NX_INLINE void projectOnLine(const NxVec3& normal, const NxVec3& pointOfLine, const NxVec3& point, NxVec3& result)
	{
	//normal should be normalized.
	result = normal * (point - pointOfLine).dot(normal) + pointOfLine;
	}

NX_INLINE void Joint::getRotatedPointsDistance(const NxVec3& ra, const NxVec3& rb, NxVec3& poseErr)	const
	{
	/*	
	Leave this comment -- its ezer to understand than below optimized code.
	if (bodies[0])
		{
		poseErr = bodies[0]->local2world.t + ra;	//world space attachment point of bodies[0]
		}
	else
		poseErr = ra;
	
	if (bodies[1])
		{
		poseErr -= bodies[1]->local2world.t + rb;	//world space attachment point of bodies[1]
		}
	else
		poseErr -= rb;
	*/
	poseErr = ra - rb;
	if(bodies[0])
	{
		poseErr += bodies[0]->getBody2WorldLL().p;	//world space attachment point of bodies[0]
	}
	if(bodies[1])	
	{
		poseErr -= bodies[1]->getBody2WorldLL().p;	//world space attachment point of bodies[1]
	}
	}
//}

Body * Joint::anybody()
	{
	if (bodies[0]) 
		return bodies[0];
	else 
		return bodies[1];
	}

Body * Joint::otherBody(Body * b)
	{
	return (b == bodies[0]) ? bodies[1] : bodies[0];
	}

bool Joint::involvesBody(Body *b)
	{
	return (b == bodies[0] || b == bodies[1]);
	}

NX_INLINE NxReal Joint::getRelativeAngularVelocity(const NxVec3& axis)
	{
		if (!bodies[1])
		{
			NxVec3 linVel0, angVel0;
			bodies[0]->getVelocityLL(linVel0, angVel0);
			return axis.dot(angVel0);
		}
		else if (!bodies[0])
		{
			NxVec3 linVel1, angVel1;
			bodies[1]->getVelocityLL(linVel1, angVel1);
			return -axis.dot(angVel1);
		}
		else
		{
			NxVec3 linVel0, angVel0;
			bodies[0]->getVelocityLL(linVel0, angVel0);
			NxVec3 linVel1, angVel1;
			bodies[1]->getVelocityLL(linVel1, angVel1);
			return axis.dot(angVel0 - angVel1);
		}
	}

	// Our own convention different from the SDK where the axis is the first axis, normal is second

NX_INLINE NxQuat Joint::getLocalD6Quat(int i) 
	{
	NxVec3 a = getLocalAxisFast(i);
	NxVec3 n = getLocalNormalFast(i);
	return !NxQuat(NxMat33(a, n, a^n));
	}

#endif
