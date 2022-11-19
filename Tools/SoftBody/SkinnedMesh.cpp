#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "SkinnedMesh.h"
#include "TetraGraphics.h"
#include "NXU_helper.h"
#include "NXU_schema.h"
#include "SoftMesh.h"
#include "SoftSkeleton.h"
#include "NXU_GraphicsMesh.h"
#include "SoftBody.h"

namespace SOFTBODY
{

static void fm_identity(float *matrix) // set 4x4 matrix to identity.
{
	matrix[0*4+0] = 1;	matrix[1*4+1] = 1;	matrix[2*4+2] = 1;	matrix[3*4+3] = 1;
	matrix[1*4+0] = 0;	matrix[2*4+0] = 0;	matrix[3*4+0] = 0;
	matrix[0*4+1] = 0;	matrix[2*4+1] = 0;	matrix[3*4+1] = 0;
	matrix[0*4+2] = 0;	matrix[1*4+2] = 0;	matrix[3*4+2] = 0;
	matrix[0*4+3] = 0;	matrix[1*4+3] = 0;	matrix[2*4+3] = 0;

}

static void getMatrix(const NxMat34 &pose,float *matrix)
{
  fm_identity(matrix);
 	pose.M.getColumnMajorStride4(matrix);
	matrix[12] = pose.t.x;
	matrix[13] = pose.t.y;
	matrix[14] = pose.t.z;
}

class SkinnedMesh
{
public:

  SkinnedMesh(const char *fname)
  {
  	strncpy(mName,fname,512);
  	if ( gFileInterface )
			fname = gFileInterface->getSoftFileName(fname,true);

		NXU::NXU_FileType type = NXU::FT_XML;
		if ( stristr(fname,".nxb") ) type = NXU::FT_BINARY;
		if ( stristr(fname,".dae") ) type = NXU::FT_COLLADA;

		mCollection = NXU::loadCollection(fname,type);

  	mGraphicsMesh[0] = 0;
  	mSoftMesh = 0;

		if ( mCollection )
		{
			if ( mCollection->mScenes.size() )
			{
				NXU::NxSceneDesc *sd = mCollection->mScenes[0];
		    NxU32 count;
				const char **props = NXU::parseUserProperties(sd->mUserProperties,count);
				if ( count )
				{
					for (NxU32 i=0; i<count; i++)
					{
						const char *key   = props[i*2];
						const char *value = props[i*2+1];

						if ( stricmp(key,"graphics_mesh") == 0  )
						{
							strncpy(mGraphicsMesh,value,512);
							NxU32 acount = sd->mActors.size();
							if ( acount )
							{

  							NXU::NxuSkeleton *skeleton = new NXU::NxuSkeleton(acount);

  							for (NxU32 i=0; i<acount; i++)
  							{
  								NXU::NxActorDesc *ad = sd->mActors[i];
  								NXU::NxuBone b;

									float matrix[16];
                  getMatrix(ad->globalPose, matrix );

									b.Set(ad->name,-1,matrix);

									skeleton->SetNxuBone(i,b);
								}

							  mSoftMesh = createSoftMeshSystem(mGraphicsMesh,skeleton);

							  delete skeleton;

							}
							break;
						}
					}
				}
			}
		}
  }

  ~SkinnedMesh(void)
  {
  	NXU::releaseCollection(mCollection);
  	releaseSoftMeshSystem(mSoftMesh);
  }

	NXU::NxuPhysicsCollection * getPhysicsCollection(void)
  {
    return mCollection;
  };



	void render(const float **transforms)
	{
		if ( mSoftMesh )
		{
		  renderSoftMeshSystem(mSoftMesh,transforms);
		}
	}

	NXU::NxuSkeleton * getSkeleton(void)
	{
		NXU::NxuSkeleton *ret = 0;

	  if ( mSoftMesh )
	  {
			ret = SOFTBODY::getSkeleton(mSoftMesh);
	  }

    return ret;
	}

	void onDeviceReset(void *device)
	{
		if ( mSoftMesh )
		{
			SOFTBODY::onDeviceReset(mSoftMesh,device);
		}
	}

