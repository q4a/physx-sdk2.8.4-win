
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // for tolower()
#include <assert.h>


#include "SoftMesh.h"
#include "SoftMeshObj.h"
#include "NXU_GraphicsMesh.h"
#include "TetraGraphics.h"
#include "SoftVertex.h"
#include "NxArray.h"
#include "NXU_Geometry.h"
#include "NxVec3.h"

namespace SOFTBODY
{

static float GetHexFloat(const char *foo) // convert 8 byte ascii hex stream to a floating point value!
{
	unsigned int ret = 0;
	for (int i=0; i<8; i++)
	{
		if ( !*foo ) break;
		unsigned char c = tolower( *foo );
		unsigned int v = 0;
		if ( c >= '0' && c <= '9' )
			v = c-'0';
		else
		{
			if ( c >= 'a' && c <= 'f' )
			{
				v = 10 + c-'a';
			}
			else
				break;
		}
		ret = (ret<<4)|v;
		foo++;
	}


	const float *fv = (const float *)&ret;

	float fret = *fv;

	return fret;
}

class SoftMeshSection
{
public:
  SoftMeshSection(TetraMaterial *mat)
  {
  	mMaterial = mat;
  	mVertices = 0;
  	mVcount   = 0;
  	mTcount   = 0;
		mVertexBuffer = 0;
		mIndexBuffer = 0;
  }

  ~SoftMeshSection(void)
  {
  	releaseSoftVertexPool(mVertices);
  	if ( gGraphicsInterface )
  	{
  		if ( mVertexBuffer )
  			gGraphicsInterface->releaseVertexBuffer(mVertexBuffer);
  		if ( mIndexBuffer )
  			gGraphicsInterface->releaseIndexBuffer(mIndexBuffer);
  	}

  }

	void addTriangle(const NXU::NxuVertex *v1,const NXU::NxuVertex *v2,const NXU::NxuVertex *v3)
	{
	}

	void addTriangle(const NXU::NxuDeformVertex *v1,const NXU::NxuDeformVertex *v2,const NXU::NxuDeformVertex *v3)
	{
		if ( mVertices == 0 )
			mVertices = createSoftVertexPool(SVT_TETRA_DEFORM_VERTEX);

		unsigned int i1 = getSoftVertexIndex(mVertices,v1);
		unsigned int i2 = getSoftVertexIndex(mVertices,v2);
		unsigned int i3 = getSoftVertexIndex(mVertices,v3);

		mIndices.push_back(i1);
		mIndices.push_back(i2);
		mIndices.push_back(i3);
	}

	void transform44(float *dest,const float *v,const float *matrix,float weight)
	{
    dest[0]+= weight*((matrix[0*4+0] * v[0]) +  (matrix[1*4+0] * v[1]) + (matrix[2*4+0] * v[2]) + matrix[3*4+0]);
    dest[1]+= weight*((matrix[0*4+1] * v[0]) +  (matrix[1*4+1] * v[1]) + (matrix[2*4+1] * v[2]) + matrix[3*4+1]);
    dest[2]+= weight*((matrix[0*4+2] * v[0]) +  (matrix[1*4+2] * v[1]) + (matrix[2*4+2] * v[2]) + matrix[3*4+2]);
	}

	void transform33(float *dest,const float *v,const float *matrix,float weight)
	{
    dest[0]+= weight*((matrix[0*4+0] * v[0]) +  (matrix[1*4+0] * v[1]) + (matrix[2*4+0] * v[2]));
    dest[1]+= weight*((matrix[0*4+1] * v[0]) +  (matrix[1*4+1] * v[1]) + (matrix[2*4+1] * v[2]));
    dest[2]+= weight*((matrix[0*4+2] * v[0]) +  (matrix[1*4+2] * v[1]) + (matrix[2*4+2] * v[2]));
	}

	void weightedTransform44(float *dest,const float *source,const float **transforms,const float *weight,const unsigned short *bone)
	{
		dest[0] = dest[1] = dest[2] = 0;
		transform44(dest,source,transforms[bone[0]],weight[0]);
		transform44(dest,source,transforms[bone[1]],weight[1]);
		transform44(dest,source,transforms[bone[2]],weight[2]);
		transform44(dest,source,transforms[bone[3]],weight[3]);
	}

