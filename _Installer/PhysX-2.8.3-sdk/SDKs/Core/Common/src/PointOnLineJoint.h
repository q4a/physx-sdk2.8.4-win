#ifndef NX_PHYSICS_POINTONLINEJOINT
#define NX_PHYSICS_POINTONLINEJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Joint.h"

class PointOnLineJoint : public Joint/*, public NxPointOnLineJoint*/
	{
	public:
	virtual void loadFromDesc(const NvJointDesc &, const NvJointPointOnLineDesc &);
	
	virtual void release();
	virtual operator NxJoint &();
	virtual Joint & getJoint();
			PointOnLineJoint* isPointOnLineJoint();

	PointOnLineJoint(const NvJointDesc & desc, const NvJointPointOnLineDesc &sdesc);
	~PointOnLineJoint();
	virtual void processToAxisConstraints(NxReal dt);
	virtual void visualize(NxFoundation::DebugRenderable &);

	virtual void disableAxCons();

	private:
	PxdConstraint constraints[2];
	};

#endif
