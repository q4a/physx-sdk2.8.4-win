#ifndef NX_CLOTHING_ASSET_AUTHORING_H
#define NX_CLOTHING_ASSET_AUTHORING_H
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
#include "NxClothingAsset.h"
#include "NxModuleClothing.h"

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT


class NxClothingPhysicalMesh;
class NxClothingIsoMesh;


/**
\brief The clothing authoring asset. This is used to generate streams that can then be deserialized into a regular asset.
*/
class NxClothingAssetAuthoring : public NxApexAssetAuthoring
{
public:

	/**
	\brief set the default value for all coefficients if none is available
	*/
	virtual void setDefaultConstrainCoefficients(const NxClothConstrainCoefficients& coeff) = 0;

	/**
	\brief set the value which is considered invalid when painting
	*/
	virtual void setInvalidConstrainCoefficients(const NxClothConstrainCoefficients& coeff) = 0;

	/**
	\brief Sets the graphcial and physical mesh for the given graphical lod level.
	\param [in] lod						Identifier for that lod level.
	\param [in] graphicalMesh			The graphical mesh to be rendered in that lod level.
	\param [in] physicalMesh			The physical mesh to be simulated on that lod level. The reference to the same physical mesh can be
										given for different graphical lods. It is copied internally.
										<br><b>Note:</b> If the same physical mesh is used several time, the paint values are copied from the
										graphical mesh the first time the physical mesh is passed in here.
	\param [in] numMaxDistReductions	The number of physical submeshes that are created from the current submesh.
										<br><b>Note:</b> If the same physical mesh is used several time, the submeshes are created with the
										number of the first time it has been passed in.
	\param [in] maxDistReductions		For each physicalLod it can be defined how much the maxDistances are reduced when that LoD is active.
										All triangles that have vertices reduced down to 0 or smaller are not simulated anymore. If no list
										with distances is provided, the mesh will be split into approximately equally large triangle patches.
	\param [in] normalResemblance		This angle (in degrees) specifies how similar the normal from the physical and graphical mesh need to
										be in order for them to be accepted as immediate mode skinning pairs. Use 90 or bigger for any
										resemblance. Must be in range (0, 90], but it's highly recommended to use more than 5 degrees.
	\param [in] ignoreUnusedVertices	All Vertices that have the 'usedForPhysics' property set to false will be ignored when transfering
										the painted values from the graphical to the physical mesh
	\param [in] fasterMeshMeshSkinning	A more leightweight and faster mesh-mesh skinning algorithm is used. It should be equivalent or better
										in quality as well.
	\param [in] progress				Progress bar callback class.
	*/
	virtual void setMeshes(physx::PxU32 lod, NxRenderMeshAssetAuthoring* graphicalMesh, NxClothingPhysicalMesh* physicalMesh,
		physx::PxU32 numMaxDistReductions, physx::PxF32* maxDistReductions = NULL, physx::PxF32 normalResemblance = 25, bool ignoreUnusedVertices = true,
		bool fasterMeshMeshSkinning = true, IProgressListener* progress = NULL) = 0;


	/**
	\brief Returns the number of LoDs that have been set in the NxClothingAssetAuthoring::setMeshes call
	*/
	virtual physx::PxU32 getNumLods() = 0;
	
	/**
	\brief Returns the n-th LoD value from the NxClothingAssetAuthoring::setMeshes call.

	\note These values do not have to be a continuous list
	*/
	virtual physx::PxI32 getLodValue(physx::PxU32 lod) = 0;

	/**
	\brief Clears all lods and meshes.
	*/
	virtual void clearMeshes() = 0;
	
	/**
	\brief Returns a reference to the internal copy of the physical mesh for the given graphical lod. Returned object MUST be destroyed by the user immediately after use.
	*/
	virtual NxClothingPhysicalMesh* getClothingPhysicalMesh(physx::PxU32 graphicalLod) = 0;

	/**
	\brief assigns name and bindpose to a bone. This needs to be called for every bone at least once

	\param [in] boneIndex	The bone index the information is provided for.
	\param [in] boneName		The bone name. An internal copy of this string will be made.
	\param [in] bindPose		The bind or rest pose of this bone.
	\param [in] parentIndex	The bone index of the parent bone, -1 for root bones.
	*/
	virtual void setBoneInfo(physx::PxU32 boneIndex, const char* boneName, const physx::PxMat44& bindPose, physx::PxI32 parentIndex) = 0;

