//
// This demo creates a vortex, which can be moved around in a scene. There is an inclusion
// shape that is kept around the center of the vortex, and an exclusion shape that is held
// in place around a shed. The exclusion shape can be turned on and off (actually switched
// between exclusion and inclusion) by pressing 'x'
//
#include "NxSampleForceField.h"

//Some global objects used for creating a binding with the main application code
extern NxPhysicsSDK* gPhysicsSDK;
extern NxForceField* gForceField;
extern NxActor* gForceFieldActor;
extern NxScene* gScene;

//This function loads the backyard XML file and sets up everything so that the renderer
//in the main application can render the scene
extern bool CreateBackyardScene(NxPhysicsSDK* sdk);
extern void CleanUpBackyardScene(NxPhysicsSDK* sdk);


//This method creates the vortex force field and include/exclude shapes.
//
//This is the most interesting function in this demo, below are other methods for controlling
//the movement of the vortex and turning on/off the exclude shape
//
void SampleVortex::createVortexForceField(const NxVec3& pos, NxActor* actor, NxScene* scene)
{
	NxForceFieldDesc ffDesc;
	NxForceFieldLinearKernelDesc	lKernelDesc;
	NxForceFieldLinearKernel*		linearKernel;

	ffDesc.coordinates = NX_FFC_CYLINDRICAL;
	//Attach the vortex in an actor (which we use for moving the field around in the world)
	ffDesc.actor = actor;
	//attach the force field at the center of the actor
	m_forceFieldTransform.id(); 
	ffDesc.pose = m_forceFieldTransform;

	//constant force of 30 towards the center (which is then counter-acted by radial forces specified below)
	//constant force of 4 upwards (creating a constant lift on the objects)
	lKernelDesc.constant = NxVec3(-30, 4.0f, 0); 

	//The target where we want the objects to end up is at radius 3 from the center. We use
	//Y=0 as the target in along the y-axis together with the m(0,1)=-5 to create a force
	//directed outwards from the center of the vortex when objects are floating towards the
	//top of the vortex.
	lKernelDesc.positionTarget = NxVec3(3,0,0);

	//Setup radial forces, depending on where the objects are positioned
	NxMat33 m;
	m.zero();
	m(0,0) = 10; //radial error -> radial force. If outside of target radius, act with a force of 10*distance inwards
	m(0,1) = -5; //axial error -> radial force. If the y component of the object position is above the target y position (0), 
				 //then act with a force of 5*distance outwards. This reduces the force of 30 inwards that we setup earlier,
				 //making the vortex broaden out in the top
	m(0,2) = 0;  //there is no tangential error in cylindrical coordinates, so we just set this to 0
	lKernelDesc.positionMultiplier = m;
	lKernelDesc.noise = NxVec3(1.3,1.3,1.3); //adds a random noise on the forces to make the objects a little more chaotic

	//Set target velocity along the tangent of the vortex to 30 (the other directions to 0)
	lKernelDesc.velocityTarget = NxVec3(0,0,30);
	m.diagonal(NxVec3(1,1,1)); //Acts with a force relative to the current velocity to reach the
							   //target velocities. If the velocity is above 0 in radial direction, then
							   //the radial velocity is decreased. If the velocity is below 30 in tangential
							   //direction, then the velocity is increased until it reaches that velocity.
	lKernelDesc.velocityMultiplier = m;

	//You can try some fall-off forces if you e.g. want the vortex to lose power 
	//along the radial direction when the distance from its center increases:
	//lKernelDesc.falloffLinear = NxVec3(5.0f, 0, 0);
	//lKernelDesc.falloffQuadratic = NxVec3(5.0f, 0, 0);
	linearKernel = gScene->createForceFieldLinearKernel(lKernelDesc);
	ffDesc.kernel = linearKernel;
	ffDesc.flags = 0;

	gForceField = scene->createForceField(ffDesc);

	//Attach an include shape, we position this so that it covers the vortex specified above
	NxForceFieldShape* s = NULL;
	NxBoxForceFieldShapeDesc b;
	b.dimensions = NxVec3(5, 7, 5);
	b.pose.t = NxVec3(0, 3.5f, 0);
	s = gForceField->getIncludeShapeGroup().createShape(b);

	//Create an exclude shape, positioned around the shed
	NxForceFieldShapeGroupDesc sgDesc;
	sgDesc.flags = NX_FFSG_EXCLUDE_GROUP;
	m_excludeGroup = gScene->createForceFieldShapeGroup(sgDesc);
	
	NxBoxForceFieldShapeDesc exclude;
	exclude.dimensions = NxVec3(2.25f, 1.5f, 1.75f);
	exclude.pose.t = NxVec3(8.85f, 1.5f, -10.3f);
	m_excludeShape = m_excludeGroup->createShape(exclude);

	gForceField->addShapeGroup(*m_excludeGroup);
}

