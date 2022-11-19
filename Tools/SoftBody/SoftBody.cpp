#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>


#include "NxPhysics.h"
#if NX_USE_SOFTBODY_API
#include "softbody/NxSoftBodyMeshDesc.h"
#include "softbody/NxSoftBodyMesh.h"
#include "softbody/NxSoftBodyDesc.h"
#include "softbody/NxSoftBody.h"
#endif

#include "SoftBody.h"
#include "SoftBodyMath.h" // utility functions
#include "TetraD3D.h"
#include "TetraMesh.h"
#include "NXU_cooking.h"
#include "NXU_streaming.h"
#include "NxTetra.h"
#include "SoftServe.h"
#include "SkinnedMesh.h"
#include "NXU_schema.h"
#include "NxMouseDrag.h"
#include "SoftMesh.h"

class SoftBodyDesc;
class SoftBodyMeshDesc;

SOFTBODY::SoftBodySystem *gSoftBodySystem=0;

namespace SOFTBODY
{

	const char *stristr(const char *str,const char *scan)
	{
		const char *ret = 0;
		if ( str )
		{
			static char scratch[512];
			strncpy(scratch,str,512);
			strlwr(scratch);
			const char *found = strstr(scratch,scan);
			if ( found )
			{
				int len = (int)(found-scratch);
				ret = str+len;
			}
		}
		return ret;
	}

#define NULL_INDEX 0xFFFFFFFF

class SoftBodyBuffer
{
public:
  SoftBodyBuffer(void)
  {
  	mNumVertices = 0;
  	mNumIndices  = 0;
  	mVertices    = 0;
  	mIndices     = 0;
  }

	~SoftBodyBuffer(void)
	{
		release();
	}

  void release(void)
  {
  	delete mVertices;
  	delete mIndices;
  	mVertices = 0;
  	mIndices  = 0;
  	mNumVertices = 0;
  	mNumIndices  = 0;
  }

	void init(NxU32 numVertices,NxU32 numTetrahedra)
	{
#if NX_USE_SOFTBODY_API
		mVertices = new NxVec3[numVertices];
  	mIndices  = new NxU32[numTetrahedra*4];
  	mNumVertices = 0;
  	mNumIndices  = 0;

		mMeshData.verticesPosBegin             = mVertices;
		mMeshData.verticesNormalBegin          = 0;
		mMeshData.verticesPosByteStride        = sizeof(NxVec3);
		mMeshData.verticesNormalByteStride     = 0;
		mMeshData.maxVertices                  = numVertices;
		mMeshData.numVerticesPtr               = &mNumVertices;
		mMeshData.indicesBegin                 = mIndices;
		mMeshData.indicesByteStride            = sizeof(NxU32);
		mMeshData.maxIndices                   = numTetrahedra*4;
		mMeshData.numIndicesPtr                = &mNumIndices;
		mMeshData.parentIndicesBegin           = 0;
		mMeshData.parentIndicesByteStride      = 0;
		mMeshData.maxParentIndices             = 0;
		mMeshData.numParentIndicesPtr          = 0;
		mMeshData.flags                        = 0;
#endif
	}

  NxMeshData       mMeshData;
	NxU32            mNumVertices;
	NxU32            mNumIndices;
	NxVec3        	*mVertices;
	NxU32           *mIndices;
};

	class SoftBody
	{
	public:

		SoftBody(TetraModel *model,const NxMat34 *gPose,NxScene *scene,NxSoftBodyDesc *desc,const char *nameid)
		{
			mStartPose.id();
			if ( gPose )
				mStartPose = *gPose;

			mScene    = scene;
			mSoftBody = 0;
			mModel    = model;
			strncpy(mName,nameid,512);
#if NX_USE_SOFTBODY_API
			if ( desc )
			{
				desc->name = mName;
				createSimulation(*desc);
			}
#endif
		}

		SoftBody(NxSoftBodyDesc &sd,TetraModel *model,const char *userProperties)
		{
#if NX_USE_SOFTBODY_API

			mStartPose = sd.globalPose;
			mSoftBody  = 0;
			mModel     = model;

			if ( sd.softBodyMesh )
			{
				NxSoftBodyMesh *sbm = sd.softBodyMesh;
				NxSoftBodyMeshDesc md;
				sbm->saveToDesc(md);

				if ( md.numVertices && md.numTetrahedra )
				{
					mBuffer.init(md.numVertices,md.numTetrahedra);
					sd.meshData = mBuffer.mMeshData;
				}
			}

#endif
		}

		void setSoftBody(NxSoftBody *sb)
		{
			mSoftBody = sb;
#if NX_USE_SOFTBODY_API
			mScene    = &sb->getScene();
#endif
		}

    bool isSimulating(void) const
    {
      bool ret = false;
      if ( mSoftBody ) ret = true;
      return ret;
    }


		void createSimulation(const NxSoftBodyDesc &type)
		{

			releaseSimulation();

#if NX_USE_SOFTBODY_API
			NxSoftBodyDesc sd = type;
  		sd.softBodyMesh                  = mModel->getSoftBodyMesh();
			sd.globalPose                    = mStartPose;
#endif

			mBuffer.release();

#if NX_USE_SOFTBODY_API

			if ( sd.softBodyMesh )
			{
				NxSoftBodyMesh *sbm = sd.softBodyMesh;
				NxSoftBodyMeshDesc md;
				sbm->saveToDesc(md);

				if ( md.numVertices && md.numTetrahedra )
				{
					mBuffer.init(md.numVertices,md.numTetrahedra);
					sd.meshData = mBuffer.mMeshData;
				}
				bool ok = sd.isValid();
				mSoftBody = mScene->createSoftBody(sd);
			}
#endif

		}

		~SoftBody(void)
		{
			releaseSimulation();
		}

		void releaseSimulation(void)
		{
			if ( mSoftBody )
			{
#if NX_USE_SOFTBODY_API
				mScene->releaseSoftBody(*mSoftBody);
#endif
				mSoftBody = 0;
			}
			mBuffer.release();
		}

		bool releaseSoftBody(TetraModel *tm)
		{
			bool ret = false;

			if ( tm == mModel )
			{
				if ( mSoftBody )
				{
#if NX_USE_SOFTBODY_API
					mScene->releaseSoftBody(*mSoftBody);
#endif
					mSoftBody = 0;
				}

				mBuffer.release();

				ret = true;
			}
			return ret;
		}

		NxSoftBody * getNxSoftBody(void) const { return mSoftBody; };

		void render(bool showBodies,bool showOverlay,bool showGraphicsMesh)
		{

			SoftBodyBuffer &sb = mBuffer;

			if ( sb.mNumVertices && sb.mNumIndices )
			{
				if (showBodies )
				{
					TetraMesh *tmesh = mModel->getTetraMesh();
					if (tmesh)
						tmesh->render(sb.mVertices,sb.mIndices,sb.mNumVertices,sb.mNumIndices/4,false,showOverlay);
				}
				else
				{
					if ( showGraphicsMesh )
						mModel->render(sb.mVertices,sb.mIndices);

					if ( showOverlay )
					{
						TetraMesh *tmesh = mModel->getTetraMesh();
						if (tmesh)
							tmesh->render(sb.mVertices,sb.mIndices,sb.mNumVertices,sb.mNumIndices/4,false,true);
					}

				}
			}
			else
			{
				if (showBodies )
				{
					TetraMesh *tmesh = mModel->getTetraMesh();
					if (tmesh)
						tmesh->render(showOverlay);
				}
				else
				{
					if ( showOverlay )
					{
						TetraMesh *tmesh = mModel->getTetraMesh();
						if (tmesh)
							tmesh->render(true);
					}
					if (showGraphicsMesh )
						mModel->render();
				}
			}
		}

		NxScene     *mScene;
		NxSoftBody  *mSoftBody; // the instance on the SDK.
		TetraModel  *mModel;
		SoftBodyBuffer mBuffer;
		NxMat34      mStartPose;
		char         mName[512];
	};

