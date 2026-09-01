#include "NxApex.h"
#ifndef NX_USER_RENDER_VERTEXBUFFER_DESC_H
#define NX_USER_RENDER_VERTEXBUFFER_DESC_H
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
\brief class NxUserRenderVertexBufferDesc, structs NxRenderDataFormat and NxRenderVertexSemantic
*/


namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
\brief Potential semantics of a vertex buffer
*/
struct NxRenderVertexSemantic
{
	enum Enum
	{
		CUSTOM = -1,			//!< User-defined

		POSITION = 0,			//!< Position of vertex
		NORMAL,					//!< Normal at vertex
		TANGENT,				//!< Tangent at vertex
		BINORMAL,				//!< Binormal at vertex
		COLOR,					//!< Color at vertex
		TEXCOORD0,				//!< Texture coord 0 of vertex
		TEXCOORD1,				//!< Texture coord 1 of vertex
		TEXCOORD2,				//!< Texture coord 2 of vertex
		TEXCOORD3,				//!< Texture coord 3 of vertex
		BONE_INDEX,				//!< Bone index of vertex
		BONE_WEIGHT,			//!< Bone weight of vertex

		NUM_SEMANTICS			//!< Count of standard semantics, not a valid semantic
	};
};

/**
\brief Texture UV direction convention
*/
struct NxTextureUVOrigin
{
	enum Enum
	{
		ORIGIN_TOP_LEFT = 0,		//!< Origin is at top left
		ORIGIN_TOP_RIGHT = 1,		//!< Origin is at top right
		ORIGIN_BOTTOM_LEFT = 2,		//!< Origin is at bottom left
		ORIGIN_BOTTOM_RIGHT = 3,	//!< Origin is at bottom right

		ORIGIN_UNKNOWN = 0xFFFFFFFF	//!< Unknown origin
	};
};


/**
\brief Describes the semantics and layout of a vertex buffer
*/
class NxUserRenderVertexBufferDesc
{
public:
	NxUserRenderVertexBufferDesc(void)
	{
		maxVerts = 0;
		hint     = NxRenderBufferHint::STATIC;
		for( physx::PxU32 i=0; i<NxRenderVertexSemantic::NUM_SEMANTICS; i++ )
		{
			buffersRequest[i] = NxRenderDataFormat::UNSPECIFIED;
		}
        moduleIdentifier = 0;
		numCustomBuffers = 0;
		customBuffersIdents = NULL;
        customBuffersRequest = NULL;
		uvOrigin         = NxTextureUVOrigin::ORIGIN_TOP_LEFT;
		canBeShared      = true;
	}
	
	bool isValid(void) const
	{
		physx::PxU32 numFailed = 0;
		if(maxVerts == 0) numFailed++;
		if(!customBuffersRequest)
		{
			physx::PxU32 vertexSize = 0;
			for(physx::PxU32 i=0; i<NxRenderVertexSemantic::NUM_SEMANTICS; i++)
			{
				vertexSize += NxRenderDataFormat::getFormatDataSize(buffersRequest[i]);
			}
			if(vertexSize == 0) numFailed++;
		}
		return (numFailed == 0);
	}
	
public:
	/**
	\brief The maximum number of vertices that APEX will store in this buffer.
	*/
	physx::PxU32				maxVerts;

	/**
	\brief A hint about the update frequency of this buffer
	*/
	NxRenderBufferHint::Enum	hint;

	/**
	\brief Array of semantics with the corresponding format.

	NxRenderDataFormat::UNSPECIFIED is used for semantics that are disabled
	*/
	NxRenderDataFormat::Enum	buffersRequest[NxRenderVertexSemantic::NUM_SEMANTICS]; 
	
	/**
	\brief List of custom semantics that will be provided for this VB.
	*/

	/**
	\brief The number of custom VB semantics.
	*/
	physx::PxU32				numCustomBuffers;
	
	/**
	\brief Array of custom buffer identifiers
	*/
	void **						customBuffersIdents;

	/**
	\brief Array of custom VB format types
	*/
	NxRenderDataFormat::Enum*	customBuffersRequest;

	/**
	\brief Identifier of module generating this request
	*/
	NxAuthObjTypeID				moduleIdentifier;

	/**
	\brief Texture UV direction convention.
	*/
	NxTextureUVOrigin::Enum		uvOrigin;

	/**
	\brief Buffer can be shared by multiple render resources
	*/
	bool						canBeShared;
};

PX_POP_PACK

}} // end namespace physx::apex

#endif
