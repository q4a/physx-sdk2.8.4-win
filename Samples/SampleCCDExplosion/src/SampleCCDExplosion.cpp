// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: CCD Explosion
// Description: This sample program shows the effect of the dynamic vs. static 
//              CCD code. The example contains of a box stack inside of a glass
//              box. Let the scene explode and watch the difference between when
//              CCD is used and when it's not.
//              CCD is useful when small objects move fast, in order to stop objects 
//              from tunneling through each other.
//
// Originally written by: Daniel Germann (Dec 2004)
//
// ===============================================================================

#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "NxPhysics.h"
#include "cooking.h"
#include "Stream.h"
#include "Timing.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "UserAllocator.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

#define DEBUG_RENDER
//#define AUTOMATIC
//#define SANITY_CHECK

#ifdef DEBUG_RENDER
#include "DebugRenderer.h"
#endif


static NxPhysicsSDK *	gPhysicsSDK = NULL;
static NxScene *		gScene = NULL;
static NxVec3			gDefaultGravity(0.0f, -9.81f, 0.0f);
static UserAllocator*	gAllocator = NULL;
#ifdef DEBUG_RENDER
static DebugRenderer	gDebugRenderer;
#endif

static int gMainHandle;
static NxVec3 Eye(20.0f, 30.0f, 20.0f);
static NxVec3 Dir(-1.0f, -1.0f, -1.0f);
static NxVec3 N;
static int mx = 0;
static int my = 0;
static NxReal ccdSetting = 1.0f;
static NxReal visSetting = 1.0f;

static NxReal gTime;
static NxReal gLastTime;
static bool gPause = false;
static bool gRequestExit = false;
bool autopause = false;//true;

static const GLfloat WHITE[]	= { 1.0f, 1.0f, 1.0f, 1.0f };
static const GLfloat GRAY[]		= { 0.5f, 0.5f, 0.5f, 1.0f };
static const GLfloat BLACK[]	= { 0.0f, 0.0f, 0.0f, 1.0f };
static const GLfloat RED[]		= { 1.0f, 0.0f, 0.0f, 1.0f };
static const GLfloat GREEN[]	= { 0.0f, 1.0f, 0.0f, 1.0f };
static const GLfloat BLUE[]		= { 0.0f, 0.0f, 1.0f, 1.0f };
static const GLfloat GLASS[]	= { 0.0f, 0.8f, 0.8f, 0.5f };

static const NxU32 nbCuboids = 24;//24;
static NxActor * gCuboids[nbCuboids];

static NxActor * gGlassBox = NULL;

	NxVec3 dim1(0.5f, 0.5f , 2.0f);


class Explosion
{
public:
	Explosion() { nbActors = 0; actors = NULL; timeToGo = 0.0f; }
	~Explosion(){};

	void start();
	void run(NxReal dt);

	NxU32 nbActors;
	NxActor ** actors;
	NxVec3 pos;
	NxReal force;
	NxReal radius;
	NxReal duration;

private:
	NxReal timeToGo;
}
gExplosion;

void Explosion::start()
{
	timeToGo = duration;
}

void Explosion::run(NxReal dt)
{
	timeToGo -= dt;
	if (timeToGo > 0.0f)
	{
		for (NxU32 i = 0; i < nbActors; i++)
		{
		if (!actors[i])
			continue;

			NxVec3 dir = actors[i]->getGlobalPosition() - pos;
			NxReal scaling = 1.0f - dir.magnitude() / radius;
			if (scaling < 0.0f) scaling = 0.0f;
			dir.setMagnitude(scaling);
			actors[i]->addForce(force * dir, NX_IMPULSE);
		}
	}
#ifdef AUTOMATIC
	else if (timeToGo < -2.0f)
		start();
#endif

}
	
#ifdef SANITY_CHECK
void sanityCheck()
	{
	NxReal tol = 0.46f;

	static NxU32 frameNo = 0;
	const NxU32 stopFrameNo = 0xffffffff;
	frameNo++;

	if (stopFrameNo == frameNo)
		{
		gPause = true;
		autopause = true;
		}

	NxBounds3 worldBounds;
	gGlassBox->getShapes()[0]->getWorldBounds(worldBounds);
		for (NxU32 i = 0; i < nbCuboids; i++)
		{
	
		if (!gCuboids[i])
			continue;

		//center check only
#if 0		
			NxVec3 pos = gCuboids[i]->getGlobalPosition();
			if (!worldBounds.contain(pos))
				printf("cuboid %d outside in frame %d!\n", i, frameNo);
#else
		//more exact chec
		NxMat34 pose = gCuboids[i]->getGlobalPose();
		for (NxReal vx = -1; vx <= 1; vx+=2)
			for (NxReal vy = -1; vy <= 1; vy+=2)
				for (NxReal vz = -1; vz <= 1; vz+=2)
					{
					NxVec3 p(vx*dim1.x*tol,vy*dim1.y*tol,vz*dim1.z*tol);
					p = pose * p;

					if (!worldBounds.contain(p))
						{
						printf("cuboid %d outside in frame %d!\n", i, frameNo);

						//delete all other actors for clarity
						for (NxU32 w = 0; w < nbCuboids; w++)
							{
							if (i != w)
								{
								gScene->releaseActor(*gCuboids[w]);
								gCuboids[w] = NULL;
								}
							}


						gPause = true;
						autopause = true;
						}

					}
#endif
		}
	}
