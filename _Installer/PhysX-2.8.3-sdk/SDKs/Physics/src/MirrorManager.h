#ifndef NX_PHYSICS_MIRROR_MANAGER
#define NX_PHYSICS_MIRROR_MANAGER


#include "NvMirrorManager.h"
#include "NxSpinMutex.h"

class NpForceField;

enum MirroredActorFlag
{
	HWA_MA_DIRTY_ACTOR_FLAGS				= (1<<0),
	HWA_MA_DIRTY_ACTOR_GROUP				= (1<<1),
	HWA_MA_DIRTY_ACTOR_USER_PAIR_FILTER		= (1<<2),
	HWA_MA_DIRTY_ACTOR_BODY_CCD				= (1<<3), 
	HWA_MA_DIRTY_ACTOR_BODY_SOLVER			= (1<<4), // Solver iteration count
	HWA_MA_DIRTY_ACTOR_BODY_FLAGS			= (1<<5), // Body flags, except NX_BF_KINEMATIC
	HWA_MA_DIRTY_ACTOR_REPORT_FLAGS			= (1<<6), // Contact report flags
	HWA_MA_DIRTY_ACTOR_REPORT_THRESHOLD		= (1<<7), // Contact report threshold
	HWA_MA_DIRTY_ACTOR						= HWA_MA_DIRTY_ACTOR_FLAGS | HWA_MA_DIRTY_ACTOR_GROUP |	HWA_MA_DIRTY_ACTOR_BODY_CCD | HWA_MA_DIRTY_ACTOR_BODY_SOLVER | 
											  HWA_MA_DIRTY_ACTOR_BODY_FLAGS | HWA_MA_DIRTY_ACTOR_REPORT_FLAGS | HWA_MA_DIRTY_ACTOR_REPORT_THRESHOLD,

	HWA_MA_DIRTY_SHAPE_FLAGS				= (1<<8),
	HWA_MA_DIRTY_SHAPE_GROUP				= (1<<9),
	HWA_MA_DIRTY_SHAPE_GROUPS_MASK			= (1<<10),
	HWA_MA_DIRTY_SHAPE_MATERIAL				= (1<<11),
	HWA_MA_DIRTY_SHAPE_SKIN_WIDTH			= (1<<12),
	HWA_MA_DIRTY_SHAPE_CCD					= (1<<13),
	HWA_MA_DIRTY_SHAPE_LOCAL_POSE			= (1<<14),
	HWA_MA_DIRTY_SHAPE_GEOMETRY				= (1<<15), // Geometry properties such as box dimension, sphere radius etc.
	HWA_MA_DIRTY_SHAPE_NI_COMPARTMENT_TYPES	= (1<<16),
	HWA_MA_DIRTY_SHAPE						= HWA_MA_DIRTY_SHAPE_FLAGS | HWA_MA_DIRTY_SHAPE_GROUP |	HWA_MA_DIRTY_SHAPE_GROUPS_MASK | HWA_MA_DIRTY_SHAPE_MATERIAL |
											  HWA_MA_DIRTY_SHAPE_SKIN_WIDTH	| HWA_MA_DIRTY_SHAPE_CCD | HWA_MA_DIRTY_SHAPE_LOCAL_POSE | HWA_MA_DIRTY_SHAPE_GEOMETRY |
											  HWA_MA_DIRTY_SHAPE_NI_COMPARTMENT_TYPES,
};

