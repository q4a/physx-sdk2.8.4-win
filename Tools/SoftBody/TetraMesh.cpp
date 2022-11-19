#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TetraMesh.h"
#include "MeshHash.h"
#include "SoftMeshObj.h"
#include "SoftMeshEZM.h"
#include "SoftMeshPSK.h"
#include "SoftVertex.h"
#include "NXU_helper.h"

#include "NxPhysics.h"
#if NX_USE_SOFTBODY_API
#include "softbody/NxSoftBodyMeshDesc.h"
#include "softbody/NxSoftBodyMesh.h"
#include "softbody/NxSoftBodyDesc.h"
#include "softbody/NxSoftBody.h"
#endif

class NxSoftBodyMesh;
class NxSoftBodyMeshDesc;
class NxSoftBody;
class NxSoftBodyDesc;

namespace SOFTBODY
{

TetraModelSection::TetraModelSection(TetraMaterial *mat)
{
	mMaterial = mat;
	mIndexBuffer = 0;
}

TetraModelSection::~TetraModelSection(void)
{
	if ( mIndexBuffer )
	{
		gGraphicsInterface->releaseIndexBuffer(mIndexBuffer);
		mIndexBuffer = 0;
	}
}

static const char *lastDot(const char *src)
{
  const char *dot = strchr(src,'.');
  const char *lastdot = dot;
  while ( dot )
  {
    lastdot = dot;
    dot = strchr(dot+1,'.');
  }
  return lastdot;
}

TetraModel::TetraModel(const char *obj,const char *tet)
{
  mGraphicsBounds.setEmpty();
	mVertexPool = 0;
	mTetraPool = 0;
	mVertexBuffer = 0;

	if ( obj )
    strncpy(mObjName,obj,512);
	else
		mObjName[0] = 0;

	if ( tet )
    strncpy(mTetName,tet,512);
	else
		mTetName[0] = 0;

  mNxSoftBodyMesh = 0;
  mTetraMesh = 0;

  mVertexPool = createSoftVertexPool(SVT_TETRA_VERTEX);
  mTetraPool  = createSoftVertexPool(SVT_TETRA_POSITION_VERTEX);
	mCurrentSection = 0;

	bool ok = false;

  const char *dot = lastDot(obj);
  if ( dot && stricmp(dot,".ezm") == 0 )
  {
	  ok = loadSoftMeshEZM(obj,this);
  }
  else if ( dot && stricmp(dot,".psk") == 0 )
  {
	  if(SOFTBODY::gUserLoadSoftMeshPSK) ok = (*SOFTBODY::gUserLoadSoftMeshPSK)(obj,this); //loadSoftMeshPSK(obj,this);
  }
  else
  {
	  ok = loadSoftMeshObj(obj,tet,this);
  }

  NxU32 vcount = 0;

	TetraVertex *vlist = (TetraVertex *)getSoftVertexPoolBuffer(mVertexPool,vcount);

	for (NxU32 i=0; i<vcount; i++)
	{
		mVertices.push_back( *vlist++ );
	}

	if ( mTetraMesh )
	{
		NxU32 vcount = 0;
		NxVec3 *vlist = (NxVec3 *) getSoftVertexPoolBuffer(mTetraPool,vcount);
		for (NxU32 i=0; i<vcount; i++)
		{
			mTetraMesh->mPositions.push_back( *vlist++ );
		}
	}

	releaseSoftVertexPool(mVertexPool);
	releaseSoftVertexPool(mTetraPool);

	mVertexPool = 0;
	mTetraPool  = 0;

  // build the mean unit normals.
  if ( mVertices.size() )
  {
    TetraVertex *vlist = &mVertices[0];
   	for (NxU32 i=0; i<mSections.size(); i++)
   	{
   		TetraModelSection *ms = mSections[i];
       ms->applyNormals(vlist);
   	}
    NxU32 vcount = mVertices.size();
    for (NxU32 i=0; i<vcount; i++)
    {
      vlist->mNormal.normalize();
      vlist++;
    }
  }


  createLinks(); // optimize the links in memory

}

TetraModel::~TetraModel(void)
{
  delete mTetraMesh;

  for (NxU32 i=0; i<mSections.size(); i++)
  {
  	TetraModelSection *tms = mSections[i];
  	delete tms;
  }

  for (NxU32 i=0; i<mMaterials.size(); i++)
  {
  	TetraMaterial *tm = mMaterials[i];
    gGraphicsInterface->releaseMaterial(tm);
  	delete tm;
  }

	if ( mVertexBuffer )
	{
		gGraphicsInterface->releaseVertexBuffer(mVertexBuffer);
	}

}


bool TetraModel::isMatch(const char *obj) const
{
  bool ret = false;

  if ( stricmp(obj,mObjName) == 0 )
    ret = true;

  return ret;
}

bool TetraModel::isOk(void) const
{
  bool ret = false;

  if ( mTetraMesh )
    ret = true;
	if ( mVertices.size() )
		ret = true;

  return ret;
}


TetraMaterial *TetraModel::locateMaterial(const char *name)
{
	TetraMaterial *ret = 0;

	for (NxU32 i=0; i<mMaterials.size(); i++)
	{
		TetraMaterial *tet = mMaterials[i];
		if ( strcmp(tet->mName,name) == 0 )
		{
			ret = tet;
			break;
		}
	}
  return ret;
}


void TetraModel::render(const NxVec3 *vertices,const NxU32 *indices)
{

 	if ( mVertexBuffer == 0)
 	{
 		NxArray< TetraGraphicsVertex > vlist;
 		for (NxU32 i=0; i<mVertices.size(); i++)
 		{
 			TetraGraphicsVertex tv;
 			mVertices[i].copyTo(tv);
 			vlist.push_back(tv);
 		}
 		mVertexBuffer = gGraphicsInterface->createVertexBuffer( vlist.size(), &vlist[0] );
 	}

 	if ( mTetraMesh  )
 	{
    TetraGraphicsVertex *gverts = 0;
    if ( mVertexBuffer )
    {
     	gverts = (TetraGraphicsVertex *)gGraphicsInterface->lockVertexBuffer(mVertexBuffer);
      if ( gverts )
      {
       	TetraVertex *tverts = &mVertices[0];

       	mTetraMesh->applyLinks(tverts,mVertices.size(),vertices,indices,gverts);

      	for (NxU32 i=0; i<mSections.size(); i++)
      	{
      		TetraModelSection *ms = mSections[i];
          ms->applyNormals(gverts);
      	}

        gGraphicsInterface->unlockVertexBuffer(mVertexBuffer);
      }
    }
  }


 	for (NxU32 i=0; i<mSections.size(); i++)
 	{
 		TetraModelSection *ms = mSections[i];
 		ms->render(mVertices,mVertexBuffer);
 	}
}


void TetraModelSection::render(const NxArray< TetraVertex > &vertices,void *vbuffer)
{

	if ( mIndexBuffer == 0 )
	{
		mIndexBuffer = gGraphicsInterface->createIndexBuffer( mIndices.size(), &mIndices[0] );
	}

	if ( mIndexBuffer )
	{
		gGraphicsInterface->renderSection(mMaterial,vbuffer,mIndexBuffer,vertices.size(), mIndices.size()/3);
  }
}


void TetraMesh::applyLinks(TetraVertex        *tverts,        // source vertices (backing store)
                          NxU32                tvcount,
                          const NxVec3        *vertices,      // tetrahedral vertices
                          const NxU32         *indices,       // tetrahedral indices
                          TetraGraphicsVertex *gverts)        // output graphics vertices
{
  TetraVertex *tv = tverts;
  TetraGraphicsVertex *gv = gverts;

  for (NxU32 i=0; i<tvcount; i++)
  {
   	const NxU32 *tet = &indices[tv->mIndex*4];

   	const NxVec3 &p0 = vertices[*tet++];
   	const NxVec3 &p1 = vertices[*tet++];
   	const NxVec3 &p2 = vertices[*tet++];
   	const NxVec3 &p3 = vertices[*tet];

    const NxVec3 &b  = tv->mBarycentric;

 		NxVec3 p = p0 * b.x + p1 * b.y + p2 * b.z + p3 * (1.0f - b.x - b.y - b.z);

		gv->mPos[0] = p.x;
		gv->mPos[1] = p.y;
		gv->mPos[2] = p.z;

    gv->mTexel[0] = tv->mTexel[0];
    gv->mTexel[1] = tv->mTexel[1];

		gv->mNormal[0] = 0;
		gv->mNormal[1] = 0;
		gv->mNormal[2] = 0;

    tv++;
    gv++;
  }

}

void TetraModel::render(void)
{
	if ( mVertexBuffer == 0 )
	{
		NxArray< TetraGraphicsVertex > vlist;
		for (NxU32 i=0; i<mVertices.size(); i++)
		{
			TetraGraphicsVertex tv;
			mVertices[i].copyTo(tv);
			vlist.push_back(tv);
		}
		mVertexBuffer = gGraphicsInterface->createVertexBuffer( vlist.size(), &vlist[0] );
	}

	for (NxU32 i=0; i<mSections.size(); i++)
	{
		TetraModelSection *ms = mSections[i];
		ms->render(mVertices,mVertexBuffer);
	}
}

const TetraPositionMesh& TetraModel::getPositionMesh(void)
{
  if ( mPositionMesh.mPositions.empty() && mVertices.size() )
  {

		SoftVertexPool *vpool = createSoftVertexPool(SVT_TETRA_POSITION_VERTEX);

    NxU32 vcount = mVertices.size();
    for (NxU32 i=0; i<vcount; i++)
    {
      NxVec3 v = mVertices[i].mPos;
      NxU32  remap = getSoftVertexIndex(vpool,&v.x);
      mPositionMesh.mTranslationTable.push_back(remap);
    }

		vcount=0;
		const NxVec3 *v = (const NxVec3 *) getSoftVertexPoolBuffer(vpool,vcount);
    for (NxU32 i=0; i<vcount; i++)
    {
      mPositionMesh.mPositions.push_back( *v );
      v++;
    }

		releaseSoftVertexPool(vpool);

    NxU32 *translate = &mPositionMesh.mTranslationTable[0];

    // copy all of the indices from all of the model sections but translate them to the
    // positions only representation.
  	for (NxU32 i=0; i<mSections.size(); i++)
  	{
  		TetraModelSection *ms = mSections[i];
      NxU32 tcount = ms->mIndices.size()/3;
      NxU32 *idx   = &ms->mIndices[0];
      for (NxU32 j=0; j<tcount; j++)
      {
        NxU32 i1 = translate[*idx++];
        NxU32 i2 = translate[*idx++];
        NxU32 i3 = translate[*idx++];
        mPositionMesh.mIndices.push_back(i1);
        mPositionMesh.mIndices.push_back(i2);
        mPositionMesh.mIndices.push_back(i3);
      }
  	}
  }
  return mPositionMesh;
}

void TetraModel::clearTetraMesh(void)
{
  delete mTetraMesh;
  mTetraMesh = 0;
  gGraphicsInterface->releaseVertexBuffer(mVertexBuffer);
  mVertexBuffer = 0;
}

void TetraModel::setTetraMesh(NxU32 vcount,const float *vertices,NxU32 tcount,const unsigned int *indices)
{
  if ( mTetraMesh )
    delete mTetraMesh;
  mTetraMesh = new TetraMesh(vcount,vertices,tcount,indices);
}


TetraMesh::TetraMesh(NxU32 vcount,const float *vertices,NxU32 tcount,const unsigned int *indices)
{
  mLastVcount = 0;
  mLastTcount = 0;
  mDeletions = 0;
  mMaterial = 0;
	mTetraVbuffer = 0;
  mBoxMaterial = 0;
  mBoxVbuffer = 0;

  mBounds.setEmpty();

  for (NxU32 i=0; i<vcount; i++)
  {
    NxVec3 v(vertices);
    mPositions.push_back(v);
    mBounds.include(v);
    vertices+=3;
  }
  for (NxU32 i=0; i<tcount*4; i++)
  {
    mIndices.push_back( *indices++ );
  }
}

TetraMesh::~TetraMesh(void)
{

  gGraphicsInterface->releaseMaterial(mBoxMaterial);
  gGraphicsInterface->releaseMaterial(mMaterial);
  gGraphicsInterface->releaseVertexBuffer(mTetraVbuffer);
  gGraphicsInterface->releaseVertexBuffer(mBoxVbuffer);

  delete mBoxMaterial;
  delete mMaterial;
}

static void getVtx(TetraGraphicsVertex &g,const NxVec3 &p,const NxVec3 &n)
{
  g.mPos[0] = p.x;
  g.mPos[1] = p.y;
  g.mPos[2] = p.z;
  g.mNormal[0] = n.x;
  g.mNormal[1] = n.y;
  g.mNormal[2] = n.z;
  g.mTexel[0] = 0;
  g.mTexel[1] = 0;
}

static TetraGraphicsVertex * addGraphicsTri(const NxVec3 &p1,const NxVec3 &p2,const NxVec3 &p3,TetraGraphicsVertex *dest)
{

  NxVec3 normal;
  normal.cross( p2 - p1, p3-p1 ); // compute the facenormal
  normal.normalize();

  getVtx(*dest++,p1,normal);
  getVtx(*dest++,p2,normal);
  getVtx(*dest++,p3,normal);

  return dest;
}


static void project(const NxVec3 &center,NxVec3 &p,float distance)
{
	NxVec3 dir = p-center;
	dir.normalize();
	dir*=distance;
	p = p-dir;
}

static void shrink(NxVec3 &v1,NxVec3 &v2,NxVec3 &v3,NxVec3 &v4,float distance)
{
	NxVec3 center = (v1+v2+v3+v4)*0.25f;
	project(center,v1,distance);
	project(center,v2,distance);
	project(center,v3,distance);
	project(center,v4,distance);
}

void TetraMesh::render(bool wireFrame)
{
  render(&mPositions[0],&mIndices[0],mPositions.size(), mIndices.size()/4, true, wireFrame);
}

static unsigned int getColor(unsigned int r,unsigned int g,unsigned int b)
{
  return (r<<16)|(g<<8)|b;
}

void TetraMesh::render(const NxVec3 *vertices,const NxU32 *sindices,NxU32 vcount,NxU32 tcount,bool considerDeletions,bool wireFrame)
{

  if (mMaterial == 0)
  {
    unsigned int ambient = getColor(53,53,105);
    unsigned int diffuse = getColor(152-53,152-53,255-105);
    unsigned int specular = getColor(255,255,255);

    mMaterial = new TetraMaterial("white","white.tga",ambient,diffuse,specular,32.0f);
  }

	if ( mBoxMaterial == 0 )
	{
		unsigned int ambient = getColor(60,0,0);
		unsigned int diffuse = getColor(255-60,0,0);
		unsigned int specular = getColor(255,255,255);
    mBoxMaterial = new TetraMaterial("white","white.tga",ambient,diffuse,specular,32.0f);
  }

  unsigned int lcolor = 0xFFFFFFFF;

  if ( vcount )
  {
    if ( vcount != mLastVcount || tcount != mLastTcount )
    {
      if ( mTetraVbuffer )
        gGraphicsInterface->releaseVertexBuffer(mTetraVbuffer);
      mTetraVbuffer = 0;
      if ( mBoxVbuffer )
        gGraphicsInterface->releaseVertexBuffer(mBoxVbuffer);
      mBoxVbuffer = 0;
      mLastVcount = vcount;
      mLastTcount = tcount;
    }

    if ( mTetraVbuffer == 0 )
    {
      NxVec3 v;
      mBounds.getExtents(v);
      mShrinkDistance = v.magnitude()*0.01f;
      mTetraVbuffer = gGraphicsInterface->createVertexBuffer( tcount*4*3, 0 );
    }

    if ( mTetraVbuffer )
    {
      if ( wireFrame )
      {
    		const NxVec3 *positions = vertices;
    		const unsigned int *idx = sindices;
        // Build an indexed triangle list of the tetrahedral mesh (for visualization purposes) with face normals.
    		for (NxU32 i=0; i<tcount; i++)
    		{
    			NxU32 i1 = *idx++;
   			  NxU32 i2 = *idx++;
      		NxU32 i3 = *idx++;
      		NxU32 i4 = *idx++;

      		NxVec3 p1 = positions[i1];
      		NxVec3 p2 = positions[i2];
      		NxVec3 p3 = positions[i3];
      		NxVec3 p4 = positions[i4];

      		shrink(p1,p2,p3,p4,mShrinkDistance); // shrink tetrahdron in to 90% of its full size

          bool ok = true;
          if ( considerDeletions && mDeletions && mDeletions[i] ) ok = false;

          if ( ok )
          {
            gGraphicsInterface->addLine(0xFFD0D0FF, &p1.x, &p2.x );
            gGraphicsInterface->addLine(0xFFD0D0FF, &p2.x, &p3.x );
            gGraphicsInterface->addLine(0xFFD0D0FF, &p3.x, &p1.x );
            gGraphicsInterface->addLine(0xFFD0D0FF, &p1.x, &p4.x );
            gGraphicsInterface->addLine(0xFFD0D0FF, &p2.x, &p4.x );
            gGraphicsInterface->addLine(0xFFD0D0FF, &p3.x, &p4.x );
          }

     		}
      }
      else
      {
        TetraGraphicsVertex *dest = (TetraGraphicsVertex *)gGraphicsInterface->lockVertexBuffer(mTetraVbuffer);
        TetraGraphicsVertex *begin = dest;

        if ( dest )
        {
      		const NxVec3 *positions = vertices;
      		const unsigned int *idx = sindices;

          // Build an indexed triangle list of the tetrahedral mesh (for visualization purposes) with face normals.
      		for (NxU32 i=0; i<tcount; i++)
      		{
      			NxU32 i1 = *idx++;
      			NxU32 i2 = *idx++;
      			NxU32 i3 = *idx++;
      			NxU32 i4 = *idx++;

      			NxVec3 p1 = positions[i1];
      			NxVec3 p2 = positions[i2];
      			NxVec3 p3 = positions[i3];
      			NxVec3 p4 = positions[i4];

      			shrink(p1,p2,p3,p4,mShrinkDistance); // shrink tetrahdron in to 90% of its full size

            bool ok = true;
            if ( considerDeletions && mDeletions && mDeletions[i] ) ok = false;

            if ( ok )
            {
        			dest = addGraphicsTri(p3,p2,p1,dest);
        			dest = addGraphicsTri(p1,p2,p4,dest);
        			dest = addGraphicsTri(p2,p3,p4,dest);
        			dest = addGraphicsTri(p3,p1,p4,dest);
            }

      		}

          gGraphicsInterface->unlockVertexBuffer(mTetraVbuffer);

          NxU32 avcount = (dest-begin);
          gGraphicsInterface->renderSection(mMaterial,mTetraVbuffer,avcount);

        }
      }
    }

    if ( wireFrame )
    {
      NxU32 pcount = vcount;
      NxU32 bcount = pcount*12; // 12 triangles per box at each position.
      NxU32 bvcount = bcount*3; //
      NxVec3 v;
      mBounds.getExtents(v);
      mShrinkDistance = (v.magnitude()*0.01f)*0.5f;
      NxVec3 center;
      mBounds.getCenter(center);

			const NxVec3 *positions = vertices;
      for (NxU32 i=0; i<pcount; i++)
      {
        NxVec3 box[8];

        NxVec3 bmin,bmax;

        bmin.x = positions->x-mShrinkDistance;
        bmin.y = positions->y-mShrinkDistance;
        bmin.z = positions->z-mShrinkDistance;

        bmax.x = positions->x+mShrinkDistance;
        bmax.y = positions->y+mShrinkDistance;
        bmax.z = positions->z+mShrinkDistance;

        gGraphicsInterface->wireBox(0xFFFF0000, &bmin.x, &bmax.x);

        positions++;
   		}

      gGraphicsInterface->flushLines();

    }
    else
    {
      NxU32 pcount  = vcount;
      NxU32 bcount  = pcount*12; // 12 triangles per box at each position.
      NxU32 bvcount = bcount*3; //

      if ( mBoxVbuffer == 0 )
      {
        mBoxVbuffer = gGraphicsInterface->createVertexBuffer( bvcount, 0 );
      }

      if ( mBoxVbuffer )
      {

        NxVec3 v;
        mBounds.getExtents(v);
        mShrinkDistance = (v.magnitude()*0.005f)*0.5f;
        NxVec3 center;
        mBounds.getCenter(center);

        TetraGraphicsVertex *dest = (TetraGraphicsVertex *)gGraphicsInterface->lockVertexBuffer(mBoxVbuffer);

        if ( dest )
        {
					const NxVec3 *positions = vertices;
          for (NxU32 i=0; i<pcount; i++)
          {

            NxVec3 box[8];

            NxVec3 bmin,bmax;

            bmin.x = positions->x-mShrinkDistance;
            bmin.y = positions->y-mShrinkDistance;
            bmin.z = positions->z-mShrinkDistance;

            bmax.x = positions->x+mShrinkDistance;
            bmax.y = positions->y+mShrinkDistance;
            bmax.z = positions->z+mShrinkDistance;


        		box[0].set( bmin.x, bmin.y, bmin.z );
        		box[1].set( bmax.x, bmin.y, bmin.z );
        		box[2].set( bmax.x, bmax.y, bmin.z );
        		box[3].set( bmin.x, bmax.y, bmin.z );
        		box[4].set( bmin.x, bmin.y, bmax.z );
        		box[5].set( bmax.x, bmin.y, bmax.z );
        		box[6].set( bmax.x, bmax.y, bmax.z );
        		box[7].set( bmin.x, bmax.y, bmax.z );

        		dest = addGraphicsTri(box[0],box[3],box[2],dest);
        		dest = addGraphicsTri(box[0],box[2],box[1],dest);
        		dest = addGraphicsTri(box[2],box[6],box[5],dest);
        		dest = addGraphicsTri(box[1],box[2],box[5],dest);
        		dest = addGraphicsTri(box[6],box[7],box[4],dest);
        		dest = addGraphicsTri(box[5],box[6],box[4],dest);
        		dest = addGraphicsTri(box[4],box[7],box[3],dest);
        		dest = addGraphicsTri(box[4],box[3],box[0],dest);
        		dest = addGraphicsTri(box[5],box[4],box[0],dest);
        		dest = addGraphicsTri(box[5],box[0],box[1],dest);
        		dest = addGraphicsTri(box[3],box[7],box[6],dest);
        		dest = addGraphicsTri(box[2],box[3],box[6],dest);

            positions++;


      		}
          gGraphicsInterface->unlockVertexBuffer(mBoxVbuffer);
          gGraphicsInterface->renderSection(mBoxMaterial,mBoxVbuffer,bvcount);

        }
      }
    }
  }
}


void TetraMesh::onDeviceReset(void)
{
  gGraphicsInterface->releaseMaterial(mMaterial);
  gGraphicsInterface->releaseMaterial(mBoxMaterial);
  gGraphicsInterface->releaseVertexBuffer(mTetraVbuffer);
  gGraphicsInterface->releaseVertexBuffer(mBoxVbuffer);
  mTetraVbuffer = 0;
  mBoxVbuffer   = 0;
}

void TetraModel::onDeviceReset(void)
{
  if ( mVertexBuffer )
  {
    gGraphicsInterface->releaseVertexBuffer(mVertexBuffer);
    mVertexBuffer = 0;
  }
  for (NxU32 i=0; i<mMaterials.size(); i++)
  {
    TetraMaterial *tm = mMaterials[i];
    gGraphicsInterface->releaseMaterial(tm);
  }
  for (NxU32 i=0; i<mSections.size(); i++)
  {
    TetraModelSection *tms = mSections[i];
    tms->onDeviceReset();
  }
  if ( mTetraMesh )
    mTetraMesh->onDeviceReset();
}

void TetraModelSection::onDeviceReset(void)
{
  gGraphicsInterface->releaseMaterial(mMaterial);
  gGraphicsInterface->releaseIndexBuffer(mIndexBuffer);
  mIndexBuffer = 0;
}

void TetraModel::setDeletions(NxU8 *deletions)
{
  if ( mTetraMesh )
    mTetraMesh->setDeletions(deletions);
}


static void barycentricCoords(const NxVec3 &p0, const NxVec3 &p1,const NxVec3 &p2, const NxVec3 &p3,const NxVec3 &p, NxVec3 &barycentricCoords)
// -----------------------------------------------------------------------------------
{
	NxVec3 q  = p-p3;
	NxVec3 q0 = p0-p3;
	NxVec3 q1 = p1-p3;
	NxVec3 q2 = p2-p3;

	NxMat33 m;
  m.setColumn(0,q0);
  m.setColumn(1,q1);
  m.setColumn(2,q2);

	NxReal det = m.determinant();

	m.setColumn(0, q);
	barycentricCoords.x = m.determinant();

	m.setColumn(0, q0); m.setColumn(1,q);
	barycentricCoords.y = m.determinant();

	m.setColumn(1, q1); m.setColumn(2,q);
	barycentricCoords.z = m.determinant();

	if (det != 0.0f)
		barycentricCoords /= det;
}

inline NxReal m3Max(NxReal r1,NxReal r2)
{
	if (r1 >= r2) return r1;
	else return r2;
}

void TetraMesh::buildLinks(NxArray< TetraVertex > &verts) // compute the links between these source graphics vertices and the tetrahedral mesh
{
  MeshHash hash;

	// hash tetrahedra for faster search
	hash.setGridSpacing(mBounds.min.distance(mBounds.max) * 0.1f);

  NxU32 tcount = mIndices.size()/4;

  NxU32 *idx = &mIndices[0];

  NxU32 index = 0;

  NxArray< NxU32 > newIndices;

	for (NxU32 i = 0; i < tcount; i++)
  {

    NxU32 i1 = *idx++;
    NxU32 i2 = *idx++;
    NxU32 i3 = *idx++;
    NxU32 i4 = *idx++;

    if ( !mDeletions || mDeletions[i] == 0 )
    {

      newIndices.push_back(i1);
      newIndices.push_back(i2);
      newIndices.push_back(i3);
      newIndices.push_back(i4);

      NxBounds3 tetraBounds;

  		tetraBounds.setEmpty();
  		tetraBounds.include(mPositions[i1]);
  		tetraBounds.include(mPositions[i2]);
  		tetraBounds.include(mPositions[i3]);
  		tetraBounds.include(mPositions[i4]);

  		hash.add(tetraBounds, index);

      index++;
    }
	}

  tcount = newIndices.size()/4;



  NxU32 vcount = verts.size();

	for (NxU32 i = 0; i < vcount; i++)
  {
		TetraVertex &tvert = verts[i];

		NxVec3 pos = tvert.mPos;

    NxArray<int> itemIndices;

		hash.queryUnique(pos, itemIndices);

		NxReal minDist = 0.0f;
		NxVec3 b;
		int isize,num;
    isize = num = (int)itemIndices.size();
		if (num == 0)
		  num = tcount;

    NxU32 *indices = &newIndices[0];

		for (int i = 0; i < num; i++)
	  {
			int j = i;

			if ( isize > 0)
      {
			  j = itemIndices[i];
      }

      NxU32 *idx = &indices[j*4];

      NxU32 i1 = *idx++;
      NxU32 i2 = *idx++;
      NxU32 i3 = *idx++;
      NxU32 i4 = *idx++;

      NxVec3 &p0 = mPositions[i1];
      NxVec3 &p1 = mPositions[i2];
      NxVec3 &p2 = mPositions[i3];
      NxVec3 &p3 = mPositions[i4];

      NxVec3 b;

			barycentricCoords(p0, p1, p2, p3, pos, b);

			// is the vertex inside the tetrahedron? If yes we take it
			if (b.x >= 0.0f && b.y >= 0.0f && b.z >= 0.0f && (b.x + b.y + b.z) <= 1.0f)
			{
        tvert.mBarycentric = b;
        tvert.mIndex       = j;
				break;
			}

			// otherwise, if we are not in any tetrahedron we take the closest one
			NxReal dist = 0.0f;
			if (b.x + b.y + b.z > 1.0f) dist = b.x + b.y + b.z - 1.0f;
			if (b.x < 0.0f) dist = (-b.x < dist) ? dist : -b.x;
			if (b.y < 0.0f) dist = (-b.y < dist) ? dist : -b.y;
			if (b.z < 0.0f) dist = (-b.z < dist) ? dist : -b.z;

			if (i == 0 || dist < minDist)
		  {
				minDist = dist;
        tvert.mBarycentric = b;
        tvert.mIndex       = j;
			}
		}
	}

}

void TetraModel::createLinks(void)
{
  if ( mTetraMesh )
  {
    mTetraMesh->buildLinks(mVertices);
  }
}


void TetraModelSection::applyNormals(TetraGraphicsVertex *verts)
{
  NxU32 tcount = mIndices.size()/3;
  if ( tcount )
  {
    NxU32 *idx = &mIndices[0];
    for (NxU32 i=0; i<tcount; i++)
    {
      NxU32 i1 = *idx++;
      NxU32 i2 = *idx++;
      NxU32 i3 = *idx++;

      NxVec3 &p1 = *(NxVec3 *) verts[i1].mPos;
      NxVec3 &p2 = *(NxVec3 *) verts[i2].mPos;
      NxVec3 &p3 = *(NxVec3 *) verts[i3].mPos;
      NxVec3 &n1 = *(NxVec3 *) verts[i1].mNormal;
      NxVec3 &n2 = *(NxVec3 *) verts[i2].mNormal;
      NxVec3 &n3 = *(NxVec3 *) verts[i3].mNormal;

      NxVec3 normal;

      normal.cross( p2 - p1, p3-p1 ); // compute the facenormal
      n1+=normal;
      n2+=normal;
      n3+=normal;

    }
  }
}

void TetraModelSection::applyNormals(TetraVertex *verts)
{
  NxU32 tcount = mIndices.size()/3;
  if ( tcount )
  {
    NxU32 *idx = &mIndices[0];
    for (NxU32 i=0; i<tcount; i++)
    {
      NxU32 i1 = *idx++;
      NxU32 i2 = *idx++;
      NxU32 i3 = *idx++;

      NxVec3 &p1 = verts[i1].mPos;
      NxVec3 &p2 = verts[i2].mPos;
      NxVec3 &p3 = verts[i3].mPos;
      NxVec3 &n1 = verts[i1].mNormal;
      NxVec3 &n2 = verts[i2].mNormal;
      NxVec3 &n3 = verts[i3].mNormal;

      NxVec3 normal;

      normal.cross( p2 - p1, p3-p1 ); // compute the facenormal
      n1+=normal;
      n2+=normal;
      n3+=normal;

    }
  }
}


void TetraModel::softMeshMaterial(const TetraMaterial &tm)
{
	TetraMaterial *mat = 0;

	for (NxU32 i=0; i<mMaterials.size(); i++)
	{
		TetraMaterial *t = mMaterials[i];
		if ( *t == tm )
		{
			mat = t;
			break;
		}
	}
	if ( mat == 0 )
	{
		mat = new TetraMaterial(tm);
		mMaterials.push_back(mat);
	}
  TetraModelSection *tms = new TetraModelSection(mat);
  mCurrentSection = tms;
  mSections.push_back(tms);
}

void TetraModel::softMeshTriangle(const TetraGraphicsVertex &v1,const TetraGraphicsVertex &v2,const TetraGraphicsVertex &v3)
{

	TetraVertex tv1(v1);
	TetraVertex tv2(v2);
	TetraVertex tv3(v3);

  mGraphicsBounds.include(tv1.mPos);
  mGraphicsBounds.include(tv2.mPos);
  mGraphicsBounds.include(tv3.mPos);

	tv1.mNormal.set(0,0,0);
	tv2.mNormal.set(0,0,0);
	tv3.mNormal.set(0,0,0);

	NxU32 i1 = getSoftVertexIndex( mVertexPool, &tv1 );
	NxU32 i2 = getSoftVertexIndex( mVertexPool, &tv2 );
	NxU32 i3 = getSoftVertexIndex( mVertexPool, &tv3 );

	if ( mCurrentSection == 0 )
	{
		mCurrentSection = new TetraModelSection(0);
		mSections.push_back(mCurrentSection);
	}

  mCurrentSection->mIndices.push_back(i1);
  mCurrentSection->mIndices.push_back(i2);
  mCurrentSection->mIndices.push_back(i3);

}

void TetraModel::softMeshTetrahedron(const float *p1,const float *p2,const float *p3,const float *p4)
{
  if ( mTetraPool )
  {
  	if ( mTetraMesh == 0 )
  	{
  		mTetraMesh = new TetraMesh;
  		mTetraMesh->mBounds.setEmpty();
  	}

  	const NxVec3 *v1 = (const NxVec3 *) p1;
  	const NxVec3 *v2 = (const NxVec3 *) p2;
  	const NxVec3 *v3 = (const NxVec3 *) p3;
  	const NxVec3 *v4 = (const NxVec3 *) p4;

  	mTetraMesh->mBounds.include(*v1);
  	mTetraMesh->mBounds.include(*v2);
  	mTetraMesh->mBounds.include(*v3);
  	mTetraMesh->mBounds.include(*v4);

  	NxU32 i1 = getSoftVertexIndex( mTetraPool, p1 );
  	NxU32 i2 = getSoftVertexIndex( mTetraPool, p2 );
  	NxU32 i3 = getSoftVertexIndex( mTetraPool, p3 );
  	NxU32 i4 = getSoftVertexIndex( mTetraPool, p4 );

  	mTetraMesh->mIndices.push_back(i1);
  	mTetraMesh->mIndices.push_back(i2);
  	mTetraMesh->mIndices.push_back(i3);
  	mTetraMesh->mIndices.push_back(i4);
  }
}

#if NX_USE_SOFTBODY_API
static const float *getPos(const NxSoftBodyMeshDesc &desc,NxU32 index)
{
  const char *p = (const char *)desc.vertices + (index*desc.vertexStrideBytes);
  return (const float *)p;
}
#endif

TetraModel::TetraModel(NxSoftBodyMesh *mesh,const char *userProperties)
{
	mVertexPool   = 0;
	mTetraPool    = 0;
	mVertexBuffer = 0;
	if ( userProperties )
  {
		const char *value = NXU::getKeyValue(userProperties,"graphics_mesh");
		if ( value )
		{
 			strcpy(mObjName,value);
		}
  }
	else
		mObjName[0] = 0;

	mTetName[0] = 0;

	mNxSoftBodyMesh = 0;

  mNxSoftBodyMesh = mesh;

  mTetraMesh = 0;

  mVertexPool = createSoftVertexPool(SVT_TETRA_VERTEX);
  mTetraPool  = 0;

	mCurrentSection = 0;

	bool ok = false;

	if ( strlen(mObjName) )
	{
		const char *dot = lastDot(mObjName);
		if ( dot && stricmp(dot,".ezm") == 0 )
			ok = loadSoftMeshEZM(mObjName,this);
		else
			ok = loadSoftMeshObj(mObjName,0,this);
	}

  NxU32 vcount = 0;

	TetraVertex *vlist = (TetraVertex *)getSoftVertexPoolBuffer(mVertexPool,vcount);

	for (NxU32 i=0; i<vcount; i++)
	{
		mVertices.push_back( *vlist++ );
	}

  mTetraPool  = createSoftVertexPool(SVT_TETRA_POSITION_VERTEX);

#if NX_USE_SOFTBODY_API

  NxSoftBodyMeshDesc desc;
  mesh->saveToDesc(desc);

	if ( desc.flags & NX_SOFTBODY_MESH_16_BIT_INDICES )
	{
    const NxU8 *tet = (const NxU8 *) desc.tetrahedra;
    for (NxU32 i=0; i<desc.numTetrahedra; i++)
    {
      const NxU16 *idx = (const NxU16 *) tet;
      NxU32 i1 = idx[0];
      NxU32 i2 = idx[1];
      NxU32 i3 = idx[2];
      NxU32 i4 = idx[3];
      const float *p1 = getPos(desc,i1);
      const float *p2 = getPos(desc,i2);
      const float *p3 = getPos(desc,i3);
      const float *p4 = getPos(desc,i4);
      softMeshTetrahedron(p1,p2,p3,p4);
      tet+=desc.tetrahedronStrideBytes;
    }
	}
	else
	{
    const NxU8 *tet = (const NxU8 *) desc.tetrahedra;
    for (NxU32 i=0; i<desc.numTetrahedra; i++)
    {
      const NxU32 *idx = (const NxU32 *) tet;
      NxU32 i1 = idx[0];
      NxU32 i2 = idx[1];
      NxU32 i3 = idx[2];
      NxU32 i4 = idx[3];
      const float *p1 = getPos(desc,i1);
      const float *p2 = getPos(desc,i2);
      const float *p3 = getPos(desc,i3);
      const float *p4 = getPos(desc,i4);
      softMeshTetrahedron(p1,p2,p3,p4);
      tet+=desc.tetrahedronStrideBytes;
    }
	}
#endif

	if ( mTetraMesh )
	{
		NxU32 vcount = 0;
		NxVec3 *vlist = (NxVec3 *) getSoftVertexPoolBuffer(mTetraPool,vcount);
		for (NxU32 i=0; i<vcount; i++)
		{
			mTetraMesh->mPositions.push_back( *vlist++ );
		}
	}

	releaseSoftVertexPool(mVertexPool);
	releaseSoftVertexPool(mTetraPool);

	mVertexPool = 0;
	mTetraPool  = 0;

  // build the mean unit normals.
  if ( mVertices.size() )
  {
    TetraVertex *vlist = &mVertices[0];
   	for (NxU32 i=0; i<mSections.size(); i++)
   	{
   		TetraModelSection *ms = mSections[i];
       ms->applyNormals(vlist);
   	}
    NxU32 vcount = mVertices.size();
    for (NxU32 i=0; i<vcount; i++)
    {
      vlist->mNormal.normalize();
      vlist++;
    }
  }


  createLinks(); // optimize the links in memory


}

}; // END OF SOFTBODY NAMESPACE
