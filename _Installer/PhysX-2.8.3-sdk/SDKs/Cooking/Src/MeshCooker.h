#ifndef NX_COOKING_MESHCOOKER
#define NX_COOKING_MESHCOOKER
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nxp.h"
class NxStream;
class TriangleMesh;

	bool NxCookHWMesh(
					NxU32 nbTris,				// number of triangles in source mesh
					NxU32 nbVerts,				// number of vertices in source mesh
					const NxTriangle32* tris,	// source triangles
					const NxPoint* verts,		// source vertices
					NxU32 nbConvexParts,		// convex decomp
					NxU32 nbFlatParts,			// convex decomp
					const NxU16* convexParts,	// convex decomp
					const NxU8* flatParts8,		// convex decomp
					const NxU16* flatParts16,	// convex decomp

					NxU32 maxNbTris,			// HW limit: max number of triangles
					NxU32 maxNbVerts,			// HW limit: max number of vertices
					NxU32 pageSize,				// HW limit: size of a mesh page in bytes

					NxStream& output			// Cooked pages go there
					);

#endif
