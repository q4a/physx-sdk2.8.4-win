#ifndef __NX_IMPACT_EMITTER_ASSET_H__
#define __NX_IMPACT_EMITTER_ASSET_H__
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

#define NX_IMPACT_EMITTER_AUTHORING_TYPE_NAME "ImpactEmitterAsset"

class NxImpactEmitterActor;
class NxEmitterLodParamDesc;

///Impact emitter asset class 
class NxImpactEmitterAsset : public NxApexAsset
{
protected:
	PX_INLINE NxImpactEmitterAsset(){}
	virtual ~NxImpactEmitterAsset(){}

public:
	/// Get the setID that corresponds to the set name \sa NxImpactEmitterActor::registerImpact()
	virtual physx::PxU32			querySetID( const char * setName ) = 0;

	/**
	\brief Get a list of the event set names.  The name index in the outSetNames list is the setID.
	\param [in,out] inOutSetNames An array of const char * with at least nameCount 
	\param [in,out] nameCount The size of setNames as input and output, set to -1 if setIDs > nameCount
	*/
	virtual void					getSetNames( const char **inOutSetNames, physx::PxU32 &nameCount ) = 0;
};

///Impact emitter asset authoring. Used to create Impact Emitter assets
class NxImpactEmitterAssetAuthoring : public NxApexAssetAuthoring
{
protected:
	virtual ~NxImpactEmitterAssetAuthoring(){}

public:
};


PX_POP_PACK

}} // end namespace physx::apex

#endif 
