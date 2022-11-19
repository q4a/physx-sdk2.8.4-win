// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Dynamic-Dynamic CCD
// Description: This sample program shows the effect of the dynamic vs. dynamic
//              CCD code. CCD is useful when small objects move fast, in order to
//              stop objects from tunneling through each other.
//
// Originally written by: Marten Svanfeldt (Oct 2005)
//
// ===============================================================================

#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "NxPhysics.h"
#include "NxCooking.h"

#include "ErrorStream.h"
#include "Timing.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "UserAllocator.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

// Physics
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;
static UserAllocator*	gAllocator = NULL;
static bool				ccdEnabled = 1;

// Rendering
static NxVec3	gEye(7.0f, 7.0f, 7.0f);
static NxVec3	gDir(-0.6f,-0.2f,-0.7f);
static NxVec3	gViewY;
static int		gMouseX = 0;
static int		gMouseY = 0;

static bool InitNx()
{
	if (!gAllocator)
		gAllocator = new UserAllocator;

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator, new ErrorStream(), desc, &errorCode);
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

	gPhysicsSDK->setParameter(NX_CONTINUOUS_CD, ccdEnabled);
	gPhysicsSDK->setParameter(NX_CCD_EPSILON, 0.01f);
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.0025f);

	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= NxVec3(0.0f, -9.81f, 0.0f);
	gScene = gPhysicsSDK->createScene(sceneDesc);
	if(gScene == NULL) 
	{
		printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
		return false;
	}

	// Set default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.8f);
	defaultMaterial->setDynamicFriction(0.5f);

	// Create ground plane
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	gScene->createActor(actorDesc);

#if defined(_XBOX) | defined(__CELLOS_LV2__)
	glutRemapButtonExt(0, 'b', false); //A/X to big box stack
	glutRemapButtonExt(8, 'c', false); //Left shoulder to toggle CCD
	glutRemapButtonExt(9, 'r', false); //Right shoulder to reset scene
#endif

	return true;
}

static void ExitNx()
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

static NxCCDSkeleton* CreateCCDSkeleton (float size)
{
	NxU32 triangles[3 * 12] = { 
		0,1,3,
		0,3,2,
		3,7,6,
		3,6,2,
		1,5,7,
		1,7,3,
		4,6,7,
		4,7,5,
		1,0,4,
		5,1,4,
		4,0,2,
		4,2,6
	};

	NxVec3 points[8];

	//static mesh
	points[0].set( size, -size, -size);
	points[1].set( size, -size,  size);
	points[2].set( size,  size, -size);
	points[3].set( size,  size,  size);

	points[4].set(-size, -size, -size);
	points[5].set(-size, -size,  size);
	points[6].set(-size,  size, -size);
	points[7].set(-size,  size,  size);

	NxSimpleTriangleMesh stm;
	stm.numVertices = 8;
	stm.numTriangles = 6*2;
	stm.pointStrideBytes = sizeof(NxVec3);
	stm.triangleStrideBytes = sizeof(NxU32)*3;

	stm.points = points;
	stm.triangles = triangles;
	stm.flags |= NX_MF_FLIPNORMALS;
	return gPhysicsSDK->createCCDSkeleton(stm);
}

static void CreateCube(const NxVec3& pos, float size=2.0f, const NxVec3* initialVelocity=NULL, bool doDynamicCCD=false)
{
	if(gScene == NULL) return;	

	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
	if(initialVelocity) bodyDesc.linearVelocity = *initialVelocity;

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3((float)size, (float)size, (float)size);
	boxDesc.ccdSkeleton = CreateCCDSkeleton(size*0.8f);
	if (doDynamicCCD)
		boxDesc.shapeFlags |= NX_SF_DYNAMIC_DYNAMIC_CCD; //Activate dynamic-dynamic CCD for this body

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t  = pos;

	gScene->createActor(actorDesc)->userData = (void*) *((size_t*)&size);

	
}

static void CreateCubeFromEye(float size)
{
	NxVec3 t = gEye;
	NxVec3 vel = gDir;
	vel.normalize();
	vel*=344.0f;
	CreateCube(t, size, &vel, true);
}

