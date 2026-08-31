#ifndef NX_COLLISION_CONVEXMESH
#define NX_COLLISION_CONVEXMESH
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/


#include "NxSimpleTriangleMesh.h"
#include "Allocateable.h"
#include "NxBounds3.h"
#include "NxSphere.h"
#include "NxTriangle.h"
#include "Opcode.h"
#include "NxMat34.h"
#include "InternalTriangleMesh.h"
#include "ConvexMeshInterface.h"
#include "NvConvexMesh.h"
#include "ConvexHull.h"

#include "px_config.h"
#include "px_convexmesh.h"

class NxStream;
class ClosestPolygonMap;
class SupportVertexMap;
#ifdef SUPPORT_INTERNAL_RADIUS
class RaycastMap;
#endif
class NpConvexMesh;
class NxConvexMeshDesc;

//namespace NxCollision {

#include "TriangleMesh.h"

#ifdef USE_HULL_TRIANGLES
	struct SurfaceInterface;
	#include "Valency.h"
	#include "HillClimbing.h"
	#include "CollisionHull.h"
#else
	class CollisionHull;
	class ConvexHull;
#endif

enum InternalConvexSerialFlag
	{
	ICSF_MATERIALS		=	(1<<0),
	ICSF_FACE_REMAP		=	(1<<1),
	ICSF_8BIT_INDICES	=	(1<<3),
	ICSF_16BIT_INDICES	=	(1<<4),
	};

// 0: includes raycast map
// 1: discarded raycast map
// 2: support map not always there
// 3: support stackless trees for non-recursive collision queries
static NxU32 gConvexVersion = 3;

