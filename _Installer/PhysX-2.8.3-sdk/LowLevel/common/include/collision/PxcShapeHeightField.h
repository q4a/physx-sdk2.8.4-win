#ifndef PXC_SHAPEHEIGHTFIELD_H
#define PXC_SHAPEHEIGHTFIELD_H

//TODO: watch out for flags & getMeshFlags()!!!

#include "px_config.h"
#include "px_geometry.h"

#include "PxnError.h"
#include "PxcVector.h"

#include "PxcHelperFunction.h"

// TODO: 
// Move out these out of here to somewhere visible to the user
#define PX_HF_0TH_VERTEX_SHARED				1
#define PX_HF_NO_BOUNDARY_EDGES				1
#define PX_MESH_SMOOTH_SPHERE_COLLISIONS	2
#define PX_HF_S16_TM						1
typedef PxU16 PxMaterialIndex;
typedef PxU32 PxHeightFieldFormat;

struct PxcHeightFieldSample
{
	PxI16	height			: 16;
	PxU8	materialIndex0	: 7;
	PxU8	tessFlag		: 1;
	PxU8	materialIndex1	: 7;
	PxU8	unused			: 1;
};

// Coordinate Spaces:
// =================
//
//  World space:
//   Needs no introduction.
//
//  Shape space:
//   A height field vertex in this space has coordinates:
//   (row * getRowScale(), sample(row,column) * getHeightScale(), column * getColumnScale())
// 
//  Height field space:
//   A height field vertex in this space has coordinates:
//   (row, sample(row,column), column)
//
//
// Indices:
// =======
//  cell                   = row * nbColumns + column
//  vertex                 = cell
//  first triangle         = 2 * cell
//  second triangle        = 2 * cell + 1
//  column edge (along z)  = 3 * cell
//  diagonal edge          = 3 * cell + 1
//  row edge (along x)     = 3 * cell + 2
//
//  Feature index is sometimes used and corresponds to triangle index if 
//  the triangle is solid. 
//  A solid triangle is one that does not have the hole material.
//
//
// Clipping to voronoi volumes:
// ============================
// 
//  AB is clipped correctly since it is on the side of the positive volume
//  CD is not clipped correctly since it is on the side of the negative volume
//
//            B  |     
//               | +  /
//               |  /
//   ____________A/
//              /|\
//            /  |  \   D  |     
//          /  - |    \    | -  /
//               |      \  |  /
//                        \C/____________ height field surface
//                        /|
//                      /  |
//                    /  + |
//                         |
//
class PxcHeightField 
{
public:

									PxcHeightField(const PxdShapeDescHeightField& desc);
	virtual							~PxcHeightField();

	// HeightField
	PXD_INLINE	PxU32				getNbRows()				const { return mNbRows; }
	PXD_INLINE	PxU32				getNbColumns()			const { return mNbColumns; }
	PXD_INLINE	PxU32				getSampleStride()		const { return mSampleStride; }
	PXD_INLINE	void*				getSamples()			const { return mSamples; }
	PXD_INLINE	PxFloat				getVerticalExtent()		const { return mVerticalExtent; }
	PXD_INLINE	PxFloat				getThickness()			const { return mThickness; }
	PXD_INLINE	PxFloat				getConvexEdgeThreshold()const { return mConvexEdgeThreshold; }
	PXD_INLINE	PxHeightFieldFormat	getFormat()				const { return mFormat; }
	PXD_INLINE	PxMaterialIndex		getTriangleMaterial(PxU32 triangleIndex) const;
	PXD_INLINE	PxFloat				getHeight(PxU32 vertexIndex) const;
	PXD_INLINE	PxU32				getMinRow(PxFloat x)	const;
	PXD_INLINE	PxU32				getMaxRow(PxFloat x)	const;
	PXD_INLINE	PxU32				getMinColumn(PxFloat z) const;
	PXD_INLINE	PxU32				getMaxColumn(PxFloat z) const;

				void				setNbRows(PxU32 rows);
				void 				setNbColumns(PxU32 columns);
				void				setSampleStride(PxU32 sampleStride);
				void				setSamples(void* samples);
				void				setVerticalExtent(PxFloat verticalExtent);
				void				setThickness(PxFloat thickness);
				void				setConvexEdgeThreshold(PxFloat convexEdgeThreshold);
	PXD_INLINE	PxBool				isDeltaHeightInsideExtent(PxFloat dy, PxFloat eps=0) const;
	PXD_INLINE	PxBool				isDeltaHeightOppositeExtent(PxFloat dy) const;
	// ~HeightField

	// properties
	PXD_INLINE	PxFloat				getHeightScale()		const	{ return mHeightScale;			}
	PXD_INLINE	PxFloat				getRowScale()			const	{ return mRowScale;				}
	PXD_INLINE	PxFloat				getColumnScale()		const	{ return mColumnScale;			}
	PXD_INLINE	PxMaterialIndex		getMaterialIndexHighBits()const	{ return mMaterialIndexHighBits;}
	PXD_INLINE	PxMaterialIndex		getHoleMaterial()		const	{ return mHoleMaterial;			}
	PXD_INLINE	PxU32				getFlags()				const	{ return mFlags; }

				void				setHeightScale(PxFloat s);
				void				setRowScale(PxFloat s);
				void				setColumnScale(PxFloat s);
				void				setMaterialIndexHighBits(PxMaterialIndex m);
				void				setHoleMaterial(PxMaterialIndex m);
				void				setFlags(PxU32 flags);

	PXD_INLINE	PxFloat				getOneOverRowScale()	const	{ return mOneOverRowScale;		}
	PXD_INLINE	PxFloat				getOneOverColumnScale()	const	{ return mOneOverColumnScale;	}
	PXD_INLINE	PxFloat				getOneOverHeightScale()	const	{ return mOneOverHeightScale;	}

				void				getShapeBounds(PxcVector& min, PxcVector& max);

	PXD_INLINE	bool				isShapePointOnHeightField(PxFloat x, PxFloat z) const;

	PXD_INLINE	PxFloat				getHeightAtShapePoint(PxFloat x, PxFloat z) const;

	// Returns the normal direction (not normalized) at the point in shape space.
	PXD_INLINE	PxcVector			getNormalAtShapePoint(PxFloat x, PxFloat z) const;
	// Returns the smoothed normal direction (not normalized) at the point in shape space.
				PxcVector			getSmoothNormalAtShapePoint(PxFloat x, PxFloat z) const;
	// Returns the vertex normal direction (normalized) at the point in shape space.
				PxcVector			getVertexNormal(PxU32 vertexIndex) const;

	// Clips a normal direction to the voronoi volume of the edge.
	// Returns true if direction was clipped.
	// Results are undefined when clipping to the *negative* volume.
				bool				clipShapeNormalToEdgeVoronoi(PxcVector& normal, PxU32 edgeIndex) const;

