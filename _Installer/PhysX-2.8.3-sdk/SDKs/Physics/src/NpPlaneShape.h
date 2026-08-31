#ifndef NX_COLLISION_NP_PLANESHAPE
#define NX_COLLISION_NP_PLANESHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxPlaneShape.h"
#include "NxPlane.h"
#include "MacroNpShape.h"

class NpPlaneShape : public NxPlaneShape, public NpShape, public NxFoundation::NxAllocateable
	{

	NPSHAPE_DECLARATION(PlaneShape)
	
	public:

	virtual		void			saveToDesc(NxPlaneShapeDesc&)	const;
	virtual		void			setPlane(const NxVec3 & normal, NxReal d);
	virtual		bool			addTensorContribution(InertiaTensorComputer& itc) const;
	virtual		NxPlane			getPlane() const;

	//internal:
	bool						loadFromDesc(const NxPlaneShapeDesc&);
	virtual		void			sendToVRD(bool create, bool fromCtor, bool callBase);
	};
//	}
#endif
