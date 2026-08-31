#ifndef NX_FRAMEWORK_PXSCENE
#define NX_FRAMEWORK_PXSCENE
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nx.h"
#include "Px.h"
#include "Array.h"
#include "PxIterator.h"

#include "px_lowlevel.h"

class PxActor;
class PxElement;
class PxInteraction;
class AgPPUConnection;

enum PxSceneFlags
{	
	PX_SCENE_FORCE_SW_BROADPHASE = (1<<0),
};


// The scene is the most central part of the interaction framework.
// It keeps lists of active actors and interactions and manages deactivation/
// island generation.
class PxScene : public NxFoundation::NxAllocateable
{
	friend class PxActor;
	friend class PxElement;
	friend class PxInteraction;
public:
	PxScene(bool isHW);
	// You must explicitly delete all actors and interactions before
	// destroying the scene. That's life.
	~PxScene();

	// Initialize the scene
	bool init(NxU32 flags, 
			  AgPPUConnection* connection, 
			  PxdBroadPhaseImplementation bp, 
			  bool useFastMassiveBPVolumeDeletion, 
			  bool useImprovedSpringSolver,
			  bool useLegacyJointDrive);

	NX_INLINE bool isValid() const { return (llContext != 0 && llBroadPhaseContext != 0); }

	NX_INLINE bool readFlag(PxSceneFlags flag) const { return (flags & flag) != 0; }

	// Get the total number of actors
	NX_INLINE unsigned int getActorCount() const { return actors.size(); }

	// Get the number of actors that are active
	NX_INLINE unsigned int getActiveActorCount() const { return activeActorCount; }

	// Get the maximum number of active actors allowed
	NX_INLINE unsigned int getActiveActorLimit() const { return activeActorLimit; }

	// Get an iterator to all actors
	NX_INLINE PxIterator<PxActor*const> getActors() const { return PxIterator<PxActor*const>(actors.begin(), actors.end()); }

	// Get an iterator to the active actors
	NX_INLINE PxIterator<PxActor*const> getActiveActors() const { return PxIterator<PxActor*const>(actors.begin(), &actors[activeActorCount]); }

	// Get the total number of interactions of a certain type
	NX_INLINE unsigned int getInteractionCount(PxInteractionType type) const { return interactions[type].size(); }

	// Get the number of active interactions of a certain type (an active interaction is an interaction that has at least one active actor)
	NX_INLINE unsigned int getActiveInteractionCount(PxInteractionType type) const  { return activeInteractionCount[type]; }

	// Get an iterator to all interactions of a certain type
	NX_INLINE PxIterator<PxInteraction*const> getInteractions(PxInteractionType type) const { return PxIterator<PxInteraction*const>(interactions[type].begin(), interactions[type].end()); }

	// Get an iterator to the active interactions of a certain type
	NX_INLINE PxIterator<PxInteraction*const> getActiveInteractions(PxInteractionType type) const { return PxIterator<PxInteraction*const>(interactions[type].begin(),&interactions[type][activeInteractionCount[type]]); }

	// Generate islands based on interaction connectivity
	void generateIslands(bool disableDeactivation);

	// Generate the island of a specific actor (does not activate or deactivate any actors, just build an island)
	void generateIslandFromActor(PxActor &actor);

	// Get number of active islands
	NX_INLINE unsigned int getActiveIslandCount() const { return activeIslandCount; }

	// Get the total number of islands
	NX_INLINE unsigned int getIslandCount() const { return islands.size(); }

	// Get number of actors in a specific island (island index from zero to getIslandCount()-1
	unsigned int getIslandActorCount(unsigned int islandIndex) const;

	NX_INLINE PxIterator<PxActor*const> getIslandActors(unsigned int islandIndex) const { 
		if (islandIndex==islands.size()-1)
			return PxIterator<PxActor*const>(&islandSortedActors[islands[islandIndex]], islandSortedActors.end()); 
		else
			return PxIterator<PxActor*const>(&islandSortedActors[islands[islandIndex]], &islandSortedActors[islands[islandIndex+1]]); 
	}

	// Get whether the island has contact with a static actor (Somewhat hackish for adaptive force, and only valid for active islands)
	bool islandHasStaticContact(unsigned int islandIndex) const { return staticContact[islandIndex]; }

	// Get the Low Level context
	NX_INLINE PxdContext &getLowLevelContext() { return llContext; }

	// Get the Low Level broadphase context
	NX_INLINE PxdContext &getLowLevelBroadPhaseContext() { return llBroadPhaseContext; }

private:
	//These are called from actor creation/destruction
	void registerActor(PxActor* actor);
	void unregisterActor(PxActor* actor);
	void registerInteraction(PxInteraction* interaction);
	void unregisterInteraction(PxInteraction* interaction);
	void notifyActorActiveChange(PxActor* actor);
	void notifyInteractionActiveChange(PxInteraction* interaction);

	void swapActorIds(NxU32 id1, NxU32 id2);
	void swapInteractionIds(NxU32 id1, NxU32 id2, PxInteractionType type);

	NxFoundation::NxArraySDK<PxActor*>			actors;
	NxU32										activeActorCount; // Actors with id < activeActorCount are active
	NxU32										activeActorLimit;

	NxFoundation::NxArraySDK<PxInteraction*>	interactions[PX_INTERACTION_TYPE_COUNT];
	NxU32										activeInteractionCount[PX_INTERACTION_TYPE_COUNT]; // Interactions with id < activeInteractionCount are active

	NxFoundation::NxArraySDK<PxActor*>			islandSortedActors;
	NxFoundation::NxArraySDK<unsigned int>		islands;
	NxU32										activeIslandCount;
	NxFoundation::NxArraySDK<bool>				staticContact;
	NxFoundation::NxArraySDK<PxActor*>			actorStack;

	PxdContext									llContext;
	PxdContext									llBroadPhaseContext;	// Usually same as normal context except when SW broadphase only is enabled

	NxU32										timestamp;
	NxU32										flags;
};

#endif //NX_FRAMEWORK_PXSCENE
