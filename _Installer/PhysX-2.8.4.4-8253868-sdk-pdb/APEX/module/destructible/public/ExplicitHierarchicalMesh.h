#ifndef EXPLICIT_HIERARCHICAL_MESH
#define EXPLICIT_HIERARCHICAL_MESH
/*
 * Copyright 2009-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
#include "NxApex.h"
#include "NxRenderMeshAsset.h"
#include "NxApexUtils.h"

class NxRay;

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
	NxExplicitVertexFormat

	This is used when authoring an NxVertexBuffer, to define which data channels exist.
 */
struct NxExplicitVertexFormat
{
	/** This value defines which vertex winding orders will be rendered.  See NxRenderCullMode. */
	physx::PxU32		mWinding;

	/** Whether or not the accompanying vertex data has defined static vertex positions. */
	bool				mHasStaticPositions;

	/** Whether or not the accompanying vertex data has defined static vertex normals. */
	bool				mHasStaticNormals;

	/** Whether or not the accompanying vertex data has defined static vertex tangents. */
	bool				mHasStaticTangents;

	/** Whether or not the accompanying vertex data has defined static vertex binormals. */
	bool				mHasStaticBinormals;

	/** Whether or not the accompanying vertex data has defined static vertex colors. */
	bool				mHasStaticColors;

	/** Whether or not to create separate render resource for a static bone index buffer. */
	bool				mHasStaticSeparateBoneBuffer;

	/** Whether or not the accompanying vertex data has defined dynamic vertex positions. */
	bool				mHasDynamicPositions;

	/** Whether or not the accompanying vertex data has defined dynamic vertex normals. */
	bool				mHasDynamicNormals;

	/** Whether or not the accompanying vertex data has defined dynamic vertex tangents. */
	bool				mHasDynamicTangents;

	/** Whether or not the accompanying vertex data has defined dynamic vertex binormals. */
	bool				mHasDynamicBinormals;

	/** Whether or not the accompanying vertex data has defined dynamic vertex colors. */
	bool				mHasDynamicColors;

	/** Whether or not to create separate render resource for a dynamic bone index buffer. */
	bool				mHasDynamicSeparateBoneBuffer;

	/** How many UV coordinate channels there are (per vertex) */
	physx::PxU32		mUVCount;

	/** How many bones may influence a vertex */
	physx::PxU32		mBonesPerVertex;

			/** Constructor, calls clear() to set formats to default settings */
			NxExplicitVertexFormat()
			{
				clear();
			}

			/**
				Set formats to default settings:

				mWinding = NxRenderCullMode::CLOCKWISE;
				mHasStaticPositions = false;
				mHasStaticNormals = false;
				mHasStaticTangents = false;
				mHasStaticBinormals = false;
				mHasStaticColors = false;
				mHasStaticSeparateBoneBuffer = false;
				mHasDynamicPositions = false;
				mHasDynamicNormals = false;
				mHasDynamicTangents = false;
				mHasDynamicBinormals = false;
				mHasDynamicColors = false;
				mHasDynamicSeparateBoneBuffer = false;
				mUVCount = 0;
				mBonesPerVertex = 0;
			*/
	void	clear()
			{
				mWinding = NxRenderCullMode::CLOCKWISE;
				mHasStaticPositions = false;
				mHasStaticNormals = false;
				mHasStaticTangents = false;
				mHasStaticBinormals = false;
				mHasStaticColors = false;
				mHasStaticSeparateBoneBuffer = false;
				mHasDynamicPositions = false;
				mHasDynamicNormals = false;
				mHasDynamicTangents = false;
				mHasDynamicBinormals = false;
				mHasDynamicColors = false;
				mHasDynamicSeparateBoneBuffer = false;
				mUVCount = 0;
				mBonesPerVertex = 0;
			}

