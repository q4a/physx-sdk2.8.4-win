#ifndef __NX_EMITTER_LOD_PARAM_DESC_H__
#define __NX_EMITTER_LOD_PARAM_DESC_H__
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

///LOD parameters fro emitters
class NxEmitterLodParamDesc : public NxApexDesc
{
public:
    physx::PxF32  maxDistance;        ///< Objects greater than this distance from the player will be culled more aggressively
    physx::PxF32  distanceWeight;     ///< Weight given to distance parameter in LOD function
    physx::PxF32  speedWeight;        ///< Weight given to velocity parameter in LOD function
    physx::PxF32  lifeWeight;         ///< Weight given to life remain parameter in LOD function
    physx::PxF32  separationWeight;   ///< Weight given to separation parameter in LOD function

    physx::PxF32  bias;               ///< Bias given to objects spawned by this emitter, relative to other emitters in the same IOS

	/**
	\brief constructor sets to default.
	*/
	PX_INLINE NxEmitterLodParamDesc() : NxApexDesc()
	{
		init();
	}

	/**
	\brief sets members to default values.
	*/
	PX_INLINE void setToDefault()
	{
		NxApexDesc::setToDefault();
		init();
	}

	/**
	\brief checks if this is a valid descriptor.
	*/
	PX_INLINE bool isValid() const
	{
		bool retVal = NxApexDesc::isValid();
        if( distanceWeight < 0.0f || speedWeight < 0.0f || lifeWeight < 0.0f )
            return retVal = false;
        if( separationWeight < 0.0f || maxDistance < 0.0f || bias < 0.0f )
            return retVal = false;
		return retVal;
	}

	enum ManifestVersions {
		initial,

		count,
		current = count-1
	};

private:

	PX_INLINE void init()
	{
        // These defaults give you pure distance based LOD weighting
        maxDistance = 0.0f;
        distanceWeight = 1.0f;
        speedWeight = 0.0f;
        lifeWeight = 0.0f;
        separationWeight = 0.0f;
        bias = 1.0f;
	}
};


PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_EMITTER_LOD_PARAM_DESC_H__
