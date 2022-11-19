#include "SampleParticleFluid.h"
#include "MyFluid.h"
#include "ParticleFactory.h"

extern NxActor *gGroundPlane;
extern bool gHardwareSimulation;
extern NxScene* gScene;
extern NxPhysicsSDK* gPhysicsSDK;
extern NxArray<MyFluid*> gMyFluids;
extern NxVec3 gCameraPos;
extern NxVec3 gCameraForward;
extern bool gWireframeMode;
extern bool gShadows;

#ifdef __PPCGEKKO__
#define MAX_PARTICLES 1000
#else
#define MAX_PARTICLES 10000
#endif

NxFluidPacketData		gFluidPacketData;
NxU32					gNumPackets;

static unsigned			gNewParticlesMax = MAX_PARTICLES;
static unsigned			gNewParticlesNum = 0;
static ParticleSDK*		gNewParticles = NULL;
static NxParticleData	gNewParticleData;

static NxActor*			gSphere = 0;
static float			gSphereRadius = 3.0f;
static float			gDistance = 0.2f;
static float			gLifetime = 0.0f;

void SamplePacketData::setup()
{
	SetTitleString(getName());
#ifdef __PPCGEKKO__
	SetHelpString("    a: throw scoop");
#else	
	SetHelpString("    b: throw scoop");
#endif

	gNewParticles = new ParticleSDK[gNewParticlesMax];
	gFluidPacketData.bufferFluidPackets = new NxFluidPacket[(NxU32)gPhysicsSDK->getParameter(NX_CONSTANT_FLUID_MAX_PACKETS)];
	gFluidPacketData.numFluidPacketsPtr = &gNumPackets;

	//Setup structure which wraps the buffers for particle creation.
	gNewParticleData.numParticlesPtr		= &gNewParticlesNum;
	gNewParticleData.bufferPos				= &gNewParticles[0].position.x;
	gNewParticleData.bufferPosByteStride	= sizeof(ParticleSDK);
	gNewParticleData.bufferVel				= &gNewParticles[0].velocity.x;
	gNewParticleData.bufferVelByteStride	= sizeof(ParticleSDK);
	gNewParticleData.bufferLife				= &gNewParticles[0].lifetime;
	gNewParticleData.bufferLifeByteStride	= sizeof(ParticleSDK);

	gPhysicsSDK->setParameter(NX_VISUALIZE_FLUID_PACKET_DATA, 1.0f);
	//gPhysicsSDK->setParameter(NX_VISUALIZE_FLUID_PACKETS, 1.0f);

	NxVec3 fluidPos(0,1,0);

	// Create actors in the scene
	CreateGroundPlane(-3.0f)->getShapes()[0]->setGroup(1);
	gGroundPlane = CreateGroundPlane();

#ifdef __PPCGEKKO1__
	CreateBox(NxVec3(  0, 0, 4), NxVec3(4.3,0.5,0.3), 0)->getShapes()[0]->setGroup(2);
	CreateBox(NxVec3(  0, 0,-4), NxVec3(4.3,0.5,0.3), 0)->getShapes()[0]->setGroup(2);
	CreateBox(NxVec3( 4, 0,  0), NxVec3(0.3,0.5,4.3), 0)->getShapes()[0]->setGroup(2);
	CreateBox(NxVec3(-4, 0,  0), NxVec3(0.3,0.5,4.3), 0)->getShapes()[0]->setGroup(2);
#else
	CreateBox(NxVec3(  0, 0, 11), NxVec3(11.3,2.5,0.3), 0)->getShapes()[0]->setGroup(2);
	CreateBox(NxVec3(  0, 0,-11), NxVec3(11.3,2.5,0.3), 0)->getShapes()[0]->setGroup(2);
	CreateBox(NxVec3( 11, 0,  0), NxVec3(0.3,2.5,11.3), 0)->getShapes()[0]->setGroup(2);
	CreateBox(NxVec3(-11, 0,  0), NxVec3(0.3,2.5,11.3), 0)->getShapes()[0]->setGroup(2);
#endif

	gSphere = CreateSphere(NxVec3(  0, -gSphereRadius, 0), gSphereRadius, 1.0f);
	gSphere->raiseActorFlag(NX_AF_FLUID_DISABLE_COLLISION);
	gSphere->getShapes()[0]->setGroup(1);
	
	// disable collision for sphere and upper plane
	gScene->setGroupCollisionFlag(0, 1, false);

	//Setup fluid descriptor
	NxFluidDesc fluidDesc;
    fluidDesc.maxParticles                  = MAX_PARTICLES;
    fluidDesc.kernelRadiusMultiplier		= 2.0f;
#ifdef __PPCGEKKO1__
    fluidDesc.restParticlesPerMeter			= 0.9f/gDistance;
    fluidDesc.packetSizeMultiplier			= 4;
    fluidDesc.stiffness						= 80.0f;
    fluidDesc.viscosity						= 80.0f;
	fluidDesc.motionLimitMultiplier			= 3.0f;
    fluidDesc.collisionDistanceMultiplier   = 0.1;
#else
    fluidDesc.restParticlesPerMeter			= 1.0f/gDistance;
	fluidDesc.motionLimitMultiplier			= 3.0f;
	fluidDesc.packetSizeMultiplier			= 8;
    fluidDesc.collisionDistanceMultiplier   = 0.1;
    fluidDesc.stiffness						= 50.0f;
    fluidDesc.viscosity						= 40.0f;
#endif    
	fluidDesc.restDensity					= 1000.0f;
    fluidDesc.damping						= 0.0f;
    fluidDesc.restitutionForStaticShapes	= 0.3f;
	fluidDesc.dynamicFrictionForStaticShapes= 0.05f;
	fluidDesc.simulationMethod				= NX_F_SPH;
	fluidDesc.fluidPacketData				= gFluidPacketData;
	if (!gHardwareSimulation)
        fluidDesc.flags &= ~NX_FF_HARDWARE;
	
	NxVec3 fluidVel;
	fluidPos.set(0,4,0);
	fluidVel.set(0,0,0);
	
	NxBounds3 aabb;
#ifdef __PPCGEKKO1__
	aabb.set(NxVec3(-2.8,0,-2.8), NxVec3(2.8,0,2.8));
	CreateParticleAABB(gNewParticleData, gNewParticlesMax, gNewParticlesMax, false, aabb, fluidVel, gLifetime, gDistance);
#else	
	aabb.set(NxVec3(-6.8,0,-6.8), NxVec3(6.8,0,6.8));
	CreateParticleAABB(gNewParticleData, gNewParticlesMax, gNewParticlesMax, false, aabb, fluidVel, gLifetime, gDistance);
	aabb.set(NxVec3(5,0,5), NxVec3(10,0,10));
	CreateParticleAABB(gNewParticleData, gNewParticlesMax, gNewParticlesMax, true, aabb, fluidVel, gLifetime, gDistance);
#endif

    fluidDesc.initialParticleData = gNewParticleData;

	//Create user fluid.
	//- create NxFluid in NxScene
	//- setup the buffers to read from data from the SDK
	//- set NxFluid::userData field to MyFluid instance
	bool trackUserData = true;
	bool provideCollisionNormals = false;
	MyFluid* fluid = new MyFluid(gScene, fluidDesc, trackUserData, provideCollisionNormals, NxVec3(0.3f,0.4f,0.7f), 0.03f);
	assert(fluid);
	gMyFluids.pushBack(fluid);
#ifdef __PPCGEKKO1__
	gCameraPos.set(6.51f,13.40f,10.43f);
#else
	gCameraPos.set(1.43f,10.96f,17.9f);
#endif	
	gCameraForward = fluidPos - gCameraPos;
	gCameraForward.normalize();
	gShadows = false;
	gWireframeMode = true;
}

