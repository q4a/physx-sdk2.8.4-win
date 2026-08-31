#ifndef PXS_FLUID_COLLISION_XBOX_H
#define PXS_FLUID_COLLISION_XBOX_H

PXD_FORCE_INLINE void PxsFluidCollision::clampToMaxMotion(PxsParticleCollData& collData)
{
	PxcVector motionVec = collData.newPos - collData.oldPos;
	PxFloat motionDistanceS = motionVec.magnitudeSquared();
	PxFloat motionDistanceR = (PxFloat)__frsqrte(motionDistanceS);
	PxFloat motionDistance = motionDistanceR * motionDistanceS;
	if (motionDistance > mParams.maxMotionDistance)
	{
		collData.newPos = collData.oldPos + (motionVec * (mParams.maxMotionDistance * motionDistanceR));
	}
}

#endif
