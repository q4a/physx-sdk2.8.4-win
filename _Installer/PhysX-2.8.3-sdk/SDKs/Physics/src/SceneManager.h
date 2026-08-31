#ifndef NX_PHYSICS_SCENE_MANAGER
#define NX_PHYSICS_SCENE_MANAGER

class AsyncScene;
class NpCompartment;
class NxCompartment;
class NxCompartmentDesc;
class MirrorManager;

#include "NxCompartmentDesc.h"

class SceneManager
	{
	public:
	SceneManager(HardwareAbstraction & ha);
	~SceneManager();
	void				initialize(NpScene * ownerScene, MirrorManager * manager);
	NvScene *			fetchSceneForNewFluid(bool hardwareScene);
	NvScene *			fetchSceneForNewCloth(bool hardwareScene);
	NvScene *			fetchSceneForNewActor(NxCompartment * );
	void				visualize(NxFoundation::DebugRenderable & vis);
	NxCompartment *		createCompartment(const NxCompartmentDesc& desc);
	NxU32				getNbCompartments() const;
	NxU32				getCompartmentArray(NxCompartment ** userBuffer, NxU32 bufferSize, NxU32 & usersIterator) const;
	NvScene *           getSceneFromCompartment(const NxCompartment *compartment);

	void				onSwapBuffers();
	void				onSetGravity();
	void				onSimulate(NxReal subStepSize, NxU32 numSubSteps, NxReal elapsedTime);
	void				endSimulate();
	void				clientsAccessParentScene();
	void				clientsPassShapeRefCountChanges();
	void				onReleaseActor(NpActor *a);
	void				onReleaseShape(NpShape *s);

	void				onCreateMaterial(const NxMaterialDesc &);
	void				onReleaseMaterial(NxMaterialIndex);
	void				onChangeMaterial(NxMaterialIndex, const NxMaterialDesc &);

	bool				raycastAnyBounds(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups, NxReal maxDist, const NxGroupsMask* groupsMask) const;
	bool				raycastAnyShape(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups, NxReal maxDist, const NxGroupsMask* groupsMask) const;
	NxU32				raycastAllBounds(const NxRay& worldRay, NxUserRaycastReport& report, NxShapesType shapesType, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask) const;
	NxU32				raycastAllShapes(const NxRay& worldRay, NxUserRaycastReport& report, NxShapesType shapesType, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask) const;
	NvShape *			raycastClosestBounds(const NxRay& worldRay, NxShapesType shapeType, NxRaycastHit& hit, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask, NvShape * closestSoFar) const;
	NvShape *			raycastClosestShape(const NxRay& worldRay, NxShapesType shapeType, NxRaycastHit& hit, NxU32 groups, NxReal maxDist, NxU32 hintFlags, const NxGroupsMask* groupsMask, NvShape * closestSoFar) const;

//	NxU32				overlapAABBTriangles	(const NxBounds3& worldBounds, NxFoundation::NxArraySDK<NxTriangle>& worldTriangles);
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
	NxU32				getNbJoints				() const;
	NvJoint *			getJointList			();
	NvJoint *			getNextJointList		();
	void				purgeJoints				();
	void				setShapePairFlags(NvShape &, NvShape &, NxU32 flags);
	NxU32				getShapePairFlags(NvShape &, NvShape &) const;
	NxU32				getNbPairs() const;
	NxU32				getPairFlagArray(NxPairFlag* userArray, NxU32 numPairs) const;

	void				sendToVRD(bool create);
	void				removeFromVRD();

	void				setUserContactReport	(NxUserContactReport* callback);
	void				setUserActorPairFiltering(NxUserActorPairFiltering* callback);
	void				setUserTriggerReport	(NxUserTriggerReport* callback);
	void				setUserNotify			(NxUserNotify* callback);

#ifdef NX_ENABLE_SCENE_STATS2
	NxSceneStats2 *		readSceneStats2(NxU32** eventIDs, NxU32** subsetStatIDs, NxU32* numSubsetStats) const;
	void				swapStatBuffers();
#endif
	NxProfilerData*		readProfileData(bool clearData, NxFoundation::ProfilerManager & );
	NX_INLINE	NpScene * getOwnerScene() { return ownerScene; } 

	private:

	NvScene *			createScene(NvSceneType type, const NxCompartmentDesc& desc);
	NxU32				selectDeviceForNewScene(NxU32 deviceCode, NxCompartmentType type);
	void				bootMirrorRBScene(NvScene *);


	NxFoundation::NxArraySDK<NpCompartment *>	compartments;
	NpCompartment * defaultSwFluidCompartment;
	NpCompartment * defaultHwFluidCompartment;
	NpCompartment * defaultSwClothCompartment;
	NpCompartment * defaultHwClothCompartment;

	NpScene * ownerScene;
	MirrorManager * mirrorManager;

	NxU32	maxHWMat;
	NxU32 jointIteratingCompartment;
	NxU32 lastUsedPPUIndex;

		//these store the merged profiler data from all scenes.
		NxFoundation::NxArraySDK< ::NxProfileZone> profData;	
		NxProfilerData rval;

	class TriggerCallback : public NxUserTriggerReport
		{
		public:
		TriggerCallback(): userReport(0)
		{}

		virtual void onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status);

		NxUserTriggerReport* userReport;
		} triggerTunnel;

	class ContactCallback : public NxUserContactReport
		{
		public:
		ContactCallback(): userReport(0)
		{}

		virtual void onContactNotify(NxContactPair& pair, NxU32 events);

		NxUserContactReport* userReport;
		} contactTunnel;

	HardwareAbstraction & hardwareAbstraction;
	};