	void weightedTransform33(float *dest,const float *source,const float **transforms,const float *weight,const unsigned short *bone)
	{
		dest[0] = dest[1] = dest[2] = 0;
		transform33(dest,source,transforms[bone[0]],weight[0]);
		transform33(dest,source,transforms[bone[1]],weight[1]);
		transform33(dest,source,transforms[bone[2]],weight[2]);
		transform33(dest,source,transforms[bone[3]],weight[3]);
	}

	void render(void)
  {
		if ( mVertexBuffer == 0 && mVertices )
		{
			getSoftVertexPoolBuffer(mVertices,mVcount);
			if ( mVcount )
  			mVertexBuffer = gGraphicsInterface->createVertexBuffer(mVcount,0);

			TetraGraphicsVertex *dest =  (TetraGraphicsVertex *)gGraphicsInterface->lockVertexBuffer(mVertexBuffer);

			if ( dest )
			{
				unsigned int vcount;
				const TetraDeformVertex *source = (const TetraDeformVertex *)getSoftVertexPoolBuffer(mVertices,vcount);
 				for (unsigned int	i=0; i<vcount; i++)
 				{
 					dest->mPos[0] 		= source->mPos[0];
 					dest->mPos[1] 		= source->mPos[1];
 					dest->mPos[2] 		= source->mPos[2];

 					dest->mNormal[0] 	= source->mNormal[0];
 					dest->mNormal[1] 	= source->mNormal[1];
 					dest->mNormal[2] 	= source->mNormal[2];

 					dest->mTexel[0]  	= source->mTexel1[0];
 					dest->mTexel[1]  	= source->mTexel1[1];

 					dest++;
 					source++;
 				}
  			gGraphicsInterface->unlockVertexBuffer(mVertexBuffer);
      }
		}

		if ( mIndexBuffer == 0 && mVertices )
		{
			mTcount = mIndices.size()/3;
			if ( mTcount )
			{
				mIndexBuffer = gGraphicsInterface->createIndexBuffer(mTcount*3, &mIndices[0] );
			}
		}

    if ( mIndexBuffer && mVertexBuffer )
    {
  		gGraphicsInterface->renderSection(mMaterial,mVertexBuffer,mIndexBuffer,mVcount,mTcount);
    }

  }

	void render(const float **transforms)
	{
		if ( mVertexBuffer == 0 && mVertices )
		{
			getSoftVertexPoolBuffer(mVertices,mVcount);
			if ( mVcount )
  			mVertexBuffer = gGraphicsInterface->createVertexBuffer(mVcount,0);
		}

		if ( mIndexBuffer == 0 && mVertices )
		{
			mTcount = mIndices.size()/3;
			if ( mTcount )
			{
				mIndexBuffer = gGraphicsInterface->createIndexBuffer(mTcount*3, &mIndices[0] );
			}
		}

	  if ( mVertexBuffer && mIndexBuffer )
	  {
			TetraGraphicsVertex *dest =  (TetraGraphicsVertex *)gGraphicsInterface->lockVertexBuffer(mVertexBuffer);
			if ( dest )
			{
				unsigned int vcount;
				const TetraDeformVertex *source = (const TetraDeformVertex *)getSoftVertexPoolBuffer(mVertices,vcount);
				if ( transforms )
				{
  				for (unsigned int	i=0; i<vcount; i++)
  				{
  					weightedTransform44(dest->mPos,   source->mPos,   transforms, source->mWeight, source->mBone );
  					weightedTransform33(dest->mNormal,source->mNormal,transforms, source->mWeight, source->mBone );

  					dest->mTexel[0]  	= source->mTexel1[0];
  					dest->mTexel[1]  	= source->mTexel1[1];

  					dest++;
  					source++;
  				}
				}
				else
				{
  				for (unsigned int	i=0; i<vcount; i++)
  				{
  					dest->mPos[0] 		= source->mPos[0];
  					dest->mPos[1] 		= source->mPos[1];
  					dest->mPos[2] 		= source->mPos[2];

  					dest->mNormal[0] 	= source->mNormal[0];
  					dest->mNormal[1] 	= source->mNormal[1];
  					dest->mNormal[2] 	= source->mNormal[2];

  					dest->mTexel[0]  	= source->mTexel1[0];
  					dest->mTexel[1]  	= source->mTexel1[1];

  					dest++;
  					source++;
  				}
  			}
				gGraphicsInterface->unlockVertexBuffer(mVertexBuffer);
				gGraphicsInterface->renderSection(mMaterial,mVertexBuffer,mIndexBuffer,mVcount,mTcount);
			}
		}

	}

