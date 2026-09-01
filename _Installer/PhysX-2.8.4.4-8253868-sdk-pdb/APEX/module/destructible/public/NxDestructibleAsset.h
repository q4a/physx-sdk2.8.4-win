#ifndef __NX_DESTRUCTIBLEASSET_H__
#define __NX_DESTRUCTIBLEASSET_H__
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

#define NX_DESTRUCTIBLE_AUTHORING_TYPE_NAME "DestructibleAsset"

#include "FractureTools.h"

class NxConvexMesh;

namespace physx {
namespace apex {

class NxDestructibleActor;
class NxDestructiblePreview;

PX_PUSH_PACK_DEFAULT
#pragma warning(push)
#pragma warning(disable:4121)

/**
	Flags that may be set for all chunks at a particular depth in the fracture hierarchy
*/
struct NxDestructibleDepthParametersFlag
{
	enum Enum
	{
		/**
			Chunks at this hierarchy depth level may take impact damage if this flag is set.
			Note, NxDestructibleParameters::forceToDamage must also be positive for this
			to take effect.
		*/
		TAKE_IMPACT_DAMAGE			= (1<<0),

		/**
			Chunks at this depth should have pose updates ignored.
		*/
		IGNORE_POSE_UPDATES			= (1<<1),

		/**
			Chunks at this depth should be ignored in raycast callbacks.
		*/
		IGNORE_RAYCAST_CALLBACKS	= (1<<2),

		/**
			Chunks at this depth should be ignored in contact callbacks.
		*/
		IGNORE_CONTACT_CALLBACKS	= (1<<3),

		/**
			User defined flags.
		*/
		USER_FLAG_0					= (1<<4),
		USER_FLAG_1					= (1<<5),
		USER_FLAG_2					= (1<<6),
		USER_FLAG_3					= (1<<7)
	};
};


/**
	Parameters that may be set for all chunks at a particular depth in the fracture hierarchy
*/
struct NxDestructibleDepthParameters
{
	/**
	\brief constructor sets to default.
	*/
	PX_INLINE		NxDestructibleDepthParameters();

	/**
	\brief (re)sets the structure to the default.	
	*/
	PX_INLINE void	setToDefault();

	/**
	\brief (re)sets the structure to parameters which are likely to be the most computationally expensive.
	*/
	PX_INLINE void	setToMostExpensive();

	/**
		A convenience function to determine if the NX_DESTRUCTIBLE_TAKE_IMPACT_DAMAGE is set.
	*/
	PX_INLINE bool	takesImpactDamage() const;

	/**
		A convenience function to determine if the NX_DESTRUCTIBLE_IGNORE_POSE_UPDATES is set.
	*/
	PX_INLINE bool	ignoresPoseUpdates() const;

	/**
		A convenience function to determine if the NX_DESTRUCTIBLE_IGNORE_RAYCAST_CALLBACKS is set.
	*/
	PX_INLINE bool	ignoresRaycastCallbacks() const;

	/**
		A convenience function to determine if the NX_DESTRUCTIBLE_IGNORE_CONTACT_CALLBACKS is set.
	*/
	PX_INLINE bool	ignoresContactCallbacks() const;

	/**
		A convenience function to determine if the NX_DESTRUCTIBLE_USER_FLAG_0, NX_DESTRUCTIBLE_USER_FLAG_1,
		NX_DESTRUCTIBLE_USER_FLAG_2, or NX_DESTRUCTIBLE_USER_FLAG_3 flag is set.
	*/
	PX_INLINE bool	hasUserFlagSet( physx::PxU32 flagIndex ) const;

	/**
		A collection of flags defined in NxDestructibleDepthParametersFlag.
	*/
	physx::PxU32	flags;
};

// NxDestructibleDepthParameters inline functions

PX_INLINE NxDestructibleDepthParameters::NxDestructibleDepthParameters()
{
	setToDefault();
}

PX_INLINE void NxDestructibleDepthParameters::setToDefault()
{
	flags = 0;
}

PX_INLINE void NxDestructibleDepthParameters::setToMostExpensive()
{
	flags = NxDestructibleDepthParametersFlag::TAKE_IMPACT_DAMAGE;
}

PX_INLINE bool NxDestructibleDepthParameters::takesImpactDamage() const
{
	return (flags&NxDestructibleDepthParametersFlag::TAKE_IMPACT_DAMAGE) != 0;
}

PX_INLINE bool NxDestructibleDepthParameters::ignoresPoseUpdates() const
{
	return (flags&NxDestructibleDepthParametersFlag::IGNORE_POSE_UPDATES) != 0;
}

PX_INLINE bool NxDestructibleDepthParameters::ignoresRaycastCallbacks() const
{
	return (flags&NxDestructibleDepthParametersFlag::IGNORE_RAYCAST_CALLBACKS) != 0;
}

PX_INLINE bool NxDestructibleDepthParameters::ignoresContactCallbacks() const
{
	return (flags&NxDestructibleDepthParametersFlag::IGNORE_CONTACT_CALLBACKS) != 0;
}

PX_INLINE bool NxDestructibleDepthParameters::hasUserFlagSet( physx::PxU32 flagIndex ) const
{
	switch( flagIndex )
	{
	case 0:	return (flags&NxDestructibleDepthParametersFlag::USER_FLAG_0) != 0;
	case 1:	return (flags&NxDestructibleDepthParametersFlag::USER_FLAG_1) != 0;
	case 2:	return (flags&NxDestructibleDepthParametersFlag::USER_FLAG_2) != 0;
	case 3:	return (flags&NxDestructibleDepthParametersFlag::USER_FLAG_3) != 0;
	default: return false;
	}
}


/**
	Flags that apply to a destructible actor
*/
struct NxDestructibleParametersFlag
{
	enum Enum
	{
		/**
			If set, chunks will "remember" damage applied to them, so that many applications of a damage amount
			below damageThreshold will eventually fracture the chunk.  If not set, a single application of
			damage must exceed damageThreshold in order to fracture the chunk.
		*/
		ACCUMULATE_DAMAGE =			0x01,

