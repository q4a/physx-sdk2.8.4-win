#include "SampleParticleFluid.h"
#include "MyFluid.h"

extern NxVec3 gDefaultGravity;
extern bool gHardwareSimulation;
extern NxScene* gScene;
extern NxArray<MyFluid*> gMyFluids;
extern NxVec3 gCameraPos;
extern NxVec3 gCameraForward;
extern bool gShadows;
extern float gTime;

#ifdef __PPCGEKKO1__
#define MAX_PARTICLES 950
#else
#define MAX_PARTICLES 8000
#endif

struct ParticleUpdateSDK
{
	NxVec3	force;
	NxU32   flags;
};

static ParticleUpdateSDK* gUpdates = NULL;

#ifdef __PPCGEKKO1__
static NxReal gGenerationRadius = 8;
static NxReal gPlanetRadius = 3;
#else
static NxReal gGenerationRadius = 20;
static NxReal gPlanetRadius = 6;
#endif

static NxVec3 gCenter(0,0,0);
static NxReal gAtmosphereRadius = gPlanetRadius + 0.1f;
static NxVec3 gRotationAxis(0.2,1,0.3);

static NxReal gRotationSpeed  = 1.5f;
static NxReal gGravityPlanet = 8.0f;
static NxReal gFriction = 0.005f;
static NxReal gViscosity = 60.0f;

NxVec3 RandNormalVec(const NxVec3& mu, NxReal sig);

void SampleForce::setup()
{
	SetTitleString(getName());
#ifdef __PPCGEKKO1__	
	SetHelpString(NULL);
#else
	SetHelpString("    +/-: planet rotation");
#endif

	gShadows = false;

	//Create a set of initial particles
	ParticleSDK*	initParticles = new ParticleSDK[MAX_PARTICLES];
	unsigned initParticlesNum = 0;

	//remove gravity.
	gScene->setGravity(NxVec3(0,0,0));

	gRotationAxis.normalize();

	while (initParticlesNum < MAX_PARTICLES)
	{
		NxVec3 pos;

		pos = RandNormalVec(NxVec3(0,0,0), gGenerationRadius/4);

		if (pos.magnitudeSquared() < gPlanetRadius*gPlanetRadius)
			continue;

		if (pos.magnitudeSquared() > gGenerationRadius*gGenerationRadius)
			continue;

		ParticleSDK& newParticle = initParticles[initParticlesNum++];
		
		newParticle.position = gCenter + pos;
		newParticle.velocity = RandNormalVec(NxVec3(0,0,0), 0.2f);
	}

	//Setup structure to pass initial particles.
	NxParticleData initParticleData;

	initParticleData.numParticlesPtr		= &initParticlesNum;

	initParticleData.bufferPos				= &initParticles[0].position.x;
	initParticleData.bufferPosByteStride	= sizeof(ParticleSDK);

	initParticleData.bufferVel				= &initParticles[0].velocity.x;
	initParticleData.bufferVelByteStride	= sizeof(ParticleSDK);

	//Setup fluid descriptor
	NxFluidDesc fluidDesc;
    fluidDesc.maxParticles                  = MAX_PARTICLES;
    fluidDesc.kernelRadiusMultiplier		= 2.0f;
    fluidDesc.restParticlesPerMeter			= 3.6f;
	fluidDesc.motionLimitMultiplier			= 2.0f;
	fluidDesc.packetSizeMultiplier			= 8;
    fluidDesc.stiffness						= 100.0f;
	fluidDesc.viscosity						= gViscosity;
	fluidDesc.restDensity					= 1000.0f;
    fluidDesc.damping						= 0.0001f;
	fluidDesc.simulationMethod				= NX_F_SPH;
	if (!gHardwareSimulation)
        fluidDesc.flags &= ~NX_FF_HARDWARE;

	fluidDesc.initialParticleData			= initParticleData;

	//Create user fluid.
	//- create NxFluid in NxScene
	//- setup the buffers to read from data from the SDK
	//- set NxFluid::userData field to MyFluid instance
	NxVec3 color(0,0,0);
	bool trackUserData = false;
	bool provideCollisionNormals = false;
	MyFluid* fluid = new MyFluid(gScene, fluidDesc, trackUserData, provideCollisionNormals, color, 0.04f);
	assert(fluid);
	gMyFluids.pushBack(fluid);
	
	delete[] initParticles;

#ifdef __PPCGEKKO1__
	gCameraPos.set(0,0,15);
#else
	gCameraPos.set(0,0,30);
#endif	
	//gCameraPos.set(1,20,1);
	gCameraForward = gCenter - gCameraPos;
	gCameraForward.normalize();
}