	void onDeviceReset(void *device)
	{
		if ( gGraphicsInterface )
		{
			gGraphicsInterface->releaseVertexBuffer(mVertexBuffer);
			gGraphicsInterface->releaseIndexBuffer(mIndexBuffer);
			mVertexBuffer = 0;
			mIndexBuffer = 0;
		}
	}

private:
  TetraMaterial	         *mMaterial;
  SoftVertexPool         *mVertices;
  NxArray< unsigned int > mIndices;
  unsigned int            mVcount;
  unsigned int            mTcount;
  void                   *mVertexBuffer;
  void                   *mIndexBuffer;
};

class SoftMesh
{
public:

  SoftMesh(const char *name)
  {
  	strncpy(mName,name,512);
  }

  ~SoftMesh(void)
  {
  	for (unsigned int i=0; i<mSections.size(); i++)
  	{
  		SoftMeshSection *sms = mSections[i];
  		delete sms;
  	}
  }

	SoftMeshSection *addSoftMeshSection(TetraMaterial *tm)
	{
		SoftMeshSection *sms = new SoftMeshSection(tm);
		mSections.push_back(sms);
		return sms;
	}

  void render(const float **transforms)
  {
  	for (unsigned int i=0; i<mSections.size(); i++)
  	{
  		mSections[i]->render(transforms);
  	}
  }

  void render(void)
  {
  	for (unsigned int i=0; i<mSections.size(); i++)
  	{
  		mSections[i]->render();
  	}
  }

  void onDeviceReset(void *device)
  {
  	for (unsigned int i=0; i<mSections.size(); i++)
  	{
			SoftMeshSection *sms = mSections[i];
			sms->onDeviceReset(device);
  	}
  }

private:
  char	                      mName[512];
  NxArray< SoftMeshSection *> mSections;
};

class SoftMeshInstance
{
public:
private:
	SoftMesh *mSoftMesh;       // the mesh to instance.
	float	    mTransform[16];      // the 4x4 matrix transform.
};

static void set(NXU::NxuVertex &v,const NxVec3 & p,const NxVec3 &n)
{
  v.mPos[0] = p.x;
  v.mPos[1] = p.y;
  v.mPos[2] = p.z;

  v.mNormal[0] = n.x;
  v.mNormal[1] = n.y;
  v.mNormal[2] = n.z;

  v.mTexel1[0] = 0;
  v.mTexel1[1] = 0;

  v.mTexel2[0] = 0;
  v.mTexel2[1] = 0;

	const float *tp1 = v.mPos;

	int i1 = 0;
	int i2 = 0;

	float nx = fabsf(n[0]);
	float ny = fabsf(n[1]);
	float nz = fabsf(n[2]);

	if ( nx <= ny && nx <= nz ) i1 = 0;
	if ( ny <= nx && ny <= nz ) i1 = 1;
	if ( nz <= nx && nz <= ny ) i1 = 2;

	switch ( i1 )
	{
		case 0:
			if ( ny < nz )
				i2 = 1;
			else
				i2 = 2;
			break;
		case 1:
			if ( nx < nz )
				i2 = 0;
			else
				i2 = 2;
			break;
		case 2:
			if ( nx < ny )
				i2 = 0;
			else
				i2 = 1;
			break;
	}

	v.mTexel1[0] = tp1[i1]*0.25f;
	v.mTexel1[1] = tp1[i2]*0.25f;

	v.mTexel2[0] = tp1[i1]*0.05f;
	v.mTexel2[1] = tp1[i2]*0.05f;

}

class SoftMeshSystem : public NXU::NxuGraphicsInterface, public SoftMeshInterface
{
public:

