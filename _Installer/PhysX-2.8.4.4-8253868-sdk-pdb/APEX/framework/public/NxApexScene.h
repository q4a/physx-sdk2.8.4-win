#include "NxApex.h"
#ifndef __NX_APEX_SCENE_H__
#define __NX_APEX_SCENE_H__
/*
 * Copyright 2009-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

/*!
\file
\brief classes NxApexScene, NxApexSceneStats, NxApexSceneDesc
*/

class NxScene;

namespace physx
{
	namespace shdfnd2
	{
	class PxIPC;
	};
}

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
\brief Data used to initialize a new NxApexScene
*/
class NxApexSceneDesc : public NxApexDesc
{
public:

	PX_INLINE NxApexSceneDesc() : NxApexDesc()
	{
		init();
	}

	PX_INLINE void setToDefault()
	{
		NxApexDesc::setToDefault();
		init();
	}

	/** \brief Give this ApexScene an existing NxScene */
	NxScene * scene;

	/**
	\brief Give this ApexScene a user defined TaskManager

	If taskManager is NULL, the APEX scene will create a default TaskManager
	and thread pool.
	*/
	physx::pxtask::TaskManager *taskManager;
	
	/**
	\brief Toggle the use of a legacy NxDebugRenderable

	If true, then debug rendering will happen through the legacy DebugRenderable interface.
	If false (the default value) it will render through the NxUserRenderResources interface.
	*/
	bool	useDebugRenderable;

	/**
	\brief Transmits debug rendering to PVD2 as well
	*/
	bool	debugVisualizeRemotely;

	/**
	\brief If 'debugVisualizeLocally' is true, then debug visualization which is being transmitted remotely will also be shown locally as well.
	*/
	bool			debugVisualizeLocally;
private:
	PX_INLINE void init()
	{
		scene = 0;
		taskManager = 0;
		useDebugRenderable = false;
		debugVisualizeRemotely = false;
		debugVisualizeLocally = true;
	}
};

/**
\brief data type definitions for stats (derived from openautomate)
*/
typedef enum
{
  STATS_TYPE_INVALID = 0,
  STATS_TYPE_STRING  = 1,
  STATS_TYPE_INT     = 2,
  STATS_TYPE_FLOAT   = 3,
  STATS_TYPE_ENUM    = 4,
  STATS_TYPE_BOOL    = 5
} ApexStatDataType;
/**
\brief data value definitions for stats (derived from openautomate)
*/
typedef struct oaValueStruct
{
  union
  {
    char*			String;      
	physx::PxI32	Int;
	physx::PxF32	Float;
    char*			Enum;
    bool			Bool;
  };
} ApexStatValue;

typedef struct
{
	char*				StatName;
	ApexStatDataType	StatType;
	ApexStatValue		StatCurrentValue;
} ApexStatsInfo;

/**
\brief Per scene statistics
*/
struct NxApexSceneStats
{
	/**
	\brief The number of actors in the scene
	*/
//todo remove	physx::PxU32			nbActors;
	/**\brief The number of ApexStats structures stored.
	*/
	physx::pubfnd2::PxU32	numApexStats;
	/**\brief Array of #ApexStatsInfo structures.
	*/
	ApexStatsInfo			*ApexStatsInfoPtr;
};

/**
\brief Types of view matrices handled by APEX

USER_CUSTOMIZED : APEX simply uses the view matrix given. Need to setViewParams()
LOOK_AT_RH: APEX gets the eye direction and eye position based on this kind of matrix.
LOOK_AT_LH: APEX gets the eye direction and eye position based on this kind of matrix.

*/
struct ViewMatrixType
{
	enum Enum
	{
		USER_CUSTOMIZED = 0,
		LOOK_AT_RH,
		LOOK_AT_LH,
	};
};

/**
\brief Types of projection matrices handled by APEX

USER_CUSTOMIZED : APEX simply uses the projection matrix given. Need to setProjParams()

*/
struct ProjMatrixType
{
	enum Enum
	{
		USER_CUSTOMIZED = 0,
	};
};

