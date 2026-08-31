#ifndef NX_PHYSICS_SCENEQUERY
#define NX_PHYSICS_SCENEQUERY
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Allocateable.h"
#include "NxSceneQuery.h"
#include "Opcode.h"
class Scene;
class SceneQuery;

#ifdef __CELLOS_LV2__
#include "cell/CellRaycast.h"
#endif

#include "NxScheduler.h"
#include "NxSync.h"
#include "NxAllocateable.h"

	class BatchedQueryTask : public NxTask, public NxFoundation::NxAllocateable
	{
		public:
						BatchedQueryTask(SceneQuery* owner);
		virtual			~BatchedQueryTask();

	    virtual void	execute();

		NxSync			mSync;
		SceneQuery*		mOwner;
	};

	// PT: just to make sure we can implement this later
	class SceneQuery : public NxSceneQuery, public NxFoundation::NxAllocateable
	{
	public:
									SceneQuery();
	virtual							~SceneQuery();

	virtual	NxSceneQueryReport*		getQueryReport();
	virtual	NxSceneQueryExecuteMode	getExecuteMode();

	virtual	void					execute();
	virtual	bool					finish(bool block=false);

	//scene query calls identical to the ones in scene that batch up calls 

	//bool result - in synchronous mode, these return the result immediately.  in async mode the return value is false, and the callback is used.  The query id of 0xffffffff requests the automatically generated query index to be used.
	virtual bool					raycastAnyShape			(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, const NxGroupsMask* groupsMask=NULL, NxShape** cache=NULL, void* userData=NULL)	const;
	virtual bool					checkOverlapSphere		(const NxSphere& worldSphere, NxShapesType shapesType=NX_ALL_SHAPES, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, void* userData=NULL)							const;
	virtual bool					checkOverlapAABB		(const NxBounds3& worldBounds, NxShapesType shapesType=NX_ALL_SHAPES, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, void* userData=NULL)							const;
	virtual bool					checkOverlapOBB			(const NxBox& worldBox, NxShapesType shapesType=NX_ALL_SHAPES, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, void* userData=NULL)									const;
	virtual bool					checkOverlapCapsule		(const NxCapsule& worldCapsule, NxShapesType shapesType=NX_ALL_SHAPES, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, void* userData=NULL)							const;

	//one ray hit: - in synchronous mode, these return the result immediately.  in async mode the return value is NULL, and the callback is used.
	virtual NxShape*				raycastClosestShape		(const NxRay& worldRay, NxShapesType shapesType, NxRaycastHit& hit, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL, NxShape** cache=NULL, void* userData=NULL) const;

	//ray hit callbacks: in synchronous mode, these return the number of  hits.  In async mode the return value is 0. 
	virtual NxU32					raycastAllShapes		(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL, void* userData=NULL) const;

	//shape list or shape callbacks:  in synchronous mode, these return the number of  hits.  In async mode the return value is 0. 
	virtual	NxU32					overlapSphereShapes		(const NxSphere& worldSphere, NxShapesType shapesType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, void* userData=NULL)		const;
	virtual	NxU32					overlapAABBShapes		(const NxBounds3& worldBounds, NxShapesType shapesType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, void* userData=NULL)		const;
	virtual	NxU32					overlapOBBShapes		(const NxBox& worldBox, NxShapesType shapesType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, void* userData=NULL)			const;
	virtual	NxU32					overlapCapsuleShapes	(const NxCapsule& worldCapsule, NxShapesType shapesType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, void* userData=NULL)	const;
	virtual	NxU32					cullShapes				(NxU32 nbPlanes, const NxPlane* worldPlanes, NxShapesType shapesType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, void* userData=NULL)	const;

	//sweep hit list or callbacks: in synchronous mode, these return the number of  hits.  In async mode the return value is 0. 
	virtual NxU32					linearOBBSweep			(const NxBox& worldBox, const NxVec3& motion, NxU32 flags, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, void* userData=NULL)			const;
	virtual	NxU32					linearCapsuleSweep		(const NxCapsule& worldCapsule, const NxVec3& motion, NxU32 flags, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, void* userData=NULL)	const;

			void					taskCallback();
			BatchedQueryTask*		task;

			Scene*					owner;
			NxSceneQueryReport*		report;
			NxSceneQueryExecuteMode	executeMode;
	mutable	Container				batchedQueries;

#ifdef __CELLOS_LV2__
	// CA: SPU raycasts
			CellRaycast* 			cellRaycast;
#endif
	};

#endif
