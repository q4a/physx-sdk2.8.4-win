#ifndef NX_PHYSICS_BODY
#define NX_PHYSICS_BODY
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Physics.h"
#include "NvBody.h"
#include "NxBounds3.h"
#include "Observable.h"
#include "Articulation.h"
#include "SpatialVec.h"
#include "Utilities.h"
#include "RbActor.h"
#include "QP.h"

class Joint;
class ShapePairBase;
class NxCapsule;
class NxSphere;
class BodyContainer;
class NvScene;
class Shape;
class AxisConstraintPool;
class NxActor;
class Scene;


enum Events
	{
	RBE_BODY_RELEASED		= 256 + 0,
	};

struct Group						//TODO: move to its own header!
{
	NX_INLINE Body * getNext()	{ return next; }
	NX_INLINE NxU32 getSize()	{ return size; }

	//tree for union/find:
	Body *	parent;
	Body *	tail;	//only valid if this is root of group, points to LList tail body.

	NxU32	rank; //rank counter for union/find. Initially zero. Is number of hops from root to furthest leaf in tree. This is just a hint to create more balanced trees.
	//linked list for traversal:
	Body *	next;	//next in list, NULL at tail.

	//stuff that is only used for group root -- could move out to the 'groups' vector:
	NxU32	size;				//for the parent, number of bodies in the group.
};

