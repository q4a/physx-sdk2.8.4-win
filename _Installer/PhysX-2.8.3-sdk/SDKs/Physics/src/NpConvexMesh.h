#ifndef NX_COLLISION_NP_CONVEXMESH
#define NX_COLLISION_NP_CONVEXMESH
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxFoundation.h"
#include "NxConvexMesh.h"
#include "Allocateable.h"
#include "RefCountable.h"

class NvConvexMesh;
class NxSimpleTriangleMesh;
class NxConvexMeshDesc;

class NpConvexMesh : public NxConvexMesh, public RefCountable, public NxFoundation::NxAllocateable
	{
	
	public:
										NpConvexMesh(NvConvexMesh* mesh);
	virtual								~NpConvexMesh();

	// Implements NxTriangleMesh
	virtual		bool					saveToDesc(NxConvexMeshDesc&)	const;

	virtual		NxU32					getSubmeshCount()							const;
	virtual		NxU32					getCount(NxSubmeshIndex, NxInternalArray)	const;
	virtual		NxInternalFormat		getFormat(NxSubmeshIndex, NxInternalArray)	const;
	virtual		const void*				getBase(NxSubmeshIndex, NxInternalArray)	const;
	virtual		NxU32					getStride(NxSubmeshIndex, NxInternalArray)	const;

	virtual		bool					load(const NxStream& stream);
	NX_INLINE	NvConvexMesh*			getConvexMesh()						{ return mMesh;	}
	NX_INLINE	const NvConvexMesh*		getConvexMeshConst()		const	{ return mMesh;	}
	void *								getInternal();
	virtual		NxU32					getReferenceCount()					{ return getNumRefs(); }
	virtual		void					getMassInformation(NxReal& mass, NxMat33& localInertia, NxVec3& localCenterOfMass)	const;

				bool					loadFast(const NxStream& stream); // Not locking scenes

				void					sendToVRD(bool create, bool fromCtor);
				void					removeFromVRD();

	private:
				NvConvexMesh*			mMesh;
	};

#endif
