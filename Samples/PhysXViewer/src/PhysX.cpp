#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <direct.h>

#define NOMINMAX
#include <windows.h>

#pragma warning(disable:4100)
#pragma warning(disable:4995)
#pragma warning(disable:4996)

#include "DxStdafx.h"
#include "PhysX.h"
#include "NXU_helper.h"
#include "TetraGraphics.h"
#include "SoftBody.h"
#include "cparser.h"
#include "sutil.h"
#include "log.h"
#include "SoftMesh.h"
#include "NxUserOutputStream.h"
#include "MyGlobals.h"

using namespace SOFTBODY;



class PhysX : public SoftFileInterface, public NxUserOutputStream, public NXU_errorReport
{
public:
  PhysX(void)
  {
  	mTimeScale = 1.0f;
  	mSimulate = false;
    mSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, 0, this );
    mScene = 0;
	mShootSphere = false;
    gSoftBodySystem = 0;
    mAsync = true;
    if ( mSDK )
    {
  	  mSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
  	  mSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
  	  mSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS,0.1f);
  	  mSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES,0.1f);
  		mSDK->setParameter(NX_VISUALIZATION_SCALE, 0.1f);

  		NxSceneDesc d;
  		d.groundPlane = false;
  		d.gravity.set(0,-9.8f,0);

  		mScene = mSDK->createScene(d);

  		if ( mScene )
  		{
  			gSoftBodySystem = new SoftBodySystem(mScene);
  			gSoftBodySystem->setSoftFileInterface(this);
  			NxTetraInterface *iface = getTetraMaker();

  			if ( iface )
  				gSoftBodySystem->setTetraMaker(iface);
  			else
  				MyMessageBox(false,"Failed to load NxTetra.dll");
  		}
			else
			{
 				MyMessageBox(false,"Failed to initial physics scene!");
			}
  	}

    mPaused = false;

		NXU::setErrorReport(this);

  }

  ~PhysX(void)
  {
		fetchResults();

  	delete gSoftBodySystem;

  	if ( mScene )
    	mSDK->releaseScene(*mScene);

  	mSDK->release();

		NXU::setErrorReport(0);
  }

	virtual	void NXU_errorMessage(bool isError, const char	*str)
	{
		if ( isError )
			gAllGlobals.gLog.ref()->Display("NXUSTREAM: ERROR   : %s\r\n",str);
		else
			gAllGlobals.gLog.ref()->Display("NXUSTREAM: WARNING : %s\r\n",str);
	}

  bool isOk(void) const
  {
  	bool ret = false;
  	if ( mSDK )
  		ret = true;
    return ret;
  };

  void simulate(float dtime)
  {
    if ( mSDK && !mPaused )
    {
      NxU32 scount = mSDK->getNbScenes();
      for (NxU32 i=0; i<scount; i++)
      {
        NxScene *s = mSDK->getScene(i);
				if ( mSimulate )
				{
					mSimulate = false;
    	    s->fetchResults(NX_RIGID_BODY_FINISHED, true);
				}
				s->setTiming(1.0f/60.0f,2,NX_TIMESTEP_FIXED);
				if (mShootSphere)
				{
					mShootSphere = false;
					shootSphereInternal();
				}
  	    s->simulate(dtime*mTimeScale);
  	    s->flushStream();
  	    mSimulate = true;
  	    if ( !mAsync )
  	    {
  	    	fetchResults();
  	    }
      }
    }
  }

  void render(void *d3device,void *hwnd,const void *view,const void *projection,float dtime)
  {
  	if ( gSoftBodySystem )
  	{
  		gSoftBodySystem->setDevice(d3device,hwnd);
  		gSoftBodySystem->render(view,projection,true,dtime);

			for (NxU32 i=0; i<mActors.size(); i++)
		  {
		  	NxActor *a = mActors[i];
		  	if ( a->userData )
		  	{
		  		SoftMeshSystem *sms = (SoftMeshSystem *) a->userData;
		  		float matrix[16];
		  		NxMat34 m = a->getGlobalPose();
		  		m.getColumnMajor44(matrix);
		  		renderSoftMeshSystem(sms,matrix);
		  	}
		  }
  	}
  }

	bool isExplicit(const char *name)
	{
		bool ret = false;

		if ( strchr(name,':') ||
			   strchr(name,'\\') ||
				 strchr(name,'/') ) ret = true;

		return ret;
	}

  const char * getSoftFileName(const char *name,bool read_access) // translate a source file to its full relative path location on disk.
  {
		const char *ret = name;

		static char scratch[512];
		FILE *fph = 0;
		if ( read_access )
		{
			fph = fopen(name,"rb");
			if ( fph )
			{
				fclose(fph);
				return ret;
			}
			WCHAR mpath[512];
			GetModuleFileName(NULL, mpath, 255);
			SetCurrentDirectory(mpath);
			const char *media = MEDIA_PATH;
			sprintf(scratch,"%s\\%s", media, name );
			fph = fopen(scratch,"rb");
			if ( fph )
			{
				fclose(fph);
				return ret = scratch;
			}
			char base[256];
			char path[256];
			char postfix[256];
			if ( IsDirectory(name,path,base,postfix) )
			{
				const char *media = MEDIA_PATH;
			  sprintf(scratch,"%s\\%s", media, base );
				ret = scratch;
			}
		}
		//if ( isExplicit(name) )
		//{

		//}
		//else
		//{
		//	const char *media = MEDIA_PATH;
  //	  sprintf(scratch,"%s\\%s", media, name );
		//	ret = scratch;
		//}
  	return ret;
  }

  void         softMessageReport(const char *str)
  {
  	gAllGlobals.gLog.ref()->Display("%s", str);
  }; // optional.  Trap this routine to receive log messages.

  void         menuCommand(const char *str)
  {
  	CPARSER.Parse("%s",str);
  };

  void         scriptCommand(const char *str)
  {
  	CPARSER.Parse("%s",str);
  };

  const char * getSaveFileName(const char *fileType,const char *initial,const char *description)
  {
  	const char *ret = 0;

		ret = getFileName(fileType,initial,true,description);

    return ret;
  }; // allows the application the opportunity to present a file save dialog box.

  const char * getLoadFileName(const char *extension,const char *description)
  {
  	const char *ret = 0;
  	ret = getFileName(extension,0,false,description);
    return ret;
  }; // present a file open dialog box with OBJ, TET, EZM, or XML files.

	void fetchResults(void)
	{
		if( mScene && mSimulate )
		{
 	    mScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
			mSimulate = false;
		}
	}

  NxScene *    resetSDK(bool groundPlane)
  {
  	mActors.clear();

  	NXU::releasePersistentMemory();

		if ( mScene )
		{
			fetchResults();
  		mSDK->releaseScene(*mScene);
  	}

  	if ( mSDK )
  	{
  		mSDK->release();
  		mSDK = 0;
  	}

  	gSoftBodySystem->setScene(0);
    mSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, 0, this );
    mScene = 0;
    if ( mSDK )
    {
  	  mSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
  	  mSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
  	  mSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS,0.1f);
  	  mSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES,0.1f);
  		mSDK->setParameter(NX_VISUALIZATION_SCALE, 0.1f);
  		NxSceneDesc d;
			d.gravity.set(0,-9.8f,0);
  		mScene = mSDK->createScene(d);
  	}

    if ( groundPlane )
    {
    	createGroundPlane();
    }
    return mScene;
  }; // reset the SDK, return the current scene, and create a ground plane if requested.


  bool MyMessageBox(bool yesno,const char *fmt, ...)
  {
  	bool ret = false;

  	char buff[2048];
  	wchar_t wbuff[2048];
  	vsprintf(buff, fmt, (char *)(&fmt+1));
  	mbstowcs(wbuff,buff,2048);
    DWORD mtype;
  	if ( yesno )
  		mtype = MB_YESNO;
  	else
  		mtype = MB_OK | MB_ICONEXCLAMATION;

  	int ok = MessageBox(0,wbuff,L"Error",mtype);
  	if ( ok == IDYES) ret = true;

  	return ret;
  }



  NxTetraInterface *getTetraMaker(void) // loads the tetra maker DLL and returns the interface pointer.
  {
    NxTetraInterface *ret = 0;
    const char *dmodule = "NxTetra.dll";
    dmodule = getSoftFileName(dmodule,true);
    wchar_t dname[512];
    mbstowcs(dname,dmodule,512);
    HMODULE module = LoadLibrary( dname );
    if ( module )
    {
      if ( module )
      {
        void *proc = GetProcAddress(module,"getTetraInterface");
        if ( proc )
        {
          typedef NxTetraInterface * (__cdecl * NX_GetToolkit)();
     		  ret = ((NX_GetToolkit)proc)();
        }
      }
    }
    return ret;
  }

  void add(wchar_t *dest,const wchar_t *src,unsigned int &index)
  {
    if ( src )
    {
      size_t len;
      StringCchLength(src,512,&len);
      for (unsigned int i=0; i<len; i++)
      {
        wchar_t c = *src++;
        dest[index++] = c;
      }
    }
    else
    {
      wchar_t c = 0;
      dest[index++] = c;
    }
  }

  const char * getFileName(const char *fileType,const char *initial,bool saveMode,const char *description) // allows the application the opportunity to present a file save dialog box.
  {
  	const char *ret = initial;

  	const char *extension = fileType;

  	static int sWhichFileType = 1;
  	char curdir[512];
  	getcwd(curdir,512);
  	wchar_t buffer[257] = L"";

  	OPENFILENAME f;

  	memset (&f, 0, sizeof(OPENFILENAME));
  	f.lStructSize	= sizeof(OPENFILENAME);
  	f.hwndOwner		= GetActiveWindow();
  	f.lpstrFile		= buffer;
  	f.nMaxFile		= 256;
  	f.nFilterIndex	= sWhichFileType;

    char exportName[512] = { 0 };
    if ( initial )
      strcpy(exportName,initial);

    assert(extension);

    wchar_t _filter[512];

    wchar_t _ext[512];
    wchar_t _desc[512];

    CharToWide(description,_desc,512);
    CharToWide(extension,_ext,512);

    unsigned int index = 0;
    _filter[0] = 0;

    add(_filter,_desc,index);
    add(_filter,L" (*",index);
    add(_filter,_ext,index);
    add(_filter,L")",index);
    add(_filter,0,index);
    add(_filter,L"*",index);
    add(_filter,_ext,index);
    add(_filter,0,index);
    add(_filter,0,index);
    add(_filter,0,index);


    f.lpstrFilter = _filter;
    f.lpstrTitle =  _desc;

  	f.lpstrInitialDir = NULL;

  	mbstowcs(buffer,exportName,512);

    int ok = 0;
    if ( saveMode )
      ok = GetSaveFileName(&f);
    else
      ok = GetOpenFileName(&f);

  	if ( ok )
  	{
  		// save off which file type they chose for next time
  		sWhichFileType = f.nFilterIndex;

  		// first change dir, and get the name in "normal" char format
  		chdir(curdir);

  		static char tmp[512];
  		wcstombs( tmp, buffer, 512 );

		int len0 = strlen(tmp);
		int len1 = strlen(extension);
		char* p = tmp + len0 - len1;
		if(stricmp(p, extension) != 0) 
		{
			strcat(tmp,extension);
		}

  		bool overwrite = true;

  		if ( saveMode )
  		{
    		FILE *fph = fopen(tmp,"rb");
    		if ( fph )
    		{
    			fclose(fph);
    			overwrite = MyMessageBox(true,"Are you sure you want to overwrite the file '%s'?", tmp );
    		}
    	}

  		if ( overwrite )
  			ret = tmp;
  		else
  			ret = 0;
  	}
  	else
  	{
  		ret = 0;
  	}

  	chdir(curdir);



    return ret;
  }


	void onDeviceReset(void *device)
	{
		if ( gSoftBodySystem )
			gSoftBodySystem->onDeviceReset(device);
	}

	void onLostDevice(void)
	{
		if ( gSoftBodySystem )
			gSoftBodySystem->onLostDevice();
	}

	void setWorld(const void *world)
	{
		if ( gSoftBodySystem )
			gSoftBodySystem->setWorld(world);
	}

	NxPhysicsSDK * getSDK(void) { return mSDK; };

  bool togglePaused(void)
  {
    if ( mPaused )
      mPaused = false;
    else
      mPaused = true;
    return mPaused;
  }


  bool setPaused(bool state)
  {
  	mPaused = state;
  	return true;
  }

  bool isPaused()
  {
	  return mPaused;
  }

  bool createGroundPlane(void)
  {
    bool ret = false;

    if ( mScene )
    {
      NxBounds3 b = gSoftBodySystem->getBounds();
    	NxPlaneShapeDesc planeDesc;
      planeDesc.d = b.min.y;
     	NxActorDesc actorDesc;
     	actorDesc.shapes.pushBack(&planeDesc);
     	NxActor *a = mScene->createActor(actorDesc);
      if ( a )
        ret = true;
    }
    return ret;
  }

	bool shootSphere(void)
	{
		mShootSphere = true;
		return true;
	}

	bool shootSphereInternal(void)
	{
		bool ret = false;


    if ( mScene )
    {

			TetraGraphicsInterface *iface = gSoftBodySystem->getTetraGraphicsInterface();

			if ( iface )
			{
      	NxVec3 pos(0,0,0);
				NxVec3 dir;

				iface->screenToWorld(1024/2,512/2,&pos.x, &dir.x );

      	NxVec3 initialVelocity;

				#define VELOCITY 20

				initialVelocity.x = dir.x*VELOCITY;
				initialVelocity.y = dir.y*VELOCITY;
				initialVelocity.z = dir.z*VELOCITY;

      	float size = 0.4f;

      	// Create body
  	    NxBodyDesc bodyDesc;
  	    bodyDesc.angularDamping	= 0.5f;
  	    bodyDesc.linearVelocity = initialVelocity;

     	  NxSphereShapeDesc sphereDesc;
     	  sphereDesc.radius = size;
     	  sphereDesc.shapeFlags|=NX_SF_VISUALIZATION;

      	NxActorDesc actorDesc;
   	    actorDesc.shapes.pushBack(&sphereDesc);

  	    actorDesc.body			    = &bodyDesc;
  	    actorDesc.density		    = 100.0f;
  	    actorDesc.globalPose.t  = pos;
  	    actorDesc.userData      = locateSphere("wood.dds",size);

  	    NxActor *a = mScene->createActor(actorDesc);

  	    if ( a )
  	    {
  	    	mActors.push_back(a);
  	      ret = true;
  	    }

  	  }

    }

    return ret;
	}

  void setGravity(const float *p)
  {
  	NxVec3 v(p);
  	if ( mScene )
  		mScene->setGravity(v);
  }

	void setAsync(bool state)
	{
		mAsync = state;
	}

	virtual void reportError(NxErrorCode code, const char * message, const char *file, int line)
	{
		gAllGlobals.gLog.ref()->Display("PhysX Error: %s   File(%s) Line(%d)\r\n", message, file, line );
	}
	/**
	\brief Reports an assertion violation.  The user should return

	\param message Message to display.
	\param file File error occured in.
	\param line Line number error occured on.
	*/
	virtual NxAssertResponse reportAssertViolation(const char * message, const char *file, int line)
	{
		gAllGlobals.gLog.ref()->Display("PhysX Assert: %s   File(%s) Line(%d)\r\n", message, file, line );
		return NX_AR_CONTINUE;
	}
	/**
	\brief Simply prints some debug text

	\param message Message to display.
	*/
	virtual void print(const char * message)
	{
		gAllGlobals.gLog.ref()->Display("PhysX Message: %s\r\n", message );
	}

  void setTimeScale(float scale) { mTimeScale = scale; };

