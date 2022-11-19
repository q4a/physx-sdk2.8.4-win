#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TetraD3D.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <d3d9types.h>
#include <ddraw.h>

#define D3DFVF_TETRAVERTEX         ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 )
#define D3DFVF_LINEVERTEX       ( D3DFVF_XYZ | D3DFVF_DIFFUSE )

namespace SOFTBODY
{

class LineVertex
{
public:
  float        mPos[3];
  unsigned int mColor;
};

#define MAXLINES 8192

static void __cdecl MyMessageBox(const char *fmt, ...)
{
	char buff[2048];
	#if defined(WIN32)
	#if defined(UNICODE)
	wchar_t wbuff[2048];
	vsprintf(buff, fmt, (char *)(&fmt+1));
	mbstowcs(wbuff,buff,2048);
	MessageBox(0,wbuff,L"Error",MB_OK | MB_ICONEXCLAMATION);
	#else
	MessageBox(0,buff,"Error",MB_OK | MB_ICONEXCLAMATION);
	#endif
	#elif defined(_XBOX)
	int dprintf(const char* format,...);
	dprintf("Error : [%s]\n",buff);
	assert(false);
	#else
	assert(false);
	#endif
}

// A Pimple class (pointer to implementation) to keep from polluting the public header file
class DirectHelper : public TetraGraphicsInterface
{
public:
  DirectHelper(void)
  {
    mHwnd = 0;
    mDevice = 0;
    mEffect = 0;
    mTechnique = 0;
    mWorldViewHandle = 0;
    mWorldViewProjectionHandle = 0;
    mDiffuseHandle = 0;
    mWorldHandle = 0;
    mViewProjectionHandle = 0;
    mProjectionHandle = 0;
    mEyePosHandle = 0;
    mWireFrame = false;
	mShowBackfaces = false;
    mWireFrameTechnique = 0;
		mLineCount = 0;
  }

  ~DirectHelper(void)
  {
  	onDeviceReset(mDevice);
  }


  void   setDevice(void *d3device,void *hwnd)
  {
  	if ( mDevice == 0 )
  	{
  		mDevice = (LPDIRECT3DDEVICE9) d3device;
  		mHwnd   = (HWND)hwnd;

  		mEffect =	loadFX("SoftBody.fx");

  		if ( mEffect )
  		{
     	  mTechnique                        = mEffect->GetTechniqueByName("SoftBodyShader");
     	  mWireFrameTechnique               = mEffect->GetTechniqueByName("SoftBodyWireFrameShader");
     	  mWorldViewHandle                  = mEffect->GetParameterByName(0,"wv");
     	  mWorldViewProjectionHandle        = mEffect->GetParameterByName(0,"wvp");
        mWorldHandle                      = mEffect->GetParameterByName(0,"wm");
        mViewProjectionHandle             = mEffect->GetParameterByName(0,"vp");
        mProjectionHandle                 = mEffect->GetParameterByName(0,"proj");
        mEyePosHandle                     = mEffect->GetParameterByName(0,"EyePos");
     	  mDiffuseHandle                    = mEffect->GetParameterByName(0,"DiffuseMap");
     	  mAmbientColorHandle               = mEffect->GetParameterByName(0,"AmbientColor");
     	  mDiffuseColorHandle               = mEffect->GetParameterByName(0,"DiffuseColor");
     	  mSpecularColorHandle              = mEffect->GetParameterByName(0,"SpecularColor");
     	  mSpecularPowerHandle              = mEffect->GetParameterByName(0,"SpecularPower");
  		}
  	}
  }

  void   onDeviceReset(void *d3device)
  {
		if ( mEffect )
		{
      mEffect->Release();
      mEffect = 0;

      mTechnique = 0;
      mWireFrameTechnique = 0;
      mWorldViewHandle = 0;
      mWorldViewProjectionHandle = 0;
      mDiffuseHandle = 0;
      mWorldHandle = 0;
      mViewProjectionHandle = 0;
      mProjectionHandle = 0;
      mEyePosHandle = 0;
      mAmbientColorHandle = 0;
      mDiffuseColorHandle = 0;
      mSpecularColorHandle = 0;
      mSpecularPowerHandle = 0;

		}
    mDevice = 0;
  }

