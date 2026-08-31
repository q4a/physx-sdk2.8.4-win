#ifndef NP_PHYSICS_PHYSICS_SDK
#define NP_PHYSICS_PHYSICS_SDK
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxPhysicsSDK.h"
#include "NvPhysicsSDK.h"
#include "ReadWriteMutex.h"
#include "Allocateable.h"
#include "NxMutex.h"
#include "NxLibrary.h"
#include "NxInterfaceStats.h"
#include "NpUtilLib.h"
#include "NpScene.h"
#include "NxCompartmentDesc.h"
#include "SortedSet.h"
#if !defined(__CELLOS_LV2__)
#include "PhysXIndicator.h"
#endif
#ifdef LINUX
#include <string.h>
#endif

class NpTriangleMesh;
class NpConvexMesh;
class NpHeightField;
class NpCCDSkeleton;

// NP_DYNAMIC_CORE is a true value if we are linking the core dynamicly.
// We should link dynamicly on anything that can use PhysX.
#if defined(WIN32) && !defined(NX_USE_SDK_STATICLIBS)
	#define NP_DYNAMIC_CORE 1
	#define NP_PHYSX_CORE   1 // Use a single multipurpose core (ie, PhysXCore.dll)
#else
	#define NP_DYNAMIC_CORE 0
#endif


struct NameBinding
	{
	NX_INLINE	NameBinding() : object(NULL), name(NULL) {}
	NX_INLINE	NameBinding(const void* o, const char* n) : object(o), name(n) {}
	const void* object;
	const char* name;

	NX_INLINE	bool operator ==(const NameBinding& m) const { return (object == m.object); }
	NX_INLINE	bool operator >(const NameBinding& m) const { return (object > m.object); }
	NX_INLINE	bool operator >=(const NameBinding& m) const { return (object >= m.object); }
	NX_INLINE	bool operator <(const NameBinding& m) const { return (object < m.object); }
	NX_INLINE	bool operator <=(const NameBinding& m) const { return (object <= m.object); }
	};

#ifndef NX_DISABLE_REMOTE_DEBUG
class NpRemoteDebuggerEventListener : public NxRemoteDebuggerEventListener
{
public:
	void setSDK(NpPhysicsSDK* physicsSDK) { sdk = physicsSDK; }
	void onConnect();
	void onDisconnect();
	void beforeMaskChange(NxU32 oldMask, NxU32 newMask);
	void afterMaskChange(NxU32 oldMask, NxU32 newMask);
private:
	NpPhysicsSDK* sdk;
};
#endif

template<class T>
class NxLockedIterator
{
public:
	NxLockedIterator(const NxFoundation::NxArraySDK<T*>& arr, NxMutex& mutex)
		: mArray(arr), mMutex(mutex), mIndex(0)
	{
		mMutex.lock();
	}

	~NxLockedIterator()
	{
		mMutex.unlock();
	}
	
#if defined(CAFE)
	NxLockedIterator (NxLockedIterator const& a)
	: mArray(a.mArray), mMutex(a.mMutex), mIndex(a.mIndex)
	{
		// Lock to match dtor's unlock
		mMutex.lock ();
	}

private:	
	// disallow default operator=
	NxLockedIterator& operator=(NxLockedIterator& a);
	
public:
#endif

	T* getNext()
	{
		return mIndex < mArray.size() ? mArray[mIndex++] : NULL;
	}

private:
	const NxFoundation::NxArraySDK<T*>& mArray;
	NxMutex& mMutex;
	NxU32 mIndex;
};


struct NpPhysicsTls: public NxFoundation::NxAllocateable
{
	NX_INLINE NpPhysicsTls()
		: mNpLinearKernelRnd(42) {}

	// list of TLS values per thread
	NxU32		mNpLinearKernelRnd;
	//NxU32		_pad[15];
};

class NpPhysicsSDK : public NxPhysicsSDK, public NxFoundation::NxAllocateable, public NxInterfaceStats
{
public:
									NpPhysicsSDK(const NxPhysicsSDKDesc &desc, NxSDKCreateError& errorCode);
	virtual							~NpPhysicsSDK();

