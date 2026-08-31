#ifndef NX_FRAMEWORK_PXINTERACTION
#define NX_FRAMEWORK_PXINTERACTION

#include "Nx.h"
#include "PxScene.h"
#include "PxActor.h"
#include "Allocateable.h"

#define PX_INVALID_INTERACTION_ACTOR_ID 0xffff
#define PX_INVALID_INTERACTION_SCENE_ID 0xffffffff

// Interactions are used for connecting actors into activation
// groups. An interaction always connects exactly two actors. 
// An interaction is implicitly active if at least one of the two 
// actors it connects is active.
// Todo: we might need an interaction callback mechanism

class PxInteraction : public NxFoundation::NxAllocateable
{
	friend class PxScene;
	friend class PxActor;
public:
	// Interactions automatically register themselves in the actors here
	NX_INLINE virtual void initialize();

	// Interactions automatically unregister themselves in the actors here
	NX_INLINE virtual void destroy();
 
	NX_INLINE PxActor& getActor0() const { return actor0; }
	NX_INLINE PxActor& getActor1() const { return actor1; }

	// Returns true if the interaction is active
	virtual bool isActive() const = 0;

	// Get the type ID of the interaction
	NX_INLINE PxInteractionType getType() const { return interactionType; }
	
protected:
	PxInteraction(PxActor& actor0, PxActor& actor1, PxInteractionType interactionType);
	virtual ~PxInteraction() {};
	
	// Called by the framework when an actor changes activity
	// This is needed because HL needs interactions to be active if at least one actor is active.
	// LL managers can currently only be active if both atoms are active.
	NX_INLINE virtual void onActorActiveChange(PxActor* actor) {};


	NX_INLINE void registerInteraction() { actor0.getPxScene().registerInteraction(this); }
	NX_INLINE void unregisterInteraction() { actor0.getPxScene().unregisterInteraction(this); }

	// Called by the framework when an interaction is activated. Called when interaction created active.
	NX_INLINE virtual void onActivate() {}
	// Called by the framework when an interaction is deactivated. Called when interaction destroyed active.
	NX_INLINE virtual void onDeactivate() {}

	NX_INLINE void setActorId (PxActor *actor, NxU16 id) { NX_ASSERT(id != PX_INVALID_INTERACTION_ACTOR_ID); if (&actor0 == actor) actorId0 = id; else actorId1 = id; }
	NX_INLINE NxU16 getActorId (PxActor *actor)  const { if (&actor0 == actor) return actorId0; else return actorId1; }

private:
	PxActor &actor0;
	PxActor &actor1;

	NxU32 sceneId;
	NxU16 actorId0;
	NxU16 actorId1;

protected:
	PxInteractionType interactionType;
	// Should be false in case of a disjoint interaction (e.g. filtered SIPs)
	bool canTransferActivity;
};

//////////////////////////////////////////////////////////////////////////
NX_INLINE void PxInteraction::initialize()
{
	actor0.getPxScene().registerInteraction(this);
	actor0.registerInteraction(this);
	actor1.registerInteraction(this);
	if (isActive())
		onActivate();
}

NX_INLINE void PxInteraction::destroy()
{
	actor0.unregisterInteraction(this);
	actor1.unregisterInteraction(this);
	actor0.getPxScene().unregisterInteraction(this);
	if (isActive())
		onDeactivate();
}

//////////////////////////////////////////////////////////////////////////

// This iterator will iterate all active interactions of the types in the typeList
#if defined(PXD_PLATFORM_WII) || defined(PXD_PLATFORM_WIIU)
template<class OutClass, class IteratorClass, NxU32 typeListCount, const PxInteractionType* typeList>
#else
template<class OutClass, class IteratorClass, NxU32 typeListCount, const PxInteractionType typeList[typeListCount]>
#endif
class ActiveInteractionIterator
{
public:
	NX_INLINE ActiveInteractionIterator(PxScene& s) : scene(s), currentTypeIndex(0), it(s.getActiveInteractions(typeList[0])) {	}

	NX_INLINE OutClass* getNext()
	{
		if (*it == it.end())
		{
			++currentTypeIndex;
			if (currentTypeIndex == typeListCount)
				return NULL;
			else
			{
				it = scene.getActiveInteractions(typeList[currentTypeIndex]);
				return getNext();
			}
		}
		else
		{
			PxInteraction* interaction = **it;
			++it;
			return (OutClass*)interaction;
		}

	}

	NX_INLINE void prefetchContents1(NxU32 prefOffset)
	{
		it.prefetchContents1(prefOffset);
	}
	NX_INLINE void prefetchContents2(NxU32 prefOffset)
	{
		it.prefetchContents2(prefOffset);
	}
	NX_INLINE void prefetchContents3(NxU32 prefOffset)
	{
		it.prefetchContents3(prefOffset);
	}
	NX_INLINE void prefetchContents4(NxU32 prefOffset)
	{
		it.prefetchContents4(prefOffset);
	}

	NX_INLINE void prefetchContents5(NxU32 prefOffset)
	{
		it.prefetchContents5(prefOffset);
	}	
private:
	PxScene& scene;
	NxU32 currentTypeIndex;
	IteratorClass it;
};


// This iterator will iterate all interactions of the types in the typeList
#if defined(PXD_PLATFORM_WII) || defined(PXD_PLATFORM_WIIU)
template<class OutClass, class IteratorClass, NxU32 typeListCount, const PxInteractionType* typeList>
#else
template<class OutClass, class IteratorClass, NxU32 typeListCount, const PxInteractionType typeList[typeListCount]>
#endif
class InteractionIterator
{
public:
	NX_INLINE InteractionIterator(PxScene& s) : scene(s), currentTypeIndex(0), it(s.getInteractions(typeList[0])) {	}

	NX_INLINE OutClass* getNext()
	{
		if (*it == it.end())
		{
			++currentTypeIndex;
			if (currentTypeIndex == typeListCount)
				return NULL;
			else
			{
				it = scene.getInteractions(typeList[currentTypeIndex]);
				return getNext();
			}
		}
		else
		{
			PxInteraction* interaction = **it;
			++it;
			return (OutClass*)interaction;
		}

	}

	NX_INLINE void prefetchContents1(NxU32 prefOffset)
	{
		it.prefetchContents1(prefOffset);
	}
	NX_INLINE void prefetchContents2(NxU32 prefOffset)
	{
		it.prefetchContents2(prefOffset);
	}
	NX_INLINE void prefetchContents3(NxU32 prefOffset)
	{
		it.prefetchContents3(prefOffset);
	}
	NX_INLINE void prefetchContents4(NxU32 prefOffset)
	{
		it.prefetchContents4(prefOffset);
	}

	NX_INLINE void prefetchContents5(NxU32 prefOffset)
	{
		it.prefetchContents5(prefOffset);
	}	
private:
	PxScene& scene;
	NxU32 currentTypeIndex;
	IteratorClass it;
};

#endif // NX_FRAMEWORK_PXINTERACTION
