#ifndef NX_PHYSICS_PHYSICS_SDK
#define NX_PHYSICS_PHYSICS_SDK
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Physics.h"
#include "NxPhysicsSDK.h"
#include "NvPhysicsSDK.h"
#include "Allocateable.h"
#include "Array.h"
#include "PairManager.h"
#if defined(LINUX) || defined(__APPLE__) || defined(__native_client__)
#include <string.h>
#endif
#include "Opcode.h"
#include "InternalTriangleMesh.h"

class CollisionMap;
class Shape;
class Scene;
class NpScene;
class ConvexMesh;
class TriangleMesh;
class NpPhysicsSDK;
class NxConvexMeshDesc;
class NvHeightField;


class PhysicsSDK : public NvPhysicsSDK, public NxFoundation::NxAllocateable
	{
	public:
								PhysicsSDK(NxFoundationSDK *, const NxPhysicsSDKDesc &, NxSDKCreateError &);
	virtual						~PhysicsSDK();

	virtual NvScene *			createScene(const NvSceneDesc & desc, NxU32 simType, NxU32 sceneType);			//note: physics SDK doesn't reference scenes.  this ptr not needed.
	virtual NvConvexMesh *		createConvexMesh();		//note: physics SDK  doesn't reference convexes.  this ptr not needed.
	virtual NvTriangleMesh *	createTriangleMesh();	//note: physics SDK  doesn't reference meshes.  this ptr not needed.

	virtual NvHeightField *		createHeightField();	//note: physics SDK  doesn't reference heightfields.  this ptr not needed.
	virtual NvCCDSkeleton *     createCCDSkeleton(const NxSimpleTriangleMesh&);
	virtual	NxHWVersion			getHWVersion() const;
	virtual	void				memoryReport();
	virtual NxU32				getNbPPUs() const	{ return 0; }
	virtual AgPPUConnection *	getPPUConnection(NxU32 index) const { return NULL; }


				bool			init();

				bool			setParameter(NxParameter paramEnum, NxReal paramValue);
				NxReal			getParameter(NxParameter paramEnum) const;


//				void			visualize(const NxUserDebugRenderer&);
//				void			clearVisualization();

				void			setNpPhysicsSDK(NpPhysicsSDK* p);
				void			release();


	//group access:
	NX_INLINE static PhysicsSDK& getInstance();

	//group access variables:
	//from rigidBody:
	//parameters:
	static NxReal params[NX_PARAMS_NUM_VALUES];
	static NxReal paramsMinValue[NX_PARAMS_NUM_VALUES];
	static NxReal paramsMaxValue[NX_PARAMS_NUM_VALUES];
	static NxReal paramsDefValue[NX_PARAMS_NUM_VALUES];

#ifdef USE_PROFILER
	NxProfilingZone * rbStartRun;
	NxProfilingZone * rbGroupGra;
	NxProfilingZone * rbIntegrate;
	NxProfilingZone * rbCollisions;
	NxProfilingZone * rbFriction;
	NxProfilingZone * rbSetupSys;
	NxProfilingZone * rbSolveSys;
#endif

	static	PhysicsSDK*					instance;
	NpPhysicsSDK*						mNpPhysicsSDK;

	static	NxFoundationSDK*			foundationSDK;

	static	CollisionMap				map;
	//constants
	static	const NxVec3				zeroVector;
	static	const NxVec3				xVector;
	static	const NxVec3				yVector;
	static	const NxVec3				zVector;
	static	const NxMat33				idMatrix;
	static		  NxU32					parameterStamp;
	};

NX_INLINE PhysicsSDK & PhysicsSDK::getInstance()
	{
	return *instance;
	}

#endif