	virtual		void				release();

	virtual		bool				setParameter(NxParameter paramEnum, NxReal paramValue);
	virtual		NxReal				getParameter(NxParameter paramEnum) const;

	virtual		NxScene*			createScene(const NxSceneDesc&);
	virtual		void				releaseScene(NxScene&);
	virtual		NxU32				getNbScenes()	const;
	virtual		NxScene*			getScene(NxU32 i);

	virtual		NxTriangleMesh*		createTriangleMesh(const NxStream&);
	// MSS HACK_TRIMESH_RELEASE
	virtual       void              releaseInternalMeshOnly(NxTriangleMesh& m);
	virtual		void				releaseTriangleMesh(NxTriangleMesh&);
	virtual		NxU32				getNbTriangleMeshes() const				{ return triangleMeshArray.size();	}
				NxLockedIterator<NpTriangleMesh> getTriangleMeshes(); // Not exposed through API


	virtual		NxConvexMesh*		createConvexMesh(const NxStream&);
	virtual		void				releaseConvexMesh(NxConvexMesh&);
	virtual		NxU32				getNbConvexMeshes() const				{ return convexMeshArray.size();	}

	virtual		NxHeightField*		createHeightField(const NxHeightFieldDesc&);
	virtual		void				releaseHeightField(NxHeightField&);
	virtual		NxU32				getNbHeightFields() const				{ return heightFieldArray.size();	}


	virtual		NxCCDSkeleton *		createCCDSkeleton(const NxSimpleTriangleMesh&);
	virtual		NxCCDSkeleton *		createCCDSkeleton(const void * memoryBuffer, NxU32 bufferSize);
	virtual		void				releaseCCDSkeleton(NxCCDSkeleton&);
	virtual		NxU32				getNbCCDSkeletons() const				{ return skeletonArray.size();		}

#if NX_USE_CLOTH_API
	virtual		NxClothMesh*		createClothMesh(NxStream& stream);
	virtual		void				releaseClothMesh(NxClothMesh&);
	virtual		NxU32				getNbClothMeshes() const;
	virtual		NxClothMesh**		getClothMeshes();
#endif

#if NX_USE_SOFTBODY_API
	virtual		NxSoftBodyMesh*		createSoftBodyMesh(NxStream& stream);
	virtual		void				releaseSoftBodyMesh(NxSoftBodyMesh&);
	virtual		NxU32				getNbSoftBodyMeshes() const;
	virtual		NxSoftBodyMesh**	getSoftBodyMeshes();
#endif

	virtual		NxFoundationSDK&	getFoundationSDK()		const;

	virtual NxU32 getInternalVersion(NxU32 &apiRev,NxU32 &descRev,NxU32 &branchId)			const;
	virtual NxInterface * getInterface(NxInterfaceType type,int versionNumber);
	virtual bool	          getHeapSize(int &used,int &unused);

	virtual NxHWVersion				getHWVersion() const;
	virtual NxU32					getReferenceCount() const { return mRefCount; }
	virtual void					incReferenceCount() { mRefCount++; }
	virtual NxU32					getNbPPUs() const;

	NX_INLINE	NxMutex&			getSceneMutex() { return mSceneRunning; }


	NX_INLINE	NxU32				getNbScenesFast()	const	{	return sceneArray.size();	}
	NX_INLINE	NpScene*			getSceneFast(NxU32 i)		{	NX_ASSERT(i < sceneArray.size()); return sceneArray[i];		}
#if NP_DYNAMIC_CORE
				NvScene *			createCompartment(NvSceneDesc &desc, 
													  NxDeviceCode deviceCode, 
													  NvSceneType type);
#else
	NX_INLINE	NvPhysicsSDK*		getPhysicsSDK()  { return mPhysicsSDK; }
#endif

				void				sendToVRD();
				void				removeFromVRD();

