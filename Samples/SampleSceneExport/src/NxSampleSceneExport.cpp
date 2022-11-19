// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Scene Export
// Description: This sample program shows how to export and import a scene to and
//              from a special file format.
//
// Originally written by: Stephen Hatcher, based on work by Pierre Terdiman (01.01.04)
//
// ===============================================================================

#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Physics code
#undef random
#include "NxPhysics.h"
#include "UserAllocator.h"
#include "ErrorStream.h"
#include "NxCooking.h"
#include "Stream.h"
#include "DebugRenderer.h"
#include "MediaPath.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"

// include exporter here
#include "NXU_helper.h"  // NxuStream helper functions.

// static variables
static bool gPause = false;
static NxPhysicsSDK *gPhysicsSDK = NULL;
static const NxU32 gMaxScenes = 3;
static NxScene *gScenes[gMaxScenes] = 
{
	NULL, NULL
};
static NxU32 gCurrentScene = 0;
static NxVec3 gDefaultGravity(0.0f,  - 9.81f, 0.0f);
static UserAllocator *gMyAllocator = NULL;
static ErrorStream gErrorStream;
static DebugRenderer gDebugRenderer;
//static PrefsRenderer    gPrefsRenderer;

static int gMainHandle;
static NxVec3 gEye(50.0f, 50.0f, 50.0f);
static NxVec3 gDir( - 0.6,  - 0.2,  - 0.7);
static NxVec3 gNormal;
static int gMouseX = 0;
static int gMouseY = 0;

// forward declarations
static bool InitNx();
static bool InitBlankScene();

static bool gSave = false;
static int	gLoad = 0;
static bool gClear = false;

#if NX_USE_FLUID_API

class FluidMem
{
public:

  FluidMem(NxFluidDesc &desc)
  {

  	NxParticleData &data = desc.particlesWriteData;

  	NxParticleData &initial = desc.initialParticleData;
		NxU32 pcount = 0;
		if ( initial.numParticlesPtr )
  	 pcount            = *initial.numParticlesPtr; // number of particles in the initial data field.

  	if (initial.bufferPosByteStride && pcount )
  	{
  		data.bufferPosByteStride = initial.bufferPosByteStride;
  		data.bufferPos = (NxF32*) new NxU8[desc.maxParticles*data.bufferPosByteStride];
  		memcpy(data.bufferPos, initial.bufferPos, sizeof(data.bufferPosByteStride*pcount) );
  	}
  	else
  	{
  		data.bufferPos = (NxF32*) new NxU8[desc.maxParticles*sizeof(NxVec3)];
  		data.bufferPosByteStride = sizeof(NxVec3);
  	}

  	if (initial.bufferVelByteStride && pcount )
  	{
  		data.bufferVelByteStride = initial.bufferVelByteStride;
  		data.bufferVel = (NxF32*) new NxU8[desc.maxParticles*data.bufferVelByteStride];
  		memcpy(data.bufferVel, initial.bufferVel, sizeof(data.bufferVelByteStride*pcount) );
  	}
  	else
  	{
  		data.bufferVel = (NxF32 *) new NxU8[desc.maxParticles*sizeof(NxVec3)];
  		data.bufferVelByteStride = sizeof(NxVec3);
  	}

  	if (initial.bufferLifeByteStride && pcount)
  	{
  		data.bufferLifeByteStride = initial.bufferLifeByteStride;
  		data.bufferLife = (NxF32*) new NxU8[desc.maxParticles*data.bufferLifeByteStride];
  		memcpy(data.bufferLife, initial.bufferLife, sizeof(data.bufferLifeByteStride*pcount) );
  	}
  	else
  	{
  		data.bufferLife = new NxF32[desc.maxParticles];
  		data.bufferLifeByteStride = sizeof(NxF32);
  	}

  	if (initial.bufferDensityByteStride && pcount )
  	{
  		data.bufferDensityByteStride = initial.bufferDensityByteStride;
  		data.bufferDensity = (NxF32 *)new NxU8[desc.maxParticles*data.bufferDensityByteStride];
  		memcpy(data.bufferDensity, initial.bufferDensity, sizeof(data.bufferDensityByteStride*pcount) );
  	}
  	else
  	{
  		data.bufferDensity = new NxF32[desc.maxParticles];
  		data.bufferDensityByteStride = sizeof(NxF32);
  	}

    #if NX_SDK_VERSION_NUMBER >= 260
    if ( initial.bufferFlagByteStride && pcount )
  	{
  		data.bufferFlagByteStride = initial.bufferFlagByteStride;
  		data.bufferFlag = (NxU32*) new NxU8[desc.maxParticles*data.bufferFlagByteStride];
  		memcpy(data.bufferFlag, initial.bufferFlag, sizeof(data.bufferFlagByteStride*pcount) );
  	}
  	else
  	{
  		data.bufferFlag = new NxU32[desc.maxParticles];
  		data.bufferFlagByteStride = sizeof(NxU32);
  	}
    #endif

    #if NX_SDK_VERSION_NUMBER >= 260
    if ( initial.bufferIdByteStride && pcount )
  	{
  		data.bufferIdByteStride = initial.bufferIdByteStride;
  		data.bufferId = (NxU32*) new NxU8[desc.maxParticles*data.bufferIdByteStride];
  		memcpy(data.bufferId, initial.bufferId, sizeof(data.bufferIdByteStride*pcount) );
  	}
  	else
  	{
  		data.bufferId = new NxU32[desc.maxParticles];
  		data.bufferIdByteStride = sizeof(NxU32);
  	}
    #endif

  	data.numParticlesPtr = &mNumParticles;
  	*data.numParticlesPtr = pcount;


  	desc.userData = this; // assign the user data pointer to this allocated helper class so we can delete it later.

		mData = data;

  }