#endif

NxTriangleMesh * staticTriangleMesh;
NxConvexMesh * convexMesh;
NxCCDSkeleton * ccds;

void CreateMeshes()
	{

	NxVec3 dim(10,10,10);


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
		points[0].set( dim.x, -dim.y, -dim.z);
		points[1].set( dim.x, -dim.y,  dim.z);
		points[2].set( dim.x,  dim.y, -dim.z);
		points[3].set( dim.x,  dim.y,  dim.z);

		points[4].set(-dim.x, -dim.y, -dim.z);
		points[5].set(-dim.x, -dim.y,  dim.z);
		points[6].set(-dim.x,  dim.y, -dim.z);
		points[7].set(-dim.x,  dim.y,  dim.z);

		bool status = InitCooking(gAllocator);
		if (!status) {
			printf("\nError: Unable to initialize the cooking library. Please make sure that you have correctly installed the latest version of the NVIDIA PhysX SDK.\n\n");
			exit(1);
		}

		NxTriangleMeshDesc		meshDesc;
		meshDesc.numVertices = 8;
		meshDesc.numTriangles = 12;
		meshDesc.pointStrideBytes = sizeof(NxVec3);
		meshDesc.triangleStrideBytes = 3 * sizeof(NxU32);
		meshDesc.points = points;
		meshDesc.triangles = triangles;
  		MemoryWriteBuffer buf;
  		status = CookTriangleMesh(meshDesc, buf);
		if (!status) {
			printf("Unable to cook a triangle mesh.");
			exit(1);
		}
  		MemoryReadBuffer readBuffer(buf.data);
  		staticTriangleMesh = gPhysicsSDK->createTriangleMesh(readBuffer);
		//
		// Please note about the created Triangle Mesh, user needs to release it when no one uses it to save memory. It can be detected
		// by API "staticTriangleMesh->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseTriangleMesh(*staticTriangleMesh);"
		//

		//dynamic convexes
		dim = dim1;
		points[0].set( dim.x, -dim.y, -dim.z);
		points[1].set( dim.x, -dim.y,  dim.z);
		points[2].set( dim.x,  dim.y, -dim.z);
		points[3].set( dim.x,  dim.y,  dim.z);

		points[4].set(-dim.x, -dim.y, -dim.z);
		points[5].set(-dim.x, -dim.y,  dim.z);
		points[6].set(-dim.x,  dim.y, -dim.z);
		points[7].set(-dim.x,  dim.y,  dim.z);

		NxConvexMeshDesc convexDesc;
		convexDesc.numVertices = 8;
		convexDesc.numTriangles = 0;
		convexDesc.pointStrideBytes = sizeof(NxVec3);
		convexDesc.points = points;
		convexDesc.flags |= NX_CF_COMPUTE_CONVEX;

		buf.clear();
		status = CookConvexMesh(convexDesc, buf);
		if (!status) {
			printf("Unable to cook a convex mesh.");
			exit(1);
		}
		{
			MemoryReadBuffer readBuffer(buf.data);
			convexMesh = gPhysicsSDK->createConvexMesh(readBuffer);
		}

		NxSimpleTriangleMesh stm;
		stm.numVertices = 8;
		stm.numTriangles = 6*2;
		stm.pointStrideBytes = sizeof(NxVec3);
		stm.triangleStrideBytes = sizeof(NxU32)*3;

		for (NxU32 i = 0; i < 8; i++)
			points[i].arrayMultiply(points[i], NxVec3(0.8f, 0.8f, 0.9f));

		stm.points = points;
		stm.triangles = triangles;
		stm.flags |= NX_MF_FLIPNORMALS;
		ccds = gPhysicsSDK->createCCDSkeleton(stm);

		CloseCooking();


	}