void SampleForce::cleanup()
{
	//MyFluid cleaned up by ReleaseNx()
	delete[] gUpdates;
	gUpdates = NULL;
}

void SampleForce::update()
{
	if (!gMyFluids.size())
		return;

	if (!gUpdates)
	{
		gUpdates = new ParticleUpdateSDK[MAX_PARTICLES];
	}

	//Get particles from MyFluid and compute forces.
    MyFluid* fluid = gMyFluids[0];
	const ParticleSDK* particles = fluid->getParticles();
	unsigned particlesNum = fluid->getParticlesNum();

	NxReal planetRadius4 = gPlanetRadius*gPlanetRadius*gPlanetRadius*gPlanetRadius;

	for (unsigned i=0; i<particlesNum; i++)
	{
		const ParticleSDK& particle = particles[i];
		ParticleUpdateSDK& update = gUpdates[i];
		NxVec3& force = update.force;
		force.set(0,0,0);
		NxU32& flags = update.flags;
		flags = 0;
		//Dist vector
		NxVec3 distVec = particle.position - gCenter;
		NxReal dist = distVec.normalize();

		if (dist > gGenerationRadius*2)
		{
			flags |= NX_FP_DELETE;
			continue;
		}

		NxReal dist2 = dist*dist;
		NxReal dist4 = dist2*dist2;

		//Tangental vectors:
		NxVec3 tan0Vec = distVec.cross(gRotationAxis);
		tan0Vec.normalize();
		NxVec3 tan1Vec = tan0Vec.cross(distVec);
		tan1Vec.normalize();

		//Collision and gravity
		if (dist < gPlanetRadius)
		{
			NxReal dot = particle.velocity.dot(distVec);
			if (dot < 0.0f)
				force -= distVec*(dot);
		}
		else
		{
			force -= distVec*(gGravityPlanet/dist2);
		}

		if (dist > gAtmosphereRadius)
			continue;

		//Friction with planet surface
		//Compute local planet speed.
		NxReal temp = distVec.dot(gRotationAxis);
		NxReal planetVelAbs = (1 - temp*temp) * gRotationSpeed;
		NxVec3 planetVel = tan0Vec * planetVelAbs;

		//project particle velocity onto planet plane.
		NxVec3 projVel = particle.velocity - distVec * particle.velocity.dot(distVec);
		
		NxVec3 velDiff = planetVel - projVel;
		NxReal velDiffAbs = velDiff.magnitude();

		NxVec3 drag = velDiff * NxMath::min(1.0f, gFriction*velDiffAbs*velDiffAbs);
		force += drag;
	}

	NxParticleUpdateData updateData;
	updateData.forceMode = NX_VELOCITY_CHANGE;
	updateData.bufferForce = &gUpdates[0].force.x;
	updateData.bufferForceByteStride = sizeof(ParticleUpdateSDK);
	updateData.bufferFlag = &gUpdates[0].flags;
	updateData.bufferFlagByteStride = sizeof(ParticleUpdateSDK);

	fluid->getNxFluid()->updateParticles(updateData);
	
}

void SampleForce::onKeyPress(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '+': 
		gRotationSpeed *= 1.5f;
		break; 
	case '-': 
		gRotationSpeed /= 1.5f;
		break;
	default: 
		break;
	}
}

/**
Just a little helper to get more interesting start conditions.
Probably not the most efficient method.
*/
NxVec3 RandNormalVec(const NxVec3& mu, NxReal sig)
{
	NxReal x, y, z;
	NxReal s;

	//choose direction, uniformly distributed.
	do
	{
		x = NxMath::rand(-1.0f, 1.0f);
		y = NxMath::rand(-1.0f, 1.0f);
		z = NxMath::rand(-1.0f, 1.0f);
		s = x*x + y*y + z*z;
	} while (s >= 1.0f);

	NxVec3 dir(x,y,z);
	dir.normalize();

	
	//compute normal distributed magnitude.
	do
	{
		x = NxMath::rand(-1.0f, 1.0f);
		y = NxMath::rand(-1.0f, 1.0f);
		s = x*x + y*y;
	} while (s >= 1.0f);

	NxReal temp = NxMath::logE(1-s)/(1-s);
	NxReal r = sig * NxMath::sqrt(-2.0f*temp);

	return mu + dir * r*x;
}