  ~FluidMem(void)
  {
  	delete mData.bufferPos;
  	delete mData.bufferVel;
  	delete mData.bufferLife;
  	delete mData.bufferDensity;
#if NX_SDK_VERSION_NUMBER >= 260
  	delete mData.bufferFlag;
  	delete mData.bufferId;
#endif
  }

  NxU32  mNumParticles;
  NxParticleData	mData;

};

#endif

class ClothMem
{
public:
  ClothMem(NxClothDesc &desc)
  {
  	desc.userData = this;
  	mClothIndices = 0;
  	mClothVertices = 0;

	  NxClothMesh *clothMesh = desc.clothMesh;
	  NxClothMeshDesc cdesc;
	  clothMesh->saveToDesc(cdesc);

		NxU32 vcount = cdesc.numVertices;
		NxU32 tcount = cdesc.numTriangles;

		if ( desc.flags & NX_CLF_TEARABLE )
		{
			vcount*=2;
			tcount*=2;
		}

		desc.flags|=NX_CLF_VISUALIZATION;

		if ( vcount && tcount )
		{

  		mClothVertices = new NxF32[vcount*3];
  		mClothIndices  = new NxU32[tcount*3];

      NxMeshData &mesh = desc.meshData;

      mesh.verticesPosBegin = mClothVertices;
      mesh.verticesNormalBegin = 0;
      mesh.verticesPosByteStride = sizeof(NxF32)*3;
      mesh.verticesNormalByteStride = 0;
      mesh.maxVertices = vcount;
      mesh.numVerticesPtr = &mNumVertices;
      mesh.indicesBegin = mClothIndices;
			mesh.indicesByteStride = sizeof(NxU32);
      mesh.maxIndices   = tcount*3;
      mesh.numIndicesPtr = &mNumIndices;
      mesh.parentIndicesBegin = 0;
      mesh.parentIndicesByteStride = 0;
      mesh.maxParentIndices = 0;
      mesh.numParentIndicesPtr = 0;
      mesh.flags = 0;
    }

  }

  ~ClothMem(void)
  {
  	delete mClothVertices;
  	delete mClothIndices;
  }

	NxU32  mNumIndices;
	NxU32  mNumVertices;

  NxU32 *mClothIndices;
  NxF32 *mClothVertices;
};

