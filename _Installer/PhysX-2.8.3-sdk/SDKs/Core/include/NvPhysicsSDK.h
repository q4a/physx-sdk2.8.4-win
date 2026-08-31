#ifndef NX_PHYSICS_NV_PHYSICS_SDK
#define NX_PHYSICS_NV_PHYSICS_SDK
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "NxPhysicsSDK.h"
#include "NxScene.h"

class NpPhysicsSDK;
class NxMaterial;
class NxFoundationSDK;
class NxDebugRenderable;
class NvScene;
class NvConvexMesh;
class NvTriangleMesh;
class NvHeightField;
class NvSceneDesc;
class NxPMap;

class AgPPUConnection;

class NvCCDSkeleton			//TODO: move to its own file.
	{
	public:
	virtual void release() = 0; 
	virtual void			setNxCCDSkeleton(NxCCDSkeleton *) = 0;
	virtual NxCCDSkeleton * getNxCCDSkeleton() = 0;

	virtual NxU32 save(void * destBuffer, NxU32 destBufferSize) = 0;	//destBufferSize must be greater or equal to getSize().  Returns number of bytes written.
	virtual NxU32 load(const void * srcBuffer, NxU32 srcBufferSize) = 0;
	virtual NxU32 getSize() = 0;		//returns size in bytes
	virtual NxU32 saveToDesc(NxSimpleTriangleMesh &desc) = 0;
	};



#if defined(WIN32)
#	if defined(CORELIB)
#		define NVP_DLL_EXPORT __declspec(dllexport)
#	else
#		define NVP_DLL_EXPORT __declspec(dllimport)
#	endif
#else
#	define NVP_DLL_EXPORT
#endif

class NvPhysicsSDK
	{
	public:
	virtual	void				release() = 0;
	virtual void				setNpPhysicsSDK(NpPhysicsSDK *) = 0;//mNpPhysicsSDK = x;
	virtual	bool				setParameter(NxParameter paramEnum, NxReal paramValue) = 0;
	virtual	NxReal				getParameter(NxParameter paramEnum) const = 0;

	//factory:
	virtual NvScene *			createScene(const NvSceneDesc &, NxU32, NxU32) = 0;	//note: physics SDK doesn't reference scenes.  this ptr not needed.
	virtual NvConvexMesh *		createConvexMesh() = 0;
	virtual NvTriangleMesh *	createTriangleMesh() = 0;
	virtual NvHeightField *		createHeightField() = 0;
	virtual NvCCDSkeleton *     createCCDSkeleton(const NxSimpleTriangleMesh&) = 0;		//'better' descriptor style construction.  TODO: could cook later.
	virtual void memoryReport(void) =0;
	virtual NxHWVersion			getHWVersion() const = 0;
	virtual NxU32				getNbPPUs() const = 0;
	virtual AgPPUConnection *	getPPUConnection(NxU32 index) const = 0;

	protected:
	~NvPhysicsSDK() {}	//use release() plz.
	};

// Software SDK.
NX_C_EXPORT NVP_DLL_EXPORT NvPhysicsSDK *NxCreateCoreSDK(NxFoundationSDK*, const NxPhysicsSDKDesc&, NxSDKCreateError&);
typedef NvPhysicsSDK *(NxCreateCoreSDK_FUNC)(NxFoundationSDK*, NxSDKCreateError&);

// GPU SDK.
NX_C_EXPORT NVP_DLL_EXPORT NvPhysicsSDK *NgCreateCoreSDK(NxFoundationSDK*, const NxPhysicsSDKDesc &, NxSDKCreateError&);
typedef NvPhysicsSDK *(NgCreateCoreSDK_FUNC)(NxFoundationSDK*, const NxPhysicsSDKDesc &, NxSDKCreateError&);	


#endif
