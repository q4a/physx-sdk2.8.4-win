#ifndef NX_COLLISION_CONVEXMESHBUILDER
#define NX_COLLISION_CONVEXMESHBUILDER
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "ConvexMesh.h"

class ConvexMeshBuilder : public ConvexMesh
	{
	public:
											ConvexMeshBuilder();
											~ConvexMeshBuilder();

				bool						loadFromDesc(const NxConvexMeshDesc&);
				bool						save(NxStream& stream, NxU32 useUnquantizedNormals=0)		const;

				bool						computeConvexHull(const NxConvexMeshDesc&);
				bool						loadConvexHull(const NxConvexMeshDesc&);
#ifndef USE_HULL_TRIANGLES
				void						importMesh(const NxConvexMeshDesc& desc, bool cleanMesh=true);
#endif
				bool						computeGaussMaps();
				void						computeLocalBounds();
				const MeshMassInfo*			computeMassInfo()			const;
				
				bool                        scale(NxReal scale);
	private:
// PXD_SUPPORT_CONVEX_OPCODE_MODEL
	#ifdef USE_HULL_TRIANGLES
				bool						createOpcodeModel();
	#endif
//~PXD_SUPPORT_CONVEX_OPCODE_MODEL
	};

#endif