  void setWorldMatrix(const void *world)
  {

    if ( world )
  		mWorld       = *(D3DXMATRIXA16 *)world;
    else
      D3DXMatrixIdentity(&mWorld);

    applyTransforms();
  }

  void applyTransforms(void)
  {
  	D3DXMatrixMultiply(&mWorldView,&mWorld,&mView);
  	D3DXMatrixMultiply(&mWorldViewProjection,&mWorldView,&mProjection);
  	D3DXMatrixMultiply(&mViewProjection,&mView,&mProjection);

		D3DXVECTOR3 v(0,0,0);
		D3DXVECTOR4 eye;
		D3DXVec3Transform(&eye,&v,&mViewProjection);
		mEyePos.x = eye.x;
		mEyePos.y = eye.y;
		mEyePos.z = eye.z;

  	if ( mWorldViewHandle )
			mEffect->SetMatrix( mWorldViewHandle, &mWorldView );

  	if ( mWorldViewProjectionHandle )
			mEffect->SetMatrix(mWorldViewProjectionHandle, &mWorldViewProjection );

    if ( mWorldHandle )
      mEffect->SetMatrix(mWorldHandle, &mWorld );

    if ( mProjectionHandle )
      mEffect->SetMatrix(mProjectionHandle,&mProjection);

    if ( mViewProjectionHandle )
      mEffect->SetMatrix(mViewProjectionHandle,&mViewProjection);

    if ( mEyePosHandle )
		{
			D3DXVECTOR4 vp = mEyePos;
      mEffect->SetVector(mEyePosHandle,&vp);
		}
  }

	void   setViewProjectionMatrix(const void *view,const void *projection)
	{
  	mView        = *(D3DXMATRIXA16 *)view;
  	mProjection  = *(D3DXMATRIXA16 *)projection;
    applyTransforms();
	}


  void * createVertexBuffer(unsigned int vcount,const TetraGraphicsVertex *vertices)
  {
  	void * ret = 0;

  	if ( mDevice )
  	{
			int flags = 0;
			LPDIRECT3DVERTEXBUFFER9 pvbVertexBuffer = 0;
			mDevice->CreateVertexBuffer(sizeof(TetraGraphicsVertex)*vcount, D3DUSAGE_WRITEONLY, D3DFVF_TETRAVERTEX, D3DPOOL_MANAGED, &pvbVertexBuffer, 0);
			if ( pvbVertexBuffer )
			{
				void* pvVertices=0;
				pvbVertexBuffer->Lock( 0, 0, &pvVertices, 0 );
				if ( pvVertices )
				{
					if ( vertices )
						memcpy( pvVertices, vertices, sizeof(TetraGraphicsVertex)*vcount );
					else
						memset( pvVertices, 0, sizeof(TetraGraphicsVertex)*vcount );

					pvbVertexBuffer->Unlock();
					ret = pvbVertexBuffer;
				}
			}
		}
  	return ret;
  }

  bool   releaseVertexBuffer(void *buffer)
  {
  	bool ret = false;
  	if ( buffer )
  	{
			LPDIRECT3DVERTEXBUFFER9 pvbVertexBuffer = (LPDIRECT3DVERTEXBUFFER9) buffer;
			pvbVertexBuffer->Release();
			ret = true;
  	}
  	return ret;
  }

  void * lockVertexBuffer(void *buffer)
  {
  	void * ret = 0;
  	if ( buffer )
  	{
			LPDIRECT3DVERTEXBUFFER9 pvbVertexBuffer = (LPDIRECT3DVERTEXBUFFER9) buffer;
			void* pvVertices=0;
			pvbVertexBuffer->Lock( 0, 0, &pvVertices, 0 );
			ret = pvVertices;
		}
  	return ret;
  }

  bool   unlockVertexBuffer(void *buffer)
  {
  	bool ret = false;
  	if ( buffer )
  	{
			LPDIRECT3DVERTEXBUFFER9 pvbVertexBuffer = (LPDIRECT3DVERTEXBUFFER9) buffer;
			pvbVertexBuffer->Unlock();
			ret = true;
  	}
  	return ret;
  }


