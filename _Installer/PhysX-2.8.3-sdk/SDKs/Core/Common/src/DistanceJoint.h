#ifndef NX_PHYSICS_DISTANCEJOINT
#define NX_PHYSICS_DISTANCEJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxSpringDesc.h"
#include "Joint.h"
//namespace NxRigidBody
//	{

class DistanceJoint : public Joint/*, public NxDistanceJoint*/
	{
	public:
//	virtual void loadFromDesc(const NxDistanceJointDesc&);
//	virtual void saveToDesc(NxDistanceJointDesc&);
//	virtual operator Joint &();
//	virtual Joint & getJoint();
//			DistanceJoint* isDistanceJoint();

	//group access:
	DistanceJoint(const NvJointDesc & desc, const NvJointDistanceDesc &sdesc);
//	void	loadFromDescInternal(const NxDistanceJointDesc&);
	~DistanceJoint();
	virtual void release();
	void computeDerived();
	virtual void processToAxisConstraints(NxReal dt);
	virtual void visualize(NxFoundation::DebugRenderable &);
	virtual		void						loadFromDesc(const NvJointDesc &, const NvJointDistanceDesc &sdesc);

	virtual	void disableAxCons();

	NX_INLINE	void setMaxDistance(NxReal md)	{ maxDistance = md; }
	NX_INLINE	void setMinDistance(NxReal md)	{ minDistance = md; }
	
	NX_INLINE	void setSpring(const NxSpringDesc & sd) { spring = sd; }
	NX_INLINE	void setDistanceFlags(NxU32 df)	{ distanceFlags = df; }

	NX_INLINE	NxReal getMaxDistance() const	{ return maxDistance; }
	NX_INLINE	NxReal getMinDistance() const	{ return minDistance; }
	
	NX_INLINE	bool getSpring(NxSpringDesc &d) const	{ d = spring; return 1; }
	NX_INLINE	NxU32 getDistanceFlags() const	{ return distanceFlags; }

	private:
	NxReal maxDistance;
	NxReal minDistance;
	//NxReal stiffness;
	NxSpringDesc spring;
	NxU32  distanceFlags;			//!< This is a combination of the bits defined by ::NxDistanceJointFlag . 
	PxdConstraint springConstraint;
	};
//	}
#endif