  // loading straight from a raw triangle mesh..
	SoftMeshSystem(const char *fname,const char *shader,unsigned int vcount,const float *vertices,unsigned int tcount,const unsigned int *indices)
	{
  	strncpy(mName,fname,512);
  	mCurrentMesh = 0;
  	mCurrentMeshSection = 0;
  	mBuildWeighting = false;
		mInferredSkeleton = 0;

    NodeMesh(fname,0);
    if ( shader )
      NodeMaterial(shader, 0 );

    const unsigned int *idx = indices;
    for (unsigned int i=0; i<tcount; i++)
    {
      unsigned int i1 = *idx++;
      unsigned int i2 = *idx++;
      unsigned int i3 = *idx++;

      NxVec3 p1(&vertices[i1*3]);
      NxVec3 p2(&vertices[i2*3]);
      NxVec3 p3(&vertices[i3*3]);

      NxVec3 normal;
      normal.cross( p2 - p1, p3-p1 ); // compute the facenormal
      normal.normalize();

      NXU::NxuVertex v1,v2,v3;

      set(v1,p1,normal);
      set(v2,p2,normal);
      set(v3,p3,normal);

      NodeTriangle(&v1,&v2,&v3);


    }

  	mCurrentMesh = 0;
  	mCurrentMeshSection = 0;
	}

  SoftMeshSystem(const char *fname,NXU::NxuSkeleton *skeleton)
  {
  	strncpy(mName,fname,512);
  	if ( gFileInterface && *fname != '@'  )
  		fname = gFileInterface->getSoftFileName(fname,true);
  	mCurrentMesh = 0;
  	mCurrentMeshSection = 0;
  	mBuildWeighting = false;
		mInferredSkeleton = 0;

		if ( *fname == '@' )
		{
			char foo[512];
			strcpy(foo,fname);

			char *shader = strchr(foo,'(');

			if ( shader )
			{
				*shader = 0;
				shader++;
				char *stop = strchr(foo,')');
				if ( stop ) *stop = 0;
			}

			int len = (int)strlen(foo);

      NXU::NxuGeometry g;

      bool ok = false;

			//           1         2
			// 0123456789012345678901234567
			// @box012345670123456701234567"
			if ( strncmp(foo,"@box",4) == 0 && len == 28)
			{
				NxVec3 dimensions;

				dimensions.x = GetHexFloat( &foo[4]  );
				dimensions.y = GetHexFloat( &foo[12] );
				dimensions.z = GetHexFloat( &foo[20] );

        ok =  NXU::createBox(dimensions,g);
			}
			//                     012345678901234
			//                     @sphere01234567
			else if ( strncmp(foo,"@sphere",7) == 0 && len == 15 )
			{
				float radius = GetHexFloat( &foo[7] );
        ok = NXU::createSphere(radius, g );
			}
			//           1         2
			// 012345678901234567890123
			// @capsule0123456701234567
			else if ( strncmp(foo,"@capsule",8) == 0 && len == 24 )
			{
				float radius = GetHexFloat(&foo[8]);
				float height = GetHexFloat(&foo[16]);
        ok = NXU::createCapsule(radius,height,g);
			}
      if ( ok ) // we are going to send it to the
      {
        NodeMesh(foo,0);
        if ( shader )
          NodeMaterial(shader, 0 );
				else
				  NodeMaterial("wood.dds",0);

        const unsigned int *idx = g.mIndices;
        for (unsigned int i=0; i<g.mTcount; i++)
        {
          unsigned int i1 = *idx++;
          unsigned int i2 = *idx++;
          unsigned int i3 = *idx++;

          NxVec3 p1(&g.mVertices[i1*3]);
          NxVec3 p2(&g.mVertices[i2*3]);
          NxVec3 p3(&g.mVertices[i3*3]);

          NxVec3 normal;
          normal.cross( p2 - p1, p3-p1 ); // compute the facenormal
          normal.normalize();

          NXU::NxuVertex v1,v2,v3;

          set(v1,p1,normal);
          set(v2,p2,normal);
          set(v3,p3,normal);

          NodeTriangle(&v1,&v2,&v3);


        }
      }
		}
		else
		{
			if ( skeleton )
			{
				mBuildWeighting = true;
				mInferredSkeleton = new NXU::NxuSkeleton(*skeleton);
			}

			if ( strstr(fname,".obj") )
			{
				loadSoftMeshObj(fname,0,this);
			}
			else
			{
				NXU::NxuLoadGraphicsMesh(fname,this);
			}

  		if ( mBuildWeighting )
  		{
    		mSkeletons.push_back(mInferredSkeleton);
			}
			else
			{
    		delete mInferredSkeleton;
    		mInferredSkeleton = 0;
			}
		}

  	mCurrentMesh = 0;
  	mCurrentMeshSection = 0;

  }

