#ifndef NX_PHYSICS_SLIDINGJOINT
#define NX_PHYSICS_SLIDINGJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Joint.h"

class CylindricalJoint : public Joint/*, public NxCylindricalJoint*/
{
public:
	virtual void loadFromDesc(const NvJointDesc &, const NvJointCylindricalDesc &);

	virtual void release();
	virtual operator Joint &();
	virtual Joint & getJoint();
	virtual CylindricalJoint* isCylindricalJoint();
	virtual void disableAxCons();

	//group access:
	CylindricalJoint(const NvJointDesc & desc, const NvJointCylindricalDesc &sdesc);
	~CylindricalJoint();
	void computeDerived();
	virtual void processToAxisConstraints(NxReal dt);
	virtual void visualize(NxFoundation::DebugRenderable &);
private:
	enum
	{
		CON1=0,
		CON2,
		CON3,
		CON4,
		LAST_CON,
	};
	PxdConstraint constraints[LAST_CON];
};
#endif
