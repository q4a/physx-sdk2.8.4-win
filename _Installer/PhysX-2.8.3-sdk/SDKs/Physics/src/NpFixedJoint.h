#ifndef NX_PHYSICS_NP_FIXEDJOINT
#define NX_PHYSICS_NP_FIXEDJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "MacroNpJoint.h"
#include "Allocateable.h"
#include "NxFixedJoint.h"
#include "NpJoint.h"

class NpFixedJoint :  public NxFixedJoint, public NpJoint, public NxFoundation::NxAllocateable
	{

	NPJOINT_DECLARATION(FixedJoint)

	public:
	virtual void loadFromDesc(const NxFixedJointDesc&);
	virtual void saveToDesc(NxFixedJointDesc&);
	virtual void sendToVRD(bool create, bool fromCtor, bool callBase);
private:
	void specificRemoveFromVRD(){}
	};

#endif