  ~SoftMeshSystem(void)
  {
		for (unsigned int i=0; i<mSoftMeshes.size(); i++)
		{
			SoftMesh *sm = mSoftMeshes[i];
			delete sm;
		}

		for (unsigned int i=0; i<mSoftMeshInstances.size(); i++)
		{
			SoftMeshInstance *smi	= mSoftMeshInstances[i];
			delete smi;
		}

		for (unsigned int i=0; i<mSkeletons.size(); i++)
		{
			NXU::NxuSkeleton *sk = mSkeletons[i];
			delete sk;
		}

		for (unsigned int i=0; i<mMaterials.size(); i++)
		{
			TetraMaterial *tm = mMaterials[i];
			delete tm;
		}
  }

	void render(const float **transforms)
	{
    gGraphicsInterface->setWorldMatrix(0);
		for (unsigned int i=0; i<mSoftMeshes.size(); i++)
		{
			SoftMesh *sm = mSoftMeshes[i];
			sm->render(transforms);
		}
	}

	void NodeMaterial(const char *name,const char *info)
	{
		if ( mCurrentMesh == 0 )
		{
			mCurrentMesh = new SoftMesh("mesh");
			mSoftMeshes.push_back(mCurrentMesh);
		}

		TetraMaterial *tm = 0;

		for (unsigned int i=0; i<mMaterials.size(); i++)
		{
			TetraMaterial *t = mMaterials[i];
			if ( stricmp(name,t->mName) == 0 )
			{
				tm = t;
				break;
			}
		}
		if ( tm == 0 )
		{
			tm = new TetraMaterial(name,name,0x505050,0xFFFFFFFF,0xFFFFFFFF,32.0f);
			mMaterials.push_back(tm);
		}
		mCurrentMeshSection = mCurrentMesh->addSoftMeshSection(tm);
	}

	void NodeMesh(const char *name,const char *info)
	{
		mCurrentMesh = new SoftMesh(name);
		mSoftMeshes.push_back(mCurrentMesh);
	}



	void NodeTriangleList(int vcount,const NXU::NxuVertex *vertex,int icount,const unsigned short *indices)
	{
		unsigned int tcount = icount/3;

		for (unsigned int i=0; i<tcount; i++)
		{
			unsigned int i1 = *indices++;
			unsigned int i2 = *indices++;
			unsigned int i3 = *indices++;
			NodeTriangle( &vertex[i1], &vertex[i2], &vertex[i3]);
		}
	}

	void NodeTriangleList(int vcount,const NXU::NxuDeformVertex *vertex,int icount,const unsigned short *indices)
	{
		unsigned int tcount = icount/3;
		for (unsigned int i=0; i<tcount; i++)
		{
			unsigned int i1 = *indices++;
			unsigned int i2 = *indices++;
			unsigned int i3 = *indices++;
			NodeTriangle(&vertex[i1],&vertex[i2],&vertex[i3]);
		}
	}


	void NodeTriangle(const NXU::NxuVertex *v1,const NXU::NxuVertex *v2,const NXU::NxuVertex *v3)
	{
		NXU::NxuDeformVertex dv1(*v1);
		NXU::NxuDeformVertex dv2(*v2);
		NXU::NxuDeformVertex dv3(*v3);

		NodeTriangle(&dv1,&dv2,&dv3);

	}

	void NodeTriangle(const NXU::NxuDeformVertex *v1,const NXU::NxuDeformVertex *v2,const NXU::NxuDeformVertex *v3)
	{
    if ( mBuildWeighting )
      addTriangle(v1,v2,v3,true,0.15f,0);
    else
      finalTriangle(v1,v2,v3);
  }

