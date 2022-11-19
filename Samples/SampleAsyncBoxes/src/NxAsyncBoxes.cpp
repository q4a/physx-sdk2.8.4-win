// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Asynchronous Boxes
// Description: This sample program shows how to create some simple dynamic objects
//              with the NVIDIA PhysX SDK in an application using the asynchronous
//              possibilites of the NVIDIA PhysX SDK to perform the simulation.
//
// Originally written by: Pierre Terdiman (01.01.04)
// Additions by: Async additions by Stefan Duthaler
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
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "UserAllocator.h"
#include "GLFontRenderer.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

//static UserAllocator*	gMyAllocator = NULL;
static bool				gPause = false;
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;
static NxVec3			gDefaultGravity(0.0f, -9.81f, 0.0f);
static ErrorStream		gErrorStream;
static UserAllocator*	gAllocator = NULL;
// max distance for my silly computation
static NxReal distance = 0;
static NxVec3 maxPos1, maxPos2;

// Render code
static NxVec3 Eye(100.0f, 100.0f, 100.0f);
static NxVec3 Dir(-0.6,-0.2,-0.7);
static NxVec3 N;
static int mx = 0;
static int my = 0;

static void CreateCube(const NxVec3& pos, int size=2, const NxVec3* initial_velocity=NULL)
{
	// Create body
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;
//	BodyDesc.maxAngularVelocity	= 10.0f;

	if(initial_velocity)	BodyDesc.linearVelocity = *initial_velocity;

	NxBoxShapeDesc BoxDesc;
	BoxDesc.dimensions		= NxVec3(float(size), float(size), float(size));
	//BoxDesc.radius = float(size);

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&BoxDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 10.0f;
	ActorDesc.globalPose.t  = pos;
	/*ActorDesc.globalPose.M.rotX(45);
	ActorDesc.globalPose.M.rotZ(45);*/
	
	gScene->createActor(ActorDesc)->userData = (void*)size;

	printf("Total: %d actors\n", gScene->getNbActors());
}

static void CreateStack(int size)
{
	float CubeSize = 2.0f;
	float Spacing = 0.0001f;
	NxVec3 Pos(0.0f, CubeSize, 0.0f);
	float Offset = -size * (CubeSize * 2.0f + Spacing) * 0.5f;
	while(size)
	{
		for(int i=0;i<size;i++)
		{
			Pos.x = Offset + float(i) * (CubeSize * 2.0f + Spacing);
			CreateCube(Pos, (int)CubeSize);
		}
		Offset += CubeSize;
		Pos.y += (CubeSize * 2.0f + Spacing);
		size--;
	}
	printf("Total: %d actors\n", gScene->getNbActors());
}

static void CreateTower(int size)
{
	float CubeSize = 2.0f;
	float Spacing = 0.01f;
	NxVec3 Pos(0.0f, CubeSize, 0.0f);
	while(size)
	{
		CreateCube(Pos, (int)CubeSize);
		Pos.y += (CubeSize * 2.0f + Spacing);
		size--;
	}
	printf("Total: %d actors\n", gScene->getNbActors());
}

static bool InitNx()
{
	if (!gAllocator)
		gAllocator = new UserAllocator;

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator, &gErrorStream, desc, &errorCode);
	if(gPhysicsSDK == NULL) 
	{
		printf("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK, exiting the sample.\n\n", errorCode, getNxSDKCreateError(errorCode));
		return false;
	}
#if SAMPLES_USE_VRD
	// The settings for the VRD host and port are found in SampleCommonCode/SamplesVRDSettings.h
	if (gPhysicsSDK->getFoundationSDK().getRemoteDebugger())
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
#endif

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.025f);

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
	if(gPhysicsSDK != NULL)
	{
		if(gScene != NULL) gPhysicsSDK->releaseScene(*gScene);
		gScene = NULL;
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
	}

	if (gAllocator)
	{
		delete gAllocator;
		gAllocator = NULL;
	}
}