	SoftBodySystem::SoftBodySystem(NxScene *scene)
	{
    mHardwareOk = false;
		mActiveState = false;
		assert( gSoftBodySystem == 0 );
    mLastEditCount = mEditCount = 0;
    mDtime = 1.0f / 60.0f;
    mDebugVisualization = false;
		mPaused = false;
		mResetScene = false;
		mLastLeftButton = false;
		mLastRightButton = false;
    mMouseDrag = 0;
    mIsoSingle = false;
    mMouseX = 0;
    mMouseY = 0;
    mXrot = 0;
    mYrot = 0;

		gSoftBodySystem = this;
		mTetraD3D = new TetraD3D;
		gGraphicsInterface = mTetraD3D;
		mEdit = 0;
		mNotify = 0;
		mScene = 0;
		mRaycastOk = true;
		mCenter.set(0,0,0);
    mRadius = 4.0f;

		mTetraMaker = 0;
		mDetailLevel = 0.5f;
		mSubdivisionLevel = 20;
		mSourceMesh = 0;
		mSurfaceMesh = 0;
		mTetraMesh = 0;
		mShowTetrahedra = false;
		mShowOverlay    = false;
		mShowGraphicsMesh = true;
		mLastX = -1;
		mLastY = -1;
		mHitIndex = NULL_INDEX;
		mLastFileName[0] = 0;
		NxMat34 mat;
		mat.id();
		mat.getColumnMajor44(mWorld); // convert to 4x4 render matrix
		setScene(scene);
#if NX_USE_SOFTBODY_API
    mSoftBodyDesc.flags|=NX_SBF_VISUALIZATION;
#endif
    mBounds.min.set(-8,0,-8);
    mBounds.max.set(8,8,8);
	}

	SoftBodySystem::~SoftBodySystem(void)
	{

		release(true);

		if ( mTetraMaker )
		{
			if ( mSourceMesh )  
			{
				mTetraMaker->releaseTetraMesh(*mSourceMesh);
				delete mSourceMesh;
			}
			if ( mSurfaceMesh ) 
			{
				mTetraMaker->releaseTetraMesh(*mSurfaceMesh);
				delete mSurfaceMesh;
			}
			if ( mTetraMesh )   
			{
				mTetraMaker->releaseTetraMesh(*mTetraMesh);
				delete mTetraMesh;
			}
			mSourceMesh = mSurfaceMesh = mTetraMesh = 0;
		}

    releaseSoftMeshSystems();

		delete mTetraD3D;
		assert( gSoftBodySystem == this );
		gSoftBodySystem = 0;
	}

	void SoftBodySystem::release(bool sdk)
	{
    mHitIndex = NULL_INDEX;

    releaseMouseDrag(mMouseDrag);
    mMouseDrag = 0;

		clearEditing();

		onDeviceReset(0);

		for (NxU32 i=0; i<mSoftBodies.size(); i++)
		{
			SoftBody *sb = mSoftBodies[i];
			delete sb;
		}

		for (NxU32 i=0; i<mSkinnedSoftBodies.size(); i++)
		{
			sm_ReleaseSkinnedMeshInstance( mSkinnedSoftBodies[i] );
		}

		if ( sdk )
		{
#if NX_USE_SOFTBODY_API
			for (NxU32 i=0; i<mTetraModels.size(); i++)
			{
				TetraModel *tm = mTetraModels[i];
				NxSoftBodyMesh *mesh = tm->getSoftBodyMesh();
				assert(mScene);
				if ( mesh && mScene )
				{
					NxPhysicsSDK &p = mScene->getPhysicsSDK();
					p.releaseSoftBodyMesh(*mesh);
				}
				tm->setSoftBodyMesh(0);
				delete tm;
			}
			mTetraModels.clear();
#endif

			for (NxU32 i=0; i<mSkinnedMeshs.size(); i++)
			{
				sm_ReleaseSkinnedMesh( mSkinnedMeshs[i] );
			}

			mSkinnedMeshs.clear();

		}

		mSoftBodies.clear();
		mSkinnedSoftBodies.clear();

		mEdit = 0;
	}


	SoftBody *   SoftBodySystem::createSoftBody(const char *fname,const NxMat34 *globalPose,const char *nameid)
	{
		SoftBody *ret = 0;

		clearEditing();

		if ( stristr(fname,".xml") || stristr(fname,".nxb") || stristr(fname,".dae") )
		{
			loadNxuStream(fname,globalPose);
		}
		else
		{
			const char *tet = 0;
			char scratch[512];
			strncpy(scratch,fname,512);
			char *dot = (char *) stristr(scratch,".obj");
			if ( dot )
			{
				*dot = 0;
				strcat(scratch,".tet");
				tet = scratch;
			}

			TetraModel *tm = locateTetraModel(fname,tet);

			if ( tm )
			{
        char scratch[512];
				if ( nameid == 0 )
				{
					sprintf(scratch,"SoftBody%d", mSoftBodies.size()+1);
					nameid = scratch;
				}
#if NX_USE_SOFTBODY_API
				ret = new SoftBody(tm,globalPose,mScene,&mSoftBodyDesc,nameid);
#else
				ret = new SoftBody(tm,globalPose,mScene,0,nameid);
#endif

				mEdit = ret;


        mBounds = tm->mGraphicsBounds;

				mSoftBodies.push_back(ret);
			}
		}
		return ret;
	}

	SoftBody *   SoftBodySystem::createSoftBody(TetraModel *tm,const NxMat34 *globalPose)
	{
		SoftBody *ret = 0;
#if NX_USE_SOFTBODY_API
		clearEditing();
		if ( tm )
		{
			char scratch[512];
			sprintf(scratch,"SoftBody%d", mSoftBodies.size()+1 );
			ret = new SoftBody(tm,globalPose,mScene,&mSoftBodyDesc,scratch);
			mEdit = ret;
			mSoftBodies.push_back(ret);
		}
#endif
		return ret;
	}

	NxSoftBody * SoftBodySystem::getNxSoftBody(SoftBody *body)
	{
		NxSoftBody *ret = 0;

		if ( body )
			ret = body->getNxSoftBody();

		return ret;
	}

	bool         SoftBodySystem::releaseSoftBody(SoftBody *body)
	{
		bool ret = false;

		clearEditing();

		if ( body )
		{
			NxArray< SoftBody *>::iterator i;
			for (i=mSoftBodies.begin(); i!=mSoftBodies.end(); i++)
			{
				if ( body == (*i) )
				{
					delete body;
					mSoftBodies.erase(i);
					ret = true;
					break;
				}
			}
		}

		return false;
	}

	void         SoftBodySystem::setDevice(void *d3device,void *hwnd) // set the d3d device
	{
		gGraphicsInterface->setDevice(d3device,hwnd);
	}

	void         SoftBodySystem::onDeviceReset(void *d3device)
	{
		for (NxU32 i=0; i<mSkinnedMeshs.size(); i++)
		{
			SkinnedMesh *fsm = mSkinnedMeshs[i];
			sm_onDeviceReset(fsm,d3device);
		}
		for (NxU32 i=0; i<mTetraModels.size(); i++)
		{
			TetraModel *tm = mTetraModels[i];
			tm->onDeviceReset();
		}
		gGraphicsInterface->onDeviceReset(d3device);
    sm_onDeviceReset(d3device);
	}

	static void initV(TetraGraphicsVertex &v,const float *p)
	{
		v.mPos[0] = p[0];
		v.mPos[1] = p[1];
		v.mPos[2] = p[2];
		v.mNormal[0] = 0;
		v.mNormal[1] = 1;
		v.mNormal[2] = 0;
		v.mTexel[0] = 0;
		v.mTexel[1] = 0;
	}