		/**
			If set, then chunks which are tagged as "support" chunks (via NxDestructibleChunkDesc::isSupportChunk)
			will have environmental support in static destructibles.
		*/
		ASSET_DEFINED_SUPPORT =		0x02,

		/**
			If set, then chunks which overlap the NxScene's static geometry will have environmental support in
			static destructibles.
		*/
		WORLD_SUPPORT =				0x04,

		/**
			Note: if both ASSET_DEFINED_SUPPORT and WORLD_SUPPORT are set, then chunks must be tagged as
			"support" chunks AND overlap the NxScene's static geometry in order to be environmentally supported.
		*/


		/**
			Whether or not chunks at or deeper than the "debris" depth (see NxDestructibleParameters::debrisDepth)
			will time out.  The lifetime is a value between NxDestructibleParameters::debrisLifetimeMin and
			NxDestructibleParameters::debrisLifetimeMax, based upon the destructible module's LOD setting.
		*/
		DEBRIS_TIMEOUT =			0x08,

		/**
			Whether or not chunks at or deeper than the "debris" depth (see NxDestructibleParameters::debrisDepth)
			will be removed if they separate too far from their origins.  The maxSeparation is a value between
			NxDestructibleParameters::debrisMaxSeparationMin and NxDestructibleParameters::debrisMaxSeparationMax,
			based upon the destructible module's LOD setting.
		*/
		DEBRIS_MAX_SEPARATION =		0x10,

		/**
			If set, the smallest chunks may be further broken down, either by fluid crumbles (if a crumble particle
			system is specified in the NxDestructibleActorDesc), or by simply removing the chunk if no crumble
			particle system is specified.  Note: the "smallest chunks" are normally defined to be the deepest level
			of the fracture hierarchy.  However, they may be taken from higher levels of the hierarchy if
			NxModuleDestructible::setMaxChunkDepthOffset is called with a non-zero value.
		*/
		CRUMBLE_SMALLEST_CHUNKS =	0x20,

		/**
			If set, the NxDestructibleActor::rayCast function will search within the nearest visible chunk hit
			for collisions with child chunks.  This is used to get a better raycast position and normal, in
			case the parent collision volume does not tightly fit the graphics mesh.  The returned chunk index
			will always be that of the visible parent that is intersected, however.
		*/
		ACCURATE_RAYCASTS =			0x40,

		/**
			If set, the validBounds field of NxDestructibleParameters will be used.  These bounds are translated
			(but not scaled or rotated) to the origin of the destructible actor.  If a chunk or chunk island moves
			outside of those bounds, it is destroyed.
		*/
		USE_VALID_BOUNDS =			0x80
	};
};

#pragma warning(disable: 4324) // structure was padded due to __declspec(align())

/**
	Parameters that apply to a destructible actor
*/
struct NxDestructibleParameters
{
	/**
	\brief constructor sets to default.
	*/
	PX_INLINE		NxDestructibleParameters();

	/**
	\brief (re)sets the structure to the default.	
	*/
	PX_INLINE void	setToDefault();

	/**
		The damage amount which will cause a chunk to fracture (break free) from the destructible.
		This is obtained from the damage value passed into the NxDestructibleActor::applyDamage,
		or NxDestructibleActor::applyRadiusDamage, or via impact (see 'forceToDamage', below).
	*/
	physx::PxF32		damageThreshold;

	/**
		Controls the distance into the destructible to propagate damage.  The damage applied to the chunk
		is multiplied by damageToRadius, to get the propagation distance.  All chunks within the radius
		will have damage applied to them.  The damage applied to each chunk varies with distance to the damage
		application position.  Full damage is taken at zero distance, and zero damage at the damage radius.
	*/
	physx::PxF32		damageToRadius;

