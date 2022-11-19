#include "SampleParticleFluid.h"
#include "MyFluid.h"

extern NxActor *gGroundPlane;
extern bool gHardwareSimulation;
extern NxScene* gScene;
extern NxArray<MyFluid*> gMyFluids;
extern NxVec3 gCameraPos;
extern NxVec3 gCameraForward;

#define MAX_PARTICLES 20000

static NxFluidEmitter* gEmitter;

void SampleEmitter::setup()
{
	SetTitleString(getName());
#ifdef __PPCGEKKO__
	SetHelpString("    a: toggle shape");
#else	
	SetHelpString("    r: toggle repulsion on body\n    b: toggle shape");
#endif	

	NxVec3 fluidPos(0,1,0);

	// Create actors in the scene
	gGroundPlane = CreateGroundPlane();
	NxActor* frameActor = CreateBox(fluidPos, NxVec3(1,1,1),2);
	assert(frameActor);

	CreateBox(NxVec3(  0, 0, 20), NxVec3(20.3,0.3,0.3), 0);
	CreateBox(NxVec3(  0, 0,-20), NxVec3(20.3,0.3,0.3), 0);
	CreateBox(NxVec3( 20, 0,  0), NxVec3(0.3,0.3,20.3), 0);
	CreateBox(NxVec3(-20, 0,  0), NxVec3(0.3,0.3,20.3), 0);


	//Setup fluid descriptor
	NxFluidDesc fluidDesc;
    fluidDesc.maxParticles                  = MAX_PARTICLES;
    fluidDesc.kernelRadiusMultiplier		= 2.0f;
    fluidDesc.restParticlesPerMeter			= 7.0f;
	fluidDesc.motionLimitMultiplier			= 3.0f;
	fluidDesc.packetSizeMultiplier			= 8;
    fluidDesc.collisionDistanceMultiplier   = 0.1;
    fluidDesc.stiffness						= 50.0f;
    fluidDesc.viscosity						= 40.0f;
	fluidDesc.restDensity					= 1000.0f;
    fluidDesc.damping						= 0.0f;
    fluidDesc.restitutionForStaticShapes	= 0.2f;
	fluidDesc.dynamicFrictionForStaticShapes= 0.05f;
	fluidDesc.simulationMethod				= NX_F_SPH;
	if (!gHardwareSimulation)
        fluidDesc.flags &= ~NX_FF_HARDWARE;

	//Create user fluid.
	//- create NxFluid in NxScene
	//- setup the buffers to read from data from the SDK
	//- set NxFluid::userData field to MyFluid instance
	bool trackUserData = false;
	bool provideCollisionNormals = false;
	MyFluid* fluid = new MyFluid(gScene, fluidDesc, trackUserData, provideCollisionNormals, NxVec3(0.3f,0.4f,0.7f), 0.03f);
	assert(fluid);
	gMyFluids.pushBack(fluid);

	gEmitter = NULL;

	//Create an attached emitter
	NxFluidEmitterDesc emitterDesc;

	emitterDesc.maxParticles = 0;
//#ifdef __PPCGEKKO__
//	emitterDesc.randomAngle = 0.001f;
//	emitterDesc.randomPos = NxVec3(0.0f,0.0f,0.0f);
//#else	
	emitterDesc.dimensionX = 0.3f;
	emitterDesc.dimensionY = 0.3f;
//#endif	
	emitterDesc.type = NX_FE_CONSTANT_FLOW_RATE;
	//The rate has no effect with type NX_FE_CONSTANT_PRESSURE
//#ifdef __PPCGEKKO__
//	emitterDesc.rate = 230.0f;
//#else	
	emitterDesc.rate = 300.0f;
//#endif	
	emitterDesc.fluidVelocityMagnitude = 10.0f;
	emitterDesc.particleLifetime = 5.0f;
	emitterDesc.shape = NX_FE_RECTANGULAR;
	emitterDesc.particleLifetime = 5.0f;

	//attach to actor
	emitterDesc.flags |= NX_FEF_ADD_BODY_VELOCITY;
	emitterDesc.repulsionCoefficient = 0.02f;

	emitterDesc.relPose.M.id();
	emitterDesc.relPose.M.rotX(-NxHalfPiF32);
	emitterDesc.relPose.t = NxVec3(0,1.1f,0);

	emitterDesc.frameShape = frameActor->getShapes()[0];
	gEmitter = fluid->getNxFluid()->createEmitter(emitterDesc);

	gCameraPos.set(1.43f,10.96f,17.9f);
	gCameraForward = fluidPos - gCameraPos;
	gCameraForward.normalize();

}

void SampleEmitter::cleanup()
{
	//MyFluid cleaned up by ReleaseNx()
}

void SampleEmitter::onKeyPress(unsigned char key, int x, int y)
{
	if (!gEmitter) 
		return;

	switch (key)
	{
	case 'r': 
		{
			bool repulsion = gEmitter->getFlag(NX_FEF_FORCE_ON_BODY) > 0;
			gEmitter->setFlag(NX_FEF_FORCE_ON_BODY, !repulsion);
		}
		break; 
	case 'b': 
		{
			NxShape* frameShape = gEmitter->getFrameShape();
			assert(frameShape);
			NxActor& oldActor = frameShape->getActor();
	
			NxActor* newActor = NULL;
			if (frameShape->getType() == NX_SHAPE_BOX)
			{
				newActor = CreateCapsule(NxVec3(0,0,0), 0.4f, 1.2f, 2);
			}
			else
			{
				newActor = CreateBox(NxVec3(0,0,0), NxVec3(1,1,1), 2);
			}
			newActor->setGlobalPose(oldActor.getGlobalPose());

			gScene->releaseActor(oldActor);
			gEmitter->setFrameShape(newActor->getShapes()[0]);
		}
		break; 
	default: 
		break;
	}
}