static NxActor * CreateCuboid(const NxVec3 & pos, const GLfloat * color = WHITE, NX_BOOL isStatic = NX_FALSE)
{
	if (!gScene) return NULL;


	NxBodyDesc BodyDesc;

	NxActorDesc actorDesc;
	if (!isStatic) actorDesc.body = &BodyDesc;
	actorDesc.density = 100.0f;
	actorDesc.globalPose.t = pos;

	NxTriangleMeshShapeDesc meshShapeDesc;
	NxConvexShapeDesc convexShapeDesc;

	if (isStatic)
	{
		meshShapeDesc.meshData = staticTriangleMesh;
		actorDesc.shapes.pushBack(&meshShapeDesc);
	}
	else
	{
		convexShapeDesc.meshData = convexMesh;
		convexShapeDesc.ccdSkeleton = ccds;
		actorDesc.shapes.pushBack(&convexShapeDesc);
	}

	actorDesc.userData = (void*) color;
	return gScene->createActor(actorDesc);
}

static void CreateStack(const NxVec3 & pos, const GLfloat * color = WHITE)
{
	NX_BOOL orientation = NX_TRUE;
	NxU32 nbLayers = nbCuboids / 2;
	NxU32 i = 0;

	NxQuat q;
	q.fromAngleAxisFast(NxPi*0.5f, NxVec3(0,1,0));

	NxVec3 height = pos;
	for (NxU32 layer = 0; layer < nbLayers; layer++)
	{
		if (orientation)
		{
			NxVec3 p(1.5f, 0.0f, 0.0f);
			gCuboids[i++] = CreateCuboid(height + p, color);
			gCuboids[i++] = CreateCuboid(height - p, color);
		}
		else
		{
			NxVec3 p(0.0f, 0.0f, 1.5f);
			gCuboids[i++] = CreateCuboid(height + p, color);
			gCuboids[i-1]->setGlobalOrientationQuat(q);
			gCuboids[i++] = CreateCuboid(height - p, color);
			gCuboids[i-1]->setGlobalOrientationQuat(q);
		}
		orientation = !orientation;
		height.y += dim1.y * 2.1f;
	}
}

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
	if (gPhysicsSDK->getFoundationSDK().getRemoteDebugger() && !gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
#endif

	gPhysicsSDK->setParameter(NX_CONTINUOUS_CD, ccdSetting);
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, visSetting);
	//Enable the next line to visualize the collision shapes
	//gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SKELETONS, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_CCD, 1);
	gPhysicsSDK->setParameter(NX_CCD_EPSILON, 0.01f);
	

	CreateMeshes();

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
	PlaneDesc.d = 0.06f;
	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&PlaneDesc);
	gScene->createActor(ActorDesc);

	// create other actors
	CreateStack(NxVec3(0.0f, 5.6f, 0.0f));
	gGlassBox = CreateCuboid(NxVec3(0, 15, 0), GLASS, NX_TRUE);

	gExplosion.actors = gCuboids;
	gExplosion.nbActors = nbCuboids;
	gExplosion.force = 100000.0f;
	gExplosion.pos.set(0.5f, 10.0f, 1.0f);
	gExplosion.radius = 10000.0f;
	gExplosion.duration = 0.1f;

	//gLastTime = getTime()*0.001;
#if defined(_XBOX) || defined(__CELLOS_LV2__)
	glutRemapButtonExt(0, 'e', false); //A/X to explode
	glutRemapButtonExt(1, 'c', false); //B/O to disable CCD
	glutRemapButtonExt(9, 'r', false); //Right shoulder to reset scene
#endif

	return true;
}

