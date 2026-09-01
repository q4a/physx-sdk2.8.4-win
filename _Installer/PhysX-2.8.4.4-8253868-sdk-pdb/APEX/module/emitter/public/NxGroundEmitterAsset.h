#ifndef __NX_GROUND_EMITTER_ASSET_H__
#define __NX_GROUND_EMITTER_ASSET_H__
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

namespace physx {
namespace apex {


PX_PUSH_PACK_DEFAULT

#define NX_GROUND_EMITTER_AUTHORING_TYPE_NAME "GroundEmitterAsset"

///Ground emitter asset. Used to create Ground emitter actors with specific properties.
class NxGroundEmitterAsset : public NxApexAsset
{
protected:
	PX_INLINE NxGroundEmitterAsset(){}
	virtual ~NxGroundEmitterAsset(){}

public:
	///Gets the range from which the density of particles whithin the volume is randomly chosen
    virtual const NxRange<physx::PxF32> & getDensityRange() const = 0;  /* Scalable parameter */
	///Gets the velocity range.	The ground emitter actor will create objects with a random velocity within the velocity range.
    virtual const NxRange<physx::PxVec3> & getVelocityRange() const = 0; /* noise parameters */
	///Gets the lifetime range. The ground emitter actor will create objects with a random lifetime (in seconds) within the lifetime range.   
	virtual const NxRange<physx::PxF32> & getLifetimeRange() const = 0;

	///Gets the radius.  The ground emitter actor will create objects within a circle of size 'radius'.
    virtual physx::PxF32                  getRadius() const = 0;
	///Gets The maximum raycasts number per frame.
	virtual physx::PxU32					getMaxRaycastsPerFrame() const = 0;
	///Gets the height from which the ground emitter will cast rays at terrain/objects opposite of the 'upDirection'.
	virtual physx::PxF32					getRaycastHeight() const = 0;
	/**
	\brief Gets the height above the ground to emit particles. 
	 If greater than 0, the ground emitter will refresh a disc above the player's position rather than
	 refreshing a circle around the player's position.
	*/
	virtual physx::PxF32					getSpawnHeight() const = 0;
	/// Gets collision groups name used to cast rays 
	virtual const char *			getRaycastCollisionGroupMaskName() const = 0;
};

///Ground emitter authoring class. Used to create Ground emitter assets.
class NxGroundEmitterAssetAuthoring : public NxApexAssetAuthoring
{
protected:
	virtual ~NxGroundEmitterAssetAuthoring(){}
};


PX_POP_PACK

}} // end namespace physx::apex

#endif // #ifndef __NX_GROUND_PARTICLES_EMITTER_H__
