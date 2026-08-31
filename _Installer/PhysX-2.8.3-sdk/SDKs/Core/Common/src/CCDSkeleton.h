#ifndef NX_PHYSICS_CCDSKELETON
#define NX_PHYSICS_CCDSKELETON
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "Pluecker.h"
#include "Allocateable.h"
#include "NvPhysicsSDK.h"	//for NvCCDSkeleton
#include "DebugRenderable.h"

class NxSimpleTriangleMesh;
/*
This is the input dataset to the CCD.  Its data is in world space (or in whatever space we will later perform the computations in)
Its worth it to precompute stuff like PluckerLines and triangle planes because each edge gets tested against n other edges.
Plus we run through the whole thing 2x, once for linear and once for the angular case.
*/
class CCDSkeleton
	{
	public:
	struct VertexInfo
		{
		NxVec3 vertex;
		NX_BOOL test;				//TODO: could just steal a bit from above but at least its 4-aligned this way.
		};

	struct EdgeInfo
		{
		NxU32 vertices[2];			//indexes into vertices[].  High bit of first vertex index is raised if this vertex is to be omitted from edge-edge tests (its only here because the trig references it!)
		PlueckerLine line;			//this accelerates trig raytests a bit though thats prolly not a good enough reason to have them.  Its also used for the static mesh's edge in edge-edge.
		NxVec3 normal;				
		NX_BOOL test;				//TODO: could just use a bit from vertices.
		};

	struct TriangleInfo
		{
		NxPlane plane;
		NxU32	signedEdgeIndices[3];		//indexes into edges[].		we need to use the sign bit of these indices to say if a unique edge needs to be used in the positive or negated sense in the trig.
		};

	NX_INLINE CCDSkeleton()
		{
		membufferStart = 0;
		membufferEnd = 0;
		triangles = 0;
		endTriangles = 0;
		edges = 0;
		endEdges = 0;
		vertices = 0;
		endVertices = 0;
		numEdgesToTest = 0;
		numVertsToTest = 0;
		}

	NX_INLINE ~CCDSkeleton()
		{
		NX_FREE(membufferStart);
		}
	//general:
	//void resetIterators() const;
	//writing:
	//allocation:
	NX_INLINE void allocMesh(NxU32 numTriangles, NxU32 numEdges, NxU32 numVertices)	//here its OK to overestimate the actual # submitted.
		{
		NxU32 trigSize = numTriangles * sizeof(TriangleInfo);
		NxU32 edgeSize = numEdges * sizeof(EdgeInfo);
		NxU32 neededBufferSize = trigSize + edgeSize + numVertices * sizeof(VertexInfo);
		NxU32 currBufferSize = (NxU32)(membufferEnd - membufferStart);
		if (currBufferSize < neededBufferSize)
			{
			NX_FREE(membufferStart);
			membufferStart = (NxU8 *)NX_ALLOC(neededBufferSize,CCD_BUFFER);
			membufferEnd = membufferStart + neededBufferSize;
			}

		endTriangles = triangles = (TriangleInfo *)membufferStart;
		endEdges = edges = (EdgeInfo *)(membufferStart + trigSize);
		endVertices = vertices = (VertexInfo *)(membufferStart + trigSize + edgeSize);
		resetTriangleIterator();
		resetEdgeIterator();
		resetVertexIterator();
		numEdgesToTest = 0;
		numVertsToTest = 0;
		}

	NX_INLINE void addTriangle(NxU32 ea, NxU32 eb, NxU32 ec)	//3 edge indices.
		{
		NX_ASSERT((NxU8 *)triangleIter < membufferEnd);
		triangleIter->signedEdgeIndices[0] = ea;
		triangleIter->signedEdgeIndices[1] = eb;
		triangleIter->signedEdgeIndices[2] = ec;
		//compute plane for triangle:
		NxU32 v0,v1,v2;
		v0 = getEdge(ea & ~(1 << 31))->vertices[(ea >> 31)];
		v1 = getEdge(eb & ~(1 << 31))->vertices[(eb >> 31)];
		v2 = getEdge(ec & ~(1 << 31))->vertices[(ec >> 31)];
		NX_ASSERT(v0 != v1);
		NX_ASSERT(v2 != v1);
		NX_ASSERT(v2 != v0);
		const NxVec3 & a = getVertex(v0)->vertex;
		const NxVec3 & b = getVertex(v1)->vertex; 
		const NxVec3 & c = getVertex(v2)->vertex; 
		triangleIter->plane.set(a,b,c);

		triangleIter++;
		endTriangles++;
		}

	NX_INLINE NxU32 addEdge(NxU32 a, NxU32 b, const NxVec3 & normal, NX_BOOL test)					//returns the edge index
		{
		NX_ASSERT((NxU8 *)edgeIter < membufferEnd);
		edgeIter->vertices[0] = a;
		edgeIter->vertices[1] = b;

		//compute data:
		const NxVec3 & va = getVertex(a)->vertex;
		const NxVec3 & vb = getVertex(b)->vertex; 
		edgeIter->line.fromLine(va,vb);
		edgeIter->normal = normal;
		edgeIter->test = test;
		edgeIter++;
		numEdgesToTest += test;
		endEdges++;
		return ((NxU32)(edgeIter - edges))-1;
		}

	NX_INLINE NxU32 addVertex(const NxVec3 & v, NX_BOOL test)					//returns the vertex index
		{
		NX_ASSERT((NxU8 *)vertexIter < membufferEnd);
		vertexIter->vertex = v;
		vertexIter->test = test;
		vertexIter++;
		numVertsToTest += test;
		endVertices++;
		return ((NxU32)(vertexIter - vertices))-1;
		}


	//testing:
	NX_INLINE NxU32 getNumEdgesToTest() const
		{
		return numEdgesToTest;
		}
	NX_INLINE NxU32 getNumVerticesToTest() const
		{
		return numVertsToTest;
		}
	
	NX_INLINE NxU32 getNumEdges() const
		{
		return ((NxU32)(endEdges - edges));
		}
	NX_INLINE NxU32 getNumVertices() const
		{
		return ((NxU32)(endVertices - vertices));
		}
	NX_INLINE NxU32 getNumTrigs() const
		{
		return ((NxU32)(endTriangles - triangles));
		}
	
	NX_INLINE NxU32 getNumEdgesIterated() const
		{
		return ((NxU32)(edgeIter - edges));
		}
	NX_INLINE NxU32 getNumVerticesIterated() const
		{
		return ((NxU32)(vertexIter - vertices));
		}
	NX_INLINE NxU32 getNumTrianglesIterated() const
		{
		return ((NxU32)(triangleIter - triangles));
		}

	NX_INLINE NxU32 getNumEdgesRemaining() const
		{
		return ((NxU32)(endEdges - edgeIter));
		}
	NX_INLINE NxU32 getNumVerticesRemaining() const
		{
		return ((NxU32)(endVertices - vertexIter));
		}
	NX_INLINE NxU32 getNumTrianglesRemaining() const
		{
		return ((NxU32)(endTriangles - triangleIter));
		}

	

	//iteration:
	NX_INLINE void resetTriangleIterator()
		{
		triangleIter = triangles;
		}
	NX_INLINE void resetEdgeIterator()
		{
		edgeIter = edges;
		}
	NX_INLINE void resetVertexIterator()
		{
		vertexIter = vertices;
		}


	NX_INLINE VertexInfo * getNextVertex()
		{
		VertexInfo * i = vertexIter;
		vertexIter++;
		return (i < endVertices) ? i : 0;
		}
	NX_INLINE EdgeInfo * getNextEdge()
		{
		EdgeInfo * i = edgeIter;
		edgeIter++;
		return (i < endEdges) ? i : 0;
		}
	NX_INLINE TriangleInfo * getNextTriangle()
		{
		TriangleInfo * i = triangleIter;
		triangleIter++;
		return (i < endTriangles) ? i : 0;
		}

	NX_INLINE EdgeInfo * getEdge(NxU32 index)
		{
		EdgeInfo * i = &edges[index];
		NX_ASSERT(i < endEdges);
		return i;
		}
	NX_INLINE VertexInfo * getVertex(NxU32 index)
		{
		VertexInfo * i = &vertices[index];
		NX_ASSERT(i < endVertices);
		return i;
		}
	void visualize(NxFoundation::DebugRenderable& dr);

private:
	//memory pool:
	NxU8 * membufferStart;
	NxU8 * membufferEnd;

	TriangleInfo * triangles;		//first, because we typically know how many we have so there is no overestimation gap between the trigs and whatever follows.
	TriangleInfo*  endTriangles;

	EdgeInfo * edges;
	EdgeInfo * endEdges;

	VertexInfo * vertices;
	VertexInfo * endVertices;

	NxU32 numEdgesToTest;
	NxU32 numVertsToTest;

	//iters point to the next field to be processed.
	TriangleInfo * triangleIter;
	EdgeInfo * edgeIter;
	VertexInfo * vertexIter;
	};


