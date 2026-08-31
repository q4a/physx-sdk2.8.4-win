#ifndef NX_PHYSICS_DEFORMABLEMESH_DESC
#define NX_PHYSICS_DEFORMABLEMESH_DESC

#include "NxClothMeshDesc.h"
#include "NxSoftBodyMeshDesc.h"
#include "DeformableDesc.h"

// a common descriptor for cloth and soft body meshes

/*----------------------------------------------------------------------------*/

class DeformableMeshDesc
{
public:
	DeformableType deformableType;	// cloth or softbody

	NxU32 numVertices;				// both
	NxU32 vertexStrideBytes;		// both

	NxU32 numTriangles;				// cloth
	NxU32 triangleStrideBytes;		// cloth

	NxU32 numTetrahedra;			// softbody
	NxU32 tetrahedronStrideBytes;	// softbody

	const void* vertices;			// both
	const void* triangles;			// cloth
	const void* tetrahedra;			// softbody

	NxU32 numHierarchyLevels;

	NxReal weldingDistance;			// only cloth supports (needs) welding

	NxU32  flags;					// cloth convention used (NxClothMeshFlags)

	NxU32 vertexMassStrideBytes;	// both
	NxU32 vertexFlagStrideBytes;	// both
	
	const void* vertexMasses;		// both
	const void* vertexFlags;		// both

	NX_INLINE DeformableMeshDesc();

	NX_INLINE void clear();
	NX_INLINE bool isValid() const;

	// conversion
	NX_INLINE void loadFromClothMeshDesc(const NxClothMeshDesc &desc);
	NX_INLINE void saveToClothMeshDesc(NxClothMeshDesc &desc) const;

	NX_INLINE void loadFromSoftBodyMeshDesc(const NxSoftBodyMeshDesc &desc);
	NX_INLINE void saveToSoftBodyMeshDesc(NxSoftBodyMeshDesc &desc) const;
};

/*----------------------------------------------------------------------------*/

NX_INLINE DeformableMeshDesc::DeformableMeshDesc()
{
	clear();
}

/*----------------------------------------------------------------------------*/
void DeformableMeshDesc::loadFromClothMeshDesc(const NxClothMeshDesc &desc)
{
	deformableType       = DEFORMABLE_TYPE_CLOTH;

	numVertices          = desc.numVertices;
	vertexStrideBytes    = desc.pointStrideBytes;
	numTriangles         = desc.numTriangles;
	triangleStrideBytes  = desc.triangleStrideBytes;

	vertices        = desc.points;
	triangles       = desc.triangles;
	weldingDistance = desc.weldingDistance;
	flags           = desc.flags;

	vertexMassStrideBytes = desc.vertexMassStrideBytes;
	vertexFlagStrideBytes = desc.vertexFlagStrideBytes;
	vertexMasses          = desc.vertexMasses;
	vertexFlags           = desc.vertexFlags;

	numHierarchyLevels    = desc.numHierarchyLevels;
}

/*----------------------------------------------------------------------------*/
void DeformableMeshDesc::saveToClothMeshDesc(NxClothMeshDesc &desc) const
{
	desc.setToDefault();

	desc.numVertices          = numVertices;
	desc.pointStrideBytes     = vertexStrideBytes;
	desc.numTriangles         = numTriangles;
	desc.triangleStrideBytes  = triangleStrideBytes;

	desc.points          = vertices;
	desc.triangles       = triangles;
	desc.weldingDistance = weldingDistance;
	desc.flags           = flags;

	desc.vertexMassStrideBytes = vertexMassStrideBytes;
	desc.vertexFlagStrideBytes = vertexFlagStrideBytes;
	desc.vertexMasses          = vertexMasses;
	desc.vertexFlags           = vertexFlags;

	desc.numHierarchyLevels    = numHierarchyLevels;

}

