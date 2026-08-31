#ifndef NX_PHYSICS_NV_MIRRORMANAGER_CLIENT
#define NX_PHYSICS_NV_MIRRORMANAGER_CLIENT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Nxp.h"
#include "NvShape.h"

class NxBounds3;
class NvScene;
class NvRawTriangleMesh;

enum NvMirrorManagerClientType
	{
	NV_MIRROR_MANAGER_CLIENT_TYPE_RB				= 0,	
	NV_MIRROR_MANAGER_CLIENT_TYPE_CLOTH				= 1,	
	NV_MIRROR_MANAGER_CLIENT_TYPE_FLUID				= 2,
	NV_MIRROR_MANAGER_CLIENT_TYPE_MESH_PRECOOKER	= 3,
	NV_MIRROR_MANAGER_CLIENT_TYPE_SOFTBODY			= 4,
	};

class NvMirrorManagerMeshClient
	{
	public:

	virtual bool isCompatibleWithStaticMesh(NvRawTriangleMesh &) const = 0;
	virtual NxU32 getClientType() const = 0;

	// Check whether a static shape needs instant mirroring.
	static bool isInstantMirrorStaticShape(const NvShape& shape)
		{
		return ( (shape.getNvBody() == NULL) && ((shape.getType() == NX_SHAPE_MESH) || (shape.getType() == NX_SHAPE_HEIGHTFIELD)) );
		}
	};

class NvMirrorManagerClient: public NvMirrorManagerMeshClient
	{
	public:

	virtual NvScene * getOwnerScene() const = 0;
	virtual void addStaticShapeToRegion(NvShape &,const NxBounds3&) = 0;
	virtual void removeStaticShapeFromRegion(NvShape &,const NxBounds3&) = 0;
	virtual void addDynamicShape(NvShape& shape) = 0;
	virtual NxReal getImpulseTransferSmoothingAlpha() const = 0;

	virtual void addInteractionWithRbShape(NvShape& nvShape, void* userData) = 0;
	virtual void removeInteractionWithRbShape(NvShape& nvShape, void* userData) = 0;
	virtual void passShapeRefCountChanges() = 0;

	// in this call back, it is safe to access the parent (sw) scene
	virtual void accessParentScene() = 0;

	// collision filtering
	virtual NxCollisionGroup getGroup() const = 0;
	virtual const NxGroupsMask* getGroupsMask() const = 0;
	};

#endif
