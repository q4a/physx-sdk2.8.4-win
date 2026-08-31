#ifndef PX_EDGELIST_H
#define PX_EDGELIST_H

#include "px_config.h"

/*!
NOTICE!

This is a data-code separated version of NxPhysics::EdgeList.

It is to be shared between high and low level code, so make sure both are recompiled
if any change is done here.
*/

// Flags
enum PxEdgeType
{
	PX_EDGE_UNDEFINED,

	PX_EDGE_BOUNDARY,		//!< Edge belongs to a single triangle
	PX_EDGE_INTERNAL,		//!< Edge belongs to exactly two triangles
	PX_EDGE_SINGULAR,		//!< Edge belongs to three or more triangles

	PX_EDGE_FORCE_DWORD	= 0x7fffffff
};

enum PxEdgeFlag
{
	PX_EDGE_ACTIVE	= (1<<0)
};


// Data

#pragma PXD_PUSH_PACK(1)

//! Basic edge-data
struct PxEdgeData
{
	//!First vertex reference
	PxU32			Ref0;
	//!Second vertex reference
	PxU32			Ref1;
};

//! Basic edge-data using 8-bit references
struct PxEdge8Data
{
	//!First vertex reference
	PxU8			Ref0;
	//!Second vertex reference
	PxU8			Ref1;
};


//! A count/offset pair = an edge descriptor
struct PxEdgeDescData
{
	PxU16			Flags;
	PxU16			Count;
	PxU32			Offset;
};


//! Edge<->triangle mapping
struct PxEdgeTriangleData
{
	PxU32			mLink[3];
};


struct PxEdgeListData
{
	// The edge list
	PxU32					mNbEdges;				//!< Number of edges in the list
	PxEdgeData*				mEdges;					//!< List of edges
	// Faces to edges
	PxU32					mNbFaces;				//!< Number of faces for which we have data
	PxEdgeTriangleData*		mEdgeFaces;				//!< Array of edge-triangles referencing mEdges
	// Edges to faces
	PxEdgeDescData*			mEdgeToTriangles;		//!< An EdgeDesc structure for each edge
	PxU32*					mFacesByEdges;			//!< A pool of face indices
};

#pragma PXD_POP_PACK

// Accessors

// PT: ??? what's the point of this class? Why not static C-style functions, or just put this in PxEdgeDescData.
class PxEdgeDescAC
{
public:
	PXD_FORCE_INLINE static void			Construct(PxEdgeDescData& data)
	{
		data.Flags = 0;
		data.Count = 0;
		data.Offset = 0;
	}

	PXD_FORCE_INLINE static bool			IsActive(const PxEdgeDescData& data)
	{
		return (data.Flags & PX_EDGE_ACTIVE)!=0;	// PT: performance warning! This function doesn't seem to be used anyway
	}

	PXD_FORCE_INLINE static PxEdgeType		GetType(const PxEdgeDescData& data)
	{
		if(data.Count >= 3) return PX_EDGE_SINGULAR;
		return PxEdgeType(data.Count);
	}
};


enum
{
	MSH_EDGE_LINK_MASK		= 0x0fffffff,
	MSH_ACTIVE_EDGE_MASK	= 0x80000000,
	MSH_ACTIVE_VERTEX_MASK	= 0x40000000,
	MSH_ACTIVE_USER_BIT0	= 0x20000000,
	MSH_ACTIVE_USER_BIT1	= 0x10000000
};

class PxEdgeTriangleAC
{
public:

	PXD_FORCE_INLINE static PxU32			GetEdge01(const PxEdgeTriangleData& data)
	{
		return data.mLink[0] & MSH_EDGE_LINK_MASK;
	}
	PXD_FORCE_INLINE static PxU32			GetEdge12(const PxEdgeTriangleData& data)
	{ 
		return data.mLink[1] & MSH_EDGE_LINK_MASK;
	}
	PXD_FORCE_INLINE static PxU32			GetEdge20(const PxEdgeTriangleData& data)
	{ 
		return data.mLink[2] & MSH_EDGE_LINK_MASK;
	}
	PXD_FORCE_INLINE static PxU32			GetEdge(const PxEdgeTriangleData& data, PxU32 i)
	{ 
		return data.mLink[i] & MSH_EDGE_LINK_MASK;
	}

	PXD_FORCE_INLINE static bool			HasActiveEdge01(const PxEdgeTriangleData& data)
	{ 
		return (data.mLink[0] & MSH_ACTIVE_EDGE_MASK)!=0;	// PT: performance warning!
	}
	PXD_FORCE_INLINE static bool			HasActiveEdge12(const PxEdgeTriangleData& data)
	{ 
		return (data.mLink[1] & MSH_ACTIVE_EDGE_MASK)!=0;	// PT: performance warning!
	}
	PXD_FORCE_INLINE static bool			HasActiveEdge20(const PxEdgeTriangleData& data)
	{ 
		return (data.mLink[2] & MSH_ACTIVE_EDGE_MASK)!=0;	// PT: performance warning!
	}
	PXD_FORCE_INLINE static bool			HasActiveEdge(const PxEdgeTriangleData& data, PxU32 i)
	{ 
		return (data.mLink[i] & MSH_ACTIVE_EDGE_MASK)!=0;	// PT: performance warning!
	}

