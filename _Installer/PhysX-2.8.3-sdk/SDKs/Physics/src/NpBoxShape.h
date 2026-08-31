#ifndef NX_COLLISION_NP_BOXSHAPE
#define NX_COLLISION_NP_BOXSHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxShape.h"
#include "NxBoxShape.h"
#include "MacroNpShape.h"
#include "NxBox.h"
#include "Actor.h"

class BoxShape;

class NpBoxShape : public NxBoxShape, public NpShape, public NxFoundation::NxAllocateable
	{
	
	NPSHAPE_DECLARATION(BoxShape)

	public:
	
	virtual		void				saveToDesc(NxBoxShapeDesc&)		const;
	virtual		void				setDimensions(const NxVec3&);
	virtual		NxVec3				getDimensions()					const;
	virtual		void				getWorldOBB(NxBox& world_obb)	const;

	//internal:
	bool							loadFromDesc(const NxBoxShapeDesc&);
	virtual bool					addTensorContribution(InertiaTensorComputer& itc) const;
	virtual	void					sendToVRD(bool create, bool fromCtor, bool callBase);

	};

//	}
#endif
