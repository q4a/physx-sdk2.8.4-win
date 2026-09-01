#ifndef NX_EXTENSION_QUICK_LOAD_H
#define NX_EXTENSION_QUICK_LOAD_H

#include "NxExtensions.h"

#define NX_EXT_QUICK_LOAD_VERSION 100

class NxConvexMeshDesc;
class NxStream;

/**
An extension that accelerates the cooking and loading of convexes through improved memory management.
*/
class NxExtensionQuickLoad : public NxExtension
	{
	protected:
	NX_INLINE NxExtensionQuickLoad(){}
	virtual ~NxExtensionQuickLoad(){}

	public:

	/**
	Used in wrapAllocator's mode parameter.
	*/
	enum Mode
	{
		/**
		In this mode, NxPhysicsSDK::releaseConvexMesh may have an NxConvexMesh passed in that was created with
		NxExtensionQuickLoad::createConvexMesh.  This also means that when the SDK is released, it can release
		any remaining convex meshes whether they are created by PhysX or by Quickload.  However, this incurs a
		small overhead per free(), regardless of whether or not it pertains to a convex mesh.

		Safe: Yes
		Correct: Yes
		Fastest: No
		*/
		AllowSDKToReleaseQLMesh,

		/**
		In this mode, the user must always call NxExtensionQuickLoad::releaseConvexMesh to release a mesh
		created by Quickload.  The SDK will not know about Quickload meshes, so it will not attempt to
		release them itself.  Thus, this mode is safe and fast.  However, some bugs may occur because of the
		removal of Quickload meshes from the SDK's internal list.  One known bug is that the PVD (formally the
		VRD) will not display convex meshes in the scene.

		Safe: Yes
		Correct: No
		Fastest: Yes
		*/
		RemoveSDKReferencesToQLMesh,

		/**
		In this mode, the application *must* release all convex meshes created by Quickload (using
		NxExtensionQuickLoad::releaseConvexMesh).  Failing to do so will result in a crash upon SDK release,
		since the SDK will hold references to Quickload-created meshes.  The advantage of this is that PVD
		behavior is correct (see the bug described in AllowSDKToReleaseQLMesh above).  This method is also
		fast.

		Safe: No
		Correct: Yes
		Fastest: Yes
		*/
		RequireUserReleaseOfQLMesh
	};

	/**
	Retrieves the NxQuickLoadAllocator

	You must use this allocator with the SDK if you are going to use the NxQuickLoad library.
	It wraps the user's allocator.  wrapAllocator() can only be returned once.  Subsequent calls return NULL.
	Alignment must be a power of two.

	See the Mode enum for a desription of the mode parameter.

	Usage:
	1) create your own user allocator
	2) call wrapAllocator
	3) create the NxPhysicsSDK using the wrapped allocator
	4) set the SDK using the setSDK() call below
	*/
	virtual NxUserAllocator * wrapAllocator(NxUserAllocator & userAllocator, Mode mode = AllowSDKToReleaseQLMesh, NxU32 alignment = 4) = 0;

	/**
	Registers the SDK library with the QuickLoad object.
	This needs to be called before calls below.  Can only be called once.
	NxInitCooking() should already have been called on the cooking interface.

	Pass NX_PHYSICS_SDK_VERSION as interfaceVersion.
	Returns false on a version mismatch or if wrapAllocator() has not yet been called.
	*/
	virtual bool initialize(NxPhysicsSDK &, NxU32 interfaceVersion = NX_PHYSICS_SDK_VERSION) = 0;

	/**
	Replacement for the SDK's own cookConvexMesh.  Requires a cooking interface to use for the cooking.
	*/
	virtual bool cookConvexMesh(const NxConvexMeshDesc& desc, NxStream& stream, NxCookingInterface &) = 0;
	/**
	Replacement for the SDK's own createConvexMesh
	*/
	virtual NxConvexMesh * createConvexMesh(NxStream& stream) = 0;
	/**
	Replacement for the SDK's own releaseConvexMesh
	*/
	virtual void releaseConvexMesh(NxConvexMesh& mesh) = 0;
	};


#endif // #ifndef NX_EXTENSION_QUICK_LOAD_H
