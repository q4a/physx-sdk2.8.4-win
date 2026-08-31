#ifndef PXN_FLUID_H
#define PXN_FLUID_H

#include "px_context.h"
#include "px_config.h"

#include "px_fluid.h"
#include "px_geometry.h"
#include "PxnError.h"
#include "PxcVector.h"


// Standard value for particle resolution
#define PXN_FLUID_REST_PARTICLE_PER_UNIT_STD 10.0f


class PxnFluid
{
public:
	virtual				void				addParticlesV(PxUInt count, PxdFluidParticleDataDesc* particles) = 0;
	virtual				void				removeAllParticlesV()									= 0;
	virtual				PxU32				getDeletedParticleIdsV(const PxU16** particleIds)	const	= 0;
	virtual				PxU32				getParticleCountV()	const = 0;
	virtual				PxU32				getParticlesV(PxdFluidParticleDataDesc* particles)	const	= 0;
	virtual				void				updateParticlesV(PxUInt intervalCount, const PxdFluidParticleUpdateInterval* updateIntervals, PxdFluidParticleUpdateDataDesc* updateData) = 0;

	virtual				void				getShapesUpdateV(PxdFluidShapeUpdateResults* updateResults) const = 0;

	virtual				PxdTask				packetShapesUpdateV()									= 0;
	virtual				PxdTask				dynamicsUpdateV()										= 0;
	virtual				PxdTask				collisionUpdateV()										= 0;

	// Fluid parameters
	virtual				PxU32				getMaxParticlesV()							const		= 0;

	virtual				PxFloat				getRestParticlesPerUnitV()					const		= 0;

	virtual				PxFloat				getRestDensityV()							const		= 0;

	virtual				PxFloat				getSelfDensityV()							const		= 0;

	virtual				PxFloat				getKernelRadiusMultiplierV()				const		= 0;

	virtual				PxFloat				getMotionLimitMultiplierV()					const		= 0;

	virtual				PxFloat				getCollisionDistanceMultiplierV()			const		= 0;

	virtual				PxU32				getPacketSizeMultiplierLog2V()				const		= 0;

	virtual				void				setStiffnessV(PxFloat value)							= 0;
	virtual				PxFloat				getStiffnessV()								const		= 0;

	virtual				void				setViscosityV(PxFloat value)							= 0;
	virtual				PxFloat				getViscosityV()								const		= 0;

	virtual				void				setSurfaceTensionV(PxFloat value)						= 0;
	virtual				PxFloat				getSurfaceTensionV()						const		= 0;

	virtual				void				setDampingV(PxFloat value)								= 0;
	virtual				PxFloat				getDampingV()								const		= 0;

	virtual				void				setFadeInTimeV(PxFloat value)							= 0;
	virtual				PxFloat				getFadeInTimeV()							const		= 0;

	virtual				void				setExternalAccelerationV(const PxdVector& v)			= 0;
	virtual				const PxdVector&	getExternalAccelerationV()					const		= 0;

	virtual				void				setStaticCollisionRestitutionV(PxFloat value)			= 0;
	virtual				PxFloat				getStaticCollisionRestitutionV()			const		= 0;

	virtual				void				setStaticCollisionAdhesionV(PxFloat value)				= 0;
	virtual				PxFloat				getStaticCollisionAdhesionV()				const		= 0;

	virtual				void				setStaticCollisionAttractionV(PxFloat value)			= 0;
	virtual				PxFloat				getStaticCollisionAttractionV()				const		= 0;

	virtual				void				setDynamicCollisionRestitutionV(PxFloat value)			= 0;
	virtual				PxFloat				getDynamicCollisionRestitutionV()			const		= 0;

	virtual				void				setDynamicCollisionAdhesionV(PxFloat value)				= 0;
	virtual				PxFloat				getDynamicCollisionAdhesionV()				const		= 0;

	virtual				void				setDynamicCollisionAttractionV(PxFloat value)			= 0;
	virtual				PxFloat				getDynamicCollisionAttractionV()			const		= 0;

	virtual				void				setCollisionResponseCoefficientV(PxFloat value)			= 0;
	virtual				PxFloat				getCollisionResponseCoefficientV()			const		= 0;

	virtual				void				setSimulationTimeStepV(PxFloat value)					= 0;
	virtual				PxFloat				getSimulationTimeStepV()					const		= 0;

	virtual				void				setSimulationMethodV(PxdFluidSimulationMethod value)	= 0;
	virtual				PxdFluidSimulationMethod getSimulationMethodV()					const		= 0;

	virtual				void				setProjectionPlaneV(const PxdPlane& p)					= 0;
	virtual				PxdPlane			getProjectionPlaneV()						const		= 0;

	virtual				void				setFlagsV(PxU32 value)									= 0;
	virtual				PxU32				getFlagsV()									const		= 0;

	virtual				void				activateV()												= 0;
	virtual				void				deactivateV()											= 0;
	virtual				bool				isActiveV()									const		= 0;

	virtual				void				destroyV()												= 0;
	virtual				PxdHandle			getHandleV()								const		= 0;

protected:
	virtual									~PxnFluid()												{}
};


#endif
