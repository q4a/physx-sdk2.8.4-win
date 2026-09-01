#include "NxApex.h"
#ifndef __NX_RENDERMESHASSET_H__
#define __NX_RENDERMESHASSET_H__
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

/*!
\file
\brief APEX RenderMesh Asset
*/

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

class NxRenderMeshActor;
class NxRenderMeshActorDesc;
class NxApexCustomBufferIterator;

//! \brief Name of NxRenderMesh authoring type namespace
#define NX_RENDER_MESH_AUTHORING_TYPE_NAME "ApexRenderMesh"

/**
\brief Stats for an NxRenderMeshAsset: memory usage, counts, etc.
*/
struct NxRenderMeshAssetStats
{
	physx::PxU32	totalBytes;				//!< Total byte size of mesh
	physx::PxU32	submeshCount;			//!< Number of submeshes
	physx::PxU32	partCount;				//!< Number of mesh parts
	physx::PxU32	vertexCount;			//!< Number of vertices
	physx::PxU32	indexCount;				//!< Size (count) of index buffer
	physx::PxU32	vertexBufferBytes;		//!< Byte size of vertex buffer
	physx::PxU32	indexBufferBytes;		//!< Byte size of index buffer
};

/**
\brief Instance buffer data mode.  DEPRECATED, to be removed by APEX 1.0
*/
struct NxRenderMeshAssetInstanceMode
{
	enum Enum
	{
		POSE_SCALE=0,
		POS_VEL_LIFE,

		NUM_MODES
	};
};


/**
\brief The full RGBA color of a vertex
*/
struct NxVertexColor
{
public:

	PX_INLINE					NxVertexColor()												{}
	PX_INLINE					NxVertexColor( const PxColorRGBA c )
								{
									const physx::PxF32 recip255 = 1/(PxReal)255;
									set( (physx::PxF32)c.r*recip255, (physx::PxF32)c.g*recip255, (physx::PxF32)c.b*recip255, (physx::PxF32)c.a*recip255 );
								}

	PX_INLINE	NxVertexColor&	operator = ( const NxVertexColor& c )						{ r = c.r; g = c.g; b = c.b; a = c.a; return *this; }

	/// \brief set the color as RGBA floats
	PX_INLINE	void			set( physx::PxF32 _r, physx::PxF32 _g, physx::PxF32 _b, physx::PxF32 _a	)			{ r = _r; g = _g; b = _b; a = _a; }

	/// \brief return the color as a 32bit integer
	PX_INLINE	PxColorRGBA		toColorRGBA() const
								{
									return PxColorRGBA( (physx::PxU8)(255*r+(physx::PxF32)0.5),
														(physx::PxU8)(255*g+(physx::PxF32)0.5),
														(physx::PxU8)(255*b+(physx::PxF32)0.5),
														(physx::PxU8)(255*a+(physx::PxF32)0.5) );
								}

	physx::PxF32 r;		//!< RED
	physx::PxF32 g;		//!< GREEN
	physx::PxF32 b;		//!< BLUE
	physx::PxF32 a;		//!< ALPHA
};


/**
\brief a simple u, v coordinate struct
*/
struct NxVertexUV
{
					NxVertexUV() {}
					NxVertexUV( physx::PxF32 _u, physx::PxF32 _v ) { set( _u, _v ); }
					NxVertexUV( const physx::PxF32 uv[] ) { set( uv ); }

	void			set( physx::PxF32 _u, physx::PxF32 _v )
					{
						u = _u;
						v = _v;
					}

	void			set( const physx::PxF32 uv[] )
					{
						u = uv[0];
						v = uv[1];
					}

	physx::PxF32&			operator [] ( int i )
					{
						PX_ASSERT( i >= 0 && i <= 1 );
						return (&u)[i];
					}

	const physx::PxF32&	operator [] ( int i ) const
					{
						PX_ASSERT( i >= 0 && i <= 1 );
						return (&u)[i];
					}

	physx::PxF32	u, v;
};


