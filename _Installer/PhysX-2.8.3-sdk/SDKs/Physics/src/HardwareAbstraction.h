#ifndef NX_PHYSICS_HARDWARE_ABSTRACTION
#define NX_PHYSICS_HARDWARE_ABSTRACTION

#include "Nxp.h"
#include "NxUserRaycastReport.h"
#include "NxUserEntityReport.h"
#include "NxScene.h"
class NvFluid;
class NvFluidDesc;
class NvCloth;
class DeformableDesc;
class NvScene;
class NpScene;
class NxShape;
class NxBounds3;
class NpActor;
class NvShape;
class NvShapeDesc;
class FluidTri;
class NvRawTriangleMesh;
class NvFluidMeshCookingDesc;
class Actor;
class NxActorDescBase;
class RigidSceneZoner;
class HardwareAbstraction;
class NpShape;
class NxMaterialDesc;
class NxRay;
class NxSphere;
class NxBox;
class NxCapsule;
class NxPlane;
class NvJoint;
class NxUserContactReport;
class NxUserTriggerReport;
class NpForceFieldShapeGroup;
namespace NxFoundation 
	{
	class DebugRenderable;
	} 

//#include "NpPhysicsSDK.h"
#include "NxUserEntityReport.h"
#include "Array.h"
#include "NvScene.h"
#include "DebugRenderable.h"
#include "NxUserContactReport.h"
#include "Profiler.h"

//TODO: we probably want HSM on all platforms, including linux...
#if defined(WIN32) || defined(_XBOX) || defined(__CELLOS_LV2__) || defined(__PPCGEKKO__) || defined(CAFE) || defined(LINUX) || defined(__APPLE__) || defined(__native_client__)

//TODO: note arbitrary local tesselation of plane: 
//TODO: could use world bounds?? --> thats not always available.  Anyway, planes are not a game typical primitive anyway.
//Still, we'll need some sort of a param for this.
#define NX_PLANESIZE 1000.0f
//#define DIRECT_SPHERE_SUPPORT
//enabling these two settings kills our ability to keep dynamic client associations with NvMirrorManagerClient::addDynamicShape()

#define LAZY_MESH_RELEASE
//#define NX_EMIT_STATIC_BOXES_AS_DYNAMIC
//#define NX_EMIT_STATIC_BOXES_AS_DYNAMIC
#define NX_EMIT_STATIC_CONVEX_AS_TRIGS_FOR_FLUID
//#define NX_RBHW_SCENES_DONT_SUPPORT_COMPOUNDS	//no compound support in hardware yet
#define NX_RBHW_NUM_MATS_LIMIT	255				//hardware has a tiny-ass material table, don't try to create more cause it will crash.
//#define EMULATE_RB_SCENE_W_SW

//#define DISABLE_MIRRORING
//#define NX_EMIT_STATIC_BOXES_AS_DYNAMIC

//#define NX_EMIT_DYNAMIC_CONVEXES_AS_MESH	// mmf

#define FLUID_PRE_STANCING_AND_MORE


enum HSMProfileZones
	{
	//for debugging:
	PrHSM_OnSimulate,
	PrHSM_EndSimulate,
	PrHSM_ReleaseActor,
	PrHSM_ReleaseShape,
	PrHSM_updateAABBs,
	PrHSM_onReleaseHardwareObj,
	PrHSM_onEnterBounds,
	PrHSM_onLeaveBounds,
	PrHSM_zonerSync,
    PrHSMNumZones
	};

#include "NvMirrorManagerClient.h"
#include "SceneManager.h"
#include "MirrorManager.h"

class MetaZone : public NxProfileZone	//just to add a ctor.
	{
	public:
	MetaZone();
	void setName(const char * n) { name = n; }
	void reset();
	void noteSceneProfile(NxProfileZone & zone);
	};