	void         SoftBodySystem::render(const void *view,const void *projection,bool _drawGrid,float dtime) // render all of the soft bodies
	{
    mDtime = dtime;


    memcpy(mViewMatrix,view,sizeof(float)*16);
    memcpy(mProjectionMatrix,projection,sizeof(float)*16);

		gGraphicsInterface->setViewProjectionMatrix(view,projection);

    if ( _drawGrid )
      drawGrid();

		gGraphicsInterface->setWorldMatrix(mWorld);


		for (NxU32 i=0; i<mSoftBodies.size(); i++)
		{
			SoftBody *sb = mSoftBodies[i];
			sb->render(mShowTetrahedra,mShowOverlay,mShowGraphicsMesh);
		}

		for (NxU32 i=0; i<mSkinnedSoftBodies.size(); i++)
		{
			SkinnedMeshInstance *fsb = mSkinnedSoftBodies[i];
			sm_RenderSkinnedMeshInstance(fsb);
		}

		if ( mSurfaceMesh )
		{
			const float *positions = mSurfaceMesh->mVertices;
			const unsigned int *idx = mSurfaceMesh->mIndices;
			unsigned int vcount = mSurfaceMesh->mVcount;

			unsigned int lcolor = 0xFF8080FF;

			for (NxU32 i=0; i<mSurfaceMesh->mTcount; i++)
			{
				NxU32 i1 = *idx++;
				NxU32 i2 = *idx++;
				NxU32 i3 = *idx++;

				const float *p1 = &positions[i1*3];
				const float *p2 = &positions[i2*3];
				const float *p3 = &positions[i3*3];

				gGraphicsInterface->addLine(lcolor,p1,p2);
				gGraphicsInterface->addLine(lcolor,p2,p3);
				gGraphicsInterface->addLine(lcolor,p3,p1);

			}
		}

		if (mTetraMesh && mHitIndex != NULL_INDEX )
		{
			const unsigned int *indices = mTetraMesh->mIndices;
			const float *positions  = mTetraMesh->mVertices;

			unsigned int i1 = indices[mHitIndex*4+0];
			unsigned int i2 = indices[mHitIndex*4+1];
			unsigned int i3 = indices[mHitIndex*4+2];
			unsigned int i4 = indices[mHitIndex*4+3];

			NxVec3 p1(&positions[i1*3]);
			NxVec3 p2(&positions[i2*3]);
			NxVec3 p3(&positions[i3*3]);
			NxVec3 p4(&positions[i4*3]);

			gGraphicsInterface->addLine(0xFFFFFF00, &p1.x, &p2.x );
			gGraphicsInterface->addLine(0xFFFFFF00, &p2.x, &p3.x );
			gGraphicsInterface->addLine(0xFFFFFF00, &p3.x, &p1.x );
			gGraphicsInterface->addLine(0xFFFFFF00, &p1.x, &p4.x );
			gGraphicsInterface->addLine(0xFFFFFF00, &p2.x, &p4.x );
			gGraphicsInterface->addLine(0xFFFFFF00, &p3.x, &p4.x );

		}

		gGraphicsInterface->flushLines();

		if ( mDebugVisualization && mScene && gFileInterface )
		{
			gFileInterface->fetchResults();
			NxMat34 wm;
    	wm.id();
    	float world[16];
			wm.getColumnMajor44(world); // convert to 4x4 render matrix
    	gGraphicsInterface->setWorldMatrix(world);


		  if ( mScene->getDebugRenderable() )
		  {
	 		  renderData(*mScene->getDebugRenderable());
		  }


			gGraphicsInterface->flushLines();
		}

	}


	TetraModel * SoftBodySystem::locateTetraModel(const char *obj,const char *tet)
	{
		TetraModel *ret = 0;

		for (NxU32 i=0; i<mTetraModels.size(); i++)
		{
			TetraModel *tm = mTetraModels[i];
			if ( tm->isMatch(obj) )
			{
				ret = tm;
				break;
			}
		}

		if ( ret == 0 )
		{
			ret = new TetraModel(obj,tet);
			if ( ret->isOk() && mScene )
			{
				rebuildSoftBodyMesh(*ret);
				mTetraModels.push_back(ret);
			}
			else
			{
				delete ret;
				ret = 0;
			}
		}
		return ret;
	}

	TetraGraphicsInterface *SoftBodySystem::getTetraGraphicsInterface(void)
	{
		return gGraphicsInterface;
	}

	void   SoftBodySystem::setSoftFileInterface(SoftFileInterface *iface)
	{
		gFileInterface = iface;
	}

	SoftFileInterface * SoftBodySystem::getSoftFileInterface(void) const
	{
		return gFileInterface;
	}

