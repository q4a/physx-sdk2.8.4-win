#ifndef PXS_FLUID_DYNAMICS_H
#define PXS_FLUID_DYNAMICS_H


#include "PxsFluidConfig.h"
#ifdef PXS_ENABLE_FLUIDS

#include "PxcHelperFunction.h"
#include "PxsFluidParticle.h"
#include "PxsFluidSpatialHash.h"


#define ORIGINAL_FLUID_IMPL

#ifdef _XBOX
#undef ORIGINAL_FLUID_IMPL
#include "PxcSimd.h"
#endif

//#ifdef _PS3
//#undef ORIGINAL_CLOTH_IMPL
//#include "Cloth_PS3.h"
//#endif

class PxsFluidParticleArray;
struct PxsFluidParticle;


struct PxsFluidDynamicsParameters
{
	PxcVector	externalAcceleration;
    PxFloat		initialDensity;
	PxFloat		selfDensity;
	PxFloat		particleMassStd;
	PxFloat		cellSize;
	PxFloat		cellSizeSq;
	PxFloat		radiusStd;
	PxFloat		radiusSqStd;
	PxFloat		densityMultiplierStd;
	PxFloat		pressureMultiplierStd;
	PxFloat		viscosityMultiplierStd;
	PxFloat		stiffnessStd;
	PxFloat		dynamicDamping;
	PxFloat		scaleToStd;
	PxFloat		scaleSqToStd;
	PxFloat		scaleToWorld;
	PxU32		packetMultLog;
	PxFloat		maxMotionDistance;
};


class PxsFluidDynamics
{
public:

	PxsFluidDynamics();
	~PxsFluidDynamics();

	void update(PxsFluidParticleArray& particles, const PxsFluidPacket* packets, const PxsFluidPacketSections* packetSections,
				PxFloat timeStep, bool doSimpleDynamics);

private:

	// Table to get the neighboring halo region indices for a packet section
	struct SectionToHaloTable
	{
		PxU32	numHaloRegions;
		PxU32	haloRegionIndices[19];	// No packet section has more than 19 neighboring halo regions
	};

	// Defines which fluid property to update
	enum UpdateType
	{
		UPDATE_DENSITY,
		UPDATE_FORCE
	};

	void updateDensity(PxsFluidParticleArray& particles, const PxsFluidPacket* packets,	const PxsFluidPacketSections* packetSections);
	void updateForce(PxsFluidParticleArray& particles, const PxsFluidPacket* packets, const PxsFluidPacketSections* packetSections, PxFloat timeStep);

	void updatePacket(UpdateType updateType, PxsFluidParticleArray& particles, const PxsFluidPacket& packet, const PxsFluidPacketSections& packetSections,
					  const PxsFluidPacketHaloRegions& haloRegions);

	PXD_FORCE_INLINE void updateSubpacketPairs(UpdateType updateType, PxU32 numParticles, PxU32 subpacketsParticleRemainder, PxsFluidParticle* particlesSpA, PxsFluidParticle* particlesOtherSps,
												PxsFluidCell* cellsSpA, PxsFluidCell* cellsSpB,	PxU16* particleIndicesSpA, PxU16* particleIndicesSpB,
												PxU32 numParticlesPerSubpacket, PxU32 numCellHashBuckets, bool storeHashKeyInParticle, bool twoWayUpdate);

	void updateCells(UpdateType updateType, PxsFluidParticle* particles, const PxsFluidCell* cells, const PxU16* particleIndices, const PxU32 numCellHashBuckets);
	void updateCells(UpdateType updateType, PxsFluidParticle* particlesSpA, PxsFluidParticle* particlesSpB, const PxsFluidCell* cellsSpA, const PxsFluidCell* cellsSpB,
					 const PxU16* particleIndicesSpA, const PxU16* particleIndicesSpB, const PxU32 numCellHashBuckets, bool twoWayUpdate);

	PXD_FORCE_INLINE void updateParticles(UpdateType updateType, PxsFluidParticle& particleA, PxsFluidParticle* particlesSpB,
										  const PxU16* particleIndicesSpB, PxU16 startIdxCellB, PxU16 firstParticleCellB,
										  PxU16 numParticlesCellB, bool twoWayUpdate);

	PXD_FORCE_INLINE void updateParticlesBruteForce(UpdateType updateType, PxU32 numParticlesA, PxU32 numParticlesB,
													PxsFluidParticle* particlesA, PxsFluidParticle* particlesB, bool twoWayUpdate);

	PXD_FORCE_INLINE void updateParticlesBruteForceHalo(UpdateType updateType, PxsFluidParticleArray& particles,
														const PxsFluidPacketSections& packetSections, const PxsFluidPacketHaloRegions& haloRegions);

#ifdef ORIGINAL_FLUID_IMPL
	PXD_FORCE_INLINE void calcDensity(PxFloat distSqr, PxsFluidParticle& particleA, PxsFluidParticle& particleB, bool twoWayUpdate);
	PXD_FORCE_INLINE void calcForce(PxFloat distSqr, const PxcVector& distVec, PxsFluidParticle& particleA, PxsFluidParticle& particleB, bool twoWayUpdate);
#endif

