#ifndef __NPHASE_CORE__
#define __NPHASE_CORE__

#include "Nxp.h"
#include "Allocateable.h"
#include "NxMutex.h"
#include "NxInterlocked.h"
#include "NxUserContactReport.h"
#include "TriggerPairs.h"
#include "Px.h"
#include "NxThread.h"

struct NvVRDContact;
struct NvVRDTrigger;
class NvVRDReporter;
class NvUserRawBoundsReport;
class Scene;
class Shape;
#if NX_USE_FLUID_API
class FluidPacketShape;
#endif
class CoreElement;
class CoreInteraction;
class RbElementInteraction;
class ShapeInstancePair;
class ShapeInstancePairHL;
class ShapeInstancePairLL;
class ElementInteractionMarker;
class TriggerInteraction;
class NPPoolManager;
class NPhaseContext;
class ActorPair;
class ActorPairContactReportData;
class PageBoundsElement;
class PageBoundsInteraction;
#if NX_USE_FLUID_API
class FluidElementRbElementInteraction;
#endif
class RawBoundsVolume;
class SIPContactReportData;
class PxVolume;
class PxVolumeOverlap;
class ContactStream;


struct UserActorPairFilterInternal
{
	PxInteractionType pairType;
	RbElementInteraction* pair;
	Shape* shape0;
	Shape* shape1;
};


typedef void (*NPhasePurgeCallback)(ShapeInstancePair* pair, void* userData);

// Internal counterpart of NxContactPair
// The size of this structure needs to be a multiple of 4bytes because it is written to a buffer which
// is based on 4byte blocks.
struct ContactPair
{
	NX_INLINE void copyTo(NxContactPair& pair)
	{
		pair.actors[0]			= actors[0];
		pair.actors[1]			= actors[1];
		pair.stream				= stream;
		pair.sumNormalForce		= sumNormalForce;
		pair.sumFrictionForce	= sumFrictionForce;
		pair.isDeletedActor[0]	= (isDeletedActor[0] != 0);
		pair.isDeletedActor[1]	= (isDeletedActor[1] != 0);
	}

	NxActor*				actors[2];
	const NxU32*			stream;
	NxVec3					sumNormalForce;
	NxVec3					sumFrictionForce;
	NxU16					isDeletedActor[2];
	//40
};

// Helper class. An ActorPair instance will only be allowed to occur in the list once.
// Note: Do not add elements once getNext() is called. 
//		 There should only exist one ActorPairSet at a time.
class ActorPairSet
{
public:
	ActorPairSet()
		: root(NULL),
		  last(NULL),
		  currentIt(NULL),
		  lastIt(NULL)
	{
	}
	~ActorPairSet()
	{
	} // Unlink all actor pairs
	bool add(ActorPair* aPair);
	ActorPair* getNext();
	NX_INLINE void reset()
	{
		while (getNext());
		root = last = currentIt = lastIt = NULL;
	}
private:
	ActorPair* root;
	ActorPair* last;
	ActorPair* currentIt;
	ActorPair* lastIt;
};

/*
Description: NPhaseCore encapsulates the near phase processing to allow multiple implementations(eg threading and non
threaded).

The broadphase inserts shape pairs into the NPhaseCore, which are then processed into contact point streams.
Pairs can then be processed into AxisConstraints by the GroupSolveCore.

*/
class NPhaseCore : public NxFoundation::NxAllocateable
{
public:
	NPhaseCore(Scene* scene);
	~NPhaseCore();

	void onOverlapCreated(PxVolume* volume0, PxVolume* volume1);
	void onOverlapRemoved(PxVolume* volume0, PxVolume* volume1);
	void onVolumeRemoved(PxVolume* volume);

#if NX_USE_FLUID_API
	void insertFluidElementRbElementPair(FluidPacketShape* fluidShape, Shape* rbShape);
	void releaseFluidElementRbElementPair(FluidElementRbElementInteraction* pair);
#endif

