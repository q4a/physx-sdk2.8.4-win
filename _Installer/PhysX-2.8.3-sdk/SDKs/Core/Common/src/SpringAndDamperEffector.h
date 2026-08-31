#ifndef NX_PHYSICS_SPRINGANDDAMPEREFFECTOR
#define NX_PHYSICS_SPRINGANDDAMPEREFFECTOR
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nxp.h"
#include "BodyPairEffector.h"
#include "NxSpringAndDamperEffectorDesc.h"

class NpSpringAndDamperEffector;
//namespace NxRigidBody
//	{

/**
 Represents a spring and damper element, which excerts a force between two bodies,
 proportional to the relative positions and the relative velocities of the bodies.
*/
class SpringAndDamperEffector : public BodyPairEffector // : public NxBodyPairEffector
{
	public:
	void release();

	void saveToDesc(NxSpringAndDamperEffectorDesc &desc);
	void setBodies(NvBody* body1, const NxVec3& global1, NvBody* body2, const NxVec3& global2);
	void getBodies(NvBody** body1, NxVec3& global1, NvBody** body2, NxVec3& global2);
	void setLinearSpring(NxReal distCompressSaturate, NxReal distRelaxed, NxReal distStretchSaturate, NxReal maxCompressForce, NxReal maxStretchForce);
	void getLinearSpring(NxReal & distCompressSaturate, NxReal & distRelaxed, NxReal & distStretchSaturate, NxReal & maxCompressForce, NxReal & maxStretchForce);
	void setLinearDamper(NxReal velCompressSaturate, NxReal velStretchSaturate, NxReal maxCompressForce, NxReal maxStretchForce);
	void getLinearDamper(NxReal & velCompressSaturate, NxReal & velStretchSaturate, NxReal & maxCompressForce, NxReal & maxStretchForce);
	
	//group access:
	SpringAndDamperEffector(Scene & ownerScene);
	virtual void applyToPair(Body * b1, Body * b2);
	
	virtual ~SpringAndDamperEffector();
	
	private:
	NxReal linearSpringForce(NxReal distance);
	NxReal linearDamperForce(NxReal velocity);

	NxVec3 local1;	//the position of one		end of the SD element fixed in the space of body1.
	NxVec3 local2;	//the position of the other end of the SD element fixed in the space of body2.


	//linear spring parameters:
	NxReal springDistCompressSaturate;
	NxReal springDistRelaxed;
	NxReal springDistStretchSaturate;
	NxReal springMaxCompressForce;
	NxReal springMaxStretchForce;

	//linear damper parameters:

	NxReal damperVelCompressSaturate;
	NxReal damperVelStretchSaturate;
	NxReal damperMaxCompressForce;
	NxReal damperMaxStretchForce;
	};
#endif
