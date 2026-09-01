#include "NxApex.h"
#ifndef NX_USER_RENDER_BONE_BUFFER_DESC_H
#define NX_USER_RENDER_BONE_BUFFER_DESC_H
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
\brief class NxUserRenderBoneBufferDesc, structs NxRenderDataFormat and NxRenderBoneSemantic
*/

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
\brief The semantics available for bone buffers
*/
struct NxRenderBoneSemantic
{
	enum Enum
	{
		POSE = 0,	  //!< A matrix that transforms from object space into animated object space or even world space
		NUM_SEMANTICS //!< Count of semantics, not a valid semantic.
	};
};



/**
\brief Descriptor to generate a bone buffer

This descriptor is filled out by APEX and helps as a guide how the
bone buffer should be generated.
*/
class NxUserRenderBoneBufferDesc
{
public:
	NxUserRenderBoneBufferDesc(void)
	{
		maxBones = 0;
		hint     = NxRenderBufferHint::STATIC;
		for(physx::PxU32 i=0; i<NxRenderBoneSemantic::NUM_SEMANTICS; i++)
		{
			buffersRequest[i] = NxRenderDataFormat::UNSPECIFIED;
		}
	}
	
	bool isValid(void) const
	{
		physx::PxU32 numFailed = 0;
		return (numFailed == 0);
	}
	
public:
	/**
	\brief The maximum amount of bones this buffer will ever hold.
	*/
	physx::PxU32				maxBones;

	/**
	\brief Hint on how often this buffer is updated.
	*/
	NxRenderBufferHint::Enum	hint;

	/**
	\brief Array of semantics with the corresponding format.

	NxRenderDataFormat::UNSPECIFIED is used for semantics that are disabled
	*/
	NxRenderDataFormat::Enum	buffersRequest[NxRenderBoneSemantic::NUM_SEMANTICS];
};

PX_POP_PACK

}} // end namespace physx::apex

#endif