#if NX_USE_SOFTBODY_API
class SoftBodyMem
{
public:
  SoftBodyMem(NxSoftBodyDesc &desc)
  {
  	desc.userData = this;

  	mSoftBodyIndices = 0;
  	mSoftBodyVertices = 0;

	  NxSoftBodyMesh *softBodyMesh = desc.softBodyMesh;
	  NxSoftBodyMeshDesc cdesc;
	  softBodyMesh->saveToDesc(cdesc);

		NxU32 vcount = cdesc.numVertices;
		NxU32 tcount = cdesc.numTetrahedra;

		desc.flags|=NX_SBF_VISUALIZATION;

		if ( vcount && tcount )
		{

  		mSoftBodyVertices = new NxF32[vcount*3];
  		mSoftBodyIndices  = new NxU32[tcount*4];

      NxMeshData &mesh = desc.meshData;

      mesh.verticesPosBegin = mSoftBodyVertices;
      mesh.verticesNormalBegin = 0;
      mesh.verticesPosByteStride = sizeof(NxF32)*3;
      mesh.verticesNormalByteStride = 0;
      mesh.maxVertices = vcount;
      mesh.numVerticesPtr = &mNumVertices;
      mesh.indicesBegin = mSoftBodyIndices;
			mesh.indicesByteStride = sizeof(NxU32);
      mesh.maxIndices   = tcount*4;
      mesh.numIndicesPtr = &mNumIndices;
      mesh.parentIndicesBegin = 0;
      mesh.parentIndicesByteStride = 0;
      mesh.maxParentIndices = 0;
      mesh.numParentIndicesPtr = 0;
      mesh.flags = 0;
    }

  }

  ~SoftBodyMem(void)
  {
  	delete mSoftBodyVertices;
  	delete mSoftBodyIndices;
  }

	NxU32  mNumIndices;
	NxU32  mNumVertices;

  NxU32 *mSoftBodyIndices;
  NxF32 *mSoftBodyVertices;
};
#endif

#if NX_USE_FLUID_API
void releaseFluids(NxScene *scene)
{
	if ( scene )
	{
  	NxU32 nb = scene->getNbFluids();
  	if ( nb )
  	{
  		NxFluid **fluids = scene->getFluids();
  		for (NxU32 i=0; i<nb; i++)
  		{
  			NxFluid *f = fluids[i];
  			FluidMem *fmem = (FluidMem *) f->userData;
  			delete fmem;
  		}
  	}
  }
}
#endif

void releaseCloths(NxScene *scene)
{
	if ( scene )
	{
  	NxU32 nb = scene->getNbCloths();
  	if ( nb )
  	{
  		NxCloth **cloths = scene->getCloths();
  		for (NxU32 i=0; i<nb; i++)
  		{
  			NxCloth *c = cloths[i];
  			ClothMem *cmem = (ClothMem *) c->userData;
  			delete cmem;
  		}
  	}
  }
}

#if NX_USE_SOFTBODY_API
void releaseSoftBodies(NxScene *scene)
{
	if ( scene )
	{
  	NxU32 nb = scene->getNbSoftBodies();
  	if ( nb )
  	{
  		NxSoftBody **softBodys = scene->getSoftBodies();
  		for (NxU32 i=0; i<nb; i++)
  		{
  			NxSoftBody *c = softBodys[i];
  			SoftBodyMem *cmem = (SoftBodyMem *) c->userData;
  			delete cmem;
  		}
  	}
  }
}
#endif

void releaseScene(NxScene &scene)
{
#if NX_USE_FLUID_API
	releaseFluids(&scene);
#endif
	releaseCloths(&scene);
#if NX_USE_SOFTBODY_API
	releaseSoftBodies(&scene);
#endif
	gPhysicsSDK->releaseScene(scene);
}


class MyUserNotify: public NXU_userNotify, public NXU_errorReport
{
public:
		virtual void NXU_errorMessage(bool isError, const char *str)
		{
			if (isError)
			{
				printf("NxuStream ERROR: %s\r\n", str);
			}
			else
			{
				printf("NxuStream WARNING: %s\r\n", str);
			}
		}

