#include "NxApex.h"
#ifndef __NX_APEX_SDK_H__
#define __NX_APEX_SDK_H__
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
\brief APEX SDK classes
*/

#include "NxSerializer.h"
#include "NxParameterized.h"
#include "Nxp.h"

namespace physx
{
	namespace pxtask
	{
		class TaskManager;
		class CudaContextManagerDesc;
		class CudaContextManager;
	}

	namespace general_PxIOStream2
	{
		class PxFileBuf;
	}

	namespace shdfnd2
	{
		class Foundation;
	}
};

/**
PhysX 2.8 SDK classes
*/
class NxShape;
class NxActor;
class NxJoint;
class NxCloth;
class NxSoftBody;
class NxPhysicsSDK;
class NxCookingInterface;
class NxDebugRenderable;

namespace physx {
namespace apex {

class NxApexStream;
class NxUserRenderResourceManager;
class NxApexScene;
class NxApexSceneDesc;
class NxRenderMeshAsset;
class NxRenderMeshAssetAuthoring;
class NxModule;
class NxApexPhysXObjectDesc;
class NxResourceProvider;
class NxResourceCallback;
class NxApexRenderDebug;
class NxApexAsset;
class NxApexAssetAuthoring;
class NxApexSDKCachedData;
class NxRenderMeshActor;

PX_PUSH_PACK_DEFAULT

/**
\brief Integer values, each representing a unique authorable object class/type
*/
typedef unsigned int NxAuthObjTypeID;

/**
\brief Descriptor class for NxApexSDK
*/
class NxApexSDKDesc : public NxApexDesc
{
public:
	/**
	\brief The allocator APEX should use
	*/
	physx::PxUserAllocator* allocator; 

 	/**
	\brief The output stream that APEX should use
	*/
	physx::PxUserOutputStream* outputStream;

 	/**
	\brief The PhysX SDK version you are building with

	A particular APEX build will be against a particular PhysX version.
	Versioning the PhysX API will require versioning of APEX.
	*/
	physx::PxU32 physXSDKVersion;

	/**
	\brief Pointer to the physX sdk (PhysX SDK version specific structure)
	*/
	NxPhysicsSDK* physXSDK;

	/**
	\brief Pointer to the cooking interface (PhysX SDK version specific structure)
	*/
	NxCookingInterface* cooking;

    /**
    \brief User defined interface for creating renderable resources.
    */
    NxUserRenderResourceManager *renderResourceManager;

	/**
	\brief Pointer to a user-callback for unresolved named resources.
	
	This function will be called by APEX when
	an unresolved named resource is requested.  The function will be called at most once for each named
	resource.  The function must directly return the pointer to the resource or NULL.
	*/
	NxResourceCallback* resourceCallback;

	/**
	\brief Path to APEX module DLLs (Windows Only)

	If specified, this string will be prepended to the APEX module names prior to calling LoadLibrary() to load them.  You can
	use this mechanism to ship the APEX module DLLS into their own subdirectory.  For example:  "APEX/"
	*/
	const char *dllLoadPath;

	/**
	\brief Wireframe material name

	If specified, this string designates the material to use when rendering wireframe data.
	It not specified, the default value is "ApexWireframe"
	*/
	const char *wireframeMaterial;

	/**
	\brief Solid material name

	If specified, this string designates the material to use when rendering solid shaded (per vertex color) lit triangles.
	If not specified, the default value is "ApexSolidShaded"
	*/
	const char *solidShadedMaterial;

	/**
	\brief constructor sets to default.
	*/
	PX_INLINE NxApexSDKDesc() : NxApexDesc()
	{
		init();
	}

	/**
	\brief (re)sets the structure to the default.
	*/
	PX_INLINE void setToDefault()
	{
		NxApexDesc::setToDefault();
		init();
	}

