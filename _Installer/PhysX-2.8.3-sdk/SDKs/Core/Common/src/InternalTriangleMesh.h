#ifndef NX_COLLISION_INTERNALTRIANGLEMESH
#define NX_COLLISION_INTERNALTRIANGLEMESH
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "NxSimpleTriangleMesh.h"

#include "Allocateable.h"
#include "Opcode.h"

#include "px_trianglemesh.h"

//#define TEST_HW_TREES

//#define SUPPORT_CONVEX_VERTEX_FLAGS	// For HW algo
#define SUPPORT_CONVEX_EDGE_FLAGS
#define SUPPORT_CONVEX_PARTS
//#define SUPPORT_CONVEX_DESC
//#define SUPPORT_FLAT_DESC			// Argh, can't sort in both convex & flat order
#define SUPPORT_EXTRA_TRIG_DATA
#define SUPPORT_ADJACENCIES

#define USE_OPCODE_HYBRID_MODEL
#ifdef USE_OPCODE_HYBRID_MODEL
	typedef	Opcode::HybridModel				MeshModel;
	typedef Opcode::HybridPlanesCollider	PlanesQuery;
	typedef Opcode::HybridSphereCollider	SphereQuery;
	typedef Opcode::HybridOBBCollider		OBBQuery;
	typedef Opcode::HybridAABBCollider		AABBQuery;
	typedef Opcode::HybridLSSCollider		LSSQuery;
	typedef Opcode::HybridRayCollider		RayQuery;
#else
	typedef	Opcode::Model					MeshModel;
	typedef Opcode::PlanesCollider			PlanesQuery;
	typedef Opcode::SphereCollider			SphereQuery;
	typedef Opcode::OBBCollider				OBBQuery;
	typedef Opcode::AABBCollider			AABBQuery;
	typedef Opcode::LSSCollider				LSSQuery;
	typedef Opcode::RayCollider				RayQuery;
#endif

#ifdef SUPPORT_CONVEX_EDGE_FLAGS
//	class NxPhysics::EdgeList;
	#include "EdgeList.h"
#endif
#ifdef SUPPORT_ADJACENCIES
	class AdjacenciesBuilder;
#endif

#ifdef SUPPORT_CONVEX_PARTS
	#ifdef SUPPORT_CONVEX_DESC
	class ConvexPatchDesc : public Allocateable
		{
		public:
		NX_INLINE	ConvexPatchDesc() : offset(0), count(0)	{ localCenter.zero();	}
		NX_INLINE	~ConvexPatchDesc()						{						}

		NxVec3	localCenter;
		NxU32	offset;
		NxU32	count;
		};
	#endif

	#ifdef SUPPORT_FLAT_DESC
	class FlatPatchDesc
		{
		public:
		NX_INLINE	FlatPatchDesc() : offset(0), count(0)	{						}
		NX_INLINE	~FlatPatchDesc()						{						}

		NxU32	offset;
		NxU32	count;
		};
	#endif
