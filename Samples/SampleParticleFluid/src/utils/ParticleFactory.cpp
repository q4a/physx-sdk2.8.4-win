// ===============================================================================
//					NVIDIA PhysX SDK Sample ProgramS
//					        PARTICLE FLUID
//
//		            Written by Simon Schirm 09-12-06
// ===============================================================================

#include "ParticleFactory.h"

void CreateParticleAABB(NxParticleData& pd, unsigned maxParticlesTotal, unsigned maxParticles, bool append, NxBounds3& aabb, const NxVec3 vel, float lifetime, float distance)
{
	char* bufferPos = reinterpret_cast<char*>(pd.bufferPos);
	char* bufferVel = reinterpret_cast<char*>(pd.bufferVel);
	char* bufferLife = reinterpret_cast<char*>(pd.bufferLife);

	if(bufferPos == NULL && bufferVel == NULL && bufferLife == NULL)
		return;

	NxVec3 aabbDim;
	aabb.getExtents(aabbDim);
	aabbDim *= 2.0f;

	unsigned sideNumX = (unsigned)NxMath::max(1.0f, NxMath::floor(aabbDim.x / distance));
	unsigned sideNumY = (unsigned)NxMath::max(1.0f, NxMath::floor(aabbDim.y / distance));
	unsigned sideNumZ = (unsigned)NxMath::max(1.0f, NxMath::floor(aabbDim.z / distance));

	if(!append)
		(*pd.numParticlesPtr) = 0;
	else
	{
		bufferPos += pd.bufferPosByteStride * (*pd.numParticlesPtr);
		bufferVel += pd.bufferVelByteStride * (*pd.numParticlesPtr);
		bufferLife += pd.bufferLifeByteStride * (*pd.numParticlesPtr);
	}

	unsigned numParticles = 0;
	for(unsigned i=0; i<sideNumX; i++)
		for(unsigned j=0; j<sideNumY; j++)
			for(unsigned k=0; k<sideNumZ; k++)
			{
				if(*pd.numParticlesPtr >= maxParticlesTotal)
					break;

				if(numParticles >= maxParticles) 
					break;

				NxVec3 p = NxVec3(i*distance,j*distance,k*distance);
				p += aabb.min;

				if(bufferPos)
				{
					NxVec3& position = *reinterpret_cast<NxVec3*>(bufferPos);
					position = p;
					bufferPos += pd.bufferPosByteStride;
				}

				if(bufferVel)
				{
					NxVec3& velocity = *reinterpret_cast<NxVec3*>(bufferVel);
					velocity = vel;
					bufferVel += pd.bufferVelByteStride;
				}

				if(bufferLife)
				{
					NxReal& life = *reinterpret_cast<NxReal*>(bufferLife);
					life = lifetime;
					bufferLife += pd.bufferLifeByteStride;
				}
				
				(*pd.numParticlesPtr)++;
				numParticles++;
			}
}

void CreateParticleSphere(NxParticleData& pd, unsigned maxParticles, bool append, const NxVec3& pos, const NxVec3 vel, float lifetime, float distance, unsigned sideNum)
{
	float rad = sideNum*distance*0.5f;
	
	char* bufferPos = reinterpret_cast<char*>(pd.bufferPos);
	char* bufferVel = reinterpret_cast<char*>(pd.bufferVel);
	char* bufferLife = reinterpret_cast<char*>(pd.bufferLife);

	if(bufferPos == NULL && bufferVel == NULL && bufferLife == NULL)
		return;

	if(!append)
		(*pd.numParticlesPtr) = 0;
	else
	{
		bufferPos += pd.bufferPosByteStride * (*pd.numParticlesPtr);
		bufferVel += pd.bufferVelByteStride * (*pd.numParticlesPtr);
		bufferLife += pd.bufferLifeByteStride * (*pd.numParticlesPtr);
	}

	for(unsigned i=0; i<sideNum; i++)
		for(unsigned j=0; j<sideNum; j++)
			for(unsigned k=0; k<sideNum; k++)
			{
				if(*pd.numParticlesPtr >= maxParticles)
					break;

				NxVec3 p = NxVec3(i*distance,j*distance,k*distance);
				if(p.distance(NxVec3(rad,rad,rad)) < rad)
				{
					p += pos;

					if(bufferPos)
					{
						NxVec3& position = *reinterpret_cast<NxVec3*>(bufferPos);
						position = p;
						bufferPos += pd.bufferPosByteStride;
					}

					if(bufferVel)
					{
						NxVec3& velocity = *reinterpret_cast<NxVec3*>(bufferVel);
						velocity = vel;
						bufferVel += pd.bufferVelByteStride;
					}

					if(bufferLife)
					{
						NxReal& life = *reinterpret_cast<NxReal*>(bufferLife);
						life = lifetime;
						bufferLife += pd.bufferLifeByteStride;
					}
					
					(*pd.numParticlesPtr)++;
				}
			}
}

#define START_DAMPING 0.1f
#define MAX_SWINGS 30

