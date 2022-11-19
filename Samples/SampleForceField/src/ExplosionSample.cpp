//
// This demo creates an explosion effect when the user presses 'x'.
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

//This method creates the explosion field and its shapes
//
//This is the most interesting function in this demo, below are other methods for controlling
//the wind gusts and turning on/off the exclude shape
//
void SampleExplosion::createExplosionForceField(const NxVec3& pos, NxActor* actor, NxScene* scene)
{
	NxForceFieldDesc ffDesc;
	NxForceFieldLinearKernelDesc	lKernelDesc;
	NxForceFieldLinearKernel*		linearKernel;

	//constant force of 100 outwards
	lKernelDesc.constant = NxVec3(100, 0, 0);

	//The forces do not depend on where the objects are positioned
	NxMat33 m;
	m.zero();
	lKernelDesc.positionMultiplier = m;
	lKernelDesc.noise = NxVec3(1.3,1.3,1.3); //adds a random noise on the forces to make the objects a little more chaotic

	//Set target velocity along the radius to 20
	lKernelDesc.velocityTarget = NxVec3(20,0,0);
	m.diagonal(NxVec3(1,0,0)); //Acts with a force relative to the current velocity to reach the
							   //target velocities. 0 means that those components won't be affected
	lKernelDesc.velocityMultiplier = m;

	// create linear kernel
	linearKernel = gScene->createForceFieldLinearKernel(lKernelDesc);
	ffDesc.kernel = linearKernel;
	
	//Attach the force field to an actor (kinematic) so that we can move it around 
	// (spawn the explosions in different places)
	ffDesc.actor = actor;

	//Create the force field around origo
	ffDesc.pose.id();
	ffDesc.coordinates = NX_FFC_SPHERICAL;
	ffDesc.flags = 0;

	gForceField = scene->createForceField(ffDesc);

	//Attach an include shape, we will animate the size of this later on, so that it grows (like a slow explosion)
	// inclusion group
	NxForceFieldShapeGroupDesc sgInclDesc;
	m_inclusionGroup = gScene->createForceFieldShapeGroup(sgInclDesc);
	NxForceFieldShape* shape = NULL;
	NxSphereForceFieldShapeDesc s;
	s.radius = 0.1f;
	s.pose.t = NxVec3(0, 0, 0);
	m_inclusionShape = m_inclusionGroup->createShape(s);

	// exclude group
	NxForceFieldShapeGroupDesc sgExclDesc;
	sgExclDesc.flags = NX_FFSG_EXCLUDE_GROUP;
	m_excludeGroup = gScene->createForceFieldShapeGroup(sgExclDesc);
	NxSphereForceFieldShapeDesc exclude;
	exclude.radius = 0.2f;
	exclude.pose.t = NxVec3(0, 0, 0);
	m_excludeShape = m_excludeGroup->createShape(exclude);

	gForceField->addShapeGroup(*m_excludeGroup);
}

//Startup for the explosion sample, loads the backyard scene and creates a force field
void SampleExplosion::setup()
{
	//Load the backyard scene (which also sets gScene)
	CreateBackyardScene(gPhysicsSDK);
	if (gScene == NULL) return; //unable to initialize the sample

	init();

	//Create a kinematic 
	{
		NxActorDesc actorDesc;
		NxBodyDesc bodyDesc;
		bodyDesc.flags |= NX_BF_KINEMATIC;
		bodyDesc.massSpaceInertia = NxVec3(1, 1, 1);
		bodyDesc.mass = 1.0f;
		actorDesc.body = &bodyDesc;
		m_kinematicActor = gScene->createActor(actorDesc);
		gForceFieldActor = m_kinematicActor;
	}

	//Add some more actors that the explosions can work on
	NxSphereShapeDesc sphere;
	sphere.radius = 0.2f;
	sphere.density = 100.0f;
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	bodyDesc.mass = 1.0f;
	actorDesc.body = &bodyDesc;
	actorDesc.shapes.pushBack(&sphere);
	for (NxU32 i = 0; i < 100; i++)
	{
		actorDesc.globalPose.t = NxVec3(NxMath::rand(-10.0f, 10.0f), NxMath::rand(0.5f, 5.0f), NxMath::rand(-10.0f, 10.0f));
		NxActor* actor = gScene->createActor(actorDesc);
		if (actor != NULL) 
		{
			m_debrisActors.pushBack(actor);
			actor->userData = new NxMat34(actorDesc.globalPose.M, actorDesc.globalPose.t);
		}
	}

	//Create the force field for this sample, see code above
	createExplosionForceField(NxVec3(0, 0, 0), m_kinematicActor, gScene);

	SetTitleString("3. Explosion");
#ifdef __PPCGEKKO__
	SetHelpString("    2: Spawn an explosion somewhere in the scene\n");
#else	
	SetHelpString("    x: Spawn an explosion somewhere in the scene\n");
#endif	
}

