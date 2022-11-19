// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Convex Sample
// Description: This sample program shows how to cook and use convex meshes.
//
// ===============================================================================

#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
// Physics code
#include "NxPhysics.h"
#include "UserAllocator.h"
#include "ErrorStream.h"
#include "DebugRenderer.h"
#include "DrawObjects.h"
#include "cooking.h"
#include "Stream.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

static UserAllocator*	gMyAllocator = NULL;
static ErrorStream*		gErrorStream = NULL;
static DebugRenderer	gDebugRenderer;

static bool				gPause = false;
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;
static NxVec3			gDefaultGravity(0.0f, -9.81f, 0.0f);

// Render code
static NxVec3 Eye(50.0f, 50.0f, 50.0f);
static NxVec3 Dir(-0.6,-0.2,-0.7);
static NxVec3 N;
static int mx = 0;
static int my = 0;
static NxF32 gCamSpeed = 2.0f;

static NxActor* gLastActor = NULL;

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

void GenerateConvex(float scale, const NxVec3& pos)
{
	// Create body
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;

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
		GenerateCirclePts(nbInsideCirclePts, verts, scale, 0.0f);
		GenerateCirclePts(nbOutsideCirclePts, verts+nbInsideCirclePts, scale*3.0f, 10.0f);
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
			ActorDesc.body			= &BodyDesc;
			ActorDesc.density		= 10.0f;
			ActorDesc.globalPose.t  = pos;
			gLastActor = gScene->createActor(ActorDesc);
		}
	}
	
	delete[] verts;
}

void ReleaseActor(NxActor* actor) 
{
	if(actor) 
	{
		NxConvexMesh* convex = NULL;
		NxTriangleMesh* mesh = NULL;
		NxU32 n = actor->getNbShapes();
		if (n > 0) 
		{
			for(NxU32 i = 0; i < n; ++i)
			{
				NxShape* shape = actor->getShapes()[i];
				if (shape->isConvexMesh()) 
				{
					convex = &(shape->isConvexMesh()->getConvexMesh());
				}
				else if(shape->isTriangleMesh()) 
				{
					mesh = &(shape->isTriangleMesh()->getTriangleMesh());
				}
				actor->releaseShape(*shape);
				if (convex && convex->getReferenceCount() == 0) {
					gPhysicsSDK->releaseConvexMesh(*convex);
					convex = NULL;
				}
				if (mesh && mesh->getReferenceCount() == 0) {
					gPhysicsSDK->releaseTriangleMesh(*mesh);
					mesh = NULL;
				}
			}
		}
		gScene->releaseActor(*actor);
	}
}

static void CreateCube(const NxVec3& pos, int size=2, const NxVec3* initial_velocity=NULL)
{
	// Create body
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;
	if(initial_velocity) BodyDesc.linearVelocity = *initial_velocity;

	NxBoxShapeDesc BoxDesc;
	BoxDesc.dimensions		= NxVec3(float(size), float(size), float(size));

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&BoxDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 10.0f;
	ActorDesc.globalPose.t  = pos;
	gLastActor = gScene->createActor(ActorDesc);
}

static void UpdateCube(NxActor* actor)
{
	if (actor->getNbShapes())
	{
		NxShape* const* shapes = actor->getShapes();
		NxConvexMesh* convex = NULL;
		if (shapes[0]->isConvexMesh())
			convex = &(shapes[0]->isConvexMesh()->getConvexMesh());
		actor->releaseShape(*shapes[0]);
		if (convex && convex->getReferenceCount() == 0)
			gPhysicsSDK->releaseConvexMesh(*convex);
	}

	NxBoxShapeDesc BoxDesc;
	BoxDesc.dimensions = NxVec3(4.0f, 4.0f, 4.0f);

	actor->createShape(BoxDesc);
	actor->updateMassFromShapes(1,0);
}

static bool InitNx()
{
	gMyAllocator = new UserAllocator();
	gErrorStream = new ErrorStream();

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gMyAllocator, gErrorStream, desc, &errorCode);
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

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.25f);

	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 10.0f);
	//Enable the following lines to visualize collision information
//	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_VNORMALS, 1);
//	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_AABBS, 1);
//	gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_POINT, 1);
//	gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_NORMAL, 1);
//	gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_FORCE, 1);

	// Initialize cooking
	bool status = InitCooking(gMyAllocator);
	if (!status) {
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

	NxMaterial * defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);		
		
	// Create ground plane
	NxPlaneShapeDesc PlaneDesc;
	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&PlaneDesc);
	gScene->createActor(ActorDesc);

	return true;
}