				bool				hasHardwareSupport(NxCompartmentType);
				NpPhysicsTls*		getNpPhysicsTls();

/*
	// TODO: get rid of these functions.
	//       replace with cookPmap and freePmap?
	//       isnt cooking done in, well, NxCooking now?
#if NP_DYNAMIC_CORE
//	NX_INLINE	NvPhysicsSDK*		getPhysicsSDK() const		{ NX_ASSERT(0); return 0; }
	// Used by pmap
	NX_INLINE	NvPhysicsSDK*		getPhysicsSDK() const		{ return mSWPhysicsSDK ? mSWPhysicsSDK : mHWPhysicsSDK; }
#else
	NX_INLINE	NvPhysicsSDK*		getPhysicsSDK() const		{ return mPhysicsSDK; }
#endif
*/
	static		const char*			getName(const void* object);
	static		bool				setName(const void* object, const char* name);
	// PT: interesting to see that this one below can't be inlined!
	static		bool				removeObjectName(const void* object)	{ return setName(object, NULL);	}

//group access variables:
	static		SortedSet<NameBinding>*	nameManager;

	static const char * reentryErrorMessage;
	static NX_BOOL apiReentryLock;

	static NpPhysicsSDK	*		instance;
	static NxFoundationSDK*		foundationSDK;

private:
	void						createVersionString(char* inoutString, NxU32 version);

	NxFoundation::NxArraySDK<NpScene*>				sceneArray;
	NxFoundation::NxArraySDK<NpCCDSkeleton*>	skeletonArray;
	NxMutex										skeletonMutex;

#if NX_USE_CLOTH_API
	typedef		NxFoundation::NxArraySDK<NpClothMesh*>	NpClothMeshArray;
	NpClothMeshArray			npClothMeshArray;	//  for NpScene::getClothMeshes()
	NxMutex						clothMeshMutex;
#endif

#if NX_USE_SOFTBODY_API
	typedef		NxFoundation::NxArraySDK<NpSoftBodyMesh*>	NpSoftBodyMeshArray;
	NpSoftBodyMeshArray			npSoftBodyMeshArray;	//  for NpScene::getSoftBodyMeshes()
	NxMutex						softBodyMeshMutex;
#endif

	NxMutex											triangleMeshMutex;
	NxMutex											convexMeshMutex;
	NxMutex											heightFieldMutex;
	NxFoundation::NxArraySDK<NpTriangleMesh*>		triangleMeshArray;
	NxFoundation::NxArraySDK<NpConvexMesh*>			convexMeshArray;
	NxFoundation::NxArraySDK<NpHeightField*>		heightFieldArray;

	NxMutex						mSceneRunning;
	NxU32						mRefCount;
#if NP_DYNAMIC_CORE
#if !NP_PHYSX_CORE
	NxLibrary                   mSWCoreLib;
	NxLibrary					mHWCoreLib;
#endif
	NvPhysicsSDK*				mSWPhysicsSDK;
	NvPhysicsSDK*				mPPUPhysicsSDK;
	NvPhysicsSDK*				mGPUPhysicsSDK;
	bool						mUsingGpu;
#else
	NvPhysicsSDK*				mPhysicsSDK;
#endif
	NxTLSId						mTlsSlot;
	NxMutex						mTlsLock;
	NxFoundation::NxArraySDK<NpPhysicsTls*>		
								mTlsStructs;
#ifndef NX_DISABLE_REMOTE_DEBUG
	NpRemoteDebuggerEventListener mVRDListener;
#endif

#if defined(WIN32) && NP_DYNAMIC_CORE
	PhysXCpuIndicator			mPhysXIndicator;
#endif
};

