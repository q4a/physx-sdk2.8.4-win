#ifndef NX_PHYSICS_NP_DISTANCEJOINT
#define NX_PHYSICS_NP_DISTANCEJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "MacroNpJoint.h"
#include "Allocateable.h"
#include "NxDistanceJoint.h"
#include "NpJoint.h"

class NpDistanceJoint :  public NxDistanceJoint, public NpJoint, public NxFoundation::NxAllocateable
	{

	NPJOINT_DECLARATION(DistanceJoint)

	public:
	virtual void loadFromDesc(const NxDistanceJointDesc&);
	virtual void saveToDesc(NxDistanceJointDesc&);

	//internal:
	virtual void visualize(NxFoundation::DebugRenderable & dr);
	virtual void sendToVRD(bool create, bool fromCtor, bool callBase);
private:
	void specificRemoveFromVRD(){}
	};

#endif
