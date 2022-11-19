// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Articulate Truck
// Description: This sample program shows how to use joints to create a truck.
//              Visualization is handled by the SDK debug visualization.
//
// Originally written by: Simon Schirm
//
// ===============================================================================

#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "GLFontRenderer.h"

#undef random

#include "NxPhysics.h"
#include "NxMath.h"

NxMaterialIndex terrainMaterial;
NxMaterialIndex wheelMaterial;

//headers of this sample code
#include "NxTerrain.h"
#include "NxTruck.h"

#include "DebugRenderer.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "UserAllocator.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;
static NxVec3			gDefaultGravity( 0.0f, -9.81f, 0.0f);
static UserAllocator*	gAllocator = NULL;

static bool       gKeyUp    = false;
static bool       gKeyDown  = false;
static bool       gKeyLeft  = false;
static bool       gKeyRight = false;

static bool       gDrive = true;
static unsigned     gVisNum = 0;

static DebugRenderer	gDebugRenderer;

static Truck truck;

static bool InitNx()
{
	if (!gAllocator)
		gAllocator = new UserAllocator;

  // Initialize PhysicsSDK
  NxPhysicsSDKDesc desc;
  NxSDKCreateError errorCode = NXCE_NO_ERROR;
  gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator, NULL, desc, &errorCode);
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

  // Create a scene
  NxSceneDesc sceneDesc;
  sceneDesc.gravity       = gDefaultGravity;

  gScene = gPhysicsSDK->createScene(sceneDesc);
  if(gScene == NULL) 
  {
    printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
    return false;
  }


  gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.075f);

  //create our materials

  NxMaterial * mat = gScene->getMaterialFromIndex(0);
  NxMaterialDesc  m;
  m.restitution   = 0.0f;
  m.staticFriction  = 2.0f;
  m.dynamicFriction = 1.0f;
  mat->loadFromDesc(m);

  m.restitution   = 0.0f;
  m.staticFriction  = 2.0f;
  m.dynamicFriction = 1.0f;
  terrainMaterial = gScene->createMaterial(m)->getMaterialIndex();
  m.restitution   = 0.0f;
  m.staticFriction  = 1.2f;
  m.dynamicFriction = 1.0f;
  wheelMaterial = gScene->createMaterial(m)->getMaterialIndex();

  InitTerrain( *gPhysicsSDK, *gScene);
  truck.create(*gScene, *gTerrain);

  // Debug rendering
  gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 2.0f);
  
#ifdef __PPCGEKKO__
  printf("Controls\n----------------------------------------------\n");
  printf("a:\t\ttoggle camera mode, truck mode\n");
  printf("Arrows:\t\tcontrol truck or camera\n");
  printf("+:\t\tswitch debug visualization\n");
#else
  printf("Controls\n----------------------------------------------\n");
  printf("Space:\t\ttoggle camera mode, truck mode\n");
  printf("Arrows:\t\tcontrol truck or camera\n");
  printf("Mouse:\t\tlook around\n");
  printf("v:\t\tswitch debug visualization\n");
#endif  

#if defined(_XBOX) | defined(__CELLOS_LV2__)
  glutRemapButtonExt(1, 'v', false); //B/O to 'v'
#endif
#if defined(__PPCGEKKO__)
  glutRemapButtonExt('+','v');
  glutRemapButtonExt('a',' ');
#endif
  return true;
}


// Simulation code

static int gMainHandle;
static NxVec3 Eye;
static NxVec3 cameraOffset(50.0f, 50.0f, 50.0f);
static NxVec3 Dir(-1,-1,-1);
static NxVec3 N;
static int mx = 0;
static int my = 0;

void cameraControl()
{
  if (gKeyUp)    cameraOffset += Dir * 1.0f;
  if (gKeyDown)  cameraOffset -= Dir * 1.0f;
  if (gKeyLeft)  cameraOffset -= N   * 1.0f;
  if (gKeyRight) cameraOffset += N   * 1.0f;
}

