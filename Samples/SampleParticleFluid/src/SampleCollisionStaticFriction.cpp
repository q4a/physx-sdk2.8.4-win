#include "SampleParticleFluid.h"
#include "MyFluid.h"
#include "utils/ParticleFactory.h"


//----------------------------------------------------------------------------//

#define MAX_PARTICLES		30000

extern NxPhysicsSDK*		gPhysicsSDK;
extern NxActor*				gGroundPlane;
extern bool					gHardwareSimulation;
extern NxScene*				gScene;
extern NxArray<MyFluid*>	gMyFluids;

static unsigned				gNewParticlesMax = MAX_PARTICLES;
static unsigned				gNewParticlesNum = 0;
static ParticleSDK*			gNewParticles = NULL;
static NxParticleData		gNewParticleData;
extern NxActor*				gGroundPlane;

extern NxVec3				gCameraPos;
extern NxVec3				gCameraForward;


extern TerrainData*			gTerrainData;
extern NxActor*				gTerrain;
extern bool					gShadows;
extern NxReal				gTime;
extern NxReal				gLastTime;
extern NxVec3				gDefaultGravity;

static NxReal				gTimeMark;
static NxVec3				gWindForce;
static NxVec3				gWindForceTarget;



//----------------------------------------------------------------------------//

static NxVec3 RandNormalVec(const NxVec3& mu, NxReal sig);
static void CreatePhysicsTerrain();
static void GenerateCirclePts(unsigned int nb, NxVec3* pts, float scale, float z);

//----------------------------------------------------------------------------//

static const bool gTestAgainstStaticShape = true;
static NxActor* gKinematicGround = NULL;

void SampleCollisionStaticFriction::setup()
{
	SetTitleString("Collision Static Friction");
	SetHelpString(NULL);
	
	//Create objects in the scene
	gGroundPlane = CreateGroundPlane(-15.0f);

	srand(16);
	if (gTestAgainstStaticShape)
	{
		CreatePhysicsTerrain();
	}
	else
	{
		gKinematicGround = CreateBox(NxVec3(0,-5,0), NxVec3(20,1,20), 1.0f);
		gKinematicGround->raiseBodyFlag(NX_BF_KINEMATIC);
	}

	gTimeMark = 3.0f;
	gWindForce.zero();
	gWindForceTarget.zero();

	gNewParticles = new ParticleSDK[gNewParticlesMax];

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
    fluidDesc.restParticlesPerMeter			= 7.0f;
	fluidDesc.motionLimitMultiplier			= 3.0f;
	fluidDesc.packetSizeMultiplier			= 8;
    fluidDesc.collisionDistanceMultiplier   = 0.1;
    fluidDesc.stiffness						= 50.0f;
    fluidDesc.viscosity						= 40.0f;
	fluidDesc.restDensity					= 1000.0f;
    fluidDesc.damping						= 0.0f;
	fluidDesc.restitutionForStaticShapes	= 0.0f;
	fluidDesc.restitutionForDynamicShapes	= 0.0f;
	fluidDesc.dynamicFrictionForStaticShapes= 0.08f;
	fluidDesc.dynamicFrictionForDynamicShapes= 0.08f;
	fluidDesc.simulationMethod				= NX_F_SPH;

	fluidDesc.initialParticleData			= gNewParticleData;

	if (!gHardwareSimulation)
        fluidDesc.flags &= ~NX_FF_HARDWARE;

	//Create user fluid.
	//- create NxFluid in NxScene
	//- setup the buffers to read from data from the SDK
	//- set NxFluid::userData field to MyFluid instance
	bool trackUserData = false;
	bool provideCollisionNormals = false;
	NxReal fieldSize = 40.0f;
	NxReal fieldStart = -fieldSize/2;
	NxBounds3 aabb;
	MyFluid* fluid = NULL;

	//fluid without static friction
	fluidDesc.staticFrictionForStaticShapes = 0.0f;
	fluidDesc.staticFrictionForDynamicShapes = 0.0f;
	aabb.set(NxVec3(fieldStart, 0, fieldStart), NxVec3(fieldStart + fieldSize/3, 0, fieldStart + fieldSize));
	CreateParticleAABB(gNewParticleData, gNewParticlesMax, gNewParticlesMax, false, aabb, NxVec3(0,0,0), 0.0f, 0.4f);
	fluid = new MyFluid(gScene, fluidDesc, trackUserData, provideCollisionNormals, NxVec3(0.3f,0.4f,0.7f), 0.06f);
	assert(fluid);
	gMyFluids.pushBack(fluid);

	//fluid with low static friction 
	fluidDesc.staticFrictionForStaticShapes = 0.4f;
	fluidDesc.staticFrictionForDynamicShapes = 0.4f;
	aabb.set(NxVec3(fieldStart + fieldSize/3, 0, fieldStart), NxVec3(fieldStart + 2*fieldSize/3, 0, fieldStart + fieldSize));
	CreateParticleAABB(gNewParticleData, gNewParticlesMax, gNewParticlesMax, false, aabb, NxVec3(0,0,0), 0.0f, 0.4f);
	fluid = new MyFluid(gScene, fluidDesc, trackUserData, provideCollisionNormals, NxVec3(0.5f,0.7f,0.9f), 0.06f);
	assert(fluid);
	gMyFluids.pushBack(fluid);

	//fluid with high static friction
	fluidDesc.staticFrictionForStaticShapes = 4.0f;
	fluidDesc.staticFrictionForDynamicShapes = 4.0f;
	aabb.set(NxVec3(fieldStart + 2*fieldSize/3, 0, fieldStart), NxVec3(fieldStart + fieldSize, 0, fieldStart + fieldSize));
	CreateParticleAABB(gNewParticleData, gNewParticlesMax, gNewParticlesMax, false, aabb, NxVec3(0,0,0), 0.0f, 0.4f);
	fluid = new MyFluid(gScene, fluidDesc, trackUserData, provideCollisionNormals, NxVec3(1.0f,1.0f,1.0f), 0.06f);
	assert(fluid);
	gMyFluids.pushBack(fluid);

	//gMyFluids[0]->getNxFluid()->setStaticFrictionForStaticShapes(5.0f);
	//gMyFluids[0]->getNxFluid()->setStaticFrictionForDynamicShapes(5.0f);
	//gMyFluids[1]->getNxFluid()->setStaticFrictionForStaticShapes(5.0f);
	//gMyFluids[1]->getNxFluid()->setStaticFrictionForDynamicShapes(5.0f);
	//gMyFluids[2]->getNxFluid()->setStaticFrictionForStaticShapes(5.0f);
	//gMyFluids[2]->getNxFluid()->setStaticFrictionForDynamicShapes(5.0f);

	gCameraPos.set(-2.91f, 10.4f, 24.6f);
	gCameraForward.set(0.0f, -0.58f, -0.81f);
	gCameraForward.normalize();

	gShadows = false; //hack
}

