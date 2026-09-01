#ifndef __NX_IOFX_ASSET_H__
#define __NX_IOFX_ASSET_H__
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
#include "NxModifierDefs.h"

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

#define NX_IOFX_AUTHORING_TYPE_NAME "IOFX"

class NxModifier;
class ApexActor;

/**
	IOFX Asset public interface. Used to define the way the visual parameters are created 
	from physical parameters of a particle
*/
class NxIofxAsset : public NxApexAsset, public NxApexContext
{
public:
	///get the name of the material used for sprite-based particles visualization
	virtual const char*							getSpriteMaterialName() const = 0;

	///get the number of different mesh assets used for mesh-based particles visualization
	virtual physx::PxU32						getMeshAssetCount() const = 0;
	///get the name of one of the mesh assets used for mesh-based particles visualization
	/// \param index mesh asset internal index 
	virtual const char*							getMeshAssetName( physx::PxU32 index = 0 ) const = 0;
	///get the weight of one of the mesh assets used for mesh-based particles visualization. Can be any value; not normalized.
	/// \param index mesh asset internal index 
	virtual physx::PxU32						getMeshAssetWeight( physx::PxU32 index = 0 ) const = 0;

	///get the list of spawn modifiers
	virtual const NxModifier*					getSpawnModifiers( physx::PxU32 &outCount ) const = 0;
	///get the list of continuous modifiers
	virtual const NxModifier*					getContinuousModifiers( physx::PxU32 &outCount ) const = 0;

	///get the biggest possible scale given the current spawn- and continuous modifiers
	///note that some modifiers depend on velocity, so the scale can get arbitrarily large.
	/// \param maxVelocity this value defines what the highest expected velocity is to compute the upper bound 
	virtual physx::PxF32						getScaleUpperBound( physx::PxF32 maxVelocity ) const = 0;

	///the IOFX asset needs to inform other actors when it is released
	/// \note only for internal use
	virtual void								addDependentActor( ApexActor *actor ) = 0;
};


class NxIofxAssetAuthoring : public NxApexAssetAuthoring
{
};

PX_POP_PACK

}} // namespace physx::apex

#endif // __NX_IOFX_ASSET_H__
