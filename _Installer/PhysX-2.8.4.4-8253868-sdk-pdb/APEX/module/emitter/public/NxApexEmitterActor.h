#ifndef __NX_EMITTER_ACTOR_H__
#define __NX_EMITTER_ACTOR_H__
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

class NxApexEmitterAsset;
class NxEmitterExplicitGeom;
class NxEmitterLodParamDesc;
class NxApexRenderVolume;

/// Apex emitter actor class. Emits particles whithin a given shape.
class NxApexEmitterActor : public NxApexActor
{
protected:
	virtual ~NxApexEmitterActor() {}

public:
	/**
	\brief Returns the asset the instance has been created from.
	*/
	virtual NxApexEmitterAsset * getEmitterAsset() const = 0;

	/**
	\brief Returns the explicit geometry for THIS ACTOR only
	*/
	virtual NxEmitterExplicitGeom * isExplicitGeom() = 0;

	/**
	\brief Gets the global pose
	*/
	virtual physx::PxMat44	     getGlobalPose() const = 0;
	/**
	\brief Sets the curent pose of the emitter
	*/
	virtual void				 setCurrentPose( const physx::PxMat44& pose ) = 0;
	/**
	\brief Sets the curent position of the emitter
	*/
	virtual void				 setCurrentPosition( const physx::PxVec3& pos ) = 0;

	/**
	\brief Attatches the emitter to an actor
	NxActor pointer can be NULL to detach existing actor 
	*/
	virtual void				 setAttachActor( NxActor* ) = 0;
	/**
	\brief sets the relative pose of the emitter in the space of the actor to which it is attatched
	*/
	virtual void				 setAttachRelativePose( const physx::PxMat44& pose ) = 0;

	/// Retirieves the actor, to which the emitter is attatched. NULL is returned for an unattatched emitter.
	virtual const NxActor *		 getAttachActor() const = 0;
	/// Retrieves the relative pose of the emitter in the space of the actor to which it is attatched
	virtual const physx::PxMat44 getAttachRelativePose() const = 0;
	///	Retrieves the particle radius
	virtual physx::PxF32         getObjectRadius() const = 0;

	/* Collision filtering settings for overlap tests */
	/// Sets collision groups used to reject particles that overlap the geometry 
	virtual void				setOverlapTestCollisionGroups( physx::PxU32 ) = 0;
	/// Sets collision groups mask. \sa NxGroupsMask
	virtual void				setOverlapTestCollisionGroupsMask( NxGroupsMask* ) = 0;

	/// Gets collision groups used to reject particles that overlap the geometry 
	virtual physx::PxU32		getOverlapTestCollisionGroups() const = 0;
	/// Gets collision groups mask. \sa NxGroupsMask
	virtual const NxGroupsMask*	getOverlapTestCollisionGroupsMask() const = 0;

	/* 
	\brief start emitting particles
	 * If persistent is true, the emitter will emit every frame until stopEmit()is
	 * called.  
	 */
	virtual void                 startEmit( bool persistent = true ) = 0;				
	///stop emitting particles
	virtual void                 stopEmit() = 0;
	///true if the emitter is  emitting particles
	virtual bool                 isEmitting() = 0;

	///	Gets	LOD settings
	virtual const NxEmitterLodParamDesc& getLodParamDesc() const = 0;
	///	Sets	LOD settings
	virtual void				 setLodParamDesc( const NxEmitterLodParamDesc& ) = 0;


	/* Override authored scalable parameters, if necessary */
	///Sets the range from which the density of particles whithin the volume is randomly chosen
	virtual void                 setDensityRange( const NxRange<physx::PxF32>& ) = 0;
	///Sets the range from which the emition rate is randomly chosen
	virtual void                 setRateRange( const NxRange<physx::PxF32>& ) = 0;

	///Sets the range from which the velocity of a particle is randomly chosen
	virtual void                 setVelocityRange( const NxRange<physx::PxVec3>& ) = 0;
	///Sets the range from which the lifetime of a particle is randomly chosen
	virtual void                 setLifetimeRange( const NxRange<physx::PxF32>& ) = 0;

	///Sets whether or not authored asset particles are emitted
	virtual void				 emitAssetParticles( bool enable ) = 0;
	///Gets whether or not authored asset particles are emitted
	virtual bool				 getEmitAssetParticles() const = 0;

	///Emitted particles are injected to specified render volume on initial frame.
	///Set to NULL to clear the preferred volume.
	virtual void                 setPreferredRenderVolume( NxApexRenderVolume* volume ) = 0;
};

PX_POP_PACK

}} // end namespace physx::apex

#endif
