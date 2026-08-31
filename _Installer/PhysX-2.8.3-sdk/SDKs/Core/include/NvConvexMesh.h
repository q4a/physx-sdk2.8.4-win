#ifndef NX_COLLISION_NV_CONVEXMESH
#define NX_COLLISION_NV_CONVEXMESH
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/


#include "Physics.h"

class NxConvexMeshDesc;
class NxStream;
class MeshMassInfo;
class NpConvexMesh;

class NvConvexMesh
	{
	public:

	virtual		void						release() = 0;

	virtual		bool						saveToDesc(NxConvexMeshDesc&)				const = 0;
	virtual		NxU32						getSubmeshCount()							const = 0;
	virtual		NxU32						getCount(NxSubmeshIndex, NxInternalArray)	const = 0;
	virtual		NxInternalFormat			getFormat(NxSubmeshIndex, NxInternalArray)	const = 0;
	virtual		const void*					getBase(NxSubmeshIndex, NxInternalArray)	const = 0;
	virtual		NxU32						getStride(NxSubmeshIndex, NxInternalArray)	const = 0;
	virtual		bool						load(const NxStream& stream) = 0;
	virtual		NpConvexMesh*				getNpConvexMesh() = 0;
	virtual		void						setNpConvexMesh(NpConvexMesh*p) = 0;
	virtual		bool						canRunOnHW() const = 0;
	//only needed as long as we have to compute inertia tensors for bodies based on shapes:
	virtual		 MeshMassInfo				getMassInformation()						const = 0;				//returns 0 when tensor computation failed.

	protected:
	~NvConvexMesh() {}	//use release() plz.
	};

#endif
