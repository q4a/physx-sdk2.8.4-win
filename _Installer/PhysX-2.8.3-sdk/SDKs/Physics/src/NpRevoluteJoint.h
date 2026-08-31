#ifndef NX_PHYSICS_NP_HINGEJOINT
#define NX_PHYSICS_NP_HINGEJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "MacroNpJoint.h"
#include "Allocateable.h"
#include "NxRevoluteJoint.h"
#include "NpJoint.h"

class NpRevoluteJoint :  public NxRevoluteJoint, public NpJoint, public NxFoundation::NxAllocateable
	{
	
	NPJOINT_DECLARATION(RevoluteJoint)
	
	public:
	virtual		void				loadFromDesc(const NxRevoluteJointDesc&);
	virtual		void				saveToDesc(NxRevoluteJointDesc&);

	virtual		void				setLimits(const NxJointLimitPairDesc &);
	virtual		bool				getLimits(NxJointLimitPairDesc &);
	virtual		void				setMotor(const NxMotorDesc &);
	virtual		bool				getMotor(NxMotorDesc &);
	virtual		void				setSpring(const NxSpringDesc &);
	virtual		bool				getSpring(NxSpringDesc &);

	virtual		NxReal				getAngle();
	virtual		NxReal				getVelocity();
	virtual		void				setFlags(NxU32 flags);
	virtual		NxU32				getFlags();
	virtual		void				setProjectionMode(NxJointProjectionMode projectionMode);
	virtual	NxJointProjectionMode	getProjectionMode();
	virtual		void				visualize(NxFoundation::DebugRenderable & dr);
	virtual		void				sendToVRD(bool create, bool fromCtor, bool callBase);
private:
	void specificRemoveFromVRD(){}
	};
#endif