  void * createIndexBuffer(unsigned int icount,const unsigned int *indices)
  {
  	void *ret = 0;

	  if ( mDevice )
	  {
  		LPDIRECT3DINDEXBUFFER9  idx=0;
  		int stride       = sizeof(unsigned int);
  		D3DFORMAT	format = D3DFMT_INDEX32;
			mDevice->CreateIndexBuffer( icount*stride, D3DUSAGE_WRITEONLY, format, D3DPOOL_MANAGED, &idx, 0 );
			if ( idx )
			{
				void * pIndices=0;
				idx->Lock(0,0,&pIndices,0);
				if ( pIndices )
				{
					if ( indices )
						memcpy(pIndices, indices, icount*stride );
					else
						memset(pIndices, 0, icount*stride );
					idx->Unlock();
					ret = idx;
				}
			}
		}
  	return ret;
  }

  bool   releaseIndexBuffer(void *ibuffer)
  {
  	bool ret = false;
  	if ( ibuffer )
  	{
  	  LPDIRECT3DINDEXBUFFER9 ib = (LPDIRECT3DINDEXBUFFER9) ibuffer;
  	  ib->Release();
  	  ret = true;
  	}
  	return ret;
  }

  void * lockIndexBuffer(void *ibuffer)
  {
  	void * ret = 0;
  	if ( ibuffer )
  	{
  	  LPDIRECT3DINDEXBUFFER9 ib = (LPDIRECT3DINDEXBUFFER9) ibuffer;
			void * pIndices=0;
			ib->Lock(0,0,&pIndices,0);
			if ( pIndices )
			{
				ret = pIndices;
			}
  	}
  	return ret;
  }

  bool   unlockIndexBuffer(void *ibuffer)
  {
  	bool ret = false;
  	if ( ret )
  	{
  	  LPDIRECT3DINDEXBUFFER9 ib = (LPDIRECT3DINDEXBUFFER9) ibuffer;
  	  ib->Unlock();
  	  ret = true;
  	}
  	return ret;
  }

  D3DXHANDLE getTechnique(void)
  {
    D3DXHANDLE ret = mTechnique;
    if ( mWireFrame ) ret = mWireFrameTechnique;
    return ret;
  }

  bool shaderSetup(TetraMaterial *material)
  {
    bool ret = false;

    D3DXHANDLE tech = getTechnique();

    if ( tech )
    {
			if ( material )
			{
				if ( material->mHandle == 0 )
				{
					const char *tname = material->mTexture;
					if ( gFileInterface )
					{
						tname = gFileInterface->getSoftFileName(tname,true);
						if ( tname == 0 )
						{
							tname = gFileInterface->getSoftFileName("wood.dds",true);
						}
					}
       		LPDIRECT3DTEXTURE9 pptex=0;
					wchar_t fname[512];
        	mbstowcs(fname, tname, 512 );
					D3DXCreateTextureFromFile(mDevice,fname, &pptex);
					if (pptex == 0 )
					{
						const char *wname = "wood.dds";
						if ( gFileInterface )
						{
							wname = gFileInterface->getSoftFileName(wname,true);
						}
						if ( wname == 0 ) wname = "white.tga";
          	mbstowcs(fname,wname, 512 );
  					D3DXCreateTextureFromFile(mDevice,fname, &pptex);
					}

					material->mHandle = pptex;
				}
				if ( material->mHandle )
					mEffect->SetTexture(mDiffuseHandle, (LPDIRECT3DBASETEXTURE9)material->mHandle);

				if ( mAmbientColorHandle )
          mEffect->SetFloatArray(mAmbientColorHandle,material->mAmbientColor,4);

        if ( mDiffuseColorHandle )
          mEffect->SetFloatArray(mDiffuseColorHandle,material->mDiffuseColor,4);

        if ( mSpecularColorHandle )
          mEffect->SetFloatArray(mSpecularColorHandle,material->mSpecularColor,4);

        if ( mSpecularPowerHandle )
          mEffect->SetFloat(mSpecularPowerHandle, material->mSpecularPower );

			}
      mDevice->SetFVF(D3DFVF_TETRAVERTEX);
      mEffect->SetTechnique(tech);
      ret = true;
    }

    return ret;
  }

