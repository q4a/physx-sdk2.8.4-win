#ifndef NX_PHYSICS_PRISMATICJOINT
#define NX_PHYSICS_PRISMATICJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Joint.h"

class PrismaticJoint : public Joint
{
public:
	virtual void		release();
	virtual operator Joint &();
	virtual Joint	&	getJoint();
	PrismaticJoint*		isPrismaticJoint();

	PrismaticJoint(const NvJointDesc & desc, const NvJointPrismaticDesc &sdesc);
	~PrismaticJoint();
	virtual		void	processToAxisConstraints(NxReal dt);
	virtual		void	visualize(NxFoundation::DebugRenderable &);
	virtual		void	loadFromDesc(const NvJointDesc &, const NvJointPrismaticDesc &);
	virtual		void	disableAxCons();

private:
	enum
	{
		LIN_CON1 = 0,
		LIN_CON2,
		LIN_CON3,
		LIN_CON4,
		ANG_CON1,
		ANG_CON2,
		ANG_CON3,
		LAST_CON,
	};
	void				updateDerived();		//maybe temp:  call this after bodies change.		Merge into updateBodySpaceFrame() maybe. Also in FixedJoint, PrismaticJoint and D6Joint.
	NxQuat				rotOffsetInv;
	PxdConstraint		constraints[LAST_CON];
};
#endif
