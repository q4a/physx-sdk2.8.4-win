#ifndef NX_COLLISION_NV_TRIANGLEMESH
#define NX_COLLISION_NV_TRIANGLEMESH
/*----------------------------------------------------------------------------*\
|
|								NovodeX Technology
|
|							     www.novodex.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"


class MeshMassInfo
	{
	public:
	NxReal				mass;			//this is mass assuming a unit density that can be scaled by instances!
	NxMat33				inertia;		//in local space of mesh!
	NxVec3				centerOfMass;	//in local space of mesh!

	MeshMassInfo()		{ mass = -1;	}
	};

class NxTriangleMeshDesc;
class NxPMap;
class NxStream;
class NpTriangleMesh;

class NvTriangleMesh
	{
	public:
	virtual		void						release() = 0;
	//MSS HACK_TRIMESH_RELEASE
	virtual		void						releaseInternalMesh() = 0;
	virtual		bool						load(const NxStream& stream) = 0;
	virtual		bool						saveToDesc(NxTriangleMeshDesc&)				const = 0;

	virtual		bool						loadPMap(const NxPMap&) = 0;
	virtual		bool						hasPMap()									const = 0;
	virtual		NxU32						getPMapSize()								const = 0;
	virtual		bool						getPMapData(NxPMap& pmap)					const = 0;
	virtual		NxU32						getPMapDensity()							const = 0;

	virtual		NxMaterialIndex				getTriangleMaterial(NxTriangleID triangleIndex)	const	= 0;

	virtual		NxU32						getSubmeshCount()							const = 0;
	virtual		NxU32						getCount(NxSubmeshIndex, NxInternalArray)	const = 0;
	virtual		NxInternalFormat			getFormat(NxSubmeshIndex, NxInternalArray)	const = 0;
	virtual		const void*					getBase(NxSubmeshIndex, NxInternalArray)	const = 0;
	virtual		NxU32						getStride(NxSubmeshIndex, NxInternalArray)	const = 0;

	virtual		NpTriangleMesh*				getNpTriangleMesh() = 0; 
	virtual		void						setNpTriangleMesh(NpTriangleMesh* m) = 0;

	//only needed as long as we have to compute inertia tensors for bodies based on shapes:
	virtual		const MeshMassInfo*			getMassInformation()						const = 0;		//returns 0 when tensor computation failed.

	virtual		NxU32						getPageCount()								const = 0;
	virtual		NxBounds3					getPageBBox(NxU32 pageIndex)				const = 0;
	virtual		void *						getPage(NxU32 pageIndex, NxU32 &length)		const = 0;

	protected:
	~NvTriangleMesh() {}	//use release() plz.
	};


#endif
