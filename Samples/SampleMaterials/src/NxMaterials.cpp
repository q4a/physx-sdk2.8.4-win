// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Materials Sample
// Description: This sample program shows how to use the material properties to
//              make different materials for a physics scene. The application
//              creates several ramps with different friction properties, and you
//              can drop different shapes with varying material properties on them.
//
// ===============================================================================

#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
// Physics code
#undef random
#include "NxPhysics.h"
#include "ErrorStream.h"
#include "DebugRenderer.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "UserAllocator.h"
#include "cooking.h"
#include "Stream.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

static bool				gPause = false;
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;
static NxVec3			gDefaultGravity(0.0f, -100.0f, 0.0f);
static ErrorStream*		gErrorStream = NULL;
static UserAllocator*	gAllocator = NULL;
DebugRenderer gDebugRenderer;

static NxMaterialIndex 
	defaultMaterialIndex, 
	somewhatBouncyMaterialIndex, 
	veryBouncyMaterialIndex, 
	frictionlessMaterialIndex,
	highFrictionMaterialIndex,
	anisoMaterialIndex;


static void CreateCapsule(const NxVec3& pos, NxF32 radius=1, NxF32 height=3, const NxVec3* initial_velocity=NULL)
{
	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
	if(initial_velocity)	bodyDesc.linearVelocity = *initial_velocity;

	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.radius		= radius;
	capsuleDesc.height		= height;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&capsuleDesc);
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t  = pos;
	actorDesc.globalPose.M.rotZ(1.57f);
	gScene->createActor(actorDesc);
}

static void CreateSphere(const NxVec3& pos, const NxReal radius=1.0f, const NxReal density=1.0f)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a sphere
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = radius;
	sphereDesc.localPose.t = NxVec3(0,radius,0);
	actorDesc.shapes.pushBack(&sphereDesc);

	actorDesc.body = &bodyDesc;
	actorDesc.density = density;
	actorDesc.globalPose.t = pos;
	gScene->createActor(actorDesc);	
}

static void CreateCube(const NxVec3& pos, const NxVec3 * vel = 0)
	{
	// Avoid creating one compound within another
	NxBounds3 bounds;
	bounds.setCenterExtents(pos, NxVec3(10.0f, 10.0f, 10.0f));
	if(gScene->checkOverlapAABB(bounds))
		return;

	// Create cube made up of 6 individual boxes as its faces, with each box having a different material
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	bodyDesc.linearVelocity.set(0,5,0);
	if (vel)
		bodyDesc.linearVelocity += *vel;

	bodyDesc.angularVelocity.set(NxMath::rand(0.0f,10.0f),NxMath::rand(0.0f,10.0f),NxMath::rand(0.0f,10.0f));	//throw up the ball with a random initial angular vel as if to roll dice.
	
	NxBoxShapeDesc boxDesc[6];
	boxDesc[0].dimensions.set(4,4,1);
	boxDesc[0].localPose.t.set(0,0,4);
	boxDesc[0].materialIndex	= defaultMaterialIndex;
	actorDesc.shapes.pushBack(&boxDesc[0]);
	boxDesc[1].dimensions.set(4,4,1);
	boxDesc[1].localPose.t.set(0,0,-4);
	boxDesc[1].materialIndex	= somewhatBouncyMaterialIndex;
	actorDesc.shapes.pushBack(&boxDesc[1]);
	boxDesc[2].dimensions.set(4,1,4);
	boxDesc[2].localPose.t.set(0,4,0);
	boxDesc[3].materialIndex	= veryBouncyMaterialIndex;
	actorDesc.shapes.pushBack(&boxDesc[2]);
	boxDesc[3].dimensions.set(4,1,4);
	boxDesc[3].localPose.t.set(0,-4,0);
	boxDesc[3].materialIndex	= defaultMaterialIndex;
	actorDesc.shapes.pushBack(&boxDesc[3]);
	boxDesc[4].dimensions.set(1,4,4);
	boxDesc[4].localPose.t.set(4,0,0);
	boxDesc[4].materialIndex	= frictionlessMaterialIndex;
	actorDesc.shapes.pushBack(&boxDesc[4]);
	boxDesc[5].dimensions.set(1,4,4);
	boxDesc[5].localPose.t.set(-4,0,0);
	boxDesc[5].materialIndex	= highFrictionMaterialIndex;
	actorDesc.shapes.pushBack(&boxDesc[5]);

	
	
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t  = pos;
	gScene->createActor(actorDesc);
	}


