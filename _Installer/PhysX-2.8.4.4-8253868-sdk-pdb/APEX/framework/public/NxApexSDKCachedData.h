#include "NxApex.h"
#ifndef __NX_APEX_SCENE_CACHED_DATA_H__
#define __NX_APEX_SCENE_CACHED_DATA_H__
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
\brief classes NxApexModuleCachedData, NxApexSDKCachedData
*/

namespace NxParameterized
{
class Interface;
};

namespace physx {
namespace apex {

/**
\brief Cached data is stored per-module.
*/
class NxApexModuleCachedData
{
public:
	/**
	 * \brief Retreives the cached data for the asset, if it exists.
	 *
	 * Otherwise returns NULL.
	 */
	virtual ::NxParameterized::Interface*	getCachedDataForAssetAtScale( NxApexAsset& asset, const physx::PxVec3& scale ) = 0;
};

/**
\brief A method for storing actor data in a scene
*/
class NxApexSDKCachedData
{
public:
	/**
	 * \brief Retreives the scene cached data for the actor, if it exists.
	 *
	 * Otherwise returns NULL.
	 */
	virtual NxApexModuleCachedData*	getCacheForModule( NxAuthObjTypeID moduleID ) = 0;

	/**
	 * \brief Save cache configuration to a stream
	 */
	virtual physx::PxFileBuf&				serialize( physx::PxFileBuf& ) const = 0;

	/**
	 * \brief Load cache configuration from a stream
	 */
	virtual physx::PxFileBuf&				deserialize( physx::PxFileBuf& ) = 0;	

	/**
	 * \brief Clear data
	 */
	virtual void					clear() = 0;

protected:
									NxApexSDKCachedData() {}
	virtual							~NxApexSDKCachedData() {}
};

}} // end namespace physx::apex


#endif // __NX_APEX_SCENE_CACHED_DATA_H__