	/**
		Limits the amount of damage applied to a chunk.  This is useful for preventing the entire destructible
		from getting pulverized by a very large application of damage.  This can easily happen when impact damage is
		used, and the damage amount is proportional to the impact force (see forceToDamage).
	*/
	physx::PxF32		damageCap;

	/**
		If a chunk is at a depth which has NX_DESTRUCTIBLE_TAKE_IMPACT_DAMAGE set (see NxDestructibleDepthParameters),
		then when a chunk has a collision in the NxScene, it will take damage equal to forceToDamage mulitplied by
		the impact force.
		The default value is zero, which effectively disables impact damage.
	*/
	physx::PxF32		forceToDamage;

	/** 
		Large impact force may be reported if rigid bodies are spawned inside one another.  In this case the realative velocity of the two
		objects will be low.  This variable allows the user to set a minimum velocity threshold for impacts to ensure that the objects are 
		moving at a min velocity in order for the impact force to be considered.  
	*/
	physx::PxF32		impactVelocityThreshold;

	/**
		When a chunk takes impact damage due to physical contact (see NxDestructibleDepthParametersFlag::TAKE_IMPACT_DAMAGE), this parameter
		is the maximum impulse the contact can generate.  Weak materials such as glass may have this set to a low value, so that
		heavier objects will pass through them during fracture.
		N.B.: Setting this parameter to 0 disables the impulse cap; that is, zero is interpreted as infinite.
		Default value = 0.0f.
	*/
	physx::PxF32		materialStrength;

	/**
		Damage applied to chunks may deform (move) a chunk without fracturing it, if damageToPercentDeformation is
		positive.  The damage applied to the chunk is multiplied by damageToPercentDeformation, and the resulting
		"percent deformation" is used to translate and rotate the chunk.  The translation is the "percent deformation"
		times the size of the chunk, in the direction given by the 'direction' paramater in applyDamage
		(see NxDestructibleActor).  For radius damage, the direction is always radial from the impact position.
		The rotation appplied is the "percent deformation" times one radian.
		The default value is zero, which disables deformation.
	*/
	physx::PxF32		damageToPercentDeformation;

	/**
		If a chunk's percent deformation (see damageToPercentDeformation) exceeds deformationPercentLimit in
		either translation or rotation, then the chunk will fracture.
	*/
	physx::PxF32		deformationPercentLimit;

	/**
		If initially static, the destructible will become part of an extended support structure if it is
		in contact with another static destructible that also has this flag set.
	*/
	physx::PxU32		formExtendedStructures;

	/**
		The chunk hierarchy depth at which to create a support graph.  Higher depth levels give more detailed support,
		but will give a higher computational load.  Chunks below the support depth will never be supported.
	*/
	physx::PxU32		supportDepth;

	/**
		The chunk hierarchy depth at which chunks are considered to be "debris."  Chunks at this depth or
		below will be considered for various debris settings, such as debrisLifetime.
		Negative values indicate that no chunk depth is considered debris.
		Default value is -1.
	*/
	physx::PxI32		debrisDepth;

	/**
		The chunk hierarchy depth up to which chunks will always be processed.  These chunks are considered
		to be essential either for gameplay or visually.
		The minimum value is 0, meaning the level 0 chunk is always considered essential.
		Default value is 0.
	*/
	physx::PxU32		essentialDepth;

	/**
		"Debris chunks" (see debrisDepth, above) will be destroyed after a time (in seconds)
		separated from non-debris chunks.  The actual lifetime is interpolated between these
		two values, based upon the module's LOD setting.  To disable lifetime, clear the
		NX_DESTRUCTIBLE_DEBRIS_TIMEOUT flag in the flags field.
		If debrisLifetimeMax < debrisLifetimeMin, the mean of the two is used for both.
		Default debrisLifetimeMin = 1.0, debrisLifetimeMax = 10.0f.
	*/
	physx::PxF32		debrisLifetimeMin;
	physx::PxF32		debrisLifetimeMax;

	/**
		"Debris chunks" (see debrisDepth, above) will be destroyed if they are separated from
		their origin by a distance greater than maxSeparation.  The actual maxSeparation is
		interpolated between these two values, based upon the module's LOD setting.  To disable
		maxSeparation, clear the NX_DESTRUCTIBLE_DEBRIS_MAX_SEPARATION flag in the flags field.
		If debrisMaxSeparationMax < debrisMaxSeparationMin, the mean of the two is used for both.
		Default debrisMaxSeparationMin = 1.0, debrisMaxSeparationMax = 10.0f.
	*/
	physx::PxF32		debrisMaxSeparationMin;
	physx::PxF32		debrisMaxSeparationMax;

	/**
		"Debris chunks" (see debrisDepth, above) will be destroyed if they are separated from
		their origin by a distance greater than maxSeparation multiplied by the original
		destructible asset size.  The actual maxSeparation is interpolated between these
		two values, based upon the module's LOD setting.  To disable maxSeparation, clear the
		NX_DESTRUCTIBLE_DEBRIS_MAX_SEPARATION flag in the flags field.
		If debrisMaxSeparationMax < debrisMaxSeparationMin, the mean of the two is used for both.
		Default debrisMaxSeparationMin = 1.0, debrisMaxSeparationMax = 10.0f.
	*/
	physx::PxBounds3	validBounds;

