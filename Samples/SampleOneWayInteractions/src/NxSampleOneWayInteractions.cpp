// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: One Way Interactions Sample
// Description: This sample program shows how to utilize Dominance Group settings
//              to have some constraints (contacts, joints) only transfer impulses
//              in one direction.
//
// There is a hoverboard, which is controlled by forces applied in response to  
// user input.
// There is also a character riding on the hoverboard, but we use one way
// interactions between the board and the character to make sure the character's
// wobbling motion doesn't disturb the hoverboard's motion.
//
// The rider's arms are in yet another dominance group so that the arms'
// wagging doesn't negatively influence the motion of the rest of the body.
//
// The debris on the playing field are also set up via dominance groups to 
// not directly influence the hoverboard's motion by colliding and blocking it.
// They still influence the motion indirectly as they are hit by the sensing rays
// cast by the hover jet controller.
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
#include "Timing.h"
#include "UserAllocator.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

// Physics
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;
static PerfRenderer    gPerfRenderer;
static UserAllocator*	gAllocator = NULL;

// Rendering
static NxVec3	gEye(0.0f, 5.0f, 5.0f);
static NxVec3	gDir(0.0f,-0.2f,-0.7f);
static NxVec3	gViewY;
static int		gMouseX = 0;
static int		gMouseY = 0;

static float	gJetPower = 300.0f;
static bool		gJumpPressed = false, gBoostFW = false, gBoostRW = false;
static bool		boardRiderOneWay = true, riderArmsOneWay = true, debrisOneWay = true;

// An example game object that needs to synchronize its internal transform
// with the NxActor it contains.
class GameObject
	{
	public:

		GameObject() : m_physxObject(NULL)
			{
			m_boxSize.zero();
			m_transform.id();
			}
		GameObject(NxActor *physxObject, NxVec3 boxSize) : m_physxObject(physxObject), m_boxSize(boxSize)
			{
			m_transform = physxObject->getGlobalPose();
			if (m_physxObject)
				m_physxObject->userData = (void *) this;
			}

		NxMat34 getTransform(void) const               { return m_transform; }
		void    setTransform(const NxMat34 &transform) { m_transform = transform; }
		const NxVec3   & getBoxSize(void) const                 { return m_boxSize; }
		void setPhysicsObject(NxActor * a, NxVec3 b) 
			{ 
			m_physxObject = a; 
			m_boxSize = b; 
			if (a) 
				a->userData = (void *) this; 
			}
		NxActor * getActor() { return m_physxObject; }

		virtual void think(NxReal dt) {}

	private:

		NxActor *m_physxObject;
		NxMat34  m_transform;
		NxVec3   m_boxSize;
	};