	// Data driven API.  Convienent to hook up to a scripting system.
	//                   Supports tweaking soft body settings in real-time as well
	//                   as authoring tetrahedral meshes.
	int  SoftBodySystem::processCommand(SoftBodyCommand sbc,int argc,const char **argv)
	{
		int ret = 0;

		bool createSimulation = false;

		if ( gFileInterface )
			gFileInterface->fetchResults();

		switch ( sbc )
		{
			case SBC_EXPORT_SURFACE:
				if ( mSurfaceMesh )
				{
					const char *fname = 0;
					if ( argc >= 2 ) fname = argv[1];
					if ( fname == 0 && gFileInterface )
						fname = gFileInterface->getSaveFileName(".obj","surface.obj","Export Surface as Wavefront OBJ");
					if ( fname )
					{
						FILE *fph = fopen(fname,"wb");
						if ( fph )
						{
							for (NxU32 i=0; i<mSurfaceMesh->mVcount; i++)
							{
								const float *v = &mSurfaceMesh->mVertices[i*3];
								fprintf(fph,"v %0.9f %0.9f %0.9f\r\n", v[0], v[1], v[2] );
							}
							for (NxU32 i=0; i<mSurfaceMesh->mTcount; i++)
							{
								const unsigned int *idx = &mSurfaceMesh->mIndices[i*3];
								fprintf(fph,"f %d %d %d\r\n", idx[0]+1, idx[1]+1, idx[2]+1 );
							}
							fclose(fph);
						}
						else
						{
							char scratch[512];
							sprintf(scratch,"Failed to open file '%s' for write access.\r\n", fname );
  					if ( gFileInterface )
	  					gFileInterface->softMessageReport(scratch);
						}
					}
				}
				else
				{
					if ( gFileInterface )
						gFileInterface->softMessageReport("No surface mesh has been generated that can be saved.\r\n");
				}
				break;
      case SBC_ISO_SINGLE:
        if ( argc >= 2 )
        {
          mIsoSingle = stricmp(argv[1],"true") == 0;
        }
        break;
			// Usage: SbcAttachSoftBody <softbodyName> <actorName>
			case SBC_ATTACH_SOFTBODY:
				if ( argc >= 3 )
				{
					attachSoftBody(argv[1],argv[2]);
				}
				break;
			//            0         1      2      3      4      5       6        7       8				9
			// Usage: SbcSoftBody <name> <posx> <posy> <posz> <quatx> <quaty> <quatz> <quatw> <nameid>
			case SBC_SOFTBODY:
				if ( argc >= 2 )
				{
					const char *fname = argv[1];
					const char *nameid = 0;
					NxVec3 pos(0,0,0);
					NxQuat quat;
					quat.id();
					if ( argc >= 5 )
					{
						pos.x = (float) atof(argv[2]);
						pos.y = (float) atof(argv[3]);
						pos.z = (float) atof(argv[4]);
						if ( argc >= 9 )
						{
							float x = (float) atof(argv[5]);
							float y = (float) atof(argv[6]);
							float z = (float) atof(argv[7]);
							float w = (float) atof(argv[8]);
							quat.setXYZW(x,y,z,w);
						}
						if ( argc >= 10 )
						{
							nameid = argv[9];
						}
					}
					NxMat34 mat;
					mat.M.fromQuat(quat);
					mat.t = pos;
					createSoftBody(fname,&mat,nameid);
				}
				break;
			case SBC_DEBUG_VISUALIZATION:
				if ( argc >= 2 )
				{
					mDebugVisualization = stricmp(argv[1],"true") == 0;
				}
				else
				{
					float vscale = 0.0f;
					if ( mDebugVisualization )
						mDebugVisualization = false;
					else
					{
						mDebugVisualization = true;
						vscale = 0.1f;
					}
					if ( mScene )
					{
			      mScene->getPhysicsSDK().setParameter(NX_VISUALIZATION_SCALE,vscale);
					}
				}
				break;
		case SBC_PAUSE_SIMULATION:
			if ( mPaused )
				mPaused = false;
			else
				mPaused = true;
			break;
		case SBC_RESET_SCENE:
	    releaseSoftMeshSystems();
			release(true);
      if ( gFileInterface )
      {
        NxScene *scene = gFileInterface->resetSDK(false);
        if ( scene )
        {
          setScene(scene);
        }
      }
			break;
		case SBC_CORE_DUMP_XML:
		case SBC_CORE_DUMP_NXB:
		case SBC_CORE_DUMP_DAE:
			if ( argc >= 1 && mScene )
			{
				const char *fname = 0;
				if ( argc >= 2 )
					fname = argv[1];
        coreDump(fname,sbc );

			}
			break;
		case SBC_EXPORT_OBJ:
			if ( argc >= 1 )
			{
				TetraModel *tm = getEditModel();
				if ( tm == 0 && argc >= 2 ) tm = locateTetraModel( argv[1], 0 );
				if ( tm )
				{
					char scratch1[512];
					char scratch2[512];
					concatFile(scratch1,tm->mObjName,"_SB.obj");
					const char *fname = scratch1;
					if ( gFileInterface )
						fname = gFileInterface->getSaveFileName(".obj",fname,"Export filename for Wavefront OBJ and TET format.");
					if ( fname )
					{
						strcpy(scratch2,fname);
						char *dot =	(char *)lastDot(scratch2);
						if ( dot )
						{
							*dot = 0;
							strcat(scratch2,".tet");
						}
					}

					tm->createLinks();

					saveSoftBodyObj(fname,scratch2,tm);

				}
			}
			break;
		case SBC_EXPORT_XML:
			if ( argc >= 1 )
			{
				TetraModel *tm = getEditModel();
				if ( tm == 0 && argc >= 2 ) locateTetraModel( argv[1], 0 );
				saveNxuStream(tm);

			}
			break;
		case SBC_EXPORT_EZM:
			if ( argc >= 1 )
			{
				TetraModel *tm = getEditModel();
				if ( tm == 0 &&  argc >= 2 ) tm = locateTetraModel( argv[1], 0 );
				if ( tm )
				{
					char scratch[512];
					concatFile(scratch,tm->mObjName,"_SB.ezm");
					const char *fname = scratch;
					if ( gFileInterface )
						fname = gFileInterface->getSaveFileName(".ezm",fname,"Export filename for EZ-Mesh");
					if ( fname )
						saveSoftBodyEZM(fname,tm);
				}
			}
			break;
		case SBC_SHOW_TETRAHEDRA:
			if ( argc >= 2 )
			{
			  bool state = stricmp(argv[1],"true") == 0;
			  setShowTetrahedra(state);
			}
			break;
		case SBC_SHOW_OVERLAY:
			if ( argc >= 2 ) mShowOverlay = stricmp(argv[1],"true") == 0;
			break;
		case SBC_SHOW_GRAPHICS_MESH:
			if ( argc >= 2 ) mShowGraphicsMesh = stricmp(argv[1],"true") == 0;
			break;
		case SBC_SHOW_BACKFACES:
			if ( argc >= 2 ) getTetraGraphicsInterface()->setShowBackfaces(stricmp(argv[1],"true") == 0);
			break;
		case SBC_RAYCAST_OK:
			if ( argc >= 2 ) mRaycastOk = stricmp(argv[1],"true") == 0;
			break;
		case SBC_COPY_SURFACE:
			if ( 1 )
			{
				TetraModel *tm = getEditModel();
				if ( tm == 0 && argc >= 2 )
					locateTetraModel( argv[1], 0 );
				copySurface(tm);
			}
			ret = 1;
			break;
		case SBC_ISO_SURFACE:
			if ( 1 )
			{
				TetraModel *tm = getEditModel();
				if ( tm == 0 && argc >= 2 )
					locateTetraModel( argv[1], 0 );
				copyIsoSurface(tm);
			}
			ret = 1;
			break;
		case SBC_GENERATE_VOLUME:
			if ( 1 )
			{
				TetraModel *tm = getEditModel();
				if ( tm == 0 && argc >= 2 ) locateTetraModel( argv[1], 0 );
				if ( mSurfaceMesh == 0 ) copyIsoSurface(tm);
				createTetrahedra(tm);
			}
			ret = 1;
			break;
		case SBC_SUBDIVISION_LEVEL:
			if ( argc >= 2 )
			{
				mSubdivisionLevel = atoi( argv[1]);
				if ( mSubdivisionLevel < 1 ) mSubdivisionLevel = 1;
				if ( mSubdivisionLevel > 100 ) mSubdivisionLevel = 100;
				if ( mTetraMaker )
					mTetraMaker->setSubdivisionLevel(mSubdivisionLevel);
			}
			ret = 1;
			break;
		case SBC_MESH_DETAIL:
			if ( argc >= 2 )
			{
				float d = (float) atof(argv[1]);
				mDetailLevel = d/100.0f;
			}
			ret = 1;
			break;
		case SBC_NEW_SOFTBODY:
			if ( mEdit )
			{
				createSoftBody(mEdit->mModel,0);
			}
			break;
		case SBC_IMPORT_OBJ:
		case SBC_IMPORT_PSK:
		case SBC_IMPORT_EZM:
		case SBC_IMPORT_XML:
		case SBC_IMPORT_NXB:
		case SBC_IMPORT_COLLADA:
		case SBC_IMPORT:
			if ( 1 )
			{
				if ( sbc == SBC_IMPORT )
				{
					if ( argc >= 2 )
					{
						if ( strstr(argv[1],".obj") ) sbc = SBC_IMPORT_OBJ;
						if ( strstr(argv[1],".psk") ) sbc = SBC_IMPORT_PSK;
						if ( strstr(argv[1],".ezm") ) sbc = SBC_IMPORT_EZM;
						if ( strstr(argv[1],".xml") ) sbc = SBC_IMPORT_XML;
						if ( strstr(argv[1],".nxb") ) sbc = SBC_IMPORT_NXB;
						if ( strstr(argv[1],".dae") ) sbc = SBC_IMPORT_COLLADA;
					}
					else
					{
						sbc = SBC_IMPORT_OBJ;
					}
				}
				const char *fname = 0;
				const char *extension = 0;
        const char *description = 0;
				switch ( sbc )
				{
				case SBC_IMPORT_OBJ:
					extension = ".obj";
          description = "Select Wavefront OBJ file to import";
					break;
				case SBC_IMPORT_PSK:
					extension = ".psk";
          description = "Select Unreal PSK to import";
					break;
				case SBC_IMPORT_EZM:
					extension = ".ezm";
          description = "Select EZ-mesh to import";
					break;
				case SBC_IMPORT_XML:
					extension = ".xml";
          description = "Select NxuStream XML file to import";
					break;
				case SBC_IMPORT_COLLADA:
					extension = ".dae";
          description = "Select COLLADA 1.4.1 file to import";
					break;
				case SBC_IMPORT_NXB:
					extension = ".nxb";
          description = "Select NxuStream NXB file to import";
					break;
				}
				if ( argc >= 2 )
				{
					fname = argv[1];
					if ( gFileInterface )
					{
						fname = gFileInterface->getSoftFileName(fname,true);
					}
				}
				if ( gFileInterface && fname == 0 )
				{
					fname = gFileInterface->getLoadFileName(extension,description);
				}

        setShowTetrahedra(false);

				if ( fname )
				{
					strcpy(mLastFileName,fname);

					createSoftBody(fname,0,0);

					if ( mEdit && mEdit->mModel && mTetraMaker )
					{
						TetraMesh *tm = mEdit->mModel->getTetraMesh();
						if ( tm )
						{
							if ( mTetraMesh == 0 )
  							mTetraMesh = new NxTetraMesh;
							mTetraMaker->createTetraMesh(*mTetraMesh, tm->mPositions.size(), &tm->mPositions[0].x, tm->mIndices.size()/4, &tm->mIndices[0], true );
              setShowTetrahedra(true);
      				if ( mSurfaceMesh )
      				{
      					mTetraMaker->releaseTetraMesh(*mSurfaceMesh);
      					delete mSurfaceMesh;
      					mSurfaceMesh = 0;
      				}
      				mLastX = -1;
      				mLastY = -1;
      				mHitIndex = NULL_INDEX;
      				mDeletions.clear();
      				mRadius = SBM_computeBoundingSphere(mTetraMesh->mVcount, mTetraMesh->mVertices, &mCenter.x );
      				for (NxU32 i=0; i<mTetraMesh->mTcount; i++)
      				{
      					mDeletions.push_back(0);
      				}
      				tm->setDeletions(&mDeletions[0]);
						}
					}
				}
			}
			break;
    case SBC_STOP_SIMULATION:
      if ( mEdit )
      {
        mEdit->releaseSimulation();
        setShowTetrahedra(true);
      }
      break;
		case SBC_SIMULATE_SOFTBODY:
			createSimulation = true;
			ret = 1;
			break;
		}


		if ( ret == 0 )
		{
			float v = 0;
			bool flag = false;

			if ( argc >= 2 )
			{
				v = (float) atof(argv[1]);
				if ( stricmp(argv[1],"true") == 0 || strcmp(argv[1],"1") == 0 )
					flag = true;
			}

#if NX_USE_SOFTBODY_API
			NxU32 fv = 0;
			switch ( sbc )
			{
			case SBC_STATIC:
				fv = NX_SBF_STATIC;
				break;
			case SBC_DISABLE_COLLISION:
				fv = NX_SBF_DISABLE_COLLISION;
				break;
			case SBC_SELFCOLLISION:
				fv = NX_SBF_SELFCOLLISION;
				break;
			case SBC_VISUALIZATION:
				fv = NX_SBF_VISUALIZATION;
				break;
			case SBC_GRAVITY:
				fv = NX_SBF_GRAVITY;
				break;
			case SBC_VOLUME_CONSERVATION:
				fv = NX_SBF_VOLUME_CONSERVATION;
				break;
			case SBC_DAMPING:
				fv = NX_SBF_DAMPING;
				break;
			case SBC_COLLISION_TWOWAY:
				fv = NX_SBF_COLLISION_TWOWAY;
				break;
			case SBC_TEARABLE:
				fv = NX_SBF_TEARABLE;
				break;
			case SBC_HARDWARE:
        if ( mHardwareOk )
        {
  				fv = NX_SBF_HARDWARE;
	  			createSimulation = true;
        }
        else
        {
          if ( gFileInterface )
            gFileInterface->softMessageReport("Hardware not present.\r\n");
        }
				break;
			case SBC_COMDAMPING:
				fv = NX_SBF_COMDAMPING;
				break;
			case SBC_FLUID_COLLISION:
				fv = NX_SBF_FLUID_COLLISION;
				break;
			}

			if ( fv )
			{
				if ( flag )
					mSoftBodyDesc.flags|=fv;
				else
					mSoftBodyDesc.flags&=~fv;
			}

			for (NxU32 i=0; i<mSoftBodies.size(); i++)
			{
				SoftBody *s    = mSoftBodies[i];
				NxSoftBody *sb = s->getNxSoftBody();
				if ( sb )
				{
					switch ( sbc )
					{
					case SBC_VOLUME_STIFFNESS:
						sb->setVolumeStiffness(v);
						mSoftBodyDesc.volumeStiffness = v;
						break;
					case SBC_STRETCHING_STIFFNESS:
						sb->setStretchingStiffness(v);
						mSoftBodyDesc.stretchingStiffness = v;
						break;
					case SBC_DAMPING_COEFFICIENT:
						sb->setDampingCoefficient(v);
						mSoftBodyDesc.dampingCoefficient = v;
						break;
					case SBC_FRICTION:
						sb->setFriction(v);
						mSoftBodyDesc.friction = v;
						break;
					case SBC_PARTICLE_RADIUS:
						sb->setParticleRadius(v);
						mSoftBodyDesc.particleRadius = v;
						break;
					case SBC_SOLVER_ITERATIONS:
						sb->setSolverIterations( (NxU32) v );
						mSoftBodyDesc.solverIterations = (NxU32) v;
						break;
					default:
					  if ( 1 )
					  {
					  	NxU32 flags = mSoftBodyDesc.flags;
					  	NxU32 cflags = sb->getFlags();
					  	if ( cflags &	NX_SBF_HARDWARE ) // if currently hardware, then it must stay hardware
					  	{
					  		flags|=NX_SBF_HARDWARE;
					  	}
					  	else
					  	{
					  		flags&=~NX_SBF_HARDWARE; // if currently software it must stay software, this cannot be toggled on the fly.
					  	}
  						sb->setFlags(flags);
  					}
						break;
					}
				}
			}
#endif
		}

		if ( createSimulation )
		{
#if NX_USE_SOFTBODY_API
			if ( mEdit )
      {
				mEdit->releaseSimulation();
        if ( mLastEditCount != mEditCount )
        {
					if ( mEdit->mModel )
					{
						mEdit->mModel->createLinks();
            rebuildSoftBodyMesh(*mEdit->mModel);
					}
        }
				mEdit->createSimulation(mSoftBodyDesc);
				mXrot = mYrot = 0;
				NxMat34 m;
				m.id();
				m.getColumnMajor44(mWorld);
      }
#endif
		}

		return ret;
	}


