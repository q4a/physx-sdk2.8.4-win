#include "NxApex.h"
#ifndef __NX_RENDERMESH_H__
#define __NX_RENDERMESH_H__
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
\brief classes NxRenderSubmesh, NxVertexBuffer, and NxMaterialNamingConvention enums
*/

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

class NxRenderMeshActor;
class NxApexContext;
struct NxVertexUV;


/**
\brief a vertex buffer that supports multiple formats
 */
class NxVertexBuffer
{
public:
	/**
	\brief Returns the number of vertices in the buffer
	*/
	virtual physx::PxU32			getVertexCount() const = 0;

	/**
	\brief Returns the data format.  See NxVertexFormat.
	*/
	virtual const NxVertexFormat&	getFormat() const = 0;
	virtual NxVertexFormat&			getFormatWritable() = 0;

	/**
	\brief Accessor for the data buffer indexed by bufferIndex. To get the buffer format, use getFormat().getBufferFormat( index ).
	If the data channel doesn't exist then this function returns NULL.
	*/
	virtual const void*				getBuffer( physx::PxU32 bufferIndex ) const = 0;

	/**
	\brief Like getBuffer(), but also returns the buffer's format.
	*/
	virtual const void*				getBufferAndFormat( NxRenderDataFormat::Enum& format, physx::PxU32 bufferIndex ) const = 0;
	virtual void*					getBufferAndFormatWritable( NxRenderDataFormat::Enum& format, physx::PxU32 bufferIndex ) = 0;

	/**
	\brief Accessor for data in a desired format from the buffer indexed by bufferIndex. If the channel does not exist, or if it is in
	a format for which there is not presently a converter to the the desired format dstBufferFormat, this function returns false.
	The dstBufferStride field must be at least the size of the dstBufferFormat data, or zero (in which case the stride is assumed to be
	the size of the dstBufferFormat data).  If neither of these conditions hold, this function returns false.
	Otherwise, dstBuffer is filled in with elementCount elements of the converted data, starting from startVertexIndex, withe the given stride.
	*/
	virtual bool					getBufferData( void* dstBuffer, physx::NxRenderDataFormat::Enum dstBufferFormat, physx::PxU32 dstBufferStride, physx::PxU32 bufferIndex,
												   physx::PxU32 startVertexIndex, physx::PxU32 elementCount ) const = 0;

protected:
	/* Do not allow class to be created directly */
									NxVertexBuffer() {}
};


/**
\brief a mesh that has only one material (or render state, in general)
 */
class NxRenderSubmesh
{
public:
	virtual							~NxRenderSubmesh() {}

	/**
		Returns the number of vertices associated with the indexed part.
	*/
	virtual physx::PxU32			getVertexCount( physx::PxU32 partIndex ) const = 0;

	/**
		Returns the submesh's index buffer (contains all parts' vertices)
	*/
	virtual const NxVertexBuffer&	getVertexBuffer() const = 0;
	virtual NxVertexBuffer&			getVertexBufferWritable() = 0;

	/**
		Vertices for a given part are contiguous within the vertex buffer.  This function
		returns the first vertex index for the indexed part.
	*/
	virtual physx::PxU32			getFirstVertexIndex( physx::PxU32 partIndex ) const = 0;

	/**
		Returns the number of indices in the part's index buffer.
	*/
	virtual physx::PxU32			getIndexCount( physx::PxU32 partIndex ) const = 0;

	/**
		Returns the index buffer associated with the indexed part.
	*/
	virtual const physx::PxU32*		getIndexBuffer( physx::PxU32 partIndex ) const = 0;

	/**
		Creates a smoothing groups list in the groups array given.
		Every triangle defined by this submesh's index buffer will be assigned a smoothing group index,
		from a contiguous set of indices starting with 0.  Any two triangles which share an edge with
		common vertex buffer indices, with the same winding, will have the same smoothing group index.
		It is assumed that the memory buffer pointed to by groupsArray holds at least
		(groupsArraySize-1)*groupsArrayByteStride + 4 bytes.
		Returns the number of elements written into the groupsArray buffer, which will be no more than
		groupsArraySize.  If the number of triangle in this submesh is less than groupsArraySize, then
		the number of triangles will be returned.
	*/
	virtual physx::PxU32			createTriangleSmoothingGroups( physx::PxU32* groupsArray, physx::PxU32 groupsArraySize, physx::PxU32 groupsArrayByteStride ) const = 0;

protected:
									NxRenderSubmesh() {}
};

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_RENDERMESH_H__
