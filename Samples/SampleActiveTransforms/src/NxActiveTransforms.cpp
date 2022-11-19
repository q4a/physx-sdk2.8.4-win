// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Active Transforms Sample
// Description: This sample program shows how to utilize Active Transform
//              Notification to eliminate the updating of game object transforms
//              unnecessarily.
//
// ===============================================================================

#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "NxPhysics.h"
#include "ErrorStream.h"
#include "PerfRenderer.h"
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
static PerfRenderer    gPerfRenderer;

// Rendering
static NxVec3	gEye(50.0f, 50.0f, 50.0f);
static NxVec3	gDir(-0.6f,-0.2f,-0.7f);
static NxVec3	gViewY;
static int		gMouseX = 0;
static int		gMouseY = 0;

// An example game object that needs to synchronize its internal transform
// with the NxActor it contains.
class GameObject
{
public:

	GameObject(NxActor *physxObject, float boxSize) : m_physxObject(physxObject), m_boxSize(boxSize)
	{
		m_transform = physxObject->getGlobalPose();
	}

	NxMat34 getTransform(void) const               { return m_transform; }
	void    setTransform(const NxMat34 &transform) { m_transform = transform; }

	float   getBoxSize(void) const                 { return m_boxSize; }

private:

	NxActor *m_physxObject;
	NxMat34  m_transform;
	float    m_boxSize;
};

class GameManager
{
public:
	
	static void addActor(NxActor *actor, float boxSize)
	{
		GameObject *gameObject = new GameObject(actor, boxSize);

		if(gameObject)
		{
			m_gameObjectList.pushBack(gameObject);

			// set the user data of the actor to point to this game object
			actor->userData = (void *)gameObject;
		}
	};

	static void removeActor(NxActor *actor)
	{
		if(actor)
		{
			GameObject *gameObject = (GameObject *)actor->userData;
			int objectIndex = -1;

			if(gameObject)
			{
				int numGameObjects = (int) m_gameObjectList.size();

				for(int i = 0; i < numGameObjects; ++i)
				{
					if(m_gameObjectList[i] == gameObject)
					{
						objectIndex = i;
						break;
					}
				}
			}

			if(objectIndex != -1)
			{
				m_gameObjectList.replaceWithLast(objectIndex);
			}
		}
	}

	static NxU32 getNbGameObjects(void)
	{
		return m_gameObjectList.size();
	}

	static GameObject **getGameObjects(void)
	{
		return m_gameObjectList.begin();
	}

	static void shutDown(void)
	{
		for(NxU32 i = 0; i < m_gameObjectList.size(); ++i)
		{
			delete m_gameObjectList[i];
		}
	}

private:

	static NxArray<GameObject *> m_gameObjectList;
};

NxArray<GameObject *> GameManager::m_gameObjectList;

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
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.05f);

	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= NxVec3(0.0f, -9.81f, 0.0f);
	
	// Enable Active Transform Notification
	sceneDesc.flags |= NX_SF_ENABLE_ACTIVETRANSFORMS;

	gScene = gPhysicsSDK->createScene(sceneDesc);
	if(gScene == NULL) 
	{
		printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
		return false;
	}

	// Set default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	// Create ground plane
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	gScene->createActor(actorDesc);
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

	// free up the GameManager
	GameManager::shutDown();

	if (gAllocator)
	{
		delete gAllocator;
		gAllocator = NULL;
	}
}

static void CreateCube(const NxVec3& pos, int size=2, const NxVec3* initialVelocity=NULL)
{
	if(gScene == NULL) return;	

	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
	if(initialVelocity) bodyDesc.linearVelocity = *initialVelocity;

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3((float)size, (float)size, (float)size);

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t  = pos;
	NxActor *actor = gScene->createActor(actorDesc);

	// add the actor to the game manager
	GameManager::addActor(actor, (float)size);
}

static void CreateCubeFromEye(int size)
{
	NxVec3 t = gEye;
	NxVec3 vel = gDir;
	vel.normalize();
	vel*=200.0f;
	CreateCube(t, size, &vel);
}

static void CreateStack(int size)
{
	const float cubeSize = 2.0f;
	const float spacing = -2.0f*gPhysicsSDK->getParameter(NX_SKIN_WIDTH);
	NxVec3 pos(0.0f, cubeSize, 0.0f);
	float offset = -size * (cubeSize * 2.0f + spacing) * 0.5f;
	while(size)
	{
		for(int i=0;i<size;i++)
		{
			pos.x = offset + (float)i * (cubeSize * 2.0f + spacing);
			CreateCube(pos, (int)cubeSize);
		}

		offset += cubeSize;
		pos.y += (cubeSize * 2.0f + spacing);
		size--;
	}
}

static void CreateTower(int size)
{
	const float cubeSize = 2.0f;
	const float spacing = 0.01f;
	NxVec3 pos(0.0f, cubeSize, 0.0f);
	while(size)
	{
		CreateCube(pos, (int)cubeSize);
		pos.y += (cubeSize * 2.0f + spacing);
		size--;
	}
}

