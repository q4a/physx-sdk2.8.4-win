#include "NxApex.h"
#ifndef __NX_RENDERMESHACTOR_H__
#define __NX_RENDERMESHACTOR_H__
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
\brief class NxRenderMeshActor
*/

#include "NxUserRenderResourceManager.h"	// For NxRenderCullMode

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT


/**
\brief Flags used for raycasting an NxRenderMeshActor
*/
struct NxRenderMeshActorRaycastFlags
{
	enum Enum
	{
		VISIBLE_PARTS =		(1<<0),
		INVISIBLE_PARTS =	(1<<1),

		ALL_PARTS =			VISIBLE_PARTS | INVISIBLE_PARTS
	};
};

/**
\brief Return data from raycasting an NxRenderMeshActor
*/
struct NxRenderMeshActorRaycastHitData
{
	/**
		The time to the hit point.  That is, the hit point is given by worldOrig + time*worldDisp.
	*/
	physx::PxF32	time;

	/**
		The part index containing the hit triangle.
	*/
	physx::PxU32	partIndex;

	/**
		The submesh index of the triangle hit.
	*/
	physx::PxU32	submeshIndex;

	/**
		The vertex indices of the triangle hit.
	*/
	physx::PxU32	vertexIndices[3];

	/**
		The lighting normal at the hit point, if the vertex format supports this.  Otherwise set to (0,0,0).
	*/
	physx::PxVec3	normal;

	/**
		The lighting tangent at the hit point, if the vertex format supports this.  Otherwise set to (0,0,0).
	*/
	physx::PxVec3	tangent;

	/**
		The lighting binormal at the hit point, if the vertex format supports this.  Otherwise set to (0,0,0).
	*/
	physx::PxVec3	binormal;
};


/**
\brief Renderable mesh (dynamic data associated with NxRenderMeshAsset)
*/
class NxRenderMeshActor : public NxApexActor, public NxApexRenderable
{
public:

	enum
	{
		InvalidInstanceID = 0xFFFFFFFF
	};

	/**
	\brief Returns the visibilities of all mesh parts in the given array.

	The user must supply the array size.  If the size is less than the
	part count, then the list will be truncated to fit the given array.

	Returns true if any of the visibility values in visibilityArray are changed.
	*/
	virtual bool					getVisibilities( physx::PxU8* visibilityArray, physx::PxU32 visibilityArraySize ) const = 0;

	/**
	\brief Set the visibility of the indexed part.
	*/
	virtual void					setVisibility( bool visible, physx::PxU16 partIndex = 0 ) = 0;

	/**
	\brief Returns the visibility of the indexed part.
	*/
	virtual bool					isVisible( physx::PxU16 partIndex = 0 ) const = 0;

	/**
	\brief Returns the number of visible parts.
	*/
	virtual physx::PxU32			visiblePartCount() const = 0;

	/**
	\brief Returns an array of visible part indices.

	The size of this array is given by visiblePartCount().  Note: the
	indices are in an arbitrary order.
	*/
	virtual const physx::PxU32*		getVisibleParts() const = 0;

	/**
	\brief Sets the local-to-world transform for the indexed bone.  The transform need not be orthonormal.
	*/
	virtual void					setTM( const physx::PxMat44& tm, physx::PxU32 boneIndex = 0 ) = 0;

	/**
	\brief Same as setTM(), but assumes tm is pure rotation.

	This can allow some optimization.  The user must supply scaling
	separately.  The scale vector is interpreted as the diagonal of a
	diagonal matrix, applied before the rotation component of tm.
	*/
	virtual void					setTM( const physx::PxMat44& tm, const physx::PxVec3& scale, physx::PxU32 boneIndex = 0 ) = 0;

	/**
	\brief Update the axis-aligned bounding box which encloses all visible parts in their world-transformed poses.
	*/
	virtual void					updateBounds() = 0;

	/**
	\brief Returns the local-to-world transform for the indexed bone.
	*/
	virtual const physx::PxMat44	getTM( physx::PxU32 partIndex = 0 ) const = 0;

	/**
	\brief If the number of visible parts becomes 0, or if instancing and the number of instances
	becomes 0, then release resources if this bool is true.
	*/
	virtual void					setReleaseResourcesIfNothingToRender( bool value ) = 0;

	/**
	\brief If this set to true, render visibility will not be updated until the user calls syncVisibility().
	*/
	virtual void					setBufferVisibility( bool bufferVisibility ) = 0;

	/**
	\brief Sync render visibility with that set by the user.  Only
	needed if bufferVisibility(true) is called, or bufferVisibility = true
	in the actor's descriptor.
	*/
	virtual void					syncVisibility() = 0;

	/**
	\brief applies the user-provided instance buffer to all submeshes. 
	*/
	virtual void					setInstanceBuffer(NxUserRenderInstanceBuffer* instBuf) = 0;
	/**
	\brief sets the range for the instance buffer

	\param from the position in the buffer (measured in number of elements) to start reading from
	\param count number of instances to be rendered. Must not exceed maxInstances for this actor  
	*/
	virtual void					setInstanceBufferRange(physx::PxU32 from, physx::PxU32 count) = 0;

	/**
		Returns true if and only if a part is hit matching various criteria given in the function
		parameters. If a part is hit, the hitData field contains information about the ray intersection.
		(hitData may be modified even if the function returns false.)
			hitData (output) = information about the mesh at the hitpoint.  See NxRenderMeshActorRaycastHitData.
			worldOrig = the origin of the ray in world space
			worldDisp = the displacement of the ray in world space (need not be normalized)
			flags = raycast control flags (see NxRenderMeshActorRaycastFlags)
			winding = winding filter for hit triangle.  If NxRenderCullMode::CLOCKWISE or
				NxRenderCullMode::COUNTER_CLOCKWISE, then triangles will be assumed to have
				that winding, and will only contribute to the raycast if front-facing.  If
				NxRenderCullMode::NONE, then all triangles will contribute.
			partIndex = If -1, then all mesh parts will be raycast, and the result returned for
				the earliest hit.  Otherwise only the part indexed by partIndex will be raycast.

		N.B. Currently only works for static (unskinned) and one transform per-part, single-weighted vertex skinning.
	*/
	virtual bool					rayCast( NxRenderMeshActorRaycastHitData& hitData,
											 const physx::PxVec3& worldOrig, const physx::PxVec3& worldDisp,
											 NxRenderMeshActorRaycastFlags::Enum flags = NxRenderMeshActorRaycastFlags::VISIBLE_PARTS,
											 NxRenderCullMode::Enum winding = NxRenderCullMode::CLOCKWISE,
											 physx::PxI32 partIndex = -1 ) const = 0;

protected:

	virtual							~NxRenderMeshActor() {}
};

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_RENDERMESHACTOR_H__