  bool matchSkinnedMesh(const char *name) const
  {
    return strcmp(name,mName) == 0;
  }

	bool hasSoftMesh(void) const
	{
		bool ret = false;
		if ( mSoftMesh ) ret = true;
		return ret;
	}

private:
  char											 mName[512];
	char                       mGraphicsMesh[512];
	SoftMeshSystem            *mSoftMesh; // the renderable graphics mesh.
  NXU::NxuPhysicsCollection	*mCollection; // the nxustream physics collection
};


class ActorExtra
{
public:

  ActorExtra(NxActor *a,SoftMeshSystem *graphicsMesh)
  {
  	mActor = a;
  	mGraphicsMesh = graphicsMesh;
  }

	~ActorExtra(void)
	{
		if ( mActor )
		{
  		NxScene &s = mActor->getScene();
	  	s.releaseActor(*mActor);
	  }
	}

	void addShape(SoftMeshSystem *sm)
	{
		mShapes.push_back(sm);
	}

  NxActor                    *mActor;
  SoftMeshSystem	           *mGraphicsMesh;
  NxArray< SoftMeshSystem * > mShapes;
};


class SkinnedMeshInstance : public NXU_userNotify
{
public:

  SkinnedMeshInstance(SkinnedMesh *model,NxScene *scene,const NxMat34 *pose)
  {
  	mModel        = model;
  	mScene        = scene;
  	mSoftSkeleton = 0;

		NXU::NxuPhysicsCollection *c = mModel->getPhysicsCollection();

		if ( c && scene )
		{

			NXU::instantiateCollection(c,scene->getPhysicsSDK(),scene,pose,this);

			if ( model->hasSoftMesh() )
			{
				NXU::NxuSkeleton *skel = mModel->getSkeleton();
				if ( skel )
				{
					NxArray< NxActor * > alist;
					for (NxU32 i=0; i<mActors.size(); i++)
					{
						alist.push_back( mActors[i]->mActor );
					}
  	  		mSoftSkeleton = createSoftSkeleton(alist.size(),&alist[0],skel);
				}
			}
		}
  }

  ~SkinnedMeshInstance(void)
  {
  	for (NxU32 i=0; i<mJoints.size(); i++)
  	{
  		NxJoint *j = mJoints[i];
  		mScene->releaseJoint(*j);
  	}

  	for (NxU32 i=0; i<mActors.size(); i++)
  	{
			ActorExtra *a = mActors[i];
  		delete a;
  	}

  	releaseSoftSkeleton(mSoftSkeleton);
  }

	void render(void)
	{
		if ( mModel )
		{
			const float **transforms = 0;
			if ( mSoftSkeleton && !gSoftBodySystem->hasShowTetrahedra() )
			{
				transforms = getTransforms(mSoftSkeleton);
   			mModel->render(transforms);
			}
			else
			{
        for (NxU32 i=0; i<mActors.size(); i++)
        {
          ActorExtra *ae = mActors[i];
          NxActor *a = ae->mActor;

          if ( ae->mGraphicsMesh && !gSoftBodySystem->hasShowTetrahedra() )
          {
            float matrix[16];
            getMatrix(a->getGlobalPose(),matrix);
						renderSoftMeshSystem(ae->mGraphicsMesh,matrix);
          }
          else
          {
            NxU32 nbShapes = a->getNbShapes();
            if ( nbShapes == ae->mShapes.size() )
            {
              NxShape *const* slist = a->getShapes();
              for (NxU32 i=0; i<nbShapes; i++)
              {
                NxShape *s = slist[i];
                SoftMeshSystem *mesh = ae->mShapes[i];
                if ( mesh )
                {
                  float matrix[16];
                  getMatrix(s->getGlobalPose(),matrix);
									renderSoftMeshSystem(mesh,matrix);
                }
              }
            }
          }
        }
			}
		}
	}


	void	NXU_notifyScene(NxU32 sno,	NxScene	*scene,	const	char *userProperties)
  {
    mScene = scene;
  };

	void	NXU_notifyJoint(NxJoint	*joint,	const	char *userProperties)
  {
  	mJoints.push_back(joint);
  };