void CreateRelaxedParticleBlock(NxParticleData& pd, unsigned maxParticlesTotal, unsigned _maxParticles, bool append, float lifetime,
								NxBounds3& aabb, const NxVec3& gravity, const NxFluidDesc& fluidDesc, float timeStepSize, unsigned maxIters,
								NxPhysicsSDK& physicsSDK)
{
	//early outs
	if(*pd.numParticlesPtr >= maxParticlesTotal)
		return;

	unsigned maxParticles = NxMath::min(maxParticlesTotal - *pd.numParticlesPtr, _maxParticles);
	if(maxParticles == 0)
		return;

	//set destination buffers
	char* bufferPos = reinterpret_cast<char*>(pd.bufferPos);
	char* bufferVel = reinterpret_cast<char*>(pd.bufferVel);
	char* bufferLife = reinterpret_cast<char*>(pd.bufferLife);

	if(bufferPos == NULL && bufferVel == NULL && bufferLife == NULL)
		return;

	if(!append)
		(*pd.numParticlesPtr) = 0;
	else
	{
		bufferPos += pd.bufferPosByteStride * (*pd.numParticlesPtr);
		bufferVel += pd.bufferVelByteStride * (*pd.numParticlesPtr);
		bufferLife += pd.bufferLifeByteStride * (*pd.numParticlesPtr);
	}

	//create scene for relaxing fluid simulation
	NxSceneDesc sceneDesc;
    sceneDesc.gravity = gravity;
	sceneDesc.maxIter = 1;
	sceneDesc.maxTimestep = timeStepSize;
    NxScene* scene = physicsSDK.createScene(sceneDesc);
	assert(scene);

	//create initial set of particles in an aabb
	unsigned numParticles = 0;
	NxParticleData relaxPd;
	relaxPd.bufferPos = (NxF32*)bufferPos;
	relaxPd.bufferPosByteStride = pd.bufferPosByteStride;
	relaxPd.bufferVel = (NxF32*)bufferVel;
	relaxPd.bufferVelByteStride = pd.bufferVelByteStride;
	relaxPd.numParticlesPtr = &numParticles;
	CreateParticleAABB(relaxPd, maxParticles, maxParticles, false, aabb, NxVec3(0,0,0), 0.0f, 1.0f / fluidDesc.restParticlesPerMeter);
	*pd.numParticlesPtr += *relaxPd.numParticlesPtr;

	//create fluid in scene, but with internal data return buffers
	NxFluidDesc relaxFluidDesc;
	relaxFluidDesc.collisionDistanceMultiplier =  fluidDesc.collisionDistanceMultiplier;
	relaxFluidDesc.kernelRadiusMultiplier = fluidDesc.kernelRadiusMultiplier;
	relaxFluidDesc.motionLimitMultiplier = fluidDesc.motionLimitMultiplier;
	relaxFluidDesc.packetSizeMultiplier =  fluidDesc.packetSizeMultiplier;
	relaxFluidDesc.restDensity = fluidDesc.restDensity;
	relaxFluidDesc.restParticlesPerMeter = fluidDesc.restParticlesPerMeter;
	relaxFluidDesc.dynamicFrictionForStaticShapes = fluidDesc.dynamicFrictionForStaticShapes;
	relaxFluidDesc.attractionForStaticShapes = fluidDesc.attractionForStaticShapes;
	relaxFluidDesc.restitutionForStaticShapes = fluidDesc.restitutionForStaticShapes;
	relaxFluidDesc.stiffness = fluidDesc.stiffness;
	relaxFluidDesc.viscosity = fluidDesc.viscosity;

	relaxFluidDesc.damping = START_DAMPING;
	relaxFluidDesc.initialParticleData = relaxPd;
	relaxFluidDesc.particlesWriteData = relaxPd;
	
	NxFluid* fluid = scene->createFluid(relaxFluidDesc);
	assert(fluid);

	NxActorDesc planeActorDesc;
	NxPlaneShapeDesc planeShapeDesc;
	planeActorDesc.shapes.pushBack(&planeShapeDesc);

	planeShapeDesc.d =			aabb.min.x;
	planeShapeDesc.normal =		NxVec3( 1, 0, 0);
	scene->createActor(planeActorDesc);

	planeShapeDesc.d =			-aabb.max.x;
	planeShapeDesc.normal =		NxVec3(-1, 0, 0);
	scene->createActor(planeActorDesc);

	planeShapeDesc.d =			aabb.min.y;
	planeShapeDesc.normal =		NxVec3( 0, 1, 0);
	scene->createActor(planeActorDesc);

	planeShapeDesc.d =			-aabb.max.y;
	planeShapeDesc.normal =		NxVec3( 0,-1, 0);
	scene->createActor(planeActorDesc);

	planeShapeDesc.d =			aabb.min.z;
	planeShapeDesc.normal =		NxVec3( 0, 0, 1);
	scene->createActor(planeActorDesc);

	planeShapeDesc.d =			-aabb.max.z;
	planeShapeDesc.normal =		NxVec3( 0, 0,-1);
	scene->createActor(planeActorDesc);

	bool relaxed = false;
	float kineticMeasure = 0.0f;
	unsigned numIterations = 0;
	unsigned numSwings = 0;
	bool accMode = true;
	while (numIterations < maxIters && !relaxed)
	{
		NxU32 error;
		scene->simulate(sceneDesc.maxTimestep);
		scene->flushStream();
		scene->fetchResults(NX_RIGID_BODY_FINISHED, true, &error);
		assert(error == 0);

		numIterations++;

		float k = 0.0f;
		for(unsigned i=0; i<*relaxPd.numParticlesPtr; i++)
		{
			const NxVec3& velocity = *reinterpret_cast<NxVec3*>(relaxPd.bufferVel + (pd.bufferVelByteStride*i/4));
			k += velocity.magnitudeSquared();
		}
		if(accMode && k < kineticMeasure)
		{
			numSwings++;
			accMode = false;
		}
		else if(!accMode && k > kineticMeasure)
		{
			numSwings++;
			accMode = true;
		}
		kineticMeasure = k;
		if(numSwings == MAX_SWINGS)
			relaxed = true;
	}

	if(pd.bufferLife)
	{
		for(unsigned i=0; i<*relaxPd.numParticlesPtr; i++)
		{
			NxF32* dstLife = pd.bufferLife + (pd.bufferLifeByteStride*i/4);
			*dstLife = lifetime;
		}
	}

	//remove scene again.
	physicsSDK.releaseScene(*scene);
}