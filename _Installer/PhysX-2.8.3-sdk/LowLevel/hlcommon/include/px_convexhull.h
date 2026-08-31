#ifndef PX_CONVEXHULL_H
#define PX_CONVEXHULL_H

#include "px_config.h"
#include "limits.h"

enum PxConvexPolygonFlag
{
	PX_CPF_INTERNAL	= (1<<0),
};


struct PxEdge8Data;
struct PxEdgeDescData;

#pragma PXD_PUSH_PACK(1)

// Data definition
struct PxHullTriangleData
{
	PxU32			mRef[3];
};

// Unfortunately, this struct is used for serialization. 
// In order to make this work on 64 bit platforms we have to create a pointer independent struct.
// mVRef8, mERef16 were stored as converted indices anyway. 
struct PxHullPolygonData
{
	PxU16			mNbVerts;		//!< Number of vertices/edges in the polygon
	PxU16			mFlags;			//!< Various polygon flags (see ::ConvexPolygonFlag)
	const PxU8*		mVRef8;			//!< Vertex references
	const PxU16*	mERef16;		//!< Edge references
	PxFloat			mPlane[4];		//!< Plane equation for this polygon
	PxFloat			mMin, mMax;
};

#define PX_HULL_POLYGON_DATA_64_BIT_FIX 1

#if PX_HULL_POLYGON_DATA_64_BIT_FIX
struct PxHullPolygonSerializeData
{
	PxU16			mNbVerts;		//!< Number of vertices/edges in the polygon
	PxU16			mFlags;			//!< Various polygon flags (see ::ConvexPolygonFlag)
	PxU32			mVRef8;			//!< Vertex references
	PxU32			mERef16;		//!< Edge references
	PxFloat			mPlane[4];		//!< Plane equation for this polygon
	PxFloat			mMin, mMax;
};

#endif // PX_HULL_POLYGON_DATA_64_BIT_FIX

struct PxConvexHullData
{
	// Triangle data
	PxU32				mNbHullFaces;			//!< Number of faces in the convex hull
	PxHullTriangleData*	mFaces;					//!< Convex hull faces
	// Reduced hull vertices (else use original cloud)
	PxU32				mNbHullVertices;		//!< Number of vertices in the convex hull
	PxdVector*			mHullVertices;			//!< Convex hull vertices
	PxdVector*			mHullNormals;			//!< Convex hull normals
	PxdVector			mCenter;				//!< Precomputed hull center
	
	// Polygon data
	PxU32				mNbPolygons;			//!< Number of planar polygons composing the hull
	PxHullPolygonData*	mPolygons;				//!< Array of mNbPolygons structures
	PxU8*				mVertexData8;			//!< Vertex indices indexed by hull polygons
	PxU16*				mEdgeData16;			//!< Edge indices indexed by hull polygons
	
	// Edge data
	PxU32				mNbEdges;
	PxEdge8Data*		mEdges8;
	PxdVector*			mEdgeNormals;
	PxEdgeDescData*		mEdgeToTriangles;
	PxU8*				mFacesByEdges8;
};

#if PX_HULL_POLYGON_DATA_64_BIT_FIX

PXD_INLINE void PxConvert(PxHullPolygonSerializeData& dstHullPolyData, const PxHullPolygonData& srcHullPolyData, const PxConvexHullData& hullData)
{
#ifndef NX64
	ASSERT(sizeof(PxHullPolygonData) == sizeof(PxHullPolygonSerializeData));
#endif

	size_t edgeOffset = (size_t)srcHullPolyData.mERef16-(size_t)hullData.mEdgeData16;
	size_t vertexOffset = (size_t)srcHullPolyData.mVRef8-(size_t)hullData.mVertexData8;
	ASSERT(edgeOffset < UINT_MAX && vertexOffset < UINT_MAX);

	dstHullPolyData.mNbVerts = srcHullPolyData.mNbVerts;
	dstHullPolyData.mFlags = srcHullPolyData.mFlags;
	dstHullPolyData.mERef16 = static_cast<PxU32>(edgeOffset);
	dstHullPolyData.mVRef8 = static_cast<PxU32>(vertexOffset);
	dstHullPolyData.mPlane[0] = srcHullPolyData.mPlane[0];
	dstHullPolyData.mPlane[1] = srcHullPolyData.mPlane[1];
	dstHullPolyData.mPlane[2] = srcHullPolyData.mPlane[2];
	dstHullPolyData.mPlane[3] = srcHullPolyData.mPlane[3];
	dstHullPolyData.mMin = srcHullPolyData.mMin;
	dstHullPolyData.mMax = srcHullPolyData.mMax;
}

// This doesn't fixup pointers yet. The caller is supposed to do that.
PXD_INLINE void PxConvert(PxHullPolygonData& dstHullPolyData, const PxHullPolygonSerializeData& srcHullPolyData)
{
	PxU64 edgeRef = srcHullPolyData.mERef16;
	PxU64 vertexRef = srcHullPolyData.mVRef8;

	dstHullPolyData.mNbVerts = srcHullPolyData.mNbVerts;
	dstHullPolyData.mFlags = srcHullPolyData.mFlags;
	dstHullPolyData.mERef16 = reinterpret_cast<const PxU16*>(edgeRef);
	dstHullPolyData.mVRef8 = reinterpret_cast<const PxU8*>(vertexRef);

	dstHullPolyData.mPlane[0] = srcHullPolyData.mPlane[0];
	dstHullPolyData.mPlane[1] = srcHullPolyData.mPlane[1];
	dstHullPolyData.mPlane[2] = srcHullPolyData.mPlane[2];
	dstHullPolyData.mPlane[3] = srcHullPolyData.mPlane[3];
	dstHullPolyData.mMin = srcHullPolyData.mMin;
	dstHullPolyData.mMax = srcHullPolyData.mMax;
}
#endif // PX_HULL_POLYGON_DATA_64_BIT_FIX

#pragma PXD_POP_PACK

// Accessors
class PxHullTriangleAC
{
public:
	PXD_INLINE static void			Flip(PxHullTriangleData& data)
	{
		PxU32 tmp = data.mRef[2];
		data.mRef[2] = data.mRef[1];
		data.mRef[1] = tmp;
	}

};



#endif