	void integrateVelocityPerPacket(PxsFluidParticleArray& particles, const PxsFluidPacket* packets, PxFloat timeStep);
	void integrateVelocity(PxsFluidParticle* particles, PxU32 numParticles, PxFloat timeStep, bool doSimpleDynamics);

	PXD_FORCE_INLINE PxFloat computeDampingFactor(PxFloat damping, PxFloat timeStep);
	PXD_FORCE_INLINE void clampVelocity(PxcVector& velocity, PxFloat maxMotion, PxFloat timeStep);


public:
						PxsFluidDynamicsParameters	mParams;

private:
			static		SectionToHaloTable			mSectionToHaloTable[26];	// Halo region table for each packet section
																				// (except middle section which does not interact with neighbors)
};


#ifdef _XBOX
#include "xbox360/PxsFluidDynamics_XBOX.h"
#endif


PXD_FORCE_INLINE void PxsFluidDynamics::updateSubpacketPairs(UpdateType updateType, PxU32 numParticles, PxU32 subpacketsParticleRemainder, PxsFluidParticle* particlesSpA, PxsFluidParticle* particlesOtherSps,
															PxsFluidCell* cellsSpA, PxsFluidCell* cellsSpB,	PxU16* particleIndicesSpA, PxU16* particleIndicesSpB,
															PxU32 numParticlesPerSubpacket, PxU32 numCellHashBuckets, bool storeHashKeyInParticle, bool twoWayUpdate)
{
	PXN_ASSERT(numParticlesPerSubpacket > 0);

	PxU32 numParticlesLocalHash = subpacketsParticleRemainder;
	PxsFluidParticle* particlesSpB = particlesOtherSps;
	PxU32 endIndexSpB = subpacketsParticleRemainder;	// The first subpacket can be smaller than the following ones
	while (endIndexSpB <= numParticles)
	{
		// MS: Optimization: Store the local hashes such that they do not have to be computed every
		//                   time again in this loop.

		// Get local cell hash for other subpacket
		PxsFluidSpatialHash::buildLocalHash(particlesSpB, numParticlesLocalHash, cellsSpB, particleIndicesSpB,
											numCellHashBuckets, storeHashKeyInParticle);

		// For the cells of subpacket A, find neighboring cells in the subpacket B and compute particle interactions.
		updateCells(updateType, particlesSpA, particlesSpB, cellsSpA, cellsSpB,
					particleIndicesSpA, particleIndicesSpB, numCellHashBuckets, twoWayUpdate);

		particlesSpB = particlesOtherSps + endIndexSpB;
		endIndexSpB += numParticlesPerSubpacket;
		numParticlesLocalHash = numParticlesPerSubpacket;	// All following subpackets will have the same number of particles
	}
}


#ifdef ORIGINAL_FLUID_IMPL
PXD_FORCE_INLINE void PxsFluidDynamics::updateParticles(UpdateType updateType, PxsFluidParticle& particleA, PxsFluidParticle* particlesSpB,
														const PxU16* particleIndicesSpB, PxU16 startIdxCellB, PxU16 firstParticleCellB,
														PxU16 numParticlesCellB, bool twoWayUpdate)
{
	// Check given particle against particles of another cell.

	for(PxU32 p=startIdxCellB; p < numParticlesCellB; p++)
	{
		PxU16 idx = particleIndicesSpB[firstParticleCellB + p];
		PxsFluidParticle& particleB = particlesSpB[idx];

		PxcVector distVec = particleA.position - particleB.position;
		PxFloat distSqr = distVec.magnitudeSquared();

		if (distSqr < mParams.cellSizeSq)
		{
			if (updateType == UPDATE_DENSITY)
			{
				// Update density
				calcDensity(distSqr, particleA, particleB, twoWayUpdate);
			}
			else
			{
				// Update force
				calcForce(distSqr, distVec, particleA, particleB, twoWayUpdate);
			}
		}
	}
}


PXD_FORCE_INLINE void PxsFluidDynamics::updateParticlesBruteForce(UpdateType updateType, PxU32 numParticlesA, PxU32 numParticlesB,
																  PxsFluidParticle* particlesA, PxsFluidParticle* particlesB, bool twoWayUpdate)
{
	// Check each particle in one array against each particle in the other array.

	PxsFluidParticle* prtA = particlesA;

	for(PxU32 p=0; p < numParticlesA; p++)
	{
		PxsFluidParticle* prtB = particlesB;

		for (PxU32 q=0; q < numParticlesB; q++)
		{
			PxcVector distVec = prtA->position - prtB->position;
			PxFloat distSqr = distVec.magnitudeSquared();

			if (distSqr < mParams.cellSizeSq)
			{
				if (updateType == UPDATE_DENSITY)
				{
					// Update density
					calcDensity(distSqr, *prtA, *prtB, twoWayUpdate);
				}
				else
				{
					// Update force
					calcForce(distSqr, distVec, *prtA, *prtB, twoWayUpdate);
				}
			}

			prtB++;
		}

		prtA++;
	}
}