	/**
		If greater than 0, the chunks' speeds will not be allowed to exceed this value.  Use 0
		to disable this feature (this is the default).
	*/
	physx::PxF32		maxChunkSpeed;

	/**
		Dynamic chunk islands will have their masses raised to this power.  Values less than 1 have the
		effect of reducing the ratio of different masses.  The closer massScaleExponent is to zero, the
		more the ratio will be "flattened."  This helps PhysX converge when there is a very large number
		of interacting rigid bodies (such as a pile of destructible chunks).
		Valid range: [0,1].  Default = 0.5.
	*/
	physx::PxF32		massScaleExponent;

	/**
		A collection of flags defined in NxDestructibleParametersFlag.
	*/
	physx::PxU32		flags;

	/**
		The relative volume (chunk volume / whole destructible volume) below which GRBs are used
		instead of RBs to represent chunks in the physics scene.
	*/
	physx::PxF32		grbVolumeLimit;

	/**
		Spacing of particle grid used to represent rigid bodies in GRB
	*/
	physx::PxF32		grbParticleSpacing;

	/**
		Scale factor used to apply an impulse force along the normal of chunk when fractured.  This is used
		in order to "push" the pieces out as they fracture.
	*/
	physx::PxF32		fractureImpulseScale;

	/**
		Optional dominance group for dynamic chunks created when fractured. (ignored if > 31)
	*/
	NxDominanceGroup	dynamicChunksDominanceGroup;

	/**
		Whether or not to usedynamicChunksGroupsMask.  If false, NULL will be passed into the DestructibleActor upon
		instantiation, through the NxDestructibleActorDesc.
	*/
	bool				useDynamicChunksGroupsMask;

	/**
		Optional groups mask for dynamic chunks created when fractured. (Used if useDynamicChunksGroupsMask is true.)
	*/
	NxGroupsMask		dynamicChunksGroupsMask;

	/**
		The maximum number of NxDestructibleDepthParameters (see depthParameters).
	*/
	enum { kDepthParametersCountMax = 16 };

	/**
		The number of NxDestructibleDepthParameters (see depthParameters).
		Must be in the range [0, kDepthParametersCountMax].
	*/
	physx::PxU32		depthParametersCount;

	/**
		Parameters that apply to every chunk at a given level (see NxDestructibleDepthParameters).
		the element [0] of the array applies to the level 0 (unfractured) chunk, element [1] applies
		to the level 1 chunks, etc.
		The number of valid depth parameters must be given in depthParametersCount.
	*/
	NxDestructibleDepthParameters	depthParameters[kDepthParametersCountMax];
};

// NxDestructibleParameters inline functions

PX_INLINE NxDestructibleParameters::NxDestructibleParameters()
{
	setToDefault();
}

PX_INLINE void NxDestructibleParameters::setToDefault()
{
	damageThreshold = 1.0f;
	damageToRadius = 0.1f;
	damageCap = 0;
	forceToDamage = 0;
	impactVelocityThreshold = 0.0f;
	materialStrength = 0.0f;
	damageToPercentDeformation = 0.0f;
	deformationPercentLimit = 1.0f;
	formExtendedStructures = false;
	supportDepth = 0;
	debrisDepth = -1;
	essentialDepth = 0;
	debrisLifetimeMin = 1.0f;
	debrisLifetimeMax = 10.0f;
	debrisMaxSeparationMin = 1.0f;
	debrisMaxSeparationMax = 10.0f;
	validBounds.set( physx::PxVec3( -10000.0f ), physx::PxVec3( 10000.0f ) );
	maxChunkSpeed = 0.0f;
	massScaleExponent = 0.5f;
	fractureImpulseScale = 0.0f;
	dynamicChunksDominanceGroup = (NxDominanceGroup)-1;
	useDynamicChunksGroupsMask = false;
	dynamicChunksGroupsMask.bits0 = dynamicChunksGroupsMask.bits1 = dynamicChunksGroupsMask.bits2 = dynamicChunksGroupsMask.bits3 = 0;
	grbParticleSpacing = 0.1f;
	grbVolumeLimit = 0.2f;
	flags = NxDestructibleParametersFlag::ACCUMULATE_DAMAGE;
	depthParametersCount = 0;
}

/**
	Descriptor to build one chunk in a fracture hierarchy.
*/
class NxDestructibleChunkDesc : public NxApexDesc
{
public:
	/**
	\brief constructor sets to default.
	*/
	PX_INLINE		NxDestructibleChunkDesc();

	/**
	\brief (re)sets the structure to the default.	
	*/
	PX_INLINE void	setToDefault();

	/**
		Returns true iff an object can be created using this descriptor.
	*/
	PX_INLINE bool	isValid() const;

