#include "SampleParticleFluid.h"
#include "MyFluid.h"
#include "ParticleFactory.h"

extern NxActor *gGroundPlane;
extern bool gHardwareSimulation;
extern NxScene* gScene;
extern NxArray<MyFluid*> gMyFluids;
extern NxVec3 gCameraPos;
extern NxVec3 gCameraForward;

#define MAX_PARTICLES 8000
static unsigned			gNewParticlesMax = MAX_PARTICLES/5;
static unsigned			gNewParticlesNum = 0;
static ParticleSDK*		gNewParticles = NULL;
static NxParticleData	gNewParticleData;
static float			gDistance = 0.1f;
#ifdef __PPCGEKKO1__
static unsigned			gSideNum = 9;
#else
static unsigned			gSideNum = 14;
#endif
static float			gLifetime = 100.0f;



void SampleUserData::setup()
{
	SetTitleString(getName());
#ifdef __PPCGEKKO__
	SetHelpString("    a: throw scoop");
#else	
	SetHelpString("    b: throw scoop");
#endif

	gNewParticles = new ParticleSDK[gNewParticlesMax];

	// Create objects in the scene
	gGroundPlane = CreateGroundPlane();
	
	//Setup structure which wraps the buffers for particle creation.
	gNewParticleData.numParticlesPtr		= &gNewParticlesNum;
	gNewParticleData.bufferPos				= &gNewParticles[0].position.x;
	gNewParticleData.bufferPosByteStride	= sizeof(ParticleSDK);
	gNewParticleData.bufferVel				= &gNewParticles[0].velocity.x;
	gNewParticleData.bufferVelByteStride	= sizeof(ParticleSDK);
	gNewParticleData.bufferLife				= &gNewParticles[0].lifetime;
	gNewParticleData.bufferLifeByteStride	= sizeof(ParticleSDK);

	//Setup fluid descriptor
	NxFluidDesc fluidDesc;
    fluidDesc.maxParticles                  = MAX_PARTICLES;
    fluidDesc.kernelRadiusMultiplier		= 2.0f;
    fluidDesc.restParticlesPerMeter			= 10.0f;
	fluidDesc.motionLimitMultiplier			= 3.0f;
	fluidDesc.packetSizeMultiplier			= 8;
    fluidDesc.collisionDistanceMultiplier   = 0.1;
    fluidDesc.stiffness						= 50.0f;
    fluidDesc.viscosity						= 40.0f;
	fluidDesc.restDensity					= 1000.0f;
    fluidDesc.damping						= 0.0f;
    fluidDesc.restitutionForStaticShapes	= 0.3f;
	fluidDesc.dynamicFrictionForStaticShapes= 0.05f;
	fluidDesc.simulationMethod				= NX_F_SPH;

	//Switch to priority queue modus, in order to always be able to add a scoop of particles.
	fluidDesc.flags |= NX_FF_PRIORITY_MODE;
	fluidDesc.numReserveParticles = gNewParticlesMax;

	if (!gHardwareSimulation)
        fluidDesc.flags &= ~NX_FF_HARDWARE;

	//Add the first particle scoop to the fluidDesc.
	NxVec3 fluidPos;
	NxVec3 fluidVel;
	
	fluidPos.set(0,4,0);
	fluidVel.set(0,0,0);
	CreateParticleSphere(gNewParticleData, gNewParticlesMax, false, fluidPos, fluidVel, gLifetime, gDistance, gSideNum);

    fluidDesc.initialParticleData = gNewParticleData;

	//Create user fluid.
	//- create NxFluid in NxScene
	//- setup the buffers to read from data from the SDK
	//- set NxFluid::userData field to MyFluid instance
	bool trackUserData = true;
	bool provideCollisionNormals = false;
	MyFluid* fluid = new MyFluid(gScene, fluidDesc, trackUserData, provideCollisionNormals, NxVec3(1,1,1), 0.03f);
	assert(fluid);
	gMyFluids.pushBack(fluid);

	//Set color for first scoop --> red
	//The NxScene::createFluid() function adds the NxFluidDesc::initialParticleData to the SDK. 
	//It also writes to the MyFluid::mCreatedParticleIds buffer, which was passed to the SDK
	//(NxFluidDesc::particleCreationIdWriteData). See MyFluid ctor.
	for (unsigned i=0; i<fluid->getCreatedIdsNum(); i++)
	{
		unsigned id = fluid->getCreatedIds()[i];
		MyParticle& myParticle = fluid->getMyParticles()[id];
		//myParticle.color = NxVec3(NxMath::rand(0.4f,0.5f), NxMath::rand(0.1f,0.2f), NxMath::rand(0.1f,0.2f));
		myParticle.rgba[0] = NxMath::rand(0.4f,0.5f);
		myParticle.rgba[1] = NxMath::rand(0.1f,0.2f);
		myParticle.rgba[2] = NxMath::rand(0.1f,0.2f);
		myParticle.rgba[3] = 1.0f;
	}

	//Create another two scoops. This time with NxFluid::addParticles().
	//The NxFluid::addParticles() function adds new particles to the SDK. 
	//It also writes to the MyFluid::mCreatedParticleIds buffer, which was passed to the SDK
	//(NxFluidDesc::particleCreationIdWriteData). See MyFluid ctor.
	fluidPos.set(-2,2,0);
	fluidVel.set(5,0,0);
	CreateParticleSphere(gNewParticleData, gNewParticlesMax, false, fluidPos, fluidVel, gLifetime, gDistance, gSideNum);
	fluid->getNxFluid()->addParticles(gNewParticleData);
	
	//Set scoop color --> blue.
	for (unsigned i=0; i<fluid->getCreatedIdsNum(); i++)
	{
		unsigned id = fluid->getCreatedIds()[i];
		MyParticle& myParticle = fluid->getMyParticles()[id];
		//myParticle.color = NxVec3(NxMath::rand(0.2f,0.3f), NxMath::rand(0.3f,0.4f), NxMath::rand(0.7f,1.0f));
		myParticle.rgba[0] = NxMath::rand(0.2f,0.3f);
		myParticle.rgba[1] = NxMath::rand(0.3f,0.4f);
		myParticle.rgba[2] = NxMath::rand(0.7f,1.0f);
		myParticle.rgba[3] = 1.0f;
	}

	fluidPos.set(2,2,0);
	fluidVel.set(-5,0,0);
	CreateParticleSphere(gNewParticleData, gNewParticlesMax, false, fluidPos, fluidVel, gLifetime, gDistance, gSideNum);
	fluid->getNxFluid()->addParticles(gNewParticleData);

	//Set scoop color --> green.
	for (unsigned i=0; i<fluid->getCreatedIdsNum(); i++)
	{
		unsigned id = fluid->getCreatedIds()[i];
		MyParticle& myParticle = fluid->getMyParticles()[id];
		//myParticle.color = NxVec3(NxMath::rand(0.2f,0.3f), NxMath::rand(0.7f,1.0f), NxMath::rand(0.3f,0.4f));
		myParticle.rgba[0] = NxMath::rand(0.2f,0.3f);
		myParticle.rgba[1] = NxMath::rand(0.7f,1.0f);
		myParticle.rgba[2] = NxMath::rand(0.3f,0.4f);
		myParticle.rgba[3] = 1.0f;
	}

	gCameraPos.set(1.43f,10.96f,17.9f);
	gCameraForward = fluidPos - gCameraPos;
	gCameraForward.normalize();
}

