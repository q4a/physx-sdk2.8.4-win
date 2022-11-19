#include "MyFluid.h"
#include "SampleParticleFluid.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "common.h"

extern NxVec3 gCameraForward;
extern NxVec3 gCameraRight;

/**
Note: There is no error handling code for simplicity. If any SDK code fails, an assert is hit.
*/
MyFluid::MyFluid(NxScene* scene, NxFluidDesc &desc, bool trackUserData, bool provideCollisionNormals, const NxVec3& particleColor, NxReal particleSize):
	mParticleBufferNum(0),
	mParticleBuffer(NULL),
	mFluid(NULL),
	mTrackUserData(trackUserData),
	mMyParticleBuffer(NULL),
	mCreatedParticleIdsNum(0),
	mCreatedParticleIds(NULL),
	mDeletedParticleIdsNum(0),
	mDeletedParticleIds(NULL),
	mParticleColor(particleColor),
	mParticleSize(particleSize),
	mRenderBuffer(NULL),
	mRenderBufferUserData(NULL)
{
	mMaxParticles = desc.maxParticles;
	mParticleBuffer = new ParticleSDK[mMaxParticles];
	desc.userData = this;

	//Setup particle write data.
	NxParticleData particleData;
	particleData.numParticlesPtr = &mParticleBufferNum;
	particleData.bufferPos = &mParticleBuffer[0].position.x;
	particleData.bufferPosByteStride = sizeof(ParticleSDK);
	particleData.bufferVel = &mParticleBuffer[0].velocity.x;
	particleData.bufferVelByteStride = sizeof(ParticleSDK);
	particleData.bufferDensity = &mParticleBuffer[0].density;
	particleData.bufferDensityByteStride = sizeof(ParticleSDK);
	particleData.bufferLife = &mParticleBuffer[0].lifetime;
	particleData.bufferLifeByteStride = sizeof(ParticleSDK);
	particleData.bufferId = &mParticleBuffer[0].id;
	particleData.bufferIdByteStride = sizeof(ParticleSDK);
	particleData.bufferDensity = &mParticleBuffer[0].density;
	particleData.bufferDensityByteStride = sizeof(ParticleSDK);

	if (provideCollisionNormals)
	{
		particleData.bufferCollisionNormal = &mParticleBuffer[0].collisionNormal.x;
		particleData.bufferCollisionNormalByteStride = sizeof(ParticleSDK);
	}
	
	desc.particlesWriteData = particleData;

	//User data buffers
	if (mTrackUserData)
	{
		mMyParticleBuffer = new MyParticle[mMaxParticles];
        mCreatedParticleIds = new NxU32[mMaxParticles];
        mDeletedParticleIds = new NxU32[mMaxParticles];

		//Setup id write data.
		NxParticleIdData idData;
		
		//Creation
		idData.numIdsPtr = &mCreatedParticleIdsNum;
		idData.bufferId = mCreatedParticleIds;
		idData.bufferIdByteStride = sizeof(NxU32);
		desc.particleCreationIdWriteData = idData;
		
		//Deletion
		idData.numIdsPtr = &mDeletedParticleIdsNum;
		idData.bufferId = mDeletedParticleIds;
		idData.bufferIdByteStride = sizeof(NxU32);
		desc.particleDeletionIdWriteData = idData;
	}

	assert(scene);
	mFluid = scene->createFluid(desc);
	assert(mFluid);
}

MyFluid::~MyFluid()
{
	delete[] mParticleBuffer;
	mParticleBuffer = NULL;
	mFluid->getScene().releaseFluid(*mFluid);
	
	if (mTrackUserData)
	{
		delete[] mMyParticleBuffer;
		mMyParticleBuffer = NULL;
		delete[] mCreatedParticleIds;
		mCreatedParticleIds = NULL;
		delete[] mDeletedParticleIds;
		mDeletedParticleIds = NULL;
	}

	delete[] mRenderBuffer;
	mRenderBuffer = 0;
}

void MyFluid::drawBillboards()
{
	//todo, move this into a small billboard rendering lib.
	if (!mRenderBuffer)
	{
		unsigned sizeFloat = mMaxParticles * 3 * 4;
		mRenderBuffer = new float[sizeFloat];

		if (mTrackUserData)
		{
			mRenderBufferUserData = new float[mMaxParticles * 4 * 4];
		}

	}

	//compute billboard vertices.
	NxVec3 upVec = gCameraRight.cross(gCameraForward);
	upVec.normalize();
	upVec *= mParticleSize;
	NxVec3 rightVec = gCameraRight;
	rightVec.normalize();
	rightVec *= mParticleSize;
	
	NxVec3 ll = -upVec - rightVec;
	NxVec3 hl = upVec - rightVec;
	NxVec3 hr = upVec + rightVec;
	NxVec3 lr = -upVec + rightVec;

	NxVec3* vecPtr = reinterpret_cast<NxVec3*>(mRenderBuffer);
	for (unsigned i=0; i<mParticleBufferNum; i++)
	{
		const ParticleSDK& particle = mParticleBuffer[i];

		vecPtr[0] = ll + particle.position;
		vecPtr[1] = hl + particle.position;
		vecPtr[2] = hr + particle.position;
		vecPtr[3] = lr + particle.position;
		
		vecPtr += 4;
	}

	if (mTrackUserData)
	{
		//reorder user data into mRenderBufferUserData per vertex data.
		float *pRenderBuf = mRenderBufferUserData;
		for (unsigned i=0; i<mParticleBufferNum; i++)
		{
			const ParticleSDK& particle = mParticleBuffer[i];
			assert(particle.id < mMaxParticles);
			const MyParticle& myParticle = mMyParticleBuffer[particle.id];
			for(unsigned j = 0; j < 4; ++ j)
			{
				*(pRenderBuf ++) = myParticle.rgba[0];
				*(pRenderBuf ++) = myParticle.rgba[1];
				*(pRenderBuf ++) = myParticle.rgba[2];
				*(pRenderBuf ++) = myParticle.rgba[3];
			}
		}
	}
	
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

	if (mTrackUserData)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), mParticleBufferNum*4, mRenderBuffer);
		glColorPointer(4, GL_FLOAT, sizeof(float) * 4, mParticleBufferNum*4, mRenderBufferUserData);

		glDrawArrays(GL_QUADS, 0, mParticleBufferNum*4);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);	
	}
	else
	{
		glColor4f(mParticleColor.x, mParticleColor.y, mParticleColor.z, 1.0f);
		glEnableClientState(GL_VERTEX_ARRAY);

		glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), mParticleBufferNum*4, mRenderBuffer);

		glDrawArrays(GL_QUADS, 0, mParticleBufferNum*4);
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void MyFluid::draw()
{
    drawBillboards();
}
