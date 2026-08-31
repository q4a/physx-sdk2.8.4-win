#ifndef NX_PHYSICS_PULLEYJOINT
#define NX_PHYSICS_PULLEYJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxMotorDesc.h"
#include "Joint.h"

class SolverBody;

class PulleyJoint : public Joint
{
public:
	virtual void release();
	virtual operator Joint &();
	virtual Joint & getJoint();
	virtual PulleyJoint* isPulleyJoint();
	virtual		void						loadFromDesc(const NvJointDesc &, const NvJointPulleyDesc &);
	virtual void disableAxCons();


	NX_INLINE void	setPulley(NxU32 i, const NxVec3 & p)	{ pulley[i] = p; disableAxCons(); }
	NX_INLINE void	setTargetDistance(NxReal td)			{ targetDistance = td; disableAxCons(); }
	NX_INLINE void	setStiffness(NxReal st)					{ stiffness = st; disableAxCons(); }
	NX_INLINE void  setRatio(NxReal r)						{ ratio = r; disableAxCons(); }
	void	setFlags(NxU32 flags);
	void	setMotor(const NxMotorDesc &);

	NX_INLINE NxVec3 getPulley(NxU32 i) const				{ return pulley[i]; }
	NX_INLINE NxReal getTargetDistance() const				{ return targetDistance; }
	NX_INLINE NxReal getStiffness() const					{ return stiffness; }
	NX_INLINE NxReal getRatio() const						{ return ratio; }
	NX_INLINE NxU32  getFlags() const						{ return pulleyFlags; }
	bool   getMotor(NxMotorDesc &) const;


	PulleyJoint(const NvJointDesc & desc, const NvJointPulleyDesc &);
	~PulleyJoint();
	virtual void processToAxisConstraints(NxReal dt);
	virtual void visualize(NxFoundation::DebugRenderable &);
private:
	enum 
	{
		PULLEY_CON = 0,
		MOTOR_CON1,
		MOTOR_CON2,
		LAST_CON,
	};
	NxVec3				pulley[2];			// GLOBALPOS
	NxReal				targetDistance;
	NxReal				stiffness;
	NxReal				ratio;				//!< transmission ratio
	NxU32				pulleyFlags;			//!< This is a combination of the bits defined by ::NxPulleyJointFlag. 
	NxMotorDesc			motor;	
	NxReal				bias;

	PxdConstraint		constraints[LAST_CON];
};
#endif
