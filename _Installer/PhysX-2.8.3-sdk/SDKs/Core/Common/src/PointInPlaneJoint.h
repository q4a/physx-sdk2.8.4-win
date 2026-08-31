#ifndef NX_PHYSICS_POINTINPLANEJOINT
#define NX_PHYSICS_POINTINPLANEJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Joint.h"

class PointInPlaneJoint : public Joint/*, public NxPointInPlaneJoint*/
	{
	public:
	virtual void loadFromDesc(const NvJointDesc &, const NvJointPointOnPlaneDesc &);
	
	virtual void release();
	virtual operator Joint &();
	virtual Joint & getJoint();

			PointInPlaneJoint* isPointInPlaneJoint();


	PointInPlaneJoint(const NvJointDesc & desc, const NvJointPointOnPlaneDesc &);
	~PointInPlaneJoint();
	virtual void processToAxisConstraints(NxReal dt);
	virtual void visualize(NxFoundation::DebugRenderable &);

	virtual void disableAxCons();

	private:
	PxdConstraint constraint;
	};
//	}
#endif