static void RenderCallback()
{
  if(gScene)
    {
    gScene->simulate(1.0/60.0f);
    gScene->flushStream();
    gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
    }
	
  NxVec3 truckPos = truck.trailer->getGlobalPosition();

  if (gDrive) 
  {
    Dir = -cameraOffset;
    Dir.normalize();
    truck.control(gKeyLeft, gKeyRight, gKeyUp, gKeyDown, 1.0/60.0f);
  }
  else    cameraControl();

  Eye = truckPos + cameraOffset;


  // Clear buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Setup camera
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 1000.0f);
  gluLookAt(Eye.x, Eye.y, Eye.z, Eye.x + Dir.x, Eye.y + Dir.y, Eye.z + Dir.z, 0.0f, 1.0f, 0.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  gDebugRenderer.renderData(*gScene->getDebugRenderable());
  RenderTerrain();
  truck.render();

#ifdef __PPCGEKKO__	
	char buf[256];
    sprintf(buf,
    "Controls\n"
  	"a:toggle camera mode, truck mode\n"
  	"Arrows:control truck or camera\n"
  	"+:switch debug visualization\n");

    GLFontRenderer::setScreenResolution(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	GLFontRenderer::setColor(0.9f, 1.0f, 0.0f, 1.0f);
	GLFontRenderer::print(0.01, 0.9, 0.036, buf, false, 11, true);   
#endif

  glFlush();
  glutSwapBuffers();
}

static void switchOffAllVisualization()
{
  gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 0);
  gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_POINT, 0);
  gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_NORMAL, 0);
  gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_FORCE, 0);
  gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_WORLD_AXES, 0);
  gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS, 0);
}


static void KeyboardCallback(unsigned char key, int x, int y)
{
  NxVec3 N;
  N.cross(Dir,NxVec3(0,1,0));

  if   (key==' ')    gDrive = !gDrive;
  else if   (key=='v')  
  {
    if (++gVisNum == 7) gVisNum = 0;

    switchOffAllVisualization();
    switch (gVisNum)
    {
      case 0: break;
      case 1: gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
          break;
      case 2: gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_POINT, 1);
          break;
      case 3: gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_NORMAL, 1);
          break;
      case 4: gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_FORCE, 1);
          break;
      case 5: gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_WORLD_AXES, 1);
          break;
      case 6: gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS, 1);
          break;
    }
  }
  else if (key==27) exit(0);


}

static void KeyboardSpecialCallback(int key, int x, int y)
{
  switch (key)
  {
    case GLUT_KEY_LEFT:  gKeyLeft = true; break;
    case GLUT_KEY_RIGHT: gKeyRight= true; break;
	case GLUT_KEY_UP:    gKeyUp   = true; break;
	case GLUT_KEY_DOWN:  gKeyDown = true; break;
    default: break;
  }
}

static void KeyboardSpecialUpCallback(int key, int x, int y)
{
  switch (key)
  {
    case GLUT_KEY_LEFT:  gKeyLeft = false; break;
    case GLUT_KEY_RIGHT: gKeyRight= false; break;
    case GLUT_KEY_UP:    gKeyUp   = false; break;
    case GLUT_KEY_DOWN:  gKeyDown = false; break;
    default: break;
  }
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

static void PassiveMotionCallback(int x, int y) 
{
  mx = x;
  my = y; 
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
  glutInit(&argc, argv);
  glutInitWindowSize(512, 512);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  gMainHandle = glutCreateWindow("SampleArticulateTruck");
  glutSetWindow(gMainHandle);
  glutDisplayFunc(RenderCallback);
  glutReshapeFunc(ReshapeCallback);
  glutIdleFunc(IdleCallback);
  glutKeyboardFunc(KeyboardCallback);
  glutSpecialFunc(KeyboardSpecialCallback);
  glutSpecialUpFunc(KeyboardSpecialUpCallback);
  MotionCallback(0,0);

  glutMotionFunc(MotionCallback);
  glutPassiveMotionFunc(PassiveMotionCallback);

  // Setup default render states
  glClearColor(0.5f, 0.4f, 0.5f, 1.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  //glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
#ifndef __PPCGEKKO__  
  glEnable(GL_CULL_FACE);
#endif  

  // Setup lighting
  glEnable(GL_LIGHTING);
  float AmbientColor[]  = { 0.1f, 0.1f, 0.1f, 0.0f };   glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
  float DiffuseColor[]  = { 1.0f, 1.0f, 1.0f, 0.0f };   glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
  float SpecularColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };   glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
  float Position[]    = { 0.0f, 10.0f, 40.0f, 1.0f };   glLightfv(GL_LIGHT0, GL_POSITION, Position);
  glEnable(GL_LIGHT0);

  GLfloat light_direction[]    = {cosf(-60.0f), 1.0f, sinf(-60.0f), 0.0f};
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);

  // Initialize physics scene and start the application main loop if scene was created
  if (InitNx())
  {
  	atexit(ExitCallback);
	glutMainLoop();
  }
  return 0;
}
