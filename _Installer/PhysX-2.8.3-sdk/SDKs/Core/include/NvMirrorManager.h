#ifndef NX_PHYSICS_NV_MIRRORMANAGER
#define NX_PHYSICS_NV_MIRRORMANAGER
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Nxp.h"
class NxBounds3;
class NvMirrorManagerClient;
class NvShape;

class NvMirrorManager
	{
	public:
	// update from PPU to SceneManager
	// tells SceneManager which AABBs (Packets) are occupied by this fluid.
	// creates and deletes new shapes in the fluid scene
	// the fluidscene will cook new static shapes

	virtual		void	updateAABBs(NvMirrorManagerClient& client, const NxBounds3* newAABBS, void** newUserData, const NxU32 numNew, const NxBounds3* delAABBS, const NxU32 numDel, NxU32 & numActiveShapes)    = 0;
	virtual		void	onNewClientAABB(NvMirrorManagerClient& client, const NxBounds3 & b, void* userData) = 0;

	virtual		void	onDelClientAABB(NvMirrorManagerClient& client, const NxBounds3 & b) = 0;
	virtual		bool	onOutOfHeapMem(NvMirrorManagerClient& caller, const NxU32 minSize, const NxU32 maxSize) = 0;

	virtual		NvShape * startMirroringShape(NvMirrorManagerClient& caller, const NvShape *shape) = 0;
	virtual		NvShape * getPermanentlyMirroredShape(NvMirrorManagerClient& caller, const NvShape *shape) = 0;
	virtual		void      stopMirroringShape(NvMirrorManagerClient& caller, const NvShape *mirroredShape) = 0;

	virtual void updateMirroredShapeRefCounts(const NvMirrorManagerClient* client, const NvShape** shapesList, const NxI32* refCountDeltaList, NxU32 numShapes) = 0;

	///new ability to pre stance and cook for fluids.
	virtual bool preCacheStaticMesh(NvScene* scene, NvMirrorManagerMeshClient& meshClient, const NxBounds3& aabb) = 0;
	};

#endif
