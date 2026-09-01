#ifndef __NX_APEX_EMITTER_ASSET_H__
#define __NX_APEX_EMITTER_ASSET_H__
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
#include "NxEmitterGeoms.h"

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

#define NX_APEX_EMITTER_AUTHORING_TYPE_NAME "ApexEmitterAsset"

class NxApexEmitterActor;
class NxApexEmitterPreview;
class NxEmitterLodParamDesc;

///APEX Emitter asset. Emits particles within some shape.
class NxApexEmitterAsset : public NxApexAsset
{
protected:
    virtual ~NxApexEmitterAsset() {}

public:
	/**
	\brief Returns the explicit geometry for the all actors based on this asset if the asset is explicit, NULL otherwise
	*/
	virtual NxEmitterExplicitGeom * isExplicitGeom() = 0;

	/**
	\brief Returns the geometry used for the all actors based on this asset
	*/
    virtual const NxEmitterGeom*	getGeom() const = 0;

	///Gets IOFX asset name that is used to visualize partices of this emitter
    virtual const char *			getInstancedObjectEffectsAssetName( void ) const = 0;
    ///Gets IOS asset name that is used to simulate partices of this emitter
    virtual const char *			getInstancedObjectSimulatorAssetName( void ) const = 0;
    ///Gets IOS asset class name that is used to simulate partices of this emitter
 	virtual const char *			getInstancedObjectSimulatorTypeName( void ) const = 0;

	/* Scalable parameter */
    virtual const NxRange<physx::PxF32> & getDensityRange() const = 0; ///< Gets the range used to choose the density of particles
	/* Scalable parameter */
	virtual const NxRange<physx::PxF32> & getRateRange() const = 0;	///< Gets the range used to choose the emition rate

	/* Noise parameter */
    virtual const NxRange<physx::PxVec3> & getVelocityRange() const = 0; ///< Gets the range used to choose the velocity of particles
    /// Gets the range used to choose the lifetime of particles
	virtual const NxRange<physx::PxF32> & getLifetimeRange() const = 0;   /* Noise parameter */
    /// For an explicit emitter, Max Samples is ignored.  For shaped emitters, it is the maximum number of objects spawned in a step.
	virtual physx::PxU32                   getMaxSamples() const = 0;
	
	/**
	\brief Gets the emitter duration in seconds
	\note If NxApexEmitterActor::startEmit() is called with persistent=true, then this duration is ignored.
	*/
	virtual physx::PxF32				getEmitDuration() const = 0;

	///Gets LOD settings for this asset
	virtual const NxEmitterLodParamDesc& getLodParamDesc() const = 0;

};

///APEX Emitter Asset Authoring. Used to create APEX Emitter assets.
class NxApexEmitterAssetAuthoring : public NxApexAssetAuthoring
{
protected:
    virtual ~NxApexEmitterAssetAuthoring() {}
};


PX_POP_PACK

}} // end namespace physx::apex

#endif
