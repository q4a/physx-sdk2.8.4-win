#ifndef NX_COLLISION_TRIGGERINTERACTION
#define NX_COLLISION_TRIGGERINTERACTION

#include "RbElementInteraction.h"
#include "ShapeInstancePair.h"
#include "NvSceneStats2.h"

#if NX_FIX_TTP_1922
enum TriggerStatus
{
	TRIGGER_DISJOINT,
	TRIGGER_INSIDE,
	TRIGGER_OVERLAP,
};

struct TriggerCache
{
	NxReal px, py, pz;
	NxU32 index;
	NxU32 state;
};
#endif

//////////////////////////////////////////////////////////////////////////

class TriggerInteraction : public RbElementInteraction
{
public:

	NX_INLINE TriggerInteraction(Shape& triggerShape, Shape& otherShape);
	virtual ~TriggerInteraction();
	void* operator new(size_t s, void* memory)
	{
		return memory;
	}

	NX_INLINE TriggerCache* getTriggerCache()
	{
		return &triggerCache;
	}
	Shape* getTriggerShape() const
	{
		return &getShape0();
	}
	Shape* getOtherShape() const
	{
		return &getShape1();
	}

	//////////////////////// PxInteraction ////////////////////////
	virtual	bool isActive() const;
	virtual	void initialize();
	virtual	void destroy();
	virtual	void onActivate();
	virtual	void onDeactivate();

protected:

	TriggerCache triggerCache;

};

//////////////////////////////////////////////////////////////////////////
TriggerInteraction::TriggerInteraction(Shape& tShape, Shape& oShape)
	: RbElementInteraction(tShape, oShape, PX_INTERACTION_TYPE_TRIGGER) {}

#endif