	PXD_FORCE_INLINE static bool			HasActiveVertex0(const PxEdgeTriangleData& data)
	{
		return (data.mLink[0] & MSH_ACTIVE_VERTEX_MASK)!=0;	// PT: performance warning!
	}
	PXD_FORCE_INLINE static bool			HasActiveVertex1(const PxEdgeTriangleData& data)
	{ 
		return (data.mLink[1] & MSH_ACTIVE_VERTEX_MASK)!=0;	// PT: performance warning!
	}
	PXD_FORCE_INLINE static bool			HasActiveVertex2(const PxEdgeTriangleData& data)
	{ 
		return (data.mLink[2] & MSH_ACTIVE_VERTEX_MASK)!=0;	// PT: performance warning!
	}
	PXD_FORCE_INLINE static bool			HasActiveVertex(const PxEdgeTriangleData& data, PxU32 i)
	{ 
		return (data.mLink[i] & MSH_ACTIVE_VERTEX_MASK)!=0;	// PT: performance warning!
	}

	// 6 bits / triangle available here
	PXD_FORCE_INLINE static bool			HasActiveUserBit0(const PxEdgeTriangleData& data)
	{
		return (data.mLink[0] & MSH_ACTIVE_USER_BIT0)!=0;	// PT: performance warning!
	}
	PXD_FORCE_INLINE static bool			HasActiveUserBit1(const PxEdgeTriangleData& data)
	{ 
		return (data.mLink[1] & MSH_ACTIVE_USER_BIT0)!=0;	// PT: performance warning!
	}
	PXD_FORCE_INLINE static bool			HasActiveUserBit2(const PxEdgeTriangleData& data)
	{ 
		return (data.mLink[2] & MSH_ACTIVE_USER_BIT0)!=0;	// PT: performance warning!
	}
	PXD_FORCE_INLINE static bool			HasActiveUserBit3(const PxEdgeTriangleData& data)
	{ 
		return (data.mLink[0] & MSH_ACTIVE_USER_BIT1)!=0;	// PT: performance warning!
	}
	PXD_FORCE_INLINE static bool			HasActiveUserBit4(const PxEdgeTriangleData& data)
	{ 
		return (data.mLink[1] & MSH_ACTIVE_USER_BIT1)!=0;	// PT: performance warning!
	}
	PXD_FORCE_INLINE static bool			HasActiveUserBit5(const PxEdgeTriangleData& data)
	{ 
		return (data.mLink[2] & MSH_ACTIVE_USER_BIT1)!=0;	// PT: performance warning!
	}

	PXD_FORCE_INLINE static void			SetActiveUserBit0(PxEdgeTriangleData& data)
	{ 
		data.mLink[0] |= MSH_ACTIVE_USER_BIT0;
	}
	PXD_FORCE_INLINE static void			SetActiveUserBit1(PxEdgeTriangleData& data)
	{ 
		data.mLink[1] |= MSH_ACTIVE_USER_BIT0;
	}
	PXD_FORCE_INLINE static void			SetActiveUserBit2(PxEdgeTriangleData& data)
	{ 
		data.mLink[2] |= MSH_ACTIVE_USER_BIT0;
	}
	PXD_FORCE_INLINE static void			SetActiveUserBit3(PxEdgeTriangleData& data)
	{ 
		data.mLink[0] |= MSH_ACTIVE_USER_BIT1;
	}
	PXD_FORCE_INLINE static void			SetActiveUserBit4(PxEdgeTriangleData& data)
	{ 
		data.mLink[1] |= MSH_ACTIVE_USER_BIT1;
	}
	PXD_FORCE_INLINE static void			SetActiveUserBit5(PxEdgeTriangleData& data)
	{ 
		data.mLink[2] |= MSH_ACTIVE_USER_BIT1;
	}

	PXD_FORCE_INLINE static void			ClearActiveUserBit0(PxEdgeTriangleData& data)
	{ 
		data.mLink[0] &= ~MSH_ACTIVE_USER_BIT0;
	}
	PXD_FORCE_INLINE static void			ClearActiveUserBit1(PxEdgeTriangleData& data)
	{ 
		data.mLink[1] &= ~MSH_ACTIVE_USER_BIT0;
	}
	PXD_FORCE_INLINE static void			ClearActiveUserBit2(PxEdgeTriangleData& data)
	{
		data.mLink[2] &= ~MSH_ACTIVE_USER_BIT0;
	}
	PXD_FORCE_INLINE static void			ClearActiveUserBit3(PxEdgeTriangleData& data)
	{ 
		data.mLink[0] &= ~MSH_ACTIVE_USER_BIT1;
	}
	PXD_FORCE_INLINE static void			ClearActiveUserBit4(PxEdgeTriangleData& data)
	{ 
		data.mLink[1] &= ~MSH_ACTIVE_USER_BIT1;
	}
	PXD_FORCE_INLINE static void			ClearActiveUserBit5(PxEdgeTriangleData& data)
	{ 
		data.mLink[2] &= ~MSH_ACTIVE_USER_BIT1;
	}
};

#endif
