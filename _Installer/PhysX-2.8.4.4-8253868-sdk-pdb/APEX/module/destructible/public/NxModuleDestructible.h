#ifndef __NX_MODULE_DESTRUCTIBLE_H__
#define __NX_MODULE_DESTRUCTIBLE_H__
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

class GrbShape;

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

class NxDestructibleAsset;
class NxDestructibleAssetAuthoring;
class NxDestructibleActor;
class NxDestructibleStructure;
class NxDestructibleActorJoint;
class NxDestructibleChunkDesc;
class NxDestructibleActorDesc;
class NxDestructibleActorJointDesc;

/**
	Flags returned by an NxUserChunkReport 
*/
struct NxApexChunkFlag
{
	enum Enum
	{
		/** The chunk is dynamic */
		DYNAMIC							=	1<<0,

		/** The chunk has environmental support, so will remain kinematic until fractured */
		EXTERNALLY_SUPPORTED			=	1<<1,

		/** The chunk has been fractured */
		FRACTURED						=	1<<2,

		/** The chunk has been destroyed because the NxActor FIFO was full */
		DESTROYED_FIFO_FULL				=	1<<3,

		/** The chunk has been destroyed because it has exceeded the maximum debris lifetime */
		DESTROYED_TIMED_OUT				=	1<<4,

		/** The chunk has been destroyed because it has exceeded the maximum debris distance */
		DESTROYED_EXCEEDED_MAX_DISTANCE	=	1<<5,

		/** The destroyed chunk has generated crumble particles */
		DESTROYED_CRUMBLED				=	1<<6,

		/** The destroyed chunk has moved beyond the destructible actor's valid bounds. */
		DESTROYED_LEFT_VALID_BOUNDS		=	1<<7
	};
};

/**
	Per-chunk data returned in NxApexDamageEventReportData
*/
struct NxApexChunkData
{
	/** The index of the chunk within the destructible asset */
	physx::PxU32			index;

	/** The hierarchy depth of the chunk */
	physx::PxU32			depth;

	/**
		The chunk's axis-aligned bounding box, in world space.
	*/
	physx::PxBounds3		worldBounds;

	/**
		How much damage the chunk has taken
	*/
	physx::PxF32			damage;

	/**
		Several flags holding chunk information, see NxApexChunkFlag.
	*/
	physx::PxU32			flags;
};

/**
	Per-actor damage event data returned by an NxUserChunkReport
*/
struct NxApexDamageEventReportData
{
	/**
		The NxDestructibleActor instance that these chunks belong to
	*/
	NxDestructibleActor*	destructible;

	/**
		Damage event hitDirection in world space.
	*/
	physx::PxVec3			hitDirection;

	/**
		The axis-aligned bounding box of all chunk fractures caused by this damage event,
		which have flags that overlap the module's chunkReportBitMask (see NxModuleDestructible::setChunkReportBitMask).
	*/
	physx::PxBounds3		worldBounds;

	/**
		Total number of fracture events caused by this damage event,
		which have flags that overlap the module's chunkReportBitMask (see NxModuleDestructible::setChunkReportBitMask).
	*/
	physx::PxU32			totalNumberOfFractureEvents;

	/**
		Min and max depth of chunk fracture events caused by this damage event,
		which have flags that overlap the module's chunkReportBitMask (see NxModuleDestructible::setChunkReportBitMask).
	*/
	physx::PxU16			minDepth;
	physx::PxU16			maxDepth;

	/**
		Array of chunk fracture event data for all chunks above a size threshold, which have flags that overlap the
		module's chunkReportBitMask (see NxModuleDestructible::setChunkReportBitMask).  Currently
		the size cutoff is determined by the NxModuleDestructible's chunkReportMaxFractureEventDepth (See
		NxModuleDestructible::setChunkReportMaxFractureEventDepth).  All chunks up to that depth, but no deeper,
		are reported in this list.  The size of this array is given by fractureEventListSize.  fractureEventList may
		be NULL if fractureEventListSize = 0.
	*/
	const NxApexChunkData* 	fractureEventList;