	virtual void	NXU_notifyScene(NxU32 sno,	NxScene	*scene,	const	char *userProperties)
  {
		gScenes[gCurrentScene] = scene;
		gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1.0f);
		gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1.0f);
		gPhysicsSDK->setParameter(NX_VISUALIZE_CLOTH_MESH, 1.0f );
#if NX_USE_FLUID_API
		gPhysicsSDK->setParameter(NX_VISUALIZE_FLUID_POSITION, 0.2f );
#endif
#if NX_USE_SOFTBODY_API
		gPhysicsSDK->setParameter(NX_VISUALIZE_SOFTBODY_MESH, 1.0f );
#endif
  };

	virtual void	NXU_notifyJoint(NxJoint	*joint,	const	char *userProperties){};

	virtual void	NXU_notifyActor(NxActor	*actor,	const	char *userProperties){};

	virtual void	NXU_notifyCloth(NxCloth	*cloth,	const	char *userProperties){};

	virtual void	NXU_notifyFluid(NxFluid	*fluid,	const	char *userProperties){};

 	virtual void 	NXU_notifyTriangleMesh(NxTriangleMesh *t,const char *userProperties){};

 	virtual void 	NXU_notifyConvexMesh(NxConvexMesh *c,const char *userProperties){};

 	virtual void 	NXU_notifyClothMesh(NxClothMesh *t,const char *userProperties){};

 	virtual void 	NXU_notifyCCDSkeleton(NxCCDSkeleton *t,const char *userProperties){};

	virtual void 	NXU_notifyHeightField(NxHeightField *t,const char *userProperties){};

	virtual NxScene *NXU_preNotifyScene(unsigned	int	sno, NxSceneDesc &scene, const char	*userProperties)
  {
		assert( gScenes[gCurrentScene] == 0 );
		if ( gScenes[gCurrentScene] )
		{
			releaseScene(*gScenes[gCurrentScene]);
			gScenes[gCurrentScene] = 0;
		}
		return 0;
  };

	virtual bool	NXU_preNotifyJoint(NxJointDesc &joint, const char	*userProperties) { return	true; };

	virtual bool	NXU_preNotifyActor(NxActorDesc &actor, const char	*userProperties)
  {
		for (NxU32 i=0; i<actor.shapes.size(); i++)
		{
			NxShapeDesc *s = actor.shapes[i];
			s->shapeFlags|=NX_SF_VISUALIZATION; // make sure the shape visualization flags are on so we can see things!
		}
		return true;
  };

	virtual bool	NXU_preNotifyCloth(NxClothDesc &cloth, const char	*userProperties)
  {
  	ClothMem *mem = new ClothMem(cloth);
    return	true;
  };


#if NX_USE_SOFTBODY_API
	virtual bool	NXU_preNotifySoftBody(NxSoftBodyDesc &softBody, const char	*userProperties)
  {
  	SoftBodyMem *mem = new SoftBodyMem(softBody);
    return	true;
  };

	void	NXU_notifySoftBody(NxSoftBody	*softBody,	const	char *userProperties)
	{
	}

	void 	NXU_notifySoftBodyMesh(NxSoftBodyMesh *t,const char *userProperties)
	{
	}

	bool 	NXU_preNotifySoftBodyMesh(NxSoftBodyMeshDesc &t,const char *userProperties)
	{
		return true;
	}

	void	NXU_notifySoftBodyFailed(NxSoftBodyDesc &softBody, const char	*userProperties)
	{
  	SoftBodyMem *mem = (SoftBodyMem *) softBody.userData;
  	delete mem;
	}

	void 	NXU_notifySoftBodyMeshFailed(NxSoftBodyMeshDesc &t,const char *userProperties)
	{
	}
#endif

#if NX_USE_FLUID_API
	virtual bool	NXU_preNotifyFluid(NxFluidDesc &fluid, const char	*userProperties)
  {
  	FluidMem *mem = new FluidMem(fluid);
  	return true;
  };
