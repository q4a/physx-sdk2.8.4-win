#ifndef NX_PHYSICS_NP_SPHERICALJOINT
#define NX_PHYSICS_NP_SPHERICALJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "MacroNpJoint.h"
#include "Allocateable.h"
#include "NxSphericalJoint.h"
#include "NpJoint.h"

class NpSphericalJoint : public NxSphericalJoint, public NpJoint, public NxFoundation::NxAllocateable
	{
	
	NPJOINT_DECLARATION(SphericalJoint)

	public:
	virtual		void				loadFromDesc(const NxSphericalJointDesc&);
	virtual		void				saveToDesc(NxSphericalJointDesc&);

	virtual		void				setFlags(NxU32 flags);
	virtual		NxU32				getFlags();
	virtual		void				setProjectionMode(NxJointProjectionMode projectionMode);
	virtual NxJointProjectionMode	getProjectionMode();
	virtual		void				visualize(NxFoundation::DebugRenderable & dr);
	virtual		void				sendToVRD(bool create, bool fromCtor, bool callBase);
	
private:
	void specificRemoveFromVRD(){}
	};
#endif