	void SoftBodySystem::logMessage(const char *fmt,...)
	{
		if ( gFileInterface )
		{
			char wbuff[8192];
			vsprintf(wbuff, fmt, (char *)(&fmt+1));
			gFileInterface->softMessageReport(wbuff);
		}
	}


	void SoftBodySystem::releaseSoftBodies(TetraModel *tm) // release all soft bodies that use this tetramodel.  Only releases the
	{
		for (NxU32 i=0; i<mSoftBodies.size(); i++)
		{
			SoftBody *sb = mSoftBodies[i];
			sb->releaseSoftBody(tm);
		}
	}

	void SoftBodySystem::clearEditing(void)
	{
    if ( mEdit && mEdit->mModel )
    {
      mEdit->mModel->setDeletions(0);
    }
		if ( mSourceMesh )
		{
			mTetraMaker->releaseTetraMesh(*mSourceMesh);
			delete mSourceMesh;
			mSourceMesh = 0;
		}
		if ( mSurfaceMesh )
		{
			mTetraMaker->releaseTetraMesh(*mSurfaceMesh);
			delete mSurfaceMesh;
			mSurfaceMesh = 0;
		}
		if ( mTetraMesh )
		{
			mTetraMaker->releaseTetraMesh(*mTetraMesh);
			delete mTetraMesh;
			mTetraMesh = 0;
		}
	}

	static bool hitCheck(const NxVec3 &pos,const NxVec3 &dir,const NxVec3 &a,const NxVec3 &b,const NxVec3 &c,float &nearest)
	{
		bool ret = false;

		float t;

		if ( SBM_rayIntersectsTriangle(&pos.x, &dir.x, &a.x, &b.x, &c.x,t) )
		{
			if ( t < nearest )
			{
				nearest = t;
				ret = true;
			}
		}

		return ret;
	}

	static bool hitCheck2(const NxVec3 &pos,const NxVec3 &dir,const NxVec3 &a,const NxVec3 &b,const NxVec3 &c,float &nearest)
	{
		bool ret = false;

		float t;

		if ( SBM_rayIntersectsTriangle(&pos.x, &dir.x, &a.x, &b.x, &c.x,t) )
		{
			if ( t > nearest )
			{
				nearest = t;
				ret = true;
			}
		}

		return ret;
	}

	void eulerToQuat(NxQuat &q,float x,float y,float z)
	{
		x*=0.5f;
		y*=0.5f;
		z*=0.5f;

		float cr = cosf(x);
		float cp = cosf(y);
		float cy = cosf(z);

		float sr = sinf(x);
		float sp = sinf(y);
		float sy = sinf(z);

		float cpcy = cp*cy;
		float spsy = sp*sy;
		float spcy = sp*cy;
		float cpsy = cp*sy;

    float w  = cr*cpcy + sr*spsy;
		float qx = sr*cpcy - cr*spsy;
		float qy = cr*spcy + cr*cpsy;
		float qz = cr*cpsy - sr*spcy;
		q.setXYZW(qx,qy,qz,w);
	}