	/**
		Size of the fractureEventList array.  This may be less than totalNumberOfFractureEvents
		if some of the fracture events involve chunks which do not meet the size criterion
		described in the notes for the fractureEventList array.
	*/
	physx::PxU32			fractureEventListSize;
};


/**
	NxUserChunkReport - API for a user-defined callback to get information about fractured or damaged chunks
*/
class NxUserChunkReport
{
public:
	/**
		User implementation of NxUserChunkReport must overload this function.
		This function will get called when a chunk is fractured or destroyed.
		See the definition of NxApexChunkData for the information provided
		to the function.
	*/
	virtual void	onDamageNotify( const NxApexDamageEventReportData& damageEvent ) = 0;

protected:
	virtual			~NxUserChunkReport() {}
};


/**
	Destructible module constants
*/
struct NxModuleDestructibleConst
{
	enum Enum
	{
		/**
			When a function returns a chunk index, or takes a chunk index as a parameter, this
			value indicates "no chunk."
		*/
		INVALID_CHUNK_INDEX	= -1
	};
};

/**
	Render mesh distinction, skinned vs. static
*/
struct NxDestructibleActorMeshType
{
	enum Enum
	{
		Skinned,
		Static,

		Count
	};
};


/**
	Descriptor used to create the Destructible APEX module.
*/
class NxModuleDestructible : public NxModule
{
public:
	/** Object creation */

	/**
		Create an NxDestructibleAssetAuthoring instance. 
		This module will own the NxDestructibleAssetAuthoring, so that any NxDestructibleAssetAuthoring created
		by it will be released when this module is released. You may also call the NxDestructibleAssetAuthoring's
		release() method at any time.
	*/
	virtual NxDestructibleAssetAuthoring*	createDestructibleAssetAuthoring() = 0;

	/**
		Release an NxDestructibleAssetAuthoring instance
	*/
	virtual void							releaseDestructibleAssetAuthoring( NxDestructibleAssetAuthoring& ) = 0;

	/**
		Create an NxDestructibleAsset from an NxDestructibleAssetAuthoring instance.
		(See NxDestructibleAsset, NxDestructibleAuthoring.) 
		This module will own the NxDestructibleAsset, so that any NxDestructibleAsset created by it will
		be released when this module is released. You may also call the NxDestructibleAsset's release()
		method at any time.
	*/
	virtual NxDestructibleAsset*			createDestructibleAsset( NxDestructibleAssetAuthoring& , const char *name ) = 0;

	/**
		Create an NxDestructibleAsset from a stream.  (See NxDestructibleAsset.)  The stream must
		point to data created by NxDestructibleAssetAuthoring::serialize().
		This module will own the NxDestructibleAsset, so that any NxDestructibleAsset created by it will
		be released when this module is released. You may also call the NxDestructibleAsset's release()
		method at any time.
	*/
	virtual NxDestructibleAsset*			createDestructibleAsset( physx::PxFileBuf& stream, const char *name ) = 0;

	/**
		Release an NxDestructibleAsset created by createDestructibleAsset()
	*/
	virtual void							releaseDestructibleAsset( NxDestructibleAsset& ) = 0;

	/**
		Create an NxDestructibleActorJoint from the descriptor.  (See NxDestructibleActorJoint and
		NxDestructibleActorJointDesc.)
		This module will own the NxDestructibleActorJoint, so that any NxDestructibleAsset created by it will
		be released when this module is released.  You may also call the NxDestructibleActorJoint's release()
		method at any time.
	*/
	virtual NxDestructibleActorJoint*		createDestructibleActorJoint( const NxDestructibleActorJointDesc& desc, NxApexScene& scene ) = 0;


	/** LOD */

	/**
		The maximum number of allowed NxActors which represent dynamic groups of chunks.  If a fracturing
		event would cause more NxActors to be created, then oldest NxActors are released and the chunks they
		represent destroyed.
	*/
	virtual void							setMaxDynamicChunkIslandCount( physx::PxU32 maxCount ) = 0;

	/**
		Same functionality as setMaxDynamicChunkIslandCount(), except for GrbActors.
	*/
    virtual void							setMaxGrbActorCount( physx::PxU32 maxCount ) = 0;