class GameManager
	{
	public:
		static void addObject(GameObject *gameObject)
			{
			if(gameObject)
				{
				m_gameObjectList.pushBack(gameObject);		
				}
			};


		static void addActor(NxActor *actor, NxVec3 boxSize)
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



class HoverBoard : public GameObject
	{
	public:
		HoverBoard(const NxVec3& pos)
			{
			//create hoverboard actor with box shape and two ray-wheels
			NxBodyDesc bodyDesc;
			bodyDesc.angularDamping	= 0.05f;
			bodyDesc.linearDamping = 1.0f;
			bodyDesc.mass = 100.0f;
			NxBoxShapeDesc boxDesc;
			boxDesc.dimensions = NxVec3(0.5f, 0.025f, 0.1f);
			boxDesc.group = 2;									//group 2 so that our own raycasts don't hit us!
			NxActorDesc actorDesc;
			actorDesc.shapes.pushBack(&boxDesc);
			actorDesc.body			= &bodyDesc;
			actorDesc.globalPose.t  = pos;
			NxActor *actor = gScene->createActor(actorDesc);
			actor->setDominanceGroup(1);

			setPhysicsObject(actor, boxDesc.dimensions);
			// add the actor to the game manager
			GameManager::addObject(this);
			}

		virtual void think(NxReal dt)
			{
			//do two raycasts to and apply forces to do the levitation bit
			NxActor * board = getActor();

			NxVec3 jetLocalPositions[4];
			jetLocalPositions[0].set(-0.45f, 0.0f, 0.0f);
			jetLocalPositions[1].set( 0.45f, 0.0f, 0.0f);

			NxMat34 boardPose = board->getGlobalPose();
			NxVec3 negY(0.0f, -1.0f, 0.0f);
			NxVec3 Z(0.0f, 0.0f, 1.0f);
			NxVec3 X(1.0f, 0.0f, 0.0f);
			NxVec3 down = boardPose.M * negY;
			down.normalize();
			NxVec3 right = boardPose.M * Z;
			NxVec3 forward = boardPose.M * X;
			NxReal jumpMultiplyer = 1.0f;
			if (gJumpPressed)
				{
				jumpMultiplyer = 10.0f;
				}
			for (unsigned i = 0; i < 2; i ++)
				{
				NxVec3 jetWorldPos = boardPose * jetLocalPositions[i];
				NxRaycastHit hit;
				//note: 1 means to only hit group 0 objects (1<<0).
				NxShape * s = gScene->raycastClosestShape(NxRay(jetWorldPos, down), NX_ALL_SHAPES, hit, 1, 1.0f, NX_RAYCAST_SHAPE|NX_RAYCAST_DISTANCE); 
				if (s)
					{
					NxReal force = -gJetPower / ((hit.distance > 0.1f) ? hit.distance : 0.1f);	//quadratic falloff

					force *= jumpMultiplyer;
					board->addForceAtLocalPos(negY * force, jetLocalPositions[i]);	//jets need to orient downwards, not normal to board, otherwise its unstable
					}
				}
			//add stabilizing controller so it doesn't roll over!
			NxReal angDamping = board->getAngularVelocity().magnitude();	//this is damping coeff

			NxReal tiltAngle = NxMath::acos(down | negY);
			NxReal sign = right | negY;
			if (sign > 0.0f) 
				tiltAngle = -tiltAngle;
			board->addLocalTorque(NxVec3(tiltAngle * 300.0f, 0.0f, 0.0f));

			//add stabilizing controller so it doesn't turn either!
			NxReal turnAngle = NxMath::acos(forward | X);
			NxReal tsign = right | X;
			if (tsign > 0.0f) 
				turnAngle = -turnAngle;
			board->addLocalTorque(NxVec3(0.0f, turnAngle * 2000.0f, 0.0f));

			//add translational stabilization so we don't drift out of z = 0 plane.
			board->addLocalForce(NxVec3(0.0f, 0.0f, -boardPose.t.z * 100.0f));


			if (gBoostRW)
				{
				board->addLocalForce(NxVec3(1000.0f, 0.0f, 0.0f));
				}
			if (gBoostFW)
				{
				board->addLocalForce(NxVec3(-1000.0f, 0.0f, 0.0f));
				}

			//make camera follow:
			gEye.x = boardPose.t.x;
			gEye.z = 4.0f;
			gEye.z += (boardPose.t.y > 3.0f) ? boardPose.t.y * 1.5f : 4.5f;
			}
	};

static void CreateDebris()
	{
	NxReal start = -5.0f;
	for (unsigned int x = 0; x < 500; x++)
		{
		float gapWidth = NxMath::rand(0.0f, 1.3f);
		NxBoxShapeDesc boxDesc;
		boxDesc.dimensions.set(NxMath::rand(0.1f, 0.5f), NxMath::rand(0.1f, 0.5f), NxMath::rand(0.1f, 0.5f));

		NxActorDesc actorDesc;
		NxBodyDesc bodyDesc;
		actorDesc.body = &bodyDesc;
		actorDesc.density = 2000.0f;
		actorDesc.shapes.pushBack(&boxDesc);
		actorDesc.globalPose.t.set(start, 3, NxMath::rand(-0.2f, -0.2f));
		NxActor *actor = gScene->createActor(actorDesc);
		actor->setDominanceGroup(4);
		GameManager::addActor(actor, boxDesc.dimensions);

		start -= gapWidth;
		}

	}

static void CreatePlatforms()
	{
	NxReal start = -1.0f;
	for (unsigned int x = 0; x < 30; x++)
		{
		float platWidth = NxMath::rand(4.0f, 10.0f);
		float gapWidth = NxMath::rand(0.1f, 4.0f);
		NxBoxShapeDesc boxDesc;
		boxDesc.dimensions.set(platWidth * 0.5f, 0.5f, 1.0f);

		NxActorDesc actorDesc;
		actorDesc.shapes.pushBack(&boxDesc);
		actorDesc.globalPose.t.set(start + platWidth * 0.5f, 0.5f, 0.0f);
		NxActor *actor = gScene->createActor(actorDesc);
		GameManager::addActor(actor, boxDesc.dimensions);

		start -= platWidth + gapWidth;
		}
	}

static NxActor * CreateCube(const NxVec3& pos, const NxVec3& size, const NxVec3* initialVelocity=NULL)
	{
	if(gScene == NULL) return NULL;	

	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
	if(initialVelocity) bodyDesc.linearVelocity = *initialVelocity;

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = size;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 3000.0f;
	actorDesc.globalPose.t  = pos;
	NxActor *actor = gScene->createActor(actorDesc);

	// add the actor to the game manager
	GameManager::addActor(actor, size);

	return actor;
	}


static void SetupDominanceGroups()
	{

	/* Always set up group interactions before creating actors, and try not to change them:
		Dominance groups:
		Hoverboard = 1
		Rider = 2
		RiderArms = 3
		Debris = 4

		In a real game, if you must change something, its much faster to change actors' group assignment
		than it is to change the dominance group pair interaction matrix.
	*/

		NxConstraintDominance boardRiderDominance(boardRiderOneWay ? 0.0f : 1.0f, 1.0f);
		gScene->setDominanceGroupPair(1, 2, boardRiderDominance);	//board - rider
		NxConstraintDominance riderArmsDominance(riderArmsOneWay ? 0.0f	: 1.0f, 1.0f);
		gScene->setDominanceGroupPair(2, 3, riderArmsDominance);	//rider - arms

		NxConstraintDominance debrisDominance(debrisOneWay ? 0.0f : 1.0f, 1.0f);
		gScene->setDominanceGroupPair(1, 4, debrisDominance);	//board - debris
		gScene->setDominanceGroupPair(2, 4, debrisDominance);	//rider - debris
		gScene->setDominanceGroupPair(3, 4, debrisDominance);	//riderArms - debris
	}

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
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01f);

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

	SetupDominanceGroups();

	// Create ground plane
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	gScene->createActor(actorDesc);

	CreatePlatforms();
	CreateDebris();
	NxVec3 boardPos(0.0f, 2.0f, 0.0f);
	HoverBoard * hb = new HoverBoard(boardPos);

	//create rider
	NxActor * frontLowerLeg = CreateCube(NxVec3(-0.3f,0.3f,0.0f) +boardPos, NxVec3(0.1f, 0.2f, 0.06f));
	NxActor *  backLowerLeg = CreateCube(NxVec3( 0.3f,0.3f,0.0f) +boardPos, NxVec3(0.1f, 0.2f, 0.06f));

	NxActor * frontUpperLeg = CreateCube(NxVec3(-0.3f,0.5f,0.0f) +boardPos, NxVec3(0.1f, 0.12f, 0.03f));
	NxActor *  backUpperLeg = CreateCube(NxVec3( 0.3f,0.5f,0.0f) +boardPos, NxVec3(0.1f, 0.12f, 0.03f));

	NxActor *		   body = CreateCube(NxVec3( 0.0f,0.9f,0.0f) +boardPos, NxVec3(0.1f, 0.3f, 0.02f));

	NxActor * frontArm = CreateCube(NxVec3(-0.4f,1.0f,0.0f) +boardPos, NxVec3(0.3f, 0.01f, 0.1f));
	NxActor *  backArm = CreateCube(NxVec3( 0.4f,1.0f,0.0f) +boardPos, NxVec3(0.3f, 0.01f, 0.1f));

	/////
	NxReal tmass = 0;

	tmass += frontLowerLeg->getMass();
	tmass += backLowerLeg->getMass();
	tmass += frontUpperLeg->getMass();
	tmass += backUpperLeg->getMass();

	tmass += body->getMass();

	tmass += frontArm->getMass();
	tmass += backArm->getMass();

	frontLowerLeg->setDominanceGroup(2);
	backLowerLeg->setDominanceGroup(2);
	frontUpperLeg->setDominanceGroup(2);
	backUpperLeg->setDominanceGroup(2);

	body->setDominanceGroup(2);

	frontArm->setDominanceGroup(3);
	backArm->setDominanceGroup(3);

	//////////////////////////////////////////////////////////////////////////

	NxD6JointDesc frontFoot;
	frontFoot.actor[0] = hb->getActor();	
	frontFoot.localAnchor[0].set(-0.3f, 0.05f, 0.0f);
	frontFoot.localAxis[0].set(0.0f, 1.0f, 1.0f);	frontFoot.localAxis[0].normalize();
	frontFoot.localNormal[0].set(1.0f, 0.0f, 0.0f);	frontFoot.localNormal[0].normalize();

	frontFoot.actor[1] = frontLowerLeg;
	frontFoot.localAnchor[1].set(0.0f, -0.25f, 0.0f);
	frontFoot.localAxis[1].set(0.0f, 1.0f, 0.0f);	frontFoot.localAxis[1].normalize();
	frontFoot.localNormal[1].set(1.0f, 0.0f, 0.0f);	frontFoot.localNormal[1].normalize();

	frontFoot.xMotion = NX_D6JOINT_MOTION_LOCKED;
	frontFoot.yMotion = NX_D6JOINT_MOTION_LOCKED;
	frontFoot.zMotion = NX_D6JOINT_MOTION_LOCKED;

	frontFoot.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	frontFoot.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	frontFoot.twistMotion = NX_D6JOINT_MOTION_LOCKED;
	frontFoot.projectionMode = NX_JPM_NONE;

	gScene->createJoint(frontFoot);

	//////////////////////////////////////////////////////////

	NxD6JointDesc backFoot;
	backFoot.actor[0] = hb->getActor();	
	backFoot.localAnchor[0].set(0.3f, 0.05f, 0.0f);
	backFoot.localAxis[0].set(0.0f, 1.0f, 1.0f);	backFoot.localAxis[0].normalize();
	backFoot.localNormal[0].set(1.0f, 0.0f, 0.0f);	backFoot.localNormal[0].normalize();

	backFoot.actor[1] = backLowerLeg;
	backFoot.localAnchor[1].set(0.0f, -0.25f, 0.0f);
	backFoot.localAxis[1].set(0.0f, 1.0f, 0.0f);	backFoot.localAxis[1].normalize();
	backFoot.localNormal[1].set(1.0f, 0.0f, 0.0f);	backFoot.localNormal[1].normalize();

	backFoot.xMotion = NX_D6JOINT_MOTION_LOCKED;
	backFoot.yMotion = NX_D6JOINT_MOTION_LOCKED;
	backFoot.zMotion = NX_D6JOINT_MOTION_LOCKED;

	backFoot.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	backFoot.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	backFoot.twistMotion = NX_D6JOINT_MOTION_LOCKED;
	backFoot.projectionMode = NX_JPM_NONE;

	gScene->createJoint(backFoot);

	//////////////////////////////////////////////////////////////////////////

	NxD6JointDesc frontKnee;
	frontKnee.actor[0] = frontLowerLeg;	
	frontKnee.localAnchor[0].set(0.0f, 0.2f, 0.0f);
	frontKnee.localAxis[0].set(0.0f, 1.0f, 0.0f);	frontKnee.localAxis[0].normalize();
	frontKnee.localNormal[0].set(1.0f, 0.0f, 0.0f);	frontKnee.localNormal[0].normalize();

	frontKnee.actor[1] = frontUpperLeg;
	frontKnee.localAnchor[1].set(0.0f, -0.12f, 0.0f);
	frontKnee.localAxis[1].set(0.0f, 0.0f, 1.0f);	frontKnee.localAxis[1].normalize();
	frontKnee.localNormal[1].set(1.0f, 0.0f, 0.0f);	frontKnee.localNormal[1].normalize();

	frontKnee.xMotion = NX_D6JOINT_MOTION_LOCKED;
	frontKnee.yMotion = NX_D6JOINT_MOTION_LOCKED;
	frontKnee.zMotion = NX_D6JOINT_MOTION_LOCKED;

	frontKnee.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	frontKnee.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	frontKnee.twistMotion = NX_D6JOINT_MOTION_LOCKED;
	frontKnee.projectionMode = NX_JPM_NONE;

	gScene->createJoint(frontKnee);
	
	//////////////////////////////////////////////////////////////////////////

	NxD6JointDesc backKnee;
	backKnee.actor[0] = backLowerLeg;	
	backKnee.localAnchor[0].set(0.0f, 0.2f, 0.0f);
	backKnee.localAxis[0].set(0.0f, 1.0f, 0.0f);	backKnee.localAxis[0].normalize();
	backKnee.localNormal[0].set(1.0f, 0.0f, 0.0f);	backKnee.localNormal[0].normalize();

	backKnee.actor[1] = backUpperLeg;
	backKnee.localAnchor[1].set(0.0f, -0.12f, 0.0f);
	backKnee.localAxis[1].set(0.0f, 0.0f, 1.0f);	backKnee.localAxis[1].normalize();
	backKnee.localNormal[1].set(1.0f, 0.0f, 0.0f);	backKnee.localNormal[1].normalize();

	backKnee.xMotion = NX_D6JOINT_MOTION_LOCKED;
	backKnee.yMotion = NX_D6JOINT_MOTION_LOCKED;
	backKnee.zMotion = NX_D6JOINT_MOTION_LOCKED;

	backKnee.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	backKnee.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	backKnee.twistMotion = NX_D6JOINT_MOTION_LOCKED;
	backKnee.projectionMode = NX_JPM_NONE;

	gScene->createJoint(backKnee);

	//////////////////////////////////////////////////////////////////////////

	NxD6JointDesc frontHip;
	frontHip.actor[0] = frontUpperLeg;	
	frontHip.localAnchor[0].set(0.0f, 0.12f, 0.0f);
	frontHip.localAxis[0].set(0.0f, 0.0f, 1.0f);	frontHip.localAxis[0].normalize();
	frontHip.localNormal[0].set(1.0f, 0.0f, 0.0f);	frontHip.localNormal[0].normalize();

	frontHip.actor[1] = body;
	frontHip.localAnchor[1].set(-0.3f, -0.3f, 0.0f);
	frontHip.localAxis[1].set(0.0f, 1.0f, 0.0f);	frontHip.localAxis[1].normalize();
	frontHip.localNormal[1].set(1.0f, 0.0f, 0.0f);	frontHip.localNormal[1].normalize();

	frontHip.xMotion = NX_D6JOINT_MOTION_LOCKED;
	frontHip.yMotion = NX_D6JOINT_MOTION_LOCKED;
	frontHip.zMotion = NX_D6JOINT_MOTION_LOCKED;

	frontHip.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	frontHip.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	frontHip.twistMotion = NX_D6JOINT_MOTION_LOCKED;
	frontHip.projectionMode = NX_JPM_NONE;

	gScene->createJoint(frontHip);

	//////////////////////////////////////////////////////////////////////////

	NxD6JointDesc backHip;
	backHip.actor[0] = backUpperLeg;	
	backHip.localAnchor[0].set(0.0f, 0.12f, 0.0f);
	backHip.localAxis[0].set(0.0f, 0.0f, 1.0f);	backHip.localAxis[0].normalize();
	backHip.localNormal[0].set(1.0f, 0.0f, 0.0f);	backHip.localNormal[0].normalize();

	backHip.actor[1] = body;
	backHip.localAnchor[1].set(0.3f, -0.3f, 0.0f);
	backHip.localAxis[1].set(0.0f, 1.0f, 0.0f);	backHip.localAxis[1].normalize();
	backHip.localNormal[1].set(1.0f, 0.0f, 0.0f);	backHip.localNormal[1].normalize();

	backHip.xMotion = NX_D6JOINT_MOTION_LOCKED;
	backHip.yMotion = NX_D6JOINT_MOTION_LOCKED;
	backHip.zMotion = NX_D6JOINT_MOTION_LOCKED;

	backHip.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	backHip.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	backHip.twistMotion = NX_D6JOINT_MOTION_LOCKED;
	backHip.projectionMode = NX_JPM_NONE;

	gScene->createJoint(backHip);

	//////////////////////////////////////////////////////////////////////////

	NxD6JointDesc frontShoulder;
	frontShoulder.actor[0] = frontArm;	
	frontShoulder.localAnchor[0].set(0.3f, 0.0f, 0.0f);

	frontShoulder.actor[1] = body;
	frontShoulder.localAnchor[1].set(-0.1f, 0.2f, 0.0f);

	frontShoulder.xMotion = NX_D6JOINT_MOTION_LOCKED;
	frontShoulder.yMotion = NX_D6JOINT_MOTION_LOCKED;
	frontShoulder.zMotion = NX_D6JOINT_MOTION_LOCKED;

	frontShoulder.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	frontShoulder.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	frontShoulder.twistMotion = NX_D6JOINT_MOTION_LOCKED;

	gScene->createJoint(frontShoulder);

	//////////////////////////////////////////////////////////////////////////

	NxD6JointDesc backShoulder;
	backShoulder.actor[0] = backArm;	
	backShoulder.localAnchor[0].set(-0.3f, 0.0f, 0.0f);

	backShoulder.actor[1] = body;
	backShoulder.localAnchor[1].set(0.1f, 0.2f, 0.0f);

	backShoulder.xMotion = NX_D6JOINT_MOTION_LOCKED;
	backShoulder.yMotion = NX_D6JOINT_MOTION_LOCKED;
	backShoulder.zMotion = NX_D6JOINT_MOTION_LOCKED;

	backShoulder.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	backShoulder.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	backShoulder.twistMotion = NX_D6JOINT_MOTION_LOCKED;

	gScene->createJoint(backShoulder);

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
		case 'a':
		case 'A':
			gJetPower *= 1.1f;
			printf("jetPower = %f\n", gJetPower);
			break;
		case 'z':	
		case 'Z':	
			gJetPower *= 0.9f;
			printf("jetPower = %f\n", gJetPower);
			break;
		case 'b':
		case 'B':
			boardRiderOneWay = !boardRiderOneWay;
			printf("board - rider one-way interactions ");
			if (boardRiderOneWay) printf("enabled\n"); else printf("disabled\n");				
			SetupDominanceGroups();
			break;
		case 'r':
		case 'R':
			riderArmsOneWay = !riderArmsOneWay;
			printf("rider - arms one-way interactions ");
			if (riderArmsOneWay) printf("enabled\n"); else printf("disabled\n");				
			SetupDominanceGroups();
			break;
		case 'd':
		case 'D':
			debrisOneWay = !debrisOneWay;
			printf("player - debris one-way interactions ");
			if (debrisOneWay) printf("enabled\n"); else printf("disabled\n");				
			SetupDominanceGroups();
			break;		
		}
	}

