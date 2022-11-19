// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Raycast Sample
// Description: This sample program shows how to build different cars using raycasts
//              and general actors.
//
// ===============================================================================

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "SampleRaycastCar.h"
#include "ErrorStream.h"
#include "DebugRenderer.h"
#include "Timing.h"
#include "DrawObjects.h"
#include "NxAllVehicles.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "cooking.h"
#include "UserAllocator.h"
#include "common.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

CreationModes		creationMode = MODE_CAR;
bool				gPause = false;
bool				gDebugVisualization = false;
bool				oldStyle = false;
NxPhysicsSDK*		gPhysicsSDK = 0;
NxScene*			gScene = 0;
NxVec3				gDefaultGravity(0.0f, -10.0f, 0.0f);
ErrorStream			gErrorStream;
DebugRenderer       gDebugRenderer;
UserAllocator*		gAllocator = NULL;
bool keyDown[256];

// Focus actor
NxActor* gSelectedActor = NULL;

static NxVec3 Eye(250, 50, 65);
static NxVec3 Dir(-0.65,-0.53,-0.54);
static NxVec3 CameraPos(250, 50, 65);
static NxVec3 CameraDir(-0.65, -0.53, -0.54);
static NxVec3 N;
static int mx = 0;
static int my = 0;
static NxI32 gLastVehicleNumber = 0;

static NxMaterialIndex defaultMaterialIndex;
static NxMaterialIndex somewhatBouncyMaterialIndex;
static NxMaterialIndex veryBouncyMaterialIndex;
static NxMaterialIndex frictionlessMaterialIndex;
static NxMaterialIndex highFrictionMaterialIndex;
static NxMaterialIndex anisoMaterialIndex;

extern NxMaterialIndex materialIce, materialRock, materialMud, materialGrass, materialDefault;

static void CreateCube(const NxVec3& pos, const NxVec3* initial_velocity=NULL)
{
	// Create body
	NxBodyDesc BodyDesc;
	if(initial_velocity)	BodyDesc.linearVelocity = *initial_velocity;

	NxBoxShapeDesc boxDesc;
	NxReal size = 0.5f;
	boxDesc.dimensions		= NxVec3(size, size, size);
	BodyDesc.mass = 10.f;

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&boxDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.globalPose.t  = pos;
	gScene->createActor(ActorDesc);
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
	if (gPhysicsSDK->getFoundationSDK().getRemoteDebugger() && !gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
#endif

	if (!InitCooking(gAllocator, &gErrorStream)) {
		printf("\nError: Unable to initialize the cooking library, exiting the sample.\n\n");
		return false;
	}

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.025f);
	//enable visualisation
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1.0f);

	gPhysicsSDK->setParameter(NX_VISUALIZE_BODY_AXES, 1.0f);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1.0f);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_AXES, 1.0f);
	gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_POINT, 1.0f);
	gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_NORMAL, 1.0f);

	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1.0f);
	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_WORLD_AXES, 1.0f);
	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS, 1.0f);

	// Don't slow down jointed objects
	gPhysicsSDK->setParameter(NX_ADAPTIVE_FORCE, 0.0f);
	
	//create some materials -- note that we reuse the same NxMaterial every time,
	//as it gets copied to the SDK with every setMaterial() call, not just referenced.
	
	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= gDefaultGravity;
	sceneDesc.userContactReport		= carContactReport;
	gScene = gPhysicsSDK->createScene(sceneDesc);
	if(gScene == NULL) 
	{
		printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
		return false;
	}
	
	//default material
	defaultMaterialIndex = 0;
	NxMaterial * defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.8f);
	defaultMaterial->setDynamicFriction(0.8f);

	//create ground plane
	NxActorDesc actorDesc;
	//embed some simple shapes in terrain to make sure we can drive on them:
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(10,1,5);
	boxDesc.localPose.t.set(30,0,30);
	boxDesc.localPose.M.fromQuat(NxQuat(-30, NxVec3(0,0,1)));
	actorDesc.shapes.pushBack(&boxDesc);
	gScene->createActor(actorDesc);

	actorDesc.setToDefault();
	NxSphereShapeDesc sphereShape;
	sphereShape.radius = 20;
	sphereShape.localPose.t.set(140, -18, 0);
	actorDesc.shapes.pushBack(&sphereShape);
	gScene->createActor(actorDesc);

	actorDesc.setToDefault();
	NxCapsuleShapeDesc capsuleShape;
	capsuleShape.radius = 10.0f;
	capsuleShape.height = 10.0f;
	capsuleShape.localPose.t.set(100, -8, 0);
	capsuleShape.localPose.M.setColumn(1, NxVec3(0,0,1));
	capsuleShape.localPose.M.setColumn(2, NxVec3(0,-1,0));
	actorDesc.shapes.pushBack(&capsuleShape);

	gScene->createActor(actorDesc);

	// Terrain
	InitTerrain();

	// Turn on all contact notifications:
	gScene->setActorGroupPairFlags(0, 0, NX_NOTIFY_ON_TOUCH);

	glColor4f(1.0f,1.0f,1.0f,1.0f);

	return true;
}