/*
This class implements parts of the rigid body dynamics simulation in software.

An instance of this class is owned by dynamic actors.
*/
class Body : public NvBody, public RbActor, public NxFoundation::NxAllocateable
#ifdef OBSERVABLE_BODY
, public NxFoundation::Observable
#endif

	{
	public:
	////////////////////////Types & friends////////////////////////
	enum InternalFlag
		{
		//TODO: values for bits can now get reset
		//joint group bits
		BF_NEW_JOINT_GROUP			= 1 << 1,	//scp: joint group is new or changed, has to be re-analyzed. Only valid for group roots.
		BF_NEED_JOINT_ARTICULATION	= 1 << 2,   //scp: at least one member of the joint group use joint project so articulation structures must be created
		BF_REBUILD_CGROUP			= 1 << 3,	//sct: the group had a contact removed, so the contact group has to be rebuilt to find the eventual disconnect. Only valid for group roots.
		//flags used temporarily during group analysis:
		BF_DISCOVERED				= 1 << 4,	//sct: this graph node was already discovered in depth first traverse
		BF_VEL_IS_AVERAGED			= 1 << 5,	//sct: indicates that the body has been put through the solver this substep. (it had at least 1 constraint)
		BF_HAS_JOINTS_TO_WORLD		= 1 << 6,	//sct: currently only valid during articulation analysys
		//CCD:
		BF_CAN_DO_CCD				= 1 << 7,	//sct: has a skeleton on at least one of its shapes.
		BF_DO_LIN_CCD				= 1 << 8,	//sct: fast enough to do lin ccd - only valid if BF_CAN_DO_CCD is raised.
		BF_DO_ANG_CCD				= 1 << 9,	//sct: fast enough to do ang ccd - only valid if BF_CAN_DO_CCD is raised.
		BF_CCD_IS_STUCK				= 1 << 10,	//scp: a flag set by ccd when its stuck so the next time it will take a different approach
		BF_CCD_ROTLOCK				= 1 << 11,	//scp: set when ccd got stuck and is trying to get unstuck by locking the rotation.

		BF_INERTIA_DIRTY			= 1 << 12,	//scp: set when global inertia needs to be recalculated
		BF_SPECIAL_SHAPE_BROADPHASE	= 1 << 13,	//scp: set when the shape really needs its updateBroadphase run on pose updates
		BF_HAS_STATIC_TOUCH			= 1 << 14,	//scp: set when a body is part of an island with static contacts. Needed to be able to recalculate adaptive force if this changes
		BF_IS_IN_DIRTY_LIST			= 1 << 15,	//scp: set when the body is added to the scene's dirty list
		BF_IS_IN_SECOND_PASS_LIST	= 1 << 16,  //scp: set when the body is added to the second pass list of afterSolver
		BF_KINEMATIC_MOVED_LAST_FRAME = 1 << 17, //scp: set when the kinematic moved last frame
		BF_HAS_METAL_CLOTH			= 1 << 18,	//scp: set when the body has metal cloth

		BF_ON_DEATHROW				= 1 << 19,	// scp: set when the body is destroyed

		BF_RESTING_LL				= 1 << 20,	// scp: set if the LL atom is in resting state
		BF_WHEELSHAPE_ADAPTIVE_OFF	= 1 << 21	// flag to ignore adaptive force for wheel shapes.
		};

	struct KinematicTarget : public NxFoundation::NxAllocateable	//TODO: may want to optimize the allocation of this further.
		{
		enum { POS_VALID = 1, ROT_VALID = 2 };	//bits
		NxVec3			pos;			//The body will move to this pose over the superstep following this getting set.
		NxU32			valid;			//When the user set a kinematic target once, but not afterwards, we do not delete this object, but rather remove the valid bit.
		NxQuat			orient;
		};

	// Important: Make sure this struct matches with fetching PXD_ATOM_POSE_AND_VELOCITY and setting PXD_ATOM_VELOCITY.
	struct LLState
		{
		NxQP pose;
		NxVec3 linVelocity;
		NxVec3 angVelocity;
		bool llVelocityDirty;
		bool llPoseDirty;
		};


	friend class Articulation;	//this guy uses some members of ours that used to be public








	////////////////////////Construction, destruction & initialization////////////////////////

			Body(Scene &, const NvBodyDesc&);
			virtual	~Body();
	public:

	void	loadFromDesc(const NvBodyDesc&);
	bool	isValid() const { return atom != 0; }

	//Implementation of NxAllocatable
	void* operator new(size_t s, void* memory) { return memory; }







	////////////////////////NvBody implementation////////////////////////
	virtual void			release();

	/* actor coordinates */
	virtual NxVec3 			getActor2WorldP() const;
	virtual NxQuat 			getActor2WorldQ() const;
	virtual void			setActor2WorldP(const NxVec3& p);
	virtual void			setActor2WorldQ(const NxQuat& q);
	virtual void			setActor2WorldPQ(const NxVec3& p, const NxQuat & q);

	/* velocity */
	virtual NxVec3			getLinearVelocity() const;
	virtual void			setLinearVelocity(const NxVec3& v);
	virtual NxVec3			getAngularVelocity() const;
	virtual void			setAngularVelocity(const NxVec3& w);

	/* force application */
	virtual void			addSpatialForce(const NxVec3* force, const NxVec3* torque, NxForceMode);
	virtual void			addForceAtPos(const NxVec3& force, const NxVec3& pos, NxForceMode);

	/* wakeup API */
	virtual NxReal			getSleepTimer()	const;
	virtual void			setSleepTimer(NxReal);
	virtual	bool			isGroupSleeping() const;

	/* constants for the body */
	virtual NxReal			getInverseMass() const;
	virtual void			setInverseMass(NxReal);
	virtual NxVec3 			getInverseInertia() const;
	virtual void 			setInverseInertia(const NxVec3&);

	virtual NxVec3			getBody2ActorP() const;
	virtual NxQuat			getBody2ActorQ() const;
	virtual void			setBody2ActorP(const NxVec3& p);
	virtual void			setBody2ActorQ(const NxQuat& q);
	virtual void			setBody2ActorPQ(const NxVec3& p, const NxQuat& q);

	virtual NxReal			getLinearDamping() const;
	virtual void			setLinearDamping(NxReal v);

	virtual NxReal			getAngularDamping() const;
	virtual void			setAngularDamping(NxReal v);

	virtual NxU32			getPublicFlags() const;
	virtual void			setPublicFlags(NxU32 v);

	virtual NxReal			getSleepLinVelSq() const;
	virtual void			setSleepLinVelSq(NxReal v);

	virtual NxReal			getSleepAngVelSq() const;
	virtual void			setSleepAngVelSq(NxReal v);

	virtual	NxReal			getSleepEnergyThreshold() const;
	virtual	void			setSleepEnergyThreshold(NxReal v);

	virtual NxReal			getMaxAngVelSq() const;
	virtual void			setMaxAngVelSq(NxReal v);

	virtual NxU32			getSolverIterationCount() const;
	virtual void			setSolverIterationCount(NxU32 v);

	virtual NxReal			getContactReportThreshold() const;
	virtual void			setContactReportThreshold(NxReal threshold);

	virtual NxReal			getCCDMotionThreshold() const;
	virtual void			setCCDMotionThreshold(NxReal v);

	virtual void			setKinematic(bool on);
	virtual void			setKinematicTargetP(const NxVec3& t);
	virtual void			setKinematicTargetQ(const NxQuat& q);
	virtual void			setKinematicTargetPQ(const NxVec3& t, const NxQuat& q);

	virtual NxU32			getID() const;

	/* shape accessor */
	virtual NvShape*		getNvShape() const;
	virtual void			setActorDominanceGroup(NxDominanceGroup);	// Sends to RbActor



	////////////////////////PxActor implementation////////////////////////
	virtual void destroy();
	virtual bool canActivate() const { return true; }
	virtual bool canDeactivate() const { return !((getPublicFlagsFast() & NX_BF_KINEMATIC) && readFlag(BF_KINEMATIC_MOVED_LAST_FRAME)) && getWakeCounter() == 0.0f; }
	virtual void onActivate();
	virtual void onDeactivate();
	void onKinematicActivate();
	void onKinematicDeactivate();

	virtual void* getAppData() { return (void*)(NvBody*)this; }
	virtual void onAllInteractionsInactiveChange(bool allInactive);
	virtual bool isIslandGenerating() const { return !(getPublicFlagsFast() & NX_BF_KINEMATIC); }

	virtual void registerElement(PxElement* element);
	virtual void unregisterElement(PxElement* element);

	////////////////////////Update calls////////////////////////
	// Call these after the last relevant write that you do, either from API or internally!
	// note: To update the actor2world after a physics based change to body2world, just call actor->update().
	void updateOnActor2WorldChange();			// Just call this on a non-physics based orientation change. Recomputes body2world* transform, derInverseInertiaWorldSpace.
	void updateOnBody2ActorChange();			// Call this on a cmass shift that doesn't move the actor. Recomputes body2world* transform, derInverseInertiaWorldSpace.
	NX_INLINE void updateActorFromBody2ActorChange();		// This is called from Actor::updateActorOnBody2WorldChange(), after integration and projection. Plus from CCD pullbacks.
	void updateActorOnBody2WorldChange(bool updateOldPose = true); //updates body->actor2world, and then updates the shapes. Only call on dynamic actors!
	void updateBpLists();						// Updates the persistently added shapes to the Scene::bpShapes and BroadPhase::updateList.

	void sizeChangeNotify();
	void finalizeMotion(NxReal dt, NxReal oneOverDt);					//called after the solver to apply some changes to the post-solver velocities.
	void updateForces(NxReal dt, NxReal oneOverDt, bool updateGravity, bool useAdaptiveForce, bool resetForces);
	NX_INLINE void setForcesToDefaults();

	////////////////////////Various accessors////////////////////////
	NX_INLINE NxQP		getActor2World() const { return apiActor2World; } //try to avoid using this!

	NX_INLINE NxU32		getComShiftCounter() const { return inpComShiftCounter; }
	NX_INLINE void		setComShiftCounter(NxU32 v) { inpComShiftCounter = v; }

	NX_INLINE void		setBody2ActorPos(const NxVec3 &v) { setBody2ActorPos2(v); inpComShiftCounter ++; }
	NX_INLINE void		setBody2ActorOrient(const NxMat33 &m) { setBody2ActorOrient2(m);  inpComShiftCounter ++; }

			  NX_BOOL	getTemporalBounds(NxBounds3 & dest);	//returns 0 if not moving fast enough and temporal bounds are not needed.

	NX_INLINE void		clearBodyConstraintCounter() { sctBodyConstraints = 0; }
	NX_INLINE void		incrementBodyConstraintCounter() { sctBodyConstraints++; }
	NX_INLINE void		decrementBodyConstraintCounter() { NX_ASSERT(sctBodyConstraints>0); sctBodyConstraints--; }

	NX_INLINE const NxVec3&		getMotionVel()		const	{ return scpMotionVel; }
	NX_INLINE const NxVec3&		getMotionAngVel()	const	{ return scpMotionAngVel; }
	NX_INLINE NxReal			getImpactTime()		const	{ return sctImpactTime; }
	NX_INLINE NxReal			getCCDRadius()		const	{ return sctRadius; }

	NX_INLINE void		markAccelerationAsDirty() { sctAccDirtyCounter = 2; }

	NX_INLINE bool		readFlag(InternalFlag flag) const { return (scpInternalFlags & flag) != 0; }	//not getFlag to avoid name conflict with interface method
	NX_INLINE NX_BOOL	readIntFlag(InternalFlag flag) const { return (scpInternalFlags & flag); }	//slightly faster!
	NX_INLINE void		raiseFlag(InternalFlag flag) { scpInternalFlags |= flag; if (flag == BF_CAN_DO_CCD) updateBpLists(); }
	NX_INLINE void		clearFlag(InternalFlag flag) { scpInternalFlags &= ~flag; if (flag == BF_CAN_DO_CCD) updateBpLists(); }

	NX_INLINE const NxQP&	getBody2WorldOld() const { return body2WorldOld; }
	NX_INLINE const NxQP&	getActor2WorldNew() const { return sctActor2World; }

	NX_INLINE NxReal	getWakeCounter() const		{ return wakeCounter; }
	NX_INLINE void		setWakeCounter(NxReal v)	{ wakeCounter = v; }

	NX_INLINE const NxVec3&  getInverseInertiaBodySpace() const { return inpInverseInertiaBodySpace; }
	NX_INLINE void			 setInverseInertiaBodySpace(const NxVec3 &v);
	NX_INLINE const NxMat33& getGlobalInertiaTensorInverse() const;

	NX_INLINE const NxQP&	getBody2Actor() const { return inpBody2Actor; }
	NX_INLINE void			 setBody2ActorPos2(const NxVec3 &v) { inpBody2Actor.p = v; }
	NX_INLINE void			 setBody2ActorOrient2(const NxQuat &q) { inpBody2Actor.q = q; }

	NX_INLINE NxReal getInvMass() const { return inpInvMass;  }
	NX_INLINE void	 setInvMass(NxReal v);

	NX_INLINE const NxVec3&		getLinearVelModPerSec() const { return insVelMod.linearPerSec; }
	NX_INLINE		void		setLinearVelModPerSec(const NxVec3& v) { insVelMod.linearPerSec = v; }
	NX_INLINE		void		accumulateLinearVelModPerSec(const NxVec3 &v) { insVelMod.linearPerSec += v; }

	NX_INLINE const NxVec3&		getAngularVelModPerSec() const { return insVelMod.angularPerSec; }
	NX_INLINE		void		setAngularVelModPerSec   (const NxVec3 &v) { insVelMod.angularPerSec = v; }
	NX_INLINE		void		accumulateAngularVelModPerSec(const NxVec3 &v) { insVelMod.angularPerSec += v; }

	NX_INLINE const NxVec3&		getLinearVelModPerStep() const { return insVelMod.linearPerStep; }
	NX_INLINE		void		setLinearVelModPerStep    (const NxVec3 &v) { insVelMod.linearPerStep = v; }
	NX_INLINE		void		accumulateLinearVelModPerStep(const NxVec3 &v) { insVelMod.linearPerStep += v; }

	NX_INLINE const NxVec3&		getAngularVelModPerStep() const { return insVelMod.angularPerStep; }
	NX_INLINE		void		setAngularVelModPerStep    (const NxVec3 &v) { insVelMod.angularPerStep = v; }
	NX_INLINE		void		accumulateAngularVelModPerStep(const NxVec3 &v) { insVelMod.angularPerStep += v; }

	NX_INLINE PxdAtom&			getLowLevelAtom() { return atom; }

	NX_INLINE NxActor*			getOwningNxActor() const { return owningNxActor; }

	NX_INLINE NxU32				getPoseTimestamp() const { return poseTimeStamp; }

	NX_INLINE NxU32				getPublicFlagsFast() const { return inpPublicFlags; }






	////////////////////////Kinematics////////////////////////
	NX_INLINE const KinematicTarget* getKinematicTarget() const { return kinematicTarget; }
	NX_INLINE void					 setKinematicTargetToInvalid() { kinematicTarget->valid = 0; }
	NX_INLINE void					 createKinematicTarget() { if (!kinematicTarget) kinematicTarget = NX_NEW(KinematicTarget);}
	NX_INLINE void					 deleteKinematicTarget() { NX_DELETE_SINGLE(kinematicTarget); }
	NX_INLINE void					 setKinematicTargetPos(const NxVec3 &v);
	NX_INLINE void					 setKinematicTargetQuat(const NxQuat &q);
			  void					 calculateKinematicVelocity(NxReal dt, NxReal oneOverDt);









	////////////////////////Joints & articulations////////////////////////
	NX_INLINE Group&	getJointGroup() { return scpJointGroup;	}
	NX_INLINE Body*		jointGroupGetRoot();
	NX_INLINE Body*		jointGroupGetNext() { return scpJointGroup.getNext();	}
			  void		jointGroupResetInfo(bool wakeBodyUp=true);
			  void		jointGroupUnion(Body& other);
			  void		groupNotifyJoint(Joint& c);
			  void		processNewJointGroup(); //if this is a root then we try to create an articulation for projection.
			  void		invalidateJointGroup(Joint* jointDeleted, NxFoundation::NxArraySDK<Joint*>& jointbin); //called if this body gets deleted or a joint. In either case all attached joints but the deleted one are moved to joint bin.

			  void		jointProjMoveTo(const NxVec3& newPos);
			  void		jointProjMoveTo(const NxVec3& newPos, const NxQuat& newOrient);
	NX_INLINE void		projectPose() { if (scpArticulations) scpArticulations->projectPose(); }

	NX_INLINE void		purgeArticulation() { NX_DELETE_SINGLE(scpArticulations); }
	NX_INLINE void		setArticulations(Articulation* a) { scpArticulations = a; }
			  void		invalidateArticulation();
	NX_INLINE bool		hasArticulation() const { return scpArticulations != 0; }

	NX_INLINE bool		hasJointLists() const { return scpJointList || scpReverseJointList; }


			  void		addToJointList(Joint& c, Body* other);
			  NX_BOOL	isAttachedByJoint(const Body& other) const;



	////////////////////////CCD////////////////////////
			  NX_BOOL	ccdImpactLinear(NxReal linToi);
			  NX_BOOL	ccdImpactAngular(NxReal angToi);
			  void		ccdUpdate(NxReal dt, NxReal oneOverDt);
			  bool		ccdImpact(NxReal toi);
			  void		integrateOrientation(NxReal dt);


	////////////////////////Sleeping////////////////////////
	NX_INLINE NX_BOOL	isSleeping() const { return !isActive(); }
			  void		sleepCheck(NxReal dt);	//progresses unmoving bodies toward sleep.
			  void		wakeUp(NxReal wakeCounterValue=NX_SLEEP_INTERVAL);	//called by constraints when the user changes them.
	NX_INLINE void		zeroAPIVelocities() { apiVelocity.zero(); apiAngVelocity.zero(); }
	// When comparing timestamps we need to take into account that the stamps are wrapped early because of INVALID_ID, hence the mask
			  bool		isForcedToSleep() const;
			  bool		wasForcedToSleepLastFrame() const;


	////////////////////////Low Level Interface////////////////////////
	NX_INLINE void syncToLL();
	NX_INLINE void syncFromLL(); // Make sure to update actor2world after!
	NX_INLINE void swapBuffers();

	NX_INLINE void cacheState() const;
	NX_INLINE void cacheInertia() const;

	NX_INLINE void			setRestingLL(bool resting);
	NX_INLINE void			getVelocityLL(NxVec3& linVelLL, NxVec3& angVelLL) const;
	NX_INLINE const NxVec3& getLinearVelocityLL() const { return llState.linVelocity; }
	NX_INLINE const NxVec3& getAngularVelocityLL() const { return llState.angVelocity; }
			  void			setVelocityLL(const NxVec3 &linVel, const NxVec3 &angVel);
			  void			setLinearVelocityLL(const NxVec3 &linVel);
			  void			setAngularVelocityLL(const NxVec3 &angVel);

	NX_INLINE const NxQP&	getBody2WorldLL() const { return llState.pose; }
              void			setBody2WorldLL(const NxQuat& rot, const NxVec3& pos);
	NX_INLINE void			setBody2WorldPosLL(const NxVec3& pos);
	NX_INLINE void			setBody2WorldRotLL(const NxQuat& rot);


	////////////////////////Helpers////////////////////////

	NX_INLINE	void	localToGlobalSpace(const NxVec3& bodyPoint, NxVec3& worldDest)			const	{ worldDest = getBody2WorldLL().transform(bodyPoint); }
	NX_INLINE	void	globalToLocalSpace(const NxVec3& worldPoint, NxVec3& bodyDest)			const	{ bodyDest = getBody2WorldLL().invTransform(worldPoint);	}
	NX_INLINE	void	localToGlobalSpaceDirection(const NxVec3& bodyPoint, NxVec3& worldDest)	const	{ worldDest = getBody2WorldLL().q.rot(bodyPoint); }
	NX_INLINE	void	globalToLocalSpaceDirection(const NxVec3& worldPoint, NxVec3& bodyDest)	const	{ bodyDest = getBody2WorldLL().q.invRot(worldPoint); }

				NX_BOOL	integrateQuaternion(NxQuat & qInOut, NxReal dt);

	NX_INLINE	void	computeImpulseResponse(const NxVec3 & normal, const NxVec3 & r, SpatialVec & response);
	NX_INLINE	void	computePointVelocity(const NxVec3 & point, NxVec3 &result) const;
				








	////////////////////////Miscellaneous////////////////////////
				void	visualize(NxFoundation::DebugRenderable & dr);


	private:
		void updateRadius(const NxSphere * worldSphere);	//used to tell the body about the total extents of the shapes attached to it. Used for CCD stuff.
		void setDirty();

		// Functions to set the api buffer velocities without affecting the low level counterparts.
		// NOTE: Only use these functions if you really know what you do.
		NX_INLINE	void	setLinearVelocityApiOnly(const NxVec3& v) { apiVelocity = v; }
		NX_INLINE	void	setAngularVelocityApiOnly(const NxVec3& w) { apiAngVelocity = w; }

	///////////////////////////////////////////////////////
	////////////////////////MEMBERS////////////////////////
	///////////////////////////////////////////////////////



	////////////////////////Frequently used members////////////////////////
	// Kept close for cache coherency
	
	mutable LLState llState;

	PxdAtom		atom;

	NxQP		body2WorldOld;		// Not a derived quantity any more. LL simulates the body and result is the body transform.
	NxQP		inpBody2Actor;		// A vector that points from the actor's position to the body's center of mass.// GLOBALPOS
	NxQP		sctActor2World;		// Updated every substep
	NxU32		poseTimeStamp;		// Timestamp for the actor2world pose

	Scene*		scene;	// Owner scene.
	NxU32	inpPublicFlags;					// Flags of type NxBodyFlag, defined in NxBodyDesc. Involves locking DOFs, setting visualization, and kinematic mode.
	mutable NxU32 scpInternalFlags;	// A bunch of flags of type InternalFlag. The different bits have different r/w status. See that enum for details.


	////////////////////////Joints & Articulation////////////////////////
	/*
	This is a tree data structure that gives us the projection order of joints in which this body is the tree root.
	this is only valid for a joint group root when BF_ARTICULATED_JOINT_GROUP is set. Owned.
	note: the link of the root body is not necces. the root link due to the re-rooting of the articulation!
	*/
	Articulation*	scpArticulations;
	Group	scpJointGroup;		// A 'simulation island' of joints. Created by a union-find algorithm every time a new joint is added to any of the involved bodies.
	NxU32	inpComShiftCounter; // Used by joints to determine when the COM has changed

	// Created at the start of the SDK substep based on joints created or destroyed previously by the user.
	Joint*	scpJointList;			// List of all joints between this body, and bodies with a larger guid().
	Joint*	scpReverseJointList;    // List of all joints between this body, and bodies with a smaller guid(). Only valid during articulation building.





	////////////////////////Sleeping////////////////////////
	NxU32	sctForcedToSleepCounter;	//forced to sleep flag....temp fix.
	NxReal	sleepEnergyThreshold;
	NxReal	sleepDamping;
	NxVec3	sleepLinVelAcc;
	NxVec3	sleepAngVelAcc;
	NxReal	wakeCounter;








	////////////////////////Buffering for API access////////////////////////
	// These are updated every swap
	NxVec3	apiVelocity;
	NxVec3	apiAngVelocity;		
	bool	apiIsSleeping;
	NxQP	apiActor2World;






	////////////////////////Input state////////////////////////
	// Semi-constant inputs from API (changes infrequently)
	NxReal	inpLinearDamping;				// The velocity is scaled by (1.0f - this * dt) inside integrateVelocity() every substep.
	NxReal	inpAngularDamping;
	NxReal	inpInvMass;						// The inverse of the body mass.
	NxVec3	inpInverseInertiaBodySpace;		// The inverse of the body space inertia tensor.
	NxReal	inpSleepLinVelSq;				// If the squared velocity of the body is below this, it decrements the wakeCounter and eventually goes to sleep.
	NxReal	inpSleepAngVelSq;
	NxReal	inpMaxAngVelSq;					// The angular velocity's magnitude is clamped to this maximum value.
	NxReal	inpCCDMotionThreshold;			// When CCD is globally enabled, then it is still not performed if the motion distance of all points on the body is below this treshold.
	NxU32	inpSolverIterationCount;		// Minimum number of iterations to run in solver when solving constraints that involve this body.
	NxReal	inpContactReportThreshold;







	////////////////////////Transient inputs////////////////////////
	// Inputs from API that are only used once and then reset again
	// Important: Struct is reset in setForcesToDefaults.
	struct VelocityMod
	{
		NxVec3	linearPerSec;		// A request to change the linear velocity by this much each second. The velocity is changed by this * dt inside integrateVelocity() every substep.
		NxVec3	angularPerSec;	
		NxVec3	linearPerStep;		// A request to change the linear velocity by this much each superstep. The velocity is changed by this / numSubsteps inside integrateVelocity() every substep.
		NxVec3	angularPerStep;
	} insVelMod;


	////////////////////////Scratchpad data////////////////////////
	// Used for various calculations during substep
	NxU32	sctBodyConstraints;			// Used by the USE_ADAPTIVE_FORCE mode only to keep track of how many constraints are on a body
	NxU32	sctLastBodyConstraints;
	NxU8	sctAccDirtyCounter;			// 2 when acceleration has just been modified, 1 when it has just been reset, 0 when it hasn't been changed
	NxReal	sctImpactTime;				// This is written by ccd to keep track of the lowest impact time found so far.

	NxVec3	scpMotionVel;     // Motion velocities. We distinguish between the velocity as an integral of acceleration and the velocity that is the derivative of pose.
	NxVec3	scpMotionAngVel;  // This motion velocity is the second one, and differs from the regular velocity by effectively position based projection terms applied by the solver.

	// This is used by broad and near phase when moving fast, and because its expensive to compute we need to cache it. Its here and not in shape because parts of a compound don't need their own. Only valid if APF_CCD_MOTION is set; updated un update()
	NxReal	sctRadius; // Instead of storing sctTemporalWorldBounds, we just store a radius from which its really easy to compute an approximative bounds or capsule on the fly. This saves storage.







	////////////////////////Miscellaneous////////////////////////
	KinematicTarget*	kinematicTarget;	// Only valid/initialized if this is kinematic
	NxActor*			owningNxActor;		// pshelus - hack added for sleep events
	NxU32		id;		// Deterministic ID for body, used instead of address for sorting and stuff like that.

	NxVec3	filteredLinVel;
	NxVec3	filteredAngVel;

	mutable NxMat33	derInverseInertiaWorldSpace;	//derived quantity. Computed as:  derBody2world.M * mIBodyInv % derBody2world.M
	};


	
	

	
	
	
