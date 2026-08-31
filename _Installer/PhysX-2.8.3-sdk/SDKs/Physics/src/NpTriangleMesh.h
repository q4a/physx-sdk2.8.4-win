#ifndef NX_COLLISION_NP_TRIANGLEMESH
#define NX_COLLISION_NP_TRIANGLEMESH
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxFoundation.h"
#include "NxTriangleMesh.h"
#include "Allocateable.h"
#include "RefCountable.h"

class NvTriangleMesh;
class NxTriangleMeshShape;
class NxSimpleTriangleMesh;
class NxTriangleMeshDesc;
class NxTriangleMeshShapeDesc;
class NxPMap;

class NpTriangleMesh : public NxTriangleMesh, public RefCountable, public NxFoundation::NxAllocateable
	{
	
	public:
										NpTriangleMesh(NvTriangleMesh* mesh);
	virtual								~NpTriangleMesh();

	// Implements NxTriangleMesh
	virtual		bool					saveToDesc(NxTriangleMeshDesc&)	const;

	virtual		bool					loadPMap(const NxPMap&);
	virtual		bool					hasPMap()					const;
	virtual		NxU32					getPMapSize()				const;
	virtual		bool					getPMapData(NxPMap& pmap)	const;
	virtual		NxU32					getPMapDensity()			const;

	virtual		NxMaterialIndex			getTriangleMaterial(NxTriangleID triangleIndex)	const;

	virtual		NxU32					getSubmeshCount()							const;
	virtual		NxU32					getCount(NxSubmeshIndex, NxInternalArray)	const;
	virtual		NxInternalFormat		getFormat(NxSubmeshIndex, NxInternalArray)	const;
	virtual		const void*				getBase(NxSubmeshIndex, NxInternalArray)	const;
	virtual		NxU32					getStride(NxSubmeshIndex, NxInternalArray)	const;

	virtual		bool					load(const NxStream& stream);

	NX_INLINE	NvTriangleMesh*			getTriangleMesh()					{ return mMesh;	}
	NX_INLINE	const NvTriangleMesh*	getTriangleMeshConst()		const	{ return mMesh;	}

	virtual		NxU32					getPageCount() const;
	virtual		NxBounds3				getPageBBox(NxU32 pageIndex) const;

	virtual		NxU32					getReferenceCount()					{ return getNumRefs(); }

	virtual		void					getMassInformation(NxReal& mass, NxMat33& localInertia, NxVec3& localCenterOfMass)		const;

				bool					loadFast(const NxStream& stream); // Not locking scenes

				void					sendToVRD(bool create, bool fromCtor);
				void					removeFromVRD();
	// MSS HACK_TRIMESH_RELEASE
	virtual			void					releaseInternalMesh();

	private:
				NvTriangleMesh*			mMesh;
	};

#endif
