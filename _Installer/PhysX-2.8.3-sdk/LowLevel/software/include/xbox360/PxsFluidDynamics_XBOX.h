#ifndef PXS_FLUID_DYNAMICS_XBOX_H
#define PXS_FLUID_DYNAMICS_XBOX_H

#ifndef PXS_FLUID_DYNAMICS_H
#error you must include Cloth.h instead
#endif

// use template technical to avoid branch in the loop and unroll loop

//template<int n, class B>
//struct _unroll 
//{
//	PXD_FORCE_INLINE static void apply(PxU32 i)
//	{
//		B::apply(i);
//		_unroll<n-1, B>::apply(i+1);
//	}
//};
//
//template<class B>
//struct _unroll<0, B>
//{
//	PXD_FORCE_INLINE static void apply(PxU32 i)
//	{
//	}
//};
//
//template<int n, class B>
//struct _for
//{
//	PXD_FORCE_INLINE static void apply(PxU32 start, PxU32 end)
//	{
//		PxU32 loop_size = end - start;
//		PxU32 loop_end = start + n * (loop_size/n);
//
//		for(PxU32 i=start; i<loop_end; i+=n)
//		{
//			_unroll<n, B>::apply(i);
//		}
//		for(PxU32 i=loop_end; i<end; i++)
//		{
//			B::apply(i);
//		}
//	}
//};

template<bool twoWayUpdate>
struct _calcDensity
{
	PXD_FORCE_INLINE static void apply(PxsFluidDynamics* pFluidDynamics, PxFloat distSqr, const PxcVector& distVec, PxsFluidParticle& particleA, PxsFluidParticle& particleB)
	{
		PxFloat dist2Std = distSqr * pFluidDynamics->mParams.scaleSqToStd;
		PxFloat radius2MinusDist2Std = pFluidDynamics->mParams.radiusSqStd - dist2Std;
		PxFloat densityStd = pFluidDynamics->mParams.densityMultiplierStd * radius2MinusDist2Std * radius2MinusDist2Std * radius2MinusDist2Std;

		particleA.density += densityStd;
		if (twoWayUpdate)
			particleB.density += densityStd;
	}

};

template<bool twoWayUpdate>
struct _calcForce_VMX // don't know why this is slower than the normal version
{
	PXD_FORCE_INLINE static void apply(PxsFluidDynamics* pFluidDynamics, PxFloat distSqr, const PxcVector& dVec, PxsFluidParticle& particleA, PxsFluidParticle& particleB)
	{
		PxcSimd::Vector4 distVec = PxcSimd::loadA(dVec);
		PxFloat dist2Std = distSqr * pFluidDynamics->mParams.scaleSqToStd;

		PxFloat recipDistStd = 1.0f/(PxFloat)__fsqrts(dist2Std);
		PxFloat distStd = dist2Std*recipDistStd;

		PxFloat radiusMinusDistStd = pFluidDynamics->mParams.radiusStd - distStd;

		//pressure force
		PxFloat multiplierPressStd = (pFluidDynamics->mParams.radiusSqStd*recipDistStd - 2*pFluidDynamics->mParams.radiusStd + distStd) * 
			pFluidDynamics->mParams.pressureMultiplierStd * (particleA.pressure + particleB.pressure);

		PxcSimd::Vector4 force = distVec * (multiplierPressStd * pFluidDynamics->mParams.scaleToStd);

		// viscosity force
		PxFloat multiplierViscStd = radiusMinusDistStd * pFluidDynamics->mParams.viscosityMultiplierStd;

		PxcSimd::Vector4 velocityA = PxcSimd::loadA(particleA.velocity);
		PxcSimd::Vector4 velocityB = PxcSimd::loadA(particleB.velocity);
		//	PxcSimd::Vector4 vDiff = (velocityB - velocityA) * (mParams.scaleToStd * multiplierViscStd);
		//	force += vDiff;

		force = PxcSimd::multiplyAdd(velocityB-velocityA, PxcSimd::loadSplat(pFluidDynamics->mParams.scaleToStd * multiplierViscStd), force);

		// do we have load&store problem? multiAdd
		PxcSimd::Vector4 forceOrNormal = PxcSimd::loadA(particleA.forceOrNormal);
		//	forceOrNormal += (force * (1.0f / particleB.density));
		forceOrNormal = PxcSimd::multiplyAdd(force, PxcSimd::loadSplat(__fres(particleB.density)), forceOrNormal);
		PxcSimd::storeA(particleA.forceOrNormal, forceOrNormal);

		if (twoWayUpdate)
		{
			forceOrNormal = PxcSimd::loadA(particleB.forceOrNormal);
			//		forceOrNormal -= (force * (1.0f / particleA.density));
			forceOrNormal = PxcSimd::negMultiplySubtract(force, PxcSimd::loadSplat(__fres(particleA.density)), forceOrNormal);
			PxcSimd::storeA(particleB.forceOrNormal, forceOrNormal);
		}
	}
};