#endif


	virtual bool 	NXU_preNotifyTriangleMesh(NxTriangleMeshDesc &t,const char *userProperties) { return true;	};

	virtual bool 	NXU_preNotifyConvexMesh(NxConvexMeshDesc &t,const char *userProperties)	{	return true; };

	virtual bool 	NXU_preNotifyClothMesh(NxClothMeshDesc &t,const char *userProperties) { return true; };

	virtual bool 	NXU_preNotifyCCDSkeleton(NxSimpleTriangleMesh &t,const char *userProperties)	{	return true; };

	virtual bool 	NXU_preNotifyHeightField(NxHeightFieldDesc &t,const char *userProperties) {	return true; }

	virtual bool 	NXU_preNotifySceneInstance(const char *name,const char *sceneName,const char *userProperties,NxMat34 &rootNode,NxU32 depth) { return true; };


	virtual void NXU_notifySceneFailed(unsigned	int	sno, NxSceneDesc &scene, const char	*userProperties) { };

	virtual void	NXU_notifyJointFailed(NxJointDesc &joint, const char	*userProperties) {  };

	virtual void	NXU_notifyActorFailed(NxActorDesc &actor, const char	*userProperties) { };

	virtual void	NXU_notifyClothFailed(NxClothDesc &cloth, const char	*userProperties)
  {
  	ClothMem *mem = (ClothMem *) cloth.userData;
  	delete mem;
  };

#if NX_USE_FLUID_API
	virtual void	NXU_notifyFluidFailed(NxFluidDesc &fluid, const char	*userProperties)
  {
  	FluidMem *mem = (FluidMem *) fluid.userData;
  	delete mem;
  };
#endif

	virtual void 	NXU_notifyTriangleMeshFailed(NxTriangleMeshDesc &t,const char *userProperties) {	};

	virtual void 	NXU_notifyConvexMeshFailed(NxConvexMeshDesc &t,const char *userProperties)	{	 };

	virtual void 	NXU_notifyClothMeshFailed(NxClothMeshDesc &t,const char *userProperties) { };

	virtual void 	NXU_notifyCCDSkeletonFailed(NxSimpleTriangleMesh &t,const char *userProperties)	{	 };

	virtual void 	NXU_notifyHeightFieldFailed(NxHeightFieldDesc &t,const char *userProperties) {	}


};

MyUserNotify gUserNotify;


//==================================================================================
//This is the function that calls the importer to load the state of the currently selected scene from a file
NX_BOOL LoadScene(const char *pFilename,NXU::NXU_FileType type)
{
	NX_BOOL success = false;

	if (gPhysicsSDK)
	{
		char LoadFilename[512];
		FindMediaFile(pFilename, LoadFilename);

		NXU::NxuPhysicsCollection *c = NXU::loadCollection(LoadFilename, type );

		if ( c )
		{
			if (gScenes[gCurrentScene])
			{
				// Release old scene
				releaseScene(*gScenes[gCurrentScene]);
				gScenes[gCurrentScene] = 0;
			}

			if (gPhysicsSDK)
			{
				success = NXU::instantiateCollection( c, *gPhysicsSDK, 0, 0, &gUserNotify );
			}
			NXU::releaseCollection(c);
		}
		else
		{
		}
	}

	gLoad = 0;

	if ( success )
	  printf("Scene %d loaded from file %s.\n", gCurrentScene, pFilename);

	return success;
}

