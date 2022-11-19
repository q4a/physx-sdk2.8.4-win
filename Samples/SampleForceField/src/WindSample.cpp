//
// This demo creates a wind effect, by animating the force of a cartesian force field.
// There is an exclusion shape that is held in place around a shed. The exclusion 
// shape can be turned on and off (actually switched between exclusion and inclusion) 
// by pressing 'x'. The force of the force field can be temporarily increased/descreased
// by pressing the UP and DOWN keys.
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

//This method creates the wind force field
//
//This is the most interesting function in this demo, below are other methods for controlling
//the wind gusts and turning on/off the exclude shape
//
void SampleWind::createWindForceField(const NxVec3& pos, NxActor* actor, NxScene* scene)
{
	NxForceFieldDesc ffDesc;
	NxForceFieldLinearKernelDesc	lKernelDesc;

	//constant force of 10 forward, which is then animated in the update() method
	lKernelDesc.constant = NxVec3(10, 0, 0);

	//The forces do not depend on where the objects are positioned
	NxMat33 m;
	m.zero();
	lKernelDesc.positionMultiplier = m;
	lKernelDesc.noise = NxVec3(1.3,1.3,1.3); //adds a random noise on the forces to make the objects a little more chaotic

	//Set target velocity along the main axis to 20
	lKernelDesc.velocityTarget = NxVec3(20,0,0);
	m.diagonal(NxVec3(1,0,0)); //Acts with a force relative to the current velocity to reach the
							   //target velocities. 0 means that those components won't be affected
	lKernelDesc.velocityMultiplier = m;

	// create linear kernel
	m_linearKernel = gScene->createForceFieldLinearKernel(lKernelDesc);
	ffDesc.kernel = m_linearKernel;
	
	ffDesc.coordinates = NX_FFC_CARTESIAN;
	//We don't attach this force field to an actor
	ffDesc.actor = NULL;
	//Create the force field around origo
	ffDesc.pose.id();
	ffDesc.flags = 0;

	gForceField = scene->createForceField(ffDesc);

	m_rbMaterial		= 0; // and all other defaults
	m_fluidMaterial		= scene->createForceFieldMaterial();
	m_clothMaterial		= scene->createForceFieldMaterial();
	m_softBodyMaterial	= scene->createForceFieldMaterial();

	//Attach an include shape, we position this so that it covers the whole scene
	NxForceFieldShape* s = NULL;
	NxBoxForceFieldShapeDesc b;
	b.dimensions = NxVec3(15, 7, 15);
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

//Startup for the wind sample, loads the backyard scene and creates a straight force field
void SampleWind::setup()
{
	//This demo does not use a kinematic actor
	gForceFieldActor = NULL;

	//Load the backyard scene (which also sets gScene)
	CreateBackyardScene(gPhysicsSDK);
	if (gScene == NULL) return; //unable to initialize the sample

	//Create the force field for this sample, see code above
	createWindForceField(NxVec3(0, 0, 0), NULL, gScene);

	setupScaling();

	SetTitleString("2. Animated wind");
#ifdef __PPCGEKKO__
	SetHelpString("    2: Toggle the exclude shape around the shed on/off\n    Up/Down: Increase the wind for a few frames\n");
#else	
	SetHelpString("    x: Toggle the exclude shape around the shed on/off\n    Up/Down: Increase the wind for a few frames\n");
#endif	
}

void SampleWind::setupScaling()
{
	for(NxU32 i = 0; i < gScene->getNbActors(); i++)
	{
		NxActor* actor = gScene->getActors()[i];
		actor->setForceFieldMaterial(m_rbMaterial);
	}
#if NX_USE_FLUID_API
	for(NxU32 i = 0; i < gScene->getNbFluids(); i++)
	{
		NxFluid* fluid = gScene->getFluids()[i];
		fluid->setForceFieldMaterial(m_fluidMaterial);
	}
#endif
	for(NxU32 i = 0; i < gScene->getNbCloths(); i++)
	{
		NxCloth* cloth = gScene->getCloths()[i];
		cloth->setForceFieldMaterial(m_clothMaterial);
	}
	for(NxU32 i = 0; i < gScene->getNbSoftBodies(); i++)
	{
		NxSoftBody* softbody = gScene->getSoftBodies()[i];
		softbody->setForceFieldMaterial(m_softBodyMaterial);
	}
}

void SampleWind::onKeyPress(unsigned char key, int x, int y)
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

void SampleWind::onVirtualKeyPress(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_UP:
		m_gushingWind = 10;
		break;
	case GLUT_KEY_DOWN:
		m_gushingWind = -10;
		break;
	}
}

//This method is called before the scene is simulated, we use it for animating the wind
//effect of the force field and for switching the "exclude shape" between exclude and include
void SampleWind::update() 
{
	if (m_switchExcludeShape && m_excludeGroup != NULL)
	{
		if(m_excludeGroupEnabled)
			gForceField->removeShapeGroup(*m_excludeGroup);
		else
			gForceField->addShapeGroup(*m_excludeGroup);
		m_switchExcludeShape = false;
		m_excludeGroupEnabled = !m_excludeGroupEnabled;
	}

	//If the UP or DOWN buttons have been pressed, increase or decrease the power of the wind for a while
	if (m_gushingWind > 0)
	{
		m_linearKernel->setConstant(NxVec3(0.0f + (float)m_gushingWind * 10.0f, 0, 0));
		m_gushingWind--;
	}
	if (m_gushingWind < 0)
	{
		m_linearKernel->setConstant(NxVec3(20.0f + (float)m_gushingWind * 10.0f, 0, 0));
		m_gushingWind++;
	}

	//Scale the force of the wind according to a sinus wave, to give some animation to the sample
	m_windPhase += 3.14159265358979323846f * 0.002f;
	if (m_windPhase > 3.14159265358979323846f*2.0f)
		m_windPhase -= 3.14159265358979323846f*2.0f;
	float windStrength = sinf(m_windPhase) * 0.2f;
	gScene->setForceFieldScale(0, m_rbMaterial, windStrength);
	gScene->setForceFieldScale(0, m_fluidMaterial, windStrength * 0.05f);
	gScene->setForceFieldScale(0, m_clothMaterial, windStrength);
	gScene->setForceFieldScale(0, m_softBodyMaterial, windStrength);
}

//Shutdown the demo, remove everything that is not removed by the main application
void SampleWind::cleanup()
{
	CleanUpBackyardScene(gPhysicsSDK);
	if (gScene != NULL)
	{
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

void SampleWind::init()
{
	m_switchExcludeShape = false;
	m_excludeGroupEnabled = true;
	m_excludeShape = NULL;
	m_excludeGroup = NULL;
	m_linearKernel = NULL;
	m_gushingWind = 0;
	m_windPhase = 3.14159265358979323846f;
}

SampleWind::SampleWind()
{
	init();
}
