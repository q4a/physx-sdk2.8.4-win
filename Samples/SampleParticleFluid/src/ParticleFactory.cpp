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

