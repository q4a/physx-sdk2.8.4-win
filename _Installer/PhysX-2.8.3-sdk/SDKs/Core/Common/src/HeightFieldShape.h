#ifndef NX_COLLISION_HEIGHTFIELDSHAPE
#define NX_COLLISION_HEIGHTFIELDSHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxHeightFieldShape.h"
#include "Shape.h"

#include "HeightField.h"

class NpHeightField;
class NvHeightFIeld;

// Coordinate Spaces:
// =================
//
//  World space:
//   Needs no introduction.
//
//  Shape space:
//   A height field vertex in this space has coordinates:
//   (row * rowScale, sample(row,column) * heightScale, column * columnScale)
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
class HeightFieldShape : public Shape
	{
	protected:
									HeightFieldShape(const NvShapeDesc & desc, Scene *, CoreActor& actor, NxU32 uid);
	virtual							~HeightFieldShape();

	public:

	// Shape interface
	virtual		Shape&				getShape()			{ return *this;	}
	virtual		const Shape&		getShape()	const	{ return *this;	}
	virtual		operator			Shape&()			{ return *this;	}
	virtual		void				visualize(NxFoundation::DebugRenderable& renderer) const;
	virtual		bool				raycast(const NxRay& worldRay, NxReal maxDist, NxU32 hintFlags, NxRaycastHit& hit, bool firstHit)	const;
	virtual		bool				checkOverlapSphere(const NxSphere& worldSphere)														const;
	virtual		bool				checkOverlapOBB(const NxBox& worldBox)																const;
	virtual		bool				checkOverlapAABB(const NxBounds3& worldBounds)														const;
	virtual		bool				checkOverlapCapsule(const NxCapsule& worldCapsule)													const;

	virtual		void				computeLocalBounds(NxBounds3& bounds) const;
	
	virtual		NxU32				getMeshFlagsV()			const	{ return getMeshFlags(); }
	virtual		NxReal				getHeightScaleV()		const	{ return getHeightScale(); }
	virtual		NxReal				getRowScaleV()			const	{ return getRowScale();	}
	virtual		NxReal				getColumnScaleV()		const	{ return getColumnScale(); }
	virtual		NxMaterialIndex		getMaterialIndexHighBitsV()const	{ return getMaterialIndexHighBits(); }
	virtual		NxMaterialIndex		getHoleMaterialV()		const	{ return getHoleMaterial(); }

	virtual		bool				isShapePointOnHeightFieldV(NxReal x, NxReal z) const 
		{ 
		return isShapePointOnHeightField(x, z); 
		}

	virtual		NxReal				getHeightAtShapePointV(NxReal x, NxReal z) const 
		{ 
		return isShapePointOnHeightField(x, z) ? getHeightAtShapePoint(x, z) : 0; 
		}

	virtual		NxMaterialIndex		getMaterialAtShapePointV(NxReal x, NxReal z) const 
		{
		if (isShapePointOnHeightField(x, z))
			{
			NxU32 triangleIndex = getFeatureIndexAtShapePoint(x, z);
			if (triangleIndex != 0xffffffff) return getHeightFieldFast().getTriangleMaterial(triangleIndex);
			}
		return 0xffff;
		}

	virtual		NxVec3				getNormalAtShapePointV(NxReal x, NxReal z) const 
		{ 
		return isShapePointOnHeightField(x, z) ? getNormalAtShapePoint(x, z) : NxVec3(0,0,0); 
		}
	
	virtual		NxVec3				getSmoothNormalAtShapePointV(NxReal x, NxReal z) const 
		{
		return isShapePointOnHeightField(x, z) ? getSmoothNormalAtShapePoint(x, z) : NxVec3(0,0,0);
		}
	//~Shape interface

				bool				overlapAABBTriangles(const NxBounds3 bounds, NxU32 flags, NxUserEntityReport<NxU32>* callback) const;

				NxU32				getTriangle(NxTriangle& worldTri, NxTriangle* edgeTri, NxU32* returnedFlags, NxTriangleID triangleIndex, bool worldSpaceTranslation=true, bool worldSpaceRotation=true) const;

				NpHeightField*		getHeightField();//TODO: make this inline again if possible after refactor
				NvHeightField*		getNvHeightField();
				const NvHeightField* getNvHeightField() const;
				void				setHeightField(NvHeightField*);

	NX_INLINE	const HeightField&	getHeightFieldFast()	const	{ return *heightField;			}
	NX_INLINE	HeightField&		getHeightFieldFast()			{ return *heightField;			}
	NX_INLINE	void				setHeightFieldFast(HeightField* hf)	{ heightField = hf;			}

	// properties

	NX_INLINE	NxU32				getMeshFlags()			const	{ return meshFlags;				}
	NX_INLINE	NxReal				getHeightScale()		const	{ return heightScale;			}
	NX_INLINE	NxReal				getRowScale()			const	{ return rowScale;				}
	NX_INLINE	NxReal				getColumnScale()		const	{ return columnScale;			}
	NX_INLINE	NxMaterialIndex		getMaterialIndexHighBits()const	{ return materialIndexHighBits;	}
	NX_INLINE	NxMaterialIndex		getHoleMaterial()		const	{ return holeMaterial;			}
	NX_INLINE	bool				useMultiMaterials()		const	{ return singleMaterial == holeMaterial; }
	NX_INLINE	NxMaterialIndex		getSingleMaterial()		const	{ return singleMaterial;		}

				void				setMeshFlags(NxU32 f);
				void				setHeightScale(NxReal s);
				void				setRowScale(NxReal s);
				void				setColumnScale(NxReal s);
				void				setMaterialIndexHighBits(NxMaterialIndex m);
				void				setHoleMaterial(NxMaterialIndex m);

	NX_INLINE	NxReal				getOneOverRowScale()	const	{ return oneOverRowScale;				}
	NX_INLINE	NxReal				getOneOverColumnScale()	const	{ return oneOverColumnScale;			}

	NX_INLINE	bool				isShapePointOnHeightField(NxReal x, NxReal z) const;

	NX_INLINE	NxReal				getHeightAtShapePoint(NxReal x, NxReal z) const;

	// Returns the normal direction (not normalized) at the point in shape space.
	NX_INLINE	NxVec3				getNormalAtShapePoint(NxReal x, NxReal z) const;
	// Returns the smoothed normal direction (not normalized) at the point in shape space.
				NxVec3				getSmoothNormalAtShapePoint(NxReal x, NxReal z) const;
	// Returns the vertex normal direction (normalized) at the point in shape space.
				NxVec3				getVertexNormal(NxU32 vertexIndex) const;

	// Clips a normal direction to the voronoi volume of the edge.
	// Returns true if direction was clipped.
	// Results are undefined when clipping to the *negative* volume.
				bool				clipShapeNormalToEdgeVoronoi(NxVec3& normal, NxU32 edgeIndex) const;

	// Clips a normal direction to the voronoi volume of the vertex.
	// Returns true if direction was clipped.
	// Results are undefined when clipping to the *negative* volume.
				bool				clipShapeNormalToVertexVoronoi(NxVec3& normal, NxU32 vertexIndex) const;

	// Transform a position vector from height field space to shape space.
	NX_INLINE	NxVec3				hf2shapep(const NxVec3& v) const;
	// Transform a direction vector from height field space to shape space.
	NX_INLINE	NxVec3				hf2shapen(const NxVec3& v) const;
	// Transform a position vector from shape space to height field space.
	NX_INLINE	NxVec3				shape2hfp(const NxVec3& v) const;
	// Transform a direction vector from shape space to height field space.
	NX_INLINE	NxVec3				shape2hfn(const NxVec3& v) const;
	// Transform a position vector from height field space to world space.
	NX_INLINE	NxVec3				hf2worldp(const NxVec3& v) const;
	// Transform a direction vector from height field space to world space.
	NX_INLINE	NxVec3				hf2worldn(const NxVec3& v) const;
	// Transform a position vector from world space to height field space.
	NX_INLINE	NxVec3				world2hfp(const NxVec3& w) const;
	// Transform a position vector from shape space to world space.
	NX_INLINE	NxVec3				shape2worldp(const NxVec3& v) const;
	// Transform a direction vector from shape space to world space.
	NX_INLINE	NxVec3				shape2worldn(const NxVec3& v) const;

	// A boundary edge has exactly one solid face.
				bool				isBoundaryEdge(NxU32 edgeIndex) const;

	// A collision edge is a boundary edge or a convex edge.
				bool				isCollisionEdge(NxU32 edgeIndex) const;

	// A collision vertex has at least one collision edge.
	// This definition is really too loose, but the alternative was too expensive.
				bool				isCollisionVertex(NxU32 vertexIndex) const;

	// Returns the index of the solid triangle at the given point in shape space or 0xffffffff.
				NxU32				getFeatureIndexAtShapePoint(NxReal x, NxReal z) const;
	
	// Returns the index of the first solid triangle that this edge is part of or 0xffffffff.
				NxU32				getEdgeFeatureIndex(NxU32 edgeIndex) const;
	
	// Returns the index of the first solid triangle that this vertex is part of or 0xffffffff.
				NxU32				getVertexFeatureIndex(NxU32 vertexIndex) const;

	// Returns the origin and extent vectors in shape space for the given edge.
				void				getEdge(NxU32 edgeIndex, NxVec3& origin, NxVec3& extent) const;

	// Returns the direction vector in shape space for the given edge.
				NxVec3				getEdgeDirection(NxU32 edgeIndex) const;

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
				NxU32				findClosestPointsOnCell(NxU32 row, NxU32 column, NxVec3 point, NxVec3* closestPoints, NxU32* features, bool testEdges = true) const;
	
	// Finds closest point in shape space on the given edge.
	// The result is stored in the closestPoint parameter and will be on the edge.
	// The return value is the projection of point on the edge and can be used to
	// check if the closest point is internal to the edge or on the vertices.
				NxReal				findClosestPointOnEdge(NxU32 edgeIndex, const NxVec3& point, NxVec3& closestPoint) const;
	
	// Finds the projection of point in shape space on the triangle.
	// If the return value is true the projection of point was inside the triangle and closestPoint is valid.
	// If the return value is false the projection of point was outside the triangle and closestPoint undefined.
				bool				findProjectionOnTriangle(NxU32 triangleIndex, const NxVec3& point, NxVec3& projection) const;

	// Traces the intersections of the line p0-p1 with the row, column and diagonal edges of the height field 
	// in the X-Z plane in shape space.
	// T is a class / struct with a method:
	//    bool contact(
	//       const HeightFieldShape& hfShape, 
	//       NxU32 edgeIndex, 
	//       NxU32 row,
	//       NxU32 column, 
	//       NxReal x, 
	//       NxReal y, 
	//       NxReal z, 
	//       NxReal h)
	// This method is called at every intersection point.
	// edgeIndex is the intersected edge.
	// row and column indicate the cell.
	// x and z are coordinates within the cell in normalized space (height field space).
	// y is the y value of the line p0-p1 at the intersection point.
	// h is the height field height at the intersection point (scaled by heightScale).
	// So, the point on the line at the intersection is: 
	//    rowScale * (row + x), y, columnScale * (column + y)
	// and the point on the height field at the intersection is: 
	//    rowScale * (row + x), h, columnScale * (column + y)
	// The reason for this non-uniform mapping is that many contact handlers will have an
	// early out depending on the difference between h and y and may not care to evaluate 
	// the points.
	template<class T>
	NX_INLINE	void				traceLine(const NxVec3& p0, const NxVec3& p1, T* callback) const;

	private:
				HeightField *		heightField;
				NxReal				heightScale;
				NxReal				rowScale;
				NxReal				columnScale;
				NxMaterialIndex		materialIndexHighBits;
				NxMaterialIndex		holeMaterial;
				NxMaterialIndex		singleMaterial; // The only material used in the HF; == holeMaterial if multiple materials used
				NxU32				meshFlags;
				NxReal				oneOverRowScale;
				NxReal				oneOverColumnScale;
				NxReal				oneOverHeightScale;

	// Internal methods
	virtual		void				computeWorldBounds(NxBounds3&)	const;
	virtual		void				computeWorldSphere(NxSphere&)	const;
	virtual		void				computeLocalSphere(NxSphere&)	const;
	
	// Internal methods called by traceLine
	template<class T>
	NX_INLINE	bool				uintercept(NxI32 row, NxI32 column, NxReal z, NxReal y, T* callback) const;
	template<class T>
	NX_INLINE	bool				vintercept(NxI32 row, NxI32 column, NxReal x, NxReal y, T* callback) const;
	template<class T>
	NX_INLINE	bool				dintercept0(NxI32 row, NxI32 column, NxReal x, NxReal z, NxReal y, T* callback) const;
	template<class T>
	NX_INLINE	bool				dintercept1(NxI32 row, NxI32 column, NxReal x, NxReal z, NxReal y, T* callback) const;

				NxMaterialIndex		findSingleMaterial();

	friend class Scene;	// For factory function
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NX_INLINE bool HeightFieldShape::isShapePointOnHeightField(NxReal x, NxReal z) const
		{
		x *= oneOverRowScale;
		z *= oneOverColumnScale;
		return ((!(x < 0))
			&&  (!(z < 0))
			&&  (x < (getHeightFieldFast().getNbRows()-1)) 
			&&  (z < (getHeightFieldFast().getNbColumns()-1)));
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NX_INLINE NxReal HeightFieldShape::getHeightAtShapePoint(NxReal x, NxReal z) const
		{
#ifdef NX_HEIGHTFIELD_DEBUG
		NX_ASSERT(isShapePointOnHeightField(x, z));
#endif
		return heightScale * heightField->getHeight(x * oneOverRowScale, z * oneOverColumnScale);
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NX_INLINE NxVec3 HeightFieldShape::getNormalAtShapePoint(NxReal x, NxReal z) const
		{
#ifdef NX_HEIGHTFIELD_DEBUG
		NX_ASSERT(isShapePointOnHeightField(x, z));
#endif
		return hf2shapen(heightField->getNormal(x * oneOverRowScale, z * oneOverColumnScale));
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NX_INLINE NxVec3 HeightFieldShape::hf2shapep(const NxVec3& v) const
		{
		return NxVec3(v.x * rowScale, v.y * heightScale, v.z * columnScale);
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NX_INLINE NxVec3 HeightFieldShape::hf2shapen(const NxVec3& v) const
		{
		return NxVec3(v.x * oneOverRowScale, v.y * oneOverHeightScale, v.z * oneOverColumnScale);
//		return NxVec3(v.x*heightScale*columnScale, v.y*rowScale*columnScale, v.z*heightScale*rowScale);
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NX_INLINE NxVec3 HeightFieldShape::shape2hfp(const NxVec3& v) const
		{
		return NxVec3(v.x * oneOverRowScale, v.y * oneOverHeightScale, v.z * oneOverColumnScale);
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NX_INLINE NxVec3 HeightFieldShape::shape2hfn(const NxVec3& v) const
		{
		return NxVec3(v.x * rowScale, v.y * heightScale, v.z * columnScale);
//		return NxVec3(v.x/(heightScale*columnScale), v.y/(rowScale*columnScale), v.z/(heightScale*rowScale));
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NX_INLINE NxVec3 HeightFieldShape::hf2worldp(const NxVec3& v) const
		{
		NxVec3 s = hf2shapep(v);
		NxVec3 w;
		getAbsPoseFast().multiply(s, w);
		return w;
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NX_INLINE NxVec3 HeightFieldShape::hf2worldn(const NxVec3& v) const
		{
		NxVec3 s = hf2shapen(v);
		NxVec3 w;
		getAbsPoseFast().M.multiply(s, w);
		return w;
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NX_INLINE NxVec3 HeightFieldShape::world2hfp(const NxVec3& w) const
		{
		NxMat34 invPose;
		getAbsPoseFast().getInverseRT(invPose);
		NxVec3 s;
		invPose.multiply(w, s);
		return shape2hfp(s);
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NX_INLINE NxVec3 HeightFieldShape::shape2worldp(const NxVec3& v) const
		{
		return getAbsPoseFast() * v;
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	NX_INLINE NxVec3 HeightFieldShape::shape2worldn(const NxVec3& v) const
		{
		return getAbsPoseFast().M * v;
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class T>
	NX_INLINE bool HeightFieldShape::uintercept(NxI32 row, NxI32 column, NxReal z, NxReal y, T* callback) const
		{
		if (row < 0) return true;
		if (column < 0) return true;
		const HeightField& hf = getHeightFieldFast();
		if (row	> NxI32(hf.getNbRows()) - 1) return true;
		if (column > NxI32(hf.getNbColumns()) - 2) return true;
		NxU32 vertexIndex = hf.getNbColumns() * row + column;
		NxReal h0 = hf.getHeight(vertexIndex);
		NxReal h1 = hf.getHeight(vertexIndex + 1);
		NxReal h = heightScale * (h0 + z * (h1 - h0));
		return callback->contact(*this, 3 * vertexIndex, row, column, 0, y, z, h);
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class T>
	NX_INLINE bool HeightFieldShape::vintercept(NxI32 row, NxI32 column, NxReal x, NxReal y, T* callback) const
		{
		if (row < 0) return true;
		if (column < 0) return true;
		const HeightField& hf = getHeightFieldFast();
		if (row > NxI32(hf.getNbRows()) - 2) return true;
		if (column > NxI32(hf.getNbColumns()) - 1) return true;
		NxU32 vertexIndex = hf.getNbColumns() * row + column;
		NxReal h0 = hf.getHeight(vertexIndex);
		NxReal h1 = hf.getHeight(vertexIndex + hf.getNbColumns());
		NxReal h = heightScale * (h0 + x * (h1 - h0));
		return callback->contact(*this, 3 * vertexIndex + 2, row, column, x, y, 0, h);
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class T>
	NX_INLINE bool HeightFieldShape::dintercept0(NxI32 row, NxI32 column, NxReal x, NxReal z, NxReal y, T* callback) const
		{
		if (row < 0) return true;
		if (column < 0) return true;
		const HeightField& hf = getHeightFieldFast();
		if (row > NxI32(hf.getNbRows()) - 2) return true;
		if (column > NxI32(hf.getNbColumns()) - 2) return true;
		NxU32 vertexIndex = hf.getNbColumns() * row + column;
		if (heightField->isZerothVertexShared(vertexIndex)) return true;
		NxReal h0 = hf.getHeight(vertexIndex + 1);
		NxReal h1 = hf.getHeight(vertexIndex + hf.getNbColumns());
		NxReal h = heightScale * (h0 + x * (h1 - h0));
		return callback->contact(*this, 3 * vertexIndex + 1, row, column, x, y, z, h);
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class T>
	NX_INLINE bool HeightFieldShape::dintercept1(NxI32 row, NxI32 column, NxReal x, NxReal z, NxReal y, T* callback) const
		{
		if (row < 0) return true;
		if (column < 0) return true;
		const HeightField& hf = getHeightFieldFast();
		if (row > NxI32(hf.getNbRows()) - 2) return true;
		if (column > NxI32(hf.getNbColumns()) - 2) return true;
		NxU32 vertexIndex = hf.getNbColumns() * row + column;
		if (!heightField->isZerothVertexShared(vertexIndex)) return true;
		NxReal h0 = hf.getHeight(vertexIndex);
		NxReal h1 = hf.getHeight(vertexIndex + hf.getNbColumns() + 1);
		NxReal h = heightScale * (h0 + x * (h1 - h0));
		return callback->contact(*this, 3 * vertexIndex + 1, row, column, x, y, z, h);
		}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<class T>
	NX_INLINE void HeightFieldShape::traceLine(const NxVec3& p0, const NxVec3& p1, T* callback) const
		{
		NxReal u0 = p0.x * oneOverRowScale;
		NxReal v0 = p0.z * oneOverColumnScale;
		NxReal h0 = p0.y; // we don't scale y

		NxReal u1 = p1.x * oneOverRowScale;
		NxReal v1 = p1.z * oneOverColumnScale;
		NxReal h1 = p1.y; // we don't scale y

		/*
		// some simple last minute optimiations
		if ((u0 >= 0) && (u0 < heightField->getNbRows() - 1) 
		&&  (u1 >= 0) && (u1 < heightField->getNbRows() - 1)
		&&  (v0 >= 0) && (v0 < heightField->getNbColumns() - 1)
		&&  (v1 >= 0) && (v1 < heightField->getNbColumns() - 1)
		&&  (heightField->getTriangleIndex(u0, v0) == heightField->getTriangleIndex(u1, v1))) return;
		*/
		const NxReal root2 = 1.414213562373095f;
		const NxReal halfRoot2 = 0.5f * root2;

		NxReal du = u1 - u0;
		NxReal dv = v1 - v0;
		NxReal dh = h1 - h0;

		NxI32 i = (NxI32)floorf(u0);
		NxI32 j = (NxI32)floorf(v0);

		NxReal oodu = (NxMath::abs(du) > NX_EPS_REAL) ? NxReal(1) / NxMath::abs(du) : NX_MAX_REAL;
		NxReal oodv = (NxMath::abs(dv) > NX_EPS_REAL) ? NxReal(1) / NxMath::abs(dv) : NX_MAX_REAL;

		NxReal eu = u0 - NxReal(i);
		NxReal ev = v0 - NxReal(j);

		NxReal dp = (du + dv) / root2; // a dot product with vector (1,1)
		NxReal oodp = (NxMath::abs(dp) > NX_EPS_REAL) ? 1 / NxMath::abs(dp) : NX_MAX_REAL;
		NxReal ep = (NxMath::abs(du + dv) > NX_EPS_REAL) ? NxMath::abs(dp) * (1 - eu - ev) / (dv + du) : NX_MAX_REAL;

		NxReal dq = (dv - du) / root2; // a dot product with vector (-1,1)
		NxReal oodq = (NxMath::abs(dq) > NX_EPS_REAL) ? 1 / NxMath::abs(dq) : NX_MAX_REAL;
		NxReal eq = (NxMath::abs(dv - du) > NX_EPS_REAL) ? NxMath::abs(dq) * (eu - ev) / (dv - du) : NX_MAX_REAL;

		if (du > 0) eu = 1 - eu;
		if (dv > 0) ev = 1 - ev;

		if (ep < 0) ep += halfRoot2;
		if (eq < 0) eq += halfRoot2;

		NxReal t = 0;
		NxReal h = h0;

		while (t - 1 < NX_EPS_REAL) 
			{

			NxReal dtOverDu = (oodu < NX_MAX_REAL) ? eu*oodu : NX_MAX_REAL;
			NxReal dtOverDv = (oodv < NX_MAX_REAL) ? ev*oodv : NX_MAX_REAL;

			NxReal dtOverDp = (oodp < NX_MAX_REAL) ? ep*oodp : NX_MAX_REAL;
			NxReal dtOverDq = (oodq < NX_MAX_REAL) ? eq*oodq : NX_MAX_REAL;

			if (dtOverDu <= dtOverDv && dtOverDu <= dtOverDp && dtOverDu <= dtOverDq) 
				{
				t += dtOverDu;
				if (t > 1) return;
				h += dh*dtOverDu;
				ev -= NxMath::abs(dv)*dtOverDu;
				ep -= NxMath::abs(dp)*dtOverDu;
				eq -= NxMath::abs(dq)*dtOverDu;
				eu = 0;
				if (du > 0) 
					{
					if (!uintercept(++i, j, (dv > 0) ? 1 - ev : ev, h, callback)) return;
					}
				else
					{
					if (!uintercept(i--, j, (dv > 0) ? 1 - ev : ev, h, callback)) return;
					}
				} 
			else if (dtOverDv <= dtOverDu && dtOverDv <= dtOverDp && dtOverDv <= dtOverDq) 
				{
				t += dtOverDv;
				if (t > 1) return;
				h += dh*dtOverDv;
				eu -= NxMath::abs(du)*dtOverDv;
				ep -= NxMath::abs(dp)*dtOverDv;
				eq -= NxMath::abs(dq)*dtOverDv;
				ev = 0;
				if (dv > 0) 
					{
					if (!vintercept(i, ++j, (du > 0) ? 1 - eu : eu, h, callback)) return;
					}
				else
					{
					if (!vintercept(i, j--, (du > 0) ? 1 - eu : eu, h, callback)) return;
					}
				} 
			else if (dtOverDp <= dtOverDq) 
				{
				t += dtOverDp;
				if (t > 1) return;
				h += dh*dtOverDp;
				eu -= NxMath::abs(du)*dtOverDp;
				ev -= NxMath::abs(dv)*dtOverDp;
				eq -= NxMath::abs(dq)*dtOverDp;
				ep = 0;
				if (!dintercept0(i, j, (du > 0) ? 1 - eu : eu, (dv > 0) ? 1 - ev : ev, h, callback)) return;
				} 
			else 
				{
				t += dtOverDq;
				if (t > 1) return;
				h += dh*dtOverDq;
				eu -= NxMath::abs(du)*dtOverDq;
				ev -= NxMath::abs(dv)*dtOverDq;
				ep -= NxMath::abs(dp)*dtOverDq;
				eq = 0;
				if (!dintercept1(i, j, (du > 0) ? 1 - eu : eu, (dv > 0) ? 1 - ev : ev, h, callback)) return;
				}

			if (eu <= 0) eu = 1;
			if (ev <= 0) ev = 1;

			if (ep <= 0) ep = halfRoot2;
			if (eq <= 0) eq = halfRoot2;
			}
		}

#endif