/**
\brief Creates an instance of the physics SDK.

Creates an instance of this class. May not be a class member to avoid name mangling.
Pass the constant NX_PHYSICS_SDK_VERSION as the argument.
Because the class is a singleton class, multiple calls return the same object.

\include NxCreatePhysicsSDK.cpp

\param sdkVersion Version number we are expecting(should be NX_PHYSICS_SDK_VERSION)
\param allocator User supplied interface for allocating memory(see #NxUserAllocator)
\param outputStream User supplied interface for reporting errors and displaying messages(see #NxUserOutputStream)
*/
NX_C_EXPORT NXP_DLL_EXPORT NxPhysicsSDK* NX_CALL_CONV NpCreatePhysicsSDK(NxU32 sdkVersion, NxUserAllocator* allocator, NxUserOutputStream* outputStream, const NxPhysicsSDKDesc &desc, NxSDKCreateError& errorCode);

/**
\brief Retrieves the Physics SDK allocator.

Used by NxAllocateable's inlines and other macros below.
*/
NX_C_EXPORT NXP_DLL_EXPORT NxUserAllocator* NpGetPhysicsSDKAllocator();

/**
\brief Retrieves the Foundation SDK after it has been created.
*/
NX_C_EXPORT NXP_DLL_EXPORT NxFoundationSDK* NpGetFoundationSDK();

/**
\brief Retrieves the Physics SDK after it has been created.
*/
NX_C_EXPORT NXP_DLL_EXPORT NxPhysicsSDK* NpGetPhysicsSDK();

/**
\brief Retrieves and instance of the utility library(former static SDK exports)
*/
NX_C_EXPORT NXP_DLL_EXPORT NxUtilLib* NX_CALL_CONV NpGetUtilLib();

/**
\brief Releases the Physics SDK object and returns the new reference count, if 0 then
the SDK was really released, otherwise only the reference counter was lowered.

\return New reference count value of the Physics SDK object
*/
NX_C_EXPORT NXP_DLL_EXPORT NxU32 NX_CALL_CONV NpReleasePhysicsSDK(NxPhysicsSDK* sdk);

/*
API reentry testing

Put on 1st line of a void API call that is OK to be reentered, but wil be calling callbacks (otherwise its pointless):
API_REENTRY_OK

Put on 1st line of a void API call that should not be reentered:
API_NO_REENTRY

Put on 1st line of an int or void * API call that should not be reentered:
API_NO_REENTRY_R0

*/

struct APILock
	{
	NX_INLINE APILock()
		{
		NpPhysicsSDK::apiReentryLock = true;
		}
	NX_INLINE ~APILock()
		{
		NpPhysicsSDK::apiReentryLock = false;
		}
	};

#define REENTRY_TEST	if (NpPhysicsSDK::apiReentryLock)   { NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_OPERATION, __FILE__, __LINE__, 0, NpPhysicsSDK::reentryErrorMessage); return; }
#define REENTRY_TEST_R0	if (NpPhysicsSDK::apiReentryLock)   { NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_OPERATION, __FILE__, __LINE__, 0, NpPhysicsSDK::reentryErrorMessage); return 0; }
#define API_REENTRY_OK						APILock apiLock;
#define API_NO_REENTRY		REENTRY_TEST	APILock apiLock;
#define API_NO_REENTRY_R0	REENTRY_TEST_R0	APILock apiLock;

/*
Deprecating API methods
*/
#define DEPRECATED(string)	static bool deprecWarned = false; if (!deprecWarned) { deprecWarned = true;  NxFoundation::FoundationSDK::getInstance().error(NX_PRINT, 0, "Warning: deprecated method: " string "\n"); }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// methods to lock and unlock all Scenes 
//

// automatic unlock of all locked write mutexes
class NxMutexUnLockAllWrite
{
public:
	NX_INLINE NxMutexUnLockAllWrite() : lockedScenes(0)
		{
		}

	NX_INLINE void addMutex() 
		{
		lockedScenes++;
		} 

	NX_INLINE void removeMutex() 
		{
		if(lockedScenes > 0)
			lockedScenes--;
		}

	NX_INLINE ~NxMutexUnLockAllWrite() 
		{
		for (NxU32 i = 0; i < lockedScenes; i++)
			{
			::NpPhysicsSDK::instance->getSceneFast(i)->EndWrite();

			}
		}

private:
	NxU32 lockedScenes;
};

