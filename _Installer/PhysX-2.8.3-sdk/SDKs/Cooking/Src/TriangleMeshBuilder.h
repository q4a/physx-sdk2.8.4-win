#ifndef NX_COLLISION_TRIANGLEMESHBUILDER
#define NX_COLLISION_TRIANGLEMESHBUILDER
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "TriangleMesh.h"

	class TriangleMeshBuilder : public TriangleMesh
	{
	public:
				bool				loadFromDesc(const NxTriangleMeshDesc&);
				bool				save(NxStream& stream)		const;
	protected:
				const MeshMassInfo*	computeMassInfo()			const;
				void				computeLocalBounds();
				bool				importMesh(const NxTriangleMeshDesc& desc);

	static		void				SaveObj(const NxTriangleMeshDesc &source,const char *fname); // debug tool
	
				NxU32				getPageCount() const							{ NX_ASSERT(0); return 0; }
				NxBounds3			getPageBBox(NxU32 pageIndex) const				{ NX_ASSERT(0); return *(NxBounds3 *)0; }
				void *				getPage(NxU32 pageIndex, NxU32 &length) const	{ NX_ASSERT(0); return 0; }
	};

#endif