//==================================================================================
//This is the function that calls the exporter to save out the state of the currently selected scene to a file
void SaveScene(const char *pFilename)
{
	char SaveFilename[512];
	GetTempFilePath(SaveFilename);
	strcat(SaveFilename, pFilename);

  NXU::setUseClothActiveState(true);
  NXU::setUseSoftBodyActiveState(true);

	NXU::NxuPhysicsCollection *c = NXU::extractCollectionScene(gScenes[gCurrentScene]);

	if (c)
	{
		char scratch[512];
		sprintf(scratch, "%s.xml", SaveFilename);
		printf("Saving NxuStream XML file to '%s'\r\n", scratch);
		NXU::saveCollection(c, scratch, NXU::FT_XML, false, false);

		sprintf(scratch, "%s.nxb", SaveFilename);
		printf("Saving NxuStream BINARY file to '%s'\r\n", scratch);
		NXU::saveCollection(c, scratch, NXU::FT_BINARY, true, true);

		sprintf(scratch, "%s.dae", SaveFilename);
		printf("Saving COLLADA Physics 1.4.1 file to '%s'\r\n", scratch);
		NXU::saveCollection(c, scratch, NXU::FT_COLLADA, false, false);

		NXU::releaseCollection(c);

	}

}


//==================================================================================
void CreateCube(const NxVec3 &pos, int size, const NxVec3 *initial_velocity = NULL)
{
	if (gScenes[gCurrentScene])
	{
		// Create body
		NxBodyDesc BodyDesc;
		BodyDesc.angularDamping = 0.5f;
		//	BodyDesc.maxAngularVelocity	= 10.0f;
		if (initial_velocity)
		{
			BodyDesc.linearVelocity =  *initial_velocity;
		}

		NxBoxShapeDesc BoxDesc;
		BoxDesc.dimensions = NxVec3(float(size), float(size), float(size));

		NxActorDesc ActorDesc;
		ActorDesc.shapes.pushBack(&BoxDesc);
		ActorDesc.body = &BodyDesc;
		ActorDesc.density = 10.0f;
		ActorDesc.globalPose.t = pos;
		gScenes[gCurrentScene]->createActor(ActorDesc)->userData = (void*)size;
	}
}

//==================================================================================
void CreateStack(int size)
{
	float CubeSize = 2.0f;
	//	float Spacing = 0.01f;
	float Spacing = 0.0001f;
	NxVec3 Pos(0.0f, CubeSize, 0.0f);
	float Offset =  - size *(CubeSize *2.0f + Spacing) *0.5f;
	while (size)
	{
		for (int i = 0; i < size; i++)
		{
			Pos.x = Offset + float(i)*(CubeSize *2.0f + Spacing);
			CreateCube(Pos, (int)CubeSize);
		}
		Offset += CubeSize;
		Pos.y += (CubeSize *2.0f + Spacing);
		size--;
	}
}

//==================================================================================
void CreateTower(int size)
{
	float CubeSize = 2.0f;
	float Spacing = 0.01f;
	NxVec3 Pos(0.0f, CubeSize, 0.0f);
	while (size)
	{
		CreateCube(Pos, (int)CubeSize);
		Pos.y += (CubeSize *2.0f + Spacing);
		size--;
	}
}

//==================================================================================
bool InitNx()
{
	if (!gMyAllocator)
	{
		gMyAllocator = new UserAllocator;
	}

	NXU::setErrorReport(&gUserNotify);
	NXU::setEndianMode(isProcessorBigEndian());

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gMyAllocator, &gErrorStream, desc, &errorCode);
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
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1.0f);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1.0f);

	for (NxU32 i = 0; i < gMaxScenes; ++i)
	{
		gCurrentScene = i;
		if (!InitBlankScene()) return false;
	}
	gCurrentScene = 0;

	return true;
}

//==================================================================================
bool InitBlankScene()
{
	if (gPhysicsSDK)
	{
		// Create a blank scene
		NxSceneDesc sceneDesc;
		sceneDesc.gravity = gDefaultGravity;

		gScenes[gCurrentScene] = gPhysicsSDK->createScene(sceneDesc);
		if(gScenes[gCurrentScene] == NULL) 
		{
			printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
			return false;
		}

		NxMaterial *defaultMaterial = gScenes[gCurrentScene]->getMaterialFromIndex(0);
		//NxMaterial *defaultMaterial = NxScene_getMaterialFromIndex( gScene, 0 );
		defaultMaterial->setRestitution(0.0f);
		defaultMaterial->setStaticFriction(0.5f);
		defaultMaterial->setDynamicFriction(0.5f);

		// Create ground plane
		NxPlaneShapeDesc planeDesc;
		planeDesc.setToDefault();

		NxActorDesc ActorDesc;
		ActorDesc.shapes.pushBack(&planeDesc);
		gScenes[gCurrentScene]->createActor(ActorDesc);
	}
	return true;
}