  float distanceSquared(const float *p1,const float *p2)
  {
    float dx = p1[0] - p2[0];
    float dy = p1[1] - p2[1];
    float dz = p1[2] - p2[2];
    return dx*dx + dy*dy + dz*dz;
  }

  void addTriangle(const NXU::NxuDeformVertex *v1,const NXU::NxuDeformVertex *v2,const NXU::NxuDeformVertex *v3,bool tesselate,float tesselateDepth,int recurse)
  {
  	bool split = false;
  	float l1,l2,l3;

  	if ( tesselate && recurse < 8 )
  	{
  	  l1 = distanceSquared(v1->mPos,v2->mPos);
    	l2 = distanceSquared(v2->mPos,v3->mPos);
    	l3 = distanceSquared(v3->mPos,v1->mPos);

    	float td = tesselateDepth*tesselateDepth;

  	  if (  l1 > td || l2 > td || l3 > td )
  	  	split = true;

    }

    if ( split )
  	{
  		int edge;

  		if ( l1 >= l2 && l1 >= l3 )
  			edge = 0;
  		else if ( l2 >= l1 && l2 >= l3 )
  			edge = 1;
  		else
  			edge = 2;

  		switch ( edge )
  		{
  			case 0:
  				if ( 1 )
  				{
  					NXU::NxuDeformVertex split;
  					split.Lerp(*v1,*v2,0.5f);
  					addTriangle( v1, &split, v3, tesselate, tesselateDepth, recurse+1 );
  					addTriangle( &split, v2, v3, tesselate, tesselateDepth, recurse+1  );
  				}
  				break;
  			case 1:
  				if ( 1 )
  				{
  					NXU::NxuDeformVertex split;
  					split.Lerp(*v2,*v3,0.5f);
  					addTriangle(v1,v2,&split, tesselate, tesselateDepth, recurse+1 );
  					addTriangle(v1,&split,v3, tesselate, tesselateDepth, recurse+1 );
  				}
  				break;
  			case 2:
  				if ( 1 )
  				{
  					NXU::NxuDeformVertex split;
  					split.Lerp(*v3,*v1,0.5f);
  					addTriangle(v1,v2,&split, tesselate, tesselateDepth, recurse+1 );
  					addTriangle(&split,v2,v3, tesselate, tesselateDepth, recurse+1 );
  				}
  				break;
  		}
  	}
  	else
  	{
      finalTriangle(v1,v2,v3);
    }

  }


	void finalTriangle(const NXU::NxuDeformVertex *v1,const NXU::NxuDeformVertex *v2,const NXU::NxuDeformVertex *v3)
  {
		NXU::NxuDeformVertex dv1 = *v1;
		NXU::NxuDeformVertex dv2 = *v2;
		NXU::NxuDeformVertex dv3 = *v3;

		if ( mBuildWeighting && mInferredSkeleton )
		{
			mInferredSkeleton->ComputeDefaultWeighting(dv1.mPos, dv1.mWeight, dv1.mNxuBone[0], dv1.mNxuBone[1], dv1.mNxuBone[2], dv1.mNxuBone[3] );
			mInferredSkeleton->ComputeDefaultWeighting(dv2.mPos, dv2.mWeight, dv2.mNxuBone[0], dv2.mNxuBone[1], dv2.mNxuBone[2], dv2.mNxuBone[3] );
			mInferredSkeleton->ComputeDefaultWeighting(dv3.mPos, dv3.mWeight, dv3.mNxuBone[0], dv3.mNxuBone[1], dv3.mNxuBone[2], dv3.mNxuBone[3] );

		}

		if ( mCurrentMesh == 0 )
		{
			NodeMaterial("wood.dds",0);
		}

		if ( mCurrentMeshSection )
			mCurrentMeshSection->addTriangle(&dv1,&dv2,&dv3);
	}

	// add a NxuSkeleton
	void NodeSkeleton(const NXU::NxuSkeleton *NxuSkeleton)
	{
		if ( NxuSkeleton->GetNxuBoneCount() > 1 )
		{
		  mBuildWeighting = false;
		  NXU::NxuSkeleton *skel = new NXU::NxuSkeleton(*NxuSkeleton);
		  mSkeletons.push_back(skel);
		}
	}

	void NodeAnimation(const NXU::NxuAnimation *NxuAnimation)
	{
	}