static void getCurrentPosAndDirection(NxVec3& pos, NxVec3& direction) {
	if(NxAllVehicles::getActiveVehicle() != NULL)
	{
		pos = CameraPos;
		direction = CameraDir;
	}
	else
	{
		pos = Eye;
		direction = Dir;
	}

}

static void appKey(unsigned char key, bool down)
{
	if (!down)
		return;
#ifndef __PPCGEKKO__	
	bool alt = (glutGetModifiers() & GLUT_ACTIVE_ALT) > 0;
#endif
	switch(key)
	{
		case 27:	exit(0); break;
		case 'p':	gPause = !gPause; break;
		case 'f':	if (NxAllVehicles::getActiveVehicle()) NxAllVehicles::getActiveVehicle()->standUp(); 
					break;
		case 'v':	gDebugVisualization = !gDebugVisualization; break;

		case '+':	if (NxAllVehicles::getActiveVehicle()) NxAllVehicles::getActiveVehicle()->gearUp(); 
					break;
		case '-':	if (NxAllVehicles::getActiveVehicle()) NxAllVehicles::getActiveVehicle()->gearDown(); 
					break;
		case 'e':
			{
			NxAllVehicles::selectNext();
			} break;
		case 'r':
			{
			NxVec3 t;
			NxVec3 vel;
			getCurrentPosAndDirection(t, vel);
			
			vel.normalize();
			vel*=30.0f;
			CreateCube(t, &vel);
			}
			break;
		case 'c': {
			if (NxAllVehicles::getActiveVehicleNumber() < 0) {
				NxAllVehicles::setActiveVehicle(gLastVehicleNumber);
			} else {
				gLastVehicleNumber = NxMath::max(0, NxAllVehicles::getActiveVehicleNumber());
				NxAllVehicles::setActiveVehicle(-1);
			}
			break;
		}
	}
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

void RenderAllActors()
{
	glColor4f(0.6f,0.6f,0.6f,1.0f);

	for(unsigned int i=0;i<gScene->getNbActors();i++)
	{
		DrawActor(gScene->getActors()[i]);
	}
}

static void RenderCallback()
{

	static unsigned int PreviousTime = 0;
	unsigned int CurrentTime = getTime();
	unsigned int ElapsedTime = CurrentTime - PreviousTime;
	if(ElapsedTime < 10.0f) return;

	PreviousTime = CurrentTime;

	// Clear buffers -- do it now so we can render some debug stuff in tickCar.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Physics code
	if(gScene && !gPause)	
	{
		//tickCar();
#ifdef __PPCGEKKO__
		bool left = keyDown['a'];
		bool right = keyDown['d'];
		bool forward = keyDown['w'];
		bool backward = keyDown['s'];
#else
		bool left = keyDown['a'] || keyDown[20];
		bool right = keyDown['d'] || keyDown[22];
		bool forward = keyDown['w'] || keyDown[21];
		bool backward = keyDown['s'] || keyDown[23];
#endif		
		NxReal steering = 0;
		if (left && !right) steering = -1;
		else if (right && !left) steering = 1;
		NxReal acceleration = 0;
		if (forward && !backward) acceleration = 1;
		else if (backward && !forward) acceleration = -1;
		bool handbrake = keyDown[' '];
		if(NxAllVehicles::getActiveVehicle())
			NxAllVehicles::getActiveVehicle()->control(steering, false, acceleration, false, handbrake);
		NxAllVehicles::updateAllVehicles(1.0f/60.f);

		gScene->simulate(1.0f/60.0f);	//Note: a real application would compute and pass the elapsed time here.
		gScene->flushStream();
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
	}
	// ~Physics code

	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	
	if(NxAllVehicles::getActiveVehicle() != NULL)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		#define BUFFERSIZE 25
		static NxMat34 mbuffer[BUFFERSIZE];
		static int frameNum = 0;
		int index = frameNum % BUFFERSIZE;
  	
		NxMat34 camera, cameraInv;
		NxVec3 cameraZ, lookAt, cr, tmp;
		NxF32 cameraInvMat[16];

		NxMat34 car = NxAllVehicles::getActiveVehicle()->getGlobalPose();
	  
		car.M.getColumn(2, cr);
		car.M.getColumn(0, tmp);
		car.M.setColumn(0, cr);
		car.M.setColumn(2, -tmp);

		if(frameNum == 0)
		{
			for(int i=0; i<BUFFERSIZE; i++)
				mbuffer[i] = car;
		}

		camera = mbuffer[index];
		mbuffer[index] = car;

		camera.t.y += NxAllVehicles::getActiveVehicle()->getCameraDistance() * 0.5f;	//camera height

		camera.M.getColumn(2, cameraZ);
		camera.t += (NxAllVehicles::getActiveVehicle()->getCameraDistance() * cameraZ);

		lookAt = (camera.t - car.t);
		lookAt.normalize();

		camera.M.setColumn(2, lookAt);
		cr = NxVec3(0,1,0).cross(lookAt);
		cr.normalize();
		camera.M.setColumn(0, cr);
		cr = lookAt.cross(cr);
		cr.normalize();
		camera.M.setColumn(1, cr);

		camera.getInverse(cameraInv);

		cameraInv.getColumnMajor44(cameraInvMat);

		glMultMatrixf(cameraInvMat);

		CameraPos = camera.t;
		CameraDir = -lookAt;

		frameNum++;
	}
	else
	{
		//camera controls:
		if (keyDown['a'] || keyDown[20]) Eye -= N;
		if (keyDown['d'] || keyDown[22]) Eye += N;
		if (keyDown['w'] || keyDown[21]) Eye += Dir;
		if (keyDown['s'] || keyDown[23]) Eye -= Dir;
	#ifdef __PPCGEKKO__	
		gluLookAt(Eye.x, Eye.y, Eye.z, Eye.x + Dir.x, Eye.y + Dir.y, Eye.z + Dir.z, 0.0f, 1.0f, 0.0f);			
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	#else
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(Eye.x, Eye.y, Eye.z, Eye.x + Dir.x, Eye.y + Dir.y, Eye.z + Dir.z, 0.0f, 1.0f, 0.0f);
	#endif	
	}

	//render ground plane:
	glColor4f(0.4f, 0.4f, 0.4f, 1.0f); 
	if(!gDebugVisualization)
	{
	/*
		glPushMatrix();
		glScalef(10, 0, 10);
		glutSolidCube(600.0f);
		glPopMatrix();
	*/
	} else {

		static NxVec3* pPlaneVertexBuffer = NULL;
		
		if(pPlaneVertexBuffer == NULL)
		{
			pPlaneVertexBuffer = new NxVec3[200*4];
			for(int i=0;i<200;i++)
			{
				float v = (float)i*2-200.0f;
				pPlaneVertexBuffer[i*4+0].set(v,0.05f,-200.0f);
				pPlaneVertexBuffer[i*4+1].set(v,0.05f,200.0f);
				pPlaneVertexBuffer[i*4+2].set(-200.0f,0.05f,v);
				pPlaneVertexBuffer[i*4+3].set(200.0f,0.05f,v);
			}
		}
		glEnableClientState(GL_VERTEX_ARRAY);

		glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), 200*4, pPlaneVertexBuffer);

		glDrawArrays(GL_LINES, 0, 200*4);
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	glPushMatrix();

	RenderTerrain();

	if(gDebugVisualization) 
	{
		gDebugRenderer.renderData(*gScene->getDebugRenderable());
	} 
	else 
	{
		RenderAllActors();
	}

	glPopMatrix();
	NxAllVehicles::drawVehicles(gDebugVisualization);

	static CreationModes _oldCreationMode = MODE_NONE;
	static OrthographicDrawing orthoDraw;
	static float timeStamp = 0;

	orthoDraw.setOrthographicProjection((float)glutGet(GLUT_WINDOW_WIDTH), (float)glutGet(GLUT_WINDOW_HEIGHT));
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if(_oldCreationMode != creationMode)
	{
		_oldCreationMode = creationMode;
		timeStamp=getCurrentTime();
	}

	char info[] =	"See Console window for help.";
	orthoDraw.drawText(100, 30, info);

	renderHUD(orthoDraw);
	
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	orthoDraw.resetPerspectiveProjection();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

