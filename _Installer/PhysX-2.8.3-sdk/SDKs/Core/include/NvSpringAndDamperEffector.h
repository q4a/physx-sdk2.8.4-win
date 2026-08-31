#ifndef NX_PHYSICS_NV_SPRINGANDDAMPEREFFECTOR
#define NX_PHYSICS_NV_SPRINGANDDAMPEREFFECTOR
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"

class NvScene;
class NvBody;
class NpSpringAndDamperEffector;
class NxSpringAndDamperEffectorDesc;

class NvSpringAndDamperEffector
	{
	public:
	//Effector:
	virtual NvScene&		getNvScene() = 0;
	virtual void							setNpEffector(NpSpringAndDamperEffector* npJ) = 0;
	virtual NpSpringAndDamperEffector*		getNpEffector() = 0;	//if we add more effector types this should return an NpEffector but for now there is no point.
	virtual NvSpringAndDamperEffector*		getNext() = 0;
	//SAD Effector:
	virtual void			release() = 0;
	virtual void            saveToDesc(NxSpringAndDamperEffectorDesc &desc) = 0;
	virtual void			setBodies(NvBody* body1, const NxVec3& global1, NvBody* body2, const NxVec3& global2) = 0;
	virtual void			getBodies(NvBody** body1, NxVec3& global1, NvBody** body2, NxVec3& global2) = 0;
	virtual void			setLinearSpring(NxReal distCompressSaturate, NxReal distRelaxed, NxReal distStretchSaturate, NxReal maxCompressForce, NxReal maxStretchForce) = 0;
	virtual void			getLinearSpring(NxReal & distCompressSaturate, NxReal & distRelaxed, NxReal & distStretchSaturate, NxReal & maxCompressForce, NxReal & maxStretchForce) = 0;
	virtual void			setLinearDamper(NxReal velCompressSaturate, NxReal velStretchSaturate, NxReal maxCompressForce, NxReal maxStretchForce) = 0;
	virtual void			getLinearDamper(NxReal & velCompressSaturate, NxReal & velStretchSaturate, NxReal & maxCompressForce, NxReal & maxStretchForce) = 0;


	protected:
	~NvSpringAndDamperEffector() {}	//use release() plz.
	};
#endif