void SamplePacketData::cleanup()
{
	//MyFluid cleaned up by ReleaseNx()
	delete[] gFluidPacketData.bufferFluidPackets;
	delete[] gNewParticles;
	gFluidPacketData.bufferFluidPackets = NULL;
}

void SamplePacketData::update()
{
	if (!gMyFluids.size())
		return;

    MyFluid* fluid = gMyFluids[0];

	// walk over packets and color fluid particles depending on their position relative to the big sphere.
	for(NxU32 p = 0; p < *gFluidPacketData.numFluidPacketsPtr; p++)
	{
		NxVec3 center, extents;
		NxFluidPacket& packet = gFluidPacketData.bufferFluidPackets[p];
		packet.aabb.fatten(gSphereRadius);
		bool insideAABB = packet.aabb.contain(gSphere->getShapes()[0]->getGlobalPosition());
		for(NxU32 i = packet.firstParticleIndex; i < packet.firstParticleIndex + packet.numParticles; i++)
		{
			const ParticleSDK& particle = fluid->getParticles()[i];
			MyParticle& myParticle = fluid->getMyParticles()[particle.id];
			if(insideAABB)
			{	
				if(particle.position.distance(gSphere->getShapes()[0]->getGlobalPosition()) <= gSphereRadius)
				{
					myParticle.rgba[0] = 1.0f;
					myParticle.rgba[1] = 0.0f;
					myParticle.rgba[2] = 0.0f;
					myParticle.rgba[3] = 1.0f;
				}
				else
				{
					myParticle.rgba[0] = 1.0f;
					myParticle.rgba[1] = 1.0f;
					myParticle.rgba[2] = 0.0f;
					myParticle.rgba[3] = 1.0f;
				}
			}
			else
			{
				myParticle.rgba[0] = 0.0f;
				myParticle.rgba[1] = 0.0f;
				myParticle.rgba[2] = 1.0f;
				myParticle.rgba[3] = 1.0f;
			}
		}
	}
}

void SamplePacketData::onKeyPress(unsigned char key, int x, int y)
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
			
#ifdef __PPCGEKKO1__
			NxVec3 fluidVel = gCameraForward + NxVec3(0,0.65f,0);
			fluidVel.setMagnitude(8);
			CreateParticleSphere(gNewParticleData, gNewParticlesMax, false, fluidPos, fluidVel, 5.0, gDistance, 6);			
#else					
			NxVec3 fluidVel = gCameraForward + NxVec3(0,0.5f,0);
			fluidVel.setMagnitude(15);

			CreateParticleSphere(gNewParticleData, gNewParticlesMax, false, fluidPos, fluidVel, 5.0, gDistance, 8);
#endif
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