	/**
		Defines the chunk to be environmentally supported, if the appropriate NxDestructibleParametersFlag flags
		are set in NxDestructibleParameters.
	*/
	bool			isSupportChunk;

	/**
		Defines the chunk to be unfractureable.  If this is true, then none of its children will be fractureable.
	*/
	bool			doNotFracture;

	/**
		Defines the chunk to be undamageable.  This means this chunk will not fracture, but its children might.
	*/
	bool			doNotDamage;

	/**
		Defines the chunk to be uncrumbleable.  This means this chunk will not be broken down into fluid mesh particles
		no matter how much damage it takes.  Note: this only applies to chunks with no children.  For a chunk with
		children, then:
		1) The chunk may be broken down into its children, and then its children may be crumbled, if the doNotCrumble flag
		is not set on them.
		2) If the Destructible module's chunk depth offset LOD may be set such that this chunk effectively has no children.
		In this case, the doNotCrumble flag will apply to it.
	*/
	bool			doNotCrumble;

	/**
		Whether or not to precompute chunk surface boundaries.  This is used if a dust system is defined.
	*/
	bool			cacheSurfaceBoundaries;

	/**
		Collision hull data.  If collisionVertexData = NULL, then the NxDestructibleAssetAuthoring's ExplicitHierarchicalMesh hulls will be used.
	*/

	/**
		Collision hull vertices.  If non-NULL, the convex hull of these points will be used.
		Every vertex data element must be in the form of float[3], although the data elements may be
		separated by any byte amount (defined by collisionVertexByteStride).
	*/
	const void*		collisionVertexData;

	/**
		How many vertices are stored in the collisionVertexData buffer
	*/
	physx::PxU32	collisionVertexCount;

	/**
		Byte offset between two vertex data elements (beginning to beginning)
	*/
	physx::PxU32	collisionVertexByteStride;
};

// NxDestructibleChunkDesc inline functions

PX_INLINE NxDestructibleChunkDesc::NxDestructibleChunkDesc()
{
	setToDefault();
}

PX_INLINE void NxDestructibleChunkDesc::setToDefault()
{
	NxApexDesc::setToDefault();
	isSupportChunk = false;
	doNotFracture = false;
	doNotDamage = false;
	doNotCrumble = false;
	cacheSurfaceBoundaries = false;
	collisionVertexData = NULL;
	collisionVertexCount = 0;
	collisionVertexByteStride = 0;
}

PX_INLINE bool NxDestructibleChunkDesc::isValid() const
{
	return NxApexDesc::isValid();
}


/**
	Descriptor for the cookChunk() method of NxDestructibleAssetAuthoring
*/
class NxDestructibleAssetCookingDesc : public NxApexDesc
{
public:
	/**
	\brief constructor sets to default.
	*/
	PX_INLINE		NxDestructibleAssetCookingDesc();

	/**
	\brief (re)sets the structure to the default.	
	*/
	PX_INLINE void	setToDefault();

	/**
		Returns true iff an object can be created using this descriptor.
	*/
	PX_INLINE bool	isValid() const;

	/**
		Beginning of array of descriptors, one for each chunk.  If this array is smaller than
		the number of parts in the NxDestructibleAssetAuthoring's IExplicitHierarchicalMesh,
		(or chunkDescs == NULL), then default chunk descriptors will be used to fill in
		missing chunk descriptors.
	*/
	NxDestructibleChunkDesc*	chunkDescs;

	/**
		The size of the chunkDescs array.  If this value is positive, then chunkDescs must
		be a valid pointer.
	*/
	physx::PxU32				chunkDescCount;
};

// NxDestructibleAssetCookingDesc inline functions

PX_INLINE NxDestructibleAssetCookingDesc::NxDestructibleAssetCookingDesc()
{
	setToDefault();
}

PX_INLINE void NxDestructibleAssetCookingDesc::setToDefault()
{
	NxApexDesc::setToDefault();
	chunkDescs = NULL;
	chunkDescCount = 0;
}

PX_INLINE bool NxDestructibleAssetCookingDesc::isValid() const
{
	if( chunkDescCount > 0 && chunkDescs == NULL )
	{
		return false;
	}
	return NxApexDesc::isValid();
}


/**
	Stats for an NxDestructibleAsset: memory usage, counts, etc.
*/

struct NxDestructibleAssetStats
{
	physx::PxU32			totalBytes;
	physx::PxU32			chunkCount;
	physx::PxU32			chunkBytes;
	physx::PxU32			chunkHullDataBytes;
	physx::PxU32			collisionCookedHullDataBytes;
	physx::PxU32			collisionMeshCount;
	NxRenderMeshAssetStats	renderMeshAssetStats;
};

/**
	Authoring API for a destructible asset.
*/
class NxDestructibleAssetAuthoring : public NxApexAssetAuthoring
{
public:

	/** Fracturing API */

