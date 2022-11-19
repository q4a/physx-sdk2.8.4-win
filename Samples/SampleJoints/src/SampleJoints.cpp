// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Joints Sample
// Description: This sample program shows how to use joints in the NVIDIA PhysX SDK.
//
// ===============================================================================

#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#undef random
#include "NxPhysics.h"
#include "ErrorStream.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "UserAllocator.h"
#include "common.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

/* physics globals */

static NxPhysicsSDK*    gPhysicsSDK = NULL;
static NxScene*         gScene = NULL;
static NxVec3           gDefaultGravity(0.0f, -1.0f, 0.0f);
static ErrorStream		gErrorStream;
static UserAllocator*	gAllocator = NULL;
/* ------------ render variables ---------- */

static bool gPause = false;
static int gMainHandle;
static bool gShadows = true;
static NxVec3 Eye(50.0f, 50.0f, 50.0f);
static NxVec3 Dir(-0.6,-0.2,-0.7);
static NxVec3 N;
static int mx = 0;
static int my = 0;

/* ------------ demo specific variables ---------- */

#define PENDULUM_COUNT 5
#define BALL_COUNT 3

NxActor *hammer, *wheelPivot, *rodPivot;
NxActor *pendulum[PENDULUM_COUNT];
NxRevoluteJoint *driveJoint;
NxActor *ball[BALL_COUNT];


void RunPhysics();
void DemoSpecificRender();

void DrawSolidShape(NxActor *actor)
{
    NxShape *shape = actor->getShapes()[0];

    switch(shape->getType())
    {
    case NX_SHAPE_BOX:
        glutSolidCube((static_cast<NxBoxShape*>(shape))->getDimensions().x*2.0f);
        break;
    case NX_SHAPE_SPHERE:
        glutSolidSphere((static_cast<NxSphereShape*>(shape))->getRadius(),8,8);
        break;
    }
}

static void RenderCallback()
{
	float glmat[16];
	
    if(gScene && !gPause)    
        RunPhysics();

    // Clear buffers

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup camera

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
    gluLookAt(Eye.x, Eye.y, Eye.z, Eye.x + Dir.x, Eye.y + Dir.y, Eye.z + Dir.z, 0.0f, 1.0f, 0.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Render all the actors in the scene

    int nbActors = gScene->getNbActors();
    NxActor** actors = gScene->getActors();
    while(nbActors--)
    {
        NxActor* actor = *actors++;

        glPushMatrix();
		actor->getGlobalPose().getColumnMajor44(glmat);
		glMultMatrixf(glmat);
        DrawSolidShape(actor);
        glPopMatrix();

        // Handle shadows
        if(gShadows)
        {
            glPushMatrix();

            const static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };

            glMultMatrixf(ShadowMat);
            glMultMatrixf(glmat);

            glDisable(GL_LIGHTING);
            glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
            DrawSolidShape(actor);       
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glEnable(GL_LIGHTING);

            glPopMatrix();
        }
    }

    DemoSpecificRender();
#ifdef __PPCGEKKO__	
	char buf[256];
    sprintf(buf,
    "Press the keys + and - to change the speed of the motor.\n"
	"Use the arrow keys move the camera.\n");

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

static void KeyboardCallback(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:               exit(0); break;   // djs: nice!

    case 101: case '8':    Eye += Dir * 2.0f; return;
    case 103: case '2':    Eye -= Dir * 2.0f; return;
    case 100: case '4':    Eye -= N * 2.0f; return;
    case 102: case '6':    Eye += N * 2.0f; return;
    case 'p':              gPause = !gPause; return;
    case 'x':              gShadows = !gShadows; return;
    }

    void DemoSpecificKeys(unsigned char key, int x, int y);
    DemoSpecificKeys(key,x,y);
}