void SampleExplosion::onKeyPress(unsigned char key, int x, int y)
{
	switch(key)
	{
		//Start an explosion
		case 'x':
		{
			if (m_explosionFrame == 0)
				m_explosionFrame = 1; //The rest is taken care of in update()
			break;
		}
	}
}

void SampleExplosion::onVirtualKeyPress(int key, int x, int y)
{
}

//This method is called before the scene is simulated, update the position and size of the explosion
void SampleExplosion::update() 
{
	if (m_explosionFrame > 0)
	{
		if (m_explosionFrame == 1)
		{
			gForceField->addShapeGroup(*m_inclusionGroup);
			NxMat34 pose;
			pose.id();
			pose.t = NxVec3(NxMath::rand(-10.0f, 10.0f), NxMath::rand(0.5f, 5.0f), NxMath::rand(-10.0f, 10.0f));
			m_kinematicActor->setGlobalPose(pose); // to set FF, could also get rid of that indirection
			m_inclusionShape->setPose(pose);
			m_excludeShape->setPose(pose);
		}

		m_inclusionShape->isSphere()->setRadius((float)m_explosionFrame * 0.1f);
		if (m_explosionFrame > 10)
			m_excludeShape->isSphere()->setRadius((float)(m_explosionFrame-10) * 0.1f);

		m_explosionFrame++;
		if (m_explosionFrame > 100)
		{
			m_explosionFrame = 0;

			gForceField->removeShapeGroup(*m_inclusionGroup);

			m_inclusionShape->isSphere()->setRadius(0.1f);
			m_excludeShape->isSphere()->setRadius(0.2f);
		}
	}

	//If some of the debris actors have dropped outside of the scene, then move them back to their original position
	for (NxU32 i = 0; i < m_debrisActors.size(); i++)
	{
		NxActor* actor = m_debrisActors[i];
		if (actor->userData == NULL) continue;
		if (actor->getGlobalPosition().y < -5)
		{
			actor->setGlobalPose(*((NxMat34*)actor->userData));
			actor->setLinearVelocity(NxVec3(0, 0, 0));
			actor->setAngularVelocity(NxVec3(0, 0, 0));
		}
	}
}

void SampleExplosion::draw()
{
}

//Shutdown the demo, remove everything that is not removed by the main application
void SampleExplosion::cleanup()
{
	CleanUpBackyardScene(gPhysicsSDK);
	if (gScene != NULL)
	{
		for (NxU32 i = 0; i < m_debrisActors.size(); i++)
		{
			if (m_debrisActors[i]->userData != NULL)
				delete ((NxMat34*)(m_debrisActors[i]->userData));
			gScene->releaseActor(*m_debrisActors[i]);
		}
		m_debrisActors.clear();

		if (gForceField != NULL)
		{
			gScene->releaseForceField(*gForceField);
			gForceField = NULL;
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

void SampleExplosion::init()
{
	m_kinematicActor = NULL;
	m_debrisActors.clear();
	m_explosionPos = NxVec3(0, 0, 0);
	m_explosionFrame = 0;
	m_inclusionShape = NULL;
	m_excludeShape = NULL;
}

SampleExplosion::SampleExplosion()
{
	init();
}
