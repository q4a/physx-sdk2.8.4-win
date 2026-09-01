#include "NxApex.h"
#include "NxRenderMesh.h"
#ifndef __NX_APEXCUSTOMEBUFFERITERATOR_H__
#define __NX_APEXCUSTOMEBUFFERITERATOR_H__
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
\brief class NxApexCustomBufferIterator
*/

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
\brief This class is used to access specific elements in an untyped chunk of memory
*/
class NxApexCustomBufferIterator
{
public:
	/**
	\brief Returns the memory start of a specific vertex.
	
	All custom buffers are stored interleaved, so this is also the memory start of the first attribute of this vertex.
	*/
	virtual void*		getVertex(physx::PxU32 triangleIndex, physx::PxU32 vertexIndex) const = 0;

	/**
	\brief Returns the index of a certain custom buffer.
	
	\note This is constant throughout the existence of this class.
	*/
	virtual physx::PxI32		getAttributeIndex(const char* attributeName) const = 0;

	/**
	\brief Returns a pointer to a certain attribute of the specified vertex/triangle.

	\param [in] triangleIndex Which triangle
	\param [in] vertexIndex Which of the vertices of this triangle (must be either 0, 1 or 2)
	\param [in] attributeName The name of the attribute you wish the data for
	\param [out] outFormat The format of the attribute, reinterpret_cast the void pointer accordingly.
	*/
	virtual void*		getVertexAttribute(physx::PxU32 triangleIndex, physx::PxU32 vertexIndex, const char* attributeName, NxRenderDataFormat::Enum& outFormat) const = 0;

	/**
	\brief Returns a pointer to a certain attribute of the specified vertex/triangle.

	\note This is the faster method than the one above since it won't do any string comparisons

	\param [in] triangleIndex Which triangle
	\param [in] vertexIndex Which of the vertices of this triangle (must be either 0, 1 or 2)
	\param [in] attributeIndex The indexof the attribute you wish the data for (use NxApexCustomBufferIterator::getAttributeIndex to find the index to a certain attribute name
	\param [out] outFormat The format of the attribute, reinterpret_cast the void pointer accordingly.
	\param [out] outName The name associated with the attribute
	*/
	virtual void*		getVertexAttribute(physx::PxU32 triangleIndex, physx::PxU32 vertexIndex, physx::PxU32 attributeIndex, NxRenderDataFormat::Enum& outFormat, const char*& outName) const = 0;

protected:
						NxApexCustomBufferIterator() {}
	virtual				~NxApexCustomBufferIterator() {}
};

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_APEXCUSTOMEBUFFERITERATOR_H__