  bool   renderSection(TetraMaterial *material,void *vbuffer,void *ibuffer,unsigned int vcount,unsigned int tcount)
  {
    bool ret = false;

    if ( shaderSetup(material) )
    {
      LPDIRECT3DINDEXBUFFER9  pib = (LPDIRECT3DINDEXBUFFER9) ibuffer;
      LPDIRECT3DVERTEXBUFFER9 pvb = (LPDIRECT3DVERTEXBUFFER9) vbuffer;

      mDevice->SetStreamSource(0,pvb,0,sizeof(TetraGraphicsVertex));
      mDevice->SetIndices(pib);

      UINT cPass = 0;

      HRESULT ok= mEffect->Begin(&cPass,0);
      if ( SUCCEEDED(ok) )
      {
        for (UINT i=0; i<cPass; i++)
        {
          mEffect->BeginPass(i);

		  mDevice->SetRenderState(D3DRS_CULLMODE, mShowBackfaces ? D3DCULL_NONE : D3DCULL_CCW );
          mDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,vcount,0,tcount);

          mEffect->EndPass();
        }

        mEffect->End();
        ret = true;
      }
    }

  	return ret;
  }

  bool   renderSection(TetraMaterial *material,void *vbuffer,unsigned int vcount)
  {
    bool ret = false;

    if ( shaderSetup(material) )
    {
      LPDIRECT3DVERTEXBUFFER9 pvb = (LPDIRECT3DVERTEXBUFFER9) vbuffer;
      mDevice->SetStreamSource(0,pvb,0,sizeof(TetraGraphicsVertex));
      UINT cPass = 0;
      HRESULT ok= mEffect->Begin(&cPass,0);
      if ( SUCCEEDED(ok) )
      {
        for (UINT i=0; i<cPass; i++)
        {
          mEffect->BeginPass(i);
          mDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,vcount/3);
          mEffect->EndPass();
        }

        mEffect->End();
        ret = true;
      }
    }

