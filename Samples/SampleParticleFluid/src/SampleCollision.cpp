#include "SampleParticleFluid.h"
#include "MyFluid.h"
#include "cooking.h"
#include "UserAllocator.h"
#include "MediaPath.h"

extern bool gHardwareSimulation;
extern NxScene* gScene;
extern NxArray<MyFluid*> gMyFluids;
extern NxVec3 gCameraPos;
extern NxVec3 gCameraForward;
extern bool gShadows;
extern ErrorStream gErrorStream;
extern UserAllocator* gAllocator;

#define MAX_PARTICLES 32768
#define REST_PARTICLES_PER_METER 10.f
#define KERNEL_RADIUS_MULTIPLIER 1.8f
#define MOTION_LIMIT_MULTIPLIER 3
#define PACKET_SIZE_MULTIPLIER 8
#define COLLISION_DISTANCE_MULTIPLIER 0.18f

void CreateDynamicActors();

void SampleCollision::setup()
{

	SetTitleString(getName());
#ifdef __PPCGEKKO__
	SetHelpString("    a: create rigid bodies");
#else	
	SetHelpString("    b: create rigid bodies");
#endif	

	gShadows = false;

	// Create objects in the scene

	if (!InitCooking(gAllocator, &gErrorStream)) 
	{
		printf("\nError: Unable to initialize the cooking library, exiting the sample.\n\n");
		return;
	}

	// Load ASE file
	CookASE("fluidSample.ase", gScene, NxVec3(1,10,0));
	CookASE("coolFlow.ase", gScene, NxVec3(1,6,-0), NxVec3(1,0.2,1));
	CloseCooking();

	// Add a box shaped drain.
	NxActorDesc boxDrainActor;
	NxBoxShapeDesc boxDrainShape;
	boxDrainActor.shapes.pushBack(&boxDrainShape);
	boxDrainShape.dimensions.set(40,1,40);
	boxDrainShape.shapeFlags |= NX_SF_FLUID_DRAIN;
	boxDrainActor.globalPose.t.set(0, 0, 0);
	gScene->createActor(boxDrainActor);

	//Pre cook hotspots
	NxBounds3 precookAABB;
	precookAABB.set(NxVec3(-20,-20,-20), NxVec3(20,20,20));
//	gScene->cookFluidMeshHotspot(precookAABB, PACKET_SIZE_MULTIPLIER, REST_PARTICLES_PER_METER, KERNEL_RADIUS_MULTIPLIER, MOTION_LIMIT_MULTIPLIER, COLLISION_DISTANCE_MULTIPLIER );


	//Create a set of initial particles
	ParticleSDK*	initParticles = new ParticleSDK[MAX_PARTICLES];
	unsigned initParticlesNum = 0;

	NxVec3 fluidPos(0, 11.6, 0);
	float distance = 0.1f;
	unsigned sideNum = 16;	
	float rad = sideNum*distance*0.5f;
	for (unsigned i=0; i<sideNum; i++)
		for (unsigned j=0; j<sideNum; j++)
			for (unsigned k=0; k<sideNum; k++)
			{
				NxVec3 p = NxVec3(i*distance,j*distance,k*distance);
				if (p.distance(NxVec3(rad,rad,rad)) < rad)
				{
					p += fluidPos;
					ParticleSDK& newParticle = initParticles[initParticlesNum++];
					newParticle.position = p;
					newParticle.velocity = NxVec3(0,0,0);
				}
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
	fluidDesc.maxParticles                  	= initParticlesNum;
	fluidDesc.kernelRadiusMultiplier			= KERNEL_RADIUS_MULTIPLIER;
	fluidDesc.restParticlesPerMeter				= REST_PARTICLES_PER_METER;
	fluidDesc.collisionDistanceMultiplier   	= COLLISION_DISTANCE_MULTIPLIER;
	fluidDesc.stiffness							= 50.0f;
	fluidDesc.viscosity							= 22.0f;
	fluidDesc.damping							= 0.0f;
	fluidDesc.restitutionForStaticShapes		= 0.4f;
	fluidDesc.dynamicFrictionForStaticShapes	= 0.03f;
	fluidDesc.simulationMethod					= NX_F_SPH; //NX_F_NO_PARTICLE_INTERACTION;

	if (!gHardwareSimulation)
		fluidDesc.flags &= ~NX_FF_HARDWARE;

	fluidDesc.initialParticleData			= initParticleData;

	//Create user fluid.
	//- create NxFluid in NxScene
	//- setup the buffers to read from data from the SDK
	//- set NxFluid::userData field to MyFluid instance
	bool trackUserData = false;
	bool provideCollisionNormals = false;
	MyFluid* fluid = new MyFluid(gScene, fluidDesc, trackUserData, provideCollisionNormals, NxVec3(0.2f,0.3f,0.7f), 0.03f);
	assert(fluid);
	gMyFluids.pushBack(fluid);

	delete[] initParticles;

	gCameraPos.set(23, 14, 23);
	gCameraForward = fluidPos - NxVec3(0, 3, 0) - gCameraPos;
	gCameraForward.normalize();

}

void SampleCollision::cleanup()
{
	//MyFluid cleaned up by ReleaseNx()
}

void SampleCollision::onKeyPress(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'b': 
		CreateDynamicActors();
		break; 
	default: 
		break;
	}
}

void CreateDynamicActors() 
{
	WaitForPhysics();

	// Add a dynamic sphere.
	NxActorDesc sphereActor;
	NxBodyDesc sphereBody;
	NxSphereShapeDesc sphereShape;
	sphereShape.radius = 0.4f;
	sphereActor.shapes.pushBack(&sphereShape);
	sphereActor.body = &sphereBody;
	sphereActor.density = 1.0f;
	sphereActor.globalPose.t.set(0.0f, 14.0f, 1.0f);
	gScene->createActor(sphereActor);

	// Add a dynamic capsule.
	NxActorDesc capsActor;
	NxCapsuleShapeDesc capsShape;
	NxBodyDesc capsBody;
	capsActor.shapes.pushBack(&capsShape);
	capsActor.body = &capsBody;
	capsActor.density = 1.0f;
	capsActor.globalPose.t.set(3.0f, 10.0f, 8.0f);
	gScene->createActor(capsActor);

	// Add a dynamic box.
	NxActorDesc boxActor;
	NxBoxShapeDesc boxShape;
	NxBodyDesc boxBody;
	boxShape.dimensions.set(0.75f, 0.75f, 0.75f);
	boxActor.shapes.pushBack(&boxShape);
	boxActor.body = &boxBody;
	boxActor.density = 1.0f;
	boxActor.globalPose.t.set(2.0f, 15.0f, 7.0f);
	gScene->createActor(boxActor);
}
