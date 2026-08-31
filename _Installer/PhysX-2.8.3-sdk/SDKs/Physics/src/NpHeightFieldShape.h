#ifndef NX_COLLISION_NP_HEIGHTFIELDSHAPE
#define NX_COLLISION_NP_HEIGHTFIELDSHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxHeightFieldShape.h"
#include "MacroNpShape.h"

class NpHeightFieldShape : public NxHeightFieldShape, public NpShape, public NxFoundation::NxAllocateable
	{
	
	NPSHAPE_DECLARATION(HeightFieldShape)

	public:

	virtual	void				saveToDesc(NxHeightFieldShapeDesc&)	const;
	virtual	NxU32				getTriangle(NxTriangle& worldTri, NxTriangle* edgeTri, NxU32* flags, NxTriangleID triangleIndex, bool worldSpaceTranslation=true, bool worldSpaceRotation=true) const;
	virtual	bool				overlapAABBTriangles(const NxBounds3& bounds, NxU32 flags, NxUserEntityReport<NxU32>* callback) const;
	virtual	bool				overlapAABBTrianglesDeprecated(const NxBounds3& bounds, NxU32 flags, NxU32& nb, const NxU32*& indices) const;
	virtual	bool				addTensorContribution(InertiaTensorComputer& itc) const;

	virtual	NxHeightField&		getHeightField() const;

	virtual NxReal				getHeightScale() const;
	virtual NxReal				getRowScale() const;
	virtual NxReal				getColumnScale() const;
	virtual NxMaterialIndex		getMaterialIndexHighBits() const;
	virtual NxMaterialIndex		getHoleMaterial() const;
	virtual NxU32				getMeshFlags() const;

	virtual void				setHeightScale(NxReal scale);
	virtual void				setRowScale(NxReal scale);
	virtual void				setColumnScale(NxReal scale);
	virtual void				setMaterialIndexHighBits(NxMaterialIndex index);
	virtual void				setHoleMaterial(NxMaterialIndex);
	virtual void				setMeshFlags(NxU32 flags);

	virtual bool				isShapePointOnHeightField(NxReal x, NxReal z) const;
	virtual NxReal				getHeightAtShapePoint(NxReal x, NxReal z) const;
	virtual NxMaterialIndex		getMaterialAtShapePoint(NxReal x, NxReal z) const;
	virtual NxVec3				getNormalAtShapePoint(NxReal x, NxReal z) const;
	virtual NxVec3				getSmoothNormalAtShapePoint(NxReal x, NxReal z) const;

	//internal:
	bool						loadFromDesc(const NxHeightFieldShapeDesc&);
	virtual void				sendToVRD(bool create, bool fromCtor, bool callBase);
	};
//	}
#endif