  	return ret;
  }

  ID3DXEffect * loadFX(const char *sfname)
  {
    ID3DXEffect *ret = 0;
   	wchar_t wname[512];

    const char *fname = sfname;
    if ( gFileInterface )
      fname = gFileInterface->getSoftFileName(fname,true);

   	mbstowcs(wname,fname,512);

   	ID3DXEffect        *effect;
   	LPD3DXBUFFER Errors = NULL;
   	DWORD dwShaderFlags = 0;

   	HRESULT hresult = D3DXCreateEffectFromFile( mDevice, wname, NULL, NULL, dwShaderFlags, NULL, &effect, &Errors );
   	if(FAILED(hresult))
   	{
   		if(Errors != NULL && Errors->GetBufferPointer() != NULL)
   		{
   			char *er = (char*)Errors->GetBufferPointer();
   			MyMessageBox("  Effect File Error (%s): %s",fname,er);
   		}
   		else
   		{
   			MyMessageBox("  Effect File Error (%s): No details!",fname);
   		}
   	}
   	else
   	{
   		ret = effect;
   	}
   	return ret;
  }

  bool   renderSection(TetraMaterial *material,TetraGraphicsVertex *vbuffer,unsigned int *ibuffer,unsigned int vcount,unsigned int tcount)
  {
    bool ret = false;

    if ( shaderSetup(material) )
    {
      UINT cPass = 0;

      HRESULT ok= mEffect->Begin(&cPass,0);
      if ( SUCCEEDED(ok) )
      {
        for (UINT i=0; i<cPass; i++)
        {
          mEffect->BeginPass(i);

		  mDevice->SetRenderState(D3DRS_CULLMODE, mShowBackfaces ? D3DCULL_NONE : D3DCULL_CCW );
          mDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,vcount,tcount, ibuffer, D3DFMT_INDEX32, vbuffer, sizeof(TetraGraphicsVertex) );

          mEffect->EndPass();
        }

        mEffect->End();
        ret = true;
      }
    }

  	return ret;
  }

  bool   renderSection(TetraMaterial *material,TetraGraphicsVertex *vbuffer,unsigned int vcount)
  {
    bool ret = false;

    if ( shaderSetup(material) )
    {
      UINT cPass = 0;
      HRESULT ok= mEffect->Begin(&cPass,0);
      if ( SUCCEEDED(ok) )
      {
        for (UINT i=0; i<cPass; i++)
        {
          mEffect->BeginPass(i);
          mDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST,vcount/3,vbuffer,sizeof(TetraGraphicsVertex) );
          mEffect->EndPass();
        }
        mEffect->End();
        ret = true;
      }
    }
    return ret;
  }

  void setWireFrame(bool state)
  {
    mWireFrame = state;
  }

  void setShowBackfaces(bool state)
  {
	  mShowBackfaces = state;
  }

  void   addLine(unsigned int color,const float *p1,const float *p2)
  {
    if ( mLineCount == MAXLINES ) flushLines();
    LineVertex *v = &mLines[mLineCount];
    v->mPos[0] = p1[0];
    v->mPos[1] = p1[1];
    v->mPos[2] = p1[2];
    v->mColor  = color;
    v++;
    v->mPos[0] = p2[0];
    v->mPos[1] = p2[1];
    v->mPos[2] = p2[2];
    v->mColor  = color;
    mLineCount+=2;
  }

  void   flushLines(void)
  {
    if ( mLineCount && mWireFrameTechnique )
    {
      if ( mDiffuseColorHandle )
      {
        float dcolor[4] = { 1, 1, 1, 1 };
        mEffect->SetFloatArray(mDiffuseColorHandle,dcolor,4);
      }
      mDevice->SetFVF(D3DFVF_LINEVERTEX);
      mEffect->SetTechnique(mWireFrameTechnique);
      UINT cPass = 0;
      HRESULT ok= mEffect->Begin(&cPass,0);
      if ( SUCCEEDED(ok) )
      {
        for (UINT i=0; i<cPass; i++)
        {
          mEffect->BeginPass(i);
          mDevice->DrawPrimitiveUP(D3DPT_LINELIST,mLineCount/2,mLines,sizeof(LineVertex) );
          mEffect->EndPass();
        }
        mEffect->End();
      }
    }
    mLineCount=0;
  }

  bool  releaseMaterial(TetraMaterial *tm) // releases device handle for this material if one is present
  {
    bool ret = false;
    if ( tm && tm->mHandle )
    {
      LPDIRECT3DTEXTURE9 pptex = (LPDIRECT3DTEXTURE9) tm->mHandle;
      pptex->Release();
			tm->mHandle = 0;
			ret = true;
    }
		return ret;
  }

  bool GetWindowSize(int &wid,int &hit) // get size of current render windowl
  {
  	bool ok = false;
  	if ( mHwnd )
  	{
  		HWND m_hWnd = (HWND) mHwnd;
  		RECT  rect = { 0, 0, 0, 0 };
  		POINT  pt = { 0, 0 };
  		// Find out where the window's client area is
  		GetClientRect(m_hWnd, &rect);
  		ClientToScreen(m_hWnd, &pt);
  		wid = rect.right - rect.left;
  		hit = rect.bottom - rect.top;
  		ok = true;
  	}
  	return ok;
  }

  bool   screenToWorld(int sx,      // screen x position
                       int sy,      // screen y position
                       float *world, // world position of the eye
                       float *direction) // direction vector into the world
  {
    bool ret = false;

  	int wid,hit;

    if ( 	GetWindowSize(wid,hit) )
    {
      if ( sx >= 0 && sx <= wid && sy >= 0 && sy <= hit )
      {
      	D3DXVECTOR3 vPickRayDir;
      	D3DXVECTOR3 vPickRayOrig;

      	POINT ptCursor;
      	ptCursor.x = sx;
      	ptCursor.y = sy;

      	// Compute the vector of the pick ray in screen space

      	D3DXVECTOR3 v;

      	v.x =  ( ( ( 2.0f * ptCursor.x ) / wid  ) - 1 ) / mProjection._11;
      	v.y = -( ( ( 2.0f * ptCursor.y ) / hit ) - 1 )  / mProjection._22;
      	v.z =  1.0f;

      	// Get the inverse view matrix
      	D3DXMATRIXA16 m;
      	D3DXMatrixInverse( &m, NULL, &mView );

      	// Transform the screen space pick ray into 3D space
      	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
      	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
      	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;

        D3DXVec3Normalize(&vPickRayDir,&vPickRayDir);

      	vPickRayOrig.x = m._41;
      	vPickRayOrig.y = m._42;
      	vPickRayOrig.z = m._43;

      	world[0] = vPickRayOrig.x;
      	world[1] = vPickRayOrig.y;
      	world[2] = vPickRayOrig.z;

      	direction[0] = vPickRayDir.x;
      	direction[1] = vPickRayDir.y;
      	direction[2] = vPickRayDir.z;

        ret = true;
      }
    }
    return ret;
  }

  void set(D3DXVECTOR3 &v,float x,float y,float z)
  {
    v.x = x;
    v.y = y;
    v.z = z;
  }

  void  wireBox(unsigned int color,const float *_bmin,const float *_bmax)
  {
    D3DXVECTOR3 bmin,bmax;

    bmin.x = _bmin[0];
    bmin.y = _bmin[1];
    bmin.z = _bmin[2];

    bmax.x = _bmax[0];
    bmax.y = _bmax[1];
    bmax.z = _bmax[2];

    D3DXVECTOR3 box[8];


 		set(box[0], bmin.x, bmin.y, bmin.z );
 		set(box[1], bmax.x, bmin.y, bmin.z );
 		set(box[2], bmax.x, bmax.y, bmin.z );
 		set(box[3], bmin.x, bmax.y, bmin.z );
 		set(box[4], bmin.x, bmin.y, bmax.z );
 		set(box[5], bmax.x, bmin.y, bmax.z );
 		set(box[6], bmax.x, bmax.y, bmax.z );
 		set(box[7], bmin.x, bmax.y, bmax.z );


		addLine(color,&box[0].x,&box[1].x);
		addLine(color,&box[1].x,&box[2].x);
		addLine(color,&box[2].x,&box[3].x);
		addLine(color,&box[3].x,&box[0].x);

		addLine(color,&box[4].x,&box[5].x);
		addLine(color,&box[5].x,&box[6].x);
		addLine(color,&box[6].x,&box[7].x);
		addLine(color,&box[7].x,&box[4].x);

		addLine(color,&box[0].x,&box[4].x);
		addLine(color,&box[1].x,&box[5].x);
		addLine(color,&box[2].x,&box[6].x);
		addLine(color,&box[3].x,&box[7].x);

  }


  void   getEyePos(float *eye)
  {
    eye[0] = mEyePos.x;
    eye[1] = mEyePos.y;
    eye[2] = mEyePos.z;
  }

