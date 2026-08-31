#ifndef NX_PHYSICS_NP_ACTOR
#define NX_PHYSICS_NP_ACTOR
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxActor.h"
#include "Allocateable.h"
#include "ReadWriteMutex.h"
#include "Actor.h"


class ActorState;
class NvScene;
/**
This is an API class. API classes run in a different thread than the simulation.
For the sake of simplicity they have their own methods, and they do not call simulation
methods directly. To set simulation state, they also have their own custom set
methods in the implementation classes.

To make this clear the implementation classes (e.g. Actor) all derive from a base
class (e.g. ActorState) that has the API's I/O accessors. The API should only communicate 
with this base class and not the derived class that is for the simulation thread only.

There is an exception to this which is the dispatch of simulation calls such as raycasting
and the simulation itself.
*/
class NpActor : public NxActor, public NxFoundation::NxAllocateable, public ReadWriteMutex
	{
	public:
	virtual							~NpActor();

	// API interface:
	// Save / Load
	virtual		void				saveToDesc(NxActorDescBase&);
	
	// Statics and dynamics
	// Debug name
	virtual		const char*			getName()			const;
	virtual		void				setName(const char*);

	// Pose
	virtual		void 				setGlobalPose(const NxMat34&);
	virtual		void 				setGlobalPosition(const NxVec3&);
	virtual		void 				setGlobalOrientation(const NxMat33&);
	virtual		void 				setGlobalOrientationQuat(const NxQuat&);

	virtual		NxMat34				getGlobalPose()			const;
	virtual		NxVec3 				getGlobalPosition()		const;
	virtual		NxMat33				getGlobalOrientation()	const;
	virtual		NxQuat 				getGlobalOrientationQuat()const;

	virtual		void 				moveGlobalPose(const NxMat34&);
	virtual		void 				moveGlobalPosition(const NxVec3&);
	virtual		void 				moveGlobalOrientation(const NxMat33&);
	virtual		void 				moveGlobalOrientationQuat(const NxQuat&);

	// Shapes
	virtual		NxShape*			createShape(const NxShapeDesc&);
	virtual		void				releaseShape(NxShape&);
	virtual		NxU32				getNbShapes()		const;
	virtual		NxShape*const *		getShapes()			const;

	// Groups
	virtual		void				setGroup(NxActorGroup);
	virtual		NxActorGroup		getGroup() const;

	virtual		void				setDominanceGroup(NxDominanceGroup dominanceGroup);
	virtual		NxDominanceGroup	getDominanceGroup() const;


	// Flags
	virtual		void				raiseActorFlag(NxActorFlag);
	virtual		void				clearActorFlag(NxActorFlag);
	virtual		bool				readActorFlag(NxActorFlag)	const;

	// User actor pair filtering
	virtual		void				resetUserActorPairFiltering();

	// Make dynamic
	virtual		bool				isDynamic()			const;

	// Dynamics only
	// Save / Load
	virtual		bool				saveBodyToDesc(NxBodyDesc&);

	// Center of mass pose
	virtual		void				setCMassOffsetLocalPose(const NxMat34&);
	virtual		void				setCMassOffsetLocalPosition(const NxVec3&);
	virtual		void				setCMassOffsetLocalOrientation(const NxMat33&);

	virtual		void				setCMassOffsetGlobalPose(const NxMat34&);
	virtual		void				setCMassOffsetGlobalPosition(const NxVec3&);
	virtual		void				setCMassOffsetGlobalOrientation(const NxMat33&);

	virtual		void 				setCMassGlobalPose(const NxMat34&);
	virtual		void 				setCMassGlobalPosition(const NxVec3&);
	virtual		void 				setCMassGlobalOrientation(const NxMat33&);

	virtual		NxMat34 			getCMassGlobalPose()			const;
	virtual		NxVec3 				getCMassGlobalPosition()		const;
	virtual		NxMat33 			getCMassGlobalOrientation()	const;

	virtual		NxMat34 			getCMassLocalPose()			const;
	virtual		NxVec3 				getCMassLocalPosition()		const;
	virtual		NxMat33 			getCMassLocalOrientation()	const;

	// Mass
	virtual		void				setMass(NxReal mass);
	virtual		NxReal				getMass()										const;

	virtual		void				setMassSpaceInertiaTensor(const NxVec3& m);

	virtual		NxVec3				getMassSpaceInertiaTensor()		const;
	virtual		NxMat33				getGlobalInertiaTensor()			const;
	virtual		NxMat33				getGlobalInertiaTensorInverse()	const;

	virtual		bool				updateMassFromShapes(NxReal density, NxReal totalMass);

	// Damping
	virtual		void				setLinearDamping(NxReal);
	virtual		NxReal				getLinearDamping()								const;
	virtual		void				setAngularDamping(NxReal);
	virtual		NxReal				getAngularDamping()								const;

	// Velocity
	virtual		void				setLinearVelocity(const NxVec3&);
	virtual		void				setAngularVelocity(const NxVec3&);
	virtual		void				setMaxAngularVelocity(NxReal);

	virtual		NxVec3				getLinearVelocity()	const;
	virtual		NxVec3				getAngularVelocity()	const;
	virtual		NxReal				getMaxAngularVelocity()	const; 

	virtual		void				setCCDMotionThreshold(NxReal);
	virtual		NxReal				getCCDMotionThreshold()	const; 



	// Momentum
	virtual		void				setLinearMomentum(const NxVec3&);
	virtual		void				setAngularMomentum(const NxVec3&);

	virtual		NxVec3				getLinearMomentum()	const;
	virtual		NxVec3				getAngularMomentum()	const;


	// Force/Torque modifiers
	virtual		void				addForceAtPos(const NxVec3& force, const NxVec3& pos, NxForceMode mode, bool wakeup);
	virtual		void				addForceAtLocalPos(const NxVec3& force, const NxVec3& pos, NxForceMode mode, bool wakeup);
	virtual		void				addLocalForceAtPos(const NxVec3& force, const NxVec3& pos, NxForceMode mode, bool wakeup);
	virtual		void				addLocalForceAtLocalPos(const NxVec3& force, const NxVec3& pos, NxForceMode mode, bool wakeup);
	virtual		void				addForce(const NxVec3&, NxForceMode mode, bool wakeup);
	virtual		void				addLocalForce(const NxVec3&, NxForceMode mode, bool wakeup);
	virtual		void				addTorque(const NxVec3&, NxForceMode mode, bool wakeup);
	virtual		void				addLocalTorque(const NxVec3&, NxForceMode mode, bool wakeup);

	// Kinetic energy & point velocities
	virtual		NxReal				computeKineticEnergy()						const;

	virtual		NxVec3				getPointVelocity(const NxVec3& point)		const;
	virtual		NxVec3				getLocalPointVelocity(const NxVec3& point)	const;


	// Sleeping
	virtual		bool				isGroupSleeping()	const;
	virtual		bool				isSleeping()		const;

    virtual     NxReal              getSleepLinearVelocity() const;
    virtual     void                setSleepLinearVelocity(NxReal threshold);

    virtual     NxReal              getSleepAngularVelocity() const;
    virtual     void                setSleepAngularVelocity(NxReal threshold);

	virtual		NxReal				getSleepEnergyThreshold() const;
	virtual		void				setSleepEnergyThreshold(NxReal threshold);

	virtual		void				wakeUp(NxReal wakeCounterValue=NX_SLEEP_INTERVAL);
	virtual		void				putToSleep();

	// Flags							
	virtual		void				raiseBodyFlag(NxBodyFlag);
	virtual		void				clearBodyFlag(NxBodyFlag);
	virtual		bool				readBodyFlag(NxBodyFlag)		const;

	virtual		NxScene&			getScene()						const;

	virtual		void				setSolverIterationCount(NxU32);
	virtual		NxU32				getSolverIterationCount() const;

	virtual		NxReal				getContactReportThreshold() const;
	virtual		void				setContactReportThreshold(NxReal threshold);
	virtual		void				setContactReportFlags(NxU32 flags);
	virtual		NxU32				getContactReportFlags() const;

	virtual NxCompartment *			getCompartment() const { return compartment; }

	virtual NxForceFieldMaterial	getForceFieldMaterial()	const;							
	virtual	void	 				setForceFieldMaterial(NxForceFieldMaterial mat);

	//group access:

//	NxMat34							getActor2World();
	NxMat34							getActor2World_API();
	bool							loadFromDesc(const NxActorDescBase &);
	void							sendToVRD(bool create, bool fromCtor);
	void							removeFromVRD();
	void							finishVRDFrame();

#if NX_SUPPORT_SWEEP_API
	virtual		NxU32				linearSweep(const NxVec3& motion, NxU32 flags, void* userData, NxU32 nbShapes, NxSweepQueryHit* shapes, NxUserEntityReport<NxSweepQueryHit>* callback, const NxSweepCache* sweepCache);
#endif


									NpActor(NvScene* scene);

	NX_INLINE	const Actor*		getActor() const { return &actor; }
	NX_INLINE		  Actor*		getActor()		 { return &actor; }

	Actor actor;
	NxCompartment * compartment;
	};

#endif
