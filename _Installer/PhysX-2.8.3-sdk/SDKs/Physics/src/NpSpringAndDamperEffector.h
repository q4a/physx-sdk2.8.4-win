#ifndef NX_PHYSICS_NP_SPRINGANDDAMPEREFFECTOR
#define NX_PHYSICS_NP_SPRINGANDDAMPEREFFECTOR
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxSpringAndDamperEffector.h"
#include "ReadWriteMutex.h"
#include "NxActor.h"
#include "Allocateable.h"
#include "NpEffector.h"

class NvSpringAndDamperEffector;
class NxSpringAndDamperEffectorDesc;


class NpSpringAndDamperEffector : public NxFoundation::NxAllocateable, public NpEffector, public NxSpringAndDamperEffector
	{
	
	public:
	
	NpSpringAndDamperEffector(NvSpringAndDamperEffector* sadEffector);
	virtual ~NpSpringAndDamperEffector();

	virtual void saveToDesc(NxSpringAndDamperEffectorDesc &desc);
	
	virtual void setBodies(NxActor* body1, const NxVec3& global1, NxActor* body2, const NxVec3& global2);

	virtual void setLinearSpring(NxReal distCompressSaturate, NxReal distRelaxed, NxReal distStretchSaturate, NxReal maxCompressForce, NxReal maxStretchForce);
	
	virtual void getLinearSpring(NxReal & distCompressSaturate, NxReal & distRelaxed, NxReal & distStretchSaturate, NxReal & maxCompressForce, NxReal & maxStretchForce);
	
	virtual void setLinearDamper(NxReal velCompressSaturate, NxReal velStretchSaturate, NxReal maxCompressForce, NxReal maxStretchForce);

	virtual void getLinearDamper(NxReal & velCompressSaturate, NxReal & velStretchSaturate, NxReal & maxCompressForce, NxReal & maxStretchForce);
	
	virtual operator NxEffector &();

	virtual NxEffector & getEffector();
	NvSpringAndDamperEffector*	getInternalEffector();

	// NxEffector
	virtual		NxSpringAndDamperEffector * isSpringAndDamperEffector();

	virtual		NxScene&					getScene() const;

	virtual NxEffectorType  getType() const;
	virtual	void			setName(const char* name);
	virtual	const char*		getName()			const;

	private:
		NvSpringAndDamperEffector* mSADEffector;
		NxActor*                   mActors[2];
	
	};
#endif
