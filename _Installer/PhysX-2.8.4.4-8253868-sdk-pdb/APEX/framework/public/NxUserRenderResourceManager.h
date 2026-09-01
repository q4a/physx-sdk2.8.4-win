#include "NxApex.h"
#ifndef NX_USER_RENDER_RESOURCE_MANAGER_H
#define NX_USER_RENDER_RESOURCE_MANAGER_H
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
\brief class NxUserRenderResourceManager, structs NxRenderPrimitiveType, NxRenderBufferHint, and NxRenderCullMode
*/

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

class NxUserRenderVertexBuffer;
class NxUserRenderVertexBufferDesc;
class NxUserRenderIndexBuffer;
class NxUserRenderIndexBufferDesc;
class NxUserRenderBoneBuffer;
class NxUserRenderBoneBufferDesc;
class NxUserRenderInstanceBuffer;
class NxUserRenderInstanceBufferDesc;
class NxUserRenderSpriteBuffer;
class NxUserRenderSpriteBufferDesc;
class NxUserRenderResource;
class NxUserRenderResourceDesc;
class NxUserOpaqueMesh;
class NxUserOpaqueMeshDesc;

/**
\brief Describe the implied vertex ordering
*/
struct NxRenderPrimitiveType
{
	enum Enum
	{
		UNKNOWN = 0,

		TRIANGLES,
		TRIANGLE_STRIP,

		LINES,
		LINE_STRIP,

		POINTS,
		POINT_SPRITES,
	};
};

/**
\brief Possible triangle culling modes
*/
struct NxRenderCullMode
{
    enum Enum
    {
        CLOCKWISE = 0,
        COUNTER_CLOCKWISE,
		NONE
    };
};

/**
\brief Hint of the buffer data lifespan
*/
struct NxRenderBufferHint
{
	enum Enum
	{
		STATIC = 0,
		DYNAMIC,
		STREAMING,
	};
};

/**
\brief User defined renderable resource manager

A render resource manager is an object that creates and manages renderable resources...
This is given to the APEX SDK at creation time via the descriptor and must be persistent through the lifetime
of the SDK.
*/
class NxUserRenderResourceManager
{
public:
	virtual								~NxUserRenderResourceManager() {}

	/**
		The create methods in this class will only be called from the context of an NxApexRenderable::updateRenderResources()
		call, but the release methods can be triggered by any APEX API call that deletes an NxApexActor.  It is up to
		the end-user to make the release methods thread safe.
	*/

	virtual NxUserRenderVertexBuffer   *createVertexBuffer( const NxUserRenderVertexBufferDesc &desc )     = 0;
	virtual void                        releaseVertexBuffer( NxUserRenderVertexBuffer &buffer )            = 0;

	virtual NxUserRenderIndexBuffer    *createIndexBuffer( const NxUserRenderIndexBufferDesc &desc )       = 0;
	virtual void                        releaseIndexBuffer( NxUserRenderIndexBuffer &buffer )              = 0;

	virtual NxUserRenderBoneBuffer     *createBoneBuffer( const NxUserRenderBoneBufferDesc &desc )         = 0;
	virtual void                        releaseBoneBuffer( NxUserRenderBoneBuffer &buffer )                = 0;

	virtual NxUserRenderInstanceBuffer *createInstanceBuffer( const NxUserRenderInstanceBufferDesc &desc ) = 0;
	virtual void                        releaseInstanceBuffer( NxUserRenderInstanceBuffer &buffer )        = 0;

	virtual NxUserRenderSpriteBuffer   *createSpriteBuffer( const NxUserRenderSpriteBufferDesc &desc )     = 0;
	virtual void                        releaseSpriteBuffer( NxUserRenderSpriteBuffer &buffer )            = 0;

	virtual NxUserRenderResource       *createResource( const NxUserRenderResourceDesc &desc )             = 0;

	/**
	releaseResource() should not release any of the included buffer pointers.  Those free methods will be
	called separately by the APEX SDK before (or sometimes after) releasing the NxUserRenderResource.
	*/
	virtual void                        releaseResource( NxUserRenderResource &resource )                  = 0;

	/**
	Get the maximum number of bones supported by a given material. Return 0 for infinite.
	For optimal rendering, do not limit the bone count (return 0 from this function).
	*/
	virtual physx::PxU32                       getMaxBonesForMaterial(void *material)                             = 0;
};

PX_POP_PACK

}} // end namespace physx::apex

#endif