/////////Implementations of inlined methods/////////
NX_INLINE void Body::updateActorFromBody2ActorChange()	//does not update inertia tensor.
	{
	// This change is swapped to the external apiActor2World on next swap buffers
	// This is called on internal change
	
	//This is what's done below: sctActor2World = getBody2WorldLL() * !getBody2Actor();
	sctActor2World.q = getBody2WorldLL().q * !getBody2Actor().q;
	sctActor2World.p = getBody2WorldLL().p - sctActor2World.q.rot(getBody2Actor().p);
	poseTimeStamp++;
	}

NX_INLINE void Body::setForcesToDefaults()
	{
	memset(&insVelMod, 0, sizeof(insVelMod));
	if (sctAccDirtyCounter>0)
		sctAccDirtyCounter--;
	}

NX_INLINE Body* Body::jointGroupGetRoot()
/*-------------------------------*\
| Implementation of FIND of 
| UNION-FIND algo.
\*-------------------------------*/
	{
	//return FIND(this);
	NX_ASSERT(scpJointGroup.parent);
	if (this != scpJointGroup.parent)
		scpJointGroup.parent = scpJointGroup.parent->jointGroupGetRoot();
	return scpJointGroup.parent;
	}

NX_INLINE void Body::setInverseInertiaBodySpace(const NxVec3 &v)
	{
	inpInverseInertiaBodySpace = v; 
	if (!(inpPublicFlags & NX_BF_KINEMATIC))
		{
		PxFloat invMass[4] = {inpInvMass, inpInverseInertiaBodySpace.x, inpInverseInertiaBodySpace.y, inpInverseInertiaBodySpace.z};
		PxdAtomSetProperty(atom, PXD_ATOM_INVERTED_MASS, invMass, sizeof(invMass));
		}
	}