	bool SoftBodySystem::trackMouse(int sx,int sy,bool lbutton,bool rbutton,bool lshift,bool rshift,bool dragOk)
	{
		bool ret = false;

	  bool dragged = false;

    bool shifted = false;
    if ( !lshift && !rshift )
      mHitIndex = NULL_INDEX;
    else
      shifted = true;

    bool ok = false;

    if ( mEdit && mTetraMesh && !mEdit->isSimulating() )
    {
      if ( shifted || rbutton )
        ok = true;
    }
    else
    {
      if ( mMouseDrag ) //&& mEdit && mEdit->isSimulating() )
      {
        if ( dragOk )
        {
					if ( gFileInterface )
					  gFileInterface->fetchResults();

          NxDragInfo *drag = mouseDrag(mMouseDrag,mViewMatrix,mProjectionMatrix,sx,sy,NGT_ALL,mDtime); // do mouse dragging

          if ( drag )
          {
            gGraphicsInterface->addLine(0xFFFF00,&drag->mDragPosition.x,&drag->mIntersect.x);
            dragged = true;
          	NxMat34 m;
          	m.id();
          	m.getColumnMajor44(mWorld);
          }
          ret = true;
        }
        else
        {
        	if ( isMouseSelected(mMouseDrag) )
        	{
  					if ( gFileInterface )
	  				  gFileInterface->fetchResults();
            mouseDragDeselect(mMouseDrag); // indicate that the right mouse button is 'up' and we can deselect
          }
        }
      }
    }



		if ( rbutton && !shifted && !dragged && dragOk )
		{
			if ( mLastRightButton )
			{
				float tmp = 3.141592654f /10.0f;

				mYrot+=tmp*(float)(mMouseX-sx);
				mXrot+=tmp*(float)(mMouseY-sy);

				if ( mXrot < -89.9999f ) mXrot = -89.9999f;
				if ( mXrot >  89.9999f ) mXrot =  89.9999f;

         NxQuat q;

				const float DEG_TO_RAD = ((2.0f * 3.141592654f) / 360.0f);

        eulerToQuat(q,mXrot*DEG_TO_RAD,mYrot*DEG_TO_RAD,0);

				NxMat34 mat;
				mat.id();
				mat.M.fromQuat(q);

				mat.getColumnMajor44(mWorld); // convert to 4x4 render matrix

				ret = true;
				ok = false;
			}
		}

		NxVec3 pos;
    if ( ok )
  		ok = gGraphicsInterface->screenToWorld(sx,sy,&pos.x,&mRayDir.x);

		if ( ok )
		{
      ret = true; // we snarfed the input

			if ( shifted ) // only if we are editing a tetrahedral mesh
			{
				float tdist;
				ret = SBM_rayIntersectsSphere(&pos.x, &mRayDir.x, &mCenter.x, mRadius, tdist);

				if ( ret )
				{
					if ( sx != mLastX || sy != mLastY || lbutton || rbutton )
					{

						mLastX = sx;
						mLastY = sy;


						float nearest = FLT_MAX;
						float nearestDeleted = 0;

						NxU32 lastIndex = mHitIndex;

						mHitIndex     = NULL_INDEX;
						NxU32 mDeletedIndex = NULL_INDEX;

						const unsigned int *idx = mTetraMesh->mIndices;
						const float *positions  = mTetraMesh->mVertices;

						NxMat34 m;
						m.id();
						m.setColumnMajor44(mWorld);
						m.t.x = mWorld[12];
						m.t.y = mWorld[13];
						m.t.z = mWorld[14];

						for (NxU32 i=0; i<mTetraMesh->mTcount; i++)
						{
							unsigned int i1 = *idx++;
							unsigned int i2 = *idx++;
							unsigned int i3 = *idx++;
							unsigned int i4 = *idx++;

							NxVec3 p1(&positions[i1*3]);
							NxVec3 p2(&positions[i2*3]);
							NxVec3 p3(&positions[i3*3]);
							NxVec3 p4(&positions[i4*3]);

							p1=m*p1;
							p2=m*p2;
							p3=m*p3;
							p4=m*p4;

							if ( mDeletions[i] )
							{
								if ( hitCheck2(pos,mRayDir,p3,p2,p1,nearestDeleted) ) mDeletedIndex = i;
								if ( hitCheck2(pos,mRayDir,p1,p2,p4,nearestDeleted) ) mDeletedIndex = i;
								if ( hitCheck2(pos,mRayDir,p2,p3,p4,nearestDeleted) ) mDeletedIndex = i;
								if ( hitCheck2(pos,mRayDir,p3,p1,p4,nearestDeleted) ) mDeletedIndex = i;
							}
							else
							{

								if ( hitCheck(pos,mRayDir,p3,p2,p1,nearest) ) mHitIndex = i;
								if ( hitCheck(pos,mRayDir,p1,p2,p4,nearest) ) mHitIndex = i;
								if ( hitCheck(pos,mRayDir,p2,p3,p4,nearest) ) mHitIndex = i;
								if ( hitCheck(pos,mRayDir,p3,p1,p4,nearest) ) mHitIndex = i;
							}

						}

						if ( mHitIndex != NULL_INDEX && lbutton && !mLastLeftButton )
						{
							mDeletions[mHitIndex] = 1;
              mEditCount++;
						}

						if ( mDeletedIndex != NULL_INDEX && rbutton && !mLastRightButton )
						{
							mDeletions[mDeletedIndex] = 0;
              mEditCount++;
						}
					}
				}
			}
		}

		mLastLeftButton  = lbutton;
		mLastRightButton = rbutton;
		mMouseX          = sx;
		mMouseY          = sy;

		return ret;
	}

	NxSoftBodyMesh * SoftBodySystem::createSoftBodyMeshes(NxSoftBodyMeshDesc &sd,bool hardware)
	{
		NxSoftBodyMesh *bmesh = 0;

#if NX_USE_SOFTBODY_API

		NxPhysicsSDK &sdk = mScene->getPhysicsSDK();

		NXU::InitCooking();
		NXU::ImportMemoryWriteBuffer	writeBuffer;
		bool status = NXU::CookSoftBodyMesh(sd,	writeBuffer);
		NXU::CloseCooking();

		if (status)
		{
			NXU::ImportMemoryReadBuffer importMemoryReadBuffer(writeBuffer.data);
			bmesh = sdk.createSoftBodyMesh(importMemoryReadBuffer);
		}
#endif

		return bmesh;
	}


		void SoftBodySystem::rebuildSoftBodyMesh(TetraModel &tm)
		{

			NxSoftBodyMesh *bmesh = 0;

#if NX_USE_SOFTBODY_API

			TetraMesh *tmesh = tm.getTetraMesh();

			if ( tmesh )
			{

				NxSoftBodyMesh *sbold = tm.getSoftBodyMesh();

				if ( sbold ) // release the old one if there is one
				{
					NxPhysicsSDK &sdk = mScene->getPhysicsSDK();
					sdk.releaseSoftBodyMesh(*sbold);
					tm.setSoftBodyMesh(0);
				}

        NxArray< NxU32 > editIndices;

        const NxU32 *indices = &tmesh->mIndices[0];
        NxU32 tcount         = tmesh->mIndices.size()/4;

        if ( tmesh->mDeletions )
        {
          const NxU32 *idx = indices;
          for (NxU32 i=0; i<tcount; i++)
          {
            NxU32 i1 = *idx++;
            NxU32 i2 = *idx++;
            NxU32 i3 = *idx++;
            NxU32 i4 = *idx++;
            if ( !tmesh->mDeletions[i] )
            {
              editIndices.push_back(i1);
              editIndices.push_back(i2);
              editIndices.push_back(i3);
              editIndices.push_back(i4);
            }
          }
          indices = &editIndices[0];
          tcount  = editIndices.size()/4;
        }

				NxSoftBodyMeshDesc sd;

				sd.numVertices            = tmesh->mPositions.size();
				sd.numTetrahedra          = tcount;
				sd.vertexStrideBytes      = sizeof(NxVec3);
				sd.tetrahedronStrideBytes = sizeof(NxU32)*4;
				sd.vertices               = &tmesh->mPositions[0].x;
				sd.tetrahedra             = indices;
				sd.flags                  = 0;
				sd.vertexMassStrideBytes  = 0;
				sd.vertexFlagStrideBytes  = 0;
				sd.vertexMasses           = 0;
				sd.vertexFlags            = 0;

				if ( 1 )
				{
					NxSoftBodyMesh *smesh = createSoftBodyMeshes(sd,false);
	  			tm.setSoftBodyMesh(smesh);
	  		}

        mLastEditCount = mEditCount;
			}
#endif
		}


		void	SoftBodySystem::NXU_notifySoftBody(NxSoftBody	*softBody,	const	char *userProperties)
		{
			assert( mNotify );
			if ( mNotify)
			{
				mNotify->setSoftBody(softBody);
				mSoftBodies.push_back(mNotify);
				mEdit = mNotify;
				mNotify = 0;
			}
		}

		void 	SoftBodySystem::NXU_notifySoftBodyMesh(NxSoftBodyMesh *t,const char *userProperties)
		{
#if NX_USE_SOFTBODY_API
			::NxSoftBodyMeshDesc d;
			t->saveToDesc(d);
			TetraModel *tm = new TetraModel(t,userProperties);
			mTetraModels.push_back(tm);
#endif
		}

		bool	SoftBodySystem::NXU_preNotifySoftBody(NxSoftBodyDesc &softBody,const char	*userProperties)
		{
			bool ret = false;

      setShowTetrahedra(true);

#if NX_USE_SOFTBODY_API
      if ( (softBody.flags & NX_SBF_HARDWARE) && !mHardwareOk )
      {
        ret = false; // don't create it if it is slated for hardware but we don't have hardware available..
        if ( gFileInterface )
        {
          gFileInterface->softMessageReport("Cannot create hardware softbody. Harware not available.\r\n");
        }
      }
      else
      {
  			if ( softBody.softBodyMesh )
  			{
  				TetraModel *tm = locateTetraModel(softBody.softBodyMesh);
  				if ( tm )
  				{
  					ret = true;
  					mNotify = new SoftBody(softBody,tm,userProperties);
  				}
  			}
      }
#endif
			return ret;
		}

		bool 	SoftBodySystem::NXU_preNotifySoftBodyMesh(NxSoftBodyMeshDesc &t,const char *userProperties)
		{
			return true;
		}

		void	SoftBodySystem::NXU_notifySoftBodyFailed(NxSoftBodyDesc &softBody, const char	*userProperties)
		{
			assert( mNotify );
			delete mNotify;
			mNotify = 0;
		}

		void 	SoftBodySystem::NXU_notifySoftBodyMeshFailed(NxSoftBodyMeshDesc &t,const char *userProperties)
		{
		}


		void  SoftBodySystem::releaseSoftBody(NxSoftBody *sbody)
		{
			for (NxU32 i=0; i<mSoftBodies.size(); i++)
			{
				SoftBody *sb = mSoftBodies[i];
				if ( sb->mSoftBody == sbody )
				{
					releaseSoftBody(sb);
					break;
				}
			}
		}


