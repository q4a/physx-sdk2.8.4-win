#ifndef NX_COLLISION_RB_ELEMENT_INTERACTION
#define NX_COLLISION_RB_ELEMENT_INTERACTION
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "ElementInteraction.h"
#include "Shape.h"
#include "Scene.h" // Only because of scene stats 2!

#ifdef NX_ENABLE_SCENE_STATS2
#undef INC_SCENE_STAT
#undef DEC_SCENE_STAT
#undef ADD_TO_SCENE_STATS
#define INC_SCENE_STAT(x) getShape0().getOwnerSceneFast()->stats2.incStat(x)
#define DEC_SCENE_STAT(x) getShape0().getOwnerSceneFast()->stats2.decStat(x)
#define ADD_TO_SCENE_STATS(x, y) getShape0().getOwnerSceneFast()->stats2.addToStat(x, y)
#else
#define INC_SCENE_STAT(x)
#define DEC_SCENE_STAT(x)
#define ADD_TO_SCENE_STATS(x, y)
#endif

class RbElementInteraction : public ElementInteraction
{
public:
	NX_INLINE RbElementInteraction(Shape& shape0, Shape& shape1, PxInteractionType type);
	virtual ~RbElementInteraction() {};
	NX_INLINE virtual void initialize();
	NX_INLINE virtual void destroy();

	NX_INLINE Shape& getShape0() const;
	NX_INLINE Shape& getShape1() const;

	NX_INLINE virtual void updateState();

	NX_INLINE static bool isRbElementInteraction(const PxInteraction& interaction);

protected:
	NX_INLINE virtual void onActivate();
	NX_INLINE virtual void onDeactivate();
private:
};

extern const PxInteractionType shapeInteractionTypeList[5];
typedef ActiveInteractionIterator<RbElementInteraction, PxIterator<PxInteraction*const>,
								  5, shapeInteractionTypeList> ActiveRbElementInteractionIterator;
typedef ActiveInteractionIterator<RbElementInteraction, PxBackwardIterator<PxInteraction*const>,
								  5, shapeInteractionTypeList> ActiveRbElementInteractionBackwardIterator;

typedef InteractionIterator<RbElementInteraction, PxIterator<PxInteraction*const>,
							5, shapeInteractionTypeList> RbElementInteractionIterator;
typedef InteractionIterator<RbElementInteraction, PxBackwardIterator<PxInteraction*const>,
							5, shapeInteractionTypeList> RbElementInteractionBackwardIterator;

//////////////////////////////////////////////////////////////////////////
RbElementInteraction::RbElementInteraction(Shape& shape0, Shape& shape1, PxInteractionType type)
: ElementInteraction(shape0, shape1, type)
{
}

void RbElementInteraction::initialize()
{
	ElementInteraction::initialize();
#ifdef NX_ENABLE_SCENE_STATS2
	INC_SCENE_STAT(StatTotalPairs);
#endif
}

void RbElementInteraction::destroy()
{
#ifdef NX_ENABLE_SCENE_STATS2
	DEC_SCENE_STAT(StatTotalPairs);
#endif
	ElementInteraction::destroy();
}

void RbElementInteraction::onActivate()
{
	ElementInteraction::onActivate();
#ifdef NX_ENABLE_SCENE_STATS2
	INC_SCENE_STAT(StatActivePairs);
#endif
}

void RbElementInteraction::onDeactivate()
{
#ifdef NX_ENABLE_SCENE_STATS2
	DEC_SCENE_STAT(StatActivePairs);
#endif
	ElementInteraction::onDeactivate();
}

void RbElementInteraction::updateState()
{
	// Copy dirty flags before calling parent updateState
	NxU16 dirtyFlags = getDirtyFlags();

	CoreInteraction::updateState();
	if (readDirtyFlag(dirtyFlags, CoreInteraction::CIF_DIRTY_ACTOR))
	{
		if ((getShape0().getRbActor().getActorPublicFlags() & NX_AF_USER_ACTOR_PAIR_FILTERING) ||
			(getShape1().getRbActor().getActorPublicFlags() & NX_AF_USER_ACTOR_PAIR_FILTERING))
			raiseCoreFlag(CoreInteraction::USER_CALLBACK_FILTERING_ENABLED);
		else
			clearCoreFlag(CoreInteraction::USER_CALLBACK_FILTERING_ENABLED);
	}
}

NX_INLINE Shape& RbElementInteraction::getShape0() const
{
	return static_cast<Shape&>(getElement0());
}

NX_INLINE Shape& RbElementInteraction::getShape1() const
{
	return static_cast<Shape&>(getElement1());
}

NX_INLINE bool RbElementInteraction::isRbElementInteraction(const PxInteraction& interaction)
{
	switch (interaction.getType())
	{
	case PX_INTERACTION_TYPE_TRIGGER:
	case PX_INTERACTION_TYPE_OVERLAP_HL:
	case PX_INTERACTION_TYPE_OVERLAP_LL:
	case PX_INTERACTION_TYPE_PAGEBOUNDS:
	case PX_INTERACTION_TYPE_MARKER:
		return true;
		break;
	case PX_INTERACTION_TYPE_JOINT:
	case PX_INTERACTION_TYPE_FLUID_BODY:
		return false;
		break;
	default:
		// Unknown type
		NX_ASSERT(0);
		return false;
	};
	return false;
}

#ifdef NX_ENABLE_SCENE_STATS2
#undef INC_SCENE_STAT
#undef DEC_SCENE_STAT
#undef ADD_TO_SCENE_STATS
#endif

#endif