			/**
				Equality operator.  All values are tested for equality except mBonesPerVertex.
			*/
	bool	operator == (const NxExplicitVertexFormat& data ) const
			{
				if( mWinding != data.mWinding )
				{
					return false;
				}
				if( mHasStaticPositions != data.mHasStaticPositions ||
					mHasStaticNormals != data.mHasStaticNormals ||
					mHasStaticTangents != data.mHasStaticTangents ||
					mHasStaticBinormals != data.mHasStaticBinormals ||
					mHasStaticColors != data.mHasStaticColors ||
					mHasStaticSeparateBoneBuffer != data.mHasStaticSeparateBoneBuffer )
				{
					return false;
				}
				if( mHasDynamicPositions != data.mHasDynamicPositions ||
					mHasDynamicNormals != data.mHasDynamicNormals ||
					mHasDynamicTangents != data.mHasDynamicTangents ||
					mHasDynamicBinormals != data.mHasDynamicBinormals ||
					mHasDynamicColors != data.mHasDynamicColors ||
					mHasDynamicSeparateBoneBuffer != data.mHasDynamicSeparateBoneBuffer )
				{
					return false;
				}
				if( mUVCount != data.mUVCount )
				{
					return false;
				}
				return true;
			}

			/**
				Returns the logical complement of the == operator.
			*/
	bool	operator != (const NxExplicitVertexFormat& data ) const
			{
				return !(*this == data);
			}

			/**
				Creates a render-ready NxVertexFormat corresponding to this structure's member values.
			*/
	void	copyToNxVertexFormat( NxVertexFormat* format ) const
			{
				format->reset();
				physx::PxU32 bi;
				if( mHasStaticPositions )
				{
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::POSITION ) );
					format->setBufferFormat( bi, NxRenderDataFormat::FLOAT3 );
					format->setBufferAccess( bi, mHasDynamicPositions ? NxRenderDataAccess::DYNAMIC :  NxRenderDataAccess::STATIC );
				}
				if( mHasStaticNormals )
				{
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::NORMAL ) );
					format->setBufferFormat( bi, NxRenderDataFormat::FLOAT3 );
					format->setBufferAccess( bi, mHasDynamicNormals ? NxRenderDataAccess::DYNAMIC :  NxRenderDataAccess::STATIC );
				}
				if( mHasStaticTangents )
				{
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::TANGENT ) );
					format->setBufferFormat( bi, NxRenderDataFormat::FLOAT3 );
					format->setBufferAccess( bi, mHasDynamicTangents ? NxRenderDataAccess::DYNAMIC :  NxRenderDataAccess::STATIC );
				}
				if( mHasStaticBinormals )
				{
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::BINORMAL ) );
					format->setBufferFormat( bi, NxRenderDataFormat::FLOAT3 );
					format->setBufferAccess( bi, mHasDynamicBinormals ? NxRenderDataAccess::DYNAMIC :  NxRenderDataAccess::STATIC );
				}
				if( mUVCount > 0 )
				{
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::TEXCOORD0 ) );
					format->setBufferFormat( bi, NxRenderDataFormat::FLOAT2 );
				}
				if( mUVCount > 1 )
				{
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::TEXCOORD1 ) );
					format->setBufferFormat( bi, NxRenderDataFormat::FLOAT2 );
				}
				if( mUVCount > 2 )
				{
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::TEXCOORD2 ) );
					format->setBufferFormat( bi, NxRenderDataFormat::FLOAT2 );
				}
				if( mUVCount > 3 )
				{
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::TEXCOORD3 ) );
					format->setBufferFormat( bi, NxRenderDataFormat::FLOAT2 );
				}
				switch( mBonesPerVertex )
				{
				case 1:
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::BONE_INDEX ) );
					format->setBufferFormat( bi, NxRenderDataFormat::USHORT1 );
					break;
				case 2:
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::BONE_INDEX ) );
					format->setBufferFormat( bi, NxRenderDataFormat::USHORT2 );
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::BONE_WEIGHT ) );
					format->setBufferFormat( bi, NxRenderDataFormat::FLOAT2 );
					break;
				case 3:
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::BONE_INDEX ) );
					format->setBufferFormat( bi, NxRenderDataFormat::USHORT3 );
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::BONE_WEIGHT ) );
					format->setBufferFormat( bi, NxRenderDataFormat::FLOAT3 );
					break;
				case 4:
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::BONE_INDEX ) );
					format->setBufferFormat( bi, NxRenderDataFormat::USHORT4 );
					bi = format->addBuffer( format->getSemanticName( NxRenderVertexSemantic::BONE_WEIGHT ) );
					format->setBufferFormat( bi, NxRenderDataFormat::FLOAT4 );
					break;
				}

				format->setHasSeparateBoneBuffer( mHasStaticSeparateBoneBuffer );
				format->setWinding( (NxRenderCullMode::Enum)mWinding );
			}
};