/**
\brief An inefficient vertex description used for authoring
*/
struct NxVertex
{
	physx::PxVec3	position;		//!< Vertex position
	physx::PxVec3	normal;			//!< Surface normal at this position
	physx::PxVec3	tangent;		//!< Surface tangent at this position
	physx::PxVec3	binormal;		//!< Surface binormal at this position
	NxVertexUV		uv[NxVertexFormat::MAX_UV_COUNT]; //!< Texture UV coordinates
	NxVertexColor	color;			//!< Color ar this position
	physx::PxU16	boneIndices[NxVertexFormat::MAX_BONE_PER_VERTEX_COUNT]; //!< Bones which are attached to this vertex
	physx::PxF32	boneWeights[NxVertexFormat::MAX_BONE_PER_VERTEX_COUNT]; //!< Per bone wieght, 0.0 if no bone

	NxVertex()
	{
		memset( this, 0, sizeof( NxVertex ) );
	}
};


/**
\brief A less inefficient triangle description used for authoring
*/
struct NxExplicitRenderTriangle
{
	NxVertex		vertices[3];		//!< The three verts that define the triangle
	physx::PxI32	submeshIndex;		//!< The submesh to which this triangle belongs
	physx::PxU32	smoothingMask;		//!< Smoothing mask
	physx::PxU32	extraDataIndex;		//!< Index of extra data
};


/**
\brief User-defined callback for creating render triangles
*/
class NxRenderMeshBuilder
{
public:
	virtual void createTriangles( NxExplicitRenderTriangle* triangleBuffer, const NxApexCustomBufferIterator& customBufferData, physx::PxU32 triangleCount, void* userData ) = 0;
};


/**
\brief Descriptor for creating a rendering mesh part
*/
struct NxRenderMeshPartData
{
	NxRenderMeshPartData() : triangleCount(0), userData(NULL) {}
	NxRenderMeshPartData( physx::PxU32 _triCount, void* _data ) : triangleCount(_triCount), userData(_data) {}

	physx::PxU32	triangleCount;	//!< Number of triangles in this mesh part
	void*			userData;		//!< User definable pointer to part data, passed back to createTriangles
};


/**
\brief Authoring interface for an NxRenderMeshAsset
*/
class NxRenderMeshAssetAuthoring : public NxApexAssetAuthoring
{
public:
	/** \brief Vertex buffer class used for mesh creation */
	typedef NxApexRenderBufferData<NxRenderVertexSemantic, NxRenderVertexSemantic::Enum> VertexBuffer;

	/** \brief How the geometry is stored.  Currently only supporting triangles. */
	struct Primitive
	{
		enum Enum
		{
			TRIANGLE_LIST,
	//		TRIANGLE_STRIP, // Not supported for now
	//		TRIANGLE_FAN,	// Not supported for now

			COUNT
		};
	};

	/** What kind of integer is used for indices. */
	struct IndexType
	{
		enum Enum
		{
			UINT,
			USHORT,

			COUNT
		};
	};

	/** Description of one submesh, corresponding to one material.  The vertex buffer format contains bone indices, so these do not need
	    to be described here.  The submesh's partitioning into parts is described here. */
	class SubmeshDesc
	{
	public:
		const char*				m_materialName;		/** Name of material associated with this geometry. */
		const VertexBuffer*		m_vertexBuffers;	/** Vertex buffers for this submesh.  One may pass in the same buffers for each submesh. */
		physx::PxU32			m_numVertexBuffers;	/** Number of vertex buffers in m_VertexBuffers array. */
		physx::PxU32			m_numVertices;		/** Number of vertices.  Each vertex buffer in m_VertexBuffers must have this many vertices. */
		Primitive::Enum			m_primitive;		/** How the geometry is represented.  See the Primitive enum. */
		IndexType::Enum			m_indexType;		/** Type of the indices used in m_VertexIndices.  See the IndexType enum. */
		const void*				m_vertexIndices;	/** Buffer of vertex indices, stored as described by primitive and indexSize.  If NULL, m_vertexIndices = {0,1,2,...} is implied. */
		physx::PxU32			m_numIndices;		/** Size (in indices) of m_VertexIndices. */
		physx::PxU32			m_firstVertex;		/** Vertex index offset. */
		const void*				m_partIndices;		/** If not NULL, an array (of m_IndexType-sized indices) into m_VertexIndices, at the start of each part. */
		physx::PxU32			m_numParts;			/** If m_PartIndices is not NULL, the number of parts. */
		NxRenderCullMode::Enum	m_cullMode;			/** Winding order of the submesh */

				/** Constructor sets default values. */
				SubmeshDesc() { memset( this, 0, sizeof( SubmeshDesc ) ); }

