#ifndef NX_PHYSICS_NP_D6JOINT
#define NX_PHYSICS_NP_D6JOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "MacroNpJoint.h"
#include "Allocateable.h"
#include "NxD6Joint.h"
#include "NpJoint.h"

class NpD6Joint :  public NxD6Joint, public NpJoint, public NxFoundation::NxAllocateable
	{

	NPJOINT_DECLARATION(D6Joint)

	public:
	virtual void loadFromDesc(const NxD6JointDesc&);
	virtual void saveToDesc(NxD6JointDesc&);

	/**
	Set the drive position
	*/
 
	virtual void setDrivePosition(const NxVec3 &position);
 
	/**
	Set the drive orientation
	*/
 
	virtual void setDriveOrientation(const NxQuat &orientation); 
 
	/**
	Set the drive linear velocity
	*/
 
	virtual void setDriveLinearVelocity(const NxVec3 &linVel);
 
	/**
	Set the drive angular velocity
	*/
 
	virtual void setDriveAngularVelocity(const NxVec3 &angVel);

	//internal:
	virtual void visualize(NxFoundation::DebugRenderable & dr);
	virtual	void sendToVRD(bool create, bool fromCtor, bool callBase);
	
private:
	void specificRemoveFromVRD();
	};

#endif