//Startup for the vortex sample, loads the backyard scene and creates a vortex
void SampleVortex::setup()
{
	//Load the backyard scene (which also sets gScene)
	CreateBackyardScene(gPhysicsSDK);
	if (gScene == NULL) return; //unable to initialize the sample

	//Create a kinematic 
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	bodyDesc.flags |= NX_BF_KINEMATIC;
	bodyDesc.massSpaceInertia = NxVec3(1, 1, 1);
	bodyDesc.mass = 1.0f;
	actorDesc.body = &bodyDesc;
	m_kinematicActor = gScene->createActor(actorDesc);
	gForceFieldActor = m_kinematicActor;

	//Create the force field for this sample, see code above
	createVortexForceField(NxVec3(0, 0, 0), m_kinematicActor, gScene);

	SetTitleString("1. Tornado");
#ifdef __PPCGEKKO__
	SetHelpString("    2: Toggle the exclude shape\n    around the shed on/off\n    Arrows: Move the tornado around\n");
#else	
	SetHelpString("    x: Toggle the exclude shape around the shed on/off\n    Arrows: Move the tornado around\n");
#endif	
}

void SampleVortex::onKeyPress(unsigned char key, int x, int y)
{
	switch(key)
	{
		//Switch the exclude shape between excluding and including (delayed until the update method)
		case 'x':
		{
			m_switchExcludeShape = true;
			break;
		}
	}
}

void SampleVortex::onVirtualKeyPress(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_LEFT:
			m_forceFieldTransform.t -= NxVec3(0.05f,0,0); 
			m_forceFieldMoved = true;
			break;
		case GLUT_KEY_RIGHT:
			m_forceFieldTransform.t += NxVec3(0.05f,0,0); 
			m_forceFieldMoved = true;
			break;
		case GLUT_KEY_UP:
			m_forceFieldTransform.t -= NxVec3(0,0,0.05f); 
			m_forceFieldMoved = true;
			break;
		case GLUT_KEY_DOWN:
			m_forceFieldTransform.t += NxVec3(0,0,0.05f); 
			m_forceFieldMoved = true;
			break;
	}
}

//This method is called before the scene is simulated, we use it for moving the force field
//and for switching the "exclude shape" between exclude and include
void SampleVortex::update() 
{
	moveForceField();
	if (m_switchExcludeShape && m_excludeGroup != NULL)
	{
		if(m_excludeGroupEnabled)
			gForceField->removeShapeGroup(*m_excludeGroup);
		else
			gForceField->addShapeGroup(*m_excludeGroup);
		m_switchExcludeShape = false;
		m_excludeGroupEnabled = !m_excludeGroupEnabled;
	}
}

//Moves the force field around (by animating a kinematic actor)
void SampleVortex::moveForceField()
{
	if (!m_forceFieldMoved) return;
	if (m_kinematicActor == NULL) return;

	m_kinematicActor->moveGlobalPose(m_forceFieldTransform);

	m_forceFieldMoved = false;
}

//Shutdown the demo, remove everything that is not removed by the main application
void SampleVortex::cleanup()
{
	CleanUpBackyardScene(gPhysicsSDK);
	if (gScene != NULL)
	{
		if (gForceField != NULL)
		{
			gScene->releaseForceField(*gForceField);
			gForceField = NULL;
		}
		if (m_kinematicActor != NULL)
		{
			gScene->releaseActor(*m_kinematicActor);
			m_kinematicActor = NULL;
			gForceFieldActor = NULL;
		}

		if (gPhysicsSDK != NULL)
		{
			gPhysicsSDK->releaseScene(*gScene);
			gScene = NULL;
		}
	}

	//Also init the state variables for the next setup() call
	init();
}

void SampleVortex::init()
{
	m_forceFieldMoved = false;
	m_switchExcludeShape = false;
	m_excludeGroupEnabled = true;
	m_forceFieldTransform.id();
	m_kinematicActor = NULL;
	m_excludeShape = NULL;
}

SampleVortex::SampleVortex()
{
	init();
}