NX_INLINE const NxMat33& Body::getGlobalInertiaTensorInverse() const 
	{ 
	cacheInertia();
	return derInverseInertiaWorldSpace; 
	}

NX_INLINE void Body::setInvMass(NxReal v)
	{ 
	inpInvMass = v; 
	if (!(inpPublicFlags & NX_BF_KINEMATIC))
		{
		PxFloat invMass[4] = {inpInvMass, inpInverseInertiaBodySpace.x, inpInverseInertiaBodySpace.y, inpInverseInertiaBodySpace.z};
		PxdAtomSetProperty(atom, PXD_ATOM_INVERTED_MASS, invMass, sizeof(invMass));
		}
	}

NX_INLINE void Body::computeImpulseResponse(const NxVec3 & normal, const NxVec3 & r, SpatialVec & response)
	{
	response.a = getGlobalInertiaTensorInverse() * r.cross(normal);
	response.b = normal * getInvMass();
	}

NX_INLINE void Body::computePointVelocity(const NxVec3 & point, NxVec3 &result) const
	{
	NxVec3 linVel, angVel;
	getVelocityLL(linVel, angVel);
	result = linVel + angVel.cross(point);
	}

NX_INLINE void Body::syncToLL()
	{
	if (llState.llPoseDirty)
		{
		NX_ASSERT(llState.pose.p.isFinite());
		NX_ASSERT(llState.pose.q.isFinite());
		PxdTransform pose;
		pose.translation = (PxdVector&)llState.pose.p;
		pose.rotation = (PxdQuaternion&)llState.pose.q;
		PxdAtomSetTransform(atom, PXD_ATOM_POSE, pose);
		llState.llPoseDirty = false;
		}

	if (llState.llVelocityDirty)
		{
		NX_ASSERT(llState.linVelocity.isFinite());
		NX_ASSERT(llState.angVelocity.isFinite());
		PxdAtomSetProperty(atom, PXD_ATOM_VELOCITY, &llState.linVelocity, 2*sizeof(NxVec3));
		llState.llVelocityDirty = false;
		}
	}

