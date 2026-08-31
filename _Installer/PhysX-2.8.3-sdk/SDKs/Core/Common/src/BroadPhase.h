#ifndef NX_COLLISION_BROADPHASE
#define NX_COLLISION_BROADPHASE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "Allocateable.h"
#include "IDPool.h"
#include "RawBoundsVolume.h"
#include "SimplePool.h"

//namespace NxCollision {

class NxUserTriggerReport;
class NxUserContactReport;

class NxShape;
class PairManager;
class NPhaseCore;
class NvUserRawBoundsReport;

class Shape;
class Scene;
class ShapeInstancePair;
class PageBoundsElement;
class TriangleMeshShape;
class RawBoundsVolume;
namespace NxFoundation
{
class DebugRenderable;
}

#ifdef NX_FLUID_IN_PRIMARY_SCENE
class FluidPacketShape;
#endif	// NX_FLUID_IN_PRIMARY_SCENE

struct NxPair
{
	NX_INLINE NxPair(Shape* _s0, Shape* _s1)
		: s0(_s0),
		  s1(_s1) {}
	Shape* s0;
	Shape* s1;
};

struct PruningInfos
{
	udword nbTailStatic;
	udword nbTailDynamic;
	udword nbBoxes;
	Opcode::Prunable** staticObjects;
	Opcode::Prunable** dynamicObjects;
};

/*
enum OpcodeSAP
{
OPC_SAP_NONE,
OPC_SAP_QUADRATIC,
OPC_SAP_RADIX,
OPC_SAP_COHERENT,
};
*/

struct InternalCallbackData
{
	PruningInfos* infos;
	PairManager* pairManager;
	NPhaseCore* nphaseCore;
	Scene* scene;
	NxU32 currentTimeStamp;
};

//typedef NX_BOOL (*NxPairCallback)	(const NxPair& pair, void* userData);

enum BPFlag
{
	BP_UPDATE_LOCK		= (1 << 0),	//!< If enabled, the update list is locked and shouldn't be modified
};

class BroadPhase : public NxFoundation::NxAllocateable
{
protected:
	BroadPhase();
	~BroadPhase();
	void prepareOverlapBuffer(NxU32 nrOverlaps, bool mayDecreaseSize);
public:
	NX_INLINE void setNPhaseCore(NPhaseCore* npc);
	void addActor(Shape&, bool dynamic);
#ifdef NX_FLUID_IN_PRIMARY_SCENE
	void addActor(FluidPacketShape&);
#endif
	NvRawBounds* addRawBounds(Scene&, const NxBounds3& bounds, bool forceField=false);
	PageBoundsElement* addPageBounds(TriangleMeshShape& meshShape, NxU32 pageIndex);
	void removeActor(Shape&);
#ifdef NX_FLUID_IN_PRIMARY_SCENE
	void removeActor(FluidPacketShape&);
#endif
	void removeRawBounds(NvRawBounds&);
	void removePageBounds(PageBoundsElement&);

	void updateVolumes(Scene&);
	void updateRawBounds(NvRawBounds& rawBounds, const NxBounds3& bounds);
	void startBroadPhase(Scene&);
	void finishBroadPhase(Scene&);
	void updateBroadPhase(Shape& shape, bool ccd);
	void cleanup();
	void visualize(NxFoundation::DebugRenderable&);
	void addToUpdateList(Shape* shape, bool persistent);
	void removeFromUpdateList(Shape* shape, bool persistent);

	NX_INLINE void addToVolumeRecreateList(Shape* shape);
	NX_INLINE NxU32 createBroadPhaseGroupId();
	NX_INLINE void releaseBroadPhaseGroupId(NxU32 id);
	static bool useHlBoundsUpdate(const Shape& shape);

private:
	NPhaseCore* nphaseCore;
private:
	NxU32 flags;

	ContainerSizeT updateList;		// TODO: PT: do that better !!!

	void* overlapReportBuffer;
	NxU32 overlapReportBufferSize;
	NxU32 overlapReportCount;
	IDPool groupIdPool;
	NxU32 staticShapeBroadphaseGroup;
	NxU32 rawBoundsBroadphaseGroup;
	NxU32 forceFieldsBroadphaseGroup;
	NxU32 fluidShapeBroadphaseGroup;

	NxFoundation::NxArraySDK<Shape*> volumeRecreateList;
	NxFoundation::NxArraySDK<RawBoundsVolume*> rawBoundsDeathrow;
	SimplePool<RawBoundsVolume> rawBoundsPool;


	friend	class Scene;	// the only one allowed to create broadphases
	friend	class Sync;
};

//////////////////////////////////////////////////////////////////////////

NX_INLINE void BroadPhase::setNPhaseCore(NPhaseCore* npc)
{
	nphaseCore = npc;
}

NX_INLINE void BroadPhase::addToVolumeRecreateList(Shape* shape)
{
	volumeRecreateList.pushBack(shape);
}
NX_INLINE NxU32 BroadPhase::createBroadPhaseGroupId()
{
	NxU32 id = groupIdPool.getNewID();
	NX_ASSERT(id<(1<<16));
	return id;
}
NX_INLINE void BroadPhase::releaseBroadPhaseGroupId(NxU32 id)
{
	groupIdPool.freeID(id);
}

//}
#endif