    /**
        The padding applied to the combined scene valid bounds and NxDestructibleActor bounds.
        The final combined and passed bounds is used to cull NxActors and GrbActors
    */
    virtual void                            setValidBoundsPadding( physx::PxF32 pad ) = 0;

	/**
		Effectively eliminates the higher level (smaller) chunks from NxDestructibleAssets (see
		NxDestructibleAsset).  If maxChunkDepthOffset = 0, all chunks can be fractured.  If maxChunkDepthOffset = 1,
		the highest level (smallest) chunks are eliminated, etc.  This prevents too many chunks from being
		formed.  In other words, the higher maxChunkDepthOffset, the lower the LOD.
	*/
	virtual void							setMaxChunkDepthOffset( physx::PxU32 maxChunkDepthOffset ) = 0;

	/**
		Every destructible asset defines a min and max lifetime, and maximum separation distance for its chunks.
		Chunk islands are destroyed after this time or separation from their origins.  This parameter sets the
		lifetimes and max separations within their min-max ranges.  The valid range is [0,1].  Default is 0.5.
	*/
	virtual void							setMaxChunkSeparationLOD( physx::PxF32 separationLOD ) = 0;


	/** General */
	/**
		Sets the user chunk fracture/destroy callback. See NxUserChunkReport.
	*/
	virtual void							setChunkReport( NxUserChunkReport* chunkReport ) = 0;

	/**
		Set a bit mask of flags (see NxApexChunkFlag) for the fracture/destroy callback (See setChunkReport.)
		Fracture events with flags that overlap this mask will contribute to the NxApexDamageEventReportData.
	*/
	virtual void							setChunkReportBitMask( physx::PxU32 chunkReportBitMask ) = 0;

	/**
		Set the maximum depth at which individual chunk fracture events will be reported in an NxApexDamageEventReportData's
		fractureEventList through the NxUserChunkReport.  Fracture events deeper than this will still contribute to the
		NxApexDamageEventReportData's worldBounds and totalNumberOfFractureEvents.
	*/
	virtual void							setChunkReportMaxFractureEventDepth( physx::PxU32 chunkReportMaxFractureEventDepth ) = 0;

	/**
		Returns true iff the NxActor was created by the Destructible module.  If true, the user must
		NOT modify the actor.
	*/
	virtual bool							owns( const NxActor* actor ) const = 0;

	/**
		Given an NxShape, returns the NxDestructibleActor and chunkIndex which belong to it, if the shape's NxActor
		is owned by the Destructible module (see the 'owns' function, above).  Otherwise returns NULL.
		Useful when a scene query such as a raycast returns a shape.
	*/
	virtual NxDestructibleActor*			getDestructibleAndChunk( const NxShape* shape, physx::PxI32* chunkIndex = NULL ) const = 0;
#if APEX_USE_GRB
	virtual NxDestructibleActor*			getDestructibleAndChunk( const GrbShape* shape, physx::PxI32* chunkIndex = NULL ) const = 0;
#endif

	/**
		Applies damage to all NxDestructibleActors within the given radius in the apex scene.
			damage = the amount of damage at the damage point
			momentum = the magnitude of the impluse to transfer to each chunk
			position = the damage location
			radius = distance from damage location at which chunks will be affected
			falloff = whether or not to decrease damage with distance from the damage location.  If true,
				damage will decrease linearly from the full damage (at zero distance) to zero damage (at radius).
				If false, full damage is applied to all chunks within the radius.
	*/
	virtual void							applyRadiusDamage( NxApexScene& scene, physx::PxF32 damage, physx::PxF32 momentum,
															   const physx::PxVec3& position, physx::PxF32 radius, bool falloff ) = 0;

	/*
		Lets the user throttle the number of SDK actor creates per frame (per scene) due to destruction, as this can be quite costly.
		The default is 0xffffffff (unlimited).
	*/
	virtual void							setMaxActorCreatesPerFrame(physx::PxU32 maxActorsPerFrame) = 0;

protected:
	virtual									~NxModuleDestructible() {}
};


#if !defined(_USRDLL)
void instantiateModuleDestructible();
#endif

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_MODULE_DESTRUCTIBLE_H__