NX_INLINE void Body::syncFromLL()
	{
	NX_ASSERT(body2WorldOld.p.isFinite());
	NX_ASSERT(body2WorldOld.q.isFinite());
	body2WorldOld = getBody2WorldLL();
	cacheState();
	raiseFlag(BF_INERTIA_DIRTY);
	}

NX_INLINE void Body::swapBuffers()
	{
	apiActor2World = sctActor2World;
	poseTimeStamp++;
	getVelocityLL(apiVelocity, apiAngVelocity);
	apiIsSleeping = !isActive();
	}

NX_INLINE void Body::cacheState() const
	{
	PxdAtomGetProperty(atom, PXD_ATOM_POSE_AND_VELOCITY, &llState, sizeof(float)*13);
	NX_ASSERT(llState.pose.p.isFinite());
	NX_ASSERT(llState.pose.q.isFinite());
	NX_ASSERT(llState.linVelocity.isFinite());
	NX_ASSERT(llState.angVelocity.isFinite());
	}

NX_INLINE void Body::cacheInertia() const
	{
	if (readFlag(BF_INERTIA_DIRTY))
		{
		transformInertiaTensor(getInverseInertiaBodySpace(), getBody2WorldLL().q, derInverseInertiaWorldSpace);
		scpInternalFlags &= ~BF_INERTIA_DIRTY; // Can't call non-const clearFlag
		}
	}

