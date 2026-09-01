#include "NxApex.h"
#ifndef __NX_RENDERMESHACTORDESC_H__
#define __NX_RENDERMESHACTORDESC_H__
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
\brief class NxRenderMeshActorDesc
*/

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
\brief Descriptor for creating a rendering mesh (collection of parts and submesh extra data)
*/
class NxRenderMeshActorDesc : public NxApexDesc
{
public:

	/**
	\brief constructor sets to default.
	*/
	PX_INLINE NxRenderMeshActorDesc()
	{
		setToDefault();
	}

	/**
	\brief (re)sets the structure to the default.	
	*/
	PX_INLINE void setToDefault()
	{
		visible = true;
		bufferVisibility = false;
		maxInstanceCount = 0;
		indexBufferHint = NxRenderBufferHint::STATIC;
		keepVisibleBonesPacked = true;
		overrideMaterials = NULL;
		overrideMaterialCount = 0;
		renderWithoutSkinning = false;
	}

	/**
	\brief Returns true if the descriptor is valid.
	\return true if the current settings are valid.
	*/
	PX_INLINE bool isValid() const
	{
		if( overrideMaterialCount != 0 && overrideMaterials == NULL )
		{
			return false;
		}
		return NxApexDesc::isValid();
	}

	/**
	\brief Initial visibility of all parts.
	*/
	bool						visible;

	/**
	\brief If this set to true, render visibility will not be updated until the
	user calls syncVisibility().
	*/
	bool						bufferVisibility;

	/**
	\brief Pack visible bones

	If set, bone transform order will be maitained in an order that keeps visible bones
	contiguous.  This is more efficient for cases where there are large number of
	nonvisible parts (it reduces the number of  bone transforms that need to be updated
	in writeBuffer).  This only works when vertices are single-weighted, and
	the number of bones equals the number of parts.
	*/
	bool						keepVisibleBonesPacked;

	/**
	\brief Render without skinning

	If set, all vertices will be transformed by one transform, set using
	NxRenderMeshActor::setTM with boneIndex = 0 (the default).
	*/
	bool						renderWithoutSkinning;

	/**
	\brief If maxInstanceCount = 0, mesh will be renedered without instancing.
	Otherwise, instance buffers (below) will be used.
	*/
	physx::PxU32				maxInstanceCount;

	/**
	\brief Hint passed along to the user describing whether the index buffer of
	this render mesh can be modified.
	*/
	NxRenderBufferHint::Enum	indexBufferHint;

	/**
	\brief Per-actor material names, to override those in the asset.
	The number of override material names is given by overrideMaterialCount.
	*/
	const char**				overrideMaterials;

	/**
	\brief Number of override material names in the overrideMaterials array.
	If this number is less than the number of materials in the asset, only
	the first overrideMaterialCount names will be overridden.  If this number
	is greater than the number of materials in the asset, the extra override
	material names will be ignored.
	*/
	physx::PxU32				overrideMaterialCount;
};

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_RENDERMESHACTORDESC_H__
