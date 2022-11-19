#include "SampleParticleFluid.h"
#include "MyFluid.h"

extern NxActor *gGroundPlane;
extern bool gHardwareSimulation;
extern NxScene* gScene;
extern NxArray<MyFluid*> gMyFluids;
extern NxVec3 gCameraPos;
extern NxVec3 gCameraForward;

#define MAX_PARTICLES 10000

unsigned gEmitterReservoir = 200;
static NxReal gVelocityMax = 11.0f;
static NxReal gVelocityMin = 8.0f;

#define NUM_PIPES 10
#define ORGAN_EXT 5.0f

class MyEmitter
{
public:

	MyEmitter(){}

	MyEmitter(bool right, NxFluidEmitter* nxEmitter):
		mRight(right), mCount(0), mUp(1), mNxEmitter(nxEmitter) 
	{}

	NxVec3 getPosition()
	{
		int pipe = mRight?mCount:-mCount;
		NxReal dist = ORGAN_EXT*2/NUM_PIPES;
		return NxVec3(pipe*dist, 0.3, 0);
	}

	NxReal getVelocity()
	{
		NxReal fraction = (mCount+1.0f) / NUM_PIPES;
		return gVelocityMax*(1-fraction) + gVelocityMin*(fraction);
	}

	void advancePosition()
	{
		if (mCount == NUM_PIPES-1)
			mUp = -mUp;

		mCount += mUp;

		if (mCount == 0)
			mUp = -mUp;
	}

	bool finishedLoop() { return (mCount == 1) && mUp == -1; }
	NxFluidEmitter* getNxEmitter() { return mNxEmitter; }

private:

	int mCount;
	bool mRight;
	int mUp;
	NxFluidEmitter* mNxEmitter;
};

static MyEmitter gMyEmitters[2];

class MyFluidUserNotify : public NxFluidUserNotify
{
public:
	virtual bool onEmitterEvent(NxFluidEmitter& emitter, NxFluidEmitterEventType eventType);
	virtual bool onEvent(NxFluid& fluid, NxFluidEventType eventType);

	virtual ~MyFluidUserNotify() {}  
};
static MyFluidUserNotify* gUserNotify = NULL;

void SampleEvents::setup()
{
	SetTitleString(getName());
	SetHelpString(NULL);
	
	NxVec3 fluidPos(0,0,0);

	// Create actors in the scene
	gGroundPlane = CreateGroundPlane();
	NxActor* frameActor = CreateBox(fluidPos, NxVec3(ORGAN_EXT*2,0.3,0.3), 0);
	assert(frameActor);

	// Setup notification for emitter
	gUserNotify = new MyFluidUserNotify();
	gScene->setFluidUserNotify(gUserNotify);


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
	MyFluid* fluid = new MyFluid(gScene, fluidDesc, trackUserData, provideCollisionNormals, NxVec3(1.0f, 0.5f, 0.2f), 0.05f);
	assert(fluid);
	gMyFluids.pushBack(fluid);

	//Create an attached emitter
	NxFluidEmitterDesc emitterDesc;

	emitterDesc.maxParticles = gEmitterReservoir;
	emitterDesc.dimensionX = 0.2f;
	emitterDesc.dimensionY = 0.2f;
	emitterDesc.randomAngle = 0.001f;
	emitterDesc.randomPos = NxVec3(0.0f,0.0f,0.0f);
	//no effect with type NX_FE_CONSTANT_PRESSURE
	emitterDesc.rate = 300.0f;
	emitterDesc.shape = NX_FE_RECTANGULAR;
	emitterDesc.type = NX_FE_CONSTANT_PRESSURE;
#ifdef __PPCGEKKO1__
	emitterDesc.particleLifetime = 2.6f;
#else	
	emitterDesc.particleLifetime = 6.0f;
#endif

	//attach to actor
	emitterDesc.flags |= NX_FEF_ADD_BODY_VELOCITY;
	emitterDesc.frameShape = frameActor->getShapes()[0];
	emitterDesc.relPose.M.id();
	emitterDesc.relPose.M.rotX(-NxHalfPiF32*(0.99f));
	emitterDesc.relPose.t = fluidPos;
	
	NxFluidEmitter* emitter0 = fluid->getNxFluid()->createEmitter(emitterDesc);
	NxFluidEmitter* emitter1 = fluid->getNxFluid()->createEmitter(emitterDesc);

	gMyEmitters[0] = MyEmitter(true, emitter0); 
	gMyEmitters[1] = MyEmitter(false, emitter1); 
	emitter0->userData = gMyEmitters + 0;
	emitter1->userData = gMyEmitters + 1;

	emitter0->setLocalPosition(gMyEmitters[0].getPosition());
	emitter0->setFluidVelocityMagnitude(gMyEmitters[0].getVelocity());
	emitter1->setLocalPosition(gMyEmitters[1].getPosition());
	emitter1->setFluidVelocityMagnitude(gMyEmitters[1].getVelocity());

	gCameraPos.set(1.43f,10.96f,17.9f);
	gCameraForward = fluidPos - gCameraPos;
	gCameraForward.normalize();

}

void SampleEvents::cleanup()
{
	//MyFluid cleaned up by ReleaseNx()
	//delete gUserNotify;
}

bool MyFluidUserNotify::onEmitterEvent(NxFluidEmitter& emitter, NxFluidEmitterEventType eventType)
{
	switch (eventType)
	{
	case NX_FEET_EMITTER_EMPTY:
		{
			MyEmitter& myEmitter = *reinterpret_cast<MyEmitter*>(emitter.userData); 
			myEmitter.advancePosition();
			emitter.setLocalPosition(myEmitter.getPosition());
			emitter.setFluidVelocityMagnitude(myEmitter.getVelocity());

			if (!myEmitter.finishedLoop())
			{
				emitter.resetEmission(gEmitterReservoir);
			}
		}
		break;
	default:
		break;
	}

	return false;
}

bool MyFluidUserNotify::onEvent(NxFluid& fluid, NxFluidEventType eventType)
{
	switch (eventType)
	{
	case NX_FET_NO_PARTICLES_LEFT:
		{
		gMyEmitters[0].getNxEmitter()->resetEmission(gEmitterReservoir);
		gMyEmitters[1].getNxEmitter()->resetEmission(gEmitterReservoir);
		}
		break;
	default:
		break;
	}

	return false;
}