NX_INLINE void Body::setRestingLL(bool resting)
	{
	if (resting && !readFlag(BF_RESTING_LL))
		{
		if (atom != 0)
			PxdAtomSetInt(atom, PXD_ATOM_HINT_RESTING, 1);
		raiseFlag(BF_RESTING_LL);
		}
	else if (!resting && readFlag(BF_RESTING_LL))
		{
		if (atom != 0)
			PxdAtomSetInt(atom, PXD_ATOM_HINT_RESTING, 0);
		clearFlag(BF_RESTING_LL);
		}
	}

NX_INLINE void Body::getVelocityLL(NxVec3& linVelLL, NxVec3& angVelLL) const 
	{ 
	linVelLL = llState.linVelocity;
	angVelLL = llState.angVelocity;
	}


NX_INLINE void Body::setVelocityLL(const NxVec3& linVel, const NxVec3& angVel)
{	 		
	if (readIntFlag(Body::BF_IS_IN_DIRTY_LIST) == NX_FALSE)
		setDirty();

	llState.llVelocityDirty = true;
	llState.linVelocity = linVel;
	llState.angVelocity = angVel;
}

NX_INLINE void Body::setLinearVelocityLL(const NxVec3& linVel)		
{	 		
	if (readIntFlag(Body::BF_IS_IN_DIRTY_LIST) == NX_FALSE)
		setDirty();

	llState.llVelocityDirty = true;
	llState.linVelocity = linVel;
}

NX_INLINE void Body::setAngularVelocityLL(const NxVec3& angVel)		
{	 		
	if (readIntFlag(Body::BF_IS_IN_DIRTY_LIST) == NX_FALSE)
		setDirty();

	llState.llVelocityDirty = true;
	llState.angVelocity = angVel;
}


NX_INLINE void Body::setKinematicTargetPos(const NxVec3 &v) 
{ 
	kinematicTarget->pos = v; 
	kinematicTarget->valid |= KinematicTarget::POS_VALID; 

	if (readIntFlag(Body::BF_IS_IN_DIRTY_LIST) == NX_FALSE)
		setDirty();
}

NX_INLINE void Body::setKinematicTargetQuat(const NxQuat &q) 
{ 
	kinematicTarget->orient = q; 
	kinematicTarget->valid |= KinematicTarget::ROT_VALID;

	if (readIntFlag(Body::BF_IS_IN_DIRTY_LIST) == NX_FALSE)
		setDirty();
}

#endif
