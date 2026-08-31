#ifndef NX_COLLISION_NV_RAW_TRIANGLEMESH
#define NX_COLLISION_NV_RAW_TRIANGLEMESH
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "NxTriangle.h"

class NvRawTriangleMesh
	{
	public:
  
	virtual		NxU32				getNumTriangles()	const		= 0;
	virtual		const NxTriangle*	getTriangles()		const		= 0;

	virtual		NxU32				getRefCount()					= 0;

	protected:
	~NvRawTriangleMesh() {}	//use PhysicsSDK::releaseRawTriMesh() plz.
	};


#endif