class CCDSkeleton2
	{
	public:
	struct VertexInfo
		{
		PlueckerLine motionLine;
		NxVec3 vertex;
		};

	struct EdgeInfo
		{
		enum Flags 
			{ 
			EDGETEST		= 0x0,
			NOEDGETEST		= 0x01000000,			//do not test this edge in edge-edge tests cause its flat / concave of something.
			NOTFLAGSMASK    = 0x00ffffff
			}; 
		PlueckerLine line;			
		NxVec3 extrudedLineCoeffs;	//the direction coeffs are the same for the extruded line as for the line, so they are not stored again.  These are only coeffs [2][4][5]
		private:
		NxU32 vertices[2];			//indexes into vertices[].  High bytes are used for flags.

		public:
		NX_INLINE NX_BOOL skipEdgeTest() const		{ return (vertices[0] & NOEDGETEST); }
		NX_INLINE NxU32	  getVertexIndex(NxU32 i) const { return (vertices[i] & NOTFLAGSMASK); }
		NX_INLINE void	  setVertexIndex0(NxU32 i, Flags f) { vertices[0] = (i | f); }
		NX_INLINE void	  setVertexIndex1(NxU32 i) { vertices[1] = i; }
		};

	struct TriangleInfo
		{
		NxPlane plane;
		NxU32	signedEdgeIndices[3];		//indexes into edges[].		we need to use the sign bit of these indices to say if a unique edge needs to be used in the positive or negated sense in the trig.
		};

	NX_INLINE CCDSkeleton2()
		{
		membufferStart = 0;
		membufferEnd = 0;
		triangles = 0;
		endTriangles = 0;
		edges = 0;
		endEdges = 0;
		vertices = 0;
		endVertices = 0;
		numEdgesToTest = 0;
		numVertsToTest = 0;
		}

	NX_INLINE ~CCDSkeleton2()
		{
		NX_FREE(membufferStart);
		}
	//general:
	//void resetIterators() const;
	//writing:
	//allocation:
	NX_INLINE void allocMesh(NxU32 numTriangles, NxU32 numEdges, NxU32 numVertices)	//here its OK to overestimate the actual # submitted.
		{
		NxU32 trigSize = numTriangles * sizeof(TriangleInfo);
		NxU32 edgeSize = numEdges * sizeof(EdgeInfo);
		NxU32 neededBufferSize = trigSize + edgeSize + numVertices * sizeof(VertexInfo);
		NxU32 currBufferSize = (NxU32)(membufferEnd - membufferStart);
		if (currBufferSize < neededBufferSize)
			{
			NX_FREE(membufferStart);
			membufferStart = (NxU8 *)NX_ALLOC(neededBufferSize,CCD_BUFFER);
			membufferEnd = membufferStart + neededBufferSize;
			}

		endTriangles = triangles = (TriangleInfo *)membufferStart;
		endEdges = edges = (EdgeInfo *)(membufferStart + trigSize);
		endVertices = vertices = (VertexInfo *)(membufferStart + trigSize + edgeSize);
		resetTriangleIterator();
		resetEdgeIterator();
		resetVertexIterator();
		numEdgesToTest = 0;
		numVertsToTest = 0;
		}

	NX_INLINE void addTriangle(NxU32 ea, NxU32 eb, NxU32 ec, const NxPlane & p)	//3 edge indices.
		{
		NX_ASSERT((NxU8 *)triangleIter < membufferEnd);
		triangleIter->signedEdgeIndices[0] = ea;
		triangleIter->signedEdgeIndices[1] = eb;
		triangleIter->signedEdgeIndices[2] = ec;
		/*
		//compute plane for triangle:
		NxU32 v0,v1,v2;
		v0 = getEdge(ea & ~(1 << 31))->vertices[(ea >> 31)];
		v1 = getEdge(eb & ~(1 << 31))->vertices[(eb >> 31)];
		v2 = getEdge(ec & ~(1 << 31))->vertices[(ec >> 31)];
		NX_ASSERT(v0 != v1);
		NX_ASSERT(v2 != v1);
		NX_ASSERT(v2 != v0);
		const NxVec3 & a = getVertex(v0)->vertex;
		const NxVec3 & b = getVertex(v1)->vertex; 
		const NxVec3 & c = getVertex(v2)->vertex; 
		triangleIter->plane.set(a,b,c);
		*/
		triangleIter->plane = p;

		triangleIter++;
		endTriangles++;
		}

	NX_INLINE NxU32 addEdge(NxU32 a, NxU32 b, const NxVec3 & offset, NX_BOOL test)					//returns the edge index
		{
		NX_ASSERT((NxU8 *)edgeIter < membufferEnd);
		edgeIter->setVertexIndex0(a, test ? EdgeInfo::EDGETEST : EdgeInfo::NOEDGETEST);
		edgeIter->setVertexIndex1(b);

		//compute data:
		const NxVec3 & va = getVertex(a)->vertex;
		const NxVec3 & vb = getVertex(b)->vertex; 
		edgeIter->line.fromLine(va,vb);

		//direct translation of plucker line:
		edgeIter->extrudedLineCoeffs[0] = IDPLUCK_0(edgeIter->line) + offset[0] * IDPLUCK_5(edgeIter->line) + IDPLUCK_2(edgeIter->line) * offset[1];
		edgeIter->extrudedLineCoeffs[1] = IDPLUCK_1(edgeIter->line) - offset[0] * IDPLUCK_4(edgeIter->line) + IDPLUCK_2(edgeIter->line) * offset[2];
		edgeIter->extrudedLineCoeffs[2] = IDPLUCK_3(edgeIter->line) - offset[1] * IDPLUCK_4(edgeIter->line) - IDPLUCK_5(edgeIter->line) * offset[2];
		edgeIter++;
		numEdgesToTest += test;
		endEdges++;
		return ((NxU32)(edgeIter - edges))-1;
		}

	NX_INLINE NxU32 addVertex(const NxVec3 & v, const NxVec3 & motionDir)					//returns the vertex index
		{
		NX_ASSERT((NxU8 *)vertexIter < membufferEnd);
		vertexIter->vertex = v;
		//vertexIter->test = test;
		vertexIter->motionLine.fromRay(v, motionDir);
		vertexIter++;
		numVertsToTest ++;//= test;
		endVertices++;
		return ((NxU32)(vertexIter - vertices))-1;
		}


	//testing:
	NX_INLINE NxU32 getNumEdgesToTest() const
		{
		return numEdgesToTest;
		}
	NX_INLINE NxU32 getNumVerticesToTest() const
		{
		return numVertsToTest;
		}
	NX_INLINE NxU32 getNumEdges() const
		{
		return ((NxU32)(endEdges - edges));
		}
	NX_INLINE NxU32 getNumVertices() const
		{
		return ((NxU32)(endVertices - vertices));
		}
	NX_INLINE NxU32 getNumEdgesRemaining() const
		{
		return ((NxU32)(edgeIter - edges));
		}
	NX_INLINE NxU32 getNumVerticesRemaining() const
		{
		return ((NxU32)(vertexIter - vertices));
		}
	NX_INLINE NxU32 getNumTrigs() const
		{
		return ((NxU32)(endTriangles - triangles));
		}

	//iteration:
	NX_INLINE void resetTriangleIterator()
		{
		triangleIter = triangles;
		}
	NX_INLINE void resetEdgeIterator()
		{
		edgeIter = edges;
		}
	NX_INLINE void resetVertexIterator()
		{
		vertexIter = vertices;
		}


	NX_INLINE VertexInfo * getNextVertex()
		{
		VertexInfo * i = vertexIter;
		vertexIter++;
		return (i < endVertices) ? i : 0;
		}
	NX_INLINE EdgeInfo * getNextEdge()
		{
		EdgeInfo * i = edgeIter;
		edgeIter++;
		return (i < endEdges) ? i : 0;
		}
	NX_INLINE TriangleInfo * getNextTriangle()
		{
		TriangleInfo * i = triangleIter;
		triangleIter++;
		return (i < endTriangles) ? i : 0;
		}

	NX_INLINE EdgeInfo * getEdge(NxU32 index)
		{
		EdgeInfo * i = &edges[index];
		NX_ASSERT(i < endEdges);
		return i;
		}
	NX_INLINE VertexInfo * getVertex(NxU32 index)
		{
		VertexInfo * i = &vertices[index];
		NX_ASSERT(i < endVertices);
		return i;
		}
	void visualize(NxFoundation::DebugRenderable& dr);
private:
	//memory pool:
	NxU8 * membufferStart;
	NxU8 * membufferEnd;

	TriangleInfo * triangles;		//first, because we typically know how many we have so there is no overestimation gap between the trigs and whatever follows.
	TriangleInfo*  endTriangles;

	EdgeInfo * edges;
	EdgeInfo * endEdges;

	VertexInfo * vertices;
	VertexInfo * endVertices;

	NxU32 numEdgesToTest;
	NxU32 numVertsToTest;

	//iters point to the next field to be processed.
	TriangleInfo * triangleIter;
	EdgeInfo * edgeIter;
	VertexInfo * vertexIter;
	};