	// Clips a normal direction to the voronoi volume of the vertex.
	// Returns true if direction was clipped.
	// Results are undefined when clipping to the *negative* volume.
				bool				clipShapeNormalToVertexVoronoi(PxcVector& normal, PxU32 vertexIndex) const;

	// Transform a position vector from height field space to shape space.
	PXD_INLINE	PxcVector			hf2shapep(const PxcVector& v) const;
	// Transform a direction vector from height field space to shape space.
	PXD_INLINE	PxcVector			hf2shapen(const PxcVector& v) const;
	// Transform a position vector from shape space to height field space.
	PXD_INLINE	PxcVector			shape2hfp(const PxcVector& v) const;
	// Transform a direction vector from shape space to height field space.
	PXD_INLINE	PxcVector			shape2hfn(const PxcVector& v) const;

	PXD_INLINE	bool				isZerothVertexShared(PxU32 vertexIndex) const;
	PXD_INLINE	PxMaterialIndex		getMaterialIndex0(PxU32 vertexIndex) const;
	PXD_INLINE	PxMaterialIndex		getMaterialIndex1(PxU32 vertexIndex) const;

	// A boundary edge has exactly one solid face.
				bool				isBoundaryEdge(PxU32 edgeIndex) const;

	// A collision edge is a boundary edge or a convex edge.
				bool				isCollisionEdge(PxU32 edgeIndex) const;

	// A collision vertex has at least one collision edge.
	// This definition is really too loose, but the alternative was too expensive.
				bool				isCollisionVertex(PxU32 vertexIndex) const;

	// Returns the index of the solid triangle at the given point in shape space or 0xffffffff.
				PxU32				getFeatureIndexAtShapePoint(PxFloat x, PxFloat z) const;

	// Returns the index of the first solid triangle that this edge is part of or 0xffffffff.
				PxU32				getEdgeFeatureIndex(PxU32 edgeIndex) const;

	// Returns the index of the first solid triangle that this vertex is part of or 0xffffffff.
				PxU32				getVertexFeatureIndex(PxU32 vertexIndex) const;

	// Returns the origin and extent vectors in shape space for the given edge.
				void				getEdge(PxU32 edgeIndex, PxcVector& origin, PxcVector& extent) const;

	// Returns the direction vector in shape space for the given edge.
				PxcVector			getEdgeDirection(PxU32 edgeIndex) const;

	// Finds closest points in shape space on the given cell using a special convention.
	// Points internal to the two faces will always be reported provided they are solid.
	// If testEdges flag is not set only points internal to the faces will be reported.
	// If testEdges flag is set closest points will be reported for the following features:
	//  top left vertex     - if a collision vertex
	//  top edge            - if a collision edge
	//  left edge           - if a collision edge
	//  diagonal            - if a collision edge
	//  top right vertex    - if a collision vertex and cell is in the last column
	//  right edge          - if a collision edge cell is in the last column
	//  left bottom vertex  - if a collision vertex and cell is in the last row
	//  bottom edge         - if a collision edge and cell is in the last row
	//  bottom right vertex - if a collision vertex and cell is in the last row & column
	// The closest points are stored in the closestPoint parameter.
	// The return value indicates the number of points stored (maximum is eleven).
	// If the features parameter is not NULL the respective feature index for each point is stored here.
				PxU32				findClosestPointsOnCell(PxU32 row, PxU32 column, PxcVector point, PxcVector* closestPoints, PxU32* features, bool testEdges = true) const;

	// Finds closest point in shape space on the given edge.
	// The result is stored in the closestPoint parameter and will be on the edge.
	// The return value is the projection of point on the edge and can be used to
	// check if the closest point is internal to the edge or on the vertices.
				PxFloat				findClosestPointOnEdge(PxU32 edgeIndex, const PxcVector& point, PxcVector& closestPoint) const;

	// Finds the projection of point in shape space on the triangle.
	// If the return value is true the projection of point was inside the triangle and closestPoint is valid.
	// If the return value is false the projection of point was outside the triangle and closestPoint undefined.
				bool				findProjectionOnTriangle(PxU32 triangleIndex, const PxcVector& point, PxcVector& projection) const;

	// Traces the intersections of the line p0-p1 with the row, column and diagonal edges of the height field 
	// in the X-Z plane in shape space.
	// T is a class / struct with a method:
	//    bool contact(
	//       const PxcHeightField& hfShape, 
	//       PxU32 edgeIndex, 
	//       PxU32 row,
	//       PxU32 column, 
	//       PxFloat x, 
	//       PxFloat y, 
	//       PxFloat z, 
	//       PxFloat h)
	// This method is called at every intersection point.
	// edgeIndex is the intersected edge.
	// row and column indicate the cell.
	// x and z are coordinates within the cell in normalized space (height field space).
	// y is the y value of the line p0-p1 at the intersection point.
	// h is the height field height at the intersection point (scaled by getHeightScale()).
	// So, the point on the line at the intersection is: 
	//    getRowScale() * (row + x), y, getColumnScale() * (column + y)
	// and the point on the height field at the intersection is: 
	//    getRowScale() * (row + x), h, getColumnScale() * (column + y)
	// The reason for this non-uniform mapping is that many contact handlers will have an
	// early out depending on the difference between h and y and may not care to evaluate 
	// the points.
	template<class T>
	PXD_INLINE	void				traceLine(const PxcVector& p0, const PxcVector& p1, T* callback) const;

private:
				PxFloat				mHeightScale;
				PxFloat				mRowScale;
				PxFloat				mColumnScale;
				PxMaterialIndex		mMaterialIndexHighBits;
				PxMaterialIndex		mHoleMaterial;
				
				PxFloat				mOneOverRowScale;
				PxFloat				mOneOverColumnScale;
				PxFloat				mOneOverHeightScale;

				// HeightField
				PxU32				mNbRows;
				PxU32				mNbColumns;
				PxHeightFieldFormat	mFormat;
				PxU32				mSampleStride;
				void *				mSamples;						
				PxFloat				mVerticalExtent;
				PxFloat				mThickness;
				PxFloat				mConvexEdgeThreshold;
				PxU32				mFlags;
				// ~HeightField

				bool				mDataBoundsValid;
				PxFloat				mMinHeight;
				PxFloat				mMaxHeight;
				
				bool				mShapeBoundsValid;
				PxcVector			mShapeBoundsMin;
				PxcVector			mShapeBoundsMax;

	// Internal methods called by traceLine
	template<class T>
	PXD_INLINE	bool				uintercept(PxI32 row, PxI32 column, PxFloat z, PxFloat y, T* callback) const;
	template<class T>
	PXD_INLINE	bool				vintercept(PxI32 row, PxI32 column, PxFloat x, PxFloat y, T* callback) const;
	template<class T>
	PXD_INLINE	bool				dintercept0(PxI32 row, PxI32 column, PxFloat x, PxFloat z, PxFloat y, T* callback) const;
	template<class T>
	PXD_INLINE	bool				dintercept1(PxI32 row, PxI32 column, PxFloat x, PxFloat z, PxFloat y, T* callback) const;