				/** Validity check, returns true if this descriptor contains valid fields. */
		bool	isValid() const
				{
					return	m_materialName != NULL &&
							m_vertexBuffers != NULL &&	// BRG - todo: check the vertex buffers for validity
							m_numVertexBuffers > 0 &&
							m_numVertices > 0 &&
							m_primitive >= (Primitive::Enum)0 && m_primitive < Primitive::COUNT &&
							m_indexType >= (IndexType::Enum)0 && m_indexType < IndexType::COUNT &&
							m_numIndices > 0 &&
							(m_partIndices == NULL || m_numParts > 0) &&
							(m_cullMode == NxRenderCullMode::CLOCKWISE || m_cullMode == NxRenderCullMode::COUNTER_CLOCKWISE || m_cullMode == NxRenderCullMode::NONE);
				}
	};

	/** Description of a mesh, used for authoring an NxApexRender mesh.  It contains a number of vertex buffers and submeshes. */
	class MeshDesc
	{
	public:
		const SubmeshDesc*	m_submeshes;	/** Array of descriptors for the submeshes in this mesh. */
		physx::PxU32		m_numSubmeshes;	/** The number of elements in m_submeshes. */

				/** Constructor sets default values. */
				MeshDesc() { memset( this, 0, sizeof( MeshDesc ) ); }

				/** Validity check, returns true if this descriptor contains valid fields. */
		bool	isValid() const
				{
					return	m_submeshes != NULL &&
							m_numSubmeshes > 0;
				}
	};


	/**
	\brief Mesh-building function.
	\param [in] meshDesc					contains the setup for all vertex buffers
	\param [in] createMappingInformation	A vertex buffer with remapping indices will be generated. The name of the buffer is VERTEX_ORIGINAL_INDEX
	*/
	virtual void			createRenderMesh( const MeshDesc& meshDesc, bool createMappingInformation ) = 0;


	/**
	Utility to reduce a vertex buffer of explicit vertices (NxVertex).
	The parameters 'map' and 'vertices' must point to arrays of size vertexCount.
	Upon return, the map array will be filled in with remapped vertex positions for a new vertex buffer.
	The return value is the number of vertices in the reduced buffer.
	Note: this function does NOT actually create the new vertex buffer.
	*/
	virtual physx::PxU32	createReductionMap( PxU32* map, const NxVertex* vertices, PxU32 vertexCount,
												const physx::PxVec3& positionTolerance, physx::PxF32 normalTolerance, physx::PxF32 UVTolerance ) = 0;


	/**
	If set, static data buffers will be deleted after they are used in createRenderResources.
	*/
	virtual void			deleteStaticBuffersAfterUse( bool set )	= 0;

	/**
	Old mesh-building interface follows (DEPRECATED, to be removed by beta release):
	*/

	/**
	\brief (DEPRECATED, to be removed by beta release) User-defined callback for creating mesh triangles.
	*/
	virtual void			setMeshBuilder( NxRenderMeshBuilder* meshBuilder ) = 0;

	/**
	\brief (DEPRECATED, to be removed by beta release) If true, will attempt to eliminate redundant vertices.  (Default = true.)
	*/
	virtual void			setReduce( bool reduce ) = 0;

	/**
	\brief (DEPRECATED, to be removed by beta release) Set scale position tolerance

	If reducing (see setReduce) and this is true, position tolerance in x, y and z will be positionTolerance
	(see setPositionTolerance) multiplied by the bounding box extents of the mesh part.  (Default = true.)
	*/
	virtual void			setScalePositionTolerance( bool scalePositionTolerance ) = 0;

	/**
	\brief (DEPRECATED, to be removed by beta release) Set position tolerance

	If reducing (see setReduce) vertices will not be merged if they are separated by more than this distance.
	*/
	virtual void			setPositionTolerance( physx::PxF32 positionTolerance ) = 0;

	/**
	\brief (DEPRECATED, to be removed by beta release) Set normal tolerance

	If reducing (see setReduce) vertices will not be merged if the difference between their normals exceeds this magnitude.
	*/
	virtual void			setNormalTolerance( physx::PxF32 normalTolerance ) = 0;

	/**
	\brief (DEPRECATED, to be removed by beta release) Set UV tolerance
	
	If reducing (see setReduce) vertices will not be merged if their u or v texture coordinates differ by more than this amount.
	*/
	virtual void			setUVTolerance( physx::PxF32 UVTolerance ) = 0;