/**
\brief An APEX wrapper for an NxScene
*/
class NxApexScene : public NxApexRenderable, public NxApexContext, public NxApexInterface
{
public:
	/**
	\brief Associate an NxScene with this NxApexScene.
	
	All NxApexActors in the NxApexSene will be added to the NxScene.
	The NxScene pointer can be NULL, which will cause all APEX actors to be removed
	from the previously specified NxScene.  This must be done before the NxScene
	can be released.
    */
	virtual void setPhysXScene(NxScene *s) = 0;

	/**
	\brief Retrieve the NxScene associated with this NxApexScene
	*/
	virtual NxScene *getPhysXScene() = 0;

	/**
	\brief Retrieve scene statistics
	*/
	virtual NxApexSceneStats* getStats( void ) = 0;

	/**
	\brief D3D(9/10) CUDA Interop synchronization method

	This method must be called by the renderer thread each frame before UpdateRenderResources and rendering 
	RenderResources should not be accessed after this call before dispatchResources
	*/
	virtual void prepareRenderResourceContexts() = 0;

	/**
	\brief Start simulation of the APEX (and PhysX) scene

	Start simulation of the NxApexActors and the NxScene associated with this NxApexScene.
	No NxApexActors should be added, deleted, or modified until fetchResults() is called.

    finalStep should be left as true, unless your application is manually sub stepping APEX
    (and PhysX) and you do not intend to try to render the output of intermediate steps.
	*/
	virtual void simulate(physx::PxF32 elapsedTime, bool finalStep = true) = 0;

	/**
	\brief Checks, and optionally blocks, for simulation completion.  Updates scene state.

	Checks if NxApexScene has completed simulating (optionally blocking for completion).  Updates
	new state of NxApexActors and the NxScene.  Returns true if simulation is complete.

	\param block [in] - block until simulation is complete
	\param errorState [out] - error value is written to this address, if not NULL
	*/
	virtual bool fetchResults(bool block, physx::PxU32 *errorState) = 0;

	/**
	\brief Returns an NxDebugRenderable object that contains the data for debug rendering of this scene
	*/
	virtual const NxDebugRenderable* getDebugRenderable() = 0;

	/**
	\brief Checks, and optionally blocks, for simulation completion.

	Performs same function as fetchResults(), but does not update scene state.  fetchResults()
	must still be called before the next simulation step can begin.
	*/
	virtual bool checkResults(bool block) = 0;

	/**
	\brief Set the resource budget for this scene.

	Sets a total resource budget that the LOD system will distribute among modules,
	and eventually among the objects of those modules.

	The resource is specified in an abstract 'resource unit' rather than any real world quantity. 
	*/
	virtual void setLODResourceBudget(physx::PxF32 totalResource) = 0;

	/**
	\brief Returns the ammount of LOD resource consumed.

	Retrieves the amount of LOD resource consumed in the last simulation frame.  The system attempts
	to keep this below the value set in setLODResourceBudget().  
	*/
	virtual physx::PxF32 getLODResourceConsumed() const = 0;

	/**
	\brief Allocate a view matrix. Returns a viewID that identifies this view matrix
		   for future calls to setViewMatrix(). The matrix is de-allocated automatically 
		   when the scene is released.

	Each call of this function allocates space for one view matrix. Since many features in 
	APEX require a projection matrix it is _required_ that the application call this method. 
	Max calls restricted to 1 for now.
	If ViewMatrixType is USER_CUSTOMIZED, setViewParams() as well using this viewID.
	If connected to PVD, PVD camera is set up.
	@see ViewMatrixType
	@see setViewParams()
	*/
	virtual physx::PxU32		allocViewMatrix(ViewMatrixType::Enum) = 0;

	/**
	\brief Allocate a projection matrix. Returns a projID that identifies this projection matrix 
	       for future calls to setProjMatrix(). The matrix is de-allocated automatically 
		   when the scene is released.

	Each call of this function allocates space for one projection matrix.  Since many features in 
	APEX require a projection matrix it is _required_ that the application call this method. 
	Max calls restricted to 1 for now.
	If ProjMatrixType is USER_CUSTOMIZED, setProjParams() as well using this projID
	@see ProjMatrixType
	@see setProjParams()
	*/
	virtual physx::PxU32			allocProjMatrix(ProjMatrixType::Enum) = 0;

