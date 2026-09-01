#include "NxApex.h"
#ifndef NX_USER_RENDER_BONE_BUFFER_H
#define NX_USER_RENDER_BONE_BUFFER_H
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
\brief class NxUserRenderBoneBuffer
*/

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
\brief type of the bone buffer data
*/
typedef NxApexRenderBufferData<NxRenderBoneSemantic, NxRenderBoneSemantic::Enum> NxApexRenderBoneBufferData;

/**
\brief Used for storing skeletal bone information used during skinning.

Separated out into its own interface as we don't know how the user is going to implement this
it could be in another vertex buffer, a texture or just an array and skinned on CPU depending
on engine and hardware support.
*/
class NxUserRenderBoneBuffer
{
public:
	virtual		~NxUserRenderBoneBuffer() {}

	/**
	\brief Called when APEX wants to update the contents of the bone buffer.

	The source data type is assumed to be the same as what was defined in the descriptor.
	APEX should call this function and supply data for ALL semantics that were originally
	requested during creation every time its called.

	\param [in] data				Contains the source data for the bone buffer.
	\param [in] firstBone			first bone to start writing to.
	\param [in] numBones			number of bones to write.
	*/
	virtual void writeBuffer(const NxApexRenderBoneBufferData &data, physx::PxU32 firstBone, physx::PxU32 numBones)
#if NX_APEX_LEGACY_WRITEBUFFER_API
	{
		// Default implementation that maps to old API...
		for(physx::PxU32 i=0; i<NxRenderBoneSemantic::NUM_SEMANTICS; i++)
		{
			NxRenderBoneSemantic::Enum semantic = (NxRenderBoneSemantic::Enum)i;
			const NxApexRenderSemanticData &semanticData = data.getSemanticData(semantic);
			if(semanticData.data)
			{
				writeBuffer(semantic, semanticData.data, semanticData.stride, firstBone, numBones);
			}
		}
	}
#else
	= 0;
#endif

private:
#if NX_APEX_LEGACY_WRITEBUFFER_API
	//! DEPRECATED! This will soon be removed!
	virtual void writeBuffer( NxRenderBoneSemantic::Enum semantic, const void *srcData, physx::PxU32 srcStride, physx::PxU32 firstDestElement, physx::PxU32 numElements ) = 0;
#endif
};

PX_POP_PACK

}} // end namespace physx::apex

#endif
