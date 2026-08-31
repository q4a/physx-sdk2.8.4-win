#ifndef NX_COLLISION_NP_CAPSULESHAPE
#define NX_COLLISION_NP_CAPSULESHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxSegment.h"
#include "NxCapsule.h"
#include "NxCapsuleShape.h"
#include "NxShape.h"
#include "MacroNpShape.h"

class CapsuleShape;
class NxCapsuleShapeDesc;


class NpCapsuleShape : public NxCapsuleShape, public NpShape, public NxFoundation::NxAllocateable
	{
	
	NPSHAPE_DECLARATION(CapsuleShape)

	public:
	virtual		void		saveToDesc(NxCapsuleShapeDesc&)		const;
	virtual		void		setDimensions(NxReal radius, NxReal height);
	virtual		void		setRadius(NxReal radius);
	virtual		void		setHeight(NxReal height);	
	virtual		NxReal		getRadius() const;
	virtual		NxReal		getHeight() const;
	virtual		void		getWorldCapsule(NxCapsule& worldCapsule) const; 
	virtual		bool		addTensorContribution(InertiaTensorComputer& itc) const;

	//internal:
	bool					loadFromDesc(const NxCapsuleShapeDesc&);
	virtual		void		sendToVRD(bool create, bool fromCtor, bool callBase);
	};

#endif