	void	NXU_notifySpringAndDamperEffector(NxSpringAndDamperEffector	*effector,	const	char *userProperties)
  {
  }

	void	NXU_notifyActor(NxActor	*actor,	const	char *userProperties)
  {
		SoftMeshSystem *graphicsMesh = 0;
		const char *gmesh = NXU::getKeyValue(userProperties,"graphics_mesh");
		if ( gmesh )
		{
			graphicsMesh = locateSoftMeshSystem(gmesh);
		}

  	ActorExtra *ae = new ActorExtra(actor,graphicsMesh);

  	mActors.push_back(ae);
  	NxU32 scount = actor->getNbShapes();
  	if ( scount )
  	{
			NxShape *const*slist = actor->getShapes();
  		for (NxU32 i=0; i<scount; i++)
  		{
  			NxShape *shape = slist[i];
  			SoftMeshSystem *sms = sm_locateRenderContext(shape);
  			ae->addShape(sms);
  		}
  	}
  }

	void	NXU_notifyCloth(NxCloth	*cloth,	const	char *userProperties){};

	void	NXU_notifySoftBody(NxSoftBody	*softBody,	const	char *userProperties)
  {
    gSoftBodySystem->NXU_notifySoftBody(softBody,userProperties);
  }

	void	NXU_notifyFluid(NxFluid	*fluid,	const	char *userProperties){};
 	void 	NXU_notifyTriangleMesh(NxTriangleMesh *t,const char *userProperties){};
 	void 	NXU_notifyConvexMesh(NxConvexMesh *c,const char *userProperties){};
 	void 	NXU_notifyClothMesh(NxClothMesh *t,const char *userProperties){};

 	void 	NXU_notifySoftBodyMesh(NxSoftBodyMesh *t,const char *userProperties)
  {
    gSoftBodySystem->NXU_notifySoftBodyMesh(t,userProperties);
  }

 	void 	NXU_notifyCCDSkeleton(NxCCDSkeleton *t,const char *userProperties){};
	void 	NXU_notifyHeightField(NxHeightField *t,const char *userProperties){};

	void	NXU_notifySaveActor(NxActor *pActor, const char** pUserProperties){};

	NxScene *NXU_preNotifyScene(unsigned	int	sno, NxSceneDesc &scene, const char	*userProperties)
  {
    return	mScene;
  };

	bool	NXU_preNotifyJoint(NxJointDesc &joint, const char	*userProperties) { return	true; };

	bool	NXU_preNotifySpringAndDamperEffector(NxSpringAndDamperEffectorDesc &effector, const char	*userProperties) { return	true; };

	bool	NXU_preNotifyActor(NxActorDesc &actor, const char	*userProperties)
  {
		for (NxU32 i=0; i<actor.shapes.size(); i++)
		{
			NxShapeDesc *s = actor.shapes[i];
			s->shapeFlags|=NX_SF_VISUALIZATION; // make sure the shape visualization flags are on so we can see things!
		}
    return	true;
  };

	bool	NXU_preNotifyCloth(NxClothDesc &cloth, const char	*userProperties) { return	false;};

	bool	NXU_preNotifySoftBody(NxSoftBodyDesc &softBody, const char	*userProperties)
  {
    return gSoftBodySystem->NXU_preNotifySoftBody(softBody,userProperties);
  };

	bool	NXU_preNotifyFluid(NxFluidDesc &fluid, const char	*userProperties) { return	false;};
	bool 	NXU_preNotifyTriangleMesh(NxTriangleMeshDesc &t,const char *userProperties) { return true;	};
	bool 	NXU_preNotifyConvexMesh(NxConvexMeshDesc &t,const char *userProperties)	{	return true; };
	bool 	NXU_preNotifyClothMesh(NxClothMeshDesc &t,const char *userProperties) { return false; };

	bool 	NXU_preNotifySoftBodyMesh(NxSoftBodyMeshDesc &t,const char *userProperties)
  {
    return gSoftBodySystem->NXU_preNotifySoftBodyMesh(t,userProperties);
  };

