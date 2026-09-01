#include "NxApex.h"
#ifndef NX_USER_RENDER_RESOURCE_DESC_H
#define NX_USER_RENDER_RESOURCE_DESC_H
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
\brief class NxUserRenderResourceDesc
*/

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

class NxApexRenderable;

/**
\brief Describes all the data that makes up a renderable resource
*/
class NxUserRenderResourceDesc
{
public:
	NxUserRenderResourceDesc(void)
	{
		firstVertex      = 0;
		numVerts         = 0;
		
		indexBuffer      = 0;
		firstIndex       = 0;
		numIndices       = 0;
		
		boneBuffer       = 0;
		firstBone        = 0;
		numBones         = 0;
		
		instanceBuffer   = 0;
		firstInstance    = 0;
		numInstances     = 0;
		
		spriteBuffer     = 0;
		firstSprite      = 0;
		numSprites       = 0;

		material         = 0;
		submeshIndex	 = 0;

		userRenderData   = 0;

		numVertexBuffers = 0;
		vertexBuffers     = NULL;
		
		cullMode         = NxRenderCullMode::CLOCKWISE;
		primitives       = NxRenderPrimitiveType::UNKNOWN;

		opaqueMesh		= NULL;
	}

	bool isValid(void) const
	{
		physx::PxU32 numFailed = 0;
		if(numVertexBuffers >= 255)				numFailed++;
		if(numIndices   && !indexBuffer)		numFailed++;
		if(numBones     && !boneBuffer)			numFailed++;
		if(numInstances && !instanceBuffer)		numFailed++;
		if(numSprites   && !spriteBuffer)		numFailed++;
		PX_ASSERT(numFailed == 0);
		return numFailed == 0;
	}

public:
	NxUserOpaqueMesh *				opaqueMesh;			//!< A user specified opaque mesh interface.
	NxUserRenderVertexBuffer**		vertexBuffers;		//!< vertex buffers used when rendering this resource.
														//!< there should be no overlap in semantics between any two VBs.
	physx::PxU32					numVertexBuffers;	//!< number of vertex buffers used when rendering this resource.

	physx::PxU32					firstVertex;		//!< First vertex to render
	physx::PxU32					numVerts;			//!< Number of vertices to render
	
	NxUserRenderIndexBuffer			*indexBuffer;		//!< optional index buffer used when rendering this resource.
	physx::PxU32					firstIndex;			//!< First index to render
	physx::PxU32					numIndices;			//!< Number of indices to render
	
	NxUserRenderBoneBuffer			*boneBuffer;		//!< optional bone buffer used for skinned meshes.
	physx::PxU32					firstBone;			//!< First bone to render
	physx::PxU32					numBones;			//!< Number of bones to render
	
	NxUserRenderInstanceBuffer		*instanceBuffer;	//!< optional instance buffer if rendering multiple instances of the same resource.
	physx::PxU32					firstInstance;		//!< First instance to render
	physx::PxU32					numInstances;		//!< Number of instances to render
	
	NxUserRenderSpriteBuffer		*spriteBuffer;		//!< optional sprite buffer if rendering sprites
	physx::PxU32					firstSprite;		//!< First sprite to render
	physx::PxU32					numSprites;			//!< Number of sprites to render

	void							*material;			//!< user defined material used when rendering this resource.
	physx::PxU32					submeshIndex;		//!< the index of the submesh that render resource belongs to

	//! user defined pointer originally passed in to NxApexRenderable::NxApexupdateRenderResources(..)
	void							*userRenderData;

	NxRenderCullMode::Enum			cullMode;			//!< Triangle culling mode
	NxRenderPrimitiveType::Enum		primitives;			//!< Rendering primitive type (triangle, line strip, etc)
};

PX_POP_PACK

}} // end namespace physx::apex

#endif
