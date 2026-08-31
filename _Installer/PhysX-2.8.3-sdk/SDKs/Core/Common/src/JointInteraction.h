#ifndef NX_PHYSICS_JOINT_INTERACTION
#define NX_PHYSICS_JOINT_INTERACTION
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "ActorInteraction.h"

class Joint;

class JointInteraction : public ActorInteraction
{
public:

	JointInteraction(Joint* joint);

	~JointInteraction();

	virtual void destroy();
	virtual bool isActive() const;
	virtual void updateState();

	virtual void onActivate();
	virtual void onDeactivate();
	virtual void onActorActiveChange(PxActor* /*actor*/);

	void updateActivityTransfer();

	NX_INLINE Joint* getJoint() const;
	NX_INLINE static bool isJointInteraction(const PxInteraction& interaction);
private:
	Joint* joint;
};

//////////////////////////////////////////////////////////////////////////
NX_INLINE Joint* JointInteraction::getJoint() const
{
	return joint;
}

NX_INLINE bool JointInteraction::isJointInteraction(const PxInteraction& interaction)
{
	return (interaction.getType() == PX_INTERACTION_TYPE_JOINT);
}

#endif

