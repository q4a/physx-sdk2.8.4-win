#ifndef NX_COLLISION_NP_TRIANGLEMESHSHAPE
#define NX_COLLISION_NP_TRIANGLEMESHSHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxTriangleMeshShape.h"
#include "MacroNpShape.h"

class NpTriangleMeshShape : public NxTriangleMeshShape, public NpShape, public NxFoundation::NxAllocateable
	{
	
	NPSHAPE_DECLARATION(TriangleMeshShape)

	public:

	virtual	void				saveToDesc(NxTriangleMeshShapeDesc&)	const;
	virtual	NxTriangleMesh&		getTriangleMesh();
	virtual	const NxTriangleMesh& getTriangleMesh()const;
	virtual	NxU32				getTriangle(NxTriangle& triangle, NxTriangle* edgeTri, NxU32* flags, NxTriangleID triangleIndex, bool worldSpaceTranslation, bool worldSpaceRotation)	const;
	virtual	bool				overlapAABBTriangles(const NxBounds3& bounds, NxU32 flags, NxUserEntityReport<NxU32>* callback)		const;
	virtual	bool				overlapAABBTrianglesDeprecated(const NxBounds3& bounds, NxU32 flags, NxU32& nb, const NxU32*& indices)			const;
	virtual	bool				addTensorContribution(InertiaTensorComputer& itc) const;
#ifdef NX_SUPPORT_MESH_SCALE
	virtual	NxF32				getScale()	const;
	virtual void				setScale(NxF32 scale);
#endif

	virtual bool				mapPageInstance(NxU32 pageIndex);
	virtual void				unmapPageInstance(NxU32 pageIndex);
	virtual bool				isPageInstanceMapped(NxU32 pageIndex) const;

	//internal:
	bool						loadFromDesc(const NxTriangleMeshShapeDesc&);
	virtual	void				sendToVRD(bool create, bool fromCtor, bool callBase);
	};
//	}
#endif
