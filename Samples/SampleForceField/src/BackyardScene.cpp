#include <NxPhysics.h>

#include "Actors.h"

#include "NXU_helper.h"
#include "MediaPath.h"

#include "MySoftBody.h"
#include "fluids/NxFluidDesc.h"

extern NxScene* gScene;
extern bool gHardwareSimulation;
extern MySoftBody* gSoftBody;

struct ParticleSDK
{
	NxVec3	position;
	NxVec3  velocity;
	NxReal	density;
	NxReal  lifetime;
	NxU32	id;
	NxVec3	collisionNormal;
};

//This will only be used when hardware should be used, so change to hardware without checking
class HardwareChangerNotify : public NXU_userNotify
{
public:
	virtual NxScene *NXU_preNotifyScene(unsigned int sno, NxSceneDesc& scene, const char* userProperties) 
	{ 
		scene.simType = NX_SIMULATION_HW;
		return 0;
	}

	virtual bool	NXU_preNotifyCompartment(NxCompartmentDesc& compartment) { 
		compartment.deviceCode = NX_DC_PPU_AUTO_ASSIGN;
		return true; 
	}

	virtual bool	NXU_preNotifyActor(NxActorDesc& actor, const char* userProperties) {
		for (NxU32 i = 0; i < actor.shapes.size(); i++)
		{
			NxShapeDesc* desc = actor.shapes[i];
			if (desc->getType() == NX_SHAPE_MESH)
			{
				NxTriangleMeshShapeDesc* tDesc = (NxTriangleMeshShapeDesc*)desc;
				tDesc->meshPagingMode = NX_MESH_PAGING_AUTO;
			}
		}
		return	true; 
	}

	virtual bool	NXU_preNotifyCloth(NxClothDesc &cloth, const char	*userProperties) 
	{ 
		cloth.flags |= NX_CLF_HARDWARE;
		return	true;
	}

	virtual bool	NXU_preNotifyFluid(NxFluidDesc &fluid, const char	*userProperties) 
	{ 
		fluid.flags |= NX_FF_HARDWARE;
		return	true;
	}

	virtual bool 	NXU_preNotifyTriangleMesh(NxTriangleMeshDesc& t, const char* userProperties) { 
		t.flags |= NX_MF_HARDWARE_MESH;
		return true;
	}

	virtual bool 	NXU_preNotifyClothMesh(NxClothMeshDesc& t,const char* userProperties) { 
		return true;
	}

	virtual bool 	NXU_preNotifySoftBodyMesh(NxSoftBodyMeshDesc &t,const char *userProperties) {
		return true;
	}

	virtual bool	NXU_preNotifySoftBody(NxSoftBodyDesc &softBody, const char	*userProperties) 
	{ 
		softBody.flags |= NX_SBF_HARDWARE;
		return true;
	}
};