	// HeightField
	PXD_INLINE	bool				isValidVertex(PxU32 vertexIndex) const;
	PXD_INLINE	PxcVector			getVertex(PxU32 vertexIndex) const;
	PXD_INLINE	void				getVertexEdgeIndices(PxU32 vertexIndex, PxU32 edgeIndices[8], PxU32& count) const;
	PXD_INLINE	bool				isConvexVertex(PxU32 vertexIndex) const;

	PXD_INLINE	bool				isValidEdge(PxU32 edgeIndex) const;
	PXD_INLINE	void				getEdgeTriangleIndices(PxU32 edgeIndex, PxU32 triangleIndices[2], PxU32& count) const;
	PXD_INLINE	void				getEdgeVertexIndices(PxU32 edgeIndex, PxU32& vertexIndex0, PxU32& vertexIndex1) const;
	PXD_INLINE	bool				isConvexEdge(PxU32 edgeIndex) const;

	PXD_INLINE	void				computeCellCoordinates(PxFloat x, PxFloat z, PxU32& row, PxU32& column, PxFloat& fracX, PxFloat& fracZ) const;

	PXD_INLINE	bool				isValidTriangle(PxU32 triangleIndex) const;
	PXD_INLINE	bool				isFirstTriangle(PxU32 triangleIndex) const;
	PXD_INLINE	void				getTriangleVertexIndices(PxU32 triangleIndex, PxU32& vertexIndex0, PxU32& vertexIndex1, PxU32& vertexIndex2) const;
	PXD_INLINE	void				getTriangleEdgeIndices(PxU32 triangleIndex, PxU32& edgeIndex0, PxU32& edgeIndex1, PxU32& edgeIndex2) const;
	PXD_INLINE	PxcVector			getTriangleNormal(PxU32 triangleIndex) const;

	PXD_INLINE	PxFloat				getHeight(PxFloat x, PxFloat z) const;
	PXD_INLINE	PxcVector			getNormal(PxFloat x, PxFloat z) const;

	PXD_INLINE	PxU32				getTriangleIndex(PxFloat x, PxFloat z) const;
	PXD_INLINE	PxMaterialIndex		getMaterial(PxFloat x, PxFloat z) const;

	PXD_INLINE	PxFloat				getMinHeight()					const	{ return mMinHeight; }
	PXD_INLINE	PxFloat				getMaxHeight()					const	{ return mMaxHeight; }

	PXD_INLINE	void *				byteOffset(PxU32 offset) { return (void*)(((PxU8*)getSamples()) + offset); }
	PXD_INLINE	const void*			byteOffset(PxU32 offset) const { return (const void*)(((PxU8*)getSamples()) + offset); }

	PXD_INLINE	const PxcHeightFieldSample&	getSample(PxU32 vertexIndex) const;
	// ~HeightField

	PXD_INLINE	void				invalidateDataBounds() { mDataBoundsValid = false; }
	PXD_INLINE	bool				areDataBoundsValid() const { return mDataBoundsValid; }
				void				computeDataBounds();
				