class HardwareAbstraction
	{
	public:
	HardwareAbstraction();
	~HardwareAbstraction();

	//API events:
	void				initialize(NpScene * ownerScene);
	void				shutDown();

	NxCompartment *		createCompartment(const NxCompartmentDesc& desc);
	NxU32				getNbCompartments() const;
	NxU32				getCompartmentArray(NxCompartment ** userBuffer, NxU32 bufferSize, NxU32 & usersIterator) const;

	NvFluid*			createFluid(NxCompartment *, const NvFluidDesc& desc);
	void				releaseFluid(NvFluid*);
	bool				cookFluidMeshHotspot(NxCompartment* compartment, const NvFluidMeshCookingDesc& desc);

	NvCloth*			createCloth(const DeformableDesc& desc);
	void				releaseCloth(NvCloth*);

	NvScene *			getHardwareSceneForNewActor(const NxActorDescBase& desc);
	const NxProfileZone *getNamedZone(NxProfileZoneName) const;

	//events:
	void				onStartSim();
	void				onFetchResults();
	void				onSwapBuffers();
	void				onSetGravity()			{ sceneManager.onSetGravity(); }
	void				onSimulate(NxReal subStepSize, NxU32 numSubSteps, NxReal elapsedTime);
	void				endSimulate();
	void				onReleaseActor(NpActor &);
	void				onReleaseShape(NpShape &);
	void				onCreateMaterial(const NxMaterialDesc &);
	void				onReleaseMaterial(NxMaterialIndex);
	void				onChangeMaterial(NxMaterialIndex, const NxMaterialDesc &);

	void				onActorChange(NpActor* npActor, MirroredActorFlag flag);

	void				onForceFieldGroupCreate(NpForceFieldShapeGroup& group);
	void				onForceFieldGroupRelease(NpForceFieldShapeGroup& group);
	void				onForceFieldGroupChange(NpForceFieldShapeGroup& group);

	bool				raycastAnyBounds(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups, NxReal maxDist, const NxGroupsMask* groupsMask) const;
	bool				raycastAnyShape(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups, NxReal maxDist, const NxGroupsMask* groupsMask) const;
	NxU32				raycastAllBounds(const NxRay& worldRay, NxUserRaycastReport& report, NxShapesType shapesType, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask) const;
	NxU32				raycastAllShapes(const NxRay& worldRay, NxUserRaycastReport& report, NxShapesType shapesType, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask) const;
	NvShape *			raycastClosestBounds(const NxRay& worldRay, NxShapesType shapeType, NxRaycastHit& hit, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask, NvShape * closestSoFar) const;
	NvShape *			raycastClosestShape(const NxRay& worldRay, NxShapesType shapeType, NxRaycastHit& hit, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask, NvShape * closestSoFar) const;
	NxU32				overlapSphereShapes		(const NxSphere& worldSphere, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups, const NxGroupsMask* groupsMask, bool accurate_collision, NxU32 swCount);
	NxU32				overlapAABBShapes		(const NxBounds3& worldBounds, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups, const NxGroupsMask* groupsMask, bool accurate_collision, NxU32 swCount);
	NxU32				overlapOBBShapes		(const NxBox& worldBox, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups, const NxGroupsMask* groupsMask, bool accurate_collision, NxU32 swCount);
	NxU32				overlapCapsuleShapes	(const NxCapsule& worldCapsule, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups, const NxGroupsMask* groupsMask, bool accurate_collision, NxU32 swCount);
	NxU32				cullShapes				(NxU32 nbPlanes, const NxPlane* worldPlanes, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups, const NxGroupsMask* groupsMask, NxU32 swCount);
	NxU32				getNbStaticShapes		() const;
	NxU32				getNbDynamicShapes		() const;
	NxU32				getTotalNbShapes		() const;
	bool				checkOverlapSphere		(const NxSphere& worldSphere, NxShapesType shapeType, NxU32 activeGroups, const NxGroupsMask* groupsMask);
	bool				checkOverlapAABB		(const NxBounds3& worldBounds, NxShapesType shapeType, NxU32 activeGroups, const NxGroupsMask* groupsMask);
	bool				checkOverlapOBB			(const NxBox& worldBox, NxShapesType shapeType, NxU32 activeGroups, const NxGroupsMask* groupsMask);
	bool				checkOverlapCapsule		(const NxCapsule& worldCapsule, NxShapesType shapeType, NxU32 activeGroups, const NxGroupsMask* groupsMask);
	void				setGroupCollisionFlag	(NxCollisionGroup group1, NxCollisionGroup group2, bool);
	void				setActorGroupPairFlags	(NxActorGroup group1, NxActorGroup group2, NxU32);
	void				setDominanceGroupPair	(NxDominanceGroup group1, NxDominanceGroup group2, NxConstraintDominance & dominance);
	void				setFilterOps			(NxFilterOp op0, NxFilterOp op1, NxFilterOp op2);
	void				setFilterBool			(bool flag);
	void				setFilterConstant0		(const NxGroupsMask& mask);
	void				setFilterConstant1		(const NxGroupsMask& mask);
	NxU32				getNbJoints() const;
	NvJoint *			getJointList();
	NvJoint *			getNextJointList();
	void				purgeJoints();
	void				setUserContactReport	(NxUserContactReport* callback);
	void				setUserActorPairFiltering(NxUserActorPairFiltering* callback);
	void				setUserTriggerReport	(NxUserTriggerReport* callback);
	void				setUserNotify			(NxUserNotify* callback);
	NxU32				getNbPairs() const;
	NxU32				getPairFlagArray(NxPairFlag* userArray, NxU32 numPairs) const;

	NX_INLINE	SceneManager * getSceneManager() { return &sceneManager; }

	void				sendToVRD(bool create);
	void				removeFromVRD();

	void				visualize(NxFoundation::DebugRenderable & vis);
#ifdef NX_ENABLE_SCENE_STATS2
	NxSceneStats2*		getStats2(NxU32** eventIDs = NULL, NxU32** subsetStatIDs = NULL, NxU32* numSubsetStats = NULL) const { return sceneManager.readSceneStats2(eventIDs, subsetStatIDs, numSubsetStats); }
	void				swapStatBuffers() { sceneManager.swapStatBuffers(); };
#endif
	NxProfilerData*		readProfileData(bool clearData) { return sceneManager.readProfileData(clearData, profiler); }

	void				noteSceneProfile(NvScene *, NxProfileZone &);
	void				fetchMetaZones(NxFoundation::NxArraySDK< ::NxProfileZone> & profData, bool clearData);
	void				clearMetaZones();

	mutable NxFoundation::ProfilerManager		profiler;

	NxU64				clientTime;
	NxU64				totalTime;
	MetaZone			cpuSceneMetaZone;
	MetaZone			clientMetaZone;
	MetaZone			totalMetaZone;
	MetaZone			ppuSceneMetaZones[4];

	private:

	SceneManager		sceneManager;
	MirrorManager		mirrorManager;
	};