//==================================================================================
void ReleaseNx()
{
	NXU::releasePersistentMemory();
	if (gPhysicsSDK)
	{
		for (NxU32 i = 0; i < gMaxScenes; ++i)
		{
			if (gScenes[i])
			{
				gPhysicsSDK->releaseScene(*gScenes[i]);
				gScenes[i] = NULL;
			}
		}
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
	}

	if (gMyAllocator != NULL)
	{
		delete gMyAllocator;
		gMyAllocator = NULL;
	}
}

//==================================================================================
void KeyboardCallback(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:
			{
				exit(0);
			}
			break;

		case '0':
			{
				//gPrefsRenderer.toggleEnable();
			}
			break;

		case ' ':
			{
				CreateCube(NxVec3(0.0f, 20.0f, 0.0f), 1+(rand() &3));
			}
			break;

		case 's':
			{
				CreateStack(10);
			}
			break;

		case 'b':
			{
				CreateStack(30);
			}
			break;

		case 't':
			{
				CreateTower(30);
			}
			break;

		case 'p':
			{
				gPause = !gPause;
			}
			break;

		case 101:
		case '8':
			{
				gEye += gDir * 2.0f;
			}
			break;

		case 103:
		case '2':
			{
				gEye -= gDir * 2.0f;
			}
			break;

		case 100:
		case '4':
			{
				gEye -= gNormal * 2.0f;
			}
			break;

		case 102:
		case '6':
			{
				gEye += gNormal * 2.0f;
			}
			break;

		case 'w':
			{
				NxVec3 t = gEye;
				NxVec3 Vel = gDir;
				Vel.normalize();
				Vel *= 200.0f;
				CreateCube(t, 8, &Vel);
			}
			break;

		case '1':
			{
				gSave = true;
			}
			break;
		case 'x':
			gLoad = 2;
			break;
		case 'y':
			gLoad = 3;
			break;
		case 'z':
			gLoad = 4;
			break;
		case '3':
			{
				gLoad = 1;
			}
			break;

		case 'c':
			{
				gClear = true;
			}
			break;

		case '\t':
			{
				//Switch to the next scene
				++gCurrentScene;
				if (gCurrentScene == gMaxScenes)
				{
					gCurrentScene = 0;
				}
				printf("Scene %d selected.\n", gCurrentScene);
			}
			break;

	}
}

//==================================================================================
void ArrowKeyCallback(int key, int x, int y)
{
	KeyboardCallback(key, x, y);
}

//==================================================================================
void MouseCallback(int button, int state, int x, int y)
{
	gMouseX = x;
	gMouseY = y;
}

//==================================================================================
void MotionCallback(int x, int y)
{
	int dx = gMouseX - x;
	int dy = gMouseY - y;

	gDir.normalize();
	gNormal.cross(gDir, NxVec3(0, 1, 0));

	NxQuat qx(NxPiF32 *dx * 20 / 180.0f, NxVec3(0, 1, 0));
	qx.rotate(gDir);
	NxQuat qy(NxPiF32 *dy * 20 / 180.0f, gNormal);
	qy.rotate(gDir);

	gMouseX = x;
	gMouseY = y;
}

//==================================================================================
bool FileExistTestSimple(const char *fname)
{
	char buff[512];
	FindMediaFile(fname, buff);

	FILE *fp=fopen(buff,"rb");
	if(fp == NULL)
	{
		return false;
	}
	else
	{
		fclose(fp);
		return true;
	}
}