	/**
		NxDestructibleAssetAuthoring contains an instantiation of IExplicitHierarchicalMesh.
		This function gives access to it.  See IExplicitHierarchicalMesh for details, it is
		the object used by the fracturing tool set for mesh fracturing operations and is used
		to generate the embedded NxApexRenderMesh as well as collision and hierarchy data
		for the destructible asset.
	*/
	virtual IExplicitHierarchicalMesh&		getExplicitHierarchicalMesh() = 0;

	/**
		NxDestructibleAssetAuthoring contains a second instantiation of IExplicitHierarchicalMesh
		used to describe the core mesh for slice fracturing (see FractureTools::FractureSliceDesc),
		done in createHierarchicallySplitMesh().  This function gives access to it.
	*/
	virtual IExplicitHierarchicalMesh&		getCoreExplicitHierarchicalMesh() = 0;

	/**
		NxDestructibleAssetAuthoring contains an instantiation of ICutoutSet used to describe the
		cutout fracturing shapes (see FractureTools::ICutoutSet), done in createChippedMesh().
		This function gives access to it.
	*/
	virtual FractureTools::ICutoutSet&		getCutoutSet() = 0;

	/**
		Builds a new IExplicitHierarchicalMesh from an array of triangles, used as the starting
		point for fracturing.  It will contain only one part, at depth 0.

		meshTriangles: pointer to array of NxExplicitRenderTriangles which make up the mesh
		meshTriangleCount the size of the meshTriangles array
		submeshData: pointer to array of NxExplicitSubmeshData, describing the submeshes
		submeshCount: the size of the submeshData array		
		meshPartition: if not NULL, an array of size meshPartitionCount, giving the end elements of contiguous subsets of meshTriangles.
			If meshPartition is NULL, one partition is assumed.
			When there is one partition, these triangles become the level 0 part.
			When there is more than one partition, these triangles become level 1 parts, while the union of the parts will be the level 0 part.
		meshPartitionCount: if meshPartition is not NULL, this is the size of the meshPartition array.
	*/
	virtual bool							setRootMesh
											(
												const NxExplicitRenderTriangle* meshTriangles,
												physx::PxU32 meshTriangleCount,
												const NxExplicitSubmeshData* submeshData,
												physx::PxU32 submeshCount,
												physx::PxU32* meshPartition = NULL,
												physx::PxU32 meshPartitionCount = 0
											) = 0;

	/**
		Builds a new IExplicitHierarchicalMesh from an array of triangles, used as the core mesh
		for slice fracture operations (see FractureTools::FractureSliceDesc, passed into
		createHierarchicallySplitMesh).

		mesh: pointer to array of NxExplicitRenderTriangles which make up the mesh
		meshTriangleCount the size of the meshTriangles array
		submeshData: pointer to array of NxExplicitSubmeshData, describing the submeshes
		submeshCount: the size of the submeshData array		
	*/
	virtual bool							setCoreMesh
											(
												const NxExplicitRenderTriangle* mesh,
												physx::PxU32 meshTriangleCount,
												const NxExplicitSubmeshData* submeshData,
												physx::PxU32 submeshCount,
												physx::PxU32* meshPartition = NULL,
												physx::PxU32 meshPartitionCount = 0
											) = 0;

	/**
		Splits the mesh in part[0], forming a hierarchy of fractured meshes in parts[1...] using
		slice-mode fracturing.

		meshProcessingParams: describes generic mesh processing directives
		desc: describes the slicing surfaces (see FractureSliceDesc)
		hullMethod: convex hulls will be generated for each part using the method.  See NxConvexHullMethod.
		materialDesc: material parameters, such as UV scale, core mesh material application, etc.  See FractureMaterialDesc.
		exportCoreMesh: if true, the core mesh will be included (at depth 1) in the hierarchically split mesh.  Otherwise, it will only be used to create a hollow space.
		randomSeed: seed for the random number generator, to ensure reproducibility.
		progressListener: The user must instantiate an IProgressListener, so that this function may report progress of this operation
		cancel: if not NULL and *cancel is set to true, the root mesh will be restored to its original state, and the function will return at its earliest opportunity.  Meant to be set from another thread.

		returns true if successful.
	*/
	virtual bool							createHierarchicallySplitMesh
											(
												const FractureTools::MeshProcessingParameters& meshProcessingParams,
												const FractureTools::FractureSliceDesc& desc,
												NxConvexHullMethod::Enum hullMethod,
												const FractureTools::FractureMaterialDesc& materialDesc,
												bool exportCoreMesh,
												physx::PxU32 randomSeed,
												IProgressListener& progressListener,
												volatile bool* cancel = NULL
											) = 0;

