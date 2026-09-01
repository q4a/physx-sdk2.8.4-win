#include "NxApex.h"
#ifndef __NX_APEX_ASSET_H__
#define __NX_APEX_ASSET_H__
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
\brief class NxApexAsset
*/



namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
\brief Base class of all APEX assets
*/
class NxApexAsset : public NxApexInterface
{
public:
	/**
	 * \brief Returns the name of this deserialized asset
	 */
	virtual const char *getName() const = 0;

	/**
	 * \brief Returns the ID of the asset's authorable object type.
	 *
	 * Every asset will correspond to an APEX authorable object type.
	 * The module must register those types with the SDK at startup.
	 */
	virtual NxAuthObjTypeID getObjTypeID() const = 0;

	/**
	 * \brief Returns the name of this asset's authorable object type
	 */
	virtual const char *getObjTypeName() const = 0;

	/**
	 * \brief Save asset configuration to a stream
	 * \note This method is DEPRECATED, use the NxParameterized::Serializer for NxParameterized-based assets
	 */
	virtual physx::PxFileBuf & serialize( physx::PxFileBuf & ) const = 0;

	/**
	 * \brief Returns the number of assets force loaded by all of
	 * this asset's named asset references
	 */
	virtual physx::PxU32 forceLoadAssets( ) = 0;

	/**
	 * \brief Returns the default actor descriptor NxParamaterized interface
	 * Memory ownership stays with this asset.  The user may modify the interface values, but they will not persist past another acll to 'getDefaultActorDesc'
	 * Typically used to create an actor after making small local editing changes.
	 */
	virtual ::NxParameterized::Interface *getDefaultActorDesc() = 0;

	/**
	 * \brief Returns the default AssetPreview descriptor NxParamaterized interface
	 */
	virtual ::NxParameterized::Interface *getDefaultAssetPreviewDesc() = 0;

	/**
	 * \brief Returns the asset's NxParamaterized interface
	 * This cannot be directly modified!  It is read only to the user.
	 */
	virtual const ::NxParameterized::Interface *getAssetNxParameterized() = 0;

	/**
	 * \brief Creates an Actor representing the Asset in a Scene
	 */
	virtual NxApexActor * createApexActor(const ::NxParameterized::Interface& parms,NxApexScene& apexScene) = 0;

	/**
	 * \brief Creates an Asset Preview for the asset.
	 */
	virtual NxApexAssetPreview* createApexAssetPreview(const ::NxParameterized::Interface& params) = 0;

	/**
	 * \brief Releases the ApexAsset but returns the NxParameterized::Interface and *ownership* to the caller.
	 */
	virtual NxParameterized::Interface * releaseAndReturnNxParameterizedInterface(void) = 0;

	/**
	 * \brief Returns true if the asset is in a state that is valid for creating an actor.
	 */
	virtual bool isValidForActorCreation(const ::NxParameterized::Interface& /*parms*/,NxApexScene& /*apexScene*/) const = 0;

protected:
	virtual ~NxApexAsset() {}; // illegal, do not call
};

/**
\brief base class of all APEX asset authoring classes
*/
class NxApexAssetAuthoring : public NxApexInterface
{
public:
	/**
	 * \brief Save asset configuration to a stream
	 * \note This method is DEPRECATED, use the NxParameterized::Serializer for NxParameterized-based assets
	 */
	virtual physx::PxFileBuf & serialize( physx::PxFileBuf & ) const = 0;

	/**
	 * \brief Load asset configuration from a stream
	 * \note This method is DEPRECATED, use the NxParameterized::Serializer for NxParameterized-based assets
	 */
	virtual physx::PxFileBuf & deserialize( physx::PxFileBuf & ) = 0;

	/**
	 * \brief Returns the name of asset author
	 */
	virtual const char *getName() const = 0;

	/**
	 * \brief Returns the name of this APEX authorable object type
	 */
	virtual const char *getObjTypeName() const = 0;

	/**
	 * \brief Returns the asset's NxParameterized interface, may return NULL
	 */
	virtual ::NxParameterized::Interface *getNxParameterized() = 0;

	/**
	 * \brief Releases the ApexAsset but returns the NxParameterized::Interface and *ownership* to the caller.
	 */
	virtual NxParameterized::Interface * releaseAndReturnNxParameterizedInterface(void) = 0;
};

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_APEX_ASSET_H__
