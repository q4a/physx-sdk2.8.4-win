#ifndef NX_COLLISION_NP_CONVEXSHAPE
#define NX_COLLISION_NP_CONVEXSHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxConvexShape.h"
#include "MacroNpShape.h"

class ConvexMesh;

class NpConvexShape : public NxConvexShape, public NpShape, public NxFoundation::NxAllocateable
	{

	NPSHAPE_DECLARATION(ConvexShape)

	
	public:

	virtual		void			saveToDesc(NxConvexShapeDesc&)	const;
	virtual		bool			addTensorContribution(InertiaTensorComputer& itc) const;
	virtual		NxConvexMesh&	getConvexMesh();
	virtual	const NxConvexMesh&	getConvexMesh() const;
	//internal:
	bool						loadFromDesc(const NxConvexShapeDesc&);
	virtual		void			sendToVRD(bool create, bool fromCtor, bool callBase);
	};
//	}
#endif