static void ExitCallback()
{
	CloseCooking();
	
	if(gPhysicsSDK != NULL)
	{
		if(gScene != NULL) gPhysicsSDK->releaseScene(*gScene);
		gScene = NULL;
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
	}

	if(gMyAllocator!=NULL)
	{
		delete gMyAllocator;
		gMyAllocator=NULL;
	}
	
	if(gErrorStream!=NULL)
	{
		delete gErrorStream;
		gErrorStream=NULL;
	}
}


static void KeyboardCallback(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case ' ':
		CreateCube(NxVec3(0.0f, 30.0f, 0.0f), 1+(rand()&3));
		break;
	case 't':
	case 'T':
		if (gLastActor)
			ReleaseActor(gLastActor);
		gLastActor = NULL;
	case 'c':
	case 'C':
		GenerateConvex(4.0f, NxVec3(0, 50, 0));
		break;
	case 'y':
	case 'Y':
		{
			if(gLastActor) UpdateCube(gLastActor);
		}
		break;
	case 'p':
	case 'P':
		gPause = !gPause;
		break;
	case 's':
	case 'S':
		{
			if(gLastActor)
			{
				ReleaseActor(gLastActor);
				gLastActor = NULL;
				if(gScene->getNbActors() > 1) {
					gLastActor = gScene->getActors()[gScene->getNbActors()-1];
				}

			}
		}
		break;
	case 101: case '8':	Eye += Dir * gCamSpeed; break;
	case 103: case '2':	Eye -= Dir * gCamSpeed; break;
	case 100: case '4':	Eye -= N * gCamSpeed; break;
	case 102: case '6':	Eye += N * gCamSpeed; break;
	case 'w':
	case 'W':
		{
		NxVec3 t = Eye;
		NxVec3 Vel = Dir;
		Vel.normalize();
		Vel*=200.0f;
		CreateCube(t, 8, &Vel);
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
	if(gScene == NULL) return;
	
	// Physics code
	if(!gPause)	
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

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, Eye.x + Dir.x, Eye.y + Dir.y, Eye.z + Dir.z, 0.0f, 1.0f, 0.0f);

	glEnable(GL_LIGHTING);

	// Keep physics & graphics in sync
	int nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while(nbActors--)
	{
		DrawActor(*actors++);
	}

	gDebugRenderer.renderData(*gScene->getDebugRenderable());
#ifdef __PPCGEKKO__	
	char buf[256];
    sprintf(buf,
    "Use the arrow keys to move the camera.\n"
	"Press the keys b, 1 and 2 to create various things.\n"	
	"Press HOME to reshape the last thing created.\n");
	
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

int main(int argc, char** argv)
{
#ifdef __PPCGEKKO__
	printf("Use the arrow keys to move the camera.\n");
	printf("Press the keys b, 1 and 2 to create various things.\n");	
	printf("Press HOME to reshape the last thing created.\n");
#else
	printf("Use the arrow keys or 2, 4, 6 and 8 to move the camera.\n");
	printf("Use the mouse to rotate the camera.\n");
	printf("Press p to pause the simulation.\n");
	printf("Press the keys space, c and w to create various things.\n");	
	printf("Press s to destroy the last thing created.\n");
	printf("Press t to replace the last thing created.\n");
	printf("Press y to reshape the last thing created.\n");
#endif	
#if defined(_XBOX) || defined(__CELLOS_LV2__)
	glutRemapButtonExt(0, 'c', false); //A/X to convex
#endif
#if defined(__PPCGEKKO__)
	glutRemapButtonExt('b','w');
	glutRemapButtonExt('1','c');
	glutRemapButtonExt('2',' ');
	glutRemapButtonExt(GLUT_KEY_HOME,'y');
	glutRemapButtonExt(GLUT_KEY_UP,'8');
	glutRemapButtonExt(GLUT_KEY_DOWN,'2');
	glutRemapButtonExt(GLUT_KEY_LEFT,'4');
	glutRemapButtonExt(GLUT_KEY_RIGHT,'6');
#endif
	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	int mainHandle = glutCreateWindow("SampleConvex");
	glutSetWindow(mainHandle);
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
#ifndef __PPCGEKKO__	
	glEnable(GL_CULL_FACE);
#endif	

	// Setup lighting
	glEnable(GL_LIGHTING);
	float AmbientColor[] = { 0.0f, 0.1f, 0.2f, 0.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	float DiffuseColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[] = { 100.0f, 100.0f, 400.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitCallback);
		glutMainLoop();
	}

	return 0;
}