bool CreateBackyardScene(NxPhysicsSDK* sdk)
{
	if (sdk == NULL) return false;

	gSoftBody = NULL;

	char LoadFilename[512];
	bool success = false;

	FindMediaFile("backyard.xml", LoadFilename);
	NXU::NxuPhysicsCollection* c = NXU::loadCollection(LoadFilename, NXU::FT_XML);
	if ( c )
	{
		HardwareChangerNotify* notify = NULL;
		if (gHardwareSimulation)
			notify = new HardwareChangerNotify();

		success = NXU::instantiateCollection(c, *sdk, NULL, NULL, notify);
		NXU::releaseCollection(c);
		if (notify != NULL)
			delete notify;

		if (success)
		{
			for (NxU32 i = 0; i < sdk->getNbScenes(); i++)
			{
				NxScene* scene = sdk->getScene(i);
				if (i == 0)
					gScene = scene;
				for (NxU32 j = 0; j < scene->getNbActors(); j++)
				{
					NxActor* actor = scene->getActors()[j];
					for (NxU32 k = 0; k < actor->getNbShapes(); k++)
					{
						NxShape* shape = actor->getShapes()[k];
						if (shape->isTriangleMesh())
						{
							NxTriangleMeshShape* triShape = shape->isTriangleMesh();
							NxTriangleMeshDesc* meshDesc = new NxTriangleMeshDesc();
							triShape->getTriangleMesh().saveToDesc(*meshDesc);
							shape->userData = meshDesc;
						}
					}
				}
#if NX_USE_FLUID_API
				for (NxU32 j = 0; j < scene->getNbFluids(); j++)
				{
					NxFluid* fluid = scene->getFluids()[j];

					ParticleSDK* newParticles = new ParticleSDK[fluid->getMaxParticles()];

					//Setup structure which wraps the buffers for particle creation.
					NxParticleData newParticleData;
					newParticleData.numParticlesPtr		= new NxU32;
					newParticleData.bufferPos				= &newParticles[0].position.x;
					newParticleData.bufferPosByteStride	= sizeof(ParticleSDK);
					newParticleData.bufferVel				= &newParticles[0].velocity.x;
					newParticleData.bufferVelByteStride	= sizeof(ParticleSDK);
					newParticleData.bufferLife				= &newParticles[0].lifetime;
					newParticleData.bufferLifeByteStride	= sizeof(ParticleSDK);

					fluid->setParticlesWriteData(newParticleData);
				}
#endif
				for (NxU32 j = 0; j < scene->getNbCloths(); j++)
				{
					NxCloth* cloth = scene->getCloths()[j];

					NxMeshData meshData;

					NxU32 maxVerts = cloth->getNumberOfParticles();
					NxVec3* verts = new NxVec3[maxVerts];
					NxVec3* normals = new NxVec3[maxVerts];
					meshData.verticesPosBegin = &verts[0].x;
					meshData.maxVertices = maxVerts;
					meshData.numVerticesPtr = new NxU32;
					meshData.verticesPosByteStride = sizeof(NxVec3);
					meshData.verticesNormalBegin = &normals[0].x;
					meshData.verticesNormalByteStride = sizeof(NxVec3);

					NxU32* indices = new NxU32[maxVerts * 3 * 2];
					meshData.indicesBegin = indices;
					meshData.numIndicesPtr = new NxU32;
					meshData.maxIndices = maxVerts*3*2;
					meshData.indicesByteStride = sizeof(NxU32);

					// init the buffers in case we want to draw the mesh 
					// before the SDK has filled in the correct values
					*meshData.numVerticesPtr = 0;
					*meshData.numIndicesPtr = 0;

					cloth->setMeshData(meshData);
				}
			}
		}
	}

	//Add a SoftBody, if the loading above went ok
	if (success)
	{
		NxSoftBodyDesc softBodyDesc;
		softBodyDesc.globalPose.t = NxVec3(0,3,0);
		softBodyDesc.particleRadius = 0.2f;
		softBodyDesc.volumeStiffness = 1.0f;
		softBodyDesc.stretchingStiffness = 0.2f;
		softBodyDesc.friction = 1.0f;
		softBodyDesc.solverIterations = 5;

		if (gHardwareSimulation)
			softBodyDesc.flags |= NX_SBF_HARDWARE;

		softBodyDesc.flags |= NX_SBF_COLLISION_TWOWAY;

		//Create an actor to attach the soft body to
		MySoftBody* softBody;
		NxReal softRadius = 2.0f;
		NxReal sphereH = 0.2f;
		NxVec3 pos(0.0f, 5.0f, 0.0f);
		softBodyDesc.globalPose.t = pos;
		softBody = new MySoftBody(gScene, softBodyDesc, SOFTBODY_SPHERE, softRadius, softRadius, softRadius, sphereH);
		if (!softBody || softBody->getNxSoftBody() == NULL)
		{
			printf("Error: unable to create a SoftBody\n");
			delete softBody;
		} else {
			NxActor* actor = CreateSphere(NxVec3(pos.x, pos.y-(softRadius-0.2f), pos.z), softRadius-0.2f, 0.2f);
			actor->userData = (void*)1;
			softBody->getNxSoftBody()->attachToCollidingShapes(NX_SOFTBODY_ATTACHMENT_TWOWAY);
			gSoftBody = softBody;

			//Create a kinematic and a joint to keep the softbody in the scene (so that it doesn't fly away)
			NxActorDesc actorDesc;
			NxBodyDesc bodyDesc;
			bodyDesc.flags |= NX_BF_KINEMATIC;
			bodyDesc.massSpaceInertia = NxVec3(1, 1, 1);
			bodyDesc.mass = 1.0f;
			actorDesc.body = &bodyDesc;
			NxActor* anchor = gScene->createActor(actorDesc);

			NxDistanceJointDesc dJoint;
			dJoint.actor[0] = anchor;
			dJoint.actor[1] = actor;
			dJoint.maxDistance = 10.0f;
			dJoint.flags |= NX_DJF_MAX_DISTANCE_ENABLED;
			NxJoint* joint = gScene->createJoint(dJoint);
		}
	}

	if (!success)
	{
		printf("Unable to load the PhysX data in Backyard.xml\n");
	}
	return success;
}

void CleanUpBackyardScene(NxPhysicsSDK* sdk)
{
	for (NxU32 i = 0; i < sdk->getNbScenes(); i++)
	{
		NxScene* scene = sdk->getScene(i);
		if (i == 0)
			gScene = scene;
		for (NxU32 j = 0; j < scene->getNbActors(); j++)
		{
			NxActor* actor = scene->getActors()[j];
			for (NxU32 k = 0; k < actor->getNbShapes(); k++)
			{
				NxShape* shape = actor->getShapes()[k];
				if (shape->isTriangleMesh())
				{
					NxTriangleMeshShape* triShape = shape->isTriangleMesh();
					if(shape->userData)
					{
						NxTriangleMeshDesc* meshDesc = (NxTriangleMeshDesc*) shape->userData;
						NX_DELETE(meshDesc);
					}
				}
			}
		}
#if NX_USE_FLUID_API
		for (NxU32 j = 0; j < scene->getNbFluids(); j++)
		{
			NxFluid* fluid = scene->getFluids()[j];
			NxParticleData particleData = fluid->getParticlesWriteData();
			ParticleSDK* particles = (ParticleSDK*)particleData.bufferPos; // dangerous
			
			NX_DELETE(particleData.numParticlesPtr);
			NX_DELETE_ARRAY(particles);
		}
#endif
		for (NxU32 j = 0; j < scene->getNbCloths(); j++)
		{
			NxCloth* cloth = scene->getCloths()[j];
			NxMeshData meshData = cloth->getMeshData();

			NxVec3* verts = (NxVec3*)meshData.verticesPosBegin;
			NxVec3* normals = (NxVec3*)meshData.verticesNormalBegin;
			NxU32* indices = (NxU32*)meshData.indicesBegin;
			NX_DELETE_ARRAY(verts);
			NX_DELETE_ARRAY(normals);
			NX_DELETE_ARRAY(indices);
			NX_DELETE(meshData.numIndicesPtr);
			NX_DELETE(meshData.numVerticesPtr);
		}
	}

	if(gSoftBody)
		NX_DELETE_SINGLE(gSoftBody);
}