#endif


	class InternalTriangleMesh
	{
	public:
										InternalTriangleMesh();
										~InternalTriangleMesh();

				void					release();

				NxPoint*				allocateVertices(NxU32 nbVertices);
				NxTriangle32*			allocateTriangles(NxU32 nbTriangles);
				NxMaterialIndex*		allocateMaterials();
				NxU32*					allocateFaceRemap();

	NX_INLINE	NxU32					getNumVertices()			const	{ return mData.mNumVertices;			}
	NX_INLINE	NxU32					getNumTriangles()			const	{ return mData.mNumTriangles;		}
	NX_INLINE	NxPoint*				getVertices()						{ return (NxPoint*)mData.mVertices;			}
	NX_INLINE	const NxPoint*			getVertices()				const	{ return (NxPoint*)mData.mVertices;	}
	NX_INLINE	const NxPoint&			getVertex(NxU32 i)			const	{ return (NxPoint&)mData.mVertices[i];			}
	NX_INLINE	NxTriangle32*			getTriangles()						{ return (NxTriangle32*)mData.mTriangles;			}
	NX_INLINE	const NxTriangle32*		getTriangles()				const	{ return (NxTriangle32*)mData.mTriangles;			}
	NX_INLINE	const NxTriangle32&		getTriangle(NxU32 i)		const	{ return (NxTriangle32&)mData.mTriangles[i];		}
	NX_INLINE	NxMaterialIndex*		getMaterials()						{ return materialIndices;	}
	NX_INLINE	const NxMaterialIndex*	getMaterials()				const	{ return materialIndices;	}
	NX_INLINE	NxU32*					getFaceRemap()						{ return faceRemap;			}
	NX_INLINE	const NxU32*			getFaceRemap()				const	{ return faceRemap;			}
	NX_INLINE	const NxVec3*			getLocalVertexNormals()		const
										{
										if(!mData.mLocalVertexNormals)	const_cast<InternalTriangleMesh*>(this)->createVertexNormals();
										return (NxVec3*)mData.mLocalVertexNormals;
										}
#ifdef SUPPORT_CONVEX_EDGE_FLAGS
	NX_INLINE	const NxPhysics::EdgeListBuilder*	getEdgeList()			const
										{
										if(!edgeList)	const_cast<InternalTriangleMesh*>(this)->createEdgeList();
										return edgeList;
										}
				void					releaseEdgeList();
#endif
#ifdef SUPPORT_CONVEX_VERTEX_FLAGS
	NX_INLINE	const bool*				getActiveVertsFlags()			const
										{
										if(!edgeList)	const_cast<InternalTriangleMesh*>(this)->createEdgeList();
										return activeVerts;
										}
#endif
#ifdef SUPPORT_ADJACENCIES
	NX_INLINE	const AdjacenciesBuilder*	getAdjacencies()			const
										{
										if(size_t(adjacencies)==1)	return NULL;
										if(!adjacencies)	const_cast<InternalTriangleMesh*>(this)->createAdjacencies();
										if(!adjacencies)	{ *(size_t*)&adjacencies = 1; return NULL;	}
										return adjacencies;
										}
#endif

	NX_INLINE	const MeshModel*		getOpcodeModel()			const	{ return mData.mOpcodeModel;			}

#ifdef SUPPORT_CONVEX_PARTS
	// Data for convex-vs-arbitrary-mesh
	NX_INLINE	const NxU16*			getConvexTags()				const	{ return mData.mConvexParts;			}
	NX_INLINE	const NxU8*				getFlatTags8()				const	{ return mData.mFlatParts8;			}
	NX_INLINE	const NxU16*			getFlatTags16()				const	{ return mData.mFlatParts16;			}
	NX_INLINE	NxU32					getNbConvexParts()			const	{ return mData.mNumConvexParts;			}
	NX_INLINE	NxU32					getNbFlatParts()			const	{ return mData.mNumFlatParts;			}
#ifdef SUPPORT_CONVEX_DESC
	NX_INLINE	const ConvexPatchDesc*	getConvexDesc()				const	{ return convexDesc;			}
#endif
#ifdef SUPPORT_FLAT_DESC
	NX_INLINE	const FlatPatchDesc*	getFlatDesc()				const	{ return flatDesc;				}
#endif
#endif
	NX_INLINE	NxReal					getConvexEdgeThreshold()	const	{ return convexEdgeThreshold;	}

#ifdef SUPPORT_EXTRA_TRIG_DATA
	NX_INLINE	NxU32					getTrigSharedEdgeFlags(NxU32 trigIndex) const;
	NX_INLINE	void					setTrigSharedEdgeFlag(NxU32 trigIndex, NxU32 edgeIndex);
#endif

				bool					computeNonPersistentData(bool convex);
				bool					loadOpcodeModel(const Stream& modelData);
#ifdef TEST_HW_TREES
				struct ConvexDesc
				{
					NxU32	offset, count;
				};
				void					testConvexTree();
				Opcode::AABBTree*		convexTree;
				ConvexDesc*				convexDesc;
				NxU32					nbHWParts;
				NxU16*					hwParts;
#endif
				NxU32					getUsedBytes()				const;


//#ifdef SUPPORT_EXTRA_TRIG_DATA
#if 0 
				//per-trig cache:
	struct ExtraTrigData : public NxFoundation::NxAllocateable
		{
		NxU8 flags;		//3 bits/triangle rounded up to 4 for fast lookup == nTrigs / 2 + 1
		/*
		low 3 bits (mask: 7) are the edge flags:
		b001 = 1 = ignore edge 0 = edge v0-->v1
		b010 = 2 = ignore edge 1 = edge v0-->v2
		b100 = 4 = ignore edge 2 = edge v1-->v2
		*/
		};
				ExtraTrigData*			extraTrigData;
#endif
			PxInternalTriangleMeshData	mData;
	protected:



				//NxU32					numVertices;
				//NxU32					numTriangles;

				//NxPoint*				vertices;
				//NxTriangle32*			triangles;

				//@@NOT SEPARATED
				NxMaterialIndex*		materialIndices;		//!< the size of the array is numTriangles.
				//@@NOT SEPARATED
				NxU32*					faceRemap;				//!< new faces to old faces mapping (after cleaning, etc). Usage: old = faceRemap[new]

	//mutable		NxVec3*					localVertexNormals;		//!< Vertex normals in mesh local space
#ifdef SUPPORT_ADJACENCIES
				//@@NOT SEPARATED
	mutable		AdjacenciesBuilder*		adjacencies;			//!< Adjacency graph
#endif
#ifdef SUPPORT_CONVEX_EDGE_FLAGS
	mutable		NxPhysics::EdgeListBuilder*	edgeList;
#endif
#ifdef SUPPORT_CONVEX_VERTEX_FLAGS
				bool*					activeVerts;
#endif

#ifdef SUPPORT_CONVEX_PARTS
public:	// only public for serial
				NxU32					internalFlags;			//!< Internal mesh flags
				// For convex-vs-mesh
				/*NxU32					nbConvexParts;			//!< Number of convex patches in the mesh
				NxU32					nbFlatParts;			//!< Number of flat polygons in the mesh
				NxU16*					convexParts;			//!< Array of convex IDs
				union
				{
					NxU8*				flatParts8;				//!< Array of flat IDs
					NxU16*				flatParts16;			//!< Array of flat IDs
				};*/
#ifdef SUPPORT_CONVEX_DESC
				ConvexPatchDesc*		convexDesc;				//!< Array of patch descriptors
#endif
#ifdef SUPPORT_FLAT_DESC
				FlatPatchDesc*			flatDesc;				//!< Array of patch descriptors
#endif
#endif
public:			NxReal					convexEdgeThreshold;


protected:
				//MeshModel*				opcodeModel;			//!< Collision model
				Opcode::MeshInterface	meshInterface;

	// Internal methods
				void					createVertexNormals();
#ifdef SUPPORT_CONVEX_EDGE_FLAGS
				void					createEdgeList();
#endif
#ifdef SUPPORT_ADJACENCIES
				void					createAdjacencies();
#endif
#ifdef CULLING_TWEAK
				void					computeExtensionValues();
public:			NxF32*					extensionValues;
#endif
				friend class InternalTriangleMeshBuilder;
	};

#ifdef SUPPORT_EXTRA_TRIG_DATA
NX_INLINE NxU32 InternalTriangleMesh::getTrigSharedEdgeFlags(NxU32 trigIndex) const
	{
	if (mData.mExtraTrigData)
		{
		/*
		nibble-packed:
		NxU32 byteIndex = trigIndex >> 1;
		NxU32 flagsShift = 4 * (trigIndex & 1);	//lo or hi nibble

		return (extraTrigData[byteIndex].flags >> flagsShift) & 7;	//low 3 bits only
		*/
		return (mData.mExtraTrigData[trigIndex]) & 7;	//low 3 bits only
		}
	else return 0;
	}
NX_INLINE void InternalTriangleMesh::setTrigSharedEdgeFlag(NxU32 trigIndex, NxU32 edgeIndex)
	{
	/*
	nibble-packed:
	NxU32 byteIndex = trigIndex >> 1;
	NxU32 flagsShift = 4 * (trigIndex & 1);	//lo or hi nibble

	extraTrigData[byteIndex].flags |=  ((1<<edgeIndex) << flagsShift);		//set new
	*/
	mData.mExtraTrigData[trigIndex] |= (1<<edgeIndex);

	}
#endif

#endif
