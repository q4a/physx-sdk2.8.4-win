#ifndef NX_COLLISION_NP_SPHERESHAPE
#define NX_COLLISION_NP_SPHERESHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxSphereShape.h"
#include "MacroNpShape.h"

class NpSphereShape : public NxSphereShape, public NpShape, public NxFoundation::NxAllocateable
	{
	
	NPSHAPE_DECLARATION(SphereShape)

	public:

	virtual void	saveToDesc(NxSphereShapeDesc&)	const;
	virtual void	setRadius(NxReal radius);

	virtual void	getWorldSphere(NxSphere&)		const;
	virtual NxReal	getRadius() const;
	virtual bool	addTensorContribution(InertiaTensorComputer& itc) const;

	//internal:
	bool			loadFromDesc(const NxSphereShapeDesc&);
	virtual	void	sendToVRD(bool create, bool fromCtor, bool callBase);

	};
//	}
#endif