/**
	NxExplicitSubmeshData

	This is used when authoring an NxRenderMeshAsset.

	This is the per-submesh data: the material name, and vertex format.
 */
struct NxExplicitSubmeshData
{
	enum
	{
		MaterialNameBufferSize = 1024
	};

	char					mMaterialName[MaterialNameBufferSize];
	NxExplicitVertexFormat	mVertexFormat;

	bool	operator == (const NxExplicitSubmeshData& data ) const
			{
				return !strcmp( mMaterialName, data.mMaterialName ) && mVertexFormat == data.mVertexFormat;
			}

	bool	operator != (const NxExplicitSubmeshData& data ) const
			{
				return !(*this == data);
			}
};


/**
	MeshFlag

	Used to describe attributes of mesh parts (see IExplicitHierarchicalMesh::meshFlags()).
*/
struct MeshFlag
{
	enum Enum
	{
		IsCutoutFaceSplit =	(1<<0),
		IsCutoutLeftover =	(1<<1)
	};
};


/**
	IExplicitHierarchicalMesh

	An interface to a representation of a render mesh that is used for authoring.

	The "hierarchical" nature of this mesh is represtented by extra parent/child relations
	among the parts that will become the parts of an NxRenderMeshAsset.
 */
class IExplicitHierarchicalMesh
{
public:
	/** Used in the visualize method to determine what to draw. */
	enum Enum
	{
		/**
			Draws the convex hulls associated with the BSP regions marked "outside," if a BSP has
			been created for this object (see calculateMeshBSP()).
		*/
		VisualizeMeshBSPOutsideRegions	= (1<<0),

		/**
			Draws the convex hulls associated with the BSP regions marked "inside," if a BSP has
			been created for this object (see calculateMeshBSP()).
		*/
		VisualizeMeshBSPInsideRegions	= (1<<1),

		/**
			Draws the convex hull associated with a single BSP regions, if a BSP has
			been created for this object (see calculateMeshBSP()).  The region index must
			be passed into the visualize function if this flag is set.
		*/
		VisualizeMeshBSPSingleRegion	= (1<<8),

		/** Currently unused */
		VisualizeSliceBSPOutsideRegions	= (1<<16),

		/** Currently unused */
		VisualizeSliceBSPInsideRegions	= (1<<17),

		/** Currently unused */
		VisualizeSliceBSPSingleRegion	= (1<<24),

		VisualizeMeshBSPAllRegions		= VisualizeMeshBSPOutsideRegions | VisualizeMeshBSPInsideRegions
	};

	/**
		Used in the serialize and deserialize methods, to embed extra data.
		The user must implement this class to serialize and deserialize
		the enumerated data type given.
	*/
	class IEmbedding
	{
	public:
		enum DataType
		{
			MaterialLibrary
		};

		virtual void	serialize( physx::PxFileBuf& stream, IEmbedding::DataType type ) const = 0;
		virtual void	deserialize( physx::PxFileBuf& stream, IEmbedding::DataType type, physx::PxU32 version ) = 0;
	};


										/**
											"Resets" this object to its initial state, freeing all internal data.
											If keepRoot is true, then parts up to the root depth will not be cleared.
											(In this case, not all of the submesh data, etc. will be deleted.)
											The root depth is set when the IExplicitHierarchicalMesh is first created.
											Fracturing methods create pieces beyond the root depth.
										*/
	virtual void						clear( bool keepRoot = false ) = 0;

										/**
											The maximum child depth in the hierarchy.  Depth 0 is the base, depth 1 parts are children of depth 0, etc.
											If there are no parts, this function returns -1.
										*/
	virtual physx::PxI32				maxDepth() const = 0;

										/**
											The index of the submesh used for newly created "interior" triangles.
											These are generated when a mesh is fractured.
											If there is no interior submesh, this function returns -1.
										*/
	virtual physx::PxI32*				interiorSubmeshIndex() = 0;