static void CreateStack(int size)
{
	const float cubeSize = 0.2f;
	const float spacing = -2.0f*gPhysicsSDK->getParameter(NX_SKIN_WIDTH);
	NxVec3 pos(0.0f, cubeSize, 0.0f);
	float offset = -size * (cubeSize * 2.0f + spacing) * 0.5f;
	while(size)
	{
		for(int i=0;i<size;i++)
		{
			pos.x = offset + (float)i * (cubeSize * 2.0f + spacing);
			CreateCube(pos, cubeSize);
		}

		offset += cubeSize;
		pos.y += (cubeSize * 2.0f + spacing);
		size--;
	}
}

static void CreateTower(int size)
{
	const float cubeSize = 0.2f;
	const float spacing = 0.001f;
	NxVec3 pos(0.0f, cubeSize, 0.0f);
	while(size)
	{
		CreateCube(pos, cubeSize);
		pos.y += (cubeSize * 2.0f + spacing);
		size--;
	}
}

void ReleaseActorAndCCD(NxActor* actor) 
{
	NxArray<NxCCDSkeleton*> skeletons;
	for (NxU32 i = 0; i < actor->getNbShapes(); i++) 
	{
		NxShape* shape = actor->getShapes()[i];
		if (shape->getCCDSkeleton() != NULL) {
			skeletons.pushBack(shape->getCCDSkeleton());
		}
	}
	gScene->releaseActor(*actor);
	for (NxU32 i = 0; i < skeletons.size(); i++) 
	{
		gPhysicsSDK->releaseCCDSkeleton(*skeletons[i]);
	}
}

static void KeyboardCallback(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 27:	exit(0); break;
	case 's':	CreateStack(10); break;
	case 'b':	CreateStack(30); break;
	case 't':	CreateTower(10); break;
	case 'w':	CreateCubeFromEye(0.1f) ;break;
	case 'r':
		{
			while(gScene->getNbActors() > 1){
				NxActor** actors = gScene->getActors();
				ReleaseActorAndCCD(actors[gScene->getNbActors()-1]);
			}
		}
		break;
	case 'c':
		{
			ccdEnabled ^= 1;
			printf("CCD %s\n", ccdEnabled?"enabled":"disabled");
			gPhysicsSDK->setParameter(NX_CONTINUOUS_CD, ccdEnabled);
		}
		break;

#ifdef __PPCGEKKO__
	case '8':	gEye.y += 0.2f; break;
	case '2':	gEye.y -= 0.2f; break;
	case '4':	gEye -= gViewY*0.2f; break;
	case '6':	gEye += gViewY*0.2f; break;
#else
	case '8':	gEye += gDir*2.0f; break;
	case '2':	gEye -= gDir*2.0f; break;
	case '4':	gEye -= gViewY*2.0f; break;
	case '6':	gEye += gViewY*2.0f; break;
#endif	
	}
}

static void ArrowKeyCallback(int key, int x, int y)
{
	//Handle keys
	switch (key)
	{
	case GLUT_KEY_UP:		key = '8'; break;
	case GLUT_KEY_DOWN:		key = '2'; break;
	case GLUT_KEY_LEFT:		key = '4'; break;
	case GLUT_KEY_RIGHT:	key = '6'; break;
	}
	KeyboardCallback(key,x,y);
}

static void MouseCallback(int button, int state, int x, int y)
{
	gMouseX = x;
	gMouseY = y;
}

static void MotionCallback(int x, int y)
{
	int dx = gMouseX - x;
	int dy = gMouseY - y;

	gDir.normalize();
	gViewY.cross(gDir, NxVec3(0,1,0));

	NxQuat qx(NxPiF32 * dx * 20/ 180.0f, NxVec3(0,1,0));
	qx.rotate(gDir);
	NxQuat qy(NxPiF32 * dy * 20/ 180.0f, gViewY);
	qy.rotate(gDir);

	gMouseX = x;
	gMouseY = y;
}