//----------------------------------------------------------------------------//

void SampleCollisionStaticFriction::cleanup()
{
	delete[] gNewParticles;

	if(gTerrain!=NULL)
	{
		gScene->releaseActor(*gTerrain);
		gTerrain=NULL; 
	}

	if(gTerrainData!=NULL)
	{
		delete gTerrainData;
		gTerrainData=NULL;		
	}
}

//----------------------------------------------------------------------------//

void SampleCollisionStaticFriction::update()
{
	if(gTime > gTimeMark)
	{
		//choose new wind direction/strength
		if(NxMath::rand(0.0f, 1.0f) > 0.6)
		{
			gWindForceTarget = RandNormalVec(NxVec3(0,0,0), 10.0f);
			gTimeMark = gTime + 5.0f;
		}
		else
		{
			gWindForceTarget = NxVec3(0,0,0);
			gTimeMark = gTime + 1.0f;
		}
	}

	NxReal alpha = 0.1f;
	gWindForce = gWindForce * (1-alpha) + gWindForceTarget * alpha;

	gScene->setGravity(gDefaultGravity + gWindForce);
}

//----------------------------------------------------------------------------//

/**
Just a little helper to get more interesting start conditions.
Probably not the most efficient method.
*/
static NxVec3 RandNormalVec(const NxVec3& mu, NxReal sig)
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

//----------------------------------------------------------------------------//

static void GenerateCirclePts(unsigned int nb, NxVec3* pts, float scale, float z)
{
	for(unsigned int i=0;i<nb;i++)
	{
		NxF32 angle = 6.28f*NxF32(i)/NxF32(nb);
		pts[i].x = cosf(angle)*scale;
		pts[i].y = z;
		pts[i].z = sinf(angle)*scale;
	}
}

//----------------------------------------------------------------------------//