static void KeyboardCallback(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:	exit(0); break;
	case ' ':			CreateCube(NxVec3(0.0f, 20.0f, 0.0f),1); break;
	case 's':			CreateStack(10); break;
	case 'b':			CreateStack(30); break;
	case 't':			CreateTower(30); break;
	case 'p':			gPause = !gPause; break;
	case 101: case '8':	Eye += Dir * 2.0f; break;
	case 103: case '2':	Eye -= Dir * 2.0f; break;
	case 100: case '4':	Eye -= N * 2.0f; break;
	case 102: case '6':	Eye += N * 2.0f; break;
	case 'w':
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
		// ASYNC: the new method to start the SDK 
		// is now called simulate() instead of startRun()
		gScene->simulate(1.0f/60.0f);	//Note: a real application would compute and pass the elapsed time here.
		gScene->flushStream();
		
		
		//ASYNC: in here we can do computations which depend only on the old
		//		 state of the scene "actors". Writing to the scene is not allowed.
		//		 Write calls in here are skipped.
		
		// here I do a stupid max distance computation
		// between all actors .. while the SDK is running
		distance = 0;
		NxU32 nbActors = gScene->getNbActors();
		NxActor** actors = gScene->getActors();
		for(NxU32 i = 0; i < nbActors; i++)
			{
			NxVec3 pos1, pos2;
			pos1 = actors[i]->getGlobalPosition();
			for(NxU32 j = i+1; j < nbActors; j++)
				{
					pos2 = actors[j]->getGlobalPosition();
					if( pos1.distance(pos2) > distance)
						{
						distance = pos1.distance(pos2);
						maxPos1 = pos1;
						maxPos2 = pos2;
						#ifdef _DEBUG
						//printf("max distance between actors is: %f \n", distance);
						#endif
						}
				}
			}

		// ASYNC:	the new method to sync with the SDK after it has finished
		//			is now called fetchResults() instead of finishRun()
		// it waits (non)blocking for the end of a given NxSimulationStatus
		// if the end is reached it swaps the buffers, fires the callbacks 
		// and removes the writelock, if all simulations of the scene have reached its end.
		// at the moment there is only a NX_RIGID_BODY_FINISHED status.

		// -> block till the rigid body simulation has finished
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

		/*
		ASYNC:
		we could also do this with non blocking calls
		
		while(!gScene->checkResults(NX_RIGID_BODY_FINISHED, false))
			{
			 // compute something till rigid body simulation has finished
			}
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

		// or

		while(!gScene->fetchResults(NX_RIGID_BODY_FINISHED, false))
			{
			 // compute something till rigid body simulation has finished
			}
		*/		
		}

	// ASYNC:
	// then we can use isWritable() to test if we fetched 
	// all simulation results and made the scene writable again.
	// since there is only one status flag it is obvious that we
	// fetched everything and the scene is writable.
	NX_ASSERT(gScene->isWritable());
	
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

	// Render distance line
	glDisable(GL_LIGHTING);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
#ifdef __PPCGEKKO__
	glBegin(GL_LINES,2);
	glVertex3d(maxPos1.x, maxPos1.y, maxPos1.z);
	glVertex3d(maxPos2.x, maxPos2.y, maxPos2.z);
	glEnd();
#else	
	glEnableClientState(GL_VERTEX_ARRAY);
	float line[2][3] = { {maxPos1.x, maxPos1.y, maxPos1.z}, {maxPos2.x, maxPos2.y, maxPos2.z}};
	glVertexPointer(3, GL_FLOAT, 0, line);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif	
	glEnable(GL_LIGHTING);

	// Keep physics & graphics in sync
	int nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while(nbActors--)
	{
		NxActor* actor = *actors++;
		if(!actor->userData)	continue;

		glPushMatrix();
		float glmat[16];
		actor->getGlobalPose().getColumnMajor44(glmat);
		glMultMatrixf(glmat);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glutSolidCube(float(size_t(actor->userData))*2.0f);
		glPopMatrix();

		// Handle shadows
		glPushMatrix();
		const static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };

		glMultMatrixf(ShadowMat);
		glMultMatrixf(glmat);

		glDisable(GL_LIGHTING);
		glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
		glutSolidCube(float(size_t(actor->userData))*2.0f);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}

	//Sleep(20);
#ifdef __PPCGEKKO__	
	char buf[256];
    sprintf(buf,"FPS: %d\nRender time:%d ms \nPhysic time:%d ms \nTotal  time:%d ms\n"
    "Press the keys b,a,1,2, and HOME to create various things.\n"
	"Use the arrow keys to move the camera.\n" );

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
	// Initialize Glut
#ifdef __PPCGEKKO__
	printf("Press the keys b,a,1,2, and HOME to create various things.\n");
	printf("Use the arrow keys to move the camera.\n");
#else	
	printf("Press the keys w,space,s,b, and t to create various things.\n");
	printf("Use the arrow keys or 2, 4, 6 and 8 to move the camera, mouse to look around.\n");
	printf("Press p to pause the simulation.\n");
#endif	
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	int mainHandle = glutCreateWindow("SampleAsyncBoxes");
	glutSetWindow(mainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(ArrowKeyCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0, 0);

	// Setup default render states
	glClearColor(0.3f, 0.4f, 0.5f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);

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

#ifdef __PPCGEKKO__
	glutRemapButtonExt('b', 'w');
	glutRemapButtonExt('a', ' ');
	glutRemapButtonExt('1', 's');
	glutRemapButtonExt('2', 'b');
	glutRemapButtonExt(GLUT_KEY_HOME, 't');
	glutRemapButtonExt(GLUT_KEY_UP, '8');
	glutRemapButtonExt(GLUT_KEY_DOWN, '2');
	glutRemapButtonExt(GLUT_KEY_LEFT, '4');
	glutRemapButtonExt(GLUT_KEY_RIGHT, '6');
#endif	
	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitCallback);
		glutMainLoop();
	}
	
	return 0;
}