	/**
		Splits the mesh in part[0], forming a hierarchy of fractured meshes in parts[1...] using
		cutout-mode (chippable) fracturing.

		meshProcessingParams: describes generic mesh processing directives
		desc: describes the slicing surfaces (see FractureCutoutDesc)
		cutoutSet: the cutout set to use for fracturing (see ICutoutSet)
		sliceDesc: used if desc.applySlicingToCutoutRegions = true
		hullMethod: convex hulls will be generated for each part using the method.  See NxConvexHullMethod.
		materialDesc: material parameters, such as UV scale, core mesh material application, etc.  See FractureMaterialDesc.
		randomSeed: seed for the random number generator, to ensure reproducibility.
		progressListener: The user must instantiate an IProgressListener, so that this function may report progress of this operation
		cancel: if not NULL and *cancel is set to true, the root mesh will be restored to its original state, and the function will return at its earliest opportunity.  Meant to be set from another thread.

		returns true if successful.
	*/
	virtual bool							createChippedMesh
											(
												const FractureTools::MeshProcessingParameters& meshProcessingParams,
												const FractureTools::FractureCutoutDesc& desc,
												const FractureTools::ICutoutSet& cutoutSet,
												const FractureTools::FractureSliceDesc& sliceDesc,
												NxConvexHullMethod::Enum hullMethod,
												const FractureTools::FractureMaterialDesc& materialDesc,
												physx::PxU32 randomSeed,
												IProgressListener& progressListener,
												volatile bool* cancel = NULL
											) = 0;

	/**
		Builds the internal cutout set.

		pixelBuffer: pointer to be beginning of the pixel buffer
		bufferWidth: the width of the buffer in pixels
		bufferHeight: the height of the buffer in pixels
		snapThreshold: the pixel distance at which neighboring cutout vertices and segments may be fudged into alignment.
	*/
	virtual void							buildCutoutSet
											(
												const physx::PxU8* pixelBuffer,
												physx::PxU32 bufferWidth,
												physx::PxU32 bufferHeight,
												physx::PxF32 snapThreshold
											) = 0;

	/**
		Serialization/deserialization of the data associated with the fracture API.  This includes
		the root mesh, core mesh, and cutout set.
	*/
	virtual void							serializeFractureToolState( physx::PxFileBuf& stream, physx::IExplicitHierarchicalMesh::IEmbedding& embedding ) const = 0;
	virtual	void							deserializeFractureToolState( physx::PxFileBuf& stream, physx::IExplicitHierarchicalMesh::IEmbedding& embedding ) = 0;

	/**
		If 'enabled' is true, then the depth value is passed directly to cacheChunkOverlapsUpToDepth.  (See
		NxDestructibleAsset::cacheChunkOverlapsUpToDepth).  If 'enabled' is false, cacheChunkOverlapsUpToDepth will
		not be called.
	*/
	virtual void							setChunkOverlapsCacheDepth( bool enabled, physx::PxI32 depth = -1 ) = 0;

	/**
		Provide an NxRenderMeshAssetAuthoring which can be used to create an NxRenderMeshAsset
		which in turn is used to instance an NxRenderMeshActor when instancing an NxDestructibleActor.
		NULL is a valid argument, and can be used to clear the internal mesh data.
		If not NULL, the NxRenderMeshAsset must have the same number of chunks as in the chunkDescs array.  The
		NxRenderMeshAsset parts will be put in one-to-one correspondence with the chunks and used as their
		graphical representation.  The NxRenderMeshAssetAuthoring is also used to serialize the destructible
		asset.
	*/
	virtual void							setRenderMeshAssetAuthoring( NxRenderMeshAssetAuthoring* ) = 0;

	/**
		Accessor to the NxRenderMeshAssetAuthoring.
	*/
	virtual NxRenderMeshAssetAuthoring*		getRenderMeshAssetAuthoring(void) = 0;

	/**
		Gets the NxRenderMeshAsset associated with this asset.
	*/
	virtual const NxRenderMeshAsset*		getRenderMeshAsset() const = 0;

	/**
		Set the parameters used for runtime destruction behavior.  See NxDestructibleParameters.
	*/
	virtual void                            setDestructibleParameters( const NxDestructibleParameters& ) = 0;

	/**
		The NxDestructibleParameters which describe the default fracturing behavior for instanced
		NxDestructibleActors.  These may be overridden by calling setParameters().
	*/
	virtual NxDestructibleParameters		getDestructibleParameters() const = 0;

	/** 
		Set the name of the emitter to use when generating crumble particles.
	*/
	virtual void                            setCrumbleEmitterName( const char * ) = 0;

	/** 
		Set the name of the emitter to use when generating fracture-line dust particles.
	*/
	virtual void                            setDustEmitterName( const char * ) = 0;

	virtual void                            setGrbVolumeLimit( physx::PxF32 volLimit ) = 0;

	/**
		Set padding used for chunk neighbor tests.  This padding is relative to the largest diagonal
		of the asset's local bounding box.
		This value must be non-negative.
		Default value = 0.001f.
	*/
	virtual void							setNeighborPadding( physx::PxF32 neighborPadding ) = 0;

	/**
		Get padding used for chunk neighbor tests.  Set setNeighborPadding().
	*/
	virtual physx::PxF32					getNeighborPadding() const = 0;