	bool 	NXU_preNotifyCCDSkeleton(NxSimpleTriangleMesh &t,const char *userProperties)	{	return true; };
	bool 	NXU_preNotifyHeightField(NxHeightFieldDesc &t,const char *userProperties) {	return false; }
	bool 	NXU_preNotifySceneInstance(const char *name,const char *sceneName,const char *userProperties,NxMat34 &rootNode,NxU32 depth) { return true; };

	void  NXU_notifySceneFailed(unsigned	int	sno, NxSceneDesc &scene, const char	*userProperties) { };
	void	NXU_notifyJointFailed(NxJointDesc &joint, const char	*userProperties) {  };
	void	NXU_notifySpringAndDamperEffectorFailed(NxSpringAndDamperEffectorDesc &effector, const char	*userProperties) {  };
	void	NXU_notifyActorFailed(NxActorDesc &actor, const char	*userProperties) { };
	void	NXU_notifyClothFailed(NxClothDesc &cloth, const char	*userProperties) { };
	void	NXU_notifySoftBodyFailed(NxSoftBodyDesc &softBody, const char	*userProperties) { };
	void	NXU_notifyFluidFailed(NxFluidDesc &fluid, const char	*userProperties) { };
	void 	NXU_notifyTriangleMeshFailed(NxTriangleMeshDesc &t,const char *userProperties) {	};
	void 	NXU_notifyConvexMeshFailed(NxConvexMeshDesc &t,const char *userProperties)	{	 };
	void 	NXU_notifyClothMeshFailed(NxClothMeshDesc &t,const char *userProperties) { };

	void 	NXU_notifySoftBodyMeshFailed(NxSoftBodyMeshDesc &t,const char *userProperties)
  {
    gSoftBodySystem->NXU_notifySoftBodyMeshFailed(t,userProperties);
  };

