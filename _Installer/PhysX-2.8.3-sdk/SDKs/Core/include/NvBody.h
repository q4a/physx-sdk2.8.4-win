#ifndef NX_PHYSICS_NV_BODY
#define NX_PHYSICS_NV_BODY
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"

class NvScene;
class NvShape;
class NxSphere;
class Body;
class NxActor;


class NvBodyDesc
	{
	public:
	NxQuat		actor2WorldQ;
	NxVec3		actor2WorldP;

	NxQuat		body2ActorQ;
	NxVec3		body2ActorP;

	NxVec3		linearVelocity;
	NxVec3		angularVelocity;

	NxReal		sleepTimer;

	//NxVec3		linVelModPerSec;
	//NxVec3		angVelModPerSec;	
	//NxVec3		linVelModPerStep;
	//NxVec3		angVelModPerStep;

	NxReal		linearDamping;
	NxReal		angularDamping;
	NxReal		inverseMass;
	NxVec3		inverseInertia;
	NxReal		sleepLinearVelocitySq;
	NxReal		sleepAngularVelocitySq;
	NxReal		maxAngularVelocitySq;
	NxReal		CCDMotionThreshold;
	NxReal		sleepEnergyThreshold;
	NxReal		sleepDamping;
	NxReal		contactReportThreshold;
	NxU32		publicFlags;
	NxU32		solverIterationCount;

	// pshelus - HACK! Sleep events need to know NxActor
	NxActor*    owningNxActor;

	NxDominanceGroup actorDominanceGroup;

	//NxVec3		kinematicPos;
	//NxQuat		kinematicQuat;
	};




class NvBody
	{
	public:
	virtual void			release() = 0;
	
	/* actor coordinates */

	virtual NxVec3 			getActor2WorldP()						const	= 0;
	virtual NxQuat 			getActor2WorldQ()						const	= 0;
	virtual void			setActor2WorldP(const NxVec3 & t)				= 0;
	virtual void			setActor2WorldQ(const NxQuat & q)				= 0;
	virtual void			setActor2WorldPQ(const NxVec3 & t, const NxQuat & q) = 0;	//combined call needed to avoid computing intermediates twice when both values change.

		
	/* velocity */

	virtual NxVec3 			getLinearVelocity()						const	= 0;
	virtual void			setLinearVelocity(const NxVec3 &v)				= 0; 
	virtual NxVec3 			getAngularVelocity()					const	= 0; 
	virtual void			setAngularVelocity(const NxVec3 &v)				= 0; 


	/* force application */
	virtual	void			addForceAtPos(const NxVec3 & force, const NxVec3 & pos, NxForceMode) = 0;
	virtual void			addSpatialForce(const NxVec3 * force, const NxVec3 * torque, NxForceMode) = 0;

	/* wakeup API */

	virtual NxReal			getSleepTimer() const =0;				//better name plus it doesn't collide with internal set/getWakeTimer(internal) and set/getWakeTimer_API().
	virtual void			setSleepTimer(NxReal) =0;
	virtual bool			isGroupSleeping() const = 0;


	/* constants for the body */

	virtual NxReal			getInverseMass()					const	= 0;
	virtual void			setInverseMass(NxReal)						= 0;
	virtual NxVec3 			getInverseInertia()					const	= 0;
	virtual void 			setInverseInertia(const NxVec3 &)			= 0;

	virtual NxVec3			getBody2ActorP() const = 0;
	virtual NxQuat			getBody2ActorQ() const = 0;
	virtual void			setBody2ActorP(const NxVec3 & t) = 0;
	virtual void			setBody2ActorQ(const NxQuat & q) = 0;
	virtual void			setBody2ActorPQ(const NxVec3 & t, const NxQuat & q) = 0;		//combined call needed to avoid computing intermediates twice when both values change.

	virtual NxReal			getLinearDamping() const  =0; 
	virtual void			setLinearDamping(NxReal v) =0; 

	virtual NxReal			getAngularDamping() const =0; 
	virtual void			setAngularDamping(NxReal v) =0; 

	virtual	NxReal			getSleepEnergyThreshold() const =0;
	virtual	void			setSleepEnergyThreshold(NxReal v) =0;

	virtual NxU32			getPublicFlags() const  =0; 
	virtual void			setPublicFlags(NxU32 v) = 0; 

	virtual NxReal			getSleepLinVelSq() const =0; 
	virtual void			setSleepLinVelSq(NxReal v) = 0; 

	virtual NxReal			getSleepAngVelSq() const =0; 
	virtual void			setSleepAngVelSq(NxReal v) = 0; 

	virtual NxReal			getMaxAngVelSq() const =0; 
	virtual void			setMaxAngVelSq(NxReal v) = 0;

	virtual NxU32			getSolverIterationCount() const	 = 0; 
	virtual void			setSolverIterationCount(NxU32) = 0; 

	virtual NxReal			getCCDMotionThreshold() const	 = 0; 
	virtual void			setCCDMotionThreshold(NxReal) = 0; 

	virtual void			setKinematic(bool on) = 0;
	virtual void			setKinematicTargetP(const NxVec3 & t)  =0;
	virtual void			setKinematicTargetQ(const NxQuat & q)  =0;
	virtual void			setKinematicTargetPQ(const NxVec3 & t, const NxQuat & q)  =0;

	virtual	NxReal			getContactReportThreshold() const =0;
	virtual	void			setContactReportThreshold(NxReal threshold) =0;

	virtual NxU32			getID() const = 0;

	/* shape accessor */

	virtual NvShape*		getNvShape() const = 0;
	virtual void			setActorDominanceGroup(NxDominanceGroup) = 0;

	protected:
	~NvBody() {}	//use release() plz.
	};



#endif
