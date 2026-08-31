#ifndef NX_EDGELIST
#define NX_EDGELIST

/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Nx.h"
#include "Opcode.h"		// ### not really needed but allows me to compile this with 0 modifications!!

// Data/code shared with LL
#include "px_edgelist.h"

#define ICE_EDGELIST_V22

namespace NxPhysics
{
//	enum EdgeType
//	{
//		EDGE_UNDEFINED,
//
//		EDGE_BOUNDARY,		//!< Edge belongs to a single triangle
//		EDGE_INTERNAL,		//!< Edge belongs to exactly two triangles
//		EDGE_SINGULAR,		//!< Edge belongs to three or more triangles
//
//		EDGE_FORCE_DWORD	= 0x7fffffff
//	};
//
//	enum EdgeFlag
//	{
//		EDGE_ACTIVE	= (1<<0)
//	};
/*
	class Edge8 : public Allocateable
	{
		public:
		//! Constructor
		inline_				Edge8()		{}
		//! Destructor
		inline_				~Edge8()	{}

				ubyte		mRef0;		//!< First vertex reference
				ubyte		mRef1;		//!< Second vertex reference
	};
	ICE_COMPILE_TIME_ASSERT(sizeof(Edge8)==2);
*/

	//! The edge-list creation structure.
	struct EDGELISTCREATE
	{
		//! Constructor
								EDGELISTCREATE()	: NbFaces(0), DFaces(null), WFaces(null), FacesToEdges(false), EdgesToFaces(false)
#ifdef ICE_EDGELIST_V22
								,Verts(null),
								Epsilon(0.1f)
#endif
								{}
				
				udword			NbFaces;		//!< Number of faces in source topo
				const udword*	DFaces;			//!< List of faces (dwords) or null
				const uword*	WFaces;			//!< List of faces (words) or null

				bool			FacesToEdges;
				bool			EdgesToFaces;
#ifdef ICE_EDGELIST_V22
				const Point*	Verts;
				float			Epsilon;
#endif
	};

//	#define MSH_EDGE_LINK_MASK		0x3fffffff
//	#define MSH_ACTIVE_EDGE_MASK	0x80000000
//	#define MSH_ACTIVE_VERTEX_MASK	0x40000000
//	#define MSH_EDGE_LINK_MASK		0x0fffffff
//	#define MSH_ACTIVE_EDGE_MASK	0x80000000
//	#define MSH_ACTIVE_VERTEX_MASK	0x40000000
//	#define MSH_ACTIVE_USER_BIT0	0x20000000
//	#define MSH_ACTIVE_USER_BIT1	0x10000000


	class EdgeList : public Allocateable
	{
		public:
		// Constructor / destructor
										EdgeList();
										~EdgeList();

				bool					Load(const Stream& stream);
		// Data access
		inline_	udword					GetNbEdges()							const	{ return mData.mNbEdges;						}
		inline_	const PxEdgeData*		GetEdges()								const	{ return mData.mEdges;						}
		inline_	const PxEdgeData&		GetEdge(udword edge_index)				const	{ return mData.mEdges[edge_index];			}
		//
		inline_	udword					GetNbFaces()							const	{ return mData.mNbFaces;						}
		inline_	const PxEdgeTriangleData* GetEdgeTriangles()					const	{ return mData.mEdgeFaces;					}
		inline_	const PxEdgeTriangleData& GetEdgeTriangle(udword face_index)	const	{ return mData.mEdgeFaces[face_index];		}
		//
		inline_	const PxEdgeDescData*	GetEdgeToTriangles()					const	{ return mData.mEdgeToTriangles;				}
		inline_	const PxEdgeDescData&	GetEdgeToTriangles(udword edge_index)	const	{ return mData.mEdgeToTriangles[edge_index];	}
		inline_	const udword*			GetFacesByEdges()						const	{ return mData.mFacesByEdges;					}
		inline_	udword					GetFacesByEdges(udword face_index)		const	{ return mData.mFacesByEdges[face_index];		}

		inline_ PxEdgeListData*			GetEdgeListData()								{ return &mData; }

		protected:
				PxEdgeListData			mData;					//!< Pointer to edgelist data
//		// The edge list
//				udword					mNbEdges;				//!< Number of edges in the list
//				PxEdgeData*				mEdges;					//!< List of edges
//		// Faces to edges
//				udword					mNbFaces;				//!< Number of faces for which we have data
//				PxEdgeTriangleData*		mEdgeFaces;				//!< Array of edge-triangles referencing mEdges
//		// Edges to faces
//				PxEdgeDescData*			mEdgeToTriangles;		//!< An EdgeDesc structure for each edge
//				udword*					mFacesByEdges;			//!< A pool of face indices
		// Internal methods
/*				bool					CreateFacesToEdges(udword nb_faces, const udword* dfaces, const uword* wfaces);
				bool					CreateEdgesToFaces(udword nb_faces, const udword* dfaces, const uword* wfaces);
#ifdef ICE_EDGELIST_V22
				bool					ComputeActiveEdges(udword nb_faces, const udword* dfaces, const uword* wfaces, const Point* verts, float epsilon);
#endif*/
	};


	class EdgeListBuilder : public EdgeList
	{
		public:
										EdgeListBuilder();
										~EdgeListBuilder();

				bool					Init(const EDGELISTCREATE& create);
//				bool					Save(Stream& stream)					const;
		private:
		// Internal methods
				bool					CreateFacesToEdges(udword nb_faces, const udword* dfaces, const uword* wfaces);
				bool					CreateEdgesToFaces(udword nb_faces, const udword* dfaces, const uword* wfaces);
#ifdef ICE_EDGELIST_V22
				bool					ComputeActiveEdges(udword nb_faces, const udword* dfaces, const uword* wfaces, const Point* verts, float epsilon);
#endif
	};

#ifndef ICE_EDGELIST_V22
	class ActiveEdges
	{
		public:
		// Constructor / destructor
									ActiveEdges();
									~ActiveEdges();

				bool				Compute(const EdgeList& edges, const IndexedTriangle* faces, const Point* verts, float epsilon=0.001f);

		// Data access
		inline_	const bool*			GetActiveEdges()						const	{ return mActiveEdges;				}
		inline_	bool				GetActiveEdge(udword edge_index)		const	{ return mActiveEdges[edge_index];	}

		private:
				bool*				mActiveEdges;			//!< mNbEdges bools marking active edges
	};
#endif
	
}

#endif
