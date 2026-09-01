#include "NxApex.h"
#ifndef NX_USER_RENDER_SPRITEBUFFER_H
#define NX_USER_RENDER_SPRITEBUFFER_H
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
\brief classes NxUserRenderSpriteBuffer and NxApexRenderSpriteBufferData
*/

/**
\brief Cuda graphics resource
*/
typedef struct CUgraphicsResource_st *CUgraphicsResource;

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT


class NxRenderSpriteTextureData
{
	public:
		NxRenderSpriteTextureLayout::Enum layout;
		physx::PxU32 arrayIndex; //!< array index for array textures or cubemap face index 
		physx::PxU32 mipLevel;   //!< mipmap level

	public:
		PX_INLINE NxRenderSpriteTextureData(void)
		{
			layout = NxRenderSpriteTextureLayout::NONE;
			arrayIndex = 0;
			mipLevel = 0;
		}
};

/**
\brief sprite buffer data
*/
class NxApexRenderSpriteBufferData : public NxApexRenderBufferData<NxRenderSpriteSemantic, NxRenderSpriteSemantic::Enum>, public NxApexModuleSpecificRenderBufferData
{
};

/**
\brief Used for storing per-sprite instance data for rendering.
*/
class NxUserRenderSpriteBuffer
{
public:
	virtual		~NxUserRenderSpriteBuffer() {}

	/**
	\brief Called when APEX wants to update the contents of the sprite buffer.

	The source data type is assumed to be the same as what was defined in the descriptor.
	APEX should call this function and supply data for ALL semantics that were originally
	requested during creation every time its called.

	\param [in] data				Contains the source data for the sprite buffer.
	\param [in] firstSprite			first sprite to start writing to.
	\param [in] numSprites			number of vertices to write.
	*/
	virtual void writeBuffer(const NxApexRenderSpriteBufferData &data, physx::PxU32 firstSprite, physx::PxU32 numSprites)
#if NX_APEX_LEGACY_WRITEBUFFER_API
	{
		// Default implementation that maps to old API...
		for(physx::PxU32 i=0; i<NxRenderSpriteSemantic::NUM_SEMANTICS; i++)
		{
			NxRenderSpriteSemantic::Enum semantic = (NxRenderSpriteSemantic::Enum)i;
			const NxApexRenderSemanticData &semanticData = data.getSemanticData(semantic);
			if(semanticData.data)
			{
				writeBuffer(semantic, semanticData.data, semanticData.stride, firstSprite, numSprites);
			}
		}
	}
#else
	= 0;
#endif
	
	///Get the low-level handle of the buffer resource 
	///\return true if succeeded, false otherwise
	virtual bool getInteropResourceHandle(CUgraphicsResource &handle)
#if NX_APEX_DEFAULT_NO_INTEROP_IMPLEMENTATION
	{
		PX_FORCE_PARAMETER_REFERENCE(&handle);
		return false;
	}
#else
		= 0;
#endif

	static const physx::PxU32 MAX_INTEROP_TEXTURES = 4;

	virtual physx::PxU32 getInteropTextureUsage(NxRenderSpriteTextureData* dataList)
	{
		PX_FORCE_PARAMETER_REFERENCE(dataList);
		return 0;
	}
	virtual bool getInteropTextureHandleList(CUgraphicsResource* handleList)
	{
		PX_FORCE_PARAMETER_REFERENCE(handleList);
		return false;
	}


private:
	//! DEPRECATED! These will soon be removed!
#if NX_APEX_LEGACY_WRITEBUFFER_API
	//! write some data into the buffer.
	//  the source data type is assumed to be the same as what was defined in the descriptor.
	virtual void writeBuffer( NxRenderSpriteSemantic::Enum semantic, const void *srcData, physx::PxU32 srcStride, physx::PxU32 firstDestElement, physx::PxU32 numElements ) = 0;
#endif
};

PX_POP_PACK

}} // end namespace physx::apex

#endif
