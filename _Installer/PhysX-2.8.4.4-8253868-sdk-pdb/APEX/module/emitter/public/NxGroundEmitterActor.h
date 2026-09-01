#ifndef __NX_GROUND_EMITTER_ACTOR_H__
#define __NX_GROUND_EMITTER_ACTOR_H__
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
#include "NxApexUtils.h"

namespace physx {
namespace apex {


PX_PUSH_PACK_DEFAULT

class NxGroundEmitterAsset;
class NxEmitterLodParamDesc;
class NxApexRenderVolume;

/**
 \brief a user calback interface used to map raycast hitpoints to material ID
 * If an instance of this class is registered with a ground emitter actor, the actor will call
 * requestMaterialLookups() in lieu of doing raycasts.  The call will occur from within the scope
 * of the ApexScene thread, so the callback must be thread safe.
 */
class NxMaterialLookupCallback
{
public:
	/// Material request structure. 
	struct MaterialRequest
	{
		physx::PxVec3	samplePosition; ///< test point position. This is filled by the emitter
		physx::PxU32	outMaterialID; ///< ID of the material at the hitpoint. This must be filled by the user 
	};

	/**
	\brief Submit the meterial lookup request. This method is called by the emitter and implemented by the user
	\param requestCount [IN] number of requests
	\param reqList [IN/OUT] the pointer to the requests. samplePosition is read and materialID is written to by the user.
	*/
	virtual void requestMaterialLookups( physx::PxU32 requestCount, MaterialRequest *reqList ) = 0;

	virtual ~NxMaterialLookupCallback() {}
};

///Ground Emitter actor. Uses raycasts against ground to spawn particles 
class NxGroundEmitterActor : public NxApexActor
{
protected:
	virtual ~NxGroundEmitterActor() {}

public:
	/**
	\brief Returns the asset the instance has been created from.
	*/
	virtual NxGroundEmitterAsset *  getEmitterAsset() const = 0;

	/**
	\brief Returns the pose of the emitter
	*/
	virtual const physx::PxMat44	getPose() const = 0;
	/**
	\brief Sets the pose of the emitter
	*/
	virtual void				    setPose( const physx::PxMat44& pos ) = 0;

	/** \brief Set the material lookup callback method that replaces raycasts */
	virtual void					setMaterialLookupCallback( NxMaterialLookupCallback* ) = 0;
	/** \brief Get the material lookup callback method that replaces raycasts */
	virtual NxMaterialLookupCallback * getMaterialLookupCallback() const = 0;

	/**
	\brief Attatches the emitter to an actor
	NxActor pointer can be NULL to detach existing actor 
	*/
	virtual void					setAttachActor( NxActor* ) = 0;
	/**
	\brief sets the relative position of the emitter in the space of the actor to which it is attatched
	*/
	virtual void					setAttachRelativePosition( const physx::PxVec3& pos ) = 0;

	/**\brief Retirieves the actor, to which the emitter is attatched. NULL is returned for an unattatched emitter. */
	virtual const NxActor *			getAttachActor() const = 0;
	/** \brief Retrieves the relative position of the emitter in the space of the actor to which it is attatched. */
	virtual const physx::PxVec3 &	getAttachRelativePosition() const = 0;

    /* Override some asset settings at run time */

	///Sets the range from which the density of particles whithin the volume is randomly chosen
    virtual void					setDensityRange( const NxRange<physx::PxF32>& ) = 0;
	///Sets the radius. The ground emitter actor will create objects within a circle of size 'radius'.
	virtual void                    setRadius( physx::PxF32 ) = 0;
	///Sets The maximum raycasts number per frame.
	virtual void                    setMaxRaycastsPerFrame( physx::PxU32 ) = 0;
	///Sets the height from which the ground emitter will cast rays at terrain/objects opposite of the 'upDirection'.
	virtual void                    setRaycastHeight( physx::PxF32 ) = 0;
	/**
	\brief Sets the height above the ground to emit particles. 
	 If greater than 0, the ground emitter will refresh a disc above the player's position rather than
	 refreshing a circle around the player's position.
	 
	*/
	virtual void                    setSpawnHeight( physx::PxF32 ) = 0;

	///Gets the range from which the density of particles whithin the volume is randomly chosen
    virtual const NxRange<physx::PxF32> & getDensityRange() const = 0;
   	///Gets the radius. The ground emitter actor will create objects within a circle of size 'radius'.
	virtual physx::PxF32			getRadius() const = 0;
	///Gets The maximum raycasts number per frame.
	virtual physx::PxU32			getMaxRaycastsPerFrame() const = 0;
	///Gets the height from which the ground emitter will cast rays at terrain/objects opposite of the 'upDirection'.
	virtual physx::PxF32			getRaycastHeight() const = 0;
	/**
	\brief Gets the height above the ground to emit particles. 
	 If greater than 0, the ground emitter will refresh a disc above the player's position rather than
	 refreshing a circle around the player's position.
	*/
	virtual physx::PxF32			getSpawnHeight() const = 0;

	/// Sets collision groups used to cast rays 
	virtual void					setRaycastCollisionGroups( physx::PxU32 ) = 0;
	/// Sets collision groups mask. \sa NxGroupsMask
	virtual void					setRaycastCollisionGroupsMask( NxGroupsMask* ) = 0;
	
	/// Gets collision groups used to cast rays 
	virtual physx::PxU32			getRaycastCollisionGroups() const = 0;
	/// Gets collision groups mask. \sa NxGroupsMask
	virtual const NxGroupsMask *	getRaycastCollisionGroupsMask() const = 0;

	///Emitted particles are injected to specified render volume on initial frame.
	///Set to NULL to clear the preferred volume.
	virtual void					setPreferredRenderVolume( NxApexRenderVolume* volume ) = 0;
};


PX_POP_PACK

}} // end namespace physx::apex

#endif