	/**
		Once the internal IExplicitHierarchicalMesh is built using the fracture tools functions
		and all emitter names and parameters set, this functions builds the destructible asset.
		Every chunk (corresponding to a part in the IExplicitHierarchicalMesh) must have
		destruction-specific data set through the descriptor passed into this function.  See
		NxDestructibleAssetCookingDesc.
	*/
	virtual void                            cookChunks( const NxDestructibleAssetCookingDesc& ) = 0;

	/**
		The relative volume (chunk volume / whole destructible volume) below which GRBs are used
		instead of RBs to represent chunks in the physics scene.
	*/
	virtual physx::PxF32					getGrbVolumeLimit() const = 0;

	/**
		The scale factor used to apply an impulse force along the normal of chunk when fractured.  This is used
		in order to "push" the pieces out as they fracture.
	*/
	virtual physx::PxF32					getFractureImpulseScale() const = 0;

	/** 
		Large impact force may be reported if rigid bodies are spawned inside one another.  In this case the realative velocity of the two
		objects will be low.  This variable allows the user to set a minimum velocity threshold for impacts to ensure that the objects are 
		moving at a min velocity in order for the impact force to be considered.  
	*/
	virtual physx::PxF32					getImpactVelocityThreshold() const = 0;

	/**
		The total number of chunks in the cooked asset.
	*/
	virtual physx::PxU32					chunkCount() const = 0;

	/**
		The total number of fracture hierarchy depth levels in the cooked asset.
	*/
	virtual physx::PxU32					depthCount() const = 0;

	/**
		Returns stats (sizes, counts) for the asset.  See NxDestructibleAssetStats.
	*/
	virtual void							getStats( NxDestructibleAssetStats& stats ) const = 0;

	/**
	\brief Apply an arbitrary transformation to destructible asset

	This is a permanent transformation and it changes the object state. Should only be called immediately before serialization
	and without further modifying the object later on.

	\param transformation	This matrix is allowed to contain a translation and a rotation
	\param scale			Apply a uniform scaling as well
	*/
	virtual void							applyTransformation(const physx::PxMat44& transformation, physx::PxF32 scale) = 0;

};

/**
	Destructible asset API.  Destructible actors are instanced from destructible assets.
*/
class NxDestructibleAsset : public NxApexAsset
{
public:
	/** Instancing */

	/**
		Instances the NxDestructibleAsset, creating an NxDestructibleActor, using the NxDestructibleActorDesc.
		See NxDestructibleActor and NxDestructibleActorDesc.  This asset will own the NxDestructibleActor,
		so that any NxDestructibleActor created by it will be released when this asset is released.
		You may also call the NxDestructibleActor's release() method at any time.
	*/
	virtual void							releaseDestructibleActor( NxDestructibleActor& actor ) = 0;


	/** General */

	/**
		The NxDestructibleParameters which describe the default fracturing behavior for instanced
		NxDestructibleActors.
	*/
	virtual NxDestructibleParameters		getDestructibleParameters() const = 0;

	/** 
		The name of the emitter to use when generating crumble particles.
		Returns NULL if no emitter is configured.
	*/
	virtual const char*						getCrumbleEmitterName() const = 0;

	/** 
		The name of the emitter to use when generating fracture-line dust particles.
		Returns NULL if no emitter is configured.
	*/
	virtual const char*						getDustEmitterName() const = 0;

	/**
		The total number of chunks in the asset.
	*/
	virtual physx::PxU32					chunkCount() const = 0;

	/**
		The total number of fracture hierarchy depth levels in the asset.
	*/
	virtual physx::PxU32					depthCount() const = 0;

	/**
		Gets the NxRenderMeshAsset associated with this asset.
	*/
	virtual const NxRenderMeshAsset*		getRenderMeshAsset() const = 0;

	/**
		Replace the NxRenderMeshAsset associated with this asset.
		The NxRenderMeshAsset must have the same number of chunks as in the chunkDescs array.  The
		NxRenderMeshAsset parts will be put in one-to-one correspondence with the chunks and used as their
		graphical representation.
		Returns true if successful.
	*/
	virtual bool							setRenderMeshAsset( NxRenderMeshAsset& ) = 0;

	/**
		Returns stats (sizes, counts) for the asset.  See NxDestructibleAssetStats.
	*/
	virtual void							getStats( NxDestructibleAssetStats& stats ) const = 0;

	/**
		Ensures that the asset has a surface trace for all chunks.
	*/
	virtual void							cacheChunkSurfaceTraces() = 0;

	/**
		Ensures that the asset has chunk overlap informationc cached up to the given depth.
		If depth < 0 (as it is by default), the depth will be taken to be the supportDepth
		given in the asset's destructibleParameters.
		It is ok to pass in a depth greater than any chunk depth in the asset.
	*/
	virtual void							cacheChunkOverlapsUpToDepth( physx::PxI32 depth = -1 ) = 0;

protected:
	/** Hidden destructor.  Use release(). */
	virtual									~NxDestructibleAsset() {}
};


#pragma warning(pop)
PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_DESTRUCTIBLEASSET_H__