static void ArrowKeyCallback(int key, int x, int y)
{
    KeyboardCallback(key,x,y);
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

void InitGlut(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(512, 512);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    gMainHandle = glutCreateWindow("SampleJoints");
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
    float AmbientColor[]    = { 0.0f, 0.1f, 0.2f, 0.0f };        glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
    float DiffuseColor[]    = { 1.0f, 1.0f, 1.0f, 0.0f };        glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
    float SpecularColor[]    = { 0.0f, 0.0f, 0.0f, 0.0f };        glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
    float Position[]        = { 100.0f, 100.0f, 400.0f, 1.0f };    glLightfv(GL_LIGHT0, GL_POSITION, Position);
    glEnable(GL_LIGHT0);
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

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.025f);

    // Create a scene
    NxSceneDesc sceneDesc;
    sceneDesc.gravity               = gDefaultGravity;
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

static NxActor *CreateSphere(const NxVec3& pos, float radius, float density = 10)
{
    NxBodyDesc BodyDesc;
    BodyDesc.angularDamping   = 0.5f;

    NxSphereShapeDesc SphereDesc;
    SphereDesc.radius         = radius;

    NxActorDesc ActorDesc;
    ActorDesc.shapes.pushBack(&SphereDesc);
    ActorDesc.body            = &BodyDesc;
    ActorDesc.density         = density;
    ActorDesc.globalPose.t    = pos;
    return gScene->createActor(ActorDesc);
}

static NxActor *CreateCube(const NxVec3& pos, float radius, float density = 10)
{
    NxBodyDesc BodyDesc;

    NxBoxShapeDesc BoxDesc;
    BoxDesc.dimensions.set(radius,radius,radius);

    NxActorDesc ActorDesc;
    ActorDesc.shapes.pushBack(&BoxDesc);
    ActorDesc.body            = &BodyDesc;
    ActorDesc.density         = density;
    ActorDesc.globalPose.t    = pos;
    return gScene->createActor(ActorDesc);
}



void DemoSpecificKeys(unsigned char key, int x, int y)
{
    /* handle demo-specific keys */

    if(key == 'q' || key=='a')
    {
        NxMotorDesc motorDesc;
        driveJoint->getMotor(motorDesc);

        if(key=='q' && motorDesc.velTarget<0.6f)
            motorDesc.velTarget+=0.01f;

        if(key=='a' && motorDesc.velTarget>-0.6f)
            motorDesc.velTarget-=0.01f;

        driveJoint->setMotor(motorDesc);
    }
}

void DemoSpecificRender()
{
    /* 
        handle demo-specific rendering - just some lines for the 
        massless rods in the system 
    */

    glLineWidth(1.0);
	float vertices[PENDULUM_COUNT*2][3];
    for(int i=0;i<PENDULUM_COUNT;i++)
    {
        NxVec3 p = pendulum[i]->getGlobalPosition();
        vertices[i*2+0][0] = p.x; vertices[i*2+0][1] = p.y; vertices[i*2+0][2] = p.z;
        vertices[i*2+1][0] = 0.5; vertices[i*2+1][1] = 10; vertices[i*2+1][2] = (float)-40-i*20;
    }
    
    glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, PENDULUM_COUNT*2, vertices);	
	glDrawArrays(GL_LINES, 0, PENDULUM_COUNT*2);

    NxVec3 w, r, h;
    w = wheelPivot->getGlobalPosition();
    r = rodPivot->getGlobalPosition();
    h = hammer->getGlobalPosition();

    glLineWidth(3.0);
    float vertices2[6][3] = { {0,10,0}, {w.x,w.y,w.z}, {w.x,w.y,w.z}, {r.x,r.y,r.z}, {r.x,r.y,r.z}, {h.x,h.y,h.z}};
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, 6, vertices2);
	glDrawArrays(GL_LINES, 0, 6);
	glDisableClientState(GL_VERTEX_ARRAY);    
}

static void SetJointParameters(NxJointDesc &desc, NxActor *a0, NxActor *a1, NxVec3 &globalPos)
{
    desc.setToDefault();
    desc.actor[0] = a0;
    desc.actor[1] = a1;
    desc.setGlobalAnchor(globalPos);
}

void RunPhysics()
{
    static bool created = false;
    static int count = 0;

    NxVec3 p;

    p = wheelPivot->getGlobalPosition();

    if(p.y>15 && !created)
    {
        if(!ball[count])
        {
            ball[count] = CreateSphere(NxVec3(0,2,-35),1,20);

            // constrain the ball to move on a line 
			NxVec3 a(0,2,0);
            NxPrismaticJointDesc prismaticDesc;
            SetJointParameters(prismaticDesc,ball[count],0,a);
            prismaticDesc.setGlobalAxis(NxVec3(0,0,1));
            gScene->createJoint(prismaticDesc);
        }
        else
        {
            ball[count]->setGlobalPosition(NxVec3(0,2,-35));
			ball[count]->setLinearVelocity(NxVec3(0,0,0));
			ball[count]->setAngularVelocity(NxVec3(0,0,0));
        }

        count = (count+1)%BALL_COUNT;
        created = true;
    }

    else if(p.y<5)
        created = false;

    /* step collision & dynamics */

	gScene->simulate(1.0f/60.0f);	//Note: a real application would compute and pass the elapsed time here.
	gScene->flushStream();
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

}