	/**
	\brief Returns the number of view matrices allocated.
	*/
	virtual physx::PxU32			getNumViewMatrices() const = 0;

	/**
	\brief Returns the number of projection matrices allocated.
	*/
	virtual physx::PxU32			getNumProjMatrices() const = 0;

	/**
	\brief Sets the view matrix for the given viewID. Should be called whenever the view matrix needs to be updated.
	
	If the given viewID's matrix type is identifiable as indicated in ViewMatrixType, eye position and eye direction are set as well, using values from this matrix.
	Otherwise, make a call to setViewParams().
	If connected to PVD, PVD camera is updated.
	*/
	virtual void					setViewMatrix(const physx::PxMat44 & viewTransform, const physx::PxU32 viewID = 0) = 0;

	/**
	\brief Returns the view matrix set by the user for the given viewID.

	@see setViewMatrix()
	*/
	virtual const physx::PxMat44 &	getViewMatrix(const physx::PxU32 viewID = 0) const = 0;

	/**
	\brief Sets the projection matrix for the given projID. Should be called whenever the projection matrix needs to be updated.

	Make a call to setProjParams().
	@see setProjParams()
	*/
	virtual void					setProjMatrix(const physx::PxMat44 & projTransform, const physx::PxU32 projID = 0) = 0;

	/**
	\brief Returns the projection matrix set by the user for the given projID.

	@see setProjMatrix()
	*/
	virtual const physx::PxMat44 &	getProjMatrix(const physx::PxU32 projID = 0) const = 0;

	/**
	\brief Sets the use of the view matrix and projection matrix as identified by their IDs. Should be called whenever either matrices needs to be updated.
	*/
	virtual void					setUseViewProjMatrix(const physx::PxU32 viewID = 0, const physx::PxU32 projID = 0) = 0;

	/**
	\brief Sets the necessary information for the view matrix as identified by its viewID. Should be called whenever any of the listed parameters needs to be updated.

	@see ViewMatrixType
	*/
	virtual void					setViewParams(const physx::PxVec3 & eyePosition, const physx::PxVec3 & eyeDirection, const physx::PxVec3 & worldUpDirection = PxVec3(0,1,0), const physx::PxU32 viewID = 0) = 0;

	/**
	\brief Sets the necessary information for the projection matrix as identified by its projID. Should be called whenever any of the listed parameters needs to be updated.

	@see ProjMatrixType
	*/
	virtual void					setProjParams(physx::PxF32 nearPlaneDistance, physx::PxF32 farPlaneDistance, physx::PxF32 fieldOfViewDegree, physx::PxU32 viewportWidth, physx::PxU32 viewportHeight, const physx::PxU32 projID = 0) = 0;

	/**
	\brief Returns the world space eye position.

	@see ViewMatrixType
	@see setViewMatrix()
	*/
	virtual const physx::PxVec3 &	getEyePosition(const physx::PxU32 viewID = 0) const = 0;

	/**
	\brief Returns the world space eye direction.

	@see ViewMatrixType
	@see setViewMatrix()
	*/
	virtual const physx::PxVec3 &	getEyeDirection(const physx::PxU32 viewID = 0) const = 0;

	/**
	\brief Returns the APEX scene's task manager
	*/
	virtual physx::pxtask::TaskManager *getTaskManager() = 0;

	/**
	\brief Toggle the use of a debug renderable
	*/
	virtual void setUseDebugRenderable(bool state) = 0;

	/**
	\brief Gets debug rendering parameters from NxParameterized
	*/
	virtual ::NxParameterized::Interface *getDebugRenderParams() = 0;

	/**
	\brief Gets module debug rendering parameters from NxParameterized
	*/
	virtual ::NxParameterized::Interface *getModuleDebugRenderParams(const char *name) = 0;
};

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_APEX_SCENE_H__
