#ifndef __NX_DESTRUCTIBLEACTORJOINT_H__
#define __NX_DESTRUCTIBLEACTORJOINT_H__
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

class NxJointDesc;

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
	Descriptor used to create the Destructible actor joint.
*/
class NxDestructibleActorJointDesc : public NxApexDesc
{
public:

	/**
	\brief constructor sets to default.
	*/
	PX_INLINE		NxDestructibleActorJointDesc();

	/**
	\brief Resets descriptor to default settings.
	*/
	PX_INLINE void	setToDefault();

	/**
		Returns true iff an object can be created using this descriptor.
	*/
	PX_INLINE bool	isValid() const;

	/**
		The PhysX SDK joint descriptor.  Note - localAnchor[i], localAxis[i], and localNormal[i]
		are not used if actor[i] is part of an NxDestructibleActor.
	*/
	NxJointDesc*			jointDesc;

	/**
		If destructible[i] is not NULL, it will effectively replace the actor[i] in jointDesc.
		At least one must be non-NULL.
	*/
	NxDestructibleActor*	destructible[2];

	/**
		If destructible[i] is not NULL and attachmentChunkIndex[i] is a valid chunk index within that
		destructible, then that chunk will be used for attachment.
		If destructible[i] is not NULL and attachmentChunkIndex[i] is NOT a valid chunk index (such as the
		default NxModuleDestructibleConst::INVALID_CHUNK_INDEX), then the nearest chunk to globalAnchor[i] is used instead.
		Once a chunk is determined, the chunk's associated NxActor will be the NxJoint's attachment actor.
	*/
	physx::PxI32					attachmentChunkIndex[2];


	/**
		Global attachment data.  Since destructibles come apart, there is no single referece frame
		associated with them.  Therefore it makes more sense to use a global reference frame
		when describing the attachment positions and axes.
	*/

	/**
	\brief global attachment positions
	*/
	physx::PxVec3			globalAnchor[2];

	/**
	\brief global axes
	*/
	physx::PxVec3			globalAxis[2];

	/**
	\brief global normals
	*/
	physx::PxVec3			globalNormal[2];
};

// NxDestructibleActorJointDesc inline functions

PX_INLINE NxDestructibleActorJointDesc::NxDestructibleActorJointDesc() : NxApexDesc()
{
	setToDefault();
}

PX_INLINE void NxDestructibleActorJointDesc::setToDefault()
{
	NxApexDesc::setToDefault();
	jointDesc = NULL;
	for( int i = 0; i < 2; ++i )
	{
		destructible[i] = NULL;
		attachmentChunkIndex[i] = NxModuleDestructibleConst::INVALID_CHUNK_INDEX;
		globalAnchor[i] = physx::PxVec3::zero();
		globalAxis[i].set(0,0,1);
		globalNormal[i].set(1,0,0);
	}
}

PX_INLINE bool NxDestructibleActorJointDesc::isValid() const
{
	if( jointDesc == NULL )
	{
		return false;
	}

	if( destructible[0] == NULL && destructible[1] == NULL )
	{
		return false;
	}

	for( int i = 0; i < 2; ++i )
	{
		if( abs( globalAxis[i].magnitudeSquared() - 1.0f) > 0.1f )
		{
			return false;
		}
		if( abs( globalNormal[i].magnitudeSquared() - 1.0f) > 0.1f )
		{
			return false;
		}
		//check orthogonal pairs
		if( abs( globalAxis[i].dot(globalNormal[i] ) ) > 0.1f )
		{
			return false;
		}
	}

	return NxApexDesc::isValid();
}


/**
	Destructible actor joint - a wrapper for an NxJoint.  This needs to be used because APEX may need to replace an NxJoint
	when fracturing occurs.  Always use the joint() method to get the currently valid joint.
*/
class NxDestructibleActorJoint : public NxApexInterface
{
public:

	/**
	Access to the NxJoint represented by the NxDestructibleActorJoint.  This is a temporary interface,
	and there are some shortcomings and restrictions:
		1) The user will have to upcast the result of joint() to the correct joint type in order
		to access the interface for derived joints.
		2) The user must never delete the joint using the PhysX SDK.  To release this joint, simply
		use the release() method of this object (defined in the NxApexInterface base class).
	*/
	virtual	NxJoint*	joint()	= 0;

protected:
	virtual				~NxDestructibleActorJoint() {}
};

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_DESTRUCTIBLEACTORJOINT_H__
