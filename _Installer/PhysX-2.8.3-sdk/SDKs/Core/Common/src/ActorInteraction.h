#ifndef NX_COLLISION_ACTOR_INTERACTION
#define NX_COLLISION_ACTOR_INTERACTION
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "PxInteraction.h"
#include "CoreInteraction.h"
#include "CoreActor.h"

class CoreActor;
class Scene;

class ActorInteraction : public PxInteraction, public CoreInteraction
{
public:
	NX_INLINE ActorInteraction(CoreActor& actor0, CoreActor& actor1, PxInteractionType interactionType);
	virtual ~ActorInteraction();
	NX_INLINE CoreActor& getCoreActor0() const;
	NX_INLINE CoreActor& getCoreActor1() const;
	virtual NPhaseCore* getNPhaseCore() const;
	NX_INLINE Scene& getScene() const;
private:
};

//////////////////////////////////////////////////////////////////////////
NX_INLINE ActorInteraction::ActorInteraction(CoreActor& actor0, CoreActor& actor1, PxInteractionType interactionType)
	: PxInteraction(actor0, actor1, interactionType),
	  CoreInteraction(false)
{
}

NX_INLINE CoreActor& ActorInteraction::getCoreActor0() const
{
	return static_cast<CoreActor&>(getActor0());
}

NX_INLINE CoreActor& ActorInteraction::getCoreActor1() const
{
	return static_cast<CoreActor&>(getActor1());
}

NX_INLINE Scene& ActorInteraction::getScene() const
{
	return getCoreActor0().getScene();
}

#endif