//==================================================================================
void RenderCallback()
{
	// Physics code
	for (NxU32 i = 0; i < gMaxScenes; ++i)
	{
		if (gScenes[i] && !gPause)
		{
			gScenes[i]->setTiming(1.0f / 60.0f, 1, NX_TIMESTEP_FIXED);
			gScenes[i]->simulate(1.0f / 60.0f);
		}
	}

	for (NxU32 i = 0; i < gMaxScenes; ++i)
	{
		if (gScenes[i] && !gPause)
		{
			gScenes[i]->flushStream();
			gScenes[i]->fetchResults(NX_RIGID_BODY_FINISHED, true);
		}
	}

	//Check if a request was made to clear, save or load a scene
	if (gClear)
	{
		if (gScenes[gCurrentScene])
		{
			gPhysicsSDK->releaseScene(*gScenes[gCurrentScene]);
		}
		InitBlankScene();
		gClear = false;
	}
	else if (gSave)
	{
		char scratch[512];
		sprintf(scratch,"SceneSave%d", gCurrentScene );
		SaveScene(scratch);
		gSave = false;
	}
	else if (gLoad)
	{
		switch ( gLoad )
		{
			case 1:
				if ( 1 )
				{
					char scratch[512];
					sprintf(scratch,"SceneSave%d.nxb", gCurrentScene ); // load the binary saved version.
					LoadScene(scratch, NXU::FT_BINARY);
				}
				break;
			case 2:
				LoadScene("test.xml", NXU::FT_XML);
				break;
			case 3:
				LoadScene("test.dae", NXU::FT_COLLADA);
				break;
			case 4:
				if(FileExistTestSimple("testbin.nxb"))
				{
					LoadScene("testbin.nxb", NXU::FT_BINARY);
				}
				else
				{
					LoadScene("test.xml", NXU::FT_XML);
					SaveScene("testbin");
				}
				break;
		}
	}
	// ~Physics code

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	gluLookAt(gEye.x, gEye.y, gEye.z, gEye.x + gDir.x, gEye.y + gDir.y, gEye.z + gDir.z, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (NxU32 i = 0; i < gMaxScenes; ++i)
	{
		if (gScenes[i])
		{
			gDebugRenderer.renderData(*gScenes[i]->getDebugRenderable());
		}
		// Print profile results (if enabled)
		//gPrefsRenderer.render(gScene->readProfileData(true), glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}

	glutSwapBuffers();
}

//==================================================================================
void ReshapeCallback(int width, int height)
{
	glViewport(0, 0, width, height);
}

//==================================================================================
void IdleCallback()
{
	glutPostRedisplay();
}

//==================================================================================
int main(int argc, char **argv)
{
	printf("Press the keys w, space, s, b, and t to create various things.\n");
	printf("Press 1 to save the current scene and 3 to load from file to the current scene.\n");
	printf("Press c to clear the current scene.\n");
	printf("Press TAB to select the next scene.\n");
	printf("Use the arrow keys or 2, 4, 6 and 8 or d, f, e and g to move around.\nUse the mouse to look around.\n");
	printf("Press p to pause the simulation.\n");
	printf("Press X to load 'test.xml'\n");
	printf("Press Y to load 'test.dae'\n");
	printf("Press Z to load 'testbin.nxb'\n");

#if defined(_XBOX) || defined(__CELLOS_LV2__)
	glutRemapButtonExt(8, '1', false); // Left shoulder to save
	glutRemapButtonExt(9, 'c', false); // Right shoulder to clear
	glutRemapButtonExt(8, '3', true); // Shift + left shoulder to load
#endif
	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("SampleSceneExport");
	glutSetWindow(gMainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(ArrowKeyCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0, 0);

	// Setup default render states
	glClearColor(0.3f, 0.4f, 0.5f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);

	// Setup lighting
	glEnable(GL_LIGHTING);
	float AmbientColor[] = 
	{
		0.0f, 0.1f, 0.2f, 0.0f
	};
	float DiffuseColor[] = 
	{
		1.0f, 1.0f, 1.0f, 0.0f
	};
	float SpecularColor[] = 
	{
		0.0f, 0.0f, 0.0f, 0.0f
	};
	float Position[] = 
	{
		100.0f, 100.0f, 400.0f, 1.0f
	};
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ReleaseNx);
		glutMainLoop();
	}

	return 0;
}