static void drawCuboid(const NxActor * cuboid, NX_BOOL drawBothSides = NX_FALSE)
{
	NxU32 numTrigs;
	const NxTriangle32* trigs;
	const NxPoint* verts;
	const NxPoint* normals;

	if(cuboid->getShapes()[0]->getType()== NX_SHAPE_MESH)
		{
		NxTriangleMesh & tm = static_cast<NxTriangleMeshShape*>(cuboid->getShapes()[0])->getTriangleMesh();

		numTrigs = tm.getCount(0, NX_ARRAY_TRIANGLES);
		trigs = (const NxTriangle32*)tm.getBase(0, NX_ARRAY_TRIANGLES);
		verts = (const NxPoint*)tm.getBase(0, NX_ARRAY_VERTICES);
		normals = (const NxPoint*)tm.getBase(0, NX_ARRAY_NORMALS);
		}
	else if(cuboid->getShapes()[0]->getType() == NX_SHAPE_CONVEX)
		{
		NxConvexMesh & tm = static_cast<NxConvexShape*>(cuboid->getShapes()[0])->getConvexMesh();

		numTrigs = tm.getCount(0, NX_ARRAY_TRIANGLES);
		trigs = (const NxTriangle32*)tm.getBase(0, NX_ARRAY_TRIANGLES);
		verts = (const NxPoint*)tm.getBase(0, NX_ARRAY_VERTICES);
		normals = (const NxPoint*)tm.getBase(0, NX_ARRAY_NORMALS);
		}
	else return;

    float* pVertList = new float[numTrigs*3*3];
    float* pNormList = new float[numTrigs*3*3];

    int vertIndex = 0;
    int normIndex = 0;
    for(unsigned int i=0;i<numTrigs;i++)
    {
      for(int j=0;j<3;j++)
      {
        pVertList[vertIndex++] = verts[trigs[i].v[j]].x;
        pVertList[vertIndex++] = verts[trigs[i].v[j]].y;
        pVertList[vertIndex++] = verts[trigs[i].v[j]].z;

        pNormList[normIndex++] = normals[trigs[i].v[j]].x;
        pNormList[normIndex++] = normals[trigs[i].v[j]].y;
        pNormList[normIndex++] = normals[trigs[i].v[j]].z;
      }
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
#ifdef __PPCGEKKO__
    glVertexPointer(3,GL_FLOAT, 0, numTrigs*3,pVertList);
    glNormalPointer(GL_FLOAT, 0, numTrigs*3,pNormList);
#else    
    glVertexPointer(3,GL_FLOAT, 0, pVertList);
    glNormalPointer(GL_FLOAT, 0, pNormList);
#endif    
    glDrawArrays(GL_TRIANGLES, 0, numTrigs*3);

	if(drawBothSides)
	{	
		glCullFace(GL_FRONT);
#ifdef __PPCGEKKO__
    	glVertexPointer(3,GL_FLOAT, 0, numTrigs*3,pVertList);
    	glNormalPointer(GL_FLOAT, 0, numTrigs*3,pNormList);
#else    
	    glVertexPointer(3,GL_FLOAT, 0, pVertList);
	    glNormalPointer(GL_FLOAT, 0, pNormList);
#endif    
	    glDrawArrays(GL_TRIANGLES, 0, numTrigs*3);
		glCullFace(GL_BACK);
	}

	glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

	delete[] pVertList;
	delete[] pNormList;
}

static void reset()
{
	if(gPhysicsSDK != NULL)
	{
		if(gScene != NULL) gPhysicsSDK->releaseScene(*gScene);
		gScene = NULL;
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
	}
	if (!InitNx()) exit(0);
}

static void drawCuboids()
{
	NxU32 i;
	float glmat[16];
	const static float ShadowMat[] = { 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };

	for (i = 0; i < nbCuboids; i++)
	{
		NxActor * actor = gCuboids[i];
		if (!actor)
			continue;
		actor->getGlobalPose().getColumnMajor44(glmat);

		// Handle shadows
		glPushMatrix();

		glMultMatrixf(ShadowMat);
		glMultMatrixf(glmat);

		glDisable(GL_LIGHTING);
		glColor4fv(BLACK);
		if (actor->getShapes()[0]->getType() == NX_SHAPE_MESH || actor->getShapes()[0]->getType() == NX_SHAPE_CONVEX)
			drawCuboid(actor);
		glEnable(GL_LIGHTING);

		glPopMatrix();

		//
		glPushMatrix();
		glMultMatrixf(glmat);
	
		if (actor->getShapes()[0]->getType() == NX_SHAPE_MESH || actor->getShapes()[0]->getType() == NX_SHAPE_CONVEX)
		{
			glColor4fv((GLfloat*)actor->userData);
			drawCuboid(actor);
		}

		glPopMatrix();
	}

		// Handle shadows
		if (gGlassBox)
			{
			gGlassBox->getGlobalPose().getColumnMajor44(glmat);

			glPushMatrix();
			glMultMatrixf(ShadowMat);
			glMultMatrixf(glmat);

			glDisable(GL_LIGHTING);
			glColor4fv(BLACK);
			drawCuboid(gGlassBox, NX_TRUE);
			glEnable(GL_LIGHTING);

			glPopMatrix();

			// draw glass box
			glEnable(GL_BLEND);

			glPushMatrix();
			glMultMatrixf(glmat);

			glColor4fv((GLfloat*)gGlassBox->userData);
			drawCuboid(gGlassBox, NX_TRUE);

			glDisable(GL_BLEND);

			glPopMatrix();
			}
}


static void KeyboardCallback(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:	gRequestExit = true; break;
	case 'p':			gPause = !gPause; break;
	case 'w': case '8':	Eye += Dir * 2.0f; break;
	case 's': case '2':	Eye -= Dir * 2.0f; break;
	case 'a': case '4':	Eye -= N * 2.0f; break;
	case 'd': case '6':	Eye += N * 2.0f; break;
	case 'e': gExplosion.start(); break;
	case 'r': reset(); break;
	case 'c': 
	{
		ccdSetting = 1.0f - ccdSetting;
		gPhysicsSDK->setParameter(NX_CONTINUOUS_CD, ccdSetting); 
		printf("CCD %s\n", (ccdSetting > 0.0f)?"enabled":"disabled");
		break;
	}
	case 'v': visSetting = 1.0f - visSetting;  gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, visSetting); break;
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

static void RenderCallback()
{
	NxF32 elapsedTime = getElapsedTime();

	if(gRequestExit) exit(0);

	if(gScene && !gPause)
	{
	if (elapsedTime != 0.0f)
		{
		elapsedTime = 0.02f;
			gExplosion.run(elapsedTime);
			gScene->simulate(elapsedTime);
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	gluLookAt(Eye.x, Eye.y, Eye.z, Eye.x + Dir.x, Eye.y + Dir.y, Eye.z + Dir.z, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(gScene && !gPause)
	{
		gScene->flushStream();
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
#ifdef SANITY_CHECK
	sanityCheck();
#endif
	}

#ifdef DEBUG_RENDER
	gDebugRenderer.renderData(*gScene->getDebugRenderable());
#endif
	if (gScene) drawCuboids();
	
#ifdef __PPCGEKKO__	
	char buf[4096];
    sprintf(buf,
    "Demonstration of continuous collision detection\n\n"
	"A box shaped triangleMesh contains box shaped convexMeshes, \nequipped with CCDSkeletons.\n"
	"Keys:\nb - apply very large forces to explode stack.\n1 - toggle CCD\n2 - toggle visualization\na - reset scene.\n"
	"Use arrow keys to move the camera.\n"
	"CCD %s\n",
    (ccdSetting > 0.0f)?"enabled":"disabled");

    GLFontRenderer::setScreenResolution(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	GLFontRenderer::setColor(0.9f, 1.0f, 0.0f, 1.0f);
	GLFontRenderer::print(0.01, 0.9, 0.036, buf, false, 11, true);   
#endif
	
	glutSwapBuffers();

	if (autopause)
		gPause = true;
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

int main(int argc, char ** argv)
{
	//print instructions:

	printf("Demonstration of continuous collision detection\n\n");
	printf("A box shaped triangleMesh contains box shaped convexMeshes, equipped with CCDSkeletons.\n");
#ifdef __PPCGEKKO__	
	printf("Keys:\nb - apply very large forces to explode stack.\n1 - toggle CCD\n2 - toggle visualization\na - reset scene.\n");
	printf("Use arrow keys to move the camera.\n");
#else
	printf("Keys:\ne - apply very large forces to explode stack.\nc - toggle CCD\nv - toggle visualization\nr - reset scene.\n");
	printf("Press p to pause the simulation.\n");
	printf("Use a, d, w and s or 2, 4, 6 and 8 to move the camera, mouse to look around.\n");
#endif

	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("SampleCCDExplosion");
	glutSetWindow(gMainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0, 0);

	// Setup default render states
	glClearColor(0.1f, 0.2f, 0.3f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	//glEnable(GL_ALPHA_TEST);
	glEnable(GL_CULL_FACE);
	//glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glEnable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);


	// Setup lighting
	glEnable(GL_LIGHTING);
	float AmbientColor[]	= { 0.2f, 0.2f, 0.2f, 1.0f };		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	float DiffuseColor[]	= { 1.0f, 1.0f, 1.0f, 1.0f };		glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[]	= { 0.0f, 0.0f, 0.0f, 1.0f };		glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[]		= { 100.0f, 100.0f, 400.0f, 1.0f };		glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

#ifdef __PPCGEKKO__	
	glutRemapButtonExt(GLUT_KEY_UP,'w');
	glutRemapButtonExt(GLUT_KEY_DOWN,'s');
	glutRemapButtonExt(GLUT_KEY_LEFT,'a');
	glutRemapButtonExt(GLUT_KEY_RIGHT,'d');
	glutRemapButtonExt('b','e');
	glutRemapButtonExt('a','r');
	glutRemapButtonExt('1','c');
	glutRemapButtonExt('2','v');
#endif	
	
	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitCallback);
		glutMainLoop();
	}
	
	return 0;
}
