#ifndef NX_PHYSICS_NP_POINTONLINEJOINT
#define NX_PHYSICS_NP_POINTONLINEJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "MacroNpJoint.h"
#include "Allocateable.h"
#include "NxPointOnLineJoint.h"
#include "NpJoint.h"

class NpPointOnLineJoint :  public NxPointOnLineJoint, public NpJoint, public NxFoundation::NxAllocateable
	{

	NPJOINT_DECLARATION(PointOnLineJoint)

	public:
	virtual void loadFromDesc(const NxPointOnLineJointDesc&);
	virtual void saveToDesc(NxPointOnLineJointDesc&);

	//internal:
	virtual void visualize(NxFoundation::DebugRenderable & dr);
	virtual void sendToVRD(bool create, bool fromCtor, bool callBase);
private:
	void specificRemoveFromVRD(){}
	};

#endif