private:
	D3DXMATRIXA16      mWorld;
	D3DXMATRIXA16      mView;
	D3DXMATRIXA16      mProjection;
	D3DXMATRIXA16      mWorldView;
	D3DXMATRIXA16      mWorldViewProjection;
  D3DXMATRIXA16      mViewProjection;
  HWND               mHwnd;
  LPDIRECT3DDEVICE9  mDevice;
  D3DXVECTOR3        mEyePos;
  ID3DXEffect       *mEffect;

  D3DXHANDLE         mTechnique;
  D3DXHANDLE         mWireFrameTechnique;
  D3DXHANDLE         mWorldViewHandle;
  D3DXHANDLE         mWorldViewProjectionHandle;
  D3DXHANDLE         mDiffuseHandle;
  D3DXHANDLE         mWorldHandle;
  D3DXHANDLE         mViewProjectionHandle;
  D3DXHANDLE         mProjectionHandle;
  D3DXHANDLE         mEyePosHandle;
  D3DXHANDLE         mAmbientColorHandle;
  D3DXHANDLE         mDiffuseColorHandle;
  D3DXHANDLE         mSpecularColorHandle;
  D3DXHANDLE         mSpecularPowerHandle;

  bool               mWireFrame;
  bool				 mShowBackfaces;
  unsigned int       mLineCount;
  LineVertex         mLines[MAXLINES];
};

