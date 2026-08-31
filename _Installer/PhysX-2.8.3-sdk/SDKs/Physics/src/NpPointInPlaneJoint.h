#ifndef NX_PHYSICS_NP_POINTINPLANEJOINT
#define NX_PHYSICS_NP_POINTINPLANEJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "MacroNpJoint.h"
#include "Allocateable.h"
#include "NxPointInPlaneJoint.h"
#include "NpJoint.h"

class NpPointInPlaneJoint : public NxPointInPlaneJoint, public NpJoint, public NxFoundation::NxAllocateable
	{

	NPJOINT_DECLARATION(PointInPlaneJoint)

	public:
	virtual void loadFromDesc(const NxPointInPlaneJointDesc&);
	virtual void saveToDesc(NxPointInPlaneJointDesc&);

	//internal:
	virtual void visualize(NxFoundation::DebugRenderable & dr);
	virtual void sendToVRD(bool create, bool fromCtor, bool callBase);
private:
	void specificRemoveFromVRD(){}
	};
#endif
