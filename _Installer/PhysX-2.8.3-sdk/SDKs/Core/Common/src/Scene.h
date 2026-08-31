#ifndef NX_PHYSICS_SCENE
#define NX_PHYSICS_SCENE
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "NxScene.h"
#include "NvScene.h"
#include "NxSceneDesc.h"
#include "PairManager.h"
#include "IDPool.h"
#include "IceSupport.h"
#include "SeparatingAxes.h"
#include "Material.h"
#include "DebugRenderable.h"
#include "SceneStats.h"
#include "SceneStats2.h"
#include "NxReaderWriterLock.h"
#include "CallTimer.h"
#include "TriggerPairs.h"

#include "px_task.h"

#define EXTRA_PROFILING 0

#if EXTRA_PROFILING
#include <cstdio>
#endif

#if NX_USE_CLOTH_API
class NvCloth;
class ClothManager;
#define ENABLE_CLOTH_SW_PROFILING 0
#endif

#if NX_SUPPORT_SWEEP_API
class SDK_SweptVolume;
class ObjectCache;
#endif

class ShapeSet;
class NxTriangle;
class BroadPhase;
class NPhaseCore;
class Sync;
class Actor;
class Body;
class PxElement;
class RbActor;
class ActContact;
class NpJoint;
class NpScene;
class WheelShape;
class QueuedEvent;
class NvSpringAndDamperEffector;
class SpringAndDamperEffector;
namespace Opcode
{
class PrunedObjects;
class Prunable;
}
class CoreDumpHelper;
class LowLevelThreadingThunk;
class NvSpringAndDamperEffector;
class Effector;
class CCDTest;
class SceneObjectPools;

class BoxShape;
class CapsuleShape;
class PlaneShape;
class SphereShape;
class TriangleMeshShape;
class Joint;
class StaticActor;
class PageBoundsInteraction;
class SceneQuery;

#if NX_USE_FLUID_API
class Fluid;
#endif

namespace NxFoundation
{
	class ProfilerManager;
};

enum SceneFlag
	{
	SCENE_COLLISIONS				= (1<<0),
	SCENE_LOCK						= (1<<1),
	SCENE_AC_LOCK					= (1<<2),

	SCENE_SIP_STATES_DIRTY			= (1<<3), 
	SCENE_VRD_CONNECTED_THIS_STEP	= (1<<4),
	SCENE_IS_HW						= (1<<5),
	SCENE_FORCE_CONE_FRICTION		= (1<<6),
	SCENE_USE_CCD					= (1<<7),

	SCENE_DEFAULT		= SCENE_COLLISIONS
	};

enum ProfileZones
	{
	PrSimulate,		//this must be the 1st zone for HSM's meta profiling to work!!
	PrRunSubstep,
	PrStepSetup,
	PrBeforeNearphase,
	PrAfterNearphase,
	PrGenerateIslands,
	PrAfterGenerateIslands,
	PrBeforeSolver,
	PrAfterSolver,
	PrAfterSolver_Sync,
	PrCheckJointBreakage,
	PrProcessCallbacks,
	PrEndStep,

	PrBroadphase_UpdateVolumes,
	PrBroadphase_RemoveOverlaps,
	PrBroadphase_FinishBroadphase,

	PrNearphase,

	PrProcessJointConstraints,
	PrProcessContactConstraints,

	PrRunCCD,

	PrVisualize,

	PrSwapBuffers,
		
	// cloth profile zones
	PrClothSimulationSubstep,
	PrClothUpdateCollision,
	PrClothComputeNewState,
	PrClothGenerateContacts,
	PrClothSolver,
	PrClothIntegrateTimeStep,
	PrClothSwapBuffer,

	// fluid profile zones
	PrFluidStartStep,
	PrFluidEndStep,
	PrFluidParticleQueue,
	PrFluidBirthUser,
	PrFluidBirthEmitters,
	PrFluidEmitConstPressure,
	PrFluidEmitConstRate,
	PrFluidUpdateBounds,
	PrFluidShapesUpdateProcessing,
	PrFluidUpdateCollision,

    PrHighLevel,

	// Exposes PXS_...zones from alternative broadphase context as well as the main one.
	PrSwLL,

	// MAKE SURE PrLowLevel is last one, otherwise the merging of LL zones might fail
	PrLowLevel,
    PrNumZones
	};