#else //!defined(NX_DISABLE_HARDWARE)

class SceneManager;

enum HSMProfileZones
{
	//for debugging:
	PrHSM_OnSimulate,
	PrHSM_EndSimulate,
	PrHSM_ReleaseActor,
	PrHSM_ReleaseShape,
	PrHSM_updateAABBs,
	PrHSM_onReleaseHardwareObj,
	PrHSM_onEnterBounds,
	PrHSM_onLeaveBounds,
	PrHSM_zonerSync,
	PrHSMNumZones
};

#include "NvMirrorManagerClient.h"
#include "SceneManager.h"
#include "MirrorManager.h"

//other platforms just stub out this class.  Could also do via virtual base class but this doesn't need dynamic binding.
class HardwareAbstraction
	{
	public:
	NX_INLINE void				initialize(NpScene * ownerScene)		{}
	NX_INLINE void				shutDown()								{}

	NX_INLINE NxCompartment *	createCompartment(const NxCompartmentDesc& desc)	{ return 0; }
	NX_INLINE NxU32				getNbCompartments() const { return 0; }
	NX_INLINE NxU32				getCompartmentArray(NxCompartment ** userBuffer, NxU32 bufferSize, NxU32 & usersIterator) const { return 0; }


	NX_INLINE NvFluid*			createFluid(NxCompartment *, const NvFluidDesc& desc)	{ return 0; }
	NX_INLINE void				releaseFluid(NvFluid*)					{}
	NX_INLINE bool				cookFluidMeshHotspot(NxCompartment* compartment, const NvFluidMeshCookingDesc& desc) { return false; }

	NX_INLINE NvCloth*			createCloth(const DeformableDesc& desc)	{ return 0; }
	NX_INLINE void				releaseCloth(NvCloth*)					{}

	NX_INLINE void				onStartSim()			{}
	NX_INLINE void				onFetchResults()			{}
	NX_INLINE void				onSwapBuffers()			{}
	NX_INLINE void				onSetGravity()			{}
	NX_INLINE void				onSimulate(NxReal subStepSize, NxU32 numSubSteps, NxReal elapsedTime)	{}
	NX_INLINE void				endSimulate() {}
	NX_INLINE void				onReleaseActor(NpActor &)		{}
	NX_INLINE void				onReleaseShape(NpShape &)		{}
	NX_INLINE void				onCreateMaterial(const NxMaterialDesc &)	{}
	NX_INLINE void				onReleaseMaterial(NxMaterialIndex)	{}
	NX_INLINE void				onChangeMaterial(NxMaterialIndex, const NxMaterialDesc &)	{}

	NX_INLINE void				onActorChange(NpActor* npActor, MirroredActorFlag flag) {}

	void						onForceFieldGroupCreate(NpForceFieldShapeGroup& group);
	void						onForceFieldGroupRelease(NpForceFieldShapeGroup& group);
	void						onForceFieldGroupChange(NpForceFieldShapeGroup& group);


	NX_INLINE bool				raycastAnyBounds(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups, NxReal maxDist, const NxGroupsMask* groupsMask) const { return false; }
	NX_INLINE bool				raycastAnyShape(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups, NxReal maxDist, const NxGroupsMask* groupsMask) const  { return false; }
	NX_INLINE NxU32				raycastAllBounds(const NxRay& worldRay, NxUserRaycastReport& report, NxShapesType shapesType, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask) const { return 0; }
	NX_INLINE NxU32				raycastAllShapes(const NxRay& worldRay, NxUserRaycastReport& report, NxShapesType shapesType, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask) const { return 0; }
	NX_INLINE NvShape *			raycastClosestBounds(const NxRay& worldRay, NxShapesType shapeType, NxRaycastHit& hit, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask, NvShape * closestSoFar) const { return closestSoFar; }
	NX_INLINE NvShape *			raycastClosestShape(const NxRay& worldRay, NxShapesType shapeType, NxRaycastHit& hit, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask, NvShape * closestSoFar) const  { return closestSoFar; }
//	NX_INLINE NxU32				overlapAABBTriangles	(const NxBounds3& worldBounds, NxFoundation::NxArraySDK<NxTriangle>& worldTriangles) { return 0; }
	NX_INLINE NxU32				overlapSphereShapes		(const NxSphere& worldSphere, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups, const NxGroupsMask* groupsMask, bool accurate_collision, NxU32 swCount) { return swCount; }
	NX_INLINE NxU32				overlapAABBShapes		(const NxBounds3& worldBounds, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups, const NxGroupsMask* groupsMask, bool accurate_collision, NxU32 swCount) { return swCount; }
	NX_INLINE NxU32				overlapOBBShapes		(const NxBox& worldBox, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups, const NxGroupsMask* groupsMask, bool accurate_collision, NxU32 swCount) { return swCount; }
	NX_INLINE NxU32				overlapCapsuleShapes	(const NxCapsule& worldCapsule, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups, const NxGroupsMask* groupsMask, bool accurate_collision, NxU32 swCount) { return swCount; }
	NX_INLINE NxU32				cullShapes				(NxU32 nbPlanes, const NxPlane* worldPlanes, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups, const NxGroupsMask* groupsMask, NxU32 swCount) { return swCount; }
	NxU32						getNbStaticShapes		() const { return 0; }
	NxU32						getNbDynamicShapes		() const { return 0; }
	NxU32						getTotalNbShapes		() const { return 0; }
	NX_INLINE bool				checkOverlapSphere		(const NxSphere& worldSphere, NxShapesType shapeType, NxU32 activeGroups, const NxGroupsMask* groupsMask) { return false; }
	NX_INLINE bool				checkOverlapAABB		(const NxBounds3& worldBounds, NxShapesType shapeType, NxU32 activeGroups, const NxGroupsMask* groupsMask) { return false; }
	NX_INLINE bool				checkOverlapOBB			(const NxBox& worldBox, NxShapesType shapeType, NxU32 activeGroups, const NxGroupsMask* groupsMask) { return false; }
	NX_INLINE bool				checkOverlapCapsule		(const NxCapsule& worldCapsule, NxShapesType shapeType, NxU32 activeGroups, const NxGroupsMask* groupsMask) { return false; }
	NX_INLINE void				setGroupCollisionFlag	(NxCollisionGroup group1, NxCollisionGroup group2, bool) {}
	NX_INLINE void				setDominanceGroupPair	(NxDominanceGroup group1, NxDominanceGroup group2, NxConstraintDominance & dominance) {}
	NX_INLINE void				setActorGroupPairFlags	(NxActorGroup group1, NxActorGroup group2, NxU32){}
	NX_INLINE void				setFilterOps			(NxFilterOp op0, NxFilterOp op1, NxFilterOp op2){}
	NX_INLINE void				setFilterBool			(bool flag){}
	NX_INLINE void				setFilterConstant0		(const NxGroupsMask& mask){}
	NX_INLINE void				setFilterConstant1		(const NxGroupsMask& mask){}
	NX_INLINE NxU32				getNbJoints() const { return 0;}
	NX_INLINE NvJoint *			getJointList() {return NULL;}
	NX_INLINE NvJoint *			getNextJointList() {return NULL;}
	NX_INLINE void				purgeJoints() {}
//	NX_INLINE void				setShapePairFlags(NvShape &, NvShape &, NxU32 flags) {}
//	NX_INLINE NxU32				getShapePairFlags(NvShape &, NvShape &) const { return 0;}
	NX_INLINE NxU32				getNbPairs() const { return 0;}
	NX_INLINE NxU32				getPairFlagArray(NxPairFlag* userArray, NxU32 numPairs) const { return 0; }


	NX_INLINE void				setUserContactReport	(NxUserContactReport* callback){}
	NX_INLINE void				setUserActorPairFiltering	(NxUserActorPairFiltering* callback) {}
	NX_INLINE void				setUserTriggerReport	(NxUserTriggerReport* callback){}
	NX_INLINE void				setUserNotify			(NxUserNotify* callback){}

#ifdef NX_ENABLE_SCENE_STATS2
	NxSceneStats2*		getStats2(NxU32** eventIDs = NULL, NxU32** subsetEventIDs = NULL, NxU32* numSubsetEvents = NULL) const { return 0; }
	void				swapStatBuffers() { };
#endif
	NX_INLINE void				visualize(NxFoundation::DebugRenderable & vis)		{}
	NX_INLINE NvScene *			getHardwareSceneForNewActor(const NxActorDescBase& desc) {return 0;}
	NX_INLINE const NxProfileZone *getNamedZone(NxProfileZoneName) const				{ return 0; }
	NX_INLINE SceneManager *	getSceneManager() { return NULL; }

	void						sendToVRD(bool create) {}
	void						removeFromVRD() {}

	NX_INLINE NxProfilerData*		readProfileData(bool clearData) 
	{
		NX_ASSERT(!"HardwareAbstraction::readProfileData() should not be called on platforms without hardware support");
		return NULL;
	}
	};

#endif


#endif