static void RenderCallback()
{
	if(gScene == NULL) return;

	// Start simulation (non blocking)
	NxF32 elapsedTime = getElapsedTime();
	if (elapsedTime> 0.02f) elapsedTime = 0.02f;
	gScene->simulate(elapsedTime);

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Setup projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, ((float)glutGet(GLUT_WINDOW_WIDTH))/((float)glutGet(GLUT_WINDOW_HEIGHT)), 1.0f, 10000.0f);
	gluLookAt(gEye.x, gEye.y, gEye.z, gEye.x + gDir.x, gEye.y + gDir.y, gEye.z + gDir.z, 0.0f, 1.0f, 0.0f);

	// Setup modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Render all actors
	int nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while(nbActors--)
	{
		NxActor* actor = *actors++;
		if(!actor->userData) continue;

		// Render actor
		glPushMatrix();
		float glMat[16];
		actor->getGlobalPose().getColumnMajor44(glMat);
		glMultMatrixf(glMat);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glutSolidCube((*((float*)&actor->userData))*2.0f);
		glPopMatrix();

		// Render shadow
		glPushMatrix();
		const static float shadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
		glMultMatrixf(shadowMat);
		glMultMatrixf(glMat);
		glDisable(GL_LIGHTING);
		glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
		glutSolidCube((*((float*)&actor->userData))*2.0f);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}

	// Fetch simulation results
	gScene->flushStream();
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);


#ifdef __PPCGEKKO__	
	char buf[4096];
    sprintf(buf,
    "Dynamic-dynamic CCD sample.\n\n"
	"All boxes created have a CCD-skeleton, those shot from the eye\n"
	"have dynamic-dynamic CCD enabled as well.\n"
	"Keyboard shortcuts:\n"
	" 1  - Create a small stack\n"
	" 2  - Create a big stack\n"
	" +  - Create a box tower\n"
	" b  - Shoot a small, fast, box into the scene\n"
	" a  - Toggle (enable/disable) CCD\n"
	"Use the arrow keys to move the camera.\n"
	"CCD %s\n",
        	ccdEnabled?"enabled":"disabled");

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
	// Help-text
	printf("Dynamic-dynamic CCD sample.\n\n");
	printf("All boxes created have a CCD-skeleton, those shot from the eye have dynamic-dynamic CCD enabled as well.\n");
	printf("Keyboard shortcuts:\n");
#ifdef __PPCGEKKO__
	printf(" 1  - Create a small stack\n");
	printf(" 2  - Create a big stack\n");
	printf(" +  - Create a box tower\n");
	printf(" b  - Shoot a small, fast, box into the scene\n");
	printf(" a  - Toggle (enable/disable) CCD\n");
	printf("Use the arrow keys to move the camera.\n");
#else	
	printf(" s  - Create a small stack\n");
	printf(" b  - Create a big stack\n");
	printf(" t  - Create a box tower\n");
	printf(" w  - Shoot a small, fast, box into the scene\n");
	printf(" r  - Reset the scene\n");
	printf(" c  - Toggle (enable/disable) CCD\n");
	printf("Use the arrow keys or 2, 4, 6 and 8 to move the camera, mouse to look around.\n");
#endif

	// Initialize glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	int mainHandle = glutCreateWindow("SampleCCDDynamic");
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

	// Setup lighting
	glEnable(GL_LIGHTING);
	float ambientColor[]	= { 0.0f, 0.1f, 0.2f, 0.0f };
	float diffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };		
	float specularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		
	float position[]		= { 100.0f, 100.0f, 400.0f, 1.0f };		
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);
	
	#ifdef __PPCGEKKO__
	glutRemapButtonExt(GLUT_KEY_UP, '8');
	glutRemapButtonExt(GLUT_KEY_DOWN, '2');
	glutRemapButtonExt(GLUT_KEY_LEFT, '4');
	glutRemapButtonExt(GLUT_KEY_RIGHT, '6');
	glutRemapButtonExt('1', 's');
	glutRemapButtonExt('2', 'b');
	glutRemapButtonExt('+', 't');
	glutRemapButtonExt('b', 'w');
	glutRemapButtonExt('a', 'c');
	#endif

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitNx);
		glutMainLoop();
	}

	return 0;
}