template<bool twoWayUpdate>
struct _calcForce
{
	PXD_FORCE_INLINE static void apply(PxsFluidDynamics* pFluidDynamics, PxFloat distSqr, const PxcVector& distVec, PxsFluidParticle& particleA, PxsFluidParticle& particleB)
	{
		PxFloat dist2Std = distSqr * pFluidDynamics->mParams.scaleSqToStd;

		PxFloat recipDistStd = 1.0f/(PxFloat)__fsqrts(dist2Std);
		PxFloat distStd = dist2Std*recipDistStd;

		PxFloat radiusMinusDistStd = pFluidDynamics->mParams.radiusStd - distStd;

		//pressure force
		PxFloat multiplierPressStd = (pFluidDynamics->mParams.radiusSqStd*recipDistStd - 2*pFluidDynamics->mParams.radiusStd + distStd) * 
			pFluidDynamics->mParams.pressureMultiplierStd * (particleA.pressure + particleB.pressure);

		PxcVector force = distVec * (multiplierPressStd * pFluidDynamics->mParams.scaleToStd);

		// viscosity force
		PxFloat multiplierViscStd = radiusMinusDistStd * pFluidDynamics->mParams.viscosityMultiplierStd;

		PxcVector vDiff = (particleB.velocity - particleA.velocity) * (pFluidDynamics->mParams.scaleToStd * multiplierViscStd);
		force += vDiff;

		particleA.forceOrNormal += (force * __fres(particleB.density));
		if (twoWayUpdate)
			particleB.forceOrNormal -= (force * __fres(particleA.density));
	}
	
};

#define UNROLL_N 4
#define __IR(x) ((PxU32&)(x)) 

