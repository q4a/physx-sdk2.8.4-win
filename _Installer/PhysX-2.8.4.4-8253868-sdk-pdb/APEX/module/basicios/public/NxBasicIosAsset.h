#ifndef __NX_BASIC_IOS_ASSET_H__
#define __NX_BASIC_IOS_ASSET_H__
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
#include "NxApex.h"
#include <limits.h>

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

#define NX_BASIC_IOS_AUTHORING_TYPE_NAME "NxBasicIosAsset"

/**
 \brief APEX Particle System Asset
 */
class NxBasicIosAsset : public NxIosAsset
{
public:
	///Get the radius of a particle
	virtual physx::PxF32						getParticleRadius() const = 0;
	///Get the rest density of a particle
	virtual physx::PxF32						getRestDensity() const = 0;
	///Get the maximum number of particles that are allowed to be newly created on each frame 
	virtual physx::PxF32						getMaxInjectedParticleCount() const	= 0;
	///Get the maximum number of particles that this IOS can simulate
	virtual physx::PxU32						getMaxParticleCount() const = 0;

protected:
	virtual ~NxBasicIosAsset()	{}
};

/**
 \brief APEX Particle System Asset Authoring class
 */
class NxBasicIosAssetAuthoring : public NxApexAssetAuthoring
{
public:
	///Set the radius of a particle
	virtual void setParticleRadius( physx::PxF32 ) = 0;
	///Set the rest density of a particle
	virtual void setRestDensity( physx::PxF32 ) = 0;
	///Set the maximum number of particles that are allowed to be newly created on each frame 
	virtual void setMaxInjectedParticleCount(physx::PxF32 count) = 0; 
	///Set the maximum number of particles that this IOS can simulate
	virtual void setMaxParticleCount(physx::PxU32 count) = 0;

protected:
	virtual ~NxBasicIosAssetAuthoring()	{}
};

PX_POP_PACK

}} // namespace physx::apex

#endif // __NX_BASIC_IOS_ASSET_H__