class RigidSceneZoner : public NvMirrorManagerClient, public NxFoundation::NxAllocateable
	{
	public:
	RigidSceneZoner(NvScene * rigidScene, NvMirrorManager * mirrorManager, NxReal cellSize, NxU32 gridPower);
	~RigidSceneZoner();

	void sync();

	//mirror manager client:
	virtual NvScene * getOwnerScene() const;
	virtual void addStaticShapeToRegion(NvShape &,const NxBounds3&);
	virtual void removeStaticShapeFromRegion(NvShape &,const NxBounds3&);
	virtual void addDynamicShape(NvShape& shape);
	virtual void addInteractionWithRbShape(NvShape& nvShape, void* userData) { NX_ASSERT(0); }
	virtual void removeInteractionWithRbShape(NvShape& nvShape, void* userData) { NX_ASSERT(0); }
	virtual bool isCompatibleWithStaticMesh(NvRawTriangleMesh &) const;
	virtual NxReal getImpulseTransferSmoothingAlpha() const { return 1.0f; }
	virtual NxU32 getClientType() const { return NV_MIRROR_MANAGER_CLIENT_TYPE_RB; }
	virtual void accessParentScene() {};	
	void passShapeRefCountChanges() { NX_ASSERT(0); }

	// collision filtering
	virtual NxCollisionGroup getGroup() const;
	virtual const NxGroupsMask* getGroupsMask() const;


	private:
	void markHashCell(NxI32 * xyz);

	struct HashCell: public NxFoundation::NxAllocateable
		{
		HashCell(const NxI32 * xyz)
			{
			minCorner[0] = xyz[0];
			minCorner[1] = xyz[1];
			minCorner[2] = xyz[2];
			bHasStuff = 1;	//init as full, otherwise there is no point in creating.
			bHadStuff = 0;
			next = NULL;
			}
		NX_INLINE void getBounds(NxBounds3 & dest, NxReal cellSize)
			{
			dest.min.set(minCorner[0] * cellSize, minCorner[1] * cellSize, minCorner[2] * cellSize);
			dest.max.set(cellSize, cellSize, cellSize);
			dest.max += dest.min;
			}
		NxI32 minCorner[3];	//max is this plus cellSize.  We use integers for exact compares.
		HashCell * next;
		NxU16 bHasStuff, bHadStuff;
		};

	HashCell ** hashTable;
	
	const NxReal invCellSize;
	const NxReal cellSize;
	const NxU32  hashTableSize;


	NxU32 numCells;		//debug counter of the number of cell objects allocated
	NvScene * rigidScene;
	NvMirrorManager * mirrorManager;
	};

#endif