	/**
	\brief Returns true if the descriptor is valid.
	\return true if the current settings are valid
	*/
	PX_INLINE bool isValid() const
	{
		bool retVal = NxApexDesc::isValid();

		if (renderResourceManager == NULL)
			return false;
		if (physXSDK == NULL)
			return false;
		if (cooking == NULL)
			return false;
		if (physXSDKVersion != NX_PHYSICS_SDK_VERSION)
			return false;
		return retVal;
	}

private:
	PX_INLINE void init()
	{
        renderResourceManager = NULL;
		allocator = NULL;
		outputStream = NULL;
		physXSDKVersion = NX_PHYSICS_SDK_VERSION;
		physXSDK = NULL;
		cooking = NULL;
		resourceCallback = NULL;
		dllLoadPath = NULL;
		solidShadedMaterial = "ApexSolidShaded";
		wireframeMaterial = "ApexWireframe";
	}
};


/**
\brief The ApexSDK abstraction. Manages scenes and modules.
*/
class NxApexSDK : public NxApexInterface
{
public:
	/**
	\brief Create an APEX Scene
	*/
	virtual NxApexScene * createScene(const NxApexSceneDesc &) = 0;

	/**
	\brief Release an APEX Scene
	*/
	virtual void releaseScene(NxApexScene *) = 0;

	/**
	 * \brief Allocates a TaskManager with optional thread pool
     *
     * If numThreads is non-zero, the new TaskManager will immediately
     * allocate a CpuDispatcher thread pool with the specified thread
     * count.  The user may query the CpuDispatcher pointer from this
     * new TaskManager and assign it to TaskManagers for other scenes.
     * The TaskManager maintains ownership of this CpuDispatcher and
     * will automatically release it when the TaskManager is released.
     *
     * If numThreads is zero, the new TaskManager's CpuDispatcher
     * reference will be NULL.  If the user does not set a CpuDispatcher
     * reference before providing the TaskManager to a scene, a
     * CpuDispatcher will be allocated the first time the simulation is
     * stepped.
	 */
	virtual physx::pxtask::TaskManager * createTaskManager( physx::PxU32 numThreads = 0 ) const = 0;

	/**
	\brief Create a physx::pxtask::CudaContextManager
	*/
	virtual physx::pxtask::CudaContextManager * createCudaContextManager(const physx::pxtask::CudaContextManagerDesc& desc) = 0;

	/**
	\brief Create/Load a module
	*/
	virtual NxModule * createModule(const char *name, NxSDKCreateError* err = NULL) = 0;

	/**
	\brief Return an object describing how APEX is using the PhysX Actor.
	\return NULL if PhysX Actor is not owned by APEX.
	*/
    virtual const NxApexPhysXObjectDesc * getPhysXObjectInfo(const NxActor *actor) const = 0;

	/**
	\brief Return an object describing how APEX is using the PhysX Shape.
	\return NULL if PhysX Shape is not owned by APEX.
	*/
	virtual const NxApexPhysXObjectDesc * getPhysXObjectInfo(const NxShape *shape) const = 0;

	/**
	\brief Return an object describing how APEX is using the PhysX Joint.
	\return NULL if PhysX Joint is not owned by APEX.
	*/
	virtual const NxApexPhysXObjectDesc * getPhysXObjectInfo(const NxJoint *joint) const = 0;


	/**
	\brief Return an object describing how APEX is using the PhysX Cloth.
	\return NULL if PhysX Cloth is not owned by APEX.
	*/
	virtual const NxApexPhysXObjectDesc * getPhysXObjectInfo(const NxCloth *cloth) const = 0;

	/**
	\brief Return an object describing how APEX is using the PhysX SoftBody.
	\return NULL if PhysX SoftBody is not owned by APEX.
	*/
	virtual const NxApexPhysXObjectDesc * getPhysXObjectInfo(const NxSoftBody *softbody) const = 0;

	/**
	\brief Return the user output stream.
	*/
	virtual physx::PxUserOutputStream * getOutputStream() = 0;

	/**
	\brief Returns the cooking interface.
	*/
	virtual NxCookingInterface * getCookingInterface() = 0;

	/**
	\brief Return the named resource provider.
	*/
	virtual NxResourceProvider * getNamedResourceProvider() = 0;

	/**
	\brief Return a user-implemented NxApexStream instance, if apexuser.dll is found.
	
	Else returns an APEX PxFileBuf instance.  For use by APEX tools and samples, not by APEX internally.
	Internally,	APEX will use a PxFileBuf directly provided by the user.
	*/
	virtual physx::general_PxIOStream2::PxFileBuf *createStream( const char *filename, physx::PxU32 apexStreamFlags ) = 0;

	/**
	\brief Return a PxFileBuf which reads from a buffer in memory.
	*/
	virtual physx::general_PxIOStream2::PxFileBuf *createMemoryReadStream(const void *mem,physx::PxU32 len) = 0;