template<template <bool> class T>
struct updateParticles_impl
{
	PXD_FORCE_INLINE static void apply(PxsFluidDynamics* pFluidDynamics, PxsFluidParticle& particleA, PxsFluidParticle* particlesSpB,
		const PxU16* particleIndicesSpB, PxU16 startIdxCellB, PxU16 firstParticleCellB, PxU16 numParticlesCellB, bool twoWayUpdate)
	{
		if(twoWayUpdate)
			return inter_calc<true>(pFluidDynamics, particleA, particlesSpB, particleIndicesSpB, startIdxCellB, firstParticleCellB, numParticlesCellB);
		else
			return inter_calc<false>(pFluidDynamics, particleA, particlesSpB, particleIndicesSpB, startIdxCellB, firstParticleCellB, numParticlesCellB);
	}

private:
	template<bool twoWayUpdate>
	PXD_FORCE_INLINE static void inter_calc(PxsFluidDynamics* pFluidDynamics, PxsFluidParticle& particleA, PxsFluidParticle* __restrict particlesSpB,
		const PxU16* __restrict particleIndicesSpB, PxU16 startIdxCellB, PxU16 firstParticleCellB, PxU16 numParticlesCellB)
	{
		PxU32 loop_size = numParticlesCellB - startIdxCellB;
		PxU32 loop_end = startIdxCellB + UNROLL_N * (loop_size/UNROLL_N);

		PxU32 u_cellSizeSq = __IR(pFluidDynamics->mParams.cellSizeSq);

		#define LOOP_BODY(x) \
		do \
		{ \
			PxU16 idx = particleIndicesSpB[firstParticleCellB + p + (x)]; \
			PxsFluidParticle& particleB = particlesSpB[idx]; \
			PxcVector __declspec(align(16)) distVec = particleA.position - particleB.position; \
			PxFloat distSqr = distVec.magnitudeSquared(); \
			PxU32 u_distSqr = __IR(distSqr); \
			if (u_distSqr < u_cellSizeSq && u_distSqr > 0) \
			{ \
				T<twoWayUpdate>::apply(pFluidDynamics, distSqr, distVec, particleA, particleB); \
			} \
		} while(0)

		for(PxU32 p=startIdxCellB; p < loop_end; p+=UNROLL_N)
		{
#if 0
			LOOP_BODY(0);
			LOOP_BODY(1);
			LOOP_BODY(2);
			LOOP_BODY(3);
#else  // the following version is a little fast
			PxU16 idx0 = particleIndicesSpB[firstParticleCellB + p    ];
			PxU16 idx1 = particleIndicesSpB[firstParticleCellB + p + 1];
			PxU16 idx2 = particleIndicesSpB[firstParticleCellB + p + 2];
			PxU16 idx3 = particleIndicesSpB[firstParticleCellB + p + 3];

			PxsFluidParticle& particleB0 = particlesSpB[idx0];
			PxsFluidParticle& particleB1 = particlesSpB[idx1];
			PxsFluidParticle& particleB2 = particlesSpB[idx2];
			PxsFluidParticle& particleB3 = particlesSpB[idx3];

			PxcVector __declspec(align(16)) distVec0 = particleA.position - particleB0.position;
			PxcVector __declspec(align(16)) distVec1 = particleA.position - particleB1.position;
			PxcVector __declspec(align(16)) distVec2 = particleA.position - particleB2.position;
			PxcVector __declspec(align(16)) distVec3 = particleA.position - particleB3.position;

			PxFloat distSqr0 = distVec0.magnitudeSquared();
			PxFloat distSqr1 = distVec1.magnitudeSquared();
			PxFloat distSqr2 = distVec2.magnitudeSquared();
			PxFloat distSqr3 = distVec3.magnitudeSquared();

			PxU32 u_distSqr0 = __IR(distSqr0);
			PxU32 u_distSqr1 = __IR(distSqr1);
			PxU32 u_distSqr2 = __IR(distSqr2);
			PxU32 u_distSqr3 = __IR(distSqr3);

			if (u_distSqr0 < u_cellSizeSq && u_distSqr0 > 0)
			{
				T<twoWayUpdate>::apply(pFluidDynamics, distSqr0, distVec0, particleA, particleB0);
			}
			if (u_distSqr1 < u_cellSizeSq && u_distSqr1 > 0)
			{
				T<twoWayUpdate>::apply(pFluidDynamics, distSqr1, distVec1, particleA, particleB1);
			}
			if (u_distSqr2 < u_cellSizeSq && u_distSqr2 > 0)
			{
				T<twoWayUpdate>::apply(pFluidDynamics, distSqr2, distVec2, particleA, particleB2);
			}
			if (u_distSqr3 < u_cellSizeSq && u_distSqr3 > 0)
			{
				T<twoWayUpdate>::apply(pFluidDynamics, distSqr3, distVec3, particleA, particleB3);
			}
#endif
		}

		for(PxU32 p=loop_end; p < numParticlesCellB; p++)
		{
			LOOP_BODY(0);
		}
		#undef LOOP_BODY
	}
};