// automatic unlock of all locked read mutexes
class NxMutexUnLockAllRead
{
public:
	NX_INLINE NxMutexUnLockAllRead() : lockedScenes(0)
		{
		}

	NX_INLINE void addMutex() 
		{
		lockedScenes++;
		}

	NX_INLINE void removeMutex() 
		{
		if(lockedScenes > 0)
			lockedScenes--;
		}

	NX_INLINE ~NxMutexUnLockAllRead() 
		{
		for (NxU32 i = 0; i < lockedScenes; i++)
			{
			::NpPhysicsSDK::instance->getSceneFast(i)->EndRead();

			}
		}

private:
	NxU32 lockedScenes;
};

#define BEGINREADLOCKALLSCENES	{}

/*
#define BEGINREADLOCKALLSCENES																				\
		NxMutexUnLockAllRead mutexList;																		\
		for(NxU32 i = 0; i < NpPhysicsSDK::instance->getNbScenesFast(); i++)								\
			{																								\
			NpPhysicsSDK::instance->getSceneFast(i)->BeginRead();											\
			mutexList.addMutex();																			\
			}
*/

#define BEGINWRITELOCKALLSCENES																				\
		NxMutexUnLockAllWrite mutexList;																	\
		for(NxU32 i = 0; i < NpPhysicsSDK::instance->getNbScenesFast(); i++)								\
			{																								\
			if (!NpPhysicsSDK::instance->getSceneFast(i)->tryBeginWrite())									\
				{																							\
				NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_OPERATION, __FILE__, __LINE__, 0, "PhysicsSDK: %s: WriteLock is still acquired. Procedure call skipped to avoid a deadlock!", __FUNCTION__);	\
				return; 																					\
				}																							\
			else																							\
				{																							\
				mutexList.addMutex();																		\
				}																							\
			}

#define BEGINWRITELOCKALLSCENESCOND(condition)																\
		NxMutexUnLockAllWrite mutexList;																	\
		if (condition)																						\
			{																									\
			for(NxU32 i = 0; i < NpPhysicsSDK::instance->getNbScenesFast(); i++)								\
				{																								\
				if (!NpPhysicsSDK::instance->getSceneFast(i)->tryBeginWrite())									\
					{																							\
					NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_OPERATION, __FILE__, __LINE__, 0, "PhysicsSDK: %s: WriteLock is still acquired. Procedure call skipped to avoid a deadlock!", __FUNCTION__);	\
					return; 																					\
					}																							\
				else																							\
					{																							\
					mutexList.addMutex();																		\
					}																							\
				}																								\
			}


#define BEGINWRITELOCKALLSCENES_NULL																		\
		NxMutexUnLockAllWrite mutexList;																	\
		for(NxU32 i = 0; i < NpPhysicsSDK::instance->getNbScenesFast(); i++)								\
			{																								\
			if (!NpPhysicsSDK::instance->getSceneFast(i)->tryBeginWrite())									\
				{																							\
				NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_OPERATION, __FILE__, __LINE__, 0, "PhysicsSDK: %s: WriteLock is still acquired. Procedure call skipped to avoid a deadlock!", __FUNCTION__);	\
				return NULL;																				\
				}																							\
			else																							\
				{																							\
				mutexList.addMutex();																		\
				}																							\
			}

#define BEGINWRITELOCKALLSCENESCOND_NULL(condition)															\
		NxMutexUnLockAllWrite mutexList;																	\
		if (condition)																						\
			{																								\
			for(NxU32 i = 0; i < NpPhysicsSDK::instance->getNbScenesFast(); i++)								\
				{																								\
				if (!NpPhysicsSDK::instance->getSceneFast(i)->tryBeginWrite())									\
					{																							\
					NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_OPERATION, __FILE__, __LINE__, 0, "PhysicsSDK: %s: WriteLock is still acquired. Procedure call skipped to avoid a deadlock!", __FUNCTION__);	\
					return NULL;																				\
					}																							\
				else																							\
					{																							\
					mutexList.addMutex();																		\
					}																							\
				}																								\
			}
#endif
