// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Transforms Sample
// Description: This sample program shows the reference frames of the NVIDIA PhysX
//              SDK objects.
//
// Originally written by: Adam Moravanszky
//
// ===============================================================================
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>

// Physics code
#undef random
#include "NxPhysics.h"
#include "ErrorStream.h"
#include "cooking.h"
#include "Stream.h"
#include "DebugRenderer.h"
#include "Timing.h"
#include "UserAllocator.h"
#include "GLFontRenderer.h"
#include "MediaPath.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"

static ErrorStream		gErrorStream;
// Bunny data
extern unsigned int		BUNNY_NBVERTICES;
extern unsigned int		BUNNY_NBFACES;
extern float			gBunnyVertices[];
extern int				gBunnyTriangles[];
//slide show
extern void				tick(NxReal dt, bool click = false);

char displayString[512] = "...";

DebugRenderer gDebugRenderer;
//bunny related SDK variables
 NxTriangleMesh	* bunnyTriangleMesh;

bool				gPause = false;
NxPhysicsSDK*	gPhysicsSDK = NULL;
NxScene*			gScene = NULL;
NxVec3			gDefaultGravity(0.0f, -9.81f, 0.0f);
NxVec3*			gBunnyNormals = NULL;
UserAllocator*	gAllocator = NULL;

int				gMainHandle;
bool				gShadows = true;
NxVec3			Eye(3.0f, 1.0f, 3.0f);
NxVec3			Dir(-0.6,-0.2,-0.7);
NxVec3			N;
int				mx = 0;
int				my = 0;
NxVec3			gBunnyDrawOffset(0, 0.65f, 0);

NxU32 getFileSize(const char* name)
{
	#ifndef SEEK_END
	#define SEEK_END 2
	#endif
	
	FILE* File = fopen(name, "rb");
	if(!File)
		return 0;

	fseek(File, 0, SEEK_END);
	NxU32 eof_ftell = ftell(File);
	fclose(File);
	return eof_ftell;
}

static void RenderBunny()
{

	static float* pVertList = NULL;
	static float* pNormList = NULL;

	if(pVertList == NULL && pNormList == NULL)
	{
		pVertList = new float[BUNNY_NBFACES*3*3];
		pNormList = new float[BUNNY_NBFACES*3*3];

		int vertIndex = 0;
		int normIndex = 0;
		const int* indices = gBunnyTriangles;
		for(unsigned int i=0;i<BUNNY_NBFACES;i++)
		{
			for(int j=0;j<3;j++)
			{
				int vref0 = *indices++;
						
				pVertList[vertIndex++] = gBunnyVertices[vref0*3+0];
				pVertList[vertIndex++] = gBunnyVertices[vref0*3+1];
				pVertList[vertIndex++] = gBunnyVertices[vref0*3+2];

				pNormList[normIndex++] = gBunnyNormals[vref0].x;
				pNormList[normIndex++] = gBunnyNormals[vref0].y;
				pNormList[normIndex++] = gBunnyNormals[vref0].z;
			}
		}
	}

	if(pVertList != NULL && pNormList != NULL)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT, 0, pVertList);
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, pNormList);
	

		glDrawArrays(GL_TRIANGLES, 0, BUNNY_NBFACES*3);
	
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		
	}
}


// Base wrapper object kept in NxActor's user-data field
class MyNXWrapper
{
		public:
							MyNXWrapper(NxActor* actor) : owner(actor)	{}
							~MyNXWrapper()								{}

		virtual		void	render()	= 0;

		NxActor*	owner;
};

// Cube wrapper
class MyNXCube : public MyNXWrapper
{
		public:
							MyNXCube(NxActor* actor, NxF32 s) : MyNXWrapper(actor), size(s)	{}
							~MyNXCube()														{}

		virtual		void	render()
							{
							float glmat[16];
							glPushMatrix();
							owner->getGlobalPose().getColumnMajor44(glmat);
							glMultMatrixf(glmat);
							glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
							glutSolidCube(size*2.0f);
							glPopMatrix();

							// Handle shadows
							if(gShadows)
								{
								glPushMatrix();

								const  static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };

								glMultMatrixf(ShadowMat);
								glMultMatrixf(glmat);
								glDisable(GL_LIGHTING);
								glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
								glutSolidCube(size*2.0f);
								glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
								glEnable(GL_LIGHTING);

								glPopMatrix();
								}
							}

		NxF32			size;
};

// Sphere wrapper
class MyNXSphere : public MyNXWrapper
{
		public:
							MyNXSphere(NxActor* actor, NxF32 s) : MyNXWrapper(actor), size(s)	{}
							~MyNXSphere()														{}