/*
Input skeleton in the shape's space.  It has to be transformed into a CCDSkeleton (by transforming to world space and precomputing some stuff.)
Its format doesn't really matter.
*/
class SourceCCDSkeleton : public NvCCDSkeleton, public NxFoundation::NxAllocateable
	{
	public:
	virtual void release();		//destruction called from np class.
	virtual void			setNxCCDSkeleton(NxCCDSkeleton *s) { nxSkeleton = s;}
	virtual NxCCDSkeleton * getNxCCDSkeleton() { return nxSkeleton; }


	struct VertexInfo
		{
		NxVec3 vertex;
		};

	struct EdgeInfo
		{

		enum Flags
			{
			EDGETEST		= 0x0,
			NOEDGETEST		= 0x01000000,			//do not test this edge in edge-edge tests cause its flat / concave of something.
			NOTFLAGSMASK    = 0x00ffffff
			};
		NxVec3 normal;
		private:
		NxU32 vertices[2];			//indexes into vertices[].  High bytes are used for flags.

		public:
		NX_INLINE NX_BOOL skipEdgeTest() const		{ return (vertices[0] & NOEDGETEST); }
		NX_INLINE NxU32	  getVertexIndex(NxU32 i) const { return (vertices[i] & NOTFLAGSMASK); }
		NX_INLINE NxU32	  getVertexIndex0() const { return (vertices[0] & NOTFLAGSMASK); }
		NX_INLINE NxU32	  getVertexIndex1() const { return vertices[1]; }
		NX_INLINE void	  setVertexIndex0(NxU32 i, Flags f) { vertices[0] = (i | f); }
		NX_INLINE void	  setVertexIndex1(NxU32 i) { vertices[1] = i; }
		};

	struct TriangleInfo
		{
		NxPlane plane;
		NxU32	signedEdgeIndices[3];		//indexes into edges[].
		};

	NX_INLINE SourceCCDSkeleton()
		{
		membufferStart = 0;
		membufferEnd = 0;
		triangles = 0;
		endTriangles = 0;
		edges = 0;
		endEdges = 0;
		vertices = 0;
		endVertices = 0;

		nxSkeleton =  0;
		temporaryDataStart = 0;
		temporaryDataSize = 0;
		mIndices = 0;
		}

	~SourceCCDSkeleton()
		{
		clear();
		}

	NX_BOOL loadFromDesc(const NxSimpleTriangleMesh &);
	NxU32 save(void * destBuffer, NxU32 destBufferSize);	//destBufferSize must be greater or equal to getSize().  Returns number of bytes written.
	NxU32 load(const void * srcBuffer, NxU32 srcBufferSize);
	NxU32 getSize();		//returns size in bytes
	NxU32 saveToDesc(NxSimpleTriangleMesh &desc);
	NX_INLINE void getMemoryNeeds(NxU32 & nTrigs, NxU32 & nEdges, NxU32 & nVerts)
		{
		nTrigs = (NxU32)(endTriangles - triangles);
		nEdges = (NxU32)(endEdges - edges);
		nVerts = (NxU32)(endVertices - vertices);
		}

	void appendToCCDSkeleton(CCDSkeleton &, const NxMat34 & transform);	//reindexes.
	void appendToCCDSkeleton2(CCDSkeleton2 &, const NxMat34 & transform, const NxVec3 & sourceSpaceMotion, const NxVec3 & destSpaceMotion);	//reindexes.
	void clear();
	void visualize(NxFoundation::DebugRenderable& dr, const NxMat34 & transform);

	//memory pool:
	NxU8 * membufferStart;
	NxU8 * membufferEnd;

	TriangleInfo * triangles;		//first, because we typically know how many we have so there is no overestimation gap between the trigs and whatever follows.
	TriangleInfo*  endTriangles;

	EdgeInfo * edges;
	EdgeInfo * endEdges;

	VertexInfo * vertices;
	VertexInfo * endVertices;

	//iters point to the next field to be processed.
	//TriangleInfo * triangleIter;
	//EdgeInfo * edgeIter;
	//VertexInfo * vertexIter;

	//temporary data for the appendToCCDSkeleton* methods
	NxU8 *temporaryDataStart;
	NxU32 temporaryDataSize;

	NxU32 *mIndices; // used for serialization.  No memory allocated unless a serialization call is performed.

	NxCCDSkeleton * nxSkeleton;
	};


#endif // NX_PHYSICS_CCDSKELETON

