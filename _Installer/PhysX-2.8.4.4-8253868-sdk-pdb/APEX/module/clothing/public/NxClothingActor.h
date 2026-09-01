#ifndef NX_CLOTHING_ACTOR_H
#define NX_CLOTHING_ACTOR_H
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
#include "NxApexDesc.h"
#include "NxClothingUserRecompute.h"
#include "NxClothingVelocityCallback.h"

class NxClothDesc;
class NxRay;

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
\brief \b DEPRECATED Various flags that can be queried and set at runtime for each NxClothingActor

\deprecated Use the NxParameterized Clothing Actor Descriptor to modify these values
*/
struct NxApexClothingFlag
{
	///Container for the flag
	enum Enum
	{
		/**
		\brief Mesh skinning for physics is done during simulation instead of before.
		This will cause the simulation to lag one frame behind
		<br><b>Default:</b> off

		\see NxClothingActor::getFrameDelay()

		*/
		PARALLEL_PHYSX_MESH_SKINNING						= 1<<0,
		
		/**
		\brief Mesh-Mesh skinning from physical to graphical mesh is done during next simulation step and not after the current one.
		This will cause the simulation to lag one frame behind
		<br><b>Default:</b> off

		\see NxClothingActor::getFrameDelay()
		*/
		PARALLEL_MESH_MESH_SKINNING							= 1<<1,

		/**
		\brief Enable debug visualization for this particular actor
		<br><b>Default:</b> on
		*/
		VISUALIZE											= 1<<2,

		/**
		\brief All graphical vertices without correspondence to physical vertices or triangles are skinned normally.
		This flag specifies whether this happens during Physics scene simulation, or after.

		\note If this flag is set, an inconsistency can arise when calling NxClothingActor::updateRenderResource in between NxApexScene::simulate
		and NxApexScene::fetchResults. As a workaround, you should only call NxClothingActor::updateRenderResources <i>after</i>
		NxApexScene::fetchResults has terminated
		<br><b>Default:</b> on
		*/
		PARALLEL_CPU_SKINNING								= 1<<3,

		/**
		\brief Recompute all normals after mesh-mesh skinning is complete

		\note This usually leads to better looking results, but is more expensive to compute
		<br><b>Default:</b> off
		*/
		RECOMPUTE_NORMALS									= 1<<4,
	};
};



/**
\brief Instance of NxClothingAsset. Can be positioned, animated, updated and rendered.
*/
class NxClothingActor : public NxApexActor, public NxApexRenderable
{
protected:
	virtual ~NxClothingActor() {}
public:

	/**
	\brief Returns a reference to the actor descriptor as it is held by the ClothingActor.

	This descriptor can be modified at any time, changes will only be read during the first part of the simulation.
	*/
	virtual ::NxParameterized::Interface* getActorDesc() = 0;

	/**
	\brief \b DEPRECATED Set the template for clothing.

	\deprecated Use NxClothingActor::getActorDesc() and modify the clothingTemplate part of it
	\note This can be called while the simulation is running, but will only affect simulation after the next NxApexScene::simulate() call
	*/
	virtual void setClothingTemplate(const NxClothDesc* desc) = 0;

	/**
	\brief \b DEPRECATED Gets the template for clothing

	\deprecated Use NxClothingActor::getActorDesc() and modify the clothingTemplate part of it
	\return false if no template has been set
	*/
	virtual bool getClothingTemplate(NxClothDesc& desc) const = 0;

	enum TeleportMode
	{
		TM_Continuous,
		TM_Teleport,
		TM_TeleportAndReset,
	};

	/**
	\brief Updates all internal bone matrices. This should be called with updated information before apex scenes start simulating.

	\param[in] globalPose				The new location of the actor
	\param[in] newBoneMatrices			Pointer to the array of transformations that contain the composite bone transformations for the
										current frame
	\param[in] boneMatricesByteStride	stride of the bone matrices, must be bigger than ::sizeof(physx::PxMat44)
	\param[in] numBoneMatrices			number of bone matrices available. This should correspond with the number of bones present in the asset
	\param[in] teleportMode				Setting this to false will force the cloth to the animated position for the next frame.
										used for teleportations and non-smooth animations mainly.

	\note This must be called before NxApexScene::simulate is called
	*/
	virtual void updateState(const physx::PxMat44& globalPose, const physx::PxMat44* newBoneMatrices, physx::PxU32 boneMatricesByteStride, physx::PxU32 numBoneMatrices, TeleportMode teleportMode) = 0;

	/**
	\brief Change the max distance of all active vertices with a scalar parameter

	\param[in] scale         Must be in [0,1] range
	\param[in] multipliable  Setting to define how the scale is applied. True will mutliply the scale on top of the max distance, False will subtract the maximum max distance.
	*/
	virtual void updateMaxDistanceScale(physx::PxF32 scale, bool multipliable) = 0;

	/**
	\brief returns the globalPose that was set with NxClothingActor::updateState()
	*/
	virtual const physx::PxMat44& getGlobalPose() const = 0;

	/**
	\brief Sets the wind strength and direction, can be called any time

	\param[in] windAdaption The rate of adaption. The higher this value, the faster the cloth reaches the wind velocity. Set to 0 to turn off wind
	\param[in] windVelocity The target velocity each vertex tries to achieve.

	\note It is safe to call this even during simulation, but it will only have an effect after the next call to NxApexScene::simulate()
	*/
	virtual void setWind(physx::PxF32 windAdaption, const physx::PxVec3& windVelocity) = 0;

