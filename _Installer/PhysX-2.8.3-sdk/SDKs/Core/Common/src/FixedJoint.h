#ifndef NX_PHYSICS_FIXEDJOINT
#define NX_PHYSICS_FIXEDJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Joint.h"
//namespace NxRigidBody
//	{

class FixedJoint : public Joint/*, public NxFixedJoint*/
{
public:
	//	virtual void loadFromDesc(const NxFixedJointDesc&);
	//	virtual void saveToDesc(NxFixedJointDesc&);
	//	virtual operator Joint &();
	//	virtual Joint & getJoint();
	//			FixedJoint* isFixedJoint();

	//group access:
	FixedJoint(const NvJointDesc & desc, const NvJointFixedDesc &);
	//	void	loadFromDescInternal(const NxFixedJointDesc&);
	~FixedJoint();
	//group access:
	virtual void release();
	virtual void processToAxisConstraints(NxReal dt);
	virtual void visualize(NxFoundation::DebugRenderable &);
	virtual		void						loadFromDesc(const NvJointDesc &, const NvJointFixedDesc &);
	virtual void disableAxCons();


	virtual void comShiftUpdate();		//maybe temp:  call this after bodies change.		Merge into updateBodySpaceFrame() maybe. Also in FixedJoint, PrismaticJoint and D6Joint.
private:
	enum
	{
		LIN_X = 0,
		LIN_Y,
		LIN_Z,
		ANG_X,
		ANG_Y,
		ANG_Z,
		DOF_LAST,
	};

	NX_INLINE bool hasConstraints() { return constraints[0] != 0; } // If we have one, we have all
	NxVec3 fixedJointOffset;
	NxQuat  rotOffsetInv;
	PxdConstraint constraints[DOF_LAST];
	Body* orderedBodies[2];  // Ordered so that any static body is first
};
//	}
#endif