	/**
	\brief (DEPRECATED, to be removed by beta release) Tells which corner of the texture is at (0,0)
	*/
	virtual void			setTextureUVOrigin( NxTextureUVOrigin::Enum origin ) = 0;

	/**
	\brief (DEPRECATED, to be removed by beta release) Adds one part descriptor, which will create one more part per submesh.
	*/
	virtual physx::PxU32	addPart( const NxRenderMeshPartData& ) = 0;

	/**
	\brief (DEPRECATED, to be removed by beta release) Adds one submesh with the material name given by the function argument.
	\return a pointer to the submesh's vertex format, which the user may modify.
	*/
	virtual NxVertexFormat*	addSubmesh( const char* materialName ) = 0;

	/**
	\brief (DEPRECATED, to be removed by beta release) Trigger the build process of the render mesh asset.
	
	This essentially replaces the now defunct descriptor based render mesh asset creation method.
	*/
	virtual void			buildRenderMesh( bool forceTangentChannel = true ) = 0;

	/* Public access to NxRenderMeshAsset get methods */

	/// \brief Return the number of submeshes
	virtual physx::PxU32			getSubmeshCount() const = 0;
	/// \brief Return the number of mesh parts
	virtual physx::PxU32			getPartCount() const = 0;
	/// \brief Return the name of a submesh
	virtual const char*				getMaterialName( physx::PxU32 submeshIndex ) const = 0;
	/// \brief Set the name of a submesh
	virtual void					setMaterialName( physx::PxU32 submeshIndex, const char *name ) = 0;
	/// \brief Set the winding order of a submesh
	virtual void					setWindingOrder(physx::PxU32 submeshIndex, NxRenderCullMode::Enum winding) = 0;
	/// \brief Return the winding order of a submesh
	virtual NxRenderCullMode::Enum	getWindingOrder(physx::PxU32 submeshIndex) const = 0;
	/// \brief Return a submesh
	virtual const NxRenderSubmesh&	getSubmesh( physx::PxU32 submeshIndex ) const = 0;
	/// \brief Return a mutable submesh
	virtual NxRenderSubmesh&		getSubmeshWritable( physx::PxU32 submeshIndex ) = 0;
	/// \brief Return the bounds of a mesh part
	virtual physx::PxBounds3		getBounds( physx::PxU32 partIndex = 0 ) const = 0;
	/// \brief Get the asset statistics
	virtual void					getStats( NxRenderMeshAssetStats& stats ) const = 0;
};


/**
\brief Rendering mesh (data) class.

To render a mesh asset, you must create an instance
*/
class NxRenderMeshAsset : public NxApexAsset
{
public:

	/**
	\brief Instance this asset, return the created NxRenderMeshActor.
	
	See NxRenderMeshActor
	*/
	virtual NxRenderMeshActor*		createActor( const NxRenderMeshActorDesc& desc ) = 0;

	/**
	\brief Releases an NxRenderMeshActor instanced by this asset.
	*/
	virtual void					releaseActor( NxRenderMeshActor& ) = 0;

	/**
	\brief Number of submeshes.
	
	Each part effectively has the same number of submeshes, even if some are empty.
	*/
	virtual physx::PxU32			getSubmeshCount() const = 0;

	/**
	\brief Number of parts.
	
	These act as separate meshes, but they share submesh data (like materials).
	*/
	virtual physx::PxU32			getPartCount() const = 0;

	/**
	\brief Returns an array of length submeshCount()
	*/
	virtual const char*				getMaterialName( physx::PxU32 submeshIndex ) const = 0;

	/**
	\brief Returns an submesh
	
	A submesh contains all the triangles in all parts with the same material
	(indexed by submeshIndex)
	*/
	virtual const NxRenderSubmesh&	getSubmesh( physx::PxU32 submeshIndex ) const = 0;

	/**
	\brief Returns the axis-aligned bounding box of the vertices for the given part.
	
	Valid range of partIndex is {0..partCount()-1}
	*/
	virtual physx::PxBounds3		getBounds( physx::PxU32 partIndex = 0 ) const = 0;

	/**
	\brief Returns stats (sizes, counts) for the asset.
	
	See NxRenderMeshAssetStats.
	*/
	virtual void					getStats( NxRenderMeshAssetStats& stats ) const = 0;
	
protected:
	virtual							~NxRenderMeshAsset() {}
};

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_RENDERMESHASSET_H__
