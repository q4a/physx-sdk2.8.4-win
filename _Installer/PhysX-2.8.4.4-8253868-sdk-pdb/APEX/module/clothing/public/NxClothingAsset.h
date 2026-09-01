#ifndef NX_CLOTHING_ASSET_H
#define NX_CLOTHING_ASSET_H
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
#include "NxApexAsset.h"
#include "NxClothingPhysicalMesh.h"

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

class NxClothingActor;
class NxClothingPreview;

#define NX_CLOTHING_AUTHORING_TYPE_NAME "NxClothingAsset"

/**
\brief Stats for an NxClothingAsset: memory usage, counts, etc.
*/
struct NxClothingAssetStats
{
	/// \brief The number of bytes allocated through the clothing asset (not including physical or graphical meshes)
	physx::PxU32						totalBytes;
	/// \brief The number of render mesh assets inside this clothing asset
	physx::PxU32						renderMeshAssetCount;
	/// \brief The accumulated stats of all render mesh assets
	NxRenderMeshAssetStats		renderMeshAssetStats;
	/// \brief The number of physical mesh assets inside this clothing asset
	physx::PxU32						physicalMeshCount;
	/// \brief the accumulated stats of all physical mesh assets
	NxClothingPhysicalMeshStats	physicalMeshStats;

	/// \brief The number of unused simulation data. Only available if some actors have already been released
	physx::PxU32						numUnusedSimulationBulks;
	/// \brief The number of unused collision data. Only available if some actors have already been released
	physx::PxU32						numUnusedNxCollisions;
};


/**
\brief A clothing asset. This is deserialzied from a stream and contains all the static and shared data for a given piece of clothing.
*/
class NxClothingAsset : public NxApexAsset
{
protected:
	// do not manually delete this object
	virtual ~NxClothingAsset() {}
public:
	/**
	\brief Returns the number of NxClothingActors this asset has created.
	*/
	virtual physx::PxU32 getNumActors() const = 0;

	/**
	\brief Returns the index'th NxClothingActor
	*/
	virtual NxClothingActor* getActor(physx::PxU32 index) = 0;

	/**
	\brief Returns the bounding box for the asset.
	*/
	virtual physx::PxBounds3 getBoundingBox() const = 0;

	/**
	\brief returns the number of LOD levels present in this asset
	*/
	virtual physx::PxU32 getNumGraphicalLodLevels() const = 0;

	/**
	\brief returns the actual LOD value of any particular level, normally this is just the identity map
	*/
	virtual physx::PxU32 getGraphicalLodValue(physx::PxU32 lodLevel) const = 0;

	/**
	\brief returns the biggest max distance of any vertex in any physical mesh
	*/
	virtual physx::PxF32 getBiggestMaxDistance() const = 0;

	/**
	\brief remaps bone with given name to a new index for updateState calls

	This is needed when the order of bones differs from authoring tool to runtime

	\note must be called after asset is deserialized. Changing this any later, especially after the first NxClothingActor::updateState call
	will lead to ill-defined behavior.
	*/
	virtual bool remapBoneIndex(const char* name, physx::PxU32 newIndex) = 0;

	/**
	\brief Returns stats (sizes, counts) for the asset.  See NxClothingAssetStats.
	*/
	virtual void getStats( NxClothingAssetStats& stats ) const = 0;

	/**
	\brief Returns the number of bones
	*/
	virtual physx::PxU32 getNumBones() const = 0;

	/**
	\brief Returns the number of bones that are actually used. They are the first ones internally.
	*/
	virtual physx::PxU32 getNumUsedBones() const = 0;

	/**
	\brief Returns the name of the bone at the givne internal index.
	*/
	virtual const char* getBoneName(physx::PxU32 internalIndex) const = 0;

	/**
	\brief Returns the bind pose transform for this bone.
	*/
	virtual bool getBoneBasePose(physx::PxU32 internalIndex, physx::PxMat44& result) const = 0;

	/**
	\brief returns the mapping from internal to external indices for a given asset.

	Use this map to transform all the boneIndices returned from the ClothingPhysicalMesh into the order you specified initially
	*/
	virtual void getBoneMapping(physx::PxU32* internal2externalMap) const = 0;

	/**
	\brief Gets the NxRenderMeshAsset associated with this asset.

	\param [in] lodLevel	The LoD level of the render mesh asset
	\return NULL if lodLevel is not valid
	\see NxClothingAsset::getNumGraphicalLodLevels()
	*/
	virtual const NxRenderMeshAsset*		getRenderMeshAsset(physx::PxU32 lodLevel) const = 0;
};

PX_POP_PACK

}} // namespace physx::apex

#endif