class Scene : public NxFoundation::NxAllocateable, public NvScene
	{
	enum MaterialFlagInternal
		{
		NX_MF_INVALID = 1 << 31		//represents an available element in the material array that can be reused.
		};

	struct SimpleBodyPair
		{
		Body* body1;
		Body* body2;
		};

	friend class Sync;						//SB: Ditto
	//virtual interfaces:
	public:
	// implement NvScene:

				NxU32						getSimType() { return readFlag(SCENE_IS_HW) ? NX_SIMULATION_HW : NX_SIMULATION_SW; }
				NxU32						getSceneType() { return mSceneType; }
				NxU32						getHwSceneType();
				void						setSceneType(NvSceneType sceneType) { mSceneType = sceneType; }

				NxU32						getPPUIndex() { return 0; };

	// user services currently not needed for software scenes:
				NvMirrorManager	*			getMirrorManager()	{ return mMirrorManager; }
				void						setMirrorManager(NvMirrorManager	*s ) { mMirrorManager = s; }
				void						notifyReleasePermanentlyMirroredShape(NvShape *shape);
	
	// Gravity
				void						setGravity(const NxVec3&);
				NxVec3						getGravity() const;

	// Pairs
				void						setShapePairFlags(NvShape&, NvShape&, NxU32 nxContactPairFlag);
				NxU32						getShapePairFlags(NvShape&, NvShape&) const;
				void						removePairFlags(NvShape& s);
				NxU32						getNbPairs() const;
				NxU32						getPairFlagArray(NxPairFlag* userArray, NxU32 numPairs) const;
	//materials
	virtual		NvMaterial *				createMaterial(const NvMaterialDesc &, NxMaterialIndex * indexDest);
	virtual		NvMaterial *				getMaterial(NxMaterialIndex);
	virtual		NxU32						getNxMaterialArray(NxMaterial ** userBuffer, NxU32 bufferSize, NxU32 & usersIterator);
	virtual		void						releaseMaterial(NvMaterial * m);
	virtual		NxMaterialIndex				getHighestMaterialIndex() const;


	// Run
				void						flushStream();
				// MSS HACK EARLY BROADPHASE UPDATE
				void						earlyUpdateBroadphase();
				void						simulate(NxU32 numSubSteps, NxU32 batchStepCount);
				void						endSimulation();
	virtual		NvQueuedEvent*				getQueuedEvents();
//@				void						finishRun();
//@				void						setTiming(NxReal maxTimestep, NxU32 maxIter, NxTimeStepMethod method);
//@				void						getTiming(NxReal & maxTimestep, NxU32 & maxIter, NxTimeStepMethod & method, NxU32* numSubSteps=NULL) const;
	virtual		void						setSubStepSize(NxReal substepSize);
	virtual		NxReal						getSubStepSize() const;
	virtual		NxDebugRenderable			getVisualization();
	virtual		NxProfilerData *			readProfileData(bool clearData);

		
				void						getLimits(NxSceneLimits& limits) const;
				void						getVRDReport(NvVRDReporter *reporter);
	// User-notify
//				void						setUserNotify(NxUserNotify* callback);
//				NxUserNotify*				getUserNotify() const;
	// User trigger report
				void						setUserTriggerReport(NxUserTriggerReport* callback);
				NxUserTriggerReport*		getUserTriggerReport() const;
	// User contact report
				void						setUserContactReport(NxUserContactReport* callback);
				NxUserContactReport*		getUserContactReport() const;
	// User actor pair filtering
				void						setUserActorPairFiltering(NxUserActorPairFiltering* callback);
				NxUserActorPairFiltering*	getUserActorPairFiltering() const;
	// Raycasting
	virtual		bool						raycastAnyBounds		(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, const NxGroupsMask* groupsMask=NULL) const;
	virtual		bool						raycastAnyShape			(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, const NxGroupsMask* groupsMask=NULL, NxShape** cache=NULL) const;
	virtual		NxU32						raycastAllBounds		(const NxRay& worldRay, NxUserRaycastReport& report, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL) const;
	virtual		NxU32						raycastAllShapes		(const NxRay& worldRay, NxUserRaycastReport& report, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL) const;
	virtual		NvShape*					raycastClosestBounds	(const NxRay& worldRay, NxShapesType shapesType, NxRaycastHit& hit, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL) const;
	virtual		NvShape*					raycastClosestShape		(const NxRay& worldRay, NxShapesType shapesType, NxRaycastHit& hit, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL, NxShape** cache=NULL) const;
	// Collision detection
	virtual		NxU32						overlapAABBTriangles	(const NxBounds3& worldBounds, NxFoundation::NxArraySDK<NxTriangle>& worldTriangles);

	virtual		NxU32						overlapSphereShapes		(const NxSphere& worldSphere, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, bool accurate_collision=false);
	virtual		NxU32						overlapAABBShapes		(const NxBounds3& worldBounds, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, bool accurate_collision=false);
	virtual		NxU32						overlapOBBShapes		(const NxBox& worldBox, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, bool accurate_collision=false);
	virtual		NxU32						overlapCapsuleShapes	(const NxCapsule& worldCapsule, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, bool accurate_collision=false);
	virtual		NxU32						cullShapes				(NxU32 nbPlanes, const NxPlane* worldPlanes, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL);
	// Placement API
	virtual		bool						checkOverlapSphere		(const NxSphere& worldSphere, NxShapesType shapeType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL);
	virtual		bool						checkOverlapAABB		(const NxBounds3& worldBounds, NxShapesType shapeType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL);
	virtual		bool						checkOverlapOBB			(const NxBox& worldBox, NxShapesType shapeType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL);
	virtual		bool						checkOverlapCapsule		(const NxCapsule& worldCapsule, NxShapesType shapeType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL);
#if NX_SUPPORT_SWEEP_API
	virtual		NxSweepCache*				createSweepCache();
	virtual		void						releaseSweepCache(NxSweepCache* cache);

				NxU32						linearSweep				(const SDK_SweptVolume& volume, const Opcode::PrunedObjects& prunedObjects, const NxBox& worldBox, const NxVec3& motion, NxU32 flags, void* userData, NxU32 nbShapes, NxSweepQueryHit* shapes, NxUserEntityReport<NxSweepQueryHit>* callback, NxU32 activeGroups, const NxGroupsMask* groupsMask, NxActor* owner, NxShape* sweepShape);
	virtual		NxU32						linearOBBSweep			(const NxBox& worldBox, const NxVec3& motion, NxU32 flags, void* userData, NxU32 nbShapes, NxSweepQueryHit* shapes, NxUserEntityReport<NxSweepQueryHit>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, NxActor* owner=NULL, NxShape* sweepShape=NULL, Opcode::PrunedObjects* prunedObjects=NULL);
	virtual		NxU32						linearCapsuleSweep		(const NxCapsule& worldCapsule, const NxVec3& motion, NxU32 flags, void* userData, NxU32 nbShapes, NxSweepQueryHit* shapes, NxUserEntityReport<NxSweepQueryHit>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, NxActor* owner=NULL, NxShape* sweepShape=NULL, Opcode::PrunedObjects* prunedObjects=NULL);
//				NxU32						fillCache(ObjectCache& cache, const NxBox& worldBox, NxShapesType shapeType, NxU32 activeGroups, const NxGroupsMask* groupsMask);
	virtual		void						updateCache(ObjectCache& cache, const NxBox& cachedVolume, const NxBox& worldBox, bool updateStatic, bool updateDynamic);
	virtual		void						updateCache(ObjectCache& cache, const NxBox& worldBox, const NxBox* cacheBox);
#endif
	// Groups
	virtual		void						setGroupCollisionFlag(NxCollisionGroup group1, NxCollisionGroup group2, bool);
	virtual		bool						getGroupCollisionFlag(NxCollisionGroup group1, NxCollisionGroup group2) const;
	virtual		void						setDominanceGroupPair(NxDominanceGroup group1, NxDominanceGroup group2, NxConstraintDominance & dominance);
	virtual		NxConstraintDominance		getDominanceGroupPair(NxDominanceGroup group1, NxDominanceGroup group2) const;
	virtual		void*						getCollisionMatrix(NxU32 & sizeBytesOut);		//for faster HSM mirroring
	virtual		void						setCollisionMatrix(void *, NxU32 sizeBytes);	//for faster HSM mirroring
	virtual		void*						getDominanceMatrix(NxU32 & sizeBytesOut);		//for faster HSM mirroring
	virtual		void						setDominanceMatrix(void *, NxU32 sizeBytes);	//for faster HSM mirroring


	virtual		NxU32						getActiveGroups(NxCollisionGroup group)									const;

	virtual		void						setActorGroupPairFlags(NxActorGroup group1, NxActorGroup group2, NxU32);
	virtual		NxU32						getActorGroupPairFlags(NxActorGroup group1, NxActorGroup group2) const;
	virtual		NxU32						getNbActorGroupPairs() const;
	virtual		NxU32						getActorGroupPairArray(NxActorGroupPair * userBuffer, NxU32 bufferSize, NxU32 & userIterator) const;

	// mask based filtering:

	virtual		void						setFilterOps(NxFilterOp op0, NxFilterOp op1, NxFilterOp op2);
	virtual		void						setFilterBool(bool flag);
	virtual		void						setFilterConstant0(const NxGroupsMask& mask);
	virtual		void						setFilterConstant1(const NxGroupsMask& mask);
	virtual		void						getFilterOps(NxFilterOp& op0, NxFilterOp& op1, NxFilterOp& op2) const;
	virtual		bool 						getFilterBool() const;
	virtual		NxGroupsMask				getFilterConstant0() const;
	virtual		NxGroupsMask				getFilterConstant1() const;


	// Synchronization
				bool						wait(NxStandardFences fence, bool block);
				bool						physicsRunning();
				void						getStats(NxSceneStats & stats) const;
	NX_INLINE	SceneStats&					getStatsFast() { return stats; }
#ifdef NX_ENABLE_SCENE_STATS2
				NxSceneStats2 *				getStats2(NxU32** eventIDs, NxU32** subsetStatIDs, NxU32* numSubsetStats);
#ifdef NX_ENABLE_COMPARTMENT_STATS
				void						addCompartmentStats(int numCompartments);
				void						swapStatBuffers();
#endif
#endif

	// Feature-tags
	// PT: feature tags are now defined per-scene, so that multiple scenes can be simulated in parallel.
				void						growMeshTags(NxU32 nbStamps);
				void						growObjectTags(NxU32 nbStamps);
	// ~Feature-tags

				void						doExecWheels();

		virtual	void						flushCaches();

		virtual NvRawBounds*				createRawBounds(const NxBounds3& bounds, bool forceField=false);
		virtual void						releaseRawBounds(NvRawBounds &);
		virtual void						updateRawBounds(NvRawBounds &, const NxBounds3 & bounds);
		virtual void						setRawBoundsReport(NvUserRawBoundsReport *);

		virtual NvRawTriangleMesh *			createRawTriangleMesh(const NxTriangle* inTriangles, const NxU32 numTri, const NxBounds3* localBounds, const NxBounds3* mirrorBounds, const NvMirrorManagerMeshClient& client, const bool isConvex) { /*only Nf*/ NX_ASSERT(0); return 0; }
		virtual NvRawTriangleMesh *			createRawTriangleMesh(const ConvexMesh* cvMesh){ /*only Nf*/ NX_ASSERT(0); return 0; }
		virtual NvRawTriangleMesh *			createRawTriangleMesh(const NxPlane& plane)    { /*only Nf*/ NX_ASSERT(0); return 0; }
		virtual bool						releaseRawTriangleMesh(NvRawTriangleMesh* rtm) { /*only Nf*/ NX_ASSERT(0); return 0; }
		virtual bool						cookFluidMeshHotspot(NvMirrorManager& mm, const NvFluidMeshCookingDesc& desc) { /*only Nf*/ NX_ASSERT(0); return false; }

				NvFluid*					createFluid(const NvFluidDesc&);
				void						releaseFluid(NvFluid*);
				NxU32						getNbFluids() const;
				NvFluid**					getFluids();

	// Cloth
#if NX_USE_CLOTH_API
				NvCloth*					createCloth(const DeformableDesc&);
				void						releaseCloth(NvCloth*);
				NxU32						getNbCloths() const;
				NvCloth**					getCloths();

				void						setClothManager(ClothManager*);
  				ClothManager*				getClothManager();
#endif

				bool						createBroadPhase(const NxBounds3* maxBounds, NxU32 up, NxU32 level, NxPruningStructure staticStructure, 
															NxPruningStructure dynamicStructure, NxU32 dynRebuildRateHint);

	//stuff implementing NvScene:
	NxU32		getNbStaticShapes() const;
	NxU32		getNbDynamicShapes() const;
	NxU32		getTotalNbShapes() const;

	void		lockScene();
//@	NxU32		getTimeStamp();
	void		release();

	//actors which have either just gone to sleep or woken up
	virtual NxActor**                       getSleepBodies(void)   { return sleepBodies.begin(); }
	virtual NxU32                           getNbSleepBodies(void) { return sleepBodies.size(); }
	virtual NxActor**                       getWokenBodies(void)    { return wokeBodies.begin(); }
	virtual NxU32                           getNbWokenBodies(void)  { return wokeBodies.size(); }
	virtual void							clearSleepWakeBodies(void) { sleepBodies.clear(); wokeBodies.clear(); }

	virtual	void							buildActiveTransforms();
	virtual NxActiveTransform*				getActiveTransforms(NxU32 &nbTransformsOut);

#ifdef NX_RAYCAST_THREADING
	virtual NX_INLINE void					lockQueries()
		{
		raycastLock.lockWriter();
		}
	virtual NX_INLINE void					unlockQueries()
		{
		raycastLock.unlockWriter();
		}
#else
	virtual NX_INLINE void					lockQueries(){}
	virtual NX_INLINE void					unlockQueries(){}
#endif

	// BATCHED_RAYCASTS
	virtual	NxSceneQuery*					createSceneQuery(const NxSceneQueryDesc&);
	virtual	bool							releaseSceneQuery(NxSceneQuery&);
	//~ BATCHED_RAYCASTS

	virtual		void						setDynamicTreeRebuildRateHint(NxU32 dynTreeRebuildRateHint);

	virtual		void						setSolverBatchSize(NxU32 solverBatchSize);


	//internal types:
	private:


	typedef		NxFoundation::NxArraySDK<Actor*>		ActorArray;

	//typedef		NxFoundation::NxArraySDK<Body*>			BodyArray;
	typedef		NxFoundation::NxArraySDK<Joint*>		JointArray;
	typedef		NxFoundation::NxArraySDK<WheelShape*>	WheelShapeArray;

	//internal public methods:
	public:
											Scene(const NvSceneDesc & desc);
	virtual									~Scene();

				void						queueEvent(NvQueuedEvent*);
	//the Actor should register its top level shapes with these.
				bool						removeShape(Shape& shape);
	//should be called when any shape in the scene (not just top-level) is destroyed.
				void						removeShapeReferences(Shape& s);
	virtual NvShape *						createShape(const NvShapeDesc &);
				void						addShape(Shape& shape, bool dynamic);

	virtual NvBody *						createBody(const NvBodyDesc&, NvShape *);
				void						addBody(Body&);
				void						removeBody(Body&);

	//virtual NvJoint *						createJoint(const NvJointDesc &);
	
	virtual NvJoint *						createJoint(const NvJointDesc &, const NvJointPrismaticDesc &);
	virtual NvJoint *						createJoint(const NvJointDesc &, const NvJointCylindricalDesc &);
	virtual NvJoint *						createJoint(const NvJointDesc &, const NvJointPointOnLineDesc &);
	virtual NvJoint *						createJoint(const NvJointDesc &, const NvJointPointOnPlaneDesc &);
	virtual NvJoint *						createJoint(const NvJointDesc &, const NvJointFixedDesc &);
	virtual NvJoint *						createJoint(const NvJointDesc &, const NvJointRevoluteDesc &);
	virtual NvJoint *						createJoint(const NvJointDesc &, const NvJointSphereicalDesc &);
	virtual NvJoint *						createJoint(const NvJointDesc &, const NvJointDistanceDesc &);
	virtual NvJoint *						createJoint(const NvJointDesc &, const NvJointPulleyDesc &);
	virtual NvJoint *						createJoint(const NvJointDesc &, const NvJointD6Desc &);

				void						addJoint(NvJoint &);
				void						removeJoint(NvJoint*);
				void						addToDeadJointList(Joint &);
	NX_INLINE	const NxMaterialDesc &		getMaterialFast(NxMaterialIndex);

				NxU32						getNbJoints()		const;
				NxU32						getNbEffectors()		const;


	virtual NvSpringAndDamperEffector *		createSpringAndDamperEffector(); 
				void						addEffector(NvSpringAndDamperEffector&);
				void						removeEffector(NvSpringAndDamperEffector*);


				void						addWheel(WheelShape *ws);
				void						removeWheel(WheelShape *ws);
				void						addWheelToExecList(WheelShape *ws);
				
	NX_INLINE	NxU32						getU32CollisionFlag(NxU32 groups1, NxU32 groups2) const;
				bool						needContacts(const Shape& a, const Shape& b) const;	

				void						purgeTriggerShape(Shape *shape);

	//internal private methods:
	private:
				void						visualize();
				void						invalidateGroup(Body& root, Joint* deletedJoint);

	/////////////////////////////////////////////////////////////

				void						runSubstep();
	//subroutines of runSubstep():
				void						stepSetup();
				void						beforeNearPhase();
				void						afterNearPhase();
				void						afterGenerateIslands();
				void						beforeSolver();
				void						afterSolver();
				void						processCallbacks();
				void						endStep();

				void						cleanup();
				void						prepareEffectors();
				void						prepareJoints();

#if NX_USE_FLUID_API
				void						runFluidSubstep();		// Substep for fluid simulation in a compartment.
				void						prepareFluids();
				void						fluidsPreCollisionUpdate();
				void						fluidsCollisionUpdate();
				void						fluidsPostCollisionUpdate();
				void						finishFluids();

				void						prepareDynamicActorsForSubStep();	// Only used for compartments
				void						swapDynamicActorsBuffers();			// Only used for compartments
#endif	// NX_USE_FLUID_API

				void						fireQueuedContactCallbacks();

				void						setU32CollisionFlag(NxU32 groups1, NxU32 groups2, bool enable);

				NxU32						reportObjects(NxU32 nb, Opcode::Prunable** prunables, const NxU32 maxShapes, NxShape** buffer, NxUserEntityReport<NxShape*>* callback, const NxGroupsMask* groupsMask, bool skipFiltering=false)	const;
				NxU32						acuteSphereReportObjects(const NxSphere& worldSphere, NxU32 nb, Opcode::Prunable** prunables, const NxU32 maxShapes, NxShape** buffer, NxUserEntityReport<NxShape*>* callback, const NxGroupsMask* groupsMask) const;
				NxU32						acuteAABBReportObjects(const NxBounds3& worldBounds, NxU32 nb, Opcode::Prunable** prunables, const NxU32 maxShapes, NxShape** buffer, NxUserEntityReport<NxShape*>* callback, const NxGroupsMask* groupsMask) const;
				NxU32						acuteOBBReportObjects(const NxBox& worldBox, NxU32 nb, Opcode::Prunable** prunables, const NxU32 maxShapes, NxShape** buffer, NxUserEntityReport<NxShape*>* callback, const NxGroupsMask* groupsMask) const;
				NxU32						acuteCapsuleReportObjects(const NxCapsule& worldCapsule, NxU32 nb, Opcode::Prunable** prunables, const NxU32 maxShapes, NxShape** buffer, NxUserEntityReport<NxShape*>* callback, const NxGroupsMask* groupsMask) const;
#ifdef NX_ENABLE_PROFILER
				void						getLowLevelProfilingData();
				//void						getCPUProfilingData();
#endif

	//inline functions:
	public:
				void						fireCallBacks();
				void						swapBuffers(NxU32 *errorState); 
	NX_INLINE	BroadPhase&					getBroadPhase()						{ NX_ASSERT(collisionSpace != NULL); return *collisionSpace;	}
				void						addToBpShapes(Shape* shape, bool persistent);
				void						removeFromBpShapes(Shape* shape, bool persistent, bool forceRemove=false);

//	NX_INLINE	NxUserNotify*				getUserNotifyFast()			const	{ return userNotify;		}
	NX_INLINE	NxUserTriggerReport*		getUserTriggerReportFast()	const	{ return userTriggerReport;	}
	NX_INLINE	NxUserContactReport*		getUserContactReportFast()	const	{ return userContactReport;	}
				NxU32						getShapePairFlagsFast(const Shape& shapeA, const Shape& shapeB) const;
				NxU32						getActorPairFlags(const RbActor& actorA, const RbActor& actorB) const;
	NX_INLINE	NxReal						getGlobalTime()				const	{ return globalTime;		}
	NX_INLINE	NxU32						getTimeStamp()				const	{ return timeStamp;			}
	NX_INLINE	NxReal						getInvNumSubSteps()			const	{ return invNumSubSteps;	}
	NX_INLINE	NxU32						getNumSubSteps()			const	{ return numSubSteps;		}
	NX_INLINE	NxU32						getCurrSubstep()			const	{ return currSubStep;		}
	NX_INLINE	NX_BOOL						isLastSubstep()				const	{ return numSubSteps == (currSubStep + 1);	}
	NX_INLINE	NxReal						getOneOverDt()				const	{ return oneOverDt;			}
	NX_INLINE	NxReal						getDt()						const	{ return dt;				}
//@	NX_INLINE	void						setNxScene(NxScene* nxScene)		{ mNxScene = nxScene; 		}
	NX_INLINE	NxScene*					getNxScene() 						{ return mNxScene;			}

	virtual		NpCompartment*				getCompartment()					{ return mCompartment;		}
	virtual		void						setCompartment(NpCompartment* compartment);

	NX_INLINE	const NxVec3 &				getGravityFast()			const	{ return gravity;			}
	NX_INLINE	bool						readFlag(SceneFlag flag)	const	{ return (sceneFlags & flag) != 0; }

	NX_INLINE	NxUserScheduler *			getScheduler() const
		{
		return scheduler;
		}

	NX_INLINE	NPhaseCore*					getNPhaseCore() const				{ return nphaseCore; }
	NX_INLINE	NxFoundation::ProfilerManager& getProfiler() const				{ return *profiler; }
	NX_INLINE	CCDTest&					getCCDTest() const					{ return *ccdTest; }

#ifdef _DEBUG
				NxU32						lastDeletionTimeStamp;		// Only used in debug mode, keep it public.
#endif

//@	NX_INLINE	void						setCollisionDetection(bool c) { if (c) sceneFlags |= SCENE_COLLISIONS; else sceneFlags &= ~SCENE_COLLISIONS; }
				void						setLimits(const NxSceneLimits	& l);
//@	NX_INLINE	void						zeroRemainder()			{  remain = 0; }
	NX_INLINE	NvJoint *					getJointList()			{ return (NvJoint*)jointList; }
	NX_INLINE	NvJoint *					getDeadJointList()		{ return (NvJoint*)deadJoints; }
				NvSpringAndDamperEffector *	getEffectorList();
				void						checkJointBreakage();

	NX_INLINE	NX_BOOL						filterFunction(const NxGroupsMask& g0, const NxGroupsMask& g1) const;
	virtual		NxFoundation::DebugRenderable& getDebugRenderable() { return debugVisualization; }
	NX_INLINE	Container&					getContactReportBuffer() { return contactReportBuffer; }

	NX_INLINE	NxU32						createActorId() { NxU32 id=actorIdPool.getNewID(); NX_ASSERT(id<(1<<16)); return id; }
	NX_INLINE	void						releaseActorId(NxU32 id) { actorIdPool.freeID(id); }
	NX_INLINE	NxU32						createBodyId() { NxU32 id=bodyIdPool.getNewID(); NX_ASSERT(id<(1<<16)); return id; }
	NX_INLINE	void						releaseBodyId(NxU32 id) { bodyIdPool.freeID(id); }
	NX_INLINE	NxU32						createShapeId() { NxU32 id=shapeIdPool.getNewID(); NX_ASSERT(id<(1<<16)); return id; }
	NX_INLINE	void						releaseShapeId(NxU32 id) { shapeIdPool.freeID(id); }

	//internal variables:
	private:
				ContainerSizeT				bpShapes;
				NxSceneLimits				limits;
				PairManager					shapePairFlags;
				PairManager					actorPairFlags;

				NxVec3						gravity;			//!< Gravity vector
				NxU32						bodyGravityDirty; // Set to true before body->updateForces() when the gravity has changed

	//time:
	//constants set with setTiming():
//				NxReal						maxTimeStep;				//Size of a time substep.
//				NxU32						maxIter;					//Maximum number of substeps to do before we discard time.
//				NxTimeStepMethod			method;						//Time stepping method.
				NxReal						dt;							//delta time for current substep (usually equal to maxTimeStep).
				NxReal						oneOverDt;					//inverse of dt.
				NxU32						numSubSteps;				//number of substeps that will be performed.
				NxReal						invNumSubSteps;				//inverse of numSubSteps.
	//substepping / counters:
//				NxReal						remain;						//Remaining time. Elapsed user time less than a time substep (maxTimeStep) is accumulated here.
				NxReal						globalTime;					//Accumulated time substeps. For debugging.
				NxU32						timeStamp;					//Counts number of substeps.
//				NxReal						elapsedTime;				//Elapsed user time passed to startRun.
	//current time substep:
				NxU32						currSubStep;				//0 based index of current substep.
				NxU32                       mTotalSubstepsSimulated;
	//containers:
	// Those ones contain shape ptrs from Actor, i.e. compound level, not subparts

				JointArray					newJointList;				//list of new joints created since last frame, not yet added to a group. A subset of jointList.

				Joint*						jointList;					//linked list of scene joints.
				Joint*						deadJoints;					//linked list of joints that broke or had one or more bodies released. They are wait here to be deleted. 
				Effector*					effectorList;				//linked list of scene effectors.
				WheelShapeArray				wheels;						//all wheels in scene.  These special shapes need ticks and buffer swaps.
				WheelShapeArray				execWheels;					//wheels in current sim island that have ground contacts.
	
				private:

				TriggerBuffer				triggerBuffer;

				/*
				This stores data like so:
				NxU32 contactFlags;
				ContactPair contactPair;	//contactPair.stream points to contactStreamBuffer field:
				contactStreamBufferSize		//in bytes
				contactStreamBuffer
				*/
				Container					contactReportBuffer;		//when using substepping we buffer up contact reports for all but the last substep.

				NvSceneType					mSceneType;					// Type of scene

				NvMirrorManager	*			mMirrorManager;

#if NX_USE_FLUID_API
	public:		IDPool						mFluidIdPool;
#endif	// NX_USE_FLUID_API

	private:
#if NX_USE_FLUID_API
				NxFoundation::NxArraySDK<Fluid *>
											mFluids;
				NxFoundation::NxArraySDK<NvFluid *>
											mNvFluids;	// MS: This array has the same elements as the one above just casted
														//     to the superclass. This is ugly, but since the fluid class uses
														//     multiple inheritance, we need this to support the getFluids()
														//     interface (else we run into troubles with the cast/vtable issue)

				NxFoundation::NxArraySDK<Fluid *>
											mEnabledFluids;		// List of enabled fluids updated every simulation step.

				NxFoundation::NxArraySDK<Fluid *>
											mTaskedFluids;		// Temporary list of fluids for which a task is running.
				NxFoundation::NxArraySDK<PxdTask>
											mFluidShapeUpdateTasks;
				NxFoundation::NxArraySDK<PxdTask>
											mFluidDynamicsUpdateTasks;
				NxFoundation::NxArraySDK<PxdTask>
											mFluidCollisionUpdateTasks;
#endif	// NX_USE_FLUID_API

				public:	//until cloths are refactored w. internal API
#if NX_USE_CLOTH_API
				ClothManager*				clothManager;			

#if ENABLE_CLOTH_SW_PROFILING
				CallTimer					mClothProfilingTimer;
#endif

#endif
				ForceFieldManager*			forceFieldManager;

				private:

				NvQueuedEvent*				queuedEventList;

				// broad phase data:
				BroadPhase*					collisionSpace;
		mutable	Opcode::PruningEngine		pruningEngine;
				Opcode::PruningType			staticPruningType;
				Opcode::PruningType			dynamicPruningType;
				NPhaseCore*					nphaseCore;
				NxUserScheduler *scheduler;
				LowLevelThreadingThunk*		llThreadingThunk;

/////////////// Threaded raycast stuff
#ifdef NX_RAYCAST_THREADING
		public: //CA: SPU raycasts
		mutable NxReaderWriterLock raycastLock;
		private:
#		define RAYCAST_READER_LOCK NxTakeReaderLock raycast_reader_lock(raycastLock);
#		define RAYCAST_WRITER_LOCK NxTakeWriterLock raycast_reader_lock(raycastLock);
#else
#		define RAYCAST_READER_LOCK
#		define RAYCAST_WRITER_LOCK
#endif

///////////////// end threaded raycast stuff.

				// User callbacks
//				NxUserNotify*				userNotify;
				NxUserTriggerReport*		userTriggerReport;
				NxUserContactReport*		userContactReport;
//				NvUserRawBoundsReport*		userRawBoundsReport;

				//etc:

				NxU32						numEffectors;
				NxU32						numJoints;
				NxU32						numShapes;
				NxU32						numStaticRootShapes;
				NxU32						numDynamicRootShapes;
				// proxy
				NxScene*					mNxScene;
				NpCompartment*				mCompartment;

	//material library:
	typedef		NxFoundation::NxArraySDK<Material*>	MaterialList;
				Material					defaultMaterial;
				MaterialList				materialList;			//this must be non-static to avoid destructor order screwup.
				NxMaterialIndex				firstFreeMaterialSlot;	//first free slot in the material array.  0 if none.
				//NxU32						numValidMaterials;		//number of valid materials in the array

	//groups:
				NxU32						groupCollisionFlags[32];	//this is a cache for the low bits of the flags.
				PairManager					actorGroupPairFlags;
				NxGroupsMask				bitsK0;
				NxGroupsMask				bitsK1;
				NxFilterOp					filterOp0, filterOp1, filterOp2;
				bool						filterBool;

				NxFoundation::DebugRenderable	debugVisualization;
public:
				void						raiseSceneFlag(SceneFlag flag) { sceneFlags |= flag; }

				//lists of actors woken up or put to sleep last simulate
				void                        onBodyWakeUp(Body *body);
				void                        onBodySleep(Body *body);

				NxFoundation::NxArraySDK<NxActor*>          sleepBodies;
				NxFoundation::NxArraySDK<NxActor*>          wokeBodies;

				NxFoundation::NxArraySDK<NxActiveTransform>	activeTransforms;
				
#ifdef NX_ENABLE_SCENE_STATS2
				mutable SceneStats2			stats2;
				NxSceneStats2				rval;
#endif

private:
				NxFoundation::ProfilerManager* profiler;
				CCDTest*					ccdTest;	
				SceneStats					stats;
				NxU32						sceneFlags;	//!< Combination of ::SceneFlag

				IDPool						bodyIdPool;
				IDPool						shapeIdPool;
				IDPool						actorIdPool;

				SceneObjectPools*			objectPools;

				void						updateFromSDKParameters();
				PxScene*					pxScene;
				StaticActor*				staticAnchor;

				NxFoundation::NxArraySDK<SceneQuery*>				  sceneQueries;
				NxFoundation::NxArraySDK<Body*>						  dirtyBodies;
				NxFoundation::NxArraySDK<SimpleBodyPair>			  lostTouchPairs;
				NxFoundation::NxArraySDK<TriangleMeshShape*>		  pageUnmapList;
				NxFoundation::NxArraySDK<PageBoundsInteraction*>	  pbiUnmapList;
				NxFoundation::NxArraySDK<RbActor*>					  actorDeathrow;
				NxFoundation::NxArraySDK<PxElement*>				  elementDeathrow;

				NxU32						parameterStamp;
				NxU32						errorState;

				NxU64						dominanceBitMatrix[32];
public:
	virtual		PxScene*					getPxScene() { return pxScene;		}
	NX_INLINE   PxScene&					getPxSceneFast() const { return *pxScene;		}
				bool						isValid() const;
	NX_INLINE	StaticActor&				getStaticAnchor()	{ return *staticAnchor;	}

				void						addToDirtyBodyList(Body* body);
				bool						removeFromDirtyBodyList(Body* body);
	NX_INLINE	void						addToLostTouchList(Body* body1, Body* body2);

	NX_INLINE	void						addToPageUnmapList(TriangleMeshShape* mesh);
				bool						removeFromPageUnmapList(TriangleMeshShape* mesh);
				void						addToPbiUnmapList(PageBoundsInteraction* pbi);
				bool						removeFromPbiUnmapList(PageBoundsInteraction* pbi);

	NX_INLINE	void						addToActorDeathrow(RbActor* actor);
	NX_INLINE	void						addToElementDeathrow(PxElement* actor);

	NX_INLINE	Opcode::PruningEngine&		getPruningEngine() const { return pruningEngine; }
				void						initDominanceMatrix();

	virtual		NxU32						getBoundForIslandSize(NvBody* body);
	virtual		NxU32						getIslandArrayFromBody(NvBody* body, NxActor** userBuffer, NxU32 bufferSize, NxU32& userIterator);
	virtual		void						forceCCD(bool on); // Remove once CCD is moved into scene flags (i.e. NX_SF_CONTINOUS_CD). Currently used by compartments.

	virtual		void						setForceFieldManager(ForceFieldManager* ffManager);

				NxConstraintDominance		getDominanceGroupPairFast(NxDominanceGroup group1, NxDominanceGroup group2) const;

#if EXTRA_PROFILING || EXTRA_PROFILING_READABLE
private:
				FILE*	        			extraProfileFile;
				NxU32 	        			lineNum;
#endif
#ifdef __CELLOS_LV2__
	//CA: SPU raycasts
				bool						cellSPURaycastsEnabled;
				bool						cellSPURaycastsAllowed;
				void						setEnableCellSPURaycasts(bool state);
				bool						areCellSPURaycastsEnabled();

	// SPU Cloth
				bool						cellSPUClothEnabled;
				void						setEnableCellSPUCloth(bool state);
				bool						isCellSPUClothEnabled();
#endif
	};