static void KeyboardCallback(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27:
			exit(0);
			break;
		case '0':	
			gPerfRenderer.toggleEnable(); 
			break;
		case ' ':	
			CreateCube(NxVec3(0.0f, 20.0f, 0.0f),1); 
			break;
		case 's':
		case 'S':
			CreateStack(10); 
			break;
		case 'b':	
		case 'B':	
			CreateStack(30); 
			break;
		case 't':
		case 'T':
			CreateTower(30);
			break;
		case 'w':
		case 'W':
			CreateCubeFromEye(8);
			break;
		case 'q':
		case 'Q':
			{
				NxActor** actors = gScene->getActors();
				if(gScene->getNbActors() > 1){
					NxActor *actor = actors[gScene->getNbActors()-1];
					GameManager::removeActor(actor);
					gScene->releaseActor(*actors[gScene->getNbActors()-1]);
				}
			}
			break;		
		case GLUT_KEY_UP:	case '8':	gEye += gDir*2.0f; break;
		case GLUT_KEY_DOWN: case '2':	gEye -= gDir*2.0f; break;
		case GLUT_KEY_LEFT:	case '4':	gEye -= gViewY*2.0f; break;
		case GLUT_KEY_RIGHT: case '6':	gEye += gViewY*2.0f; break;
	}
}

static void ArrowKeyCallback(int key, int x, int y)
{
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
	
	// Get the Active Transforms from the scene
	NxU32 nbTransforms = 0;
	NxActiveTransform *activeTransforms = gScene->getActiveTransforms(nbTransforms);

	// Start simulation (non blocking)
	gScene->simulate(1.0f/60.0f);
	
		
	// update the game objects for only the actors with active transforms
	if(nbTransforms && activeTransforms)
	{
		for(NxU32 i = 0; i < nbTransforms; ++i)
		{
			// the user data of the actor holds the game object pointer
			GameObject *gameObject = (GameObject *)activeTransforms[i].userData;

			// update the game object's transform to match the NxActor
			if(gameObject)
				gameObject->setTransform(activeTransforms[i].actor2World);
		}
	}

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Render any game objects in the GameManager
	NxU32        nbGameObjects  = GameManager::getNbGameObjects();
	GameObject **gameObjectList = GameManager::getGameObjects();

	if(nbGameObjects > 0)
	{
		// Setup projection matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
		gluLookAt(gEye.x, gEye.y, gEye.z, gEye.x + gDir.x, gEye.y + gDir.y, gEye.z + gDir.z, 0.0f, 1.0f, 0.0f);

		// Setup modelview matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Render all game objects
		for(NxU32 i = 0; i < nbGameObjects; ++i)
		{
			GameObject *gameObject = gameObjectList[i];

			// Render game object
			glPushMatrix();
			float glMat[16];
			gameObject->getTransform().getColumnMajor44(glMat);
			glMultMatrixf(glMat);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glutSolidCube(gameObject->getBoxSize()*2.0f);
			glPopMatrix();

			// Render shadow
			glPushMatrix();
			const static float shadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
			glMultMatrixf(shadowMat);
			glMultMatrixf(glMat);
			glDisable(GL_LIGHTING);
			glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
			glutSolidCube(gameObject->getBoxSize()*2.0f);
			glEnable(GL_LIGHTING);
			glPopMatrix();
		}
	}

	// Fetch simulation results
	gScene->flushStream();
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

#ifdef __PPCGEKKO__
	char buf[256];
    sprintf(buf,
		"Use the arrow keys to move the camera.\n"
		"Press the keys b, 1, 2 create various things.\n");

    GLFontRenderer::setScreenResolution(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	GLFontRenderer::setColor(0.9f, 1.0f, 0.0f, 1.0f);
	GLFontRenderer::print(0.01, 0.9, 0.036, buf, false, 11, true);   
#else
	// Print profile results (if enabled)
	gPerfRenderer.render(gScene->readProfileData(true), glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
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
	// Initialize glut
#ifdef __PPCGEKKO__
	printf("Use the arrow keys to move the camera.\n");
	printf("Press the keys b, 1, 2 create various things.\n");
#else	
	printf("Use the arrow keys or 2, 4, 6 and 8 to move the camera.\n");
	printf("Use the mouse to rotate the camera.\n");
	printf("Press the keys w, t, s, b and space to create various things.\n");
	printf("Press q to destroy the last thing created.\n");
	printf("Press 0 to toggle the frame rate display.\n");
#endif	
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	int mainHandle = glutCreateWindow("SampleActiveTransforms");
	glutSetWindow(mainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(ArrowKeyCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0,0);
	
#ifdef __PPCGEKKO__
	glutRemapButtonExt('1','s');
	glutRemapButtonExt('b','w');
	glutRemapButtonExt('a','0');
	glutRemapButtonExt('2','t');
	glutRemapButtonExt(GLUT_KEY_HOME,'0');
#endif

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

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitNx);
		glutMainLoop();
	}
	else
		ExitNx();

	return 0;
}