										/**
											The number of parts in this mesh.
										*/
	virtual physx::PxU32				partCount() const = 0;

										/**
											The parent index of the part indexed by partIndex.
											Depth 0 parts have no parents, snd for those parts this function returns -1.
										*/
	virtual physx::PxI32*				parentIndex( physx::PxU32 partIndex ) = 0;

										/**
											The number of triangles in the part indexed by partIndex.
											This includes all submeshes.
										*/
	virtual physx::PxU32				meshTriangleCount( physx::PxU32 partIndex ) const = 0;

										/**
											A pointer into the array of NxExplicitRenderTriangles which form the mesh
											of the part indexed by partIndex.
										*/
	virtual NxExplicitRenderTriangle*	meshTriangles( physx::PxU32 partIndex ) = 0;

										/**
											The axis aligned bounding box of the triangles for the part index by partIndex.
										*/
	virtual physx::PxBounds3			meshBounds( physx::PxU32 partIndex ) const = 0;

										/**
											Flags describing attributes of the part indexed by partIndex.
											See MeshFlag.
										*/
	virtual physx::PxU32				meshFlags( physx::PxU32 partIndex ) const = 0;

										/**
											Build a convex hull that wraps the part indexed by partIndex, using
											the method given.  (See NxConvexHullMethod.)
										*/
	virtual void						buildCollisionHull( physx::PxU32 partIndex, NxConvexHullMethod::Enum method ) = 0;

										/**
											Collision hull property for the part indexed by partIndex.
											This is its axis aligned bounding box.
										*/
	virtual physx::PxBounds3			collisionHullBounds( physx::PxU32 partIndex ) const = 0;

										/**
											Collision hull property for the part indexed by partIndex.
											This is the number of vertices in the convex hull.
										*/
	virtual physx::PxU32				collisionHullVertexCount( physx::PxU32 partIndex ) const = 0;

										/**
											Collision hull property for the part indexed by partIndex.
											This is the vertex indexed by vertexIndex, which must in
											the range [0, collisionHullVertexCount(partIndex)-1].
										*/
	virtual physx::PxVec3				collisionHullVertex( physx::PxU32 partIndex, physx::PxU32 vertexIndex ) const = 0;

										/**
											Collision hull property for the part indexed by partIndex.
											This is the number of edges in the convex hull.
										*/
	virtual physx::PxU32				collisionHullEdgeCount( physx::PxU32 partIndex ) const = 0;

										/**
											Collision hull property for the part indexed by partIndex.
											This is an edge endpoint indexed by edgeIndex, which must in
											the range [0, collisionHullEdgeCount(partIndex)-1], and
											whichEndpoint, which must be 0 or 1.
										*/
	virtual physx::PxVec3				collisionHullEdgeEndpoint( physx::PxU32 partIndex, physx::PxU32 edgeIndex, physx::PxU32 whichEndpoint ) const = 0;

										/**
											Collision hull property for the part indexed by partIndex.
											This is the number of planes which bound the convex hull.
										*/
	virtual physx::PxU32				collisionHullPlaneCount( physx::PxU32 partIndex ) const = 0;

										/**
											Collision hull property for the part indexed by partIndex.
											This is the plane indexed by planeIndex, which must in
											the range [0, collisionHullPlaneCount(partIndex)-1].
										*/
	virtual NxPlane						collisionHullPlane( physx::PxU32 partIndex, physx::PxU32 planeIndex ) const = 0;