static void CreateDemo()
{
    NxVec3 wheelPivotPos(0,2,0), rodPivotPos(0,2,-20), hammerPos(0,2,-30);

    NxRevoluteJointDesc revoluteDesc;

    wheelPivot = CreateCube(wheelPivotPos,1);

    /* a revolute joint with a motor */
	NxVec3 a(0,10,0);
    SetJointParameters(revoluteDesc,wheelPivot,0,a);
    NxVec3 b(1,0,0);
    revoluteDesc.setGlobalAxis(b);

    revoluteDesc.motor.maxForce = 1000;
    revoluteDesc.motor.velTarget = 0.15;
    revoluteDesc.flags |= NX_RJF_MOTOR_ENABLED;

    driveJoint = static_cast<NxRevoluteJoint*>(gScene->createJoint(revoluteDesc));

	// Setup the two distance joints

	NxDistanceJointDesc distanceDesc;
	

    rodPivot = CreateSphere(rodPivotPos,0.5f);
	revoluteDesc.setToDefault();

    SetJointParameters(distanceDesc,wheelPivot,rodPivot,rodPivotPos);
	distanceDesc.flags = NX_DJF_MAX_DISTANCE_ENABLED | NX_DJF_MIN_DISTANCE_ENABLED;
	distanceDesc.minDistance = distanceDesc.maxDistance = rodPivotPos.distance(wheelPivotPos);
	distanceDesc.localAnchor[0].zero();
	distanceDesc.localAnchor[1].zero();
    gScene->createJoint(distanceDesc);


    /* a hammer, with linear motion driven by the wheel */

    hammer = CreateCube(hammerPos,1);

    SetJointParameters(distanceDesc,rodPivot,hammer,hammerPos);
	distanceDesc.minDistance = distanceDesc.maxDistance = rodPivotPos.distance(hammerPos);
	distanceDesc.localAnchor[0].zero();
	distanceDesc.localAnchor[1].zero();
    gScene->createJoint(distanceDesc);

    NxPrismaticJointDesc prismaticDesc;

	SetJointParameters(prismaticDesc,rodPivot,0,rodPivotPos);
	NxVec3 f(0,0,1);
	prismaticDesc.setGlobalAxis(f);
	gScene->createJoint(prismaticDesc);

    SetJointParameters(prismaticDesc,hammer,0,hammerPos);
    
    prismaticDesc.setGlobalAxis(f);
    gScene->createJoint(prismaticDesc);

    /* create some very light pendulums, a little off center so they have
       slightly more interesting behaviour when they get hit.
    */

    for(int i=0;i<PENDULUM_COUNT;i++)
    {
		pendulum[i] = CreateCube(NxVec3(0.5,2,(float)-41-i*20),1.5,0.5);
		pendulum[i]->wakeUp(1e10);
        NxSphericalJointDesc sphericalDesc;
		NxVec3 g(0.5,10,(float)-41-i*20);
        SetJointParameters(sphericalDesc,pendulum[i],0,g);
        gScene->createJoint(sphericalDesc);
    }

    /* set the ball array to zero */

    for(int j=0;j<BALL_COUNT;j++)
        ball[j] = 0;


    /* prevent any of the jointed bodies from going to sleep*/

    wheelPivot->wakeUp(1e10);
    rodPivot->wakeUp(1e10);
    hammer->wakeUp(1e10);
}

int main(int argc, char** argv)
{
#ifdef __PPCGEKKO__
    printf("Press the keys + and - to change the speed of the motor.\n");
	printf("Use the arrow keys move the camera.\n");
#else
    printf("Press the keys q and a to change the speed of the motor.\n");
	printf("Press p to pause the simulation.\n");
	printf("Press x to toggle shadows.\n");
	printf("Use the arrow keys or 2, 4, 6 and 8 to move the camera, mouse to look around.\n");
#endif	
    InitGlut(argc, argv);
	
#ifdef __PPCGEKKO__
	glutRemapButtonExt(GLUT_KEY_UP, 	'8');
	glutRemapButtonExt(GLUT_KEY_DOWN,	'2');
	glutRemapButtonExt(GLUT_KEY_LEFT,	'4');
	glutRemapButtonExt(GLUT_KEY_RIGHT,	'6');
	glutRemapButtonExt('+', 'q');
	glutRemapButtonExt('-', 'a');
	glutRemapButtonExt('2', 'i');
#endif
	// Initialize physics scene and start the application main loop if scene was created

	if (InitNx())
	{
		CreateDemo();
		atexit(ExitCallback);
		glutMainLoop();
	}

    return 0;
}
