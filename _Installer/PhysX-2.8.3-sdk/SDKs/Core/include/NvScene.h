#ifndef NX_PHYSICS_NV_SCENE
#define NX_PHYSICS_NV_SCENE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "NxSceneDesc.h"		//may want to extract just the enums later
#include "NxUserRaycastReport.h"
#include "NxUserEntityReport.h"
#include "NxTriangle.h"
#include "NxShape.h"
#include "Array.h"
#include "NxBounds3.h"
#include "NvMaterial.h"
#include "NvJoint.h"
#include "DebugRenderable.h"
#include "NxSceneStats2.h"
#include "NvSceneStats2.h"

#define TDMARK_PROFILING				0
#define TDMARK_NUM_HW_SCENES			4
#define TDMARK_SW_SCENES_ENABLED		1
#define TDMARK_PROFILING_FLUIDS_ENABLED	1
#define TDMARK_PROFILING_CLOTH_ENABLED	1

#define LOG_3DMARK_SIM_FETCH_CALLS		0

class NvJoint;
class NxSceneLimits;
class Body;
class NvShape;
class NvFluid;
class NxFluid;
class NvFluidDesc;
class NvSpringAndDamperEffector;
class PairManager;
class NxRay;
class NxSphere;
class NxCapsule;
class NxPlane;
class NpScene;
class NvBody;
class NvJoint;
class NxSceneStats;
class NxMaterial;
class NxPairFlag;
class NvConvexMesh;
class NvTriangleMesh;
class NvBodyDesc;
class NvQueuedEvent;
class NpShape;
class NxProfilerData;
class NvShapeDesc;
class NvVRDReporter;
class NvCloth;
class DeformableDesc;
class NxScheduler;
class NvMirrorManager;
class NvMirrorManagerMeshClient;
class NvFluidMeshCookingDesc;
class NvUserRawBoundsReport;
class NvRawTriangleMesh;
class ForceFieldManager;
class ConvexMesh;
class NxUserActorPairFiltering;
struct NxActiveTransform;
class PxScene;
#if NX_SUPPORT_SWEEP_API
struct NxSweepQueryHit;
class ObjectCache;
class NxSweepCache;
class AgPPUConnection;
class NpCompartment;
//class PrunedObjects;
//class Opcode::PrunedObjects;
#include "Opcode.h"
#undef Log
#endif
// BATCHED_RAYCASTS
class NxSceneQuery;
class NxSceneQueryDesc;
//~ BATCHED_RAYCASTS

enum NvSceneType
	{
	NX_SCENE_TYPE_PRIMARY			= 0,	//!< Specifies the primary scene.
	NX_SCENE_TYPE_HW_RB				= 1,	//!< Specifies a rigid body hardware scene.
	NX_SCENE_TYPE_HW_FLUID			= 2,	//!< Specifies a fluid hardware scene.
	NX_SCENE_TYPE_HW_CLOTH			= 3,	//!< Specifies a cloth hardware scene.
	NX_SCENE_TYPE_SW_FLUID			= 4,	//!< Specifies a fluid software scene.
	NX_SCENE_TYPE_SW_CLOTH			= 5,	//!< Specifies a cloth software scene.
	};

class NvSceneDesc
	{
	public:
	NxVec3					gravity;
//	NxUserNotify*			userNotify;
	NxUserTriggerReport*	userTriggerReport;
	NxUserContactReport*	userContactReport;
	NxUserActorPairFiltering* userActorPairFiltering;
	NvUserRawBoundsReport*  userRawBoundsReport;
	NxBounds3				maxBounds;
	NxSceneLimits			limits;
	NxU32					upAxis;
	NxU32					subdivisionLevel;
	NxPruningStructure		dynamicStructure;
	NxPruningStructure		staticStructure;
	NxU32					dynamicTreeRebuildRateHint;

	NxBroadPhaseType		bpType;
	NxU32					nbGridCellsX;		//!< Must be power of two
	NxU32					nbGridCellsY;		//!< Must be power of two

	NxVec3					bpOrigin;

	NxU32					solverBatchSize;

	NxScene*				nxScene;
	NxReal					subStepSize;
	NxU32					flags;		//of type NxSceneFlags
	NvMaterialDesc			defaultMaterial;

	NxUserScheduler*		scheduler;
	NxU32                   numInternalThreads;

	//new for HSM:
	NvMirrorManager*		mirrorManager;
	AgPPUConnection * 		connection;		//backdoor hack for context sharing
	NxU32					PPUIndex;		//TODO: merge with above or something.
	};


	enum RawBoundsType
	{
		RB_UNDEFINED,
		RB_FLUID_PACKET_BOUNDS,			// Fluid packet of a compartment
#ifdef NX_FLUID_IN_PRIMARY_SCENE
		RB_FLUID_PACKET_SHAPE,			// Fluid packet of the primary scene
#endif
		RB_FORCE_FIELD,
		RB_CLOTH_HW,
		RB_CLOTH_SW,
		RB_SOFTBODY_HW,
		RB_SOFTBODY_SW,

		RB_FORCE_DWORD	= 0x7fffffff
	};

	typedef void	(*PreBPCallback)	(void* user_data);
	typedef void	(*PostBPCallback)	(void* user_data);