static void ArrowKeyCallback(int key, int x, int y)
	{
	switch (key)
		{
		case GLUT_KEY_UP:	
			gJumpPressed = true;
			break;
		case GLUT_KEY_DOWN: 
			break;
		case GLUT_KEY_LEFT:	
			gBoostFW = true;			
			break;
		case GLUT_KEY_RIGHT:
			gBoostRW = true;
			break;
		}
	}
static void ArrowKeyUpCallback(int key, int x, int y)
	{
	switch (key)
		{
		case GLUT_KEY_UP:	
			gJumpPressed = false;
			break;
		case GLUT_KEY_DOWN: 
			break;
		case GLUT_KEY_LEFT:	
			gBoostFW = false;			
			break;
		case GLUT_KEY_RIGHT:
			gBoostRW = false;
			break;
		}
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
	
	NxReal dt = 1.0f/60.0f;

	static NxReal elapsed = 0.0f;
	elapsed += getElapsedTime();
	if (elapsed < dt)		//throttle simulation
		return;
	elapsed = 0.0f;

	NxU32        nbGameObjects  = GameManager::getNbGameObjects();
	GameObject **gameObjectList = GameManager::getGameObjects();

	for(NxU32 i = 0; i < nbGameObjects; ++i)
		{
		gameObjectList[i]->think(dt);
		}

	// Get the Active Transforms from the scene
	NxU32 nbTransforms = 0;
	NxActiveTransform *activeTransforms = gScene->getActiveTransforms(nbTransforms);

	// Start simulation (non blocking)
	gScene->simulate(dt);

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
			const NxVec3 & s = gameObject->getBoxSize();
			glScalef(s.x, s.y, s.z);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glutSolidCube(2.0f);

			glPopMatrix();
			}
		}

	// Fetch simulation results
	gScene->flushStream();
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

