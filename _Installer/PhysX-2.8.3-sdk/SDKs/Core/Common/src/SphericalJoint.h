#ifndef NX_PHYSICS_SPHERICALJOINT
#define NX_PHYSICS_SPHERICALJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Joint.h"
#include "NxJointLimitPairDesc.h"
#include "NxSpringDesc.h"

class SphericalJoint : public Joint
{
public:
	virtual void release();
	virtual		void							loadFromDesc(const NvJointDesc &, const NvJointSphereicalDesc &sdesc);
	virtual		void							disableAxCons();

	virtual		void							setSolverExtrapolationFactor(NxReal sef);
	virtual		void							setUseAccelerationSpring(bool b);

	NX_INLINE	void							setFlags(NxU32 f) { sphereFlags = f; disableAxCons(); }
	NX_INLINE	NxU32							getFlags() const	{return sphereFlags;}

	NX_INLINE	void							setSphereTwistLimit(const NxJointLimitPairDesc & l) { twistLimit = l; disableAxCons(); }
	NX_INLINE	NxJointLimitPairDesc  			getSphereTwistLimit() const { return twistLimit; }
	NX_INLINE	void							setSwingLimit(const NxJointLimitDesc & l) { swingLimit = l; cosOfswingAngle = NxMath::cos(swingLimit.value); disableAxCons(); }
	NX_INLINE	NxJointLimitDesc 				getSwingLimit() const { return swingLimit; }

	NX_INLINE	void							setTwistSpring(const NxSpringDesc & s) { twistSpring = s; disableAxCons(); }
	NX_INLINE	void							setSwingSpring(const NxSpringDesc & s) { swingSpring = s; disableAxCons(); }
	NX_INLINE	void							setJointSpring(const NxSpringDesc & s) { jointSpring = s; disableAxCons(); }
	NX_INLINE	NxSpringDesc  					getTwistSpring() const { return twistSpring; }
	NX_INLINE	NxSpringDesc  					getSwingSpring() const { return swingSpring; }
	NX_INLINE	NxSpringDesc  					getJointSpring() const { return jointSpring; }
	NX_INLINE	void				 			setSwingAxis(const NxVec3 & s) { jswingAxis = s; 
	NxMat33 j2b;
	j2b.setColumn(0, getLocalNormalFast(0));
	j2b.setColumn(1, getLocalBinormalFast(0));
	j2b.setColumn(2, getLocalAxisFast(0));
	j2b.multiply(jswingAxis, bswingAxis);
	disableAxCons();
	}
	NX_INLINE	NxVec3				 			getSwingAxis() const { return jswingAxis; }

	NX_INLINE	void							setProjectionDistance(NxReal dist) { projectionDistance = dist; disableAxCons(); }
	NX_INLINE	NxReal							getProjectionDistance() const { return projectionDistance; }

	SphericalJoint(const NvJointDesc & desc, const NvJointSphereicalDesc &sdesc);
	~SphericalJoint();
	virtual		void				processToAxisConstraints(NxReal dt);
	virtual		void				processLimitsToAxisConstraints(NxReal dt);
	virtual		void				visualize(NxFoundation::DebugRenderable&);
	virtual		void				projectPose(Body * childBody);

	NxReal				getTwist(NxVec3& twistAxis, NxReal& reliability);

private:
	enum
	{
		D6_CON = 0,
		TWIST_SPRING,
		SWING_SPRING1,
		SWING_SPRING2,
		LAST_CON,
	};

	NxJointLimitPairDesc			twistLimit;
	NxJointLimitDesc				swingLimit;

	NxSpringDesc					twistSpring;
	NxSpringDesc					swingSpring;
	NxSpringDesc					jointSpring;

	NxVec3							jswingAxis;			//joint space swing axis, fixed in the joint space of body1. 
	NxVec3							bswingAxis;			//this is also const:  joint2body * jswingAxis
	NxReal							cosOfswingAngle;	//NxMath::cos(swingLimit.value);

	NxU32							sphereFlags;		//!< This is a combination of the bits defined by ::NxSphericalJointFlag . 
	NxReal							projectionDistance;	//if this is negative we don't project

	PxdHandle						constraints[LAST_CON];


};
#endif
