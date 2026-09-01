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
#ifndef __NX_FLUID_IOS_ACTOR_H__
#define __NX_FLUID_IOS_ACTOR_H__

#include "NxApex.h"

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

class NxFluidIosAsset;
/**
\brief Fluid IOS Actor. PhysX-based partice system.
 * This actor class does not have any set methods because actor creation will often times
 * result in immmediate NxFluid creation, and most NxFluid parameters are not run time 
 * configurable.  Thus all parameters must be specified up front in the asset file.
 */
class NxFluidIosActor : public NxApexActor
{
public:
	/* Get configuration data */
	/// Get the collision group of this fluid actor
	virtual NxCollisionGroup			getCollisionGroup() const = 0;

	/* Get run time data */
	///Get the current particle count
	virtual physx::PxU32				getParticleCount() const = 0;

protected:
	virtual ~NxFluidIosActor()	{}
};

PX_POP_PACK

}} // namespace physx::apex

#endif // __NX_FLUID_IOS_ACTOR_H__