	void 	NXU_notifyCCDSkeletonFailed(NxSimpleTriangleMesh &t,const char *userProperties)	{	 };
	void 	NXU_notifyHeightFieldFailed(NxHeightFieldDesc &t,const char *userProperties) {	}

private:
  SkinnedMesh		         *mModel;
  NxScene                *mScene;
  NxArray< ActorExtra * > mActors;
  NxArray< NxJoint * >    mJoints;
  SoftSkeleton           *mSoftSkeleton;
};

SkinnedMesh *sm_CreateSkinnedMesh(const char *fname)
{
	SkinnedMesh *ret = 0;

	ret = new SkinnedMesh(fname);

  return ret;
}

SkinnedMeshInstance  *sm_CreateSkinnedMeshInstance(SkinnedMesh *model,NxScene *scene,const NxMat34 *rootNode)
{
	SkinnedMeshInstance *ret = 0;

	ret = new SkinnedMeshInstance(model,scene,rootNode);

  return ret;
}
void           sm_ReleaseSkinnedMeshInstance(SkinnedMeshInstance *fb)
{
	delete fb;
}

void           sm_ReleaseSkinnedMesh(SkinnedMesh *fm)
{
	delete fm;
}

void           sm_RenderSkinnedMeshInstance(SkinnedMeshInstance *fb)
{
	fb->render();
}


bool           sm_MatchSkinnedMesh(const SkinnedMesh *model,const char *name) // true if these are the same names.
{
	bool ret = false;

  if ( model )
  {
    ret = model->matchSkinnedMesh(name);
  }

  return ret;
}

void           sm_onDeviceReset(SkinnedMesh *fsm,void *device)
{
	if ( fsm )
	{
		fsm->onDeviceReset(device);
	}
}

SoftMeshSystem *   sm_locateRenderContext(NxShape *shape) // builds a rendering context for a shape of this type.
{
	SoftMeshSystem *ret = 0;


  switch ( shape->getType() )
  {
	  case NX_SHAPE_MESH:
		  if ( 1 )
		  {
			  NxTriangleMeshShape *tmesh = shape->isTriangleMesh();
			  NxTriangleMeshShapeDesc desc;
			  tmesh->saveToDesc(desc);
			  NxTriangleMesh *meshData = desc.meshData;
			  if ( meshData )
			  {
			  	char scratch[512];
			  	sprintf(scratch,"NxTriangleMesh%08X", meshData );
					NxTriangleMeshDesc desc;
					meshData->saveToDesc(desc);

					NxArray< NxVec3 > vertices;
					NxArray< NxU32  > indices;
					const char *scan = (const char *) desc.points;
					for (NxU32 i=0; i<desc.numVertices; i++)
					{
						const NxVec3 *source = (const NxVec3 *) scan;
						vertices.push_back( *source );
						scan+=desc.pointStrideBytes;
					}
					scan = (const char *) desc.triangles;
					for (NxU32 i=0; i<desc.numTriangles; i++)
					{
						if ( desc.flags & NX_CF_16_BIT_INDICES )
						{
							const NxU16 *source = (const NxU16 *) scan;
							indices.push_back( source[0] );
							indices.push_back( source[1] );
							indices.push_back( source[2] );
						}
						else
						{
							const NxU32 *source = (const NxU32 *) scan;
							indices.push_back( source[0] );
							indices.push_back( source[1] );
							indices.push_back( source[2] );
						}
						scan+=desc.triangleStrideBytes;
					}
					ret = locateTriangleMesh(scratch,"wood.dds", vertices.size(), &vertices[0].x, indices.size()/3, &indices[0] );
        }
      }
			break;
	  case NX_SHAPE_CONVEX:
  		if ( 1 )
			{
				NxConvexShape *convex = shape->isConvexMesh();
				NxConvexShapeDesc desc;
				convex->saveToDesc(desc);
				NxConvexMesh *meshData = desc.meshData;
				if ( meshData )
				{
					char scratch[512];
					sprintf(scratch,"NxConvex%08X", meshData );

					NxConvexMeshDesc desc;
					meshData->saveToDesc(desc);

					NxArray< NxVec3 > vertices;
					NxArray< NxU32  > indices;
					const char *scan = (const char *) desc.points;
					for (NxU32 i=0; i<desc.numVertices; i++)
					{
						const NxVec3 *source = (const NxVec3 *) scan;
						vertices.push_back( *source );
						scan+=desc.pointStrideBytes;
					}
					scan = (const char *) desc.triangles;
					for (NxU32 i=0; i<desc.numTriangles; i++)
					{
						if ( desc.flags & NX_CF_16_BIT_INDICES )
						{
							const NxU16 *source = (const NxU16 *) scan;
							indices.push_back( source[0] );
							indices.push_back( source[1] );
							indices.push_back( source[2] );
						}
						else
						{
							const NxU32 *source = (const NxU32 *) scan;
							indices.push_back( source[0] );
							indices.push_back( source[1] );
							indices.push_back( source[2] );
						}
						scan+=desc.triangleStrideBytes;
					}
					ret = locateTriangleMesh(scratch,"wood.dds", vertices.size(), &vertices[0].x, indices.size()/3, &indices[0] );
				}
			}
			break;
		case NX_SHAPE_SPHERE:
			if ( 1 )
			{
				NxSphereShape *bs = shape->isSphere();
				if ( bs )
				{
					ret = locateSphere(0, bs->getRadius() );
				}
			}
			break;
		case NX_SHAPE_CAPSULE:
			if ( 1 )
			{
				NxCapsuleShape *bs = shape->isCapsule();
				if ( bs )
				{
					ret = locateCapsule(0, bs->getRadius(), bs->getHeight());
				}
			}
			break;
		case NX_SHAPE_BOX:
			if ( 1 )
			{
				NxBoxShape *bs = shape->isBox();
				if ( bs )
				{
					NxVec3 sides =  bs->getDimensions();
					ret = locateBox(0,&sides.x);
				}
			}
			break;
		case NX_SHAPE_PLANE:
			// No rendering context for a plane
			if ( 1 )
			{
				NxPlaneShape *p = shape->isPlane();
				if ( p )
				{
					NxPlane plane = p->getPlane();
					float pp[4];
					pp[0] = plane.normal.x;
					pp[1] = plane.normal.y;
					pp[2] = plane.normal.z;
					pp[3] = plane.d;

					ret = locatePlane(0,pp);
				}
			}
			break;
		default:
		  // needs to be implemented!!
		  break;
	}

  return ret;
}




};