	/**
	\brief Return a PxFileBuf which writes to memory.
	*/
	virtual physx::general_PxIOStream2::PxFileBuf *createMemoryWriteStream(void) = 0;

	/**
	\brief Return the address and length of the contents of a memory write buffer stream.
	*/
	virtual const void * getMemoryWriteBuffer(physx::general_PxIOStream2::PxFileBuf &stream,physx::PxU32 &len) = 0;

	/**
	\brief Release a previously created PxFileBuf used as a read stream
	*/
	virtual void releaseMemoryReadStream(physx::general_PxIOStream2::PxFileBuf &stream) = 0;

	/**
	\brief Release a previously created PxFileBuf used as a write stream
	*/
	virtual void releaseMemoryWriteStream(physx::general_PxIOStream2::PxFileBuf &stream) = 0;

	/**
	\brief Return the PhysX SDK.
	*/
	virtual NxPhysicsSDK * getPhysXSDK() = 0;

	/**
	\brief Return the number of modules.
	*/
	virtual physx::PxU32 getNbModules() = 0;

	/**
	\brief Return an array of module pointers.
	*/
	virtual NxModule** getModules() = 0;

	/**
	\brief Release a previously loaded module
	*/
	virtual void releaseModule(NxModule* module) = 0;


   	/**
   	\brief Creates a render mesh asset from a stream

	\deprecated
   	*/
	virtual NxRenderMeshAsset * createRenderMeshAsset( physx::general_PxIOStream2::PxFileBuf&, const char *name, NxAuthObjTypeID ownerModuleID = 0 ) = 0;

	/**
	\brief Creates a render mesh asset from an authoring asset.
	*/
	virtual NxRenderMeshAsset *	createRenderMeshAsset( NxRenderMeshAssetAuthoring&, const char *name, NxAuthObjTypeID ownerModuleID = 0 ) = 0;

	/**
	\brief Releases a render mesh asset.
	*/
   	virtual void				releaseRenderMeshAsset( NxRenderMeshAsset& ) = 0;

   	/**
   	\brief Creates an ApexDebugRender interface
   	*/
	virtual NxApexRenderDebug * createApexRenderDebug(bool useRemoteDebugVisualization=false,bool debugVisualizeLocally=true) = 0;

   	/**
   	\brief Releases an ApexDebugRender interface
   	*/
	virtual void                releaseApexRenderDebug(NxApexRenderDebug &debug) = 0;

	/**
	\brief Create a render mesh asset authoring instance
	*/
	virtual NxRenderMeshAssetAuthoring * createRenderMeshAssetAuthoring() = 0;

	/**
	\brief Release a render mesh asset authoring instance
	*/
	virtual void releaseRenderMeshAssetAuthoring( NxRenderMeshAssetAuthoring& ) = 0;

	/**
	\brief Return the number of assets force loaded by all of the existing APEX modules 
	*/
	virtual physx::PxU32 forceLoadAssets( ) = 0;

	/**
	\brief Get a list of the APEX authorable types.

	The memory for the strings returned is owned by the APEX SDK and should only be read, not written or freed.
	Returns false if 'inCount' is not large enough to contain all of the names.
	*/
	virtual bool getAuthorableObjectNames( const char ** authTypeNames, physx::PxU32 &outCount, physx::PxU32 inCount) = 0;

	/** 
	\brief Get the a pointer to APEX's instance of the ::NxParameterized::Traits class
	*/
	virtual ::NxParameterized::Traits * getParameterizedTraits( ) = 0;

	/**
	\brief Creates an APEX asset from a stream.

	\deprecated
	*/
	virtual NxApexAsset * createAsset( physx::general_PxIOStream2::PxFileBuf&, const char *name ) = 0;

	/**
	\brief Creates an APEX asset from an NxApexAssetAuthoring object.
	*/
	virtual NxApexAsset * createAsset( NxApexAssetAuthoring&, const char *name ) = 0;

	/**
	\brief Creates an APEX asset from a parameterized object.
	\note The NxParameterized::Interface object's ownership passes to the asset, if a copy is needed, use the NxParameterized::Interface::copy() method
	*/
	virtual NxApexAsset * createAsset( ::NxParameterized::Interface *, const char *name ) = 0;

	/**
	\brief Release an APEX asset
	*/
	virtual void releaseAsset( NxApexAsset& ) = 0;