#endif

PXD_FORCE_INLINE void PxsFluidDynamics::updateParticlesBruteForceHalo(UpdateType updateType, PxsFluidParticleArray& particles,
																	  const PxsFluidPacketSections& packetSections,
																	  const PxsFluidPacketHaloRegions& haloRegions)
{
	for(PxU32 i=0; i < 26; i++)
	{
		if (packetSections.numParticles[i] == 0)
			continue;

		PxsFluidParticle* particlesA = &particles[packetSections.firstParticle[i]];

		//
		// Get neighboring halo regions for the packet section
		//
		PxU32 numHaloRegions = mSectionToHaloTable[i].numHaloRegions;
		PxU32* haloRegionIndices = mSectionToHaloTable[i].haloRegionIndices;

		//
		// Iterate over neighboring halo regions and update particles
		//
		for(PxU32 j=0; j < numHaloRegions; j++)
		{
			PxU32 idx = haloRegionIndices[j];

			if (haloRegions.numParticles[idx] == 0)
				continue;

			PxsFluidParticle* particlesB = &particles[haloRegions.firstParticle[idx]];

			// It is important that no data is written to particles in neighboring packets. The interaction effect
			// of the current packet on the neighboring packet will be considered when the neighboring packet is processed.
			updateParticlesBruteForce(updateType, packetSections.numParticles[i], haloRegions.numParticles[idx],
										particlesA, particlesB, false);
		}
	}
}

#ifdef ORIGINAL_FLUID_IMPL

PXD_FORCE_INLINE void PxsFluidDynamics::calcDensity(PxFloat distSqr, PxsFluidParticle& particleA, PxsFluidParticle& particleB, bool twoWayUpdate)
{
	PxFloat dist2Std = distSqr * mParams.scaleSqToStd;
	PxFloat radius2MinusDist2Std = mParams.radiusSqStd - dist2Std;
	PxFloat densityStd = mParams.densityMultiplierStd * radius2MinusDist2Std * radius2MinusDist2Std * radius2MinusDist2Std;

	particleA.density += densityStd;
	if (twoWayUpdate)
		particleB.density += densityStd;
}


PXD_FORCE_INLINE void PxsFluidDynamics::calcForce(PxFloat distSqr, const PxcVector& distVec, PxsFluidParticle& particleA, PxsFluidParticle& particleB,
												  bool twoWayUpdate)
{
	PxFloat dist2Std = distSqr * mParams.scaleSqToStd;
				
	if (dist2Std == 0.0f)
		return;
#if defined(PXD_PLATFORM_WII) || defined(PXD_PLATFORM_WIIU)
	PxFloat recipDistStd = PxcInvSqrt(dist2Std);
	PxFloat distStd = dist2Std * recipDistStd;
#else
	PxFloat distStd = PxcSqrt(dist2Std);
	PxFloat recipDistStd = 1.0f / distStd;
#endif

	PxFloat radiusMinusDistStd = mParams.radiusStd - distStd;

	//pressure force
	PxFloat multiplierPressStd = (mParams.radiusSqStd*recipDistStd - 2*mParams.radiusStd + distStd) * 
								  mParams.pressureMultiplierStd * (particleA.pressure + particleB.pressure);

	PxcVector force = distVec * multiplierPressStd * mParams.scaleToStd;

	// viscosity force
	PxFloat multiplierViscStd = radiusMinusDistStd * mParams.viscosityMultiplierStd;

	PxcVector vDiff = (particleB.velocity - particleA.velocity) * mParams.scaleToStd;
	force += (vDiff * multiplierViscStd);

	/*
	particleA.forceOrNormal += (force * particleB.invDensity);
	if (twoWayUpdate)
		particleB.forceOrNormal -= (force * particleA.invDensity);
	*/
	particleA.forceOrNormal += (force * (1.0f / particleB.density));
	if (twoWayUpdate)
		particleB.forceOrNormal -= (force * (1.0f / particleA.density));
}
#endif

PXD_FORCE_INLINE void PxsFluidDynamics::clampVelocity(PxcVector& velocity, PxFloat maxMotion, PxFloat timeStep)
{
	PxFloat velocityMagnitude = velocity.magnitude();
	if (velocityMagnitude * timeStep > maxMotion)
	{
		PxFloat scaleFactor = maxMotion / (velocityMagnitude * timeStep);
		velocity *= scaleFactor;
	}
}

PXD_FORCE_INLINE PxFloat PxsFluidDynamics::computeDampingFactor(PxFloat damping, PxFloat timeStep)
{
	PxFloat dampingDt = damping * timeStep;
	if (dampingDt < 1.0f)
		return 1.0f - dampingDt;
	else
		return 0.0f;
}

#endif	// PXS_ENABLE_FLUIDS

#endif