	PXD_INLINE	void				invalidateShapeBounds() { mShapeBoundsValid = false; }
	PXD_INLINE	bool				areShapeBoundsValid() const { return mShapeBoundsValid && areDataBoundsValid(); }
				void				computeShapeBounds();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxBool PxcHeightField::isDeltaHeightInsideExtent(PxFloat dy, PxFloat eps) const
{ 
	return (mThickness != 0)
		? (mThickness <= 0 && dy <= eps && dy >= mThickness) || (mThickness > 0 && dy > -eps && dy < mThickness) 
		: (mVerticalExtent <= 0 && dy <= eps) || (mVerticalExtent > 0 && dy > -eps);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxBool PxcHeightField::isDeltaHeightOppositeExtent(PxFloat dy) const
{ 
	return (mThickness != 0)
		? (mThickness <= 0 && dy > 0) || (mThickness > 0 && dy < 0) 
		: (mVerticalExtent <= 0 && dy > 0) || (mVerticalExtent > 0 && dy < 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE bool PxcHeightField::isShapePointOnHeightField(PxFloat x, PxFloat z) const
{
	x *= getOneOverRowScale();
	z *= getOneOverColumnScale();
	return ((!(x < 0))
		&&  (!(z < 0))
		&&  (x < (getNbRows()-1)) 
		&&  (z < (getNbColumns()-1)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxFloat PxcHeightField::getHeightAtShapePoint(PxFloat x, PxFloat z) const
{
	return getHeightScale() * getHeight(x * getOneOverRowScale(), z * getOneOverColumnScale());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxcVector PxcHeightField::getNormalAtShapePoint(PxFloat x, PxFloat z) const
{
	return hf2shapen(getNormal(x * getOneOverRowScale(), z * getOneOverColumnScale()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxcVector PxcHeightField::hf2shapep(const PxcVector& v) const
{
	//return PxcVector(v.x * getRowScale(), v.y * getHeightScale(), v.z * getColumnScale());
	PxcVector temp;
	temp.set(v.x * getRowScale(), v.y * getHeightScale(), v.z * getColumnScale());
	return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxcVector PxcHeightField::hf2shapen(const PxcVector& v) const
{
	//return PxcVector(v.x * getOneOverRowScale(), v.y * getOneOverHeightScale(), v.z * getOneOverColumnScale());
	PxcVector temp;
	temp.set(v.x * getOneOverRowScale(), v.y * getOneOverHeightScale(), v.z * getOneOverColumnScale());
	return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxcVector PxcHeightField::shape2hfp(const PxcVector& v) const
{
	//return PxcVector(v.x * getOneOverRowScale(), v.y * getOneOverHeightScale(), v.z * getOneOverColumnScale());
	PxcVector temp;
	temp.set(v.x * getOneOverRowScale(), v.y * getOneOverHeightScale(), v.z * getOneOverColumnScale());
	return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxcVector PxcHeightField::shape2hfn(const PxcVector& v) const
{
	//return PxcVector(v.x * getRowScale(), v.y * getHeightScale(), v.z * getColumnScale());
	PxcVector temp;
	temp.set(v.x * getRowScale(), v.y * getHeightScale(), v.z * getColumnScale());
	return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
PXD_INLINE bool PxcHeightField::uintercept(PxI32 row, PxI32 column, PxFloat z, PxFloat y, T* callback) const
{
	if (row < 0) return true;
	if (column < 0) return true;
	if (row	> PxI32(getNbRows()) - 1) return true;
	if (column > PxI32(getNbColumns()) - 2) return true;
	PxU32 vertexIndex = getNbColumns() * row + column;
	PxFloat h0 = getHeight(vertexIndex);
	PxFloat h1 = getHeight(vertexIndex + 1);
	PxFloat h = getHeightScale() * (h0 + z * (h1 - h0));
	return callback->contact(*this, 3 * vertexIndex, row, column, 0, y, z, h);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
PXD_INLINE bool PxcHeightField::vintercept(PxI32 row, PxI32 column, PxFloat x, PxFloat y, T* callback) const
{
	if (row < 0) return true;
	if (column < 0) return true;
	if (row > PxI32(getNbRows()) - 2) return true;
	if (column > PxI32(getNbColumns()) - 1) return true;
	PxU32 vertexIndex = getNbColumns() * row + column;
	PxFloat h0 = getHeight(vertexIndex);
	PxFloat h1 = getHeight(vertexIndex + getNbColumns());
	PxFloat h = getHeightScale() * (h0 + x * (h1 - h0));
	return callback->contact(*this, 3 * vertexIndex + 2, row, column, x, y, 0, h);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
PXD_INLINE bool PxcHeightField::dintercept0(PxI32 row, PxI32 column, PxFloat x, PxFloat z, PxFloat y, T* callback) const
{
	if (row < 0) return true;
	if (column < 0) return true;
	if (row > PxI32(getNbRows()) - 2) return true;
	if (column > PxI32(getNbColumns()) - 2) return true;
	PxU32 vertexIndex = getNbColumns() * row + column;
	if (isZerothVertexShared(vertexIndex)) return true;
	PxFloat h0 = getHeight(vertexIndex + 1);
	PxFloat h1 = getHeight(vertexIndex + getNbColumns());
	PxFloat h = getHeightScale() * (h0 + x * (h1 - h0));
	return callback->contact(*this, 3 * vertexIndex + 1, row, column, x, y, z, h);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
PXD_INLINE bool PxcHeightField::dintercept1(PxI32 row, PxI32 column, PxFloat x, PxFloat z, PxFloat y, T* callback) const
{
	if (row < 0) return true;
	if (column < 0) return true;
	if (row > PxI32(getNbRows()) - 2) return true;
	if (column > PxI32(getNbColumns()) - 2) return true;
	PxU32 vertexIndex = getNbColumns() * row + column;
	if (!isZerothVertexShared(vertexIndex)) return true;
	PxFloat h0 = getHeight(vertexIndex);
	PxFloat h1 = getHeight(vertexIndex + getNbColumns() + 1);
	PxFloat h = getHeightScale() * (h0 + x * (h1 - h0));
	return callback->contact(*this, 3 * vertexIndex + 1, row, column, x, y, z, h);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
PXD_INLINE void PxcHeightField::traceLine(const PxcVector& p0, const PxcVector& p1, T* callback) const
{
	PxFloat u0 = p0.x * getOneOverRowScale();
	PxFloat v0 = p0.z * getOneOverColumnScale();
	PxFloat h0 = p0.y; // we don't scale y

	PxFloat u1 = p1.x * getOneOverRowScale();
	PxFloat v1 = p1.z * getOneOverColumnScale();
	PxFloat h1 = p1.y; // we don't scale y

	const PxFloat root2 = 1.4142135623730950488016887242097f;
	const PxFloat halfRoot2 = 0.5f * root2;
	const PxFloat oneOverRoot2 = 1.0f / root2;

	PxFloat du = u1 - u0;
	PxFloat dv = v1 - v0;
	PxFloat dh = h1 - h0;

	PxI32 i = (PxI32)PxcFloor(u0);
	PxI32 j = (PxI32)PxcFloor(v0);

	PxFloat adu = PxcAbs(du);
	PxFloat adv = PxcAbs(dv);

	PxFloat oodu = (adu > PXD_FLOAT_EPSILON) ? 1 / adu : PXD_FLOAT_MAX;
	PxFloat oodv = (adv > PXD_FLOAT_EPSILON) ? 1 / adv : PXD_FLOAT_MAX;

	PxFloat eu = u0 - PxFloat(i);
	PxFloat ev = v0 - PxFloat(j);

	PxFloat dp = (du + dv) * oneOverRoot2; // a dot product with vector (1,1)
	PxFloat adp = PxcAbs(dp);
	PxFloat oodp = (adp > PXD_FLOAT_EPSILON) ? 1 / adp : PXD_FLOAT_MAX;
	// eu + ep/root2 + ev + ep/root2 = 1
	// ep = root2 * (1 - eu - ev) / 2
	PxFloat ep = (1 - eu - ev) * oneOverRoot2;
	if (ep < 0) ep += halfRoot2;

	PxFloat dq = (dv - du) * oneOverRoot2; // a dot product with vector (-1,1)
	PxFloat adq = PxcAbs(dq);
	PxFloat oodq = (adq > PXD_FLOAT_EPSILON) ? 1 / adq : PXD_FLOAT_MAX;
	// eu - eq/root2 = ev + eq/root2
	// eq = root2 * (eu - ev) / 2
	PxFloat eq = (eu - ev) * oneOverRoot2;
	if (eq < 0) eq += halfRoot2;

	if (du > 0) eu = 1 - eu;
	if (dv > 0) ev = 1 - ev;

	if (dp < 0) ep = halfRoot2 - ep;
	if (dq < 0) eq = halfRoot2 - eq;

	PxFloat t = 0;
	PxFloat h = h0;

	while (t - 1 < PXD_FLOAT_EPSILON) 
	{
		PxFloat dtOverDu = (oodu < PXD_FLOAT_MAX) ? eu*oodu : PXD_FLOAT_MAX;
		PxFloat dtOverDv = (oodv < PXD_FLOAT_MAX) ? ev*oodv : PXD_FLOAT_MAX;

		PxFloat dtOverDp = (oodp < PXD_FLOAT_MAX) ? ep*oodp : PXD_FLOAT_MAX;
		PxFloat dtOverDq = (oodq < PXD_FLOAT_MAX) ? eq*oodq : PXD_FLOAT_MAX;

		if (dtOverDu <= dtOverDv && dtOverDu <= dtOverDp && dtOverDu <= dtOverDq) 
		{
			t += dtOverDu;
			if (t > 1) return;
			h += dh*dtOverDu;
			ev -= adv*dtOverDu;
			ep -= adp*dtOverDu;
			eq -= adq*dtOverDu;
			eu = 1;
			PxFloat v = (dv > 0) ? 1 - ev : ev;
			if (du > 0) 
			{
				if (!uintercept(++i, j, v, h, callback)) return;
			}
			else
			{
				if (!uintercept(i--, j, v, h, callback)) return;
			}
		} 
		else if (dtOverDv <= dtOverDu && dtOverDv <= dtOverDp && dtOverDv <= dtOverDq) 
		{
			t += dtOverDv;
			if (t > 1) return;
			h += dh*dtOverDv;
			eu -= adu*dtOverDv;
			ep -= adp*dtOverDv;
			eq -= adq*dtOverDv;
			ev = 1;			
			PxFloat u = (du > 0) ? 1 - eu : eu;
			if (dv > 0) 
			{
				if (!vintercept(i, ++j, u, h, callback)) return;
			}
			else
			{
				if (!vintercept(i, j--, u, h, callback)) return;
			}
		} 
		else if (dtOverDp <= dtOverDq) 
		{
			t += dtOverDp;
			if (t > 1) return;
			h += dh*dtOverDp;
			eu -= adu*dtOverDp;
			ev -= adv*dtOverDp;
			eq -= adq*dtOverDp;
			ep = halfRoot2;
			PxFloat u = (du > 0) ? 1 - eu : eu;
			PxFloat v = (dv > 0) ? 1 - ev : ev;
			if (PxcAbs(u + v - 1) < 0.5f)
				if (!dintercept0(i, j, u, v, h, callback)) return;
		} 
		else
		{
			t += dtOverDq;
			if (t > 1) return;
			h += dh*dtOverDq;
			eu -= adu*dtOverDq;
			ev -= adv*dtOverDq;
			ep -= adp*dtOverDq;
			eq = halfRoot2;
			PxFloat u = (du > 0) ? 1 - eu : eu;
			PxFloat v = (dv > 0) ? 1 - ev : ev;
			if (PxcAbs(u - v) < 0.5f)
				if (!dintercept1(i, j, u, v, h, callback)) return;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE const PxcHeightFieldSample& PxcHeightField::getSample(PxU32 vertexIndex) const
{
	PXN_ASSERT(isValidVertex(vertexIndex));
	return *(const PxcHeightFieldSample*)byteOffset(vertexIndex*getSampleStride());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE bool PxcHeightField::isValidVertex(PxU32 vertexIndex) const
{
	return vertexIndex < getNbRows()*getNbColumns();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE bool PxcHeightField::isZerothVertexShared(PxU32 vertexIndex) const
{
	return (getSample(vertexIndex).tessFlag & PX_HF_0TH_VERTEX_SHARED);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxFloat PxcHeightField::getHeight(PxU32 vertexIndex) const
{
	return PxFloat(getSample(vertexIndex).height);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxMaterialIndex PxcHeightField::getMaterialIndex0(PxU32 vertexIndex) const
{
	return getSample(vertexIndex).materialIndex0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxMaterialIndex PxcHeightField::getMaterialIndex1(PxU32 vertexIndex) const
{
	return getSample(vertexIndex).materialIndex1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxcVector PxcHeightField::getVertex(PxU32 vertexIndex) const
{
	PxU32 row    = vertexIndex / getNbColumns();
	PxU32 column = vertexIndex % getNbColumns();
	//return PxcVector(PxFloat(row), getHeight(row * getNbColumns() + column), PxFloat(column));
	PxcVector temp;
	temp.set(PxFloat(row), getHeight(row * getNbColumns() + column), PxFloat(column));
	return temp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE void PxcHeightField::getVertexEdgeIndices(PxU32 vertexIndex, PxU32 edgeIndices[8], PxU32& count) const
{
	PxU32 row    = vertexIndex / getNbColumns();
	PxU32 column = vertexIndex % getNbColumns();
	count = 0;

	if (row > 0) 
	{
		/*
		//   COL -->
		//       
		// R     |
		// O     |
		// W     +  
		// |      
		// V  
		*/
		edgeIndices[count++] = 3 * (vertexIndex - getNbColumns()) + 2;
	}

	if (column < getNbColumns()-1)
	{
		/*
		//   COL -->
		//       
		// R       /
		// O      / 
		// W     +--
		// |      \
		// V       \ 
		*/
		if (row > 0)
		{
			/*
			//   COL -->
			//       
			// R       /
			// O      / 
			// W     +
			// |       
			// V
			*/
			if (!isZerothVertexShared(vertexIndex - getNbColumns()))
			{
				edgeIndices[count++] = 3 * (vertexIndex - getNbColumns()) + 1;
			}
		}
		/*
		//   COL -->
		//       
		// R     
		// O     
		// W     +--
		// |       
		// V         
		*/
		edgeIndices[count++] = 3 * vertexIndex;
		if (row < getNbRows() - 1)
		{
			/*
			//   COL -->
			//       
			// R     
			// O     
			// W     +  
			// |      \
			// V       \ 
			*/
			if (isZerothVertexShared(vertexIndex))
			{
				edgeIndices[count++] = 3 * vertexIndex + 1;
			}
		}
	}
	/*
	//   COL -->
	//       
	// R     
	// O     
	// W     +  
	// |     |
	// V     | 
	*/
	if (row < getNbRows() - 1)
	{
		edgeIndices[count++] = 3 * vertexIndex + 2;
	}
	if (column > 0)
	{
		/*
		//   COL -->
		//        
		// R   \  
		// O    \ 
		// W   --+
		// |    / 
		// V   /   
		*/
		if (row < getNbRows() - 1)
		{
			if (!isZerothVertexShared(vertexIndex - 1))
			{
				edgeIndices[count++] = 3 * (vertexIndex - 1) + 1;
			}
		}
		edgeIndices[count++] = 3 * (vertexIndex - 1);
		if (row > 0)
		{
			if (isZerothVertexShared(vertexIndex - getNbColumns() - 1))
			{
				edgeIndices[count++] = 3 * (vertexIndex - getNbColumns() - 1) + 1;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE bool PxcHeightField::isConvexVertex(PxU32 vertexIndex) const
{
	PxU32 row    = vertexIndex / getNbColumns();
	PxU32 column = vertexIndex % getNbColumns();

	PxFloat definedInX = false;
	PxFloat definedInZ = false;

	PxFloat convexityX = 0;
	PxFloat convexityZ = 0;

	PxFloat h0 = PxFloat(2) * getHeight(vertexIndex);

	if ((row > 0) &&  (row < getNbRows() - 1))
	{
		convexityX = h0 - getHeight(vertexIndex + getNbColumns()) - getHeight(vertexIndex - getNbColumns());
		definedInX = true;
	}
	if ((column > 0) &&  (column < getNbColumns() - 1))
	{
		convexityZ = h0 - getHeight(vertexIndex + 1) - getHeight(vertexIndex - 1);
		definedInZ = true;
	}

	if (definedInX || definedInZ)
	{
		// saddle points
		if ((convexityX > 0) && (convexityZ < 0)) 
			return false;		
		if ((convexityX < 0) && (convexityZ > 0)) 
			return false;
		// inequality depends on vertical extent and offset by threshold.
		if (getVerticalExtent() <= 0 && getThickness() <= 0) 
			return (convexityX + convexityZ) > getConvexEdgeThreshold();
		else 
			return (convexityX + convexityZ) < -getConvexEdgeThreshold();
	}

	// this has to be one of the two corner vertices
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE bool PxcHeightField::isValidEdge(PxU32 edgeIndex) const
{
	PxU32 cell   = (edgeIndex / 3);
	PxU32 row    = cell / getNbColumns();
	PxU32 column = cell % getNbColumns();
	switch (edgeIndex % 3)
	{
	case 0:
		if (row > getNbRows() - 1) return false;
		if (column >= getNbColumns() - 1) return false;
		break;
	case 1:
		if (row >= getNbRows() - 1) return false;
		if (column >= getNbColumns() - 1) return false;
		break;
	case 2:
		if (row >= getNbRows() - 1) return false;
		if (column > getNbColumns() - 1) return false;
		break;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE void PxcHeightField::getEdgeTriangleIndices(PxU32 edgeIndex, PxU32 triangleIndices[2], PxU32& count) const
{
	PxU32 cell   = edgeIndex / 3;
	PxU32 row    = cell / getNbColumns();
	PxU32 column = cell % getNbColumns();
	count = 0;
	switch (edgeIndex % 3)
	{
	case 0:
		if (column < getNbColumns() - 1)
		{
			if (row > 0)
			{
				if (isZerothVertexShared(cell - getNbColumns()))
					triangleIndices[count++] = ((cell - getNbColumns()) << 1);
				else 
					triangleIndices[count++] = ((cell - getNbColumns()) << 1) + 1;
			}
			if (row < getNbRows() - 1)
			{
				if (isZerothVertexShared(cell))
					triangleIndices[count++] = (cell << 1) + 1;
				else 
					triangleIndices[count++] = cell << 1;
			}
		}
		break;
	case 1:
		if ((row < getNbRows() - 1) && (column < getNbColumns() - 1))
		{
			triangleIndices[count++] = cell << 1;
			triangleIndices[count++] = (cell << 1) + 1;
		}
		break;
	case 2:
		if (row < getNbRows() - 1)
		{
			if (column > 0)
				triangleIndices[count++] = ((cell - 1) << 1) + 1;
			if (column < getNbColumns() - 1)
				triangleIndices[count++] = cell << 1;
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE void PxcHeightField::getEdgeVertexIndices(PxU32 edgeIndex, PxU32& vertexIndex0, PxU32& vertexIndex1) const
{
	PxU32 cell = edgeIndex / 3;
	switch (edgeIndex % 3)
	{
	case 0:
		vertexIndex0 = cell;
		vertexIndex1 = cell + 1;
		break;
	case 1:
		if (isZerothVertexShared(cell))
		{
			vertexIndex0 = cell;
			vertexIndex1 = cell + getNbColumns() + 1;
		}
		else
		{
			vertexIndex0 = cell + 1;
			vertexIndex1 = cell + getNbColumns();
		}
		break;
	case 2:
		vertexIndex0 = cell;
		vertexIndex1 = cell + getNbColumns();
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE bool PxcHeightField::isConvexEdge(PxU32 edgeIndex) const
{
	PxU32 cell = edgeIndex / 3;

	PxU32 row = cell / getNbColumns();
	if (row > getNbRows()-2) return false;

	PxU32 column = cell % getNbColumns();
	if (column > getNbColumns()-2) return false;

	PxFloat h0 = 0, h1 = 0, h2 = 0, h3 = 0;
	PxFloat convexity = 0;

	switch (edgeIndex % 3)
	{
	case 0:
		if (row < 1) return false;
		if(isZerothVertexShared(cell - getNbColumns())) 
		{
			//      <------ COL  
			//       +----+  0  R
			//       |   /  /#  O
			//       |  /  / #  W
			//       | /  /  #  |
			//       |/  /   #  |
			//       +  +====1  |
			//                  |
			//                  |
			//                  |
			//                  |
			//                  |
			//                  |
			//                  V
			//      
			h0 = getHeight(cell - getNbColumns());
			h1 = getHeight(cell);
		}
		else
		{
			//      <------ COL  
			//       0  +----+  R
			//       #\  \   |  O
			//       # \  \  |  W
			//       #  \  \ |  |
			//       #   \  \|  |
			//       1====+  +  |
			//                  |
			//                  |
			//                  |
			//                  |
			//                  |
			//                  |
			//                  V
			//      
			h0 = getHeight(cell - getNbColumns() + 1);
			h1 = getHeight(cell + 1);
		}
		if(isZerothVertexShared(cell)) 
		{
			//      <------ COL  
			//                  R
			//                  O
			//                  W
			//                  |
			//                  |
			//                  |
			//       2====+  0  |
			//       #   /  /|  |
			//       #  /  / |  |
			//       # /  /  |  |
			//       #/  /   |  |
			//       3  +----+  |
			//                  V
			//      
			h2 = getHeight(cell + 1);
			h3 = getHeight(cell + getNbColumns() + 1);
		}
		else
		{
			//      <------ COL  
			//                  R
			//                  O
			//                  W
			//                  |
			//                  |
			//                  |
			//       +  +====2  |
			//       |\  \   #  |
			//       | \  \  #  |
			//       |  \  \ #  |
			//       |   \  \#  |
			//       +----+  3  |
			//                  V
			//      
			h2 = getHeight(cell);
			h3 = getHeight(cell + getNbColumns());
		}
		//convex = (h3-h2) < (h1-h0);
		convexity = (h1-h0) - (h3-h2);
		break;
	case 1:
		h0 = getHeight(cell);
		h1 = getHeight(cell + 1);
		h2 = getHeight(cell + getNbColumns());
		h3 = getHeight(cell + getNbColumns() + 1);
		if (isZerothVertexShared(cell))
			//convex = (h0 + h3) > (h1 + h2);
			convexity = (h0 + h3) - (h1 + h2);
		else 
			//convex = (h2 + h1) > (h0 + h3);
			convexity = (h2 + h1) - (h0 + h3);
		break;
	case 2:
		if (column < 1) return false;
		if(isZerothVertexShared(cell-1)) 
		{
			//      <-------------- COL  
			//                1====0  + R
			//                +   /  /| O
			//                +  /  / | W
			//                + /  /  | |
			//                +/  /   | |
			//                +  +----+ V
			//      
			h0 = getHeight(cell - 1);
			h1 = getHeight(cell);
		}
		else
		{
			//      <-------------- COL  
			//                +  +----+ R
			//                +\  \   | O
			//                + \  \  | W
			//                +  \  \ | |
			//                +   \  \| |
			//                1====0  + V
			//      
			h0 = getHeight(cell - 1 + getNbColumns());
			h1 = getHeight(cell + getNbColumns());
		}
		if(isZerothVertexShared(cell)) 
		{
			//      <-------------- COL  
			//       +----+  +          R
			//       |   /  /+          O
			//       |  /  / +          W
			//       | /  /  +          |
			//       |/  /   +          |
			//       +  3====2          V
			//      
			h2 = getHeight(cell + getNbColumns());
			h3 = getHeight(cell + getNbColumns() + 1);
		}
		else
		{
			//      <-------------- COL  
			//       +  3====2          R
			//       |\  \   +          O
			//       | \  \  +          W
			//       |  \  \ +          |
			//       |   \  \+          |
			//       +----+  +          V
			//      
			h2 = getHeight(cell);
			h3 = getHeight(cell + 1);
		}
		//convex = (h3-h2) < (h1-h0);
		convexity = (h1-h0) - (h3-h2);
		break;
	}

	if (getVerticalExtent() <= 0 && getThickness() <= 0)
	{
		return convexity > getConvexEdgeThreshold();
	}
	else
	{
		return convexity < -getConvexEdgeThreshold();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE void PxcHeightField::computeCellCoordinates(PxFloat x, PxFloat z, PxU32& row, PxU32& column, PxFloat& fracX, PxFloat& fracZ) const
{
	// ptchernev: 
	// Had to remove this assert since it may happen sometimes due to float precision (or lack thereof).
	// When HeightFieldShape::raycast() clips the ray the clipped points may end up ever so slightly on
	// the wrong side of the height field bounds. I could have fixed the problem there but I could only 
	// guarantee that the points where inside the shape space bounds. Something may still have gone wrong 
	// when transforming to height field grid space.
	// PXN_ASSERT(!((x < 0) || (z < 0) || (x > getNbRows() - 1) || (z > getNbColumns() - 1)));

	// ptchernev:
	// Apparently there is no standard for float to int conversion of negative values (cause that would make sense).
	if (x < 0) x = 0;
	if (z < 0) z = 0;

	row = (PxU32)x;
	fracX = x - PxFloat(row);
	if (row > getNbRows() - 2) 
	{
		row = getNbRows() - 2;
		fracX = PxFloat(1);
	}

	column = (PxU32)z;
	fracZ = z - PxFloat(column);
	if (column > getNbColumns() - 2) 
	{
		column = getNbColumns() - 2;
		fracZ = PxFloat(1);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxU32 PxcHeightField::getMinRow(PxFloat x) const
{
	return PxcClamp(PxI32(PxcFloor(x)), PxI32(getNbRows()-2), PxI32(0));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxU32 PxcHeightField::getMaxRow(PxFloat x) const
{
	return PxcClamp(PxI32(PxcCeil(x)), PxI32(getNbRows()-1), PxI32(0));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxU32 PxcHeightField::getMinColumn(PxFloat z) const
{
	return PxcClamp(PxI32(PxcFloor(z)), PxI32(getNbColumns()-2), PxI32(0));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxU32 PxcHeightField::getMaxColumn(PxFloat z) const
{
	return PxcClamp(PxI32(PxcCeil(z)), PxI32(getNbColumns()-1), PxI32(0));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE bool PxcHeightField::isValidTriangle(PxU32 triangleIndex) const
{
	PxU32 cell = triangleIndex >> 1;
	PxU32 row  = cell / getNbColumns();
	if (row >= (getNbRows() - 1)) return false;
	PxU32 column = cell % getNbColumns();
	if (column >= (getNbColumns() - 1)) return false;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE bool PxcHeightField::isFirstTriangle(PxU32 triangleIndex) const
{
	return ((triangleIndex & 0x1) == 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxMaterialIndex PxcHeightField::getTriangleMaterial(PxU32 triangleIndex) const
{
	return isFirstTriangle(triangleIndex) ? getMaterialIndex0(triangleIndex >> 1) : getMaterialIndex1(triangleIndex >> 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE void PxcHeightField::getTriangleVertexIndices(PxU32 triangleIndex, PxU32& vertexIndex0, PxU32& vertexIndex1, PxU32& vertexIndex2) const
{
	PxU32 cell = triangleIndex >> 1;
	if (isZerothVertexShared(cell))
	{
		//      <---- COL  
		//      0----2  1 R
		//      | 1 /  /| O
		//      |  /  / | W
		//      | /  /  | |
		//      |/  / 0 | |
		//      1  2----0 V
		//      
		if (isFirstTriangle(triangleIndex))
		{
			vertexIndex0 = cell + getNbColumns();
			vertexIndex1 = cell;
			vertexIndex2 = cell + getNbColumns() + 1;
		}
		else
		{
			vertexIndex0 = cell + 1;
			vertexIndex1 = cell + getNbColumns() + 1;
			vertexIndex2 = cell;
		}
	}
	else
	{
		//      <---- COL  
		//      2  1----0 R
		//      |\  \ 0 | O
		//      | \  \  | W
		//      |  \  \ | |
		//      | 1 \  \| |
		//      0----1  2 V
		//                   
		if (isFirstTriangle(triangleIndex))
		{
			vertexIndex0 = cell;
			vertexIndex1 = cell + 1;
			vertexIndex2 = cell + getNbColumns();
		}
		else
		{
			vertexIndex0 = cell + getNbColumns() + 1;
			vertexIndex1 = cell + getNbColumns();
			vertexIndex2 = cell + 1;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE void PxcHeightField::getTriangleEdgeIndices(PxU32 triangleIndex, PxU32& edgeIndex0, PxU32& edgeIndex1, PxU32& edgeIndex2) const
{
	PxU32 cell = triangleIndex >> 1;
	PxU32 edge = 3 * cell;
	//      <-- COL  
	//      +--0--+ R
	//      |:   :| O
	//      | : : | W
	//      |  1  2 |
	//      | : : | |
	//      |:   :| |
	//      +-----+ V
	//      
	if (isZerothVertexShared(cell))
	{
		//      <---- COL  
		//      +-2--+  + R
		//      |   /  /| O
		//      0  1  / | W
		//      | /  1  0 |
		//      |/  /   | |
		//      +  +--2-+ V
		//      
		if (isFirstTriangle(triangleIndex))
		{
			edgeIndex0 = edge + 2;
			edgeIndex1 = edge + 1;
			edgeIndex2 = edge + 3*getNbColumns();
		}
		else
		{
			edgeIndex0 = edge + 3 + 2;
			edgeIndex1 = edge + 1;
			edgeIndex2 = edge;
		}
	}
	else
	{
		//      <---- COL  
		//      +  +--0-+ R
		//      |\  \   | O
		//      | \  1  2 W
		//      2  1  \ | |
		//      |   \  \| |
		//      +-0--+  + V
		//      
		if (isFirstTriangle(triangleIndex))
		{
			edgeIndex0 = edge;
			edgeIndex1 = edge + 1;
			edgeIndex2 = edge + 2;
		}
		else
		{
			edgeIndex0 = edge + 3*getNbColumns();
			edgeIndex1 = edge + 1;
			edgeIndex2 = edge + 3 + 2;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxcVector PxcHeightField::getTriangleNormal(PxU32 triangleIndex) const
{
	PxU32 v0;
	PxU32 v1;
	PxU32 v2;
	getTriangleVertexIndices(triangleIndex, v0, v1, v2); 
	PxFloat h0 = getHeight(v0);
	PxFloat h1 = getHeight(v1);
	PxFloat h2 = getHeight(v2);
	PxcVector n/* = PxcVector(0,1,0)*/;
	n.set(0.0f,1.0f,0.0f);
	PxU32 cell = triangleIndex >> 1;
	if (isZerothVertexShared(cell))
	{
		//      <---- COL  
		//      0----2  1 R
		//      | 1 /  /| O
		//      |  /  / | W
		//      | /  /  | |
		//      |/  / 0 | |
		//      1  2----0 V
		//      
		if (isFirstTriangle(triangleIndex))
		{
			n.x = -(h0-h1);
			n.z = -(h2-h0);
		}
		else
		{
			n.x = -(h1-h0);
			n.z = -(h0-h2);
		}
	}
	else
	{
		//      <---- COL  
		//      2  1----0 R
		//      |\  \ 0 | O
		//      | \  \  | W
		//      |  \  \ | |
		//      | 1 \  \| |
		//      0----1  2 V
		//                   
		if (isFirstTriangle(triangleIndex))
		{
			n.x = -(h2-h0);
			n.z = -(h1-h0);
		}
		else
		{
			n.x = -(h0-h2);
			n.z = -(h0-h1);
		}
	}
	return n;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxFloat PxcHeightField::getHeight(PxFloat x, PxFloat z) const
{
	PxU32 row, column;
	PxFloat fracX, fracZ;
	computeCellCoordinates(x, z, row, column, fracX, fracZ);
	PxU32 vertexIndex = row * getNbColumns() + column;
	if (isZerothVertexShared(vertexIndex))
	{
		//    <----Z---+
		//      +----+ | 
		//      |   /| |
		//      |  / | X
		//      | /  | |
		//      |/   | |
		//      +----+ |
		//             V
		if (fracZ > fracX)
		{
			//    <----Z---+
			//      1----0 | 
			//      |   /  |
			//      |  /   X
			//      | /    |
			//      |/     |
			//      2      |
			//             V
			PxFloat h0 = getHeight(vertexIndex);
			PxFloat h1 = getHeight(vertexIndex + 1);
			PxFloat h2 = getHeight(vertexIndex + getNbColumns() + 1);
			return h0 + fracZ*(h1-h0) + fracX*(h2-h1);
		}
		else
		{
			//    <----Z---+
			//           0 | 
			//          /| |
			//         / | X
			//        /  | |
			//       /   | |
			//      2----1 |
			//             V
			PxFloat h0 = getHeight(vertexIndex);
			PxFloat h1 = getHeight(vertexIndex + getNbColumns());
			PxFloat h2 = getHeight(vertexIndex + getNbColumns() + 1);
			return h0 + fracX*(h1-h0) + fracZ*(h2-h1);
		}
	}
	else
	{
		//    <----Z---+
		//      +----+ | 
		//      |\   | |
		//      | \  | X
		//      |  \ | |
		//      |   \| |
		//      +----+ |
		//             V
		if (fracX + fracZ < 1)
		{
			//    <----Z---+
			//      1----0 | 
			//       \   | |
			//        \  | X
			//         \ | |
			//          \| |
			//           2 |
			//             V
			PxFloat h0 = getHeight(vertexIndex);
			PxFloat h1 = getHeight(vertexIndex + 1);
			PxFloat h2 = getHeight(vertexIndex + getNbColumns());
			return h0 + fracZ*(h1-h0) + fracX*(h2-h0);
		}
		else
		{
			//    <----Z---+
			//      2      | 
			//      |\     |
			//      | \    X
			//      |  \   |
			//      |   \  |
			//      0----1 |
			//             V
			//
			// Note that we need to flip fracX and fracZ since we are moving the origin
			PxFloat h2 = getHeight(vertexIndex + 1);
			PxFloat h1 = getHeight(vertexIndex + getNbColumns());
			PxFloat h0 = getHeight(vertexIndex + getNbColumns() + 1);
			return h0 + (1-fracZ)*(h1-h0) + (1-fracX)*(h2-h0);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxcVector PxcHeightField::getNormal(PxFloat x, PxFloat z) const
{
	PxU32 row, column;
	PxFloat fracX, fracZ;
	computeCellCoordinates(x, z, row, column, fracX, fracZ);

	PxcVector normal;
	PxU32 vertexIndex = row * getNbColumns() + column;
	if (isZerothVertexShared(vertexIndex))
	{
		//    <----Z---+
		//      +----+ | 
		//      |   /| |
		//      |  / | X
		//      | /  | |
		//      |/   | |
		//      +----+ |
		//             V
		if (fracZ >= fracX)
		{
			//    <----Z---+
			//      1----0 | 
			//      |   /  |
			//      |  /   X
			//      | /    |
			//      |/     |
			//      2      |
			//             V
			PxFloat h0 = getHeight(vertexIndex);
			PxFloat h1 = getHeight(vertexIndex + 1);
			PxFloat h2 = getHeight(vertexIndex + getNbColumns() + 1);
			normal.set(-(h2-h1),1,-(h1-h0));
		}
		else
		{
			//    <----Z---+
			//           0 | 
			//          /| |
			//         / | X
			//        /  | |
			//       /   | |
			//      2----1 |
			//             V
			PxFloat h0 = getHeight(vertexIndex);
			PxFloat h1 = getHeight(vertexIndex + getNbColumns());
			PxFloat h2 = getHeight(vertexIndex + getNbColumns() + 1);
			normal.set(-(h1-h0),1,-(h2-h1));
		}
	}
	else
	{
		//    <----Z---+
		//      +----+ | 
		//      |\   | |
		//      | \  | X
		//      |  \ | |
		//      |   \| |
		//      +----+ |
		//             V
		if (fracX + fracZ <= PxFloat(1))
		{
			//    <----Z---+
			//      1----0 | 
			//       \   | |
			//        \  | X
			//         \ | |
			//          \| |
			//           2 |
			//             V
			PxFloat h0 = getHeight(vertexIndex);
			PxFloat h1 = getHeight(vertexIndex + 1);
			PxFloat h2 = getHeight(vertexIndex + getNbColumns());
			normal.set(-(h2-h0),1,-(h1-h0));
		}
		else
		{
			//    <----Z---+
			//      2      | 
			//      |\     |
			//      | \    X
			//      |  \   |
			//      |   \  |
			//      0----1 |
			//             V
			//
			// Note that we need to flip fracX and fracZ since we are moving the origin
			PxFloat h2 = getHeight(vertexIndex + 1);
			PxFloat h1 = getHeight(vertexIndex + getNbColumns());
			PxFloat h0 = getHeight(vertexIndex + getNbColumns() + 1);
			normal.set(-(h0-h2),1,-(h0-h1));
		}
	}

	return (getVerticalExtent() <= 0 && getThickness() <= 0) ? normal : -normal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxU32 PxcHeightField::getTriangleIndex(PxFloat x, PxFloat z) const
{
	PxU32 row, column;
	PxFloat fracX, fracZ;
	computeCellCoordinates(x, z, row, column, fracX, fracZ);
	PxU32 cell = row * getNbColumns() + column;
	if (isZerothVertexShared(cell))
		return (fracZ > fracX) ? (cell << 1) + 1 : (cell << 1);
	else
		return (fracX + fracZ > 1) ? (cell << 1) + 1 : (cell << 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PXD_INLINE PxMaterialIndex PxcHeightField::getMaterial(PxFloat x, PxFloat z) const
{
	return getTriangleMaterial(getTriangleIndex(x, z));
}

#endif