private:
  NxScene       *mScene;
  NxPhysicsSDK  *mSDK;    // the physics SDK.
  TetraD3D      *mTetraD3D; // d3d graphics rendering context for soft bodies.
  bool           mPaused;
  NxArray< NxActor *> mActors;
  bool          mSimulate;
  bool          mAsync;
  bool			mShootSphere;
  float         mTimeScale;
};

static PhysX *gPhysX=0;

bool px_init(void)
{
	assert( gPhysX == 0 );
	delete gPhysX;
	gPhysX = new PhysX;
	return gPhysX->isOk();
}

void px_release(void)
{
	delete gPhysX;
	gPhysX = 0;
}

void px_simulate(float dtime)
{
	if ( gPhysX )
		gPhysX->simulate(dtime);
}

void px_render(void *d3device,void *hwnd,const void *view,const void *projection,float dtime)
{
	if ( gPhysX )
	{
		gPhysX->render(d3device,hwnd,view,projection,dtime);
	}
}

void px_onDeviceReset(void *device)
{
	if ( gPhysX )
		gPhysX->onDeviceReset(device);
}

void px_setWorld(const void *world)
{
	if ( gPhysX )
		gPhysX->setWorld(world);
}


NxPhysicsSDK * getSDK(void)
{
	NxPhysicsSDK * ret = 0;

  if ( gPhysX )
  	ret = gPhysX->getSDK();

  return ret;
}

void px_onLostDevice(void)
{
  if ( gPhysX )
    gPhysX->onLostDevice();
}


bool px_togglePaused(void)
{
  bool ret = false;

  if ( gPhysX )
    ret = gPhysX->togglePaused();

  return ret;
}

bool px_paused()
{
	return gPhysX->isPaused();
}

bool px_setPaused(bool state)
{
  bool ret = false;

  if ( gPhysX )
    ret = gPhysX->setPaused(state);

  return ret;
}

bool px_createGroundPlane(void)
{
  bool ret = false;

  if ( gPhysX )
    ret = gPhysX->createGroundPlane();

  return ret;
}


bool px_shootSphere(void)
{
	bool ret = false;

  if ( gPhysX )
    ret = gPhysX->shootSphere();

  return ret;
}


void px_setGravity(const float *p)
{
	if ( gPhysX )
		gPhysX->setGravity(p);
}

void px_fetchResults(void)
{
	if ( gPhysX )
		gPhysX->fetchResults();
}


void px_setAsync(bool state)
{
	if ( gPhysX )
		gPhysX->setAsync(state);
}

void px_setTimeScale(float scale)
{
	if ( gPhysX )
		gPhysX->setTimeScale(scale);
}