class MirrorManager : 	public NvMirrorManager,  public NvUserRawBoundsReport
	{
	public:	
	MirrorManager(HardwareAbstraction & ha);
	~MirrorManager();
	void initialize(NpScene * ownerScene);
	void shutDown();
	void onNewHardwareObject(NvScene * , NvMirrorManagerClient *, NxBounds3 &);
	void onReleaseHarwareObject(NvScene * , NvMirrorManagerClient *);
	void onSimulate();
	void onEndSimulate() { simulating = false; }

	void onCompartmentCreate(NxCompartment* compartment);
	void onReleaseActor(NpActor &);
	void onReleaseShape(NpShape &);
	void visualize(NxFoundation::DebugRenderable & vis);
	void transferImpulses();
	void processBoundsUpdates();
	void processForceFieldUpdates();

	void onNewClientAABBInstant(NvMirrorManagerClient& client, const NxBounds3 & b, void* userData);
	void onNewClientAABBDelayed(NvMirrorManagerClient& client, const NxBounds3 & b, void* userData);

	//implement NvMirrorManager:
	virtual void updateAABBs(NvMirrorManagerClient& caller, const NxBounds3* newAABBS, void** newUserData, const NxU32 numNew, const NxBounds3* delAABBS, const NxU32 numDel, NxU32 & numActiveShapes);
	virtual void onNewClientAABB(NvMirrorManagerClient& client, const NxBounds3 & b, void* userData);
	virtual void onDelClientAABB(NvMirrorManagerClient& client, const NxBounds3 & b);

	virtual void updateMirroredShapeRefCounts(const NvMirrorManagerClient* client, const NvShape** shapesList, const NxI32* refCountDeltaList, NxU32 numShapes);
	void processDirtyMirroredActors();

	virtual bool onOutOfHeapMem(NvMirrorManagerClient& caller, const NxU32 minSize, const NxU32 maxSize);
	virtual	NvShape * startMirroringShape(NvMirrorManagerClient& caller, const NvShape *shape);
	virtual	NvShape * getPermanentlyMirroredShape(NvMirrorManagerClient& caller, const NvShape *shape);
	virtual	void      stopMirroringShape(NvMirrorManagerClient& caller, const NvShape *mirroredShape);

	virtual bool preCacheStaticMesh(NvScene* clientDestScene, NvMirrorManagerMeshClient& meshClient, const NxBounds3& aabb);

	//implement NvUserRawBoundsReport:
	virtual void onEnterBounds(NvRawBounds &, NvShape &);
	virtual void onLeaveBounds(NvRawBounds &, NvShape &);
	virtual void onEnterBounds(NvRawBounds&, NvRawBounds&);
	virtual void onLeaveBounds(NvRawBounds&, NvRawBounds&);

	void onActorChange(NpActor* npActor, MirroredActorFlag flag);

	void onForceFieldGroupCreate(NpForceFieldShapeGroup& group);
	void onForceFieldGroupRelease(NpForceFieldShapeGroup& group);
	void onForceFieldGroupChange(NpForceFieldShapeGroup& group);


	private:
	struct MirroredActor;

	struct CachedMesh : public NxFoundation::NxAllocateable	//for dynamics and statics
		{
		NvRawTriangleMesh * mesh;
		NxU32 refcount;	//if refcount is 0, it is created but could be removed.  Ref-d by a single BoundsMirror::staticMirror, or several shapes in a DynamicMirror-s.
		//for matching:
		NxVec3 infos;	
		NxShapeType type;			//NX_SHAPE_MESH, NX_SHAPE_BOX, NX_SHAPE_CONVEX, NX_SHAPE_RAW_MESH(means static)

		NX_INLINE CachedMesh(NvRawTriangleMesh * m, NxShapeType t, const NxVec3 & i, NxU32 r = 0) 
			{
			type = t;
			infos = i;
			mesh = m; 
			refcount = r; 
			}
		};

	struct DynamicMirror : public NxFoundation::NxAllocateable
		{
		NX_INLINE DynamicMirror(MirroredActor * sa, NvScene * s, NvMirrorManagerClient * c)
			{
			sourceActor = sa;
			mirroredShape = NULL;
			mirrorDestScene = s;
			mirrorDestClient = c;
			refcount = 0;
			}

		MirroredActor * sourceActor;
		NvShape * mirroredShape;	//either singleton or a compound!
		NxFoundation::NxArraySDK<CachedMesh*> mirroredRawMeshes;	//we can have multiple cuz we can have compounds.  May want to use a LList ??
		//destination of this mirror:
		NvScene * mirrorDestScene;
		NvMirrorManagerClient * mirrorDestClient;
		NxU32 refcount;			//referenced by one or more BoundsMirror-s.  All the referencing BoundsMirror-s should be for the same scene.
		};

	struct MirroredActor : public NxFoundation::NxAllocateable	//only used for DYNAMIC source actors.
		{
			NX_INLINE MirroredActor(NpActor * sa, NxU32 i, NxU8 wc)
			{
			sourceActor = sa;
			bkSourceActor = sa;
			index = i;
			wakeCount = wc;
			flags = 0;
			}

		NpActor * sourceActor;
		NpActor * bkSourceActor;
		NxFoundation::NxArraySDK<DynamicMirror*> mirroredInstances;	//one actor may mirror into several hw scenes.  Array may be empty cuz we don't free this when we run out of DynamicMirror-s.
		NxU32 index;			//index in the mirroredActors array.
		NxU8 wakeCount;		//if >0, we mirror its pos
		NxU32 flags;		// Combination of MirroredActorFlags
		};

	struct BoundsMirror	: public NxFoundation::NxAllocateable //the stuff a particular bounds object is mirroring
		{
		NX_INLINE BoundsMirror(const NxBounds3 & b, NvScene * ds, NvMirrorManagerClient * dc, NvRawBounds * rb)
			{
			bounds = b;
			mirrorDestScene = ds;
			mirrorDestClient = dc;

			rawBounds = rb;

			staticCachedMesh = NULL;
			staticMirror = NULL;
//			fakeDynamicMirror = NULL;
			next = NULL;
			sequenceRefCount = 1;

			userData = NULL;
			}

		NxBounds3 bounds;
		//destination of this mirror:
		NvScene * mirrorDestScene;
		NvMirrorManagerClient * mirrorDestClient;	//this boundsMirror is shared between this and all 'compatible' clients.
		NxU32	sequenceRefCount;	
		//source trigger of mirror in sw scene:
		NvRawBounds * rawBounds;	//its userData points to this BoundsMirror.
		//mirrored static mesh data to dest scene of this exact bounds:
		CachedMesh * staticCachedMesh;	//may be NULL if no geometry at this location
		NvShape * staticMirror;			//may be NULL if no geometry at this location
		//fluids don't support some shape types 'natively' as statics, so we just create a fake nonmoving dynamic for those.
		//that gets stored here and is otherwise treated the same as staticMirror.	May be a compound!
//		NvShape * fakeDynamicMirror;	//TODO: this is imperfect because fluid receives these guys as dynamics, and could share them between packets, but we duplicate them.
		void* userData;	// User specific data that can be attached to boundsMirror. Used by SW fluids to add reference to fluid shape. 

		BoundsMirror * next;		//next bounds mirror with same hash
		};

	struct ForceFieldGroupBounds	: public NxFoundation::NxAllocateable
	{
		ForceFieldGroupBounds() : boundsDestScene(NULL), rawBounds(NULL) {}
		ForceFieldGroupBounds(NvScene* destScene, NvRawBounds* bounds) : boundsDestScene(destScene), rawBounds(bounds) {}
		NvScene* boundsDestScene;
		NvRawBounds* rawBounds;
	};

	struct MirroredForceFieldGroup : public NxFoundation::NxAllocateable
	{
		MirroredForceFieldGroup(NpForceFieldShapeGroup* group) : sourceGroup(group) {}
		NpForceFieldShapeGroup* sourceGroup;
		NxFoundation::NxArraySDK<ForceFieldGroupBounds> ffBounds;
	};


#ifdef FLUID_PRE_STANCING_AND_MORE
	void overlapTest(NvMirrorManagerMeshClient& meshClient, const NxBounds3& aabb, const NxGroupsMask* groupsMask, FluidTri * fluidTrigs, BoundsMirror *bm = 0);
#else
	void overlapTest(BoundsMirror & bm, FluidTri * fluidTrigs); //const NxBounds3 &, bool dynamics, NvScene * mirrorDestScene, NvMirrorManagerClient * mirrorDestClient);	//from Rocket
#endif

	DynamicMirror* mirrorStaticMeshShape(BoundsMirror& bm, NvShape& shape, NvShape*& mirroredShape);
	bool instantStaticShapeMirroring(BoundsMirror& bm);

	NvShape* mirrorShapeCreate(NvShape* shape, DynamicMirror* dm, BoundsMirror& bm, NvBody* body);
	DynamicMirror * mirrorActorCreate(NpActor *npactor, MirroredActor * ma, BoundsMirror & bm);//, const NxBounds3 & bounds, NvScene * mirrorDestScene, NvMirrorManagerClient * mirrorDestClient);
	bool mirrorActorRelease(NpActor *a);
		
	bool emitStaticMesh(NvShape & shape, const NxBounds3 & bounds, FluidTri * fluidTrigs);
	bool emitStaticMeshBox(NvShape & shape, FluidTri * fluidTrigs);
	bool emitStaticMeshConvex(NvShape & shape, FluidTri * fluidTrigs);
	bool emitStaticMeshPlane(NvShape & shape, FluidTri * fluidTrigs);

	CachedMesh * meshToRawMesh(NvShape *, BoundsMirror & bm);
	CachedMesh * convexToRawMesh(NvShape *, BoundsMirror & bm);
	CachedMesh * hfToRawMesh(NvShape *, BoundsMirror & bm);
	NvShape* instanceCommon(NvShapeDesc& destDesc, NvShape* sourceShape, DynamicMirror* dm, BoundsMirror& bm, NvBody* body);
	NvShape* instanceCapsule(NvShape *, DynamicMirror * dm, BoundsMirror & bm, NvBody * body);
	NvShape* instanceSphere(NvShape *, DynamicMirror * dm, BoundsMirror & bm, NvBody * body);
	NvShape* instanceBox(NvShape *, DynamicMirror * dm, BoundsMirror & bm, NvBody * body);
	NvShape* instanceConvex(NvShape *, DynamicMirror * dm, BoundsMirror & bm, NvBody * body);
	NvShape* instanceMesh(NvShape *, DynamicMirror * dm, BoundsMirror & bm, NvBody * body);
	NvShape* instanceDynamicCachedMesh(NvShape *, CachedMesh * cm, DynamicMirror * dm, NvBody * body);
	NvShape* instancePlane(NvShape *, DynamicMirror * dm, BoundsMirror & bm);
	NvShape* instanceHeightField(NvShape *, DynamicMirror * dm, BoundsMirror & bm, NvBody * body);
	NvShape* instanceCompound(BoundsMirror & bm, DynamicMirror * md = NULL, NvBody * body = NULL, NxU32 numShapes = 1);
	void instanceDynamicMirror(BoundsMirror & bm, MirroredActor * ma, NvBody* nvbody, bool createCompound, DynamicMirror ** dm, NvBody ** body, NxU32 numShapes);
	void addToBoundsHash(BoundsMirror *);
	bool stanceAndCook(BoundsMirror & bm);
	void pageMeshes(BoundsMirror & bm, DynamicMirror & dm, bool pageIn);

	NxU32 hashBounds(const NxBounds3 & b);
	BoundsMirror * findHashedBounds(NvMirrorManagerClient * client, const NxBounds3 & b, BoundsMirror ** prevInLLOut, NxU32 hashIndex);
	DynamicMirror * findMirrorForClient(const MirroredActor* ma, const NvMirrorManagerClient* client, NxU32* idx = NULL);

	void unlinkMirroredActor(MirroredActor * ma);
	void releaseMirror(MirroredActor * ma, NxU32 i);
	void releaseBoundsMirror(BoundsMirror * bm, NxU32 hashIndex, BoundsMirror * prevInLL, bool cacheRawBounds = false);
	void printDebugReport();
	void freeShape(NvShape *);
	CachedMesh * matchMesh(NxShapeType, NxVec3);
	CachedMesh * matchStaticMesh(NvMirrorManagerMeshClient &, const NxBounds3 &, bool increaseRef);

	void processEnterBounds(NvRawBounds &, NvShape &);
	bool processLeaveBounds(NvRawBounds &, MirroredActor * ma, NvShape *);

	// mmf added code -----------------------------

	void transferImpulse(Actor *sourceActor, NvBody *sourceBody, NvBody *mirrorBody, NxReal smoothingAlpha);
	void mirrorShapes();
	void mirrorPose(NvBody *sourceBody, NvBody *mirrorBody, NxReal dt, NxU32 sceneType);
	void mirrorActorProperties(MirroredActor* ma);
	void mirrorShapeProperties(MirroredActor* ma);
	void mirrorShapeProperties(NvShape* srcShape, NvShape* dstShape, NxU32 mirrorFlags);
	void mirrorShapeGeometryProperties(NvShape* srcShape, NvShape* dstShape);
#ifndef __CELLOS_LV2__
	bool isShapeMirroringAllowed(const NvShape& sourceShape, NvScene& mirrorDestScene);
	void ensureMirroredShapesValidity(MirroredActor* ma);
#endif

	void updateForceFieldGroupBounds(NpForceFieldShapeGroup& group, ForceFieldGroupBounds& bounds);

	struct PermanentlyMirroredShape
	{
		PermanentlyMirroredShape() {
			sourceShape = NULL;
			mirrorShape = NULL;
			refCounter = 0;
		}

		NvShape *sourceShape;
		NvScene *mirrorScene;
		NvShape *mirrorShape;
		NxU32 refCounter;
	};

	bool createPermanentyMirroredShape(NvShape *sourceShape, NvScene *mirrorScene, 
			PermanentlyMirroredShape &ms);
	void mirrorPermanentlyMirroredShapes();

	void releasePermanentlyMirroredShapes();
	void releasePermanentlyMirroredShapes(NpActor &npActor);
	void releasePermanentlyMirroredShapes(NpShape &npShape);

	void releaseShapeFromSwCloth(DynamicMirror *dm, NpShape * nps = 0);

	NxFoundation::NxArraySDK<PermanentlyMirroredShape> permanentlyMirroredShapes;

	// end mmf code -------------------------------

//	FluidTri * fluidTrigs;
	NpScene * ownerScene;
	NvScene * ownerNvScene;	//we can normally get this from ownerScene, except after its been destroyed, which is before our destructor gets called, where this is needed.  So it must be cached.
	//NvScene * mirrorDestScene;
	//NvFluid * mirrorDestFluid;
#define NXBH_HASHSIZE 512

	NxU32 DEBUGnBounds;
	NxU32 DEBUGnShapes;

	NxFoundation::NxArraySDK<CachedMesh*> mirroredRawMeshes;
	//NxFoundation::NxArraySDK<DynamicMirror*> dynamicMirrors;
	NxFoundation::NxArraySDK<BoundsMirror*> mirroredBoundsHash;	//hash container for mirrored bounds
	NxFoundation::NxArraySDK<MirroredActor*> mirroredActors;
	NxFoundation::NxArraySDK<MirroredForceFieldGroup*> mirroredForceFieldGroups;
	NxFoundation::NxArraySDK<NpForceFieldShapeGroup*> forcefieldGroupUpdates;

	NxFoundation::NxArraySDK<CachedMesh*> unreferencedRawMeshes;
 
	HardwareAbstraction & hardwareAbstraction;

	/**
	lock to prevent concurrent data access from different compartments and primary scene for the following methods:
	- bool MirrorManager::onOutOfHeapMem(NvMirrorManagerClient& caller, const NxU32 minSize, const NxU32 maxSize)
	- void MirrorManager::onNewClientAABBInstant(NvMirrorManagerClient& client, const NxBounds3 & b, void* userData)
	- NvShape * MirrorManager::getPermanentlyMirroredShape(NvMirrorManagerClient& caller, const NvShape *shape)
	- void MirrorManager::stopMirroringShape(NvMirrorManagerClient& caller, const NvShape *mirroredShape) 
	Locking for the permanently mirrored shapes is just necessary because of the non-buffered calls to  
	MirrorManager::stopMirroringShape(NvMirrorManagerClient& caller, const NvShape *mirroredShape) by cloth compartments.
	*/
	NxSpinMutex compartmentAccessLock;

	enum UpdateType
		{
		UT_ONENTER, UT_ONLEAVE
		};
	struct AABBUpdate
		{
		NvRawBounds bounds;
		NvShape *shape;
		MirroredActor * ma;
		UpdateType type;
		};
	NxFoundation::NxArraySDK<AABBUpdate> aabbUpdates;
	NxFoundation::NxArraySDK<BoundsMirror*> deadBoundsMirrors;
	
	//LZ fixed a HSM
	NxFoundation::NxArraySDK<MirroredActor*> deadMirroredActors;
	NxFoundation::NxArraySDK<MirroredActor*> dirtyMirroredActors;	// List of mirrored actors which might have mirrored shapes that are not referenced anymore
	
	bool simulating;

	friend class NpForceField;	// PT: sorry but the actual bounds are stored in the BoundsMirror class instead of NvRawBounds, and I need to access them
	friend class NpForceFieldShapeGroup;	
	};


#endif
