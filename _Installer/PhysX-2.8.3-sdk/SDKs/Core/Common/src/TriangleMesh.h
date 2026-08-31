#ifndef NX_COLLISION_TRIANGLEMESH
#define NX_COLLISION_TRIANGLEMESH
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
#include "EdgeList.h"
#include "NvTriangleMesh.h"

class NxStream;
class NpTriangleMesh;
class NxTriangleMeshDesc;
class NxPMap;

//namespace NxCollision {

struct PageDesc
	{
	NxU32	length;
	NxU32	offset;
	};

enum InternalMeshFlag
	{
	IMF_COMPUTE_CONVEX	=	(1<<0),
	};

enum InternalMeshSerialFlag
	{
	IMSF_MATERIALS		=	(1<<0),
	IMSF_FACE_REMAP		=	(1<<1),
	IMSF_HARDWARE_MESH	=	(1<<2),
	IMSF_8BIT_INDICES	=	(1<<3),
	IMSF_16BIT_INDICES	=	(1<<4),
	};

// 1: support stackless collision trees for non-recursive collision queries
static NxU32 gMeshVersion = 1;

class PenetrationMap;

class TriangleMesh : public NvTriangleMesh, public NxFoundation::NxAllocateable
	{
	public:

	enum HeightFieldSignedInfDir
		{
		NEG_X = 0,
		NEG_Y = 1,
		NEG_Z = 2,
		POS_X = 8 + 0,
		POS_Y = 8 + 1,
		POS_Z = 8 + 2,
		SID_NOT_HEIGHTFIELD	= 0xff
		};

											TriangleMesh();
											~TriangleMesh();

		// NvTriangleMesh
		virtual	void						release(); 
		// MSS HACK_TRIMESH_RELEASE	
		virtual   void						releaseInternalMesh();
		virtual	bool						saveToDesc(NxTriangleMeshDesc&)				const;
		virtual	bool						load(const NxStream& stream);
		virtual	bool						loadPMap(const NxPMap&);
		virtual	bool						hasPMap()									const;
		virtual	NxU32						getPMapSize()								const;
		virtual	bool						getPMapData(NxPMap& pmap)					const;
		virtual	NxU32						getPMapDensity()							const;
		virtual	NxMaterialIndex				getTriangleMaterial(NxTriangleID triangleIndex)	const	{ return getTriangleMaterial(triangleIndex, 0xffff);	}
		virtual	NxU32						getSubmeshCount()							const;
		virtual	NxU32						getCount(NxSubmeshIndex, NxInternalArray)	const;
		virtual	NxInternalFormat			getFormat(NxSubmeshIndex, NxInternalArray)	const;
		virtual	const void*					getBase(NxSubmeshIndex, NxInternalArray)	const;
		virtual	NxU32						getStride(NxSubmeshIndex, NxInternalArray)	const;
		virtual	NpTriangleMesh*				getNpTriangleMesh()						{ return mNpMesh;			}
		virtual	void						setNpTriangleMesh(NpTriangleMesh* m)	{ mNpMesh = m;				}
		virtual	const MeshMassInfo*			getMassInformation()						const;
		virtual	NxU32						getNumVertices()							const	{ return mesh.getNumVertices();		} 
		virtual	NxU32						getNumTriangles()							const	{ return mesh.getNumTriangles();	}
		virtual	const NxTriangle32*			getTriangles()								const	{ return mesh.getTriangles();		}
		virtual	const NxPoint*				getVertices()								const	{ return mesh.getVertices();		}
		virtual NxU32						getPageCount()								const;
		virtual NxBounds3					getPageBBox(NxU32 pageIndex)				const;
		// ~NvTriangleMesh

				const MeshMassInfo*			getMassInfo()								const;
				void *						getPage(NxU32 pageIndex, NxU32 &length)		const;


	NX_INLINE	const NxU32*				getFaceRemap()						const	{ return mesh.getFaceRemap();			}
	NX_INLINE	const NxVec3*				getLocalVertexNormals()				const	{ return mesh.getLocalVertexNormals();	}

	NX_INLINE	NxMaterialIndex				getTriangleMaterial(NxTriangleID triangleIndex, NxMaterialIndex defaultMaterial)	const;		//returns default material if we don't have a per-trig override
	NX_INLINE	const NxTriangle32*			getInternalTris()					const	{ return mesh.getTriangles();		}
	NX_INLINE	const NxTriangle32&			getTriangleFast(NxU32 i)			const	{ return mesh.getTriangle(i);		}
	NX_INLINE	const NxPoint*				getInternalVerts()					const	{ return mesh.getVertices();		}
	NX_INLINE	const NxPoint&				getVertexFast(NxU32 i)				const	{ return mesh.getVertex(i);			}
	NX_INLINE	NxU32						getNumVerticesFast()				const	{ return mesh.getNumVertices();		} 
	NX_INLINE	NxU32						getNumTrianglesFast()				const	{ return mesh.getNumTriangles();	}
#ifdef SUPPORT_EXTRA_TRIG_DATA
	NX_INLINE	NxU32						getTrigSharedEdgeFlags(NxU32 trigIndex) const			{ return mesh.getTrigSharedEdgeFlags(trigIndex);	}
	NX_INLINE	void						setTrigSharedEdgeFlag(NxU32 trigIndex, NxU32 edgeIndex)	{ mesh.setTrigSharedEdgeFlag(trigIndex, edgeIndex);	}
#endif
	// Heightfield
	NX_INLINE	NxU32						getHeightFieldSignedInfDir()		const	{ return heightFieldSignedInfDir;	}
	NX_INLINE	void						getHeightFieldUpVector(NxVec3& up)	const	{ up.zero();	up[heightFieldSignedInfDir & 3] = ((heightFieldSignedInfDir & 8) == 8) ? -1.0f : 1.0f ;	}
	NX_INLINE	NxHeightFieldAxis			getHeightFieldAxis()				const	{ return heightFieldVerticalAxis;	}
	NX_INLINE	NxReal						getHeightFieldExtent()				const	{ return heightFieldVerticalExtent;	}
#ifdef SUPPORT_ADJACENCIES
	NX_INLINE	const AdjacenciesBuilder*	getAdjacencies()					const	{ return mesh.getAdjacencies();		}
#endif
	NX_INLINE	void						computeWorldTriangle(NxTriangle& worldTri, NxTriangleID triangleIndex, const NxMat34& worldMatrix) const;
	NX_INLINE	void						computeWorldTriangle(NxTriangle& worldTri, NxTriangleID triangleIndex, const NxMat33& worldMatrix) const;
	NX_INLINE	const PenetrationMap*		getPMap()							const	{ return pmap;						}
	NX_INLINE	NxReal						getGeomEpsilon()					const	{ return geomEpsilon;				}
	NX_INLINE	const NxBounds3&			getLocalBounds()					const	{ return localBounds;				}
	NX_INLINE	const NxSphere&				getLocalSphere()					const	{ return localSphere;				}

#ifdef SUPPORT_CONVEX_EDGE_FLAGS
	NX_INLINE	const NxPhysics::EdgeListBuilder*	getEdgeList()						const	{ return mesh.getEdgeList();		}
#endif

	NX_INLINE	const MeshModel*			getOpcodeModel()					const	{ return mesh.getOpcodeModel();		}

#ifdef SUPPORT_CONVEX_PARTS
	// Data for convex-vs-arbitrary-mesh
	NX_INLINE	const NxU16*				getConvexTags()						const	{ return mesh.getConvexTags();		}
	NX_INLINE	const NxU8*					getFlatTags8()						const	{ return mesh.getFlatTags8();		}
	NX_INLINE	const NxU16*				getFlatTags16()						const	{ return mesh.getFlatTags16();		}
	NX_INLINE	NxU32						getNbConvexParts()					const	{ return mesh.getNbConvexParts();	}
	NX_INLINE	NxU32						getNbFlatParts()					const	{ return mesh.getNbFlatParts();		}
#endif

				InternalTriangleMesh		mesh;
				// MSS HACK_TRIMESH_RELEASE
				bool								meshIsReleased;
				void								releaseMeshOnly();

	protected:
				NxBounds3					localBounds;			//!< Local bounds
				NxSphere					localSphere;			//!< Local bounding sphere
				NxReal						geomEpsilon;			//!< Used for ray-mesh tests
				NxU32						heightFieldSignedInfDir;
				NxHeightFieldAxis			heightFieldVerticalAxis;
				NxReal						heightFieldVerticalExtent;

				//
				PenetrationMap*				pmap;					// For mesh-mesh

				// Page data
				NxU32						pageCount;
				NxBounds3*					pageBounds;
				PageDesc*					pageDesc;
				NxU8*						pageData;
				NxU8*						originalPageData;
				// ~Page data

	mutable		MeshMassInfo				massInfo;		//this is a cache that is valid if the mass is nonnegative.
				//proxy
				NpTriangleMesh*				mNpMesh;

				void						releaseMemory();
				void						updateHeightfieldData();
				bool						loadPages(const NxStream& stream);
	};