void SampleUserData::cleanup()
{
	//MyFluid cleaned up by ReleaseNx()
	delete[] gNewParticles;
	gNewParticles = NULL;

}

void SampleUserData::update()
{
}

void SampleUserData::onKeyPress(unsigned char key, int x, int y)
{
	if(!gMyFluids.size())
		return;

    MyFluid* fluid = gMyFluids[0];

	switch(key)
	{
	case 'b':
		{
			//Throw a small scoop of random color.
			//Note: if to many particles are added (exceeding of per frame limitation or hitting NxFluid::getMaxParticles()). 
			//a warning appears in the console.
			NxVec3 baseColor(NxMath::rand(0.2f,0.8f), NxMath::rand(0.2f,0.8f), NxMath::rand(0.2f,0.8f));

			NxVec3 fluidPos = gCameraPos + NxVec3(0,-2,0);
			NxVec3 fluidVel = gCameraForward + NxVec3(0,0.5f,0);
			fluidVel.setMagnitude(15);

			CreateParticleSphere(gNewParticleData, gNewParticlesMax, false, fluidPos, fluidVel, gLifetime, gDistance, 10);
			fluid->getNxFluid()->addParticles(gNewParticleData);

			for(unsigned i=0; i<fluid->getCreatedIdsNum(); i++)
			{
				unsigned id = fluid->getCreatedIds()[i];
				MyParticle& myParticle = fluid->getMyParticles()[id];
				myParticle.rgba[0] = NxMath::rand(baseColor.x - 0.2f, baseColor.x + 0.2f);
				myParticle.rgba[1] = NxMath::rand(baseColor.y - 0.2f, baseColor.y + 0.2f);
				myParticle.rgba[2] = NxMath::rand(baseColor.z - 0.2f, baseColor.z + 0.2f);
				myParticle.rgba[3] = 1.0f;
			}
		}
		break; 
	default: 
		break;
	}
}

