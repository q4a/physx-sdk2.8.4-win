#ifndef NX_PHYSICS_NP_PULLEYJOINT
#define NX_PHYSICS_NP_PULLEYJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "MacroNpJoint.h"
#include "Allocateable.h"
#include "NxPulleyJoint.h"
#include "NpJoint.h"


class NpPulleyJoint :  public NxPulleyJoint, public NpJoint, public NxFoundation::NxAllocateable
	{

	NPJOINT_DECLARATION(PulleyJoint)

	public:
	virtual void loadFromDesc(const NxPulleyJointDesc&);
	virtual void saveToDesc(NxPulleyJointDesc&);

	virtual		void				setMotor(const NxMotorDesc &);
	virtual		bool				getMotor(NxMotorDesc &);
	virtual		void				setFlags(NxU32 flags);
	virtual		NxU32				getFlags();

	//internal:
	virtual void visualize(NxFoundation::DebugRenderable & dr);
	virtual void sendToVRD(bool create, bool fromCtor, bool callBase);
	
private:
	void specificRemoveFromVRD(){}
	};

#endif