NX_INLINE void TriangleMesh::computeWorldTriangle(NxTriangle& worldTri, NxTriangleID triangleIndex, const NxMat34& worldMatrix) const
	{
	const NxTriangle32& T = getTriangleFast(triangleIndex);
	worldMatrix.multiply(getVertexFast(T.v[0]), worldTri.verts[0]);
	worldMatrix.multiply(getVertexFast(T.v[1]), worldTri.verts[1]);
	worldMatrix.multiply(getVertexFast(T.v[2]), worldTri.verts[2]);
	}

NX_INLINE void TriangleMesh::computeWorldTriangle(NxTriangle& worldTri, NxTriangleID triangleIndex, const NxMat33& worldMatrix) const
	{
	const NxTriangle32& T = getTriangleFast(triangleIndex);
	worldMatrix.multiply(getVertexFast(T.v[0]), worldTri.verts[0]);
	worldMatrix.multiply(getVertexFast(T.v[1]), worldTri.verts[1]);
	worldMatrix.multiply(getVertexFast(T.v[2]), worldTri.verts[2]);
	}

NX_INLINE NxMaterialIndex TriangleMesh::getTriangleMaterial(NxTriangleID triangleIndex, NxMaterialIndex defaultMaterial)	const
	{
	return mesh.getMaterials() ? mesh.getMaterials()[triangleIndex] : defaultMaterial;
	}

//	}
#endif