static void CreatePhysicsTerrain()
{
	if(gTerrain!=NULL)
	{
		gScene->releaseActor(*gTerrain);
		gTerrain=NULL; 
	}

	if(gTerrainData!=NULL)
	{
		delete gTerrainData;
		gTerrainData=NULL;		
	}

	//init(NxU32 size, NxF32 offset, NxF32 width, NxF32 chaos, bool flat=false, const NxVec3* pos=NULL);
	
	NxU32  terrainSize		= 32;
	NxReal terrainOffset	= -20.0f;
	NxReal terrainWidth		= 2.0f; //20.0f;
	NxReal terrainChaos		= 10.0f; //15.0f;

	//NxU32  terrainSize		= 2;
	//NxReal terrainOffset	= -20.0f;
	//NxReal terrainWidth		= 30.0f;
	//NxReal terrainChaos		= 0.0f; //15.0f;

	gTerrainData = new TerrainData;
	gTerrainData->init(terrainSize, terrainOffset, terrainWidth, terrainChaos);

	// Build physical model
	NxTriangleMeshDesc terrainDesc;
	terrainDesc.numVertices					= gTerrainData->nbVerts;
	terrainDesc.numTriangles				= gTerrainData->nbFaces;
	terrainDesc.pointStrideBytes			= sizeof(NxVec3);
	terrainDesc.triangleStrideBytes			= 3*sizeof(NxU32);
	terrainDesc.points						= gTerrainData->verts;
	terrainDesc.triangles					= gTerrainData->faces;
	terrainDesc.flags						= 0;

	terrainDesc.heightFieldVerticalAxis		= NX_Y;
	terrainDesc.heightFieldVerticalExtent	= -1000.0f;

	//bool status = InitCooking(gMyAllocator, &gMyErrorStream);
	bool status = InitCooking();
	if (!status) {
		printf("Unable to initialize the cooking library. Please make sure that you have correctly installed the latest version of the AGEIA PhysX SDK.");
		exit(1);
	}

	MemoryWriteBuffer buf;
	status = CookTriangleMesh(terrainDesc, buf);
	if (!status) {
		printf("Unable to cook a triangle mesh.");
		exit(1);
	}
	MemoryReadBuffer readBuffer(buf.data);
	NxTriangleMesh* terrainMesh = gPhysicsSDK->createTriangleMesh(readBuffer);

	NxTriangleMeshShapeDesc terrainShapeDesc;
	terrainShapeDesc.meshData = terrainMesh;
	terrainShapeDesc.shapeFlags	= NX_SF_FEATURE_INDICES;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&terrainShapeDesc);
	
	NxQuat q;
	//q.fromAngleAxis(90.0f, NxVec3(1.0f, 0.0f, 0.0f));
	//q.fromAngleAxis(0.0f, NxVec3(0.0f, 0.0f, 1.0f));
	//actorDesc.globalPose = NxMat34(NxMat33(q), NxVec3(0.0f, 3.5f, 0.0f));

	gTerrain = gScene->createActor(actorDesc);
	gTerrain->userData = (void*)0;

	CloseCooking();
}

//----------------------------------------------------------------------------//

static void GenerateConvex(float scale, const NxVec3& pos)
{
	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;

	int nbInsideCirclePts = NxMath::rand(3, 8);
	int nbOutsideCirclePts = NxMath::rand(3, 8);
	int nbVerts = nbInsideCirclePts + nbOutsideCirclePts;
	
	// Generate random vertices
	NxVec3* verts = new NxVec3[nbVerts];
	
	// Two methods
	if(NxMath::rand(0, 100) > 50)
	{
		for(int i=0;i<nbVerts;i++)
		{
			verts[i].x = scale * NxMath::rand(-2.5f, 2.5f);
			verts[i].y = scale * NxMath::rand(-2.5f, 2.5f);
			verts[i].z = scale * NxMath::rand(-2.5f, 2.5f);
		}
	}
	else
	{
		//GenerateCirclePts(nbInsideCirclePts, verts, scale, 0.0f);
		//GenerateCirclePts(nbOutsideCirclePts, verts+nbInsideCirclePts, scale*3.0f, 10.0f);
		GenerateCirclePts(nbInsideCirclePts, verts, scale, 0.0f);
		GenerateCirclePts(nbOutsideCirclePts, verts+nbInsideCirclePts, scale*3.0f, 2.0f * scale);
	}

	// Create descriptor for convex mesh
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices			= nbVerts;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.points				= verts;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;
	
	// Cooking from memory
	MemoryWriteBuffer buf;
	if(CookConvexMesh(convexDesc, buf))
	{

		NxConvexShapeDesc convexShapeDesc;
		convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));

		if(convexShapeDesc.meshData)
		{
			NxActorDesc ActorDesc;
			ActorDesc.shapes.pushBack(&convexShapeDesc);
			ActorDesc.body			= &bodyDesc;
			ActorDesc.density		= 1.0f; //10.0f;
			ActorDesc.globalPose.t  = pos;
			//gLastActor = gScene->createActor(ActorDesc);
			gScene->createActor(ActorDesc);
		}
	}
	
	delete[] verts;
}

//----------------------------------------------------------------------------//