		TetraModel *SoftBodySystem::locateTetraModel(NxSoftBodyMesh *mesh)
		{
			TetraModel *ret = 0;
			for (NxU32 i=0; i<mTetraModels.size(); i++)
			{
				TetraModel *tm = mTetraModels[i];
				if ( tm->mNxSoftBodyMesh == mesh )
				{
					ret = tm;
					break;
				}
			}
			return ret;
		}

		TetraModel *SoftBodySystem::getEditModel(void)
		{
			TetraModel *ret = 0;
			if ( mEdit ) ret = mEdit->mModel;
			return ret;
		}

		void SoftBodySystem::loadNxuStream(const char *fname,const NxMat34 *globalPose)
		{
			NXU::NXU_FileType type = NXU::FT_XML;
			if ( stristr(fname,".nxb") ) type = NXU::FT_BINARY;
			if ( stristr(fname,".dae") ) type = NXU::FT_COLLADA;

			NXU::NxuPhysicsCollection *collection = NXU::loadCollection(fname,type);

			if ( collection )
			{
				createSkinnedMeshInstance(fname,globalPose);
				NXU::releaseCollection(collection);
			}
		}

		void SoftBodySystem::copySurface(TetraModel *tm)
		{

			if ( mEdit )
				mEdit->releaseSimulation();
			if ( tm )
			{
				setShowTetrahedra(false);
				releaseSoftBodies(tm);
				tm->clearTetraMesh();
				const TetraPositionMesh &mesh =  tm->getPositionMesh();
				if ( mTetraMaker )
				{

					if ( mTetraMesh )
					{
						mTetraMaker->releaseTetraMesh(*mTetraMesh);
						mTetraMesh = 0;
					}

					if ( mSourceMesh == 0 )
						mSourceMesh = new NxTetraMesh;

					mTetraMaker->createTetraMesh( *mSourceMesh,
						mesh.mPositions.size(),
						&mesh.mPositions[0].x,
						mesh.mIndices.size()/3,
						&mesh.mIndices[0],
						false);
					if ( mSurfaceMesh == 0 )
						mSurfaceMesh = new NxTetraMesh;

					mTetraMaker->simplifySurface(mDetailLevel,*mSourceMesh,*mSurfaceMesh);

					logMessage("SourceMesh: %d triangles  SurfaceMesh: %d triangles.\r\n", mSourceMesh->mTcount, mSurfaceMesh->mTcount );

				}

			}
		}

		void SoftBodySystem::copyIsoSurface(TetraModel *tm)
		{
			if ( mEdit )
				mEdit->releaseSimulation();
			if ( tm )
			{

        setShowTetrahedra(false);
				releaseSoftBodies(tm);
				const TetraPositionMesh &mesh =  tm->getPositionMesh();
				if ( mTetraMaker )
				{

					if ( mTetraMesh )
					{
						mTetraMaker->releaseTetraMesh(*mTetraMesh);
						mTetraMesh = 0;
					}

					tm->clearTetraMesh();

					if ( mSourceMesh == 0 )
						mSourceMesh = new NxTetraMesh;

					mTetraMaker->createTetraMesh( *mSourceMesh,
						mesh.mPositions.size(),
						&mesh.mPositions[0].x,
						mesh.mIndices.size()/3,
						&mesh.mIndices[0],
						false);
					if ( mSurfaceMesh == 0 )
						mSurfaceMesh = new NxTetraMesh;

					NxTetraMesh iso;
					mTetraMaker->createIsoSurface(*mSourceMesh,iso,mIsoSingle);
					mTetraMaker->simplifySurface(mDetailLevel,iso,*mSurfaceMesh);
					mTetraMaker->releaseTetraMesh(iso);
					logMessage("SourceMesh: %d triangles  SurfaceMesh: %d triangles.\r\n", mSourceMesh->mTcount, mSurfaceMesh->mTcount );
				}

			}

		}

		void SoftBodySystem::createTetrahedra(TetraModel *tm)
		{

			if ( tm && mSurfaceMesh )
			{
				releaseSoftBodies(tm);
				if ( mTetraMesh == 0 )
					mTetraMesh = new NxTetraMesh;

				mTetraMaker->createTetraMesh(*mSurfaceMesh,*mTetraMesh);

				tm->setTetraMesh(mTetraMesh->mVcount,mTetraMesh->mVertices,mTetraMesh->mTcount,mTetraMesh->mIndices);

				logMessage("SourceMesh: %d triangles and %d vertices\r\n", mSurfaceMesh->mVcount, mSurfaceMesh->mTcount );
				logMessage("TetraMesh: %d tetrahedrons %d vertices %d triangles.\r\n", mTetraMesh->mTcount, mTetraMesh->mVcount, mTetraMesh->mTcount*4 );

        setShowTetrahedra(true);

				if ( mSurfaceMesh )
				{
					mTetraMaker->releaseTetraMesh(*mSurfaceMesh);
					delete mSurfaceMesh;
					mSurfaceMesh = 0;
				}
				mLastX = -1;
				mLastY = -1;
				mHitIndex = NULL_INDEX;
				mDeletions.clear();

				mRadius = SBM_computeBoundingSphere(mTetraMesh->mVcount, mTetraMesh->mVertices, &mCenter.x );

				for (NxU32 i=0; i<mTetraMesh->mTcount; i++)
				{
					mDeletions.push_back(0);
				}

				tm->setDeletions(&mDeletions[0]);
				tm->createLinks();

				rebuildSoftBodyMesh(*tm);

			}
		}



		void SoftBodySystem::saveNxuStream(TetraModel *tm)
		{

			if ( tm )
			{
				TetraMesh *tmesh = tm->mTetraMesh;
				if ( tmesh )
				{
					char scratch[512];
					concatFile(scratch,tm->mObjName,".xml");

					const char *fname = scratch;
					if ( gFileInterface )
						fname = gFileInterface->getSaveFileName(".xml",fname,"Export filename for NxuStream");
					if ( fname )
					{
						char scratch2[512];
						sprintf(scratch2,"graphics_mesh=%s", getQuoteString(tm->mObjName) );

#if NX_USE_SOFTBODY_API
						NxSoftBodyMeshDesc sd;

						sd.numVertices            = tmesh->mPositions.size();
						sd.numTetrahedra          = tmesh->mIndices.size()/4;
						sd.vertexStrideBytes      = sizeof(NxVec3);
						sd.tetrahedronStrideBytes = sizeof(NxU32)*4;
						sd.vertices               = &tmesh->mPositions[0].x;
						sd.tetrahedra             = &tmesh->mIndices[0];
						sd.flags                  = mSoftBodyDesc.flags;
						sd.vertexMassStrideBytes  = 0;
						sd.vertexFlagStrideBytes  = 0;
						sd.vertexMasses           = 0;
						sd.vertexFlags            = 0;

						saveSoftBodyNxuStream(fname,scratch2,mSoftBodyDesc,sd);
#endif

					}
				}
			}
		}



		SkinnedMeshInstance * SoftBodySystem::createSkinnedMeshInstance(const char *name,const NxMat34 *rootNode)
		{
			SkinnedMeshInstance *ret = 0;

			SkinnedMesh *model = locateSkinnedMesh(name);

			if ( model )
			{
				ret = sm_CreateSkinnedMeshInstance(model,mScene,rootNode);
				if ( ret )
				{
					mSkinnedSoftBodies.push_back(ret);
				}
			}

			return ret;
		}

		SkinnedMesh * SoftBodySystem::locateSkinnedMesh(const char *name)
		{
			SkinnedMesh *ret = 0;

			for (NxU32 i=0; i<mSkinnedMeshs.size(); i++)
			{
				SkinnedMesh *model = mSkinnedMeshs[i];
				if ( sm_MatchSkinnedMesh(model,name) )
				{
					ret = model;
					break;
				}
			}

			if ( !ret )
			{
				ret = sm_CreateSkinnedMesh(name);
				if ( ret )
				{
					mSkinnedMeshs.push_back(ret);
				}
			}

			return ret;
		}

		void SoftBodySystem::setWorld(const void *world)
		{
			memcpy(mWorld,world,sizeof(float)*16);
		}


		void SoftBodySystem::setScene(NxScene *scene)
		{
			if ( scene != mScene )
			{
				mScene = scene;

        mHardwareOk = false;
				if ( mScene )
				{
					NxPhysicsSDK &sdk = mScene->getPhysicsSDK();
					NxHWVersion v = sdk.getHWVersion();
					if ( v != NX_HW_VERSION_NONE )
					{
						mHardwareOk = true;
					}
				}

	      releaseMouseDrag(mMouseDrag);
		    mMouseDrag = 0;
			  if ( mScene )
				{
					mMouseDrag = createMouseDrag(&mScene->getPhysicsSDK(), mScene, 1024, 768 );
				}
			}
		};