NX_INLINE const NxMaterialDesc& Scene::getMaterialFast(NxMaterialIndex index)
	{
	if (index >= materialList.size())
		index = 0;						//should always exist and be valid.
	return *materialList[index];
	}

// Collision utils

NX_INLINE NxU32 convertFlags(NxShapesType shapeType)
	{
	NxU32 flags = 0;
	if(shapeType & NX_STATIC_SHAPES)	flags |= Opcode::PQF_STATIC;
	if(shapeType & NX_DYNAMIC_SHAPES)	flags |= Opcode::PQF_DYNAMIC;
	return flags;
	}


NX_INLINE NxU32 Scene::getU32CollisionFlag(NxU32 groups1, NxU32 groups2) const
	{
	// PT: special case for compounds! I assume the speed hit is negligible.
//	if(groups1==0xffffffff || groups2==0xffffffff)	return NX_TRUE;
	if(groups1==0xffff || groups2==0xffff)	return NX_TRUE;

	// Default code path for single shapes
	return groupCollisionFlags[groups1] & (1 << groups2);
	}

NX_INLINE void Scene::addToLostTouchList(Body* body1, Body* body2)
	{
	NX_ASSERT(body1 != 0);
	NX_ASSERT(body2 != 0);
	lostTouchPairs.pushBack();
	lostTouchPairs.back().body1 = body1;
	lostTouchPairs.back().body2 = body2;
	}

NX_INLINE void Scene::addToPageUnmapList(TriangleMeshShape* mesh)
	{
	pageUnmapList.pushBack(mesh);
	}

NX_INLINE void Scene::addToActorDeathrow(RbActor* actor)
{
	actorDeathrow.pushBack(actor);
}

NX_INLINE void Scene::addToElementDeathrow(PxElement* actor)
{
	elementDeathrow.pushBack(actor);
}

#ifdef __CELLOS_LV2__

NX_INLINE void Scene::setEnableCellSPURaycasts(bool state)
	{
	cellSPURaycastsEnabled = state;
	}

NX_INLINE bool Scene::areCellSPURaycastsEnabled()
	{
	return (cellSPURaycastsEnabled && cellSPURaycastsAllowed);
	}

NX_INLINE void Scene::setEnableCellSPUCloth(bool state)
	{
	cellSPUClothEnabled = state;
	}

NX_INLINE bool Scene::isCellSPUClothEnabled()
	{
	return cellSPUClothEnabled;
	}
#endif

#endif
