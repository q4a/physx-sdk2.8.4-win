#ifndef NX_PHYSICS_NV_CLOTHMESH
#define NX_PHYSICS_NV_CLOTHMESH

/*----------------------------------------------------------------------------*\
|
| NVIDIA PhysX Technology
|
| www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "DeformableDesc.h"

class NpClothMesh;
class NpSoftBodyMesh;
class DeformableMeshDesc;
class NxStream;

/*----------------------------------------------------------------------------*/

class NvClothMesh
{
public:
	virtual	bool saveToDesc(DeformableMeshDesc& desc) const = 0;
	virtual	bool load(const NxStream& stream)		= 0;

	virtual void setNpClothMesh(NpClothMesh*) = 0;
	virtual NpClothMesh* getNpClothMesh() = 0;

	virtual void setNpSoftBodyMesh(NpSoftBodyMesh*) = 0;
	virtual NpSoftBodyMesh* getNpSoftBodyMesh() = 0;

	virtual DeformableType getDeformableType() = 0;
};

/*----------------------------------------------------------------------------*/

#endif