										/**
											Perform a ray cast against the convex hull of the part indexed by partIndex.

											in: this MUST be set to the minimum 'time' that you wish to have reported for intersection.
												you may consider this an origin offset for the ray.
												On exit, if the hull is intersected, this value will contain the time of intersection,
												or its original value, which ever is larger.

											out: this MUST be set to the maximum 'time' that you wish to have reported for intersection.
												you may consider this the endpoint of a line segment intersection.
												On exit, if the hull is intersected, this value will contain the time that the ray
												exits the hull, or its original value, which ever is smaller.
											
											worldRay: the ray to intersect with the convex hull.
											
											localToWorldRT: the rotation applied to the convex hull.
											
											scale: the scale applied to the convex hull.
											
											normal: if not NULL, *normal will contain the surface normal of the convex hull at the
												point of intersection (at the 'in' time).  If the point on the ray at the 'in' time lies
												within the volume of the convex hull, then *normal will be set to (0,0,0).
											
											Return value: returns true if the line segment described by the user's supplied 'in' and 'out'
											parameters along the worldRay intersects the convex hull, false otherwise.
										*/
	virtual bool						collisionHullRayCast( physx::PxU32 partIndex, physx::PxF32& in, physx::PxF32& out, const NxRay& worldRay,
															  const physx::PxMat44& localToWorldRT, const physx::PxVec3& scale, physx::PxVec3* normal = NULL ) const = 0;

										/**
											The number of submeshes.  The explicit mesh representation is just a list
											of NxExplicitRenderTriangles for each part, and each NxExplicitRenderTriangle has
											a submesh index.  These indices will lie in a contiguous range from 0 to submeshCount()-1.
										*/
	virtual physx::PxU32				submeshCount() const = 0;

										/**
											The submeshData indexded by submeshIndex.  See NxExplicitSubmeshData.
										*/
	virtual NxExplicitSubmeshData*		submeshData( physx::PxU32 submeshIndex ) = 0;

										/**
											Creates a submesh and adds it to the list of submeshes, and returns the index of
											the newly created submesh.
										*/
	virtual physx::PxU32				addSubmesh( const NxExplicitSubmeshData& submeshData ) = 0;

										/**
											If there is an interior submesh (see interiorSubmeshIndex()), then triangles belonging
											to that submesh will have materials applied to them with a using a coordinate frame.
											In the event that materials need to be re-applied (possibly at a different scale), it
											is convenient to store the material frames used.  This function returns the array of
											material frames.  The index extraDataIndex stored in each NxExplicitRenderTriangle
											references this array.
										*/
	virtual physx::PxU32				getMaterialFrameCount() const = 0;
	virtual physx::PxMat44&				getMaterialFrame( physx::PxU32 index ) = 0;
	virtual physx::PxMat44&				addMaterialFrame() = 0;

										/**
											Serialization.  The user must instantiate IEmbedding in order to successfully
											serialize any embedded data.
										*/
	virtual void						serialize( physx::PxFileBuf& stream, IEmbedding& embedding ) const = 0;
	virtual void						deserialize( physx::PxFileBuf& stream, IEmbedding& embedding ) = 0;

										/**
											Copies the input mesh in to this object.
										*/
	virtual void						set( const IExplicitHierarchicalMesh& mesh ) = 0;

										/**
											Creates an internal BSP representation of the mesh parts up to the root depth.
											This is used by authoring tools to perform CSG operations.  If the user instantiates
											IProgressListener, they may pass it in to report progress of this operation.
										*/
	virtual void						calculateMeshBSP( physx::PxU32 randomSeed, IProgressListener* progressListener = NULL ) = 0;

										/**
											Draws various components of this object to the debugRenderer, as
											defined by the flags (see the visualization Enum above).  Some
											of the flags require an index be passed in as well.
										*/
	virtual void						visualize( NxApexRenderDebug& debugRender, physx::PxU32 flags, physx::PxU32 index = 0 ) const = 0;

										/**
											Releases all memory associated with this object and deletes itself.
										*/
	virtual void						release() = 0;

protected:
										/**
											Constructor and destructor are not public
											Use createExplicitHierarchicalMesh() to instantiate an IExplicitHierarchicalMesh and
											IExplicitHierarchicalMesh::release() to destroy it.
										*/
										IExplicitHierarchicalMesh() {}
	virtual								~IExplicitHierarchicalMesh() {}

private:
										/** The assignment operator is disabled, use set() instead. */
	IExplicitHierarchicalMesh&			operator = ( const IExplicitHierarchicalMesh& ) { return *this; }
};


/** Instantiates an IExplicitHierarchicalMesh */
IExplicitHierarchicalMesh*	createExplicitHierarchicalMesh();


PX_POP_PACK

}} // end namespace physx::apex

#endif // #ifndef EXPLICIT_HIERARCHICAL_MESH