static bool InitNx()
	{

	if (!gAllocator)
		gAllocator = new UserAllocator;
	gErrorStream = new ErrorStream();

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator, gErrorStream, desc, &errorCode);
	if(gPhysicsSDK == NULL) 
		{
		printf("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK, exiting the sample.\n\n", errorCode, getNxSDKCreateError(errorCode));
		return false;
		}
#if SAMPLES_USE_VRD
	// The settings for the VRD host and port are found in SampleCommonCode/SamplesVRDSettings.h
	if (gPhysicsSDK->getFoundationSDK().getRemoteDebugger() && !gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
#endif

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.025f);
	//enable visualisation
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1.0f);
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1.0f);

	// Initialize cooking
	if (!InitCooking(gAllocator)) {
		printf("Unable to initialize the cooking library. Please make sure that you have correctly installed the latest version of the NVIDIA PhysX SDK.");
		exit(1);
	}
		
	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= gDefaultGravity;
	gScene = gPhysicsSDK->createScene(sceneDesc);
	if(gScene == NULL) 
		{
		printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
		return false;
		}

	//create some materials -- note that we reuse the same NxMaterialDesc every time,
	//as it gets copied to the SDK with every call, not just referenced.
	NxMaterialDesc	material;
	
	//default material
	material.restitution		= 0.0f;
	material.staticFriction		= 0.5f;
	material.dynamicFriction	= 0.5f;
	defaultMaterialIndex = 0;
	gScene->getMaterialFromIndex(defaultMaterialIndex)->loadFromDesc(material);//we redefine the default material in slot 0
	
	
	//somewhat bouncy material
	material.restitution		= 0.75f;
	material.staticFriction		= 0.5f;
	material.dynamicFriction	= 0.5f;
	somewhatBouncyMaterialIndex = gScene->createMaterial(material)->getMaterialIndex();
	
	//very bouncy material
	material.restitution		= 1.0f;
	material.staticFriction		= 0.5f;
	material.dynamicFriction	= 0.5f;
	veryBouncyMaterialIndex = gScene->createMaterial(material)->getMaterialIndex();
	
	//almost frictionless material
	material.restitution		= 0.0f;
	material.staticFriction		= 0.01f;
	material.dynamicFriction	= 0.01f;
	frictionlessMaterialIndex = gScene->createMaterial(material)->getMaterialIndex();
	
	//really high friction material
	material.restitution		= 0.0f;
	material.staticFriction		= 20.0f;//static friction can be higher than 1.
	material.dynamicFriction	= 1.0f;	//dynamic friction should not be higher than 1.
	highFrictionMaterialIndex = gScene->createMaterial(material)->getMaterialIndex();
	
	//anisotropic friction material
	material.restitution		= 0.0f;
	material.staticFriction		= 0.1f;
	material.dynamicFriction	= 0.1f;	
	material.dynamicFrictionV	= 0.8f;
	material.staticFrictionV	= 1.0f;
	material.dirOfAnisotropy.set(1.5,0,1);	//this will be used on one of the ramps so that boxes slide off at an angle
	material.dirOfAnisotropy.normalize();
	material.flags				= NX_MF_ANISOTROPIC;
	anisoMaterialIndex = gScene->createMaterial(material)->getMaterialIndex();
	material.setToDefault();		//turn off aniso on our material object in case we want to use it again.
	

	
	// Create ground plane
	NxPlaneShapeDesc planeDesc;
	planeDesc.materialIndex = frictionlessMaterialIndex;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	gScene->createActor(actorDesc);

	// Create box primitive ramp (anisotropic friction)
	NxBoxShapeDesc anisoBoxDesc;
	anisoBoxDesc.dimensions		= NxVec3(float(20), float(2), float(30));
	anisoBoxDesc.materialIndex	= anisoMaterialIndex;
	
	actorDesc.setToDefault();
	actorDesc.shapes.pushBack(&anisoBoxDesc);
	actorDesc.globalPose.t.set(0,20.0f,0);
	float angle = NxMath::degToRad(30.0f);
	actorDesc.globalPose.M(0,0) = 1;		actorDesc.globalPose.M(0,1) = 0;		actorDesc.globalPose.M(0,2) = 0;
	actorDesc.globalPose.M(1,0) = 0;		actorDesc.globalPose.M(1,1) = cos(angle);		actorDesc.globalPose.M(1,2) = -sin(angle);
	actorDesc.globalPose.M(2,0) = 0;		actorDesc.globalPose.M(2,1) = sin(angle);		actorDesc.globalPose.M(2,2) = cos(angle);
	gScene->createActor(actorDesc);

	// Create box primitive ramp (frictionless)
	NxBoxShapeDesc frictlessBoxDesc;
	frictlessBoxDesc.dimensions		= NxVec3(float(20), float(2), float(30));
	frictlessBoxDesc.materialIndex	= frictionlessMaterialIndex;

	actorDesc.setToDefault();
	actorDesc.shapes.clear();
	actorDesc.shapes.pushBack(&frictlessBoxDesc);
	actorDesc.globalPose.t.set(-40,20.0f,0);
	actorDesc.globalPose.M(0,0) = 1;		actorDesc.globalPose.M(0,1) = 0;		actorDesc.globalPose.M(0,2) = 0;
	actorDesc.globalPose.M(1,0) = 0;		actorDesc.globalPose.M(1,1) = cos(angle);		actorDesc.globalPose.M(1,2) = -sin(angle);
	actorDesc.globalPose.M(2,0) = 0;		actorDesc.globalPose.M(2,1) = sin(angle);		actorDesc.globalPose.M(2,2) = cos(angle);
	gScene->createActor(actorDesc);

	// Create box primitive ramp (high friction)
	NxBoxShapeDesc hiFrictBoxDesc;
	hiFrictBoxDesc.dimensions		= NxVec3(float(20), float(2), float(30));
	hiFrictBoxDesc.materialIndex	= highFrictionMaterialIndex;

	actorDesc.setToDefault();
	actorDesc.shapes.clear();
	actorDesc.shapes.pushBack(&hiFrictBoxDesc);
	actorDesc.globalPose.t.set(40,20.0f,0);
	actorDesc.globalPose.M(0,0) = 1;		actorDesc.globalPose.M(0,1) = 0;		actorDesc.globalPose.M(0,2) = 0;
	actorDesc.globalPose.M(1,0) = 0;		actorDesc.globalPose.M(1,1) = cos(angle);		actorDesc.globalPose.M(1,2) = -sin(angle);
	actorDesc.globalPose.M(2,0) = 0;		actorDesc.globalPose.M(2,1) = sin(angle);		actorDesc.globalPose.M(2,2) = cos(angle);
	gScene->createActor(actorDesc);
	
	// Create convex ramp (high friction)
	float scale = 2.0f;
	int nbVerts = 8;
	NxVec3* verts = new NxVec3[nbVerts];

	verts[0].x = scale * 10;
	verts[0].y = scale * 1;
	verts[0].z = scale * 15;

	verts[1].x = scale * -10;
	verts[1].y = scale * 1;
	verts[1].z = scale * 15;

	verts[2].x = scale * 10;
	verts[2].y = scale * 1;
	verts[2].z = scale * -15;

	verts[3].x = scale * -10;
	verts[3].y = scale * 1;
	verts[3].z = scale * -15;

	verts[4].x = scale * 10;
	verts[4].y = scale * -1;
	verts[4].z = scale * 15;

	verts[5].x = scale * -10;
	verts[5].y = scale * -1;
	verts[5].z = scale * 15;

	verts[6].x = scale * 10;
	verts[6].y = scale * -1;
	verts[6].z = scale * -15;

	verts[7].x = scale * -10;
	verts[7].y = scale * -1;
	verts[7].z = scale * -15;	

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
		convexShapeDesc.materialIndex	= highFrictionMaterialIndex;

		if(convexShapeDesc.meshData)
		{
			NxActorDesc ActorDesc;
			ActorDesc.setToDefault();
			ActorDesc.shapes.clear();
			ActorDesc.shapes.pushBack(&convexShapeDesc);
			ActorDesc.density		= 10.0f;
			ActorDesc.globalPose.t.set(80,20.0f,0);
			ActorDesc.globalPose.M(0,0) = 1;		ActorDesc.globalPose.M(0,1) = 0;		ActorDesc.globalPose.M(0,2) = 0;
			ActorDesc.globalPose.M(1,0) = 0;		ActorDesc.globalPose.M(1,1) = cos(angle);		ActorDesc.globalPose.M(1,2) = -sin(angle);
			ActorDesc.globalPose.M(2,0) = 0;		ActorDesc.globalPose.M(2,1) = sin(angle);		ActorDesc.globalPose.M(2,2) = cos(angle);
			gScene->createActor(ActorDesc);
		}
	}
	
	// Create trimesh ramp (high friction)
	NxU32* faces = new NxU32[2*3];
	faces[0] = 0;
	faces[1] = 2;
	faces[2] = 1;
	faces[3] = 3;
	faces[4] = 1;
	faces[5] = 2;

	// Build physical model
	NxTriangleMeshDesc triMeshDesc;
	triMeshDesc.numVertices					= 4;
	triMeshDesc.numTriangles				= 2;
	triMeshDesc.pointStrideBytes			= sizeof(NxVec3);
	triMeshDesc.triangleStrideBytes			= 3*sizeof(NxU32);
	triMeshDesc.points						= verts;
	triMeshDesc.triangles					= faces;
	triMeshDesc.flags						= 0;

	triMeshDesc.heightFieldVerticalAxis		= NX_Y;
	triMeshDesc.heightFieldVerticalExtent	= -1000.0f;

	NxTriangleMeshShapeDesc terrainShapeDesc;

	MemoryWriteBuffer writeBuffer;
	bool status2 = CookTriangleMesh(triMeshDesc, writeBuffer);
	if (!status2) {
		printf("Unable to cook a triangle mesh.");
		exit(1);
	}

	terrainShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(writeBuffer.data));
	terrainShapeDesc.materialIndex	= highFrictionMaterialIndex;

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&terrainShapeDesc);
	ActorDesc.globalPose.t.set(120,20.0f,0);
	ActorDesc.globalPose.M(0,0) = 1;		ActorDesc.globalPose.M(0,1) = 0;		ActorDesc.globalPose.M(0,2) = 0;
	ActorDesc.globalPose.M(1,0) = 0;		ActorDesc.globalPose.M(1,1) = cos(angle);		ActorDesc.globalPose.M(1,2) = -sin(angle);
	ActorDesc.globalPose.M(2,0) = 0;		ActorDesc.globalPose.M(2,1) = sin(angle);		ActorDesc.globalPose.M(2,2) = cos(angle);

	gScene->createActor(ActorDesc);
	
	delete[] verts;

	return true;
	}