/*----------------------------------------------------------------------------*/
void DeformableMeshDesc::loadFromSoftBodyMeshDesc(const NxSoftBodyMeshDesc &desc)
{
	deformableType	      = DEFORMABLE_TYPE_SOFTBODY;

	numVertices           = desc.numVertices;
	vertexStrideBytes     = desc.vertexStrideBytes;
	numTetrahedra         = desc.numTetrahedra;
	tetrahedronStrideBytes = desc.tetrahedronStrideBytes;

	vertices   = desc.vertices;
	tetrahedra = desc.tetrahedra;

	vertexMassStrideBytes = desc.vertexMassStrideBytes;
	vertexFlagStrideBytes = desc.vertexFlagStrideBytes;
	vertexMasses          = desc.vertexMasses;
	vertexFlags           = desc.vertexFlags;

	// convert to cloth convention
	flags = 0;
	if (desc.flags & NX_SOFTBODY_MESH_TEARABLE)
		flags |= NX_CLOTH_MESH_TEARABLE;
	if (desc.flags & NX_SOFTBODY_MESH_16_BIT_INDICES)
		flags |= NX_MF_16_BIT_INDICES;

	// mmf todo: add hierarchy support for soft bodies
	numHierarchyLevels    = 0;
}

/*----------------------------------------------------------------------------*/
void DeformableMeshDesc::saveToSoftBodyMeshDesc(NxSoftBodyMeshDesc &desc) const
{
	desc.setToDefault();

	desc.numVertices            = numVertices;
	desc.vertexStrideBytes      = vertexStrideBytes;
	desc.numTetrahedra          = numTetrahedra;
	desc.tetrahedronStrideBytes = tetrahedronStrideBytes;

	desc.vertices   = vertices;
	desc.tetrahedra = tetrahedra;

	desc.vertexMassStrideBytes = vertexMassStrideBytes;
	desc.vertexFlagStrideBytes = vertexFlagStrideBytes;
	desc.vertexMasses          = vertexMasses;
	desc.vertexFlags           = vertexFlags;

	// convert from cloth convention
	desc.flags = 0;
	if (flags & NX_CLOTH_MESH_TEARABLE)
		desc.flags |= NX_SOFTBODY_MESH_TEARABLE;
	if (flags & NX_MF_16_BIT_INDICES)
		desc.flags |= NX_SOFTBODY_MESH_16_BIT_INDICES;

	// mmf debug: add support for soft bodies
	//desc.numHierarchyLevels    = numHierarchyLevels;
}

/*----------------------------------------------------------------------------*/
NX_INLINE void DeformableMeshDesc::clear()
{
	// this just clears the descriptor
	// the setToDefault methods are done in the cloth / soft body mesh descriptors

	deformableType = DEFORMABLE_TYPE_NONE;

	numVertices = 0;
	vertexStrideBytes = 0;
	numTriangles = 0;	
	triangleStrideBytes = 0;
	numTetrahedra = 0;
	tetrahedronStrideBytes = 0;
	vertices = NULL;
	triangles = NULL;
	tetrahedra = NULL;
	weldingDistance = 0.0f;
	flags = 0;
	vertexMassStrideBytes = 0;
	vertexFlagStrideBytes = 0;
	vertexMasses = NULL;
	vertexFlags = NULL;
}

/*----------------------------------------------------------------------------*/
NX_INLINE bool DeformableMeshDesc::isValid() const
{
	if (deformableType == DEFORMABLE_TYPE_CLOTH) {
		NxClothMeshDesc clothMeshDesc;
		saveToClothMeshDesc(clothMeshDesc);
		return clothMeshDesc.isValid();
	}
	else if (deformableType == DEFORMABLE_TYPE_SOFTBODY) {
		NxSoftBodyMeshDesc softBodyMeshDesc;
		saveToSoftBodyMeshDesc(softBodyMeshDesc);
		return softBodyMeshDesc.isValid();
	}
	else return false;
}

#endif