	void NodeInstance(const char *name,const float *transform)
	{
	}

  void NodeTetrahedron(const float *p1,const float *p2,const float *p3,const float *p4)
  {
  }

	void softMeshMaterial(const TetraMaterial &tm)
	{
		NodeMaterial(tm.mTexture,tm.mName);
	}

	void get(NXU::NxuDeformVertex &d,const TetraGraphicsVertex &v)
	{
		d.mPos[0] = v.mPos[0];
		d.mPos[1] = v.mPos[1];
		d.mPos[2] = v.mPos[2];
		d.mNormal[0] = v.mNormal[0];
		d.mNormal[1] = v.mNormal[1];
		d.mNormal[2] = v.mNormal[2];
		d.mTexel1[0] = v.mTexel[0];
		d.mTexel1[1] = v.mTexel[1];
		d.mTexel2[0] = v.mTexel[0];
		d.mTexel2[1] = v.mTexel[1];
		d.mWeight[0] = 1.0f;
		d.mWeight[1] = 0.0f;
		d.mWeight[2] = 0.0f;
		d.mWeight[3] = 0.0f;
		d.mNxuBone[0] = 0;
		d.mNxuBone[1] = 0;
		d.mNxuBone[2] = 0;
		d.mNxuBone[3] = 0;
	}

	void softMeshTriangle(const TetraGraphicsVertex &v1,const TetraGraphicsVertex &v2,const TetraGraphicsVertex &v3)
	{
		NXU::NxuDeformVertex dv1,dv2,dv3;
		get(dv1,v1);
		get(dv2,v2);
		get(dv3,v3);
		NodeTriangle(&dv1,&dv2,&dv3);
	}

	void softMeshTetrahedron(const float *p1,const float *p2,const float *p3,const float *p4)
	{
	}

	NXU::NxuSkeleton * getSkeleton(void) const
	{
		NXU::NxuSkeleton *ret = 0;
		if ( mSkeletons.size() )
			ret = mSkeletons[0];
		return ret;
	}

	void onDeviceReset(void *device)
	{
		for (unsigned int i=0; i<mMaterials.size(); i++)
		{
			TetraMaterial *tm = mMaterials[i];
			gGraphicsInterface->releaseMaterial(tm);
		}
		for (unsigned int i=0; i<mSoftMeshes.size(); i++)
		{
			SoftMesh *sm = mSoftMeshes[i];
			sm->onDeviceReset(device);
		}
	}

	const char *getName(void) const { return mName; };

