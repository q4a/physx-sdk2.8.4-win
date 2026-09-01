#ifndef __NX_DESTRUCTIBLEACTOR_H__
#define __NX_DESTRUCTIBLEACTOR_H__
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
#include "NxModuleDestructible.h"
#include "NxActorDesc.h"

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

struct NxDestructibleParameters;
class NxDestructibleJoint;
class NxApexRenderVolume;
class NxApexEmitterActor;


/**
	Flags for NxDestructibleActor::raycast()
*/
struct NxDestructibleActorRaycastFlags
{
	enum Enum
	{
		StaticChunks =				(1<<0),
		DynamicChunks =				(1<<1),

		AllChunks =					StaticChunks|DynamicChunks,

		SegmentIntersect =			(1<<2),	// Do not consider intersect times > 1

		ForceAccurateRaycastsOn =	(1<<3),
		ForceAccurateRaycastsOff =	(1<<4),
	};
};

/**
	Determines which type of emitter to associate with a render volume
*/
struct NxDestructibleEmitterType
{
	enum Enum
	{
		Crumble,
		Dust,
		Count
	};
};


/**
	Destructible actor API.  The destructible actor is instanced from an NxDestructibleAsset.
*/
class NxDestructibleActor : public NxApexActor, public NxApexActorSource, public NxApexRenderable
{
public:
	virtual const NxRenderMeshActor* getRenderMeshActor( NxDestructibleActorMeshType::Enum type = NxDestructibleActorMeshType::Skinned ) const = 0;

	/**
		Get/set state
	*/

	/**
		Gets the global pose used when the actor is added to the scene, in the NxDestructibleActorDesc
	*/
	virtual physx::PxMat44			getInitialGlobalPose() const = 0;

	/**
		Gets the destructible actor's 3D (possibly nonuniform) scale
	*/
	virtual physx::PxVec3			getScale() const = 0;

	/**
		Returns true iff the destructible actor starts off life dynamic.
	*/
	virtual bool					isInitiallyDynamic() const = 0;

	/**
		Returns an array of visibility data for each chunk.  Each byte in the array is 0 if the
		corresponding chunkIndex is invisible, 1 if visibile.
			visibilityArray = a pointer to the byte buffer to hold the visibility values.
			visibilityArraySize = the size of the visibilityArray
	*/
	virtual void					getChunkVisibilities( physx::PxU8* visibilityArray, physx::PxU32 visibilityArraySize ) const = 0;

	/**
		Returns the number of visible chunks for the given mesh type.  This is the number of 1's written to the visibilityArray
		by getChunkVisibilities.
	*/
	virtual physx::PxU32			getNumVisibleChunks( NxDestructibleActorMeshType::Enum type = NxDestructibleActorMeshType::Skinned ) const = 0;

	/**
		Returns a pointer to an array of visible chunk indices for the given mesh type.  The array has valid values
		in positions [0] through [getNumVisibleChunks()-1].
	*/
	virtual const physx::PxU32*		getVisibleChunks( NxDestructibleActorMeshType::Enum type = NxDestructibleActorMeshType::Skinned ) const = 0;

	/**
		Returns a chunk's pose, without scaling.
			index = the chunk index within the actor
	*/
	virtual physx::PxMat44			getChunkPose( physx::PxU32 index ) const = 0;

	/**
		Returns a chunk's linear velocity in world space.
			index = the chunk index within the actor
	*/
	virtual physx::PxVec3			getChunkLinearVelocity( physx::PxU32 index ) const = 0;

	/**
		Returns a chunk's angular velocity in world space.
			index = the chunk index within the actor
	*/
	virtual physx::PxVec3			getChunkAngularVelocity( physx::PxU32 index ) const = 0;

	/**
		Returns the transform of the chunk's graphical representation.  This may have
		a scale component.
			index = the chunk index within the actor
	*/
	virtual const physx::PxMat44 	getPartTM( physx::PxU32 index ) const = 0;

	/**
		Sets the linear velocity of every dynamic chunk to the given value.
	*/
	virtual void					setLinearVelocity( const physx::PxVec3& linearVelocity ) = 0;

	/**
		Sets the angular velocity of every dynamic chunk to the given value.
	*/
	virtual void					setAngularVelocity( const physx::PxVec3& angularVelocity ) = 0;

	/**
		If the indexed chunk is visible, it is made dynamic (if it is not already).
		If NxModuleDestructibleConst::INVALID_CHUNK_INDEX is passed in, all visible chunks in the
		destructible actor are made dynamic, if they are not already.
	*/
	virtual void					setDynamic( physx::PxI32 chunkIndex = NxModuleDestructibleConst::INVALID_CHUNK_INDEX ) = 0;

	/**
		Damage
	*/

