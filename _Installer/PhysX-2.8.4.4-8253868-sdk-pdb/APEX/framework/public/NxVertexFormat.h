#include "NxApex.h"
#ifndef __NX_VERTEXFORMAT_H__
#define __NX_VERTEXFORMAT_H__
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
\brief class NxVertexFormat and struct NxVertexFormatFlag
*/


namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
\brief Color stored in 32 bits
*/
struct PxColorRGBA
{
	PxColorRGBA( PxU8 _r, PxU8 _g, PxU8 _b, PxU8 _a ) : r(_r), g(_g), b(_b), a(_a) {}

	PxU8	r, g, b, a;
};


struct NxRenderDataAccess
{
	enum Enum
	{
		STATIC = 0,
		DYNAMIC,
		STREAMING,

		ACCESS_TYPE_COUNT
	};
};

/**
\brief Describes the format of an NxVertexBuffer.
*/
class NxVertexFormat
{
public:

	enum
	{
		MAX_UV_COUNT = 4,
		MAX_BONE_PER_VERTEX_COUNT = 4,
	};

	typedef physx::PxU32 BufferID;


	/** \brief Resets the format to the initial state */
	virtual void							reset() = 0;


	/** \brief Sets the winding (cull mode) for this format */
	virtual void							setWinding( NxRenderCullMode::Enum winding ) = 0;

	/** \brief Sets whether or not a separate bone buffer is used */
	virtual void							setHasSeparateBoneBuffer( bool hasSeparateBoneBuffer ) = 0;

	/** \brief Accessor to read winding (cull mode) */
	virtual NxRenderCullMode::Enum			getWinding() const = 0;

	/** \brief Accessor to read if a seperate vertex buffer for bone indices and weights is generated */
	virtual bool							hasSeparateBoneBuffer() const = 0;


	/** \brief Returns a buffer name for a semantic. Returns NULL if the semantic is invalid */
	virtual const char*						getSemanticName( NxRenderVertexSemantic::Enum semantic ) const = 0;

	/** \brief Returns a buffer ID for a semantic. For custom buffers, use the getID() function. */
	virtual BufferID						getSemanticID( NxRenderVertexSemantic::Enum semantic ) const = 0;

	/** \brief Returns a buffer ID for a named buffer. For standard semantics, the getSemanticID( semantic ) function is faster, but
	is equivalent to getID( getSemanticName( semantic ) ). Returns 0 if name == NULL */
	virtual BufferID						getID( const char* name ) const = 0;


	/** \brief Adds a vertex buffer channel to this format
	\param [in] name the name of a new buffer (use getSemanticName for standard semantics)
	\return The buffer index. If the buffer for the semantic already exists, the index of the existing buffer is returned. Returns -1 if there is an error (e.g. name == NULL).
	*/
	virtual physx::PxI32					addBuffer( const char* name ) = 0;

	/** \brief Removes a buffer
	\param [in] index the buffer to remove
	\return True if successful, false otherwise (if the buffer index was invalid)
	*/
	virtual bool							bufferReplaceWithLast( physx::PxU32 index ) = 0;


	/** \brief Set the format for a semantic
	\return True if successful, false otherwise (if the buffer index was invalid)
	*/
	virtual bool							setBufferFormat( physx::PxU32 index, NxRenderDataFormat::Enum format ) = 0;

	/** \brief Set the access type for a buffer (static, dynamic, etc.)
	\return True if successful, false otherwise (if the buffer index was invalid)
	*/
	virtual bool							setBufferAccess( physx::PxU32 index, NxRenderDataAccess::Enum access ) = 0;

	/** \brief Set whether or not the buffer should be serialized
	\return True if successful, false otherwise (if the buffer index was invalid)
	*/
	virtual bool							setBufferSerialize( physx::PxU32 index, bool serialize ) = 0;


	/** \brief Accessor to read the name of a given buffer
	\return The buffer name if successful, NULL otherwise.
	*/
	virtual const char*						getBufferName( physx::PxU32 index ) const = 0;

	/** \brief Accessor to read the semantic of a given buffer
	\return The buffer semantic if successful, NxRenderVertexSemantic::NUM_SEMANTICS otherwise.
	*/
	virtual NxRenderVertexSemantic::Enum	getBufferSemantic( physx::PxU32 index ) const = 0;

	/** \brief Accessor to read the ID of a given buffer
	\return The buffer semantic if successful, 0 otherwise.
	*/
	virtual BufferID						getBufferID( physx::PxU32 index ) const = 0;

	/** \brief Get the format for a buffer
	\return The buffer format if successful, NxRenderDataFormat::UNSPECIFIED otherwise.
	*/
	virtual NxRenderDataFormat::Enum		getBufferFormat( physx::PxU32 index ) const = 0;

	/** \brief Get the access type for a buffer (static, dynamic, etc.)
	\return The buffer access if successful, NxRenderDataAccess::ACCESS_TYPE_COUNT otherwise.
	*/
	virtual NxRenderDataAccess::Enum		getBufferAccess( physx::PxU32 index ) const = 0;

	/** \brief Get whether or not the buffer should be serialized
	\return Whether or not the buffer should be serialized if successful, false otherwise.
	*/
	virtual bool							getBufferSerialize( physx::PxU32 index ) const = 0;


	/** \brief Accessor to read the number of buffers */
	virtual physx::PxU32					getBufferCount() const = 0;

	/** \brief Returns the number of buffers that are user-specified */
	virtual physx::PxU32					getCustomBufferCount() const = 0;

	/** \brief Accessor to get the buffer index
	If the buffer is not found, -1 is returned
	*/
	virtual physx::PxI32					getBufferIndexFromID( BufferID id ) const = 0;


	/** \brief Serialize the vertex format to a stream */
	virtual physx::PxFileBuf&				serialize( physx::PxFileBuf& stream ) const = 0;

	/** \brief Deserialize the vertex format from a stream */
	virtual physx::PxFileBuf&				deserialize( physx::PxFileBuf& stream, physx::PxU32 headerVersion ) = 0;

protected:
											NxVertexFormat()	{}
	virtual									~NxVertexFormat()	{}
};

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_VERTEXFORMAT_H__