TetraD3D::TetraD3D(void)
{
	mDirect = new DirectHelper;
}

TetraD3D::~TetraD3D(void)
{
	delete mDirect;
}

void   TetraD3D::setDevice(void *device,void *hwnd)
{
	mDirect->setDevice(device,hwnd);
}

void   TetraD3D::onDeviceReset(void *d3device)
{
	mDirect->onDeviceReset(d3device);
}


void   TetraD3D::setViewProjectionMatrix(const void *view,const void *projection)
{
	mDirect->setViewProjectionMatrix(view,projection);
}


void * TetraD3D::createVertexBuffer(unsigned int vcount,const TetraGraphicsVertex *vertices)
{
	return mDirect->createVertexBuffer(vcount,vertices);
}

bool   TetraD3D::releaseVertexBuffer(void *buffer)
{
	return mDirect->releaseVertexBuffer(buffer);
}

void * TetraD3D::lockVertexBuffer(void *buffer)
{
	return mDirect->lockVertexBuffer(buffer);
}

bool   TetraD3D::unlockVertexBuffer(void *buffer)
{
	return mDirect->unlockVertexBuffer(buffer);
}


void * TetraD3D::createIndexBuffer(unsigned int icount,const unsigned int *indices)
{
	return mDirect->createIndexBuffer(icount,indices);
}

bool   TetraD3D::releaseIndexBuffer(void *ibuffer)
{
	return mDirect->releaseIndexBuffer(ibuffer);
}

void * TetraD3D::lockIndexBuffer(void *buffer)
{
	return mDirect->lockIndexBuffer(buffer);
}

bool   TetraD3D::unlockIndexBuffer(void *buffer)
{
	return mDirect->unlockIndexBuffer(buffer);
}


bool   TetraD3D::renderSection(TetraMaterial *material,void *buffer,void *ibuffer,unsigned int vcount,unsigned int tcount)
{
	return mDirect->renderSection(material,buffer,ibuffer,vcount,tcount);
}



bool   TetraD3D::renderSection(TetraMaterial *material,TetraGraphicsVertex *vbuffer,unsigned int *ibuffer,unsigned int vcount,unsigned int tcount)
{
  return mDirect->renderSection(material,vbuffer,ibuffer,vcount,tcount);
}

bool   TetraD3D::renderSection(TetraMaterial *material,TetraGraphicsVertex *vbuffer,unsigned int vcount)
{
  return mDirect->renderSection(material,vbuffer,vcount);
}

bool   TetraD3D::renderSection(TetraMaterial *material,void *vbuffer,unsigned int vcount)
{
  return mDirect->renderSection(material,vbuffer,vcount);
}

void   TetraD3D::setWireFrame(bool state)
{
  return mDirect->setWireFrame(state);
}

void   TetraD3D::setShowBackfaces(bool state)
{
	return mDirect->setShowBackfaces(state);
}

void   TetraD3D::addLine(unsigned int color,const float *p1,const float *p2)
{
  mDirect->addLine(color,p1,p2);
}

void   TetraD3D::flushLines(void)
{
  mDirect->flushLines();
}

bool   TetraD3D::releaseMaterial(TetraMaterial *tm) // releases device handle for this material if one is present
{
  return mDirect->releaseMaterial(tm);
}

bool   TetraD3D::screenToWorld(int sx,      // screen x position
                       int sy,      // screen y position
                       float *world, // world position of the eye
                       float *direction) // direction vector into the world
{
  return mDirect->screenToWorld(sx,sy,world,direction);
}

void  TetraD3D::wireBox(unsigned int color,const float *bmin,const float *bmax)
{
  mDirect->wireBox(color,bmin,bmax);
}


void   TetraD3D::getEyePos(float *eye)
{
  mDirect->getEyePos(eye);
}

void TetraD3D::setWorldMatrix(const void *world)
{
  mDirect->setWorldMatrix(world);
}

}; // END OF SOFTBODY NAMESPACE
