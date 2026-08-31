#ifndef PXS_FLUID_H
#define PXS_FLUID_H


#include "PxsFluidConfig.h"
#ifdef PXS_ENABLE_FLUIDS

#include "PxnFluid.h"
#include "PxcArray.h"
#include "PxcVector.h"
#include "PxcSmallArray.h"
#include "PxcBitMap.h"
#include "PxcGridCellVector.h"
#include "PxsFluidParticle.h"
#include "PxsFluidDynamics.h"
#include "PxsFluidCollision.h"

class PxsFluidSpatialHash;
class PxsContext;
class PxsFluidShape;


class PxsFluid : public PxnFluid
{
public:
											PxsFluid(PxsContext* context, PxU32 index);
	virtual									~PxsFluid();

						void				init(const PxdFluidDesc& desc);

	//---------------------------
	// Implements PxnFluid
	virtual				void				addParticlesV(PxUInt count, PxdFluidParticleDataDesc* particles);
	virtual				void				removeAllParticlesV();
	virtual				PxU32				getDeletedParticleIdsV(const PxU16** particleIds)	const;
	virtual				PxU32				getParticleCountV()	const;
	virtual				PxU32				getParticlesV(PxdFluidParticleDataDesc* particles)	const;
	virtual				void				updateParticlesV(PxUInt intervalCount, const PxdFluidParticleUpdateInterval* updateIntervals, PxdFluidParticleUpdateDataDesc* updateData);

	virtual				void				getShapesUpdateV(PxdFluidShapeUpdateResults* updateResults) const;

	virtual				PxdTask				packetShapesUpdateV();
	virtual				PxdTask				dynamicsUpdateV();
	virtual				PxdTask				collisionUpdateV();

	// Fluid parameters
	virtual				PxU32				getMaxParticlesV()							const;

	virtual				PxFloat				getRestParticlesPerUnitV()					const;

	virtual				PxFloat				getRestDensityV()							const;

	virtual				PxFloat				getSelfDensityV()							const;

	virtual				PxFloat				getKernelRadiusMultiplierV()				const;

	virtual				PxFloat				getMotionLimitMultiplierV()					const;

	virtual				PxFloat				getCollisionDistanceMultiplierV()			const;

	virtual				PxU32				getPacketSizeMultiplierLog2V()				const;

	virtual				void				setStiffnessV(PxFloat value);
	virtual				PxFloat				getStiffnessV()								const;

	virtual				void				setViscosityV(PxFloat value);
	virtual				PxFloat				getViscosityV()								const;

	virtual				void				setSurfaceTensionV(PxFloat value);
	virtual				PxFloat				getSurfaceTensionV()						const;

	virtual				void				setDampingV(PxFloat value);
	virtual				PxFloat				getDampingV()								const;

	virtual				void				setFadeInTimeV(PxFloat value);
	virtual				PxFloat				getFadeInTimeV()							const;

	virtual				void				setExternalAccelerationV(const PxdVector& v);
	virtual				const PxdVector&	getExternalAccelerationV()					const;

	virtual				void				setStaticCollisionRestitutionV(PxFloat value);
	virtual				PxFloat				getStaticCollisionRestitutionV()			const;

	virtual				void				setStaticCollisionAdhesionV(PxFloat value);
	virtual				PxFloat				getStaticCollisionAdhesionV()				const;

	virtual				void				setStaticCollisionAttractionV(PxFloat value);
	virtual				PxFloat				getStaticCollisionAttractionV()				const;

	virtual				void				setDynamicCollisionRestitutionV(PxFloat value);
	virtual				PxFloat				getDynamicCollisionRestitutionV()			const;

	virtual				void				setDynamicCollisionAdhesionV(PxFloat value);
	virtual				PxFloat				getDynamicCollisionAdhesionV()				const;

	virtual				void				setDynamicCollisionAttractionV(PxFloat value);
	virtual				PxFloat				getDynamicCollisionAttractionV()			const;

	virtual				void				setCollisionResponseCoefficientV(PxFloat value);
	virtual				PxFloat				getCollisionResponseCoefficientV()			const;

	virtual				void				setSimulationTimeStepV(PxFloat value);
	virtual				PxFloat				getSimulationTimeStepV()					const;

	virtual				void				setSimulationMethodV(PxdFluidSimulationMethod value);
	virtual				PxdFluidSimulationMethod getSimulationMethodV()					const;

	virtual				void				setProjectionPlaneV(const PxdPlane& p);
	virtual				PxdPlane			getProjectionPlaneV()						const;

	virtual				void				setFlagsV(PxU32 value);
	virtual				PxU32				getFlagsV()									const;

	virtual				void				activateV();
	virtual				void				deactivateV();
	virtual				bool				isActiveV()									const;

	virtual				PxdHandle			getHandleV()								const;
	virtual				void				destroyV();
	//~Implements PxnFluid
	//---------------------------

