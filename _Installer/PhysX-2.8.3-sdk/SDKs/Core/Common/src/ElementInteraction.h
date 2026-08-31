#ifndef NX_COLLISION_ELEMENT_INTERACTION
#define NX_COLLISION_ELEMENT_INTERACTION
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "CoreInteraction.h"
#include "PxElementInteraction.h"
#include "CoreElement.h"

class CoreElement;
class NPhaseCore;

class ElementInteraction : public CoreInteraction, public PxElementInteraction
{
public:
	NX_INLINE ElementInteraction(CoreElement& element0, CoreElement& element1, PxInteractionType type);
	virtual ~ElementInteraction() {};
	
	NX_INLINE CoreElement& getCoreElement0() const;
	NX_INLINE CoreElement& getCoreElement1() const;
	virtual NPhaseCore* getNPhaseCore() const;

private:
};

//////////////////////////////////////////////////////////////////////////
NX_INLINE ElementInteraction::ElementInteraction(CoreElement &element0, CoreElement &element1, PxInteractionType type)
: CoreInteraction(true), PxElementInteraction(element0, element1, type)
{
}

NX_INLINE CoreElement& ElementInteraction::getCoreElement0() const
{
	return static_cast<CoreElement&>(getElement0());
}

NX_INLINE CoreElement& ElementInteraction::getCoreElement1() const
{
	return static_cast<CoreElement&>(getElement1());
}

#endif