	/**
		Apply damage at a point.  Damage will be propagated into the destructible based
		upon its NxDestructibleParameters.
			damage = the amount of damage at the damage point
			momentum = the magnitude of the impluse to transfer to the actor
			position = the damage location
			direction = direction of impact.  This is valid even if momentum = 0, for use in deformation calculations.
			chunkIndex = which chunk to damage (returned by rayCast and NxModuleDestructible::getDestructibleAndChunk).
				If chunkIndex = NxModuleDestructibleConst::INVALID_CHUNK_INDEX, then the nearest visible chunk hit is found.
	*/
	virtual void					applyDamage( physx::PxF32 damage, physx::PxF32 momentum, const physx::PxVec3& position, const physx::PxVec3& direction, physx::PxI32 chunkIndex = NxModuleDestructibleConst::INVALID_CHUNK_INDEX ) = 0;

	/**
		Apply damage to all chunks within a radius.  Damage will also propagate into the destructible
		based upon its NxDestructibleParameters.
			damage = the amount of damage at the damage point
			momentum = the magnitude of the impluse to transfer to each chunk
			position = the damage location
			radius = distance from damage location at which chunks will be affected
			falloff = whether or not to decrease damage with distance from the damage location.  If true,
				damage will decrease linearly from the full damage (at zero distance) to zero damage (at radius).
				If false, full damage is applied to all chunks within the radius.
	*/
	virtual void					applyRadiusDamage( physx::PxF32 damage, physx::PxF32 momentum, const physx::PxVec3& position, physx::PxF32 radius, bool falloff ) = 0;


	/**
		Queries
	*/

	/**
		Returns the index of the first visible chunk hit in the actor by worldRay, if any.
		Otherwise returns NxModuleDestructibleConst::INVALID_CHUNK_INDEX.
		If a chunk is hit, the time and normal fields are modified.
			time = (return value) of the time to the hit chunk, if any.
			normal = (return value) the surface normal of the hit chunk's collision volume, if any.
			worldRay = the ray to fire at the actor (the direction need not be normalized)
			flags = raycast control flags (see NxDestructibleActorRaycastFlags)
			parentChunkIndex = (if not equal to NxModuleDestructibleConst::INVALID_CHUNK_INDEX)
				the chunk subhierarchy in which to confine the raycast.  If parentChunkIndex =
				NxModuleDestructibleConst::INVALID_CHUNK_INDEX, then the whole actor is searched.
	*/
	virtual physx::PxI32			rayCast( physx::PxF32& time, physx::PxVec3& normal, const NxRay& worldRay, NxDestructibleActorRaycastFlags::Enum flags, physx::PxI32 parentChunkIndex = NxModuleDestructibleConst::INVALID_CHUNK_INDEX ) const = 0;

	/**
		Returns the index of the first visible chunk hit in the actor by swept oriented bounding box, if any.
		Otherwise returns NxModuleDestructibleConst::INVALID_CHUNK_INDEX.
		If a chunk is hit, the time and normal fields are modified.
			time = (return value) of the time to the hit chunk, if any.
			normal = (return value) the surface normal of the hit chunk's collision volume, if any.
			worldBox = the obb to sweep against the actor, oriented in world space
			worldDisplacement = the displacement of the center of the worldBox through the sweep, in world space
			flags = raycast control flags (see NxDestructibleActorRaycastFlags)
	*/
	virtual physx::PxI32			obbSweep( physx::PxF32& time, physx::PxVec3& normal, const NxBox& worldBox, const physx::PxVec3& worldDisplacement, NxDestructibleActorRaycastFlags::Enum flags ) const = 0;

	/**
		Misc.
	*/
	virtual void					setLODWeights(physx::PxF32 maxDistance, physx::PxF32 distanceWeight, physx::PxF32 massWeight, physx::PxF32 speedWeight, physx::PxF32 lifeWeight, physx::PxF32 separationWeight, physx::PxF32 bias) = 0;

	/**
		Enable/disable the crumble emitter
	*/
	virtual void					setCrumbleEmitterState( bool enable ) = 0;
	
	/**
		Enable/disable the dust emitter
	*/	
	virtual void					setDustEmitterState( bool enable ) = 0;

	/**
		Sets a preferred render volume for a dust or crumble emitter
	*/	
	virtual void					setPreferredRenderVolume(NxApexRenderVolume *volume,NxDestructibleEmitterType::Enum type) = 0;


	/**
		Returns the NxApexEmitterActor of either a dust or crumble emitter
	*/	
	virtual NxApexEmitterActor*		getApexEmitter(NxDestructibleEmitterType::Enum type) = 0; 

	/**
	*  Recreates the Apex Emitter, if necessary.  Use this method to re-initialize the crumble or dust emitter after a change has been made to a dependent asset
	*/
	virtual bool					recreateApexEmitter(NxDestructibleEmitterType::Enum type) = 0;
	

protected:
	virtual							~NxDestructibleActor() {}
};

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_DESTRUCTIBLEACTOR_H__