template<template <bool> class T>
struct updateParticlesBruteForce_impl
{
	PXD_FORCE_INLINE static void apply(PxsFluidDynamics* pFluidDynamics, PxU32 numParticlesA, PxU32 numParticlesB,
		PxsFluidParticle* particlesA, PxsFluidParticle* particlesB, bool twoWayUpdate)
	{
		if(twoWayUpdate)
			return inter_calc<true>(pFluidDynamics, numParticlesA, numParticlesB, particlesA, particlesB);
		else
			return inter_calc<false>(pFluidDynamics, numParticlesA, numParticlesB, particlesA, particlesB);
	}

private:
	template<bool twoWayUpdate>
	PXD_FORCE_INLINE static void inter_calc(PxsFluidDynamics* pFluidDynamics, PxU32 numParticlesA, PxU32 numParticlesB,
		PxsFluidParticle* particlesA, PxsFluidParticle* particlesB)
	{
		PxsFluidParticle* __restrict prtA = particlesA;

		for(PxU32 p=0; p < numParticlesA; p++)
		{
			PxsFluidParticle* __restrict prtB = particlesB;

			PxU32 u_cellSizeSq = __IR(pFluidDynamics->mParams.cellSizeSq);

			PxU32 loop_end = UNROLL_N * (numParticlesB/UNROLL_N);

			#define LOOP_BODY() \
			do \
			{ \
				PxcVector __declspec(align(16)) distVec = prtA->position - prtB->position; \
				PxFloat distSqr = distVec.magnitudeSquared(); \
				PxU32 u_distSqr = __IR(distSqr); \
				if (u_distSqr < u_cellSizeSq && u_distSqr > 0) \
				{ \
					T<twoWayUpdate>::apply(pFluidDynamics, distSqr, distVec, *prtA, *prtB); \
				} \
				prtB++; \
			}while(0)

			for (PxU32 q=0; q < loop_end; q+=UNROLL_N)
			{
#if 0
				LOOP_BODY();
				LOOP_BODY();
				LOOP_BODY();
				LOOP_BODY();
#else

				PxcVector __declspec(align(16)) distVec0 = prtA->position - (prtB+0)->position;
				PxcVector __declspec(align(16)) distVec1 = prtA->position - (prtB+1)->position;
				PxcVector __declspec(align(16)) distVec2 = prtA->position - (prtB+2)->position;
				PxcVector __declspec(align(16)) distVec3 = prtA->position - (prtB+3)->position;
				
				PxFloat distSqr0 = distVec0.magnitudeSquared();
				PxFloat distSqr1 = distVec1.magnitudeSquared();
				PxFloat distSqr2 = distVec2.magnitudeSquared();
				PxFloat distSqr3 = distVec3.magnitudeSquared();

				PxU32 u_distSqr0 = __IR(distSqr0);
				PxU32 u_distSqr1 = __IR(distSqr1);
				PxU32 u_distSqr2 = __IR(distSqr2);
				PxU32 u_distSqr3 = __IR(distSqr3);

				if (u_distSqr0 < u_cellSizeSq && u_distSqr0 > 0)
				{
					T<twoWayUpdate>::apply(pFluidDynamics, distSqr0, distVec0, *prtA, *(prtB+0));
				}
				if (u_distSqr1 < u_cellSizeSq && u_distSqr1 > 0)
				{
					T<twoWayUpdate>::apply(pFluidDynamics, distSqr1, distVec1, *prtA, *(prtB+1));
				}
				if (u_distSqr2 < u_cellSizeSq && u_distSqr2 > 0)
				{
					T<twoWayUpdate>::apply(pFluidDynamics, distSqr2, distVec2, *prtA, *(prtB+2));
				}
				if (u_distSqr3 < u_cellSizeSq && u_distSqr3 > 0)
				{
					T<twoWayUpdate>::apply(pFluidDynamics, distSqr3, distVec3, *prtA, *(prtB+3));
				}

				prtB += 4;
#endif
			}

			for (PxU32 q=loop_end; q < numParticlesB; q++)
			{
				LOOP_BODY();
			}
			#undef LOOP_BODY
			prtA++;
		}
	}
};

#undef UNROLL_N
#undef __IR

PXD_FORCE_INLINE void PxsFluidDynamics::updateParticles(UpdateType updateType, PxsFluidParticle& particleA, PxsFluidParticle* particlesSpB,
														const PxU16* particleIndicesSpB, PxU16 startIdxCellB, PxU16 firstParticleCellB,
														PxU16 numParticlesCellB, bool twoWayUpdate)
{
	// Check given particle against particles of another cell.

	if (updateType == UPDATE_DENSITY)
	{
		return updateParticles_impl<_calcDensity>::apply(this, particleA, particlesSpB, particleIndicesSpB, startIdxCellB, firstParticleCellB, numParticlesCellB, twoWayUpdate);
	}
	else
	{
		return updateParticles_impl<_calcForce>::apply(this, particleA, particlesSpB, particleIndicesSpB, startIdxCellB, firstParticleCellB, numParticlesCellB, twoWayUpdate);
	}
}

PXD_FORCE_INLINE void PxsFluidDynamics::updateParticlesBruteForce(UpdateType updateType, PxU32 numParticlesA, PxU32 numParticlesB,
																  PxsFluidParticle* particlesA, PxsFluidParticle* particlesB, bool twoWayUpdate)
{
	// Check each particle in one array against each particle in the other array.

	if (updateType == UPDATE_DENSITY)
	{
		return updateParticlesBruteForce_impl<_calcDensity>::apply(this, numParticlesA, numParticlesB, particlesA, particlesB, twoWayUpdate);
	}
	else
	{
		return updateParticlesBruteForce_impl<_calcForce>::apply(this, numParticlesA, numParticlesB, particlesA, particlesB, twoWayUpdate);
	}
}

#endif // PXS_FLUID_DYNAMICS_XBOX_H