		virtual		void	render()
							{
							float glmat[16];
							glPushMatrix();
							owner->getGlobalPose().getColumnMajor44(glmat);
							glMultMatrixf(glmat);
							glColor4f(0.8f, 1.0f, 0.8f, 1.0f);
							glutSolidSphere(size, 10, 10);
							glPopMatrix();

							// Handle shadows
							if(gShadows)
								{
								glPushMatrix();

								const  static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };

								glMultMatrixf(ShadowMat);
								glMultMatrixf(glmat);

								glDisable(GL_LIGHTING);
								glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
								glutSolidSphere(size, 10, 10);
								glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
								glEnable(GL_LIGHTING);

								glPopMatrix();
								}
							}

		NxF32			size;
};

// Bunny wrapper
class MyNXBunny : public MyNXWrapper
{
		public:
							MyNXBunny(NxActor* actor) : MyNXWrapper(actor)					{}
							~MyNXBunny()													{}

		virtual		void	render()
							{
							float glmat[16];
							glPushMatrix();
							NxMat34 m = owner->getGlobalPose();
							m.t += m.M * gBunnyDrawOffset;
							m.getColumnMajor44(glmat);
							glLoadMatrixf(glmat);
							glColor4f(235.0f/255.0f, 159.0f/255.0f, 177.0f/255.0f, 1.0f);
							RenderBunny();
							glPopMatrix();

							// Handle shadows
							if(gShadows)
								{
								glPushMatrix();

								const  static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };

								glMultMatrixf(ShadowMat);
								glMultMatrixf(glmat);

								glDisable(GL_LIGHTING);
								glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
								RenderBunny();
								glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
								glEnable(GL_LIGHTING);

								glPopMatrix();
								}
							}
};


void CreateCube(const NxVec3& pos, int size=2, const NxVec3* initial_velocity=NULL)
{
	// Create body
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;
	if(initial_velocity)	BodyDesc.linearVelocity = *initial_velocity;

	NxF32 coeff = NxF32(size)*0.1f;

	// Create box shape
	NxBoxShapeDesc BoxDesc;
	BoxDesc.dimensions		= NxVec3(coeff, coeff, coeff);

	// Create actor
	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&BoxDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 1.0f;
	ActorDesc.globalPose.t  = pos;
	NxActor* newActor = gScene->createActor(ActorDesc);

	// Create app-controlled wrapper
	MyNXCube* newCube = new MyNXCube(newActor, coeff);

	// Bind our class to NX class
	newActor->userData = newCube;
}

void CreateSphere(const NxVec3& pos, int size=2, const NxVec3* initial_velocity=NULL)
{
	// Create body
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;
	if(initial_velocity)	BodyDesc.linearVelocity = *initial_velocity;

	NxF32 radius = NxF32(size)*0.1f;

	// Create sphere shape
	NxSphereShapeDesc SphereDesc;
	SphereDesc.radius		= radius;

	// Create actor
	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&SphereDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 1.0f;
	ActorDesc.globalPose.t  = pos;
	NxActor* newActor = gScene->createActor(ActorDesc);

	// Create app-controlled wrapper
	MyNXSphere* newSphere = new MyNXSphere(newActor, radius);

	// Bind our class to NX class
	newActor->userData = newSphere;
}

bool InitBunny()
{
	// Scale the bunny a bit
//	for(NxU32 i=0;i<BUNNY_NBVERTICES*3;i++)	gBunnyVertices[i] *= 10.0f;

	// Build vertex normals (used only for rendering)
	gBunnyNormals = new NxVec3[BUNNY_NBVERTICES];
	NxBuildSmoothNormals(BUNNY_NBFACES, BUNNY_NBVERTICES, (const NxVec3*)gBunnyVertices, (const NxU32*)gBunnyTriangles, NULL, gBunnyNormals, true);

	// Initialize PMap
	NxPMap bunnyPMap;
	bunnyPMap.dataSize	= 0;
	bunnyPMap.data		= NULL;

	// Build physical model
	NxTriangleMeshDesc bunnyDesc;
	bunnyDesc.numVertices				= BUNNY_NBVERTICES;
	bunnyDesc.numTriangles				= BUNNY_NBFACES;
	bunnyDesc.pointStrideBytes			= sizeof(NxVec3);
	bunnyDesc.triangleStrideBytes		= 3*sizeof(NxU32);
	bunnyDesc.points					= gBunnyVertices;
	bunnyDesc.triangles					= gBunnyTriangles;							
	bunnyDesc.flags						= 0;

	bool status = InitCooking(gAllocator);
	if (!status) {
		printf("Error: Unable to initialize the cooking library. Please make sure that you have correctly installed the latest version of the NVIDIA PhysX SDK.\n");
		return false;
	}
	MemoryWriteBuffer buf;
	status = CookTriangleMesh(bunnyDesc, buf);
	if (!status) {
		printf("Error: Unable to cook a triangle mesh.\n");
		return false;
	}
	MemoryReadBuffer readBuffer(buf.data);
	bunnyTriangleMesh = gPhysicsSDK->createTriangleMesh(readBuffer);
	//
	// Please note about the created Triangle Mesh, user needs to release it when no one uses it to save memory. It can be detected
	// by API "meshData->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseTriangleMesh(*meshData);"
	//

	CloseCooking();
	return true;
}