#ifdef __PPCGEKKO__	
	char buf[4096];
    sprintf(buf,
    "\nOne Way Interactions Sample Game!\n\n"
	"Use left/right/up arrow to navigate hoverboard \nacross platforms.\n"
	"Use + to toggle one way interaction settings \nbetween the rider and his arms.\n"
	"Use 2 to toggle one way interaction settings \nbetween the board and the rider.\n"
	"Use - to toggle one way interaction settings \nbetween the player and the debris.\n"
	"Use a/b to increase/decrease board handling force.\n" );
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
	printf("One Way Interactions Sample Game!\n\n");
#ifdef __PPCGEKKO__
	printf("Use left/right/up arrow to navigate hoverboard across platforms.\n");
	printf("Use + to toggle one way interaction settings between the rider and his arms.\n");
	printf("Use 2 to toggle one way interaction settings between the board and the rider.\n");
	printf("Use - to toggle one way interaction settings between the player and the debris.\n");
	printf("Use a/b to increase/decrease board handling force.\n");
#else	
	printf("Use left/right/up arrow to navigate hoverboard across platforms.\n");
	printf("Use r to toggle one way interaction settings between the rider and his arms.\n");
	printf("Use b to toggle one way interaction settings between the board and the rider.\n");
	printf("Use d to toggle one way interaction settings between the player and the debris.\n");
	printf("Use a/z to increase/decrease board handling force.\n");
	printf("Press 0 to toggle the frame rate display.\n");
#endif	
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	int mainHandle = glutCreateWindow("SampleOneWayInteractions");
	glutSetWindow(mainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(ArrowKeyCallback);
	glutSpecialUpFunc(ArrowKeyUpCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0,0);
	
#ifdef __PPCGEKKO__	
	glutRemapButtonExt('a','a');
	glutRemapButtonExt('b','z');
	glutRemapButtonExt('1','0');
	glutRemapButtonExt('2','b');
	glutRemapButtonExt('+','r');
	glutRemapButtonExt('-','d');
#endif

	// Setup default render states
	glClearColor(0.3f, 0.4f, 0.5f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);

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
