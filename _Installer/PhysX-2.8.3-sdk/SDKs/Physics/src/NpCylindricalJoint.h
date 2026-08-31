#ifndef NX_PHYSICS_NP_SLIDINGJOINT
#define NX_PHYSICS_NP_SLIDINGJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "MacroNpJoint.h"
#include "Allocateable.h"
#include "NxCylindricalJoint.h"


class NpCylindricalJoint : public NxCylindricalJoint, public NpJoint, public NxFoundation::NxAllocateable
	{

	NPJOINT_DECLARATION(CylindricalJoint)

	public:
	//NxJoint:
	virtual void loadFromDesc(const NxCylindricalJointDesc&);
	virtual void saveToDesc(NxCylindricalJointDesc&);

	//internal:
	virtual void visualize(NxFoundation::DebugRenderable &);
	virtual	void sendToVRD(bool create, bool fromCtor, bool callBase);
private:
	void specificRemoveFromVRD(){}
	};

#endif