NxActor* CreateBunny(const NxVec3 & pos, bool detailed=true)
{
	// Create body
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;
	BodyDesc.maxAngularVelocity	= 10.0f;

	NxActorDesc ActorDesc;
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 1.0f;
	ActorDesc.globalPose.t  =  pos;

	if (detailed) {
		{
			NxSphereShapeDesc& sphere = *(new NxSphereShapeDesc());
			sphere.radius = 0.6f;
			sphere.localPose.t = NxVec3(0, 0.55f, -0.4f);
			ActorDesc.shapes.pushBack(&sphere);
		}
		{
			NxSphereShapeDesc& sphere = *(new NxSphereShapeDesc());
			sphere.radius = 0.2f;
			sphere.localPose.t = NxVec3(0, 0.55f+0.3, -0.4f-0.5);
			ActorDesc.shapes.pushBack(&sphere);
		}
		{
			NxSphereShapeDesc& sphere = *(new NxSphereShapeDesc());
			sphere.radius = 0.35f;
			sphere.localPose.t = NxVec3(0, 0.7f, 0.65f);
			ActorDesc.shapes.pushBack(&sphere);
		}
		{
			NxSphereShapeDesc& sphere = *(new NxSphereShapeDesc());
			sphere.radius = 0.4f;
			sphere.localPose.t = NxVec3(0, 0.6f, 0.3f);
			ActorDesc.shapes.pushBack(&sphere);
		}
		{
			NxSphereShapeDesc& sphere = *(new NxSphereShapeDesc());
			sphere.radius = 0.5f;
			sphere.localPose.t = NxVec3(0, 0.65f, -0.15f);
			ActorDesc.shapes.pushBack(&sphere);
		}

		{
			NxBoxShapeDesc& box = *(new NxBoxShapeDesc());
			box.dimensions = NxVec3(0.1f, 0.1f, 0.3f);
			box.localPose.t = NxVec3(0.2f, 0.05f, -0.4f);
			ActorDesc.shapes.pushBack(&box);
		}
		{
			NxBoxShapeDesc& box = *(new NxBoxShapeDesc());
			box.dimensions = NxVec3(0.1f, 0.1f, 0.3f);
			box.localPose.t = NxVec3(-0.2f, 0.05f, -0.4f);
			ActorDesc.shapes.pushBack(&box);
		}
		{
			NxBoxShapeDesc& box = *(new NxBoxShapeDesc());
			box.dimensions = NxVec3(0.1f, 0.1f, 0.1f);
			box.localPose.t = NxVec3(0.2f, 0.05f, 0.4f);
			ActorDesc.shapes.pushBack(&box);
		}
		{
			NxBoxShapeDesc& box = *(new NxBoxShapeDesc());
			box.dimensions = NxVec3(0.1f, 0.1f, 0.1f);
			box.localPose.t = NxVec3(-0.2f, 0.05f, 0.4f);
			ActorDesc.shapes.pushBack(&box);
		}
		{
			NxBoxShapeDesc& box = *(new NxBoxShapeDesc());
			box.dimensions = NxVec3(0.1f, 0.25f, 0.1f);
			box.localPose.t = NxVec3(-0.2f, 0.7f+0.5f, 0.5f);
			box.localPose.M.rotZ(0.1f);
			box.localPose.M.rotX(-0.4f);
			ActorDesc.shapes.pushBack(&box);
		}
		{
			NxBoxShapeDesc& box = *(new NxBoxShapeDesc());
			box.dimensions = NxVec3(0.1f, 0.25f, 0.1f);
			box.localPose.t = NxVec3(0.2f, 0.7f+0.5f, 0.5f);
			box.localPose.M.rotZ(-0.1f);
			box.localPose.M.rotX(-0.4f);
			ActorDesc.shapes.pushBack(&box);
		}
	} else {
		{
			NxSphereShapeDesc& sphere = *(new NxSphereShapeDesc());
			sphere.radius = 0.6f;
			sphere.localPose.t = NxVec3(0, 0.55f, -0.4f);
			ActorDesc.shapes.pushBack(&sphere);
		}
		{
			NxBoxShapeDesc& box = *(new NxBoxShapeDesc());
			box.dimensions = NxVec3(0.4f, 0.1f, 0.6f);
			box.localPose.t = NxVec3(0.0f, 0.05f, -0.1f);
			ActorDesc.shapes.pushBack(&box);
		}
		{
			NxSphereShapeDesc& sphere = *(new NxSphereShapeDesc());
			sphere.radius = 0.5f;
			sphere.localPose.t = NxVec3(0, 0.8f, 0.5f);
			ActorDesc.shapes.pushBack(&sphere);
		}
	}
	NxActor* newActor = gScene->createActor(ActorDesc);

	// Clean up allocations
	for (NxU32 i = 0; i < ActorDesc.shapes.size(); i++) {
		delete ActorDesc.shapes[i];
	}
	
	// Create app-controlled wrapper
	MyNXBunny* newBunny = new MyNXBunny(newActor);

	// Bind our class to NX class
	newActor->userData = newBunny;
	return newActor;
}

 bool InitNx()
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
	if (gPhysicsSDK->getFoundationSDK().getRemoteDebugger() && !gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
#endif

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.005f);

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

	// Initialize bunny data
	return InitBunny();
}

 void KeyboardCallback(unsigned char key, int x, int y)
{

switch (key)
	{
	case 27:
		exit(0);
		break;
	case ' ':
		tick(0, true);
		break;

	case 201: case '8':	case 'w': Eye += Dir * 2.0f; break;
	case 203: case '2':	case 's': Eye -= Dir * 2.0f; break;
	case 200: case '4':	case 'a': Eye -= N * 2.0f; break;
	case 202: case '6':	case 'd': Eye += N * 2.0f; break;
	}
}

 void ArrowKeyCallback(int key, int x, int y)
	{
	KeyboardCallback(key+100,x,y);
	}

 void MouseCallback(int button, int state, int x, int y)
{
	mx = x;
	my = y;
}

 void MotionCallback(int x, int y)
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