	void fireUserFilteringCallbacks();

	void addToDirtyInteractionList(CoreInteraction* interaction);
	bool removeFromDirtyInteractionList(CoreInteraction* interaction);
	void updateDirtyInteractions();


	/*
	  Description: Perform/Complete processing of shape instance pairs into contacts streams. Also
	  links shape instance pairs into bodies to allow the later retrieval of the contacts.
	 */
	void narrowPhase();

	/*
	  Description: Start processing of pairs inserted/touched during the second broadphase pass for CCD.
	 */
	void beginCCDPhase();

	/*
	  Description: Completes/Performs CCD processing of shape pairs.
	 */
	void doCCDMotions();


	/*
	  Description: Calls contact callbacks or adds contacts to contactReport buffer to be fed to the user.
	  
	  If contactReport==NULL then contacts are inserted into contactReportBuffer otherwise contactReport is used
	  to pass them directly to the user.
	 */
	void callContactCallbacks(NxUserContactReport* contactReport, Container& contactReportBuffer);

	void callContactCallback(NxUserContactReport* contactReport, Container& contactReportBuffer, ActorPair* pair,
							 NxU32 triggeredFlags, bool useCache);

	/*
	  Description: Take trigger pairs generated during collision detection and insert them into triggerBuffer.
	 */
	void updateTriggerCallbackBuffer(TriggerBuffer& triggerBuffer);
	/*
	  Description: Remove all trigger pairs which reference shape.
	 */
	void purgeTriggerPair(Shape* shape);
	/*
	  Description: Displays visualizations associated with the near phase.
	 */
	void visualize(NxU32 timeStamp, NxFoundation::DebugRenderable& d);

	/*
	  Description: Retrieves debugging information for the visual remote debugger.
	 */
	void getVRDData(NvVRDReporter* reporter);

	/*
	  Description: Add trigger debugging data to the VRD data.
	 */
	void addTriggerData(TriggerBuffer& triggerBuffer);
	/*
	  Description: Called when a new mesh instance is added to the scene, This ensures that all NPhaseContexts are
	  notified(for each thread) and resize appropriately.
	 */
	void growMeshTags(NxU32 nbStamps);
	void growObjectTags(NxU32 nbStamps);

	/**
	  Description: retrieve the thread context for this thread(ie each thread has its own set of colliders)
	  Moved from NPhaseCoreMulti to handle raycasts in parallel(who also want the colliders)
	 */
	NPhaseContext* getContext();
	void putContext(NPhaseContext* context);

	NX_INLINE Scene* getScene();

	/*
	  Description: Generate contacts for a specific shape instance pair.
	  
	  Threading: Should be thread safe as it can be called from multiple threads by 
	  threaded implementations.
	 */
	void findPairContacts(ShapeInstancePairHL* sip, NPhaseContext& context);
	void findTriggerContacts(TriggerInteraction* tri, NPhaseContext& context);

	void sortPools();

	void addToActiveSlowPairList(ShapeInstancePair* sip);
	void removeFromActiveSlowPairList(ShapeInstancePair* sip);
	NX_INLINE NxU32 getActiveSlowPairCount() const;
	NX_INLINE ShapeInstancePair*const * getActiveSlowPairs() const;
	static bool isInActiveSlowPairList(const ShapeInstancePair* sip);

	NX_INLINE void setRawBoundsReport(NvUserRawBoundsReport* rbr);
	NX_INLINE void setUserActorPairFiltering(NxUserActorPairFiltering* callback);
	NX_INLINE NxUserActorPairFiltering* getUserActorPairFiltering()	const;

	SIPContactReportData* createSIPContactReportData() const;
	void releaseSIPContactReportData(SIPContactReportData* data);

	ActorPairContactReportData* createActorPairContactReportData() const;
	void releaseActorPairContactReportData(ActorPairContactReportData* data);
protected:


	/*
	  Description: insert trigger pairs gathered in the context into the trigger pair buffer.
	 */
	void insertTriggerPairs(NPhaseContext& context);

private:
	void doCCDShapeInstancePair(ShapeInstancePair* sip);
	void computePairEvents(ActorPair* pair, NxU32& pairEvents, bool& useCache);

	void onPageBoundsOverlapCreated(PageBoundsElement& pageBoundsElement, Shape& shape);
	void onPageBoundsOverlapRemoved(PageBoundsInteraction* pbi, PageBoundsElement* pbe);

	void onRawBoundsOverlapCreated(RawBoundsVolume& rawBoundsElement, PxVolume& volume);
	void onRawBoundsOverlapRemoved(RawBoundsVolume& rawBoundsElement, PxVolume& volume);
	void onRawBoundsOverlapRemoved(PxVolumeOverlap& overlap);

	void insertRbElementPair(Shape* shape0, Shape* shape1);
	void releaseElementPair(ElementInteraction* pair);
	void releaseShapeInstancePair(ShapeInstancePair* pair);

	ElementInteraction* createRbElementInteraction(Shape& s0, Shape& s1, ActorPair* aPair);
	ShapeInstancePairHL* createShapeInstancePairHL(Shape& s0, Shape& s1, ActorPair* aPair);
	ShapeInstancePairLL* createShapeInstancePairLL(Shape& s0, Shape& s1, ActorPair* aPair);
	PageBoundsInteraction* createPageBoundsInteraction(Shape& s0, Shape& s1, ActorPair* aPair);
	TriggerInteraction* createTriggerInteraction(Shape& s0, Shape& s1);
	ElementInteractionMarker* createElementInteractionMarker(CoreElement& e0, CoreElement& e1);

	void updatePair(CoreInteraction* pair);
	ElementInteraction* convert(ElementInteraction* pair, PxInteractionType type);

	void addUserCallbackFilterRbElementPair(Shape* shape0, Shape* shape1);
	void removeFromUserCallbackFilterList(CoreInteraction* pair);

	void convertDeletedShapesInContactStream(ContactStream& stream);

	ActorPair* findActorPair(Shape* s0, Shape* s1);

	NxInterlocked::SListHeader contextRoot;
	Scene* ownerScene;
	NxU32 maxMeshTags;
	NxU32 maxObjectTags;
	NxTLSId tlsSlot;

	TriggerPairs triggerPairs;
	NxFoundation::NxArraySDK<NvVRDTrigger> debugTriggerList;
	NX_BOOL inCCDPhase;
	ActorPairSet actorPairSet;
	NPPoolManager* poolManager;
	NxFoundation::NxArraySDK<ShapeInstancePair*> slowPairList;
	NxFoundation::NxArraySDK<NxActorPairFilter> userActorPairFilter;
	NxFoundation::NxArraySDK<UserActorPairFilterInternal> userActorPairFilterInternal;
	NxFoundation::NxArraySDK<CoreInteraction*> dirtyInteractions;
	NxUserActorPairFiltering* userActorPairFiltering;
	NvUserRawBoundsReport* userRawBoundsReport;
};

//////////////////////////////////////////////////////////////////////////
NX_INLINE Scene* NPhaseCore::getScene()
{
	return ownerScene;
}

NX_INLINE NxU32 NPhaseCore::getActiveSlowPairCount() const
{
	return slowPairList.size();
}
NX_INLINE ShapeInstancePair*const * NPhaseCore::getActiveSlowPairs() const
{
	return &slowPairList[0];
}

NX_INLINE void NPhaseCore::setRawBoundsReport(NvUserRawBoundsReport* rbr)
{
	userRawBoundsReport = rbr;
}
NX_INLINE void NPhaseCore::setUserActorPairFiltering(NxUserActorPairFiltering* callback)
{
	userActorPairFiltering = callback;
}
NX_INLINE NxUserActorPairFiltering* NPhaseCore::getUserActorPairFiltering() const
{
	return userActorPairFiltering;
}

#endif
