#ifndef NX_PHYSICS_NP_PRISMATICJOINT
#define NX_PHYSICS_NP_PRISMATICJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "MacroNpJoint.h"
#include "Allocateable.h"
#include "NxPrismaticJoint.h"
#include "NpJoint.h"


class NpPrismaticJoint :  public NxPrismaticJoint, public NpJoint, public NxFoundation::NxAllocateable
	{

	NPJOINT_DECLARATION(PrismaticJoint)

	public:
	virtual void loadFromDesc(const NxPrismaticJointDesc&);
	virtual void saveToDesc(NxPrismaticJointDesc&);

	//internal:
	virtual void visualize(NxFoundation::DebugRenderable & dr);
	virtual	void sendToVRD(bool create, bool fromCtor, bool callBase);
private:
	void specificRemoveFromVRD(){}
	};
#endif