void DisplayText()
{
	float y = 0.95f;
	int len = (int)strlen(displayString);
	len = (len < 512)?len:511;
	int start = 0;
	char textBuffer[512];
	for(int i=0;i<len;i++)
	{
		if(displayString[i] == '\n' || i == len-1)
		{
			int offset = i;
			if(i == len-1) offset= i+1;
			memcpy(textBuffer, displayString+start, offset-start);
			textBuffer[offset-start]=0;
			GLFontRenderer::print(0.01, y, 0.03f, textBuffer);
			y -= 0.035f;
			start = offset+1;
		}
	}
}

void RenderCallback()
{
	static unsigned int PreviousTime = getTime();
	unsigned int CurrentTime = getTime();
	unsigned int ElapsedTime = CurrentTime - PreviousTime;
	PreviousTime = CurrentTime;
	NxF32 elapsedTime = NxF32(ElapsedTime)*0.001f;

	// Physics code
	if(gScene && !gPause)	
		{
		tick(elapsedTime);
		gScene->simulate(elapsedTime);
		gScene->flushStream();
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
		}
	// ~Physics code

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, ((float)glutGet(GLUT_WINDOW_WIDTH))/((float)glutGet(GLUT_WINDOW_HEIGHT)), 1.0f, 10000.0f);
	gluLookAt(Eye.x, Eye.y, Eye.z, Eye.x + Dir.x, Eye.y + Dir.y, Eye.z + Dir.z, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Keep physics & graphics in sync
	int nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while(nbActors--)
	{
		NxActor* actor = *actors++;

		MyNXWrapper* wrapper = (MyNXWrapper*)actor->userData;
		if(!wrapper)	continue;	// Happens for ground plane

		wrapper->render();
	}

	glClear(GL_DEPTH_BUFFER_BIT);	//clear z for visualization.

	gDebugRenderer.renderData(*gScene->getDebugRenderable());

	DisplayText();


	glFlush();
	glutSwapBuffers();
}

 void ReshapeCallback(int width, int height)
{
	glViewport(0, 0, width, height);
}

 void IdleCallback()
{
	glutPostRedisplay();
}

void ExitNx()
{
	if (gPhysicsSDK)
	{
		if (gScene) gPhysicsSDK->releaseScene(*gScene);
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

int main(int argc, char** argv)
{
	printf("Use the mouse to rotate the camera.\n");
	printf("Use the arrow keys or 2, 4, 6 and 8 to move the camera.\n");
	printf("Press spacebar to advance the tutorial.\n");

	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(640, 480);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("SampleTransforms");
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
		atexit(ExitNx);
		glutMainLoop();
	}

	return 0;
}