	PXD_FORCE_INLINE	PxU32				getIndex()									const { return mIndex; }

	PXD_FORCE_INLINE	PxsContext*			getContext()								const { return mContext; }

	PXD_FORCE_INLINE	void				addFluidShape(PxsFluidShape* fluidShape);
	PXD_FORCE_INLINE	void				removeFluidShape(PxsFluidShape* fluidShape);

	PXD_FORCE_INLINE	PxU32				getFluidShapeCount() const { return mFluidShapes.getSize(); }
	PXD_FORCE_INLINE	PxsFluidShape**		getFluidShapes() { return &mFluidShapes[0]; }

	PXD_FORCE_INLINE	void				getPacketCenterExtent(const PxcGridCellVector& coord, PxcVector& origin, PxcVector& extent);

	PXD_FORCE_INLINE	PxFloat				computeViscosityMultiplier(PxFloat viscosityStd, PxFloat particleMassStd, PxFloat radius6Std);

private:
						void				remapShapesToPackets();

						void				clearParticleConstraints();

private:
						PxsContext*				mContext;
						PxU32					mIndex;

						PxcArray<PxU16>			mDeletedParticleIds;	// IDs of deleted particles.

						PxcArray<PxsFluidShape*> mFluidShapes;	// The fluid packet shapes.
						PxcArray<PxdShape>		mCreatedDeletedFluidShapes;	// Handles of created and deleted fluid packet shapes.
						PxU32					mNumCreatedFluidShapes;
						PxU32					mNumDeletedFluidShapes;
						PxcBitMap				mMappedFluidPackets;	// Marks the fluid packets that are mapped to a fluid shape

						PxsFluidParticleArray	mParticleBuffer[2];	// Swap buffers for the fluid particles
						PxU8					mParticleReadBufferIdx;	// Index of the particle buffer which is
																		// active (double buffering)

						PxsFluidConstraintPair* mConstraintBuffer[2];	// Swap buffers for fluid particle constraints.
						PxU8					mConstraintReadBufferIdx;	// Index of the constraint buffer which is
																			// active (double buffering)

						// Spatial ordering, packet generation
						PxsFluidSpatialHash*	mSpatialHash;

						// Dynamics update
						PxsFluidDynamics		mDynamics;

						// Collision update
						PxsFluidCollision		mCollision;

						PxU16					mMaxParticles;
						
						PxFloat					mRestParticlesPerUnit;
						PxFloat					mKernelRadiusMultiplier;
						PxFloat					mMotionLimitMultiplier;
						PxFloat					mCollisionDistanceMultiplier;
						PxFloat					mViscosity;
						PxFloat					mSurfaceTension;
						PxFloat					mFadeInTime;
						PxFloat					mCollisionRange;
						PxFloat					mSimulationTimeStep;
						PxdFluidSimulationMethod	mSimulationMethod;
						PxU32					mFlags;
						//~Fluid parameters

						bool					mIsActive;
};

PXD_FORCE_INLINE void PxsFluid::addFluidShape(PxsFluidShape* fluidShape)
{
	PXN_ASSERT(fluidShape != NULL);

	mFluidShapes.insert(fluidShape);
}

PXD_FORCE_INLINE void PxsFluid::removeFluidShape(PxsFluidShape* fluidShape)
{
	PXN_ASSERT(fluidShape != NULL);

	// TODO: Optimize (put index in fluid shape but don't forget to adjust index for swapped entry)
	PxUInt nbShapes = mFluidShapes.getSize();
	for(PxUInt i=0; i < nbShapes; i++)
	{
		if (mFluidShapes[i] == fluidShape)
		{
			mFluidShapes.removeFast(i);
			return;
		}
	}
}

/*!
Compute AABB of a packet given its coordinates.
*/
PXD_FORCE_INLINE void PxsFluid::getPacketCenterExtent(const PxcGridCellVector& coord, PxcVector& origin, PxcVector& extent)
{
	extent.set(mCollision.mParams.packetSize);
	extent *= 0.5;

	origin.x = static_cast<PxFloat>(coord.x);
	origin.y = static_cast<PxFloat>(coord.y);
	origin.z = static_cast<PxFloat>(coord.z);

	origin *= mCollision.mParams.packetSize;
	origin += extent;

	PxcVector collisionRangeVec;
	collisionRangeVec.set(mCollisionRange);
	extent += collisionRangeVec;
	// Enlarge the bounding box such that a particle on the current boundary could
	// travel the maximum distance and would still be inside the enlarged volume.
}

PXD_FORCE_INLINE PxFloat PxsFluid::computeViscosityMultiplier(PxFloat viscosityStd, PxFloat particleMassStd, PxFloat radius6Std)
{
	PxFloat wViscosityLaplacianScalarStd = 45.0f / (PXD_PI * radius6Std);
	return (wViscosityLaplacianScalarStd * viscosityStd * particleMassStd);
}


#endif	// PXS_ENABLE_FLUIDS

#endif
