#ifndef NX_COLLISION_CORE_ELEMENT
#define NX_COLLISION_CORE_ELEMENT
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "PxElement.h"
#include "CoreActor.h"

class CoreElement : public PxElement
{
public:
	NX_INLINE CoreElement(CoreActor& actor, PxVolumeType type);
	NX_INLINE ~CoreElement();
	NX_INLINE CoreActor& getCoreActor() const;

private:
};

//////////////////////////////////////////////////////////////////////////
NX_INLINE CoreElement::CoreElement(CoreActor& actor, PxVolumeType type) : PxElement(actor, type)
{
}

NX_INLINE CoreElement::~CoreElement()
{
}

NX_INLINE CoreActor& CoreElement::getCoreActor() const
{
	return static_cast<CoreActor&>(getPxActor());
}

#endif