#ifdef __PPCGEKKO__	
	char buf[256];
    sprintf(buf,
	"2 flip car upright (use when stuck)\n"
	"+ switch cars\n"
	"HOME for debug render mode\n"
	"1 to toggle free flight camera\n"
	"a to hand brake.\n"
	"b to shoot boxes.\n");

    GLFontRenderer::setScreenResolution(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	GLFontRenderer::setColor(0.9f, 1.0f, 0.0f, 1.0f);
	GLFontRenderer::print(0.20, 0.9, 0.036, buf, false, 11, true);   
#endif

	glutSwapBuffers();
/*
    GLenum errCode = glGetError();
    if(errCode != GL_NO_ERROR){
        const unsigned char* errString = gluErrorString(errCode);
		printf("Error OpenGL: %s\n", errString);
    }
*/
}

void ReshapeCallback(int width, int height)
{
	glViewport(0, 0, width, height);
}

void IdleCallback()
{
	glutPostRedisplay();
}

//void callback_special(int key, int x, int y)
//{
//	//map specials to ASCII too:
//	unsigned char c = (unsigned char) key;
//	if(c >= 100) c -= 80;
//	keyDown[c] = true;
//	appKey(c, true);
//}
//
//void callback_specialUp(int key, int x, int y)//view control
//{
//	//map specials to ASCII too:
//	unsigned char c = (unsigned char) key;
//	if(c >= 100) c -= 80;
//	keyDown[c] = false;
//	appKey(c, false);
//}