class ConvexMesh : public NvConvexMesh, public NxFoundation::NxAllocateable
	{
	public:
											ConvexMesh();
											~ConvexMesh();
	// NvConvexMesh
	virtual		void						release(); 
	virtual		bool						saveToDesc(NxConvexMeshDesc&)				const;
	virtual		NxU32						getSubmeshCount()							const;
	virtual		NxU32						getCount(NxSubmeshIndex, NxInternalArray)	const;
	virtual		NxInternalFormat			getFormat(NxSubmeshIndex, NxInternalArray)	const;
	virtual		const void*					getBase(NxSubmeshIndex, NxInternalArray)	const;
	virtual		NxU32						getStride(NxSubmeshIndex, NxInternalArray)	const;
	virtual		bool						load(const NxStream& stream);
	virtual		MeshMassInfo				getMassInformation()						const;
	virtual		NpConvexMesh*				getNpConvexMesh()					{ return getNpConvexMeshFast();	}
	virtual		void						setNpConvexMesh(NpConvexMesh*p)		{ setNpConvexMeshFast(p);		}
	virtual		NxU32						getNumVertices()			const	{ return getNumVerticesFast();	}
	virtual		NxU32						getNumTriangles()			const	{ return getNumTrianglesFast();	}
#ifdef USE_HULL_TRIANGLES
	virtual		const NxTriangle32*			getInternalTris()			const	{ return (const NxTriangle32*)getInternalTrisFast();	}
#else
	virtual		const NxTriangle32*			getInternalTris()			const	{ return getInternalTrisFast();	}
#endif
	virtual		const NxPoint*				getInternalVerts()			const	{ return getInternalVertsFast();}
	//~ NvConvexMesh

				const MeshMassInfo*			getMassInfo()				const;

#ifndef USE_HULL_TRIANGLES
	NX_INLINE	const NxU32*				getFaceRemap()				const	{ return mesh.getFaceRemap();			}
#endif
#ifdef USE_HULL_TRIANGLES
	NX_INLINE	const NxVec3*				getLocalVertexNormals()		const	{ return (const NxVec3*)chull.GetVertexNormals();	}
	NX_INLINE	const PxHullTriangleData*	getInternalTrisFast()		const	{ return chull.GetFaces();				}
	NX_INLINE	const NxPoint*				getInternalVertsFast()		const	{ return (const NxPoint*)chull.GetVerts();	}

	NX_INLINE	const PxHullTriangleData&	getTriangle(NxU32 i)		const	{ return chull.GetFaces()[i];			}
	NX_INLINE	const NxPoint&				getVertex(NxU32 i)			const	{ return (const NxPoint&)chull.GetVerts()[i];	}
	NX_INLINE	NxU32						getNumVerticesFast()		const	{ return chull.GetNbVerts();			}
	NX_INLINE	NxU32						getNumTrianglesFast()		const	{ return chull.GetNbFaces();			}
#else
	NX_INLINE	const NxVec3*				getLocalVertexNormals()		const	{ return mesh.getLocalVertexNormals();	}
	NX_INLINE	const NxTriangle32*			getInternalTrisFast()		const	{ return mesh.getTriangles();			}
	NX_INLINE	const NxPoint*				getInternalVertsFast()		const	{ return mesh.getVertices();			}

	NX_INLINE	const NxTriangle32&			getTriangle(NxU32 i)		const	{ return mesh.getTriangles()[i];		}
	NX_INLINE	const NxPoint&				getVertex(NxU32 i)			const	{ return mesh.getVertices()[i];			}
	NX_INLINE	NxU32						getNumVerticesFast()		const	{ return mesh.getNumVertices();			}
	NX_INLINE	NxU32						getNumTrianglesFast()		const	{ return mesh.getNumTriangles();		}
#endif

#ifdef USE_HULL_TRIANGLES
//	NX_INLINE const NxPhysics::EdgeListBuilder* getEdgeList()			const	{ 	NX_ASSERT(0);return 0; }	//AM: accessor added for CCD, mandatory for now until we improve the data structure.
	NX_INLINE	const NxPhysics::EdgeListBuilder*	getEdgeList()		const
											{
											if(!edgeList)	const_cast<ConvexMesh*>(this)->createEdgeList();
											return edgeList;
											}
				void						releaseEdgeList();
#else
	NX_INLINE const NxPhysics::EdgeListBuilder* getEdgeList()			const	{ return mesh.getEdgeList(); }	//AM: accessor added for CCD, mandatory for now until we improve the data structure.
#endif
	// Convex hull
	NX_INLINE	CollisionHull*				getHull()							{ return &chull;					}
	NX_INLINE	const CollisionHull*		getHull()					const	{ return &chull;					}
//	NX_INLINE	const ClosestPolygonMap*	getClosestPolygonMap()		const	{ return closestPolygonMap;			}
	NX_INLINE	const SupportVertexMap*		getSupportVertexMap()		const	{ return supportVertexMap;			}
#ifdef SUPPORT_INTERNAL_RADIUS
	NX_INLINE	const RaycastMap*			getRaycastMap()				const	{ return raycastMap;				}
#endif
	NX_INLINE	NxReal						getGeomEpsilon()			const	{ return geomEpsilon;				}
	NX_INLINE	const NxBounds3&			getLocalBounds()			const	{ return localBounds;				}
	NX_INLINE	const NxSphere&				getLocalSphere()			const	{ return localSphere;				}

// PXD_SUPPORT_CONVEX_OPCODE_MODEL
	#ifdef USE_HULL_TRIANGLES
	NX_INLINE	const MeshModel*			getOpcodeModel()			const	{ return &mMeshData.mOpcodeModel;	}
	#else
	NX_INLINE	const MeshModel*			getOpcodeModel()			const	{ return mesh.getOpcodeModel();		}
	#endif
//~PXD_SUPPORT_CONVEX_OPCODE_MODEL

	NX_INLINE	NpConvexMesh*				getNpConvexMeshFast()				{ return mNpMesh;					}
	NX_INLINE	void						setNpConvexMeshFast(NpConvexMesh*p)	{ mNpMesh = p;						}
	NX_INLINE	bool						canRunOnHW() const					{ return getNumTrianglesFast() <= 32 && getNumVerticesFast() <= 32; }					
	NX_INLINE	PxConvexMeshData*			getConvexMeshData()					{ return &mMeshData;				}

	protected:
#ifdef USE_HULL_TRIANGLES
				void						createEdgeList();

				PxConvexMeshData			mMeshData;

	mutable		NxPhysics::EdgeListBuilder*	edgeList;				//Used for CCD
				
// PXD_SUPPORT_CONVEX_OPCODE_MODEL
				Opcode::MeshInterface		meshInterface;
				bool						loadOpcodeModel(const Stream& modelData);
//~PXD_SUPPORT_CONVEX_OPCODE_MODEL
#else
				InternalTriangleMesh		mesh;
#endif

				NxU32						internalFlags;			//!< Internal mesh flags
				NxBounds3					localBounds;			//!< Local bounds
				NxSphere					localSphere;			//!< Local bounding sphere
				NxReal						geomEpsilon;			//!< Used for ray-mesh tests
				CollisionHull				chull;
//				ClosestPolygonMap*			closestPolygonMap;
				SupportVertexMap*			supportVertexMap;
#ifdef SUPPORT_INTERNAL_RADIUS
				RaycastMap*					raycastMap;
#endif
	mutable		MeshMassInfo				massInfo;				//this is a cache that is valid if the mass is nonnegative.
				//proxy
				NpConvexMesh*				mNpMesh;
				void						releaseMemory();
				bool						computeNonPersistentData();
				void						createVertexNormals();
	};

#ifndef NX_COOKING
class ConvexMeshRuntime : public ConvexMesh, public ConvexMeshInterface
	{
	public:
											ConvexMeshRuntime();
											~ConvexMeshRuntime();

	// Convex interface
	virtual		const Point&				GetCenter()				const;
	virtual		udword						GetNbVerts()			const;
	virtual		const Point*				GetVerts()				const;
	virtual		udword						GetNbPolygons()			const;
	virtual		const PxHullPolygonData&	GetPolygon(udword i)	const;
	virtual		const PxHullPolygonData*	GetPolygons()			const;
	virtual		udword						GetNbEdges()			const;
#ifdef SUPPORT_CONVEX_EDGE_DATA
	virtual		const Vertices*				GetEdgeDirections()		const;
	virtual		const NxPhysics::Edge8*		GetEdges8()				const;
	virtual		const NxPhysics::EdgeDesc*	GetEdgeToTriangles()	const;
	virtual		const uword*				GetFacesByEdges16()		const;
#endif
	virtual		udword						SelectClosestPolygon(const Point& world_witness, const Matrix4x4* world=null)					const;
	virtual		udword						SelectClosestEdge(const Point& world_witness, const Matrix4x4* world=null, udword* code=null)	const;

	virtual		void						ProjectHull(NPhaseContext& context, float& min, float& max, const Point& dir, const Matrix4x4& world4x4/*, const SupportVertexMap* supportVertex*/) const;

	virtual		void						GetLocalBoundingSphere(NxSphere& sphere)	const;
	//~Convex interface
	};
#endif

//	}
#endif