// Render code

static int gMainHandle;
static NxVec3 Eye(70.0f, 50.0f, 80.0f);
static NxVec3 Dir(-0.6,-0.2,-0.7);
static NxVec3 N;
static int mx = 0;
static int my = 0;

static void KeyboardCallback(unsigned char key, int x, int y)
	{
	switch (key)
		{
		case 27:	exit(0); break;
		case '1':			CreateCube(NxVec3(-40.0f, 60.0f, -18.0f));
							CreateCube(NxVec3(0.0f, 60.0f, -18.0f));
							CreateCube(NxVec3(40.0f, 60.0f, -18.0f)); 
							CreateCube(NxVec3(80.0f, 60.0f, -18.0f)); 
							CreateCube(NxVec3(120.0f, 60.0f, -18.0f)); break;

		case '2':			CreateSphere(NxVec3(-40.0f, 60.0f, -18.0f),2);
							CreateSphere(NxVec3(0.0f, 60.0f, -18.0f),2);
							CreateSphere(NxVec3(40.0f, 60.0f, -18.0f),2); 
							CreateSphere(NxVec3(80.0f, 60.0f, -18.0f),2); 
							CreateSphere(NxVec3(120.0f, 60.0f, -18.0f),2); break;

		case '3':			CreateCapsule(NxVec3(-40.0f, 60.0f, -18.0f),2,10);
							CreateCapsule(NxVec3(0.0f, 60.0f, -18.0f),2,10);
							CreateCapsule(NxVec3(40.0f, 60.0f, -18.0f),2,10); 
							CreateCapsule(NxVec3(80.0f, 60.0f, -18.0f),2,10); 
							CreateCapsule(NxVec3(120.0f, 60.0f, -18.0f),2,10); break;

		case 'p':			gPause = !gPause; break;
		case 101: 			Eye += Dir * 2.0f; break;
		case 103: 			Eye -= Dir * 2.0f; break;
		case 100: 			Eye -= N * 2.0f; break;
		case 102: 			Eye += N * 2.0f; break;
		case 'w':
			{
			NxVec3 t = Eye;
			NxVec3 Vel = Dir;
			Vel.normalize();
			Vel*=100.0f;
			CreateCube(t, &Vel);
			}
			break;
		}
	
	}