void callback_key(unsigned char c, int x, int y)
{
	if(c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
	keyDown[c] = true;
	appKey(c, true);
}

void callback_keyUp(unsigned char c, int x, int y)
{
	if(c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
	keyDown[c] = false;
	appKey(c,false);
}

void ExitCallback()
{
	CloseCooking();

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
#ifdef __PPCGEKKO__
	printf("Arrows drive car\n");
	printf("2 flip car upright (use when stuck)\n");
	printf("+ switch cars\n");
	printf("HOME for debug render mode\n");
	printf("1 to toggle free flight camera\n");
	printf("a to hand brake.\n");
	printf("b to shoot boxes.\n");
#else
	printf("w,a,s,d - drive car\n");
	printf("f flip car upright (use when stuck)\n");
	printf("e - switch cars\n");
	printf("v for debug render mode\n");
	printf("p for pausing the simulation\n");
	printf("c to toggle free flight camera\n");
	printf("r to shoot boxes.\n");
#endif	

#if defined(_XBOX) || defined(__CELLOS_LV2__)
	glutRemapButtonExt(4, 'w', false); //Digital control to drive car
	glutRemapButtonExt(5, 's', false);
	glutRemapButtonExt(6, 'a', false);
	glutRemapButtonExt(7, 'd', false);
	glutRemapButtonExt(0, 'r', false); //A/X to shoot boxes
	glutRemapButtonExt(1, 'v', false); //B/O to debug render
	glutRemapButtonExt(2, 'c', false); //X/S to free flight
	glutRemapButtonExt(3, 'e', false); //Y/T to switch cars
	glutRemapButtonExt(9, 'f', false); //Right shoulder to flip car
#endif
#if defined(__PPCGEKKO__)
	glutRemapButtonExt(GLUT_KEY_UP, 'w'); 
	glutRemapButtonExt(GLUT_KEY_DOWN, 's'); 
	glutRemapButtonExt(GLUT_KEY_LEFT, 'a'); 
	glutRemapButtonExt(GLUT_KEY_RIGHT, 'd'); 
	glutRemapButtonExt('b', 'r'); 
	glutRemapButtonExt(GLUT_KEY_HOME, 'v'); 
	glutRemapButtonExt('1', 'c');
	glutRemapButtonExt('a', ' ');  
	glutRemapButtonExt('2', 'f');
	glutRemapButtonExt('+', 'e');
	glutRemapButtonExt('-', 'k');
#endif
	// Init Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	int mainHandle = glutCreateWindow("SampleRaycastCar");
	glutSetWindow(mainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(callback_key);
	glutKeyboardUpFunc(callback_keyUp);

	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0,0);
	
	
	// Setup default render states
	glClearColor(0.3f, 0.4f, 0.5f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
#if !defined(__PPCGEKKO__)
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

	// Initialize physics SDK and scene
	if (!InitNx()) return 0;

	NxReal xpos = 120;
	NxReal ypos = 2.0f;
	NxReal zdist = 5.0f;
	NxReal zpos = 10.f;

	createCarWithDesc(NxVec3(xpos,ypos,zpos + zdist * 0), true, false, false, false, false, gPhysicsSDK);//"1 - normal fw car\n"
	//createCarWithDesc(NxVec3(xpos,ypos,zpos + zdist * 1), true, false, false, false, true, gPhysicsSDK);//old style "1 - normal fw car\n"
	createCarWithDesc(NxVec3(xpos,ypos,zpos + zdist * 1), false, true, true, false, false, gPhysicsSDK);//corvette rw
	createCart		 (NxVec3(xpos,ypos,zpos + zdist * 2), false, true, false);//cart rw
	createCarWithDesc(NxVec3(xpos,ypos,zpos + zdist * 3), true, true, false, true, false, gPhysicsSDK);//old style box style monster truck, 4x4
	createTruckPuller(NxVec3(xpos, ypos, zpos + zdist * 4), 10, false);			//truck
	createFullTruck(NxVec3(xpos, ypos, zpos + zdist * 6), 32, false, false);	//full truck
	createFullTruck(NxVec3(xpos, ypos, zpos + zdist * 7), 32, true, false);		//full truck 4 axis
	createTruckWithTrailer1(NxVec3(xpos-50, ypos, zpos + zdist * 3),15, false);	//truck w trailer
	createFullTruckWithTrailer2(NxVec3(xpos-50, ypos, zpos + zdist * 5), 16, false);//full truck 4 axis w trailer

	NxAllVehicles::selectNext();
	NxAllVehicles::selectNext();	//select 1st car.

	atexit(ExitCallback);

	// Run
	glutMainLoop();
    
    return 0;
}
