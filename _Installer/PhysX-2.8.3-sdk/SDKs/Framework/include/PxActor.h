#ifndef NX_FRAMEWORK_PXACTOR
#define NX_FRAMEWORK_PXACTOR
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nx.h"
#include "Px.h"
#include "NxArray.h"
#include "PxIterator.h"

#include "px_lowlevel.h"

class PxScene;
class PxInteraction;
class PxElement;

// The actor represents a simulated item that can be
// connected to other actors through interactions. An actor
// is created inactive but can be set either active or inactive. 

class PxActor
{
	friend class PxScene;
	friend class PxInteraction;
	friend class PxElement;
public:
	//Actors automatically register themselves in the scene here
	virtual void initialize();

	//Actors automatically unregister themselves from the scene here
	virtual void destroy();

	// Get the scene the actor resides in
	NX_INLINE PxScene& getPxScene() const { return ownerScene; }

	// Get whether the actor is active or not
	NX_INLINE bool isActive() const { return active; }

	// Get the number of interactions connected to the actor
	NX_INLINE NxU32 getInteractionCount() const { return interactions.size(); }

	// Prepares the actor for less than n interactions
	virtual void setInteractionCountHint(NxU32 n) { interactions.reserve(n); } 

	// Get an iterator to the interactions connected to the actor
	NX_INLINE PxIterator<PxInteraction*const> getInteractions() const { return PxIterator<PxInteraction*const>(interactions.begin(), interactions.end()); }

	// Get the number of elements in the actor
	NX_INLINE NxU32 getElementCount() const { return elements.size(); }

	// Prepares the actor for less than n elements
	virtual void setElementCountHint(NxU32 n) { elements.reserve(n); }

	// Get an iterator to the elements in the actor
	NX_INLINE PxIterator<PxElement*const> getElements() const { return PxIterator<PxElement*const>(elements.begin(), elements.end()); }

	// Get the type ID of the actor
	NX_INLINE NxU32 getPxActorType() const { return actorType; }

	// Set active state - should not be called in the midst of a simulation step
	void setActive(bool active);

	// Queried whenever a body wants to be activated
	virtual bool canActivate() const { return false; }

	// Queried every step for deactivation readiness
	virtual bool canDeactivate() const = 0;

	// Increase static touch counter
	NX_INLINE void incStaticTouch() { staticTouchCount++; }

	// Decrease static touch counter
	NX_INLINE void decStaticTouch() { staticTouchCount--; }

	// Returns the static touch counter
	NX_INLINE NxU32 getStaticTouch() { return staticTouchCount; }

	// Get the number of interactions of a specific type
	NX_INLINE NxU32 getNumInteractionsOfType(NxU32 interactionType) const { NX_ASSERT(interactionType<PX_INTERACTION_TYPE_COUNT); return numInteractionsOfType[interactionType]; }

	// Get the index of the island that the actors is currently part of (Only valid for actors that have a valid island)
	NX_INLINE NxU32 getIslandIndex() const { return islandIndex; }

	// Returns whether the actor is valid (not pending deletion) or not
	NX_INLINE NxU32 isValid() const { return id != -1; }

	// Really ugly hack to get from PxActor to NvBody in the HSM. Needs to be redone, but there's no time now...
	virtual void* getAppData() { return 0; }

	// Returns whether this actor has a valid island, i.e. participated in last generateIslands or has an island explicitly generated aftwards
	bool hasValidIsland() const;

	// Temporary fix. Needed so that kinematics can tell whether they have active interactions or not.
	NX_INLINE bool hasActiveInteraction() const { return numActiveInteractions != 0; }

	// Returns whether this actor should be able to build islands on its own or not
	virtual bool isIslandGenerating() const { return true; }

	NX_INLINE void incTransferCount();
	NX_INLINE void decTransferCount();

protected:
	PxActor(PxScene& scene, NxU32 actorType);
	virtual ~PxActor();

	// Called by the framework when an actor is activated. 
	virtual void onActivate() {}
	// Called by the framework when an actor is deactivated. 
	virtual void onDeactivate() {}

	// Temporary fix. Needed so that kinematics can tell whether they have active interactions or not.
	// Called by the framework when the last active interaction deactivates or the first inactive interaction activates.
	virtual void onAllInteractionsInactiveChange(bool allInactive) {};

	//These are called from element creation/destruction
	virtual void registerElement(PxElement* element);
	virtual void unregisterElement(PxElement* element);

private:

	//These are called from interaction creation/destruction
	void registerInteraction(PxInteraction* interaction);
	void unregisterInteraction(PxInteraction* interaction);

	// Temporary fix. Needed so that kinematics can tell wether they have active interactions or not.
	void notifyInteractionActiveChange(PxInteraction* interaction);
	NX_INLINE void incActiveInteractions();
	NX_INLINE void decActiveInteractions();

	NxU32										id; // Used by PxScene 

	NxU32										actorType;

	PxScene&									ownerScene;
	NxFoundation::NxArraySDK<PxInteraction*>	interactions;
	NxU32										numInteractionsOfType[PX_INTERACTION_TYPE_COUNT];
	NxU32										numTransferringInteractions;
	NxU32										numActiveInteractions;

	NxU32										timestamp;
	NxU32										staticTouchCount;
	NxU32										islandIndex;
	NxU32										transferCount;

	NxFoundation::NxArraySDK<PxElement*>		elements;

	bool										active;
};

//////////////////////////////////////////////////////////////////////////

NX_INLINE void PxActor::incActiveInteractions()
{
	++numActiveInteractions;
}

NX_INLINE void PxActor::decActiveInteractions()
{
	NX_ASSERT(numActiveInteractions>0);
	--numActiveInteractions;
}

NX_INLINE void PxActor::incTransferCount()
{
	++transferCount;
}

NX_INLINE void PxActor::decTransferCount()
{
	NX_ASSERT(transferCount>0);
	--transferCount;
}

#endif // NX_FRAMEWORK_PXACTOR