	/**
	\brief add a convex collision representation to a bone, vertices must be in bone-bind-pose space!
	
	\param [in] boneName		name of the bone this collision info is for.
	\param [in] positions		array of vertices with the positions.
	\param [in] numPositions	number of elements in positions array. if set to 0, bone collision will be capsule.
	*/
	virtual physx::PxU32 addBoneConvex(const char* boneName, const physx::PxVec3* positions, physx::PxU32 numPositions) = 0;

	/**
	\brief add a convex collision representation to a bone, vertices must be in bone-bind-pose space!

	\param [in] boneIndex		index of the bone this collision info is for.
	\param [in] positions		array of vertices with the positions.
	\param [in] numPositions	number of elements in positions array. if set to 0, bone collision will be capsule.
	*/
	virtual physx::PxU32 addBoneConvex(physx::PxU32 boneIndex, const physx::PxVec3* positions, physx::PxU32 numPositions) = 0;

	/**
	\brief add a capsule to the bone's collision. all relative to bone-bind-pose space!

	\param [in] boneName		name of the bone this collision info is for.
	\param [in] capsuleRadius	The radius of the capsule. if set to 0, bone collision will default back to convex
	\param [in] capsuleHeight	The height of the capsule
	\param [in] localPose		The frame of the capsule relative to the frame of the bone (not world space)
	*/
	virtual void addBoneCapsule(const char* boneName, physx::PxF32 capsuleRadius, physx::PxF32 capsuleHeight, const physx::PxMat44& localPose) = 0;

	/**
	\brief add a capsule to the bone's collision. all relative to bone-bind-pose space!

	\param [in] boneIndex		index of the bone this collision info is for.
	\param [in] capsuleRadius	if set to 0, bone collision will default back to convex
	\param [in] capsuleHeight	The height of the capsule
	\param [in] localPose		The frame of the capsule relative to the frame of the bone (not world space)
	*/
	virtual void addBoneCapsule(physx::PxU32 boneIndex, physx::PxF32 capsuleRadius, physx::PxF32 capsuleHeight, const physx::PxMat44& localPose) = 0;

	/**
	\brief clear all collision information for a given bone name
	*/
	virtual void clearBoneActors(const char* boneName) = 0;
	
	/**
	\brief clear all collision information for a given bone index
	*/
	virtual void clearBoneActors(physx::PxU32 boneIndex) = 0;

	/**
	\brief clear all collision information for any bone
	*/
	virtual void clearAllBoneActors() = 0;

	/**
	\brief Number of hierarchical levels. 0 to turn off

	If either the hierarchical solver iterations or the hierarchical levels are set to 0, this feature is turned off
	*/
	virtual void setSimulationHierarchicalLevels(physx::PxU32 levels) = 0;


	/**
	\brief The radius of the cloth/softbody particles.
	*/
	virtual void setSimulationThickness(physx::PxF32 thickness) = 0;

	/**
	\brief The radius of the cloth/softbody particles for the purpose of self-collision.
	*/
	virtual void setSimulationSelfcollisionThickness(physx::PxF32 thickness) = 0;

	/**
	\brief The sleep velocity. If all vertices of a cloth/softbody are below this velocity for some time, it falls asleep
	*/
	virtual void setSimulationSleepLinearVelocity(physx::PxF32 sleep) = 0;

	/**
	\brief Turn off Continuous collision detection for clothing.

	Fast moving objects can cause massive forces to the cloth, especially when some kinematic actors move large distances within one frame.
	Without CCD those collisions are not detected and thus don't influence the cloth.

	\note Whenever the isContinuous parameter in NxClothingActor::updateState is set to false, cloth CCD will be temporarily disabled as well
	*/
	virtual void setSimulationDisableCCD(bool disable) = 0;

	/**
	\brief Turns on self collision. This prevents two vertices coming closer than their thickness allows, and thus preventing from tunneling
	if configured correctly.

	\note This comes with a performance penalty
	*/
	virtual void setSimulationSelfcollision(bool enable) = 0;