static void ArrowKeyCallback(int key, int x, int y)
	{
	KeyboardCallback(key,x,y);
	}


static void MouseCallback(int button, int state, int x, int y)
	{
	mx = x;
	my = y;
	}

#include <stdio.h>
static void MotionCallback(int x, int y)
	{
	int dx = mx - x;
	int dy = my - y;
	
	Dir.normalize();
	N.cross(Dir,NxVec3(0,1,0));
	
	NxQuat qx(NxPiF32 * dx * 20/ 180.0f, NxVec3(0,1,0));
	qx.rotate(Dir);
	NxQuat qy(NxPiF32 * dy * 20/ 180.0f, N);
	qy.rotate(Dir);
	
	mx = x;
	my = y;
	}

static void RenderCallback()
	{
	// Physics code
	if(gScene && !gPause)	
		{
		gScene->simulate(1.0f/60.0f);	//Note: a real application would compute and pass the elapsed time here.
		gScene->flushStream();
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
		}
	// ~Physics code
	
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	gluLookAt(Eye.x, Eye.y, Eye.z, Eye.x + Dir.x, Eye.y + Dir.y, Eye.z + Dir.z, 0.0f, 1.0f, 0.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gDebugRenderer.renderData(*gScene->getDebugRenderable());	
#ifdef __PPCGEKKO__	
	char buf[256];
    sprintf(buf,"Press 1,2,+,-,a,HOME to drop boxes. Press b to shoot a box.\nArrows camera.\n" );

    GLFontRenderer::setScreenResolution(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	GLFontRenderer::setColor(0.9f, 1.0f, 0.0f, 1.0f);
	GLFontRenderer::print(0.01, 0.9, 0.036, buf, false, 11, true);   
#endif	
	glutSwapBuffers();
	}

static void ReshapeCallback(int width, int height)
	{
	glViewport(0, 0, width, height);
	}

static void IdleCallback()
	{
	glutPostRedisplay();
	}

static void ExitCallback()
{
	if (gPhysicsSDK)
	{
		if (gScene) gPhysicsSDK->releaseScene(*gScene);
		gPhysicsSDK->release();
	}

	if (gAllocator)
	{
		delete gAllocator;
		gAllocator = NULL;
	}
}

int main(int argc, char** argv)
	{
	// Initialize Glut
#ifdef __PPCGEKKO__
	printf("Press 1,2,+,-,a,HOME to drop boxes. Press b to shoot a box.\nArrows camera.\n");
#else
	printf("Press 1-3 to drop boxes, spheres and capsules. Press w to shoot a box.\nArrows and mouse move camera.\n");
	printf("Press p to pause the simulation.\n");
#endif

#if defined(_XBOX) || defined(__CELLOS_LV2__)
	glutRemapButtonExt(0, 'w', false); // A/X to shoot box
	glutRemapButtonExt(1, '1', false); // B/O, X/S, Y/T shifted/unshifted to drop boxes
	glutRemapButtonExt(2, '2', false);
	glutRemapButtonExt(3, '3', false);
	glutRemapButtonExt(1, '4', true);
	glutRemapButtonExt(2, '5', true);
	glutRemapButtonExt(3, '6', true);
#endif

#ifdef __PPCGEKKO__	
	glutRemapButtonExt('a','1');
	glutRemapButtonExt('b','w');
	glutRemapButtonExt('1','2');
	glutRemapButtonExt('2','3');
	glutRemapButtonExt('+','4');
	glutRemapButtonExt('-','5');
	glutRemapButtonExt(GLUT_KEY_HOME,'6');
	glutRemapButtonExt(GLUT_KEY_UP,101);
	glutRemapButtonExt(GLUT_KEY_DOWN,103);
	glutRemapButtonExt(GLUT_KEY_LEFT,100);
	glutRemapButtonExt(GLUT_KEY_RIGHT,102);
#endif

	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("SampleMaterials");
	glutSetWindow(gMainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(ArrowKeyCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0,0);
		
	// Setup default render states
	glClearColor(0.3f, 0.4f, 0.5f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);
	
	// Setup lighting
	glEnable(GL_LIGHTING);
	float AmbientColor[]	= { 0.0f, 0.1f, 0.2f, 0.0f };		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	float DiffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[]		= { 100.0f, 100.0f, 400.0f, 1.0f };		glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitCallback);
		glutMainLoop();
	}

	return 0;
	}