  void render(const float *transform)
  {
    gGraphicsInterface->setWorldMatrix(transform);
		for (unsigned int i=0; i<mSoftMeshes.size(); i++)
		{
			SoftMesh *sm = mSoftMeshes[i];
			sm->render();
		}
  }

private:
  char	                   			mName[512];          // name of the mesh system.
  bool                          mBuildWeighting;     // build the bone weighting on the fly..
  NxArray< SoftMesh * >    			mSoftMeshes;         // collection of unique mesh components.
  NxArray< SoftMeshInstance *> 	mSoftMeshInstances;  // mesh instances.
  NxArray< NXU::NxuSkeleton *> 	mSkeletons;          // skeletons encountered in the mesh system.
  NxArray< TetraMaterial *>			mMaterials;          // materials used by the mesh system.
  SoftMesh                     *mCurrentMesh;
  SoftMeshSection              *mCurrentMeshSection;
  NXU::NxuSkeleton             *mInferredSkeleton;
};

SoftMeshSystem * createSoftMeshSystem(const char *fname,NXU::NxuSkeleton *skeleton)
{
	SoftMeshSystem *ret = 0;

	ret = new SoftMeshSystem(fname,skeleton);

  return ret;
}

void       releaseSoftMeshSystem(SoftMeshSystem *smesh)
{
  delete smesh;
}


void       renderSoftMeshSystem(SoftMeshSystem *smesh,const float **transforms)
{
	if ( smesh )
  	smesh->render(transforms);
}

void       renderSoftMeshSystem(SoftMeshSystem *smesh,const float *transform)
{
	if ( smesh )
  	smesh->render(transform);
}


NXU::NxuSkeleton * getSkeleton(SoftMeshSystem *smesh) // return a pointer to the graphics skeleton if there is one.
{
	NXU::NxuSkeleton *ret = 0;
	if ( smesh )
		ret = smesh->getSkeleton();
	return ret;
}


void       onDeviceReset(SoftMeshSystem *smesh,void *device)
{
	if ( smesh )
		smesh->onDeviceReset(device);
}

NxArray< SoftMeshSystem * > gSoftMeshes;

SoftMeshSystem * findMeshSystem(const char *fname)
{
	SoftMeshSystem *ret = 0;

	for (NxU32 i=0; i<gSoftMeshes.size(); i++)
	{
		SoftMeshSystem *sms = gSoftMeshes[i];
		if ( strcmp(sms->getName(),fname) == 0 )
		{
			ret = sms;
			break;
		}
	}
	return ret;
}

SoftMeshSystem    * locateSoftMeshSystem(const char *fname)
{
	SoftMeshSystem *ret = 0;

  ret = findMeshSystem(fname);

	if ( !ret )
	{
		ret = new SoftMeshSystem(fname,0);
		gSoftMeshes.push_back(ret);
	}

	return ret;
}


SoftMeshSystem *   locateBox(const char *shader,const float *sides) // return a mesh asset for a box of this size.
{
	SoftMeshSystem *ret = 0;

	char scratch[512];

	unsigned int *iv1 = (unsigned int *) sides;
	unsigned int *iv2 = (unsigned int *) sides+1;
	unsigned int *iv3 = (unsigned int *) sides+2;

	sprintf(scratch,"@box%8x%8x%8x", *iv1, *iv2, *iv3 );
	if ( shader && strlen(shader) )
	{
		char temp[512];
		sprintf(temp,"(%s)",shader);
		strcat(scratch,temp);
	}

	ret = locateSoftMeshSystem(scratch);

	return ret;
}

SoftMeshSystem *   locatePlane(const char *shader,const float *plane) // return a mesh asset for a box of this size.
{
	SoftMeshSystem *ret = 0;

  return ret;
}

SoftMeshSystem *   locateSphere(const char *shader,float radius)
{
	SoftMeshSystem *ret = 0;

	char scratch[512];
	unsigned int *iv1 = (unsigned int *) &radius;
	sprintf(scratch,"@sphere%8x", *iv1 );

	if ( shader && strlen(shader) )
	{
		char temp[512];
		sprintf(temp,"(%s)",shader);
		strcat(scratch,temp);
	}

	ret = locateSoftMeshSystem(scratch);

  return ret;
}

SoftMeshSystem *   locateCapsule(const char *shader,float radius,float height)
{
	SoftMeshSystem *ret = 0;

	char scratch[512];
	unsigned int *iv1 = (unsigned int *) &radius;
	unsigned int *iv2 = (unsigned int *) &height;
	sprintf(scratch,"@capsule%8x%8x", *iv1, *iv2 );

	if ( shader && strlen(shader) )
	{
		char temp[512];
		sprintf(temp,"(%s)",shader);
		strcat(scratch,temp);
	}

	ret = locateSoftMeshSystem(scratch);

  return ret;
}

SoftMeshSystem *   locateTriangleMesh(const char *name,const char *shader,unsigned int vcount,const float *verts,unsigned int tcount,const unsigned int *indices)
{
	SoftMeshSystem *ret = 0;

  ret = findMeshSystem(name);

	if ( !ret )
	{
		ret = new SoftMeshSystem(name,shader,vcount,verts,tcount,indices);
		gSoftMeshes.push_back(ret);
	}

	return ret;
}

void  sm_onDeviceReset(void *device) // reset all mesh systems
{
  	for (NxU32 i=0; i<gSoftMeshes.size(); i++)
  	{
  		SoftMeshSystem *sms = gSoftMeshes[i];
        sms->onDeviceReset(device);
  	}
}

void  releaseSoftMeshSystems(void) // reset all mesh systems
{
  	for (NxU32 i=0; i<gSoftMeshes.size(); i++)
  	{
  		SoftMeshSystem *sms = gSoftMeshes[i];
      sms->onDeviceReset(0);
      releaseSoftMeshSystem(sms);
  	}
    gSoftMeshes.clear();
}

}; // end of namespace
