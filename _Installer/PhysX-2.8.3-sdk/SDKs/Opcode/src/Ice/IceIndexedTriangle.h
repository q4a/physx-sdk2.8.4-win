///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a handy indexed triangle class.
 *	\file		IceIndexedTriangle.h
 *	\author		Pierre Terdiman
 *	\date		January, 17, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEINDEXEDTRIANGLE_H
#define ICEINDEXEDTRIANGLE_H

	// Forward declarations
//	enum CubeIndex;

	// An indexed triangle class.
	class ICEMATHS_API IndexedTriangle : public Allocateable
	{
		public:
		//! Constructor
		inline_					IndexedTriangle()									{}
		//! Constructor
		inline_					IndexedTriangle(udword r0, udword r1, udword r2)	{ mRef[0]=r0; mRef[1]=r1; mRef[2]=r2; }
		//! Copy constructor
		inline_					IndexedTriangle(const IndexedTriangle& triangle)
								{
									mRef[0] = triangle.mRef[0];
									mRef[1] = triangle.mRef[1];
									mRef[2] = triangle.mRef[2];
								}
		//! Destructor
		inline_					~IndexedTriangle()									{}
		//! Vertex-references
				udword			mRef[3];

		// Methods
				void			Flip();
				float			Area(const Point* verts)											const;
				float			Perimeter(const Point* verts)										const;
				float			Compacity(const Point* verts)										const;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Computes the triangle normal.
 *	\param		verts	[in] the list of indexed vertices
 *	\param		normal	[out] the computed normal
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline_ void Normal(const Point* verts, Point& normal)	const
{
	if(!verts)	return;

	const Point& p0 = verts[mRef[0]];
	const Point& p1 = verts[mRef[1]];
	const Point& p2 = verts[mRef[2]];
	normal = ((p2-p1)^(p0-p1)).Normalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Computes the triangle denormalized normal.
 *	\param		verts	[in] the list of indexed vertices
 *	\param		normal	[out] the computed normal
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline_ void DenormalizedNormal(const Point* verts, Point& normal)	const
{
	if(!verts)	return;

	const Point& p0 = verts[mRef[0]];
	const Point& p1 = verts[mRef[1]];
	const Point& p2 = verts[mRef[2]];
	normal = ((p2-p1)^(p0-p1));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Computes the triangle center.
 *	\param		verts	[in] the list of indexed vertices
 *	\param		center	[out] the computed center
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline_ void Center(const Point* verts, Point& center)	const
{
	if(!verts)	return;

	const Point& p0 = verts[mRef[0]];
	const Point& p1 = verts[mRef[1]];
	const Point& p2 = verts[mRef[2]];
	center = (p0+p1+p2)*INV3;
}

				void			CenteredNormal(const Point* verts, Point& normal)					const;
				void			RandomPoint(const Point* verts, Point& random)						const;
				bool			IsVisible(const Point* verts, const Point& source)					const;
				bool			BackfaceCulling(const Point* verts, const Point& source)			const;
				float			ComputeOcclusionPotential(const Point* verts, const Point& view)	const;
				bool			ReplaceVertex(udword oldref, udword newref);
				bool			IsDegenerate()														const;
				bool			HasVertex(udword ref)												const;
				bool			HasVertex(udword ref, udword* index)								const;
				ubyte			FindEdge(udword vref0, udword vref1)								const;
				udword			OppositeVertex(udword vref0, udword vref1)							const;
		inline_	udword			OppositeVertex(ubyte edgenb)										const	{ return mRef[2-edgenb];	}
				void			GetVRefs(ubyte edgenb, udword& vref0, udword& vref1, udword& vref2)	const;
				float			MinEdgeLength(const Point* verts)									const;
				float			MaxEdgeLength(const Point* verts)									const;
				void			ComputePoint(const Point* verts, float u, float v, Point& pt, udword* nearvtx=null)	const;
				float			Angle(const IndexedTriangle& tri, const Point* verts)				const;
		inline_	Plane			PlaneEquation(const Point* verts)									const	{ return Plane(verts[mRef[0]], verts[mRef[1]], verts[mRef[2]]);	}
				bool			Equal(const IndexedTriangle& tri)									const;
//				CubeIndex		ComputeCubeIndex(const Point* verts)								const;
	};

	ICE_COMPILE_TIME_ASSERT(sizeof(IndexedTriangle)==12);

	class ICEMATHS_API IndexedTriangle16 : public Allocateable
	{
		public:
		//! Constructor
		inline_					IndexedTriangle16()									{}
		//! Constructor
		inline_					IndexedTriangle16(uword r0, uword r1, uword r2)		{ mRef[0]=r0; mRef[1]=r1; mRef[2]=r2; }
		//! Copy constructor
		inline_					IndexedTriangle16(const IndexedTriangle16& triangle)
								{
									mRef[0] = triangle.mRef[0];
									mRef[1] = triangle.mRef[1];
									mRef[2] = triangle.mRef[2];
								}
		//! Destructor
		inline_					~IndexedTriangle16()								{}
		//! Vertex-references
				uword			mRef[3];

				void			Flip();
				float			Area(const Point* verts)									const;
				void			Center(const Point* verts, Point& center)					const;
				void			DenormalizedNormal(const Point* verts, Point& normal)		const;
				bool			BackfaceCulling(const Point* verts, const Point& source)	const;
				bool			IsDegenerate()												const;
		inline_	Plane			PlaneEquation(const Point* verts)							const	{ return Plane(verts[mRef[0]], verts[mRef[1]], verts[mRef[2]]);	}
	};
	ICE_COMPILE_TIME_ASSERT(sizeof(IndexedTriangle16)==6);

#endif // ICEINDEXEDTRIANGLE_H