class NvRawBounds
	{
	public:
	NX_INLINE						NvRawBounds() : type(RB_UNDEFINED), userData(NULL)	{								}
	NX_INLINE		RawBoundsType	getType()									const	{ return type;					}
	NX_INLINE		void*			getUserData()								const	{ return userData;				}
	NX_INLINE		void			init(void* ud, RawBoundsType rbt)					{ type = rbt; userData = ud;	}

	private:
	RawBoundsType	type;
	void*			userData;
	};


class NvUserRawBoundsReport
	{
	public:
	virtual void onEnterBounds(NvRawBounds&, NvShape&) = 0;
	virtual void onLeaveBounds(NvRawBounds&, NvShape&) = 0;
	virtual void onEnterBounds(NvRawBounds&, NvRawBounds&) = 0;
	virtual void onLeaveBounds(NvRawBounds&, NvRawBounds&) = 0;
	};


class NvScene
	{
	public:

	NX_INLINE							NvScene() : mPreBPCB(NULL), mPostBPCB(NULL), mCBUserData(NULL)	{}

	virtual	void						release() = 0;		//virtual destructor so NpScene can easily delete.  Not really a destructor so this object can be in a separate heap if needed.

	virtual NxScene*					getNxScene() = 0;
	virtual NpCompartment*				getCompartment() = 0; //is NULL for the main scene (doesn't have a compartment)
	virtual void						setCompartment(NpCompartment* compartment) = 0;

	virtual NxU32						getSimType() = 0;

	virtual NxU32						getSceneType() = 0;
	virtual void						setSceneType(NvSceneType sceneType) = 0;

	virtual NxU32						getPPUIndex() = 0;

	virtual NvMirrorManager*			getMirrorManager() = 0;
	virtual void						setMirrorManager(NvMirrorManager*) = 0;
	virtual void						notifyReleasePermanentlyMirroredShape(NvShape *shape) = 0;

	virtual void						setGravity(const NxVec3&) = 0;
	virtual NxVec3						getGravity() const = 0;

	virtual void						setSubStepSize(NxReal substepSize) = 0;
	virtual NxReal						getSubStepSize() const = 0;

	virtual NvShape*					createShape(const NvShapeDesc &) = 0;

	virtual NxU32						getNbStaticShapes()			const = 0; //mScene->getBroadPhase().getNbStaticShapes();
	virtual NxU32						getNbDynamicShapes()		const = 0; //mScene->getBroadPhase().getNbDynamicShapes();
	virtual NxU32						getTotalNbShapes()			const = 0;//mScene->getBroadPhase().getNbDynamicSubShapes();

	virtual NvBody *					createBody(const NvBodyDesc&, NvShape *) = 0;

	virtual NvJoint *					createJoint(const NvJointDesc &, const NvJointPrismaticDesc &)    = 0;
	virtual NvJoint *					createJoint(const NvJointDesc &, const NvJointCylindricalDesc &)  = 0;
	virtual NvJoint *					createJoint(const NvJointDesc &, const NvJointPointOnLineDesc &)  = 0;
	virtual NvJoint *					createJoint(const NvJointDesc &, const NvJointPointOnPlaneDesc &) = 0;
	virtual NvJoint *					createJoint(const NvJointDesc &, const NvJointFixedDesc &)        = 0;
	virtual NvJoint *					createJoint(const NvJointDesc &, const NvJointRevoluteDesc &)     = 0;
	virtual NvJoint *					createJoint(const NvJointDesc &, const NvJointSphereicalDesc &)   = 0;
	virtual NvJoint *					createJoint(const NvJointDesc &, const NvJointDistanceDesc &)     = 0;
	virtual NvJoint *					createJoint(const NvJointDesc &, const NvJointPulleyDesc &)       = 0;
	virtual NvJoint *					createJoint(const NvJointDesc &, const NvJointD6Desc &)           = 0;

	virtual void						removeJoint(NvJoint*) = 0;
	virtual NxU32						getNbJoints()		const = 0;
	virtual NvJoint *					getJointList() = 0;
	virtual NvJoint *					getDeadJointList() = 0;

	virtual NvSpringAndDamperEffector * createSpringAndDamperEffector() = 0; 
	virtual void						removeEffector(NvSpringAndDamperEffector*) = 0;
	virtual NxU32						getNbEffectors()		const = 0;
	virtual NvSpringAndDamperEffector *	getEffectorList() = 0;

	virtual	NvMaterial *				createMaterial(const NvMaterialDesc &, NxMaterialIndex * indexDest) = 0;
	virtual	NvMaterial *				getMaterial(NxMaterialIndex) = 0;	//returns a 0 if the index is invalid or out of range.
	virtual NxU32						getNxMaterialArray(NxMaterial ** userBuffer, NxU32 bufferSize, NxU32 & iterator) = 0;
	virtual	void						releaseMaterial(NvMaterial * m) = 0;
	virtual NxMaterialIndex				getHighestMaterialIndex() const = 0; 

	virtual void						setUserTriggerReport(NxUserTriggerReport* callback) = 0;
	virtual void						setUserContactReport(NxUserContactReport* callback) = 0;
	virtual void						setUserActorPairFiltering(NxUserActorPairFiltering* callback) = 0;

	// MSS HACK EARLY BROADPHASE UPDATE
	virtual void						earlyUpdateBroadphase() = 0;
	virtual void						simulate(NxU32 numSubSteps, NxU32 batchStepCount) = 0;
	virtual void						endSimulation() = 0;
	virtual void						flushStream() = 0;                            
	virtual void						fireCallBacks() = 0;							//callbacks that are fired on the core side.
	virtual NvQueuedEvent*				getQueuedEvents() = 0;							//callbacks that have to be fired on the Np side. (because they delete objs after the call)
	virtual void						swapBuffers(NxU32 *error) = 0; 



	virtual void						getLimits(NxSceneLimits& limits) const = 0;

	virtual void						setShapePairFlags(NvShape&, NvShape&, NxU32 nxContactPairFlag) = 0;
	virtual NxU32						getShapePairFlags(NvShape&, NvShape&) const = 0;
	virtual	void						removePairFlags(NvShape& s) = 0;
	virtual	NxU32						getPairFlagArray(NxPairFlag* userArray, NxU32 numPairs) const = 0;


	virtual NxUserTriggerReport*		getUserTriggerReport() const = 0;
	virtual NxUserContactReport*		getUserContactReport() const = 0;
	virtual NxUserActorPairFiltering*	getUserActorPairFiltering() const = 0;
	
	virtual void *						getInternal() { return 0; }

	virtual	NxFoundation::DebugRenderable&	getDebugRenderable() = 0;
	virtual	NxDebugRenderable			getVisualization() = 0;
	virtual NxProfilerData *			readProfileData(bool clearData) { return 0; }


	// Raycasting
	virtual bool						raycastAnyBounds		(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, const NxGroupsMask* groupsMask=NULL) const = 0;
	virtual bool						raycastAnyShape			(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, const NxGroupsMask* groupsMask=NULL, NxShape** cache=NULL) const = 0;
	virtual NxU32						raycastAllBounds		(const NxRay& worldRay, NxUserRaycastReport& report, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL) const = 0;
	virtual NxU32						raycastAllShapes		(const NxRay& worldRay, NxUserRaycastReport& report, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL) const = 0;
	virtual NvShape*					raycastClosestBounds	(const NxRay& worldRay, NxShapesType shapesType, NxRaycastHit& hit, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL) const = 0;
	virtual NvShape*					raycastClosestShape		(const NxRay& worldRay, NxShapesType shapesType, NxRaycastHit& hit, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL, NxShape** cache=NULL) const = 0;
	// Collision detection
	virtual NxU32						overlapAABBTriangles	(const NxBounds3& worldBounds, NxFoundation::NxArraySDK<NxTriangle>& worldTriangles) = 0;
	virtual NxU32						overlapSphereShapes		(const NxSphere& worldSphere, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, bool accurate_collision=false) = 0;
	virtual NxU32						overlapAABBShapes		(const NxBounds3& worldBounds, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, bool accurate_collision=false) = 0;
	virtual NxU32						overlapOBBShapes		(const NxBox& worldBox, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, bool accurate_collision=false) = 0;
	virtual NxU32						overlapCapsuleShapes	(const NxCapsule& worldCapsule, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, bool accurate_collision=false) = 0;
	virtual NxU32						cullShapes				(NxU32 nbPlanes, const NxPlane* worldPlanes, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL) = 0;

	// Placement API
	virtual bool						checkOverlapSphere		(const NxSphere& worldSphere, NxShapesType shapeType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL) = 0;
	virtual bool						checkOverlapAABB		(const NxBounds3& worldBounds, NxShapesType shapeType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL) = 0;
	virtual bool						checkOverlapOBB			(const NxBox& worldBox, NxShapesType shapeType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL) = 0;
	virtual bool						checkOverlapCapsule		(const NxCapsule& worldCapsule, NxShapesType shapeType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL) = 0;

#if NX_SUPPORT_SWEEP_API
	// Sweep API
	virtual	NxSweepCache*				createSweepCache()	= 0;
	virtual	void						releaseSweepCache(NxSweepCache* cache) = 0;

	virtual	NxU32						linearOBBSweep			(const NxBox& worldBox, const NxVec3& motion, NxU32 flags, void* userData, NxU32 nbShapes, NxSweepQueryHit* shapes, NxUserEntityReport<NxSweepQueryHit>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, NxActor* owner=NULL, NxShape* sweepShape=NULL, Opcode::PrunedObjects* prunedObjects=NULL)			= 0;
	virtual	NxU32						linearCapsuleSweep		(const NxCapsule& worldCapsule, const NxVec3& motion, NxU32 flags, void* userData, NxU32 nbShapes, NxSweepQueryHit* shapes, NxUserEntityReport<NxSweepQueryHit>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, NxActor* owner=NULL, NxShape* sweepShape=NULL, Opcode::PrunedObjects* prunedObjects=NULL)	= 0;
	virtual void						updateCache				(ObjectCache& cache, const NxBox& cachedVolume, const NxBox& worldBox, bool updateStatic, bool updateDynamic) = 0;
	virtual	void						updateCache				(ObjectCache& cache, const NxBox& worldBox, const NxBox* cacheBox)	= 0;
#endif


	// Groups
	virtual	void						setGroupCollisionFlag(NxCollisionGroup group1, NxCollisionGroup group2, bool) = 0;
	virtual	bool						getGroupCollisionFlag(NxCollisionGroup group1, NxCollisionGroup group2) const = 0;
	virtual void						setDominanceGroupPair(NxDominanceGroup group1, NxDominanceGroup group2, NxConstraintDominance & dominance) = 0;
	virtual NxConstraintDominance		getDominanceGroupPair(NxDominanceGroup group1, NxDominanceGroup group2) const = 0;
	virtual	void*						getCollisionMatrix(NxU32 & sizeBytesOut) = 0;		//for faster HSM mirroring
	virtual	void						setCollisionMatrix(void *, NxU32 sizeBytes) = 0;	//for faster HSM mirroring
	virtual	void*						getDominanceMatrix(NxU32 & sizeBytesOut) = 0;		//for faster HSM mirroring
	virtual	void						setDominanceMatrix(void *, NxU32 sizeBytes) = 0;	//for faster HSM mirroring

	virtual	NxU32						getActiveGroups(NxCollisionGroup group) const = 0;
	virtual	void						setActorGroupPairFlags(NxActorGroup group1, NxActorGroup group2, NxU32) = 0;
	virtual	NxU32						getActorGroupPairFlags(NxActorGroup group1, NxActorGroup group2) const = 0;
	virtual NxU32						getNbActorGroupPairs() const = 0;
	virtual	NxU32						getActorGroupPairArray(NxActorGroupPair * userBuffer, NxU32 bufferSize, NxU32 & userIterator) const = 0;


	// Mask based filtering:
	virtual	void						setFilterOps(NxFilterOp op0, NxFilterOp op1, NxFilterOp op2)		= 0;
	virtual	void						setFilterBool(bool flag)											= 0;
	virtual	void						setFilterConstant0(const NxGroupsMask& mask)						= 0;
	virtual	void						setFilterConstant1(const NxGroupsMask& mask)						= 0;

	virtual	void						getFilterOps(NxFilterOp& op0, NxFilterOp& op1, NxFilterOp& op2)const= 0;
	virtual	bool 						getFilterBool() const												= 0;
	virtual	NxGroupsMask				getFilterConstant0() const											= 0;
	virtual	NxGroupsMask				getFilterConstant1() const											= 0;

	virtual void						getVRDReport(NvVRDReporter *reporter)								= 0;


	//used to give the user greater control over parallel raycast/overlap tests.
	virtual	void						lockQueries()		{}
	virtual	void						unlockQueries()		{}

	// BATCHED_RAYCASTS
	virtual	NxSceneQuery*				createSceneQuery(const NxSceneQueryDesc&)	= 0;
	virtual	bool						releaseSceneQuery(NxSceneQuery&)			= 0;
	//~ BATCHED_RAYCASTS

	virtual		void					setDynamicTreeRebuildRateHint(NxU32 dynamicTreeRebuildRateHint) {}

	virtual		void					setSolverBatchSize(NxU32 solverBatchSize) {}

//for fluids:
	virtual NvFluid*					createFluid(const NvFluidDesc&)=0;
	virtual void						releaseFluid(NvFluid*) = 0;
	virtual NxU32						getNbFluids()		const = 0;
	virtual NvFluid**					getFluids() = 0;
	//virtual void						setSoftwareScene(NvScene&) = 0;

	// Cloths
#if NX_USE_CLOTH_API
	virtual NvCloth*					createCloth(const DeformableDesc&)=0;
	virtual void						releaseCloth(NvCloth*) = 0;
	virtual NxU32						getNbCloths() const = 0;
	virtual NvCloth**					getCloths() = 0;
#endif

	virtual NxU32						getNbPairs() const = 0;  /* QUERY(djs): why expose this? */

	virtual void						getStats(NxSceneStats & stats) const = 0;
#ifdef NX_ENABLE_SCENE_STATS2
	virtual NxSceneStats2*				getStats2(NxU32** eventIDs = NULL, NxU32** subsetStatIDs = NULL, NxU32* numSubsetStats = NULL) = 0;
#ifdef NX_ENABLE_COMPARTMENT_STATS
	virtual void						addCompartmentStats(int numCompartments) = 0;
	virtual void						swapStatBuffers() = 0;
#endif
#endif

	virtual	void						flushCaches()	= 0;

	// trigger bounds for high level abstraction:
	virtual NvRawBounds*				createRawBounds(const NxBounds3& bounds, bool forceField=false) = 0;	// PIERRE_FF
	virtual void						releaseRawBounds(NvRawBounds &) = 0;
	virtual void						updateRawBounds(NvRawBounds &, const NxBounds3 & bounds) = 0;
	virtual void						setRawBoundsReport(NvUserRawBoundsReport *) = 0;

	// raw triangle data
	virtual NvRawTriangleMesh *			createRawTriangleMesh(const NxTriangle* inTriangles, const NxU32 numTri, const NxBounds3* localBounds, const NxBounds3* mirrorBounds, const NvMirrorManagerMeshClient& client, const bool isConvex) = 0;
	virtual NvRawTriangleMesh *			createRawTriangleMesh(const ConvexMesh* cvMesh) = 0;
	virtual NvRawTriangleMesh *			createRawTriangleMesh(const NxPlane& plane) = 0;
	virtual bool						releaseRawTriangleMesh(NvRawTriangleMesh* rtm) = 0;
	virtual bool						cookFluidMeshHotspot(NvMirrorManager& mirrorManager, const NvFluidMeshCookingDesc& desc) = 0;


	// sleep/wake actors
	virtual NxActor**                   getSleepBodies(void)   = 0; // bodies put to sleep last simulate
	virtual NxU32                       getNbSleepBodies(void) = 0; // number of bodies put to sleep last simulate
	virtual NxActor**                   getWokenBodies(void)   = 0; // bodies woken up last simulate
	virtual NxU32                       getNbWokenBodies(void) = 0; // number of bodies woken up last simulate
	virtual void						clearSleepWakeBodies(void) = 0;  // resets the sleep/wake lists

	virtual void						buildActiveTransforms() = 0; // build the list of active transforms
	virtual NxActiveTransform*			getActiveTransforms(NxU32 &nbTransformsOut) = 0;

	virtual PxScene*					getPxScene() = 0;

	virtual NxU32						getBoundForIslandSize(NvBody* body) = 0;
	virtual NxU32						getIslandArrayFromBody(NvBody* body, NxActor** userBuffer, NxU32 bufferSize, NxU32& userIterator) = 0;

	virtual void						forceCCD(bool on) = 0; // Remove once CCD is moved into scene flags NX_SF_CONTINOUS_CD. Used by compartments.

			PreBPCallback				mPreBPCB;
			PostBPCallback				mPostBPCB;
			void*						mCBUserData;
	virtual void						setForceFieldManager(ForceFieldManager* ffManager) = 0;

#ifdef __CELLOS_LV2__
	//CA: SPU raycasts
	virtual	void						setEnableCellSPURaycasts(bool state) = 0;
	virtual	bool						areCellSPURaycastsEnabled() = 0;

	// SPU Cloth
	virtual	void						setEnableCellSPUCloth(bool state) = 0;
	virtual	bool						isCellSPUClothEnabled() = 0;
#endif
	protected:
	~NvScene() {}	//use release() plz.
	};


#endif