		void SoftBodySystem::getCenter(float *center) const
		{
			center[0] = mCenter.x;
			center[1] = mCenter.y;
			center[2] = mCenter.z;
		}


   //==================================================================================
   static unsigned int GetColor(unsigned int r,unsigned int g,unsigned int b)
   {
   	unsigned int c = (r<<16) | (g<<8) | b;
   	return c;
   }

   static unsigned int GetColor(unsigned int r,unsigned int g,unsigned int b,float percent)
   {
    unsigned int dr = (unsigned int)((float)r*percent);
    unsigned int dg = (unsigned int)((float)g*percent);
    unsigned int db = (unsigned int)((float)b*percent);
    r-=dr;
    g-=dg;
    b-=db;
   	unsigned int c = (r<<16) | (g<<8) | b;
   	return c;
   }

  static void SwapYZ(NxVec3 &p)
  {
    float y = p.y;
    p.y = p.z;
    p.z = y;
  }

  void SoftBodySystem::drawGrid(void)
  {

    NxMat34 wm;
    wm.id();
    float world[16];
		wm.getColumnMajor44(world); // convert to 4x4 render matrix
    gGraphicsInterface->setWorldMatrix(world);

 	  unsigned int c1 = GetColor(133,153,181,0.1f);
 	  unsigned int c2 = GetColor(133,153,181,0.3f);
 	  unsigned int c3 = GetColor(133,153,181,0.5f);

	  const int   GRIDSIZE = 40;
	  const float TSCALE   = 1.0f;

	  float BASELOC = mBounds.min.y-0.05f;

	  for (int x=-GRIDSIZE; x<=GRIDSIZE; x++)
	  {
		  unsigned int c = c1;
		  bool ok = true;

		  if ( (x%10) == 0 ) c = c2;
		  if ( (x%GRIDSIZE) == 0 ) c = c3;

		  NxVec3 p1( (float)x,(float) -GRIDSIZE, BASELOC );
		  NxVec3 p2( (float)x,(float) +GRIDSIZE, BASELOC );
		  p1*=TSCALE;
		  p2*=TSCALE;

		  if ( 1 )
		  {
			  SwapYZ(p1);
			  SwapYZ(p2);
	    }

      gGraphicsInterface->addLine(c, &p1.x, &p2.x );

    }

	  for (int y=-GRIDSIZE; y<=GRIDSIZE; y++)
	  {
		  unsigned int c = c1;
		  bool ok = true;

		  if ( (y%10) == 0 ) c = c2;
		  if ( (y%GRIDSIZE) == 0 ) c = c3;

		  NxVec3 p1((float) -GRIDSIZE, (float)y, BASELOC );
		  NxVec3 p2( (float)+GRIDSIZE, (float)y, BASELOC );

		  p1*=TSCALE;
		  p2*=TSCALE;

		  if ( 1 )
		  {
			  SwapYZ(p1);
			  SwapYZ(p2);
	    }

		  gGraphicsInterface->addLine(c,&p1.x,&p2.x);
    }
    gGraphicsInterface->flushLines();
  }

  void SoftBodySystem::renderData(const NxDebugRenderable &data)
  {
  	int nol		= data.getNbLines();

  	if ( nol )
  	{
  		const NxDebugLine *lines = data.getLines();
  		for (int i=0; i<nol; ++i)
  		{
  			gGraphicsInterface->addLine( lines->color, &lines->p0.x, &lines->p1.x );
  			lines++;
  		}
  	}

  	int iNumPoints = data.getNbPoints();
  	const NxDebugPoint *pPoints = data.getPoints();
  	if( pPoints )
  	{
  		assert(0);
  		for(int i=0; i<iNumPoints; ++i)
  		{
  		}
  	}

  	int iNumTris = data.getNbTriangles();
  	const NxDebugTriangle *pTris = data.getTriangles();

  	if(pTris)
  	{
  		assert(0);
  		for(int i=0; i<iNumTris; ++i)
  		{
  		}
  	}
  }


	const char *SoftBodySystem::getImportName(NxBounds3 &b) // return the name of the last file imported and its bounding volume.
  {
    b = mBounds;
    // set the default particle size!
    float dim = mBounds.min.distance(mBounds.max);
    dim = dim*0.05f; // 1/20th is default..
#if NX_USE_SOFTBODY_API
		mSoftBodyDesc.particleRadius = dim;
	  if ( 1 )
	  {
	  	char scratch[512];
	  	sprintf(scratch,"/SbcParticleRadius %f", dim );
		  if ( gFileInterface )
				gFileInterface->menuCommand(scratch);
	  }
#endif

		return mLastFileName;
	}

  bool  SoftBodySystem::attachSoftBody(const char *softbodyName,const char *actorName) // attach this actor to this softbody.
  {
  	bool ret = false;

#if NX_USE_SOFTBODY_API
		NxSoftBody *sb = locateSoftBody(softbodyName);
		NxActor    *at = locateActor(actorName);
		if ( sb && at )
		{
			NxU32 scount = at->getNbShapes();
			NxShape *const* slist = at->getShapes();
			for (NxU32 i=0; i<scount; i++)
			{
				sb->attachToShape(slist[i], NX_SOFTBODY_ATTACHMENT_TWOWAY);
			}
		}
#endif
    return ret;
  }

	NxActor * SoftBodySystem::locateActor(const char *_name)
	{
		NxActor *ret = 0;

		if ( mScene )
		{
			NxU32 count = mScene->getNbActors();
			NxActor **alist = mScene->getActors();
			for (NxU32 i=0; i<count; i++)
			{
				NxActor *a = alist[i];
				const char * name = a->getName();
				if ( name && strcmp(name,_name) == 0 )
				{
					ret = a;
					break;
				}
			}
		}

    return ret;
	}

  NxSoftBody * SoftBodySystem::locateSoftBody(const char *name)
  {
  	NxSoftBody *ret = 0;
#if NX_USE_SOFTBODY_API
	  if ( mScene )
	  {
	  	NxU32 count = mScene->getNbSoftBodies();
	  	NxSoftBody **slist = mScene->getSoftBodies();
	  	for (NxU32 i=0; i<count; i++)
	  	{
	  		NxSoftBody *sb = slist[i];
	  		const char *n = sb->getName();
	  		if ( n && strcmp(n,name) == 0 )
	  		{
	  			ret = sb;
	  			break;
	  		}
	  	}
	  }
#endif
    return ret;
  }

  void SoftBodySystem::onLostDevice(void)
  {
    onDeviceReset(0);
  }

  void SoftBodySystem::coreDump(const char *fname,SoftBodyCommand mode)
  {
  	NXU::NXU_FileType ftype = NXU::FT_XML;

		if ( mActiveState )
		{
			NXU::setUseClothActiveState(mActiveState);
			NXU::setUseSoftBodyActiveState(mActiveState);
		}

		if ( fname == 0 && gFileInterface )
		{
			switch ( mode )
			{
				case SBC_CORE_DUMP_NXB:
    			fname = gFileInterface->getSaveFileName(".nxb","SoftBody_DUMP.nxb","Export filename for CoreDump in BINARY NxuStream format");
    			ftype = NXU::FT_BINARY;
    			break;
    		case SBC_CORE_DUMP_DAE:
    			fname = gFileInterface->getSaveFileName(".dae","SoftBody_DUMP.dae","Export filename for CoreDump in COLLADA format");
    			ftype = NXU::FT_COLLADA;
    			break;
    		default:
  	  		fname = gFileInterface->getSaveFileName(".xml","SoftBody_DUMP.xml","Export filename for CoreDump in XML NxuStream format");
  	  		break;
  	  	}
		}
		if ( fname )
		{
			NXU::coreDump(&mScene->getPhysicsSDK(), fname, ftype, true, false, false );
		}
  }

  void SoftBodySystem::setShowTetrahedra(bool state)
  {
    mShowTetrahedra = state;
		if ( gFileInterface )
    {
       if ( state )
				gFileInterface->menuCommand("/SbcShowTetrahedra true");
       else
				gFileInterface->menuCommand("/SbcShowTetrahedra false");
    }
  }

	bool SoftBodySystem::processMouseWheel(int iDelta)
	{
		bool ret = false;

	  if ( mMouseDrag && mEdit && mEdit->isSimulating() )
	  {
			mouseDragWheel(mMouseDrag,iDelta);
			ret = true;
	  }

    return ret;
	}

	}; // END OF SOFTBODY NAMESPACE