	/**
	\brief Turns on twoway interaction with rigid body. Only of limited use for clothing
	*/
	virtual void setSimulationTwowayInteraction(bool enable) = 0;

	/**
	\brief EXPERIMENTAL: Turns on local untangling

	\see NxClothDesc.flags NX_CLF_UNTANGLING
	*/
	virtual void setSimulationUntangling(bool enable) = 0;

	/**
	\brief Turn on target FPS for this piece of clothing

	\param [in] frequency	must be either 0 or in a sane range > 10 Hz
	*/
	virtual void setSimulationTargetFrequency(PxF32 frequency) = 0;

	/**
	\brief Provide a scaling to the serialize functionality.

	This is useful when the game runs in a different scale than the tool this asset was created in

	\deprecated Only works with deprecated serialization, use applyTransformation instead!
	*/
	virtual void setExportScale(physx::PxF32 scale) = 0;

	/**
	\brief Apply an arbitrary transformation to either the physical, the graphical mesh or both.

	This is a permanent transformation and it changes the object state. Should only be called immediately before serialization
	and without further modifying the object later on.

	\param transformation	This matrix is allowed to contain a translation and a rotation.
	\param scale			Apply a uniform scaling as well
	\param applyToGraphics	Apply the transformation and scale to the graphical part of the asset
	\param applyToPhysics	Apply the transformation and scale to the physical part of the asset

	\note if the 3x3 part of the transformation parameter contains mirroring of axis (= negative determinant) some additional processing is done.
	This includes flipping of simulation triangles and adaption of the mesh-mesh skinning.
	*/
	virtual void applyTransformation(const physx::PxMat44& transformation, physx::PxF32 scale, bool applyToGraphics, bool applyToPhysics) = 0;

	/**
	\brief Move every bone bind pose to a new position.

	\param newBindPoses					array of matrices for the new bind poses. This must be in object space (not relative to the parent bone!)
	\param newBindPosesCount			number of matrices in the specified array
	\param isInternalOrder				using internal numbering or external numbering?
	\param maintainCollisionWorldSpace	The collision volumes do not move to the new bone bind pose
	*/
	virtual void updateBindPoses(const physx::PxMat44* newBindPoses, physx::PxU32 newBindPosesCount, bool isInternalOrder, bool maintainCollisionWorldSpace) = 0;

	/**
	\brief If enabled, the normals of the physical mesh are derived
	from the directions of the physical vertex to the bind poses of the assigned bones
	*/
	virtual void setDeriveNormalsFromBones(bool enable) = 0;

	/**
	\brief Returns the material interface
	*/
	virtual NxParameterized::Interface* getMaterialLibrary() = 0;

	/**
	\brief Adds a new material library to the asset

	\param [in] materialLibrary		User-generated material library object.
	\param [in] materialIndex		Default value for the Actor to know which material to use in the library
	\param [in] transferOwnership	Hands ownership to the authoring asset.
	*/
	virtual bool setMaterialLibrary(NxParameterized::Interface* materialLibrary, physx::PxU32 materialIndex, bool transferOwnership) = 0;

	/**
	\brief Returns stats (sizes, counts) for the asset.  See NxClothingAssetStats.
	*/
	virtual void getStats( NxClothingAssetStats& stats ) const = 0;

	/**
	\brief Gets the parameterized render mesh associated with this asset.

	\param [in] lodLevel	The LoD level of the render mesh asset

	\return NULL if lodLevel is not valid

	\see NxClothingAssetAuthoring::getNumLods()

	\note Under no circumstances you should modify the positions of the render mesh asset's vertices. This will certainly break clothing
	*/
	virtual ::NxParameterized::Interface*	getRenderMeshAssetAuthoring(physx::PxU32 lodLevel) = 0;

	/**
	\brief Retrieves the bind pose transform of this bone.
	*/
	virtual	bool getBoneBindPose(physx::PxU32 boneIndex, physx::PxMat44& bindPose) = 0;

	/**
	\brief Directly sets the bind pose transform of this bone
	*/
	virtual bool setBoneBindPose(physx::PxU32 boneIndex,const physx::PxMat44& bindPose) = 0;
};

PX_POP_PACK

}} // namespace physx::apex

#endif // __NX_CLOTHING_ASSET_AUTHORING_H__