	/**
	\brief Creates an APEX asset authoring object
	*/
	virtual NxApexAssetAuthoring * createAssetAuthoring( const char *authorTypeName ) = 0;

	/**
	\brief Create an APEX asset authoring object with a name
	*/
	virtual NxApexAssetAuthoring * createAssetAuthoring( const char *authorTypeName, const char *name ) = 0;

	/**
	\brief Create an APEX asset authoring object from a parameterized object.
	*/
	virtual NxApexAssetAuthoring * createAssetAuthoring( ::NxParameterized::Interface *, const char *name ) = 0;

	/**
	\brief Releases an APEX asset authoring object
	*/
	virtual void releaseAssetAuthoring( NxApexAssetAuthoring& ) = 0;

	/**
	\brief Returns the scene data cache for actors in the scene
	*/
	virtual NxApexSDKCachedData& getCachedData() const = 0;

	/**
	\brief Create a Serialization object
	*/
	virtual ::NxParameterized::Serializer * createSerializer(::NxParameterized::Serializer::SerializeType type) = 0;

	/**
	\brief Create a Serialization object from custom traits
	*/
	virtual ::NxParameterized::Serializer * createSerializer(::NxParameterized::Serializer::SerializeType type, ::NxParameterized::Traits *traits) = 0;

	/**
	\brief Figure out whether a given chunk of data was xml or binary serialized.
	To properly detect XML formats, 32 bytes of data are scanned, so dlen should be at least 32 bytes
	*/
	virtual ::NxParameterized::Serializer::SerializeType getSerializeType(const void *data,PxU32 dlen) = 0;

	/**
	\brief Figure out whether a given chunk of data was xml or binary serialized.
	*/
	virtual ::NxParameterized::Serializer::SerializeType getSerializeType(physx::general_PxIOStream2::PxFileBuf &stream) = 0;

	/**
	\brief Find native platform for a given chunk of data.
	*/
	virtual NxParameterized::Serializer::ErrorType getSerializePlatform(physx::general_PxIOStream2::PxFileBuf &stream, NxParameterized::SerializePlatform &platform) = 0;

	/**
	\brief Find native platform for a given chunk of data.
	\note To properly detect platform 64 bytes of data are scanned, so dlen should be at least 64 bytes
	*/
	virtual NxParameterized::Serializer::ErrorType getSerializePlatform(const void *data,PxU32 dlen, NxParameterized::SerializePlatform &platform) = 0;

	/**
	\brief Gets NxParameterized debug rendering color parameters.
	Modiying NxParameterized debug colors automatically updates color table in debug renderer
	*/
	virtual ::NxParameterized::Interface *getDebugColorParams() const = 0;

	/**
	\brief Gets a string for the material to use when rendering wireframe data.
	*/
	virtual const char * getWireframeMaterial(void) = 0;

	/**
	\brief Gets a string for the material to use when rendering solid shaded (per vertex color) lit triangles.
	*/
	virtual const char * getSolidShadedMaterial(void) = 0;

	/***
	\brief Return the ApexSDK internal Foundation pointer if needed by other APEX DLLs
	*/
	virtual physx::shdfnd2::Foundation * getFoundation(void) = 0;

protected:
	virtual ~NxApexSDK() {}

};

PX_POP_PACK

/// \def NXAPEX_API		Export the function declaration from its DLL
/// \def NX_CALL_CONV	Use C calling convention, required for exported functions

#if defined(PX_WINDOWS)
#define NXAPEX_API extern "C" __declspec(dllexport)
#define NX_CALL_CONV __cdecl
#else
#define NXAPEX_API extern "C"
#define NX_CALL_CONV /* void */
#endif

/**
\brief Global function to create the SDK object.  NX_APEX_SDK_VERSION must be passed for the APEXsdkVersion.
*/
NXAPEX_API NxApexSDK*	NX_CALL_CONV NxCreateApexSDK(const NxApexSDKDesc& desc, NxSDKCreateError* errorCode=NULL, physx::PxU32 APEXsdkVersion = NX_APEX_SDK_VERSION);

/**
\brief Returns global SDK pointer.
*/
NXAPEX_API NxApexSDK*	NX_CALL_CONV NxGetApexSDK();

}} // end namespace physx::apex

#endif //__NX_APEX_SDK_H__