	/**
	\brief \b DEPRECATED Time in seconds how long it takes to go from 0 maxDistance to full maxDistance

	\deprecated Use NxClothingActor::getActorDesc() and modify the maxDistanceBlendTime part of it
	<b>Default:</b> 1.0
	\note This also influences how quickly different physical LoDs can be switched
	\note It is safe to call this even during simulation.
	*/
	virtual void setMaxDistanceBlendTime(physx::PxF32 blendTime) = 0;

	/**
	\brief \b DEPRECATED Time in seconds how long it takes to go from 0 maxDistance to full maxDistance
	
	\deprecated Use NxClothingActor::getActorDesc() and read the maxDistanceBlendTime part of it
	*/
	virtual physx::PxF32 getMaxDistanceBlendTime() const = 0;

	/**
	\brief \b DEPRECATED sets or clears flag.

	\deprecated Use NxClothingActor::getActorDesc() and modify the flag part of it
	\note It is safe to call this even during simulation.
	*/
	virtual void setFlag(NxApexClothingFlag::Enum flag, bool on) = 0;

	/**
	\brief \b DEPRECATED gets the current value of a flag.

	\deprecated Use NxClothingActor::getActorDesc() and read the flag part of it
	*/
	virtual bool getFlag(NxApexClothingFlag::Enum flag) = 0;

	/**
	\brief returns the current numbers of frames that the output is delayed from the animation input.
	This frame delay depends on the flags for parallel skinning and is in the range of [0, 2].
	*/
	virtual physx::PxU32 getFrameDelay() = 0;

	/**
	\brief Tells the actor if it will be rendered or not.
	If an actor is simulated, but not rendered, some computations (skinning, normal and tangent calculation)
	doesn't need to be done.

	disabling is set immediately, disabling is buffered for the next frame.
	*/
	virtual void setVisible(bool enable) = 0;

	/**
	\brief Returns the current visibility setting.
	The most recently set value is returned (i.e. the buffered value, not the actual value).
	*/
	virtual bool isVisible() const = 0;

	/**
	\brief Stops simulating the actor.
	*/
	virtual void setFrozen(bool enable) = 0;

	/**
	\brief Returns if the simulation is currently stopped for this actor.
	*/
	virtual bool isFrozen() const = 0;

	/**
	\brief \b DEPRECATED Sets LOD parameters that control how the distance from the eye position should influence the benefit of an object.

	The LOD benefit function for cloth is: 

	benefit = (distanceWeight * max(0, maxDistance - distanceFromEye) + bias) * simulationCost + benefitBias

	\param [in] maxDistance		the benefit of an actor should fall off linearly with eye-distance from zero to this maxDistance.
	\param [in] distanceWeight	the weight of the distance-benefit in the benefit function
	\param [in] bias			bias of the distance formula
	\param [in] benefitBias		bias of the overall benefit

	\deprecated Use NxClothingActor::getActorDesc() and modify the lodWeights part of it
	\note It is safe to call this even during simulation
	*/
	virtual void setLODWeights(physx::PxF32 maxDistance, physx::PxF32 distanceWeight, physx::PxF32 bias, physx::PxF32 benefitBias) = 0;

	/**
	\brief sets the graphical Lod
	This chooses the graphical mesh of all the meshes stored in the asset to be rendered. It has to be set
	before the simulate call to take effect for the next time the actor is rendered. Otherwise, the
	given value will be buffered and used as soon as possible.

	\param [in] lod		lod used to render the mesh

	\note It is safe to call this even during simulation
	*/
	virtual void setGraphicalLOD(physx::PxU32 lod) = 0;

	/**
	\brief returns the graphical Lod
	This returns the buffered most recently set graphical lod, even if it's not active yet.
	*/
	virtual PxU32 getGraphicalLod() = 0;

	/**
	\brief Raycasts against the NxClothingActor

	\param [in] worldRay	The world ray, direction needs not to be normalized
	\param [out] time		Impact time
	\param [out] normal		Impact normal in world space

	\return true if this actor is hit
	*/
	virtual bool rayCast(const NxRay& worldRay, physx::PxF32& time, physx::PxVec3& normal) = 0;

	/**
	\brief Returns the actively selected material.
	*/
	virtual physx::PxU32 getClothingMaterial() const = 0;

	/**
	\brief Sets which clothing material is used from the assets library
	*/
	virtual void setClothingMaterial(physx::PxU32 index) = 0;

	/**
	\brief sets the recompute callback for an individual actor, can be NULL to use default.
	*/
	virtual void setUserRecompute(NxClothingUserRecompute* recompute) = 0;

	/**
	\brief sets the velocity callback for an individual actor, turned off when NULL
	*/
	virtual void setVelocityCallback(NxClothingVelocityCallback* callback) = 0;

	/**
	\brief This method will return the position of all vertices that have come out of the physics simulation

	\note This should usually not be needed other than for rendering the physics mesh yourself
	\note Must be called in between NxApexScene::fetchResults and NxApexScene::simulate.
	*/
	virtual void getSimulationVertices(void* buffer, physx::PxU32 byteStride) = 0;

	/**
	\brief This method will return the normals of all vertices that have come out of the physics simulation

	\note This should usually not be needed other than for rendering the physics mesh yourself
	\note Must be called in between NxApexScene::fetchResults and NxApexScene::simulate.
	*/
	virtual void getSimulationNormals(void* buffer, physx::PxU32 byteStride) = 0;

	/**
	\brief Returns how much an Actor will cost at maximum
	*/
	virtual physx::PxU32 getMaximumSimulationBudget() const = 0;
};

PX_POP_PACK

}} // namespace physx::apex

#endif
