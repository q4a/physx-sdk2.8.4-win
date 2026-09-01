#ifndef __NX_APEX_RENDER_VOLUME_H__
#define __NX_APEX_RENDER_VOLUME_H__
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

class NxIofxAsset;
class NxIofxActor;

/**
	An object which "owns" a volume of world space.  Any particles which enter the
	owned volume will be migrated to an IOFX actor owned by this object (if the
	particle's IOFX asset is affected by this volume).

	When volumes overlap, their relative priorities break the tie.  If multiple volumes
	have the same priority, the tie breaker goes to the volume that owns the particle.
*/
class NxApexRenderVolume : public NxApexRenderable, public NxApexInterface
{
public:
	/// Returns true if the volume affects all IOFX assets
	virtual bool	getAffectsAllIofx() const = 0;

	/// Adds IOFX asset to volume's list of affected IOFX assets, returns false on failure
	virtual bool    addIofxAsset( NxIofxAsset& iofx ) = 0;

	/// Moves the render volume while maintaining its dimensions
	virtual void	setPosition( const PxVec3& pos ) = 0;

	/// Directly assigns a new AABB ownership volume
	virtual void    setOwnershipBounds( const PxBounds3& b ) = 0;

	/// Retrieves the configured AABB bounds of the volume.  Call getBounds() for the "live" bounds.
	virtual PxBounds3 getOwnershipBounds() const = 0;

	/** \brief Retrieve list of IOFX actors owned by this volume (one per IOFX Asset per IOS actor)
	 *
	 * Returns count of 0 if empty.  Returned pointer is undefined when count is 0.
	 *
	 * The bounds of each of these IOFX is guaranteed to be within the bounds of the volume itself.
	 * Calling the updateRenderResources and dispatchRenderResources() methods of the volume will
	 * implicitly call the same methods of each of these IOFX actors, so there is no need to iterate
	 * over them for rendering purposes, unless you require special logic per IOFX.
	 *
	 * It is not necessary to release these actors, they will be released automatically when their
	 * volume, their IOFX Asset, or their host IOS actor are released.
	 *
	 * This call is not thread-safe.  The returned buffer is only valid until the next APEX API
	 * call that steps the simulation or modifies the number of IOFX actors in a scene.
	 */
	virtual NxIofxActor* const *	getIofxActorList( PxU32& count ) const = 0;

	/** \brief Retrieve list of volume's affected IOFX assets.
	 *
	 * Returns count of 0 if empty or volume affects all IOFX assets.  Returned pointer is
	 * undefined when count is 0.
	 *
	 * The returned buffer not thread-safe, and is only valid until the next APEX API
	 * call that steps the simulation or modifies the number of IOFX assets in a scene.
	 */
	virtual NxIofxAsset* const *	getIofxAssetList( PxU32& count ) const = 0;

	virtual PxVec3					getPosition() const = 0; ///< Returns center of ownership bounds
	virtual PxU32					getPriority() const = 0; ///< Returns priority of volume

	/** \brief Returns true if this volume affects the specified IOFX asset
     *
	 * Callers must acquire render lock of the volume before calling
     * this function, for thread safety.
     */
	virtual bool					affectsIofxAsset( const NxIofxAsset& iofx ) const = 0;

protected:
	virtual ~NxApexRenderVolume() {}
};

PX_POP_PACK

}} // namespace physx::apex

#endif // __NX_APEX_RENDER_VOLUME_H__
