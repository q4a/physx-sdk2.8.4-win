#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "SoftServe.h"
#include "TetraMesh.h"
#include "TetraGraphics.h"
#include "SoftVertex.h"
#include "NXU_helper.h"

namespace SOFTBODY
{

static char	data[1024]; // 16*64

const char *lastDot(const char *src)
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

static const char	*fstring(NxF32 v,	bool binary	=	false)
{
	static int index = 0;

	char *ret	=	&data[index	*64];
	index++;
	if (index	== 16)
	{
		index	=	0;
	}

	if (v	== FLT_MAX)
	{
		strcpy(ret,	"FLT_MAX");
	}
	else if	(v ==	FLT_MIN)
	{
		strcpy(ret,	"FLT_MIN");
	}
	else if	(v ==	1)
	{
		strcpy(ret,	"1");
	}
	else if	(v ==	0)
	{
		strcpy(ret,	"0");
	}
	else if	(v ==	 - 1)
	{
		strcpy(ret,	"-1");
	}
	else
	{
		if (binary)
		{
			unsigned int *iv = (unsigned int*) &v;
			sprintf(ret, "%.4f$%x",	v,	*iv);
		}
		else
		{
			sprintf(ret, "%.9f", v);
			const	char *dot	=	strstr(ret,	".");
			if (dot)
			{
				int	len	=	(int)strlen(ret);
				char *foo	=	&ret[len - 1];
				while	(*foo	== '0')
				{
					foo--;
				}
				if (*foo ==	'.')
				{
					*foo = 0;
				}
				else
				{
					foo[1] = 0;
				}
			}
		}
	}

	return ret;
}


bool saveSoftBodyNxuStream(const char *fname,      // the name to store the asset on disk.
                           const char *userProperties,  // user properties associated with this asset (identify graphics mesH)
                           const NxSoftBodyDesc &desc,        // The descriptor for the soft body
                           const NxSoftBodyMeshDesc &mesh)        // The descriptor for the soft body tetrahedral mesh
{
  bool ret = false;

  const char *saveName = fname;
  if ( gFileInterface )
    saveName = gFileInterface->getSoftFileName(saveName,false);
	if ( !saveName ) return false;

  NXU::NxuPhysicsCollection *c = NXU::createCollection(fname,"SoftBodyMesh Asset");

  if ( c )
  {

    NXU::addSoftBody(*c,desc,mesh,userProperties,fname);

    ret = NXU::saveCollection(c,saveName,NXU::FT_XML,true,false);
    NXU::releaseCollection(c);
  }

  return ret;
}

bool saveSoftBodyObj(const char *oname,const char *tname,TetraModel *model) // save as an OBJ + TET file.
{
	if(oname == NULL) return false;

  bool ret = false;

  const char *saveName = oname;
  if  ( gFileInterface )
    saveName = gFileInterface->getSoftFileName(oname,false);

	if ( !saveName ) return false;

	char temp[512];
  if(1)
  {
    strcpy(temp,oname);
    char *dot = (char *)lastDot(temp);
    if ( dot )
    {
      *dot = 0;
      strcat(temp,".mtl");
      const char *fname = temp;
      if ( gFileInterface )
        fname = gFileInterface->getSoftFileName(fname,false);

      FILE *mtl = fopen(fname,"wb");
      if ( mtl )
      {
        for (NxU32 i=0; i<model->mMaterials.size(); i++)
        {
          TetraMaterial *tm = model->mMaterials[i];
          fprintf(mtl,"newmtl %s\r\n", tm->mName );
          fprintf(mtl,"Ns 30\r\n");
          fprintf(mtl,"Ka 0 0 0\r\n");
          fprintf(mtl,"Kd 0.992157 0.796078 0.717647\r\n");
          fprintf(mtl,"Ks 0.0901961 0.0901961 0.0901961\r\n");
          fprintf(mtl,"map_Kd %s\r\n", tm->mTexture);
        }
        fclose(mtl);
      }
    }
  }

  FILE *fph = fopen(saveName,"wb");

  if ( fph )
  {
    fprintf(fph,"# %s saved from the SoftBody editor written by John W. Ratcliff\r\n");

    NxU32 vcount = model->mVertices.size();

    const TetraVertex *vertices = &model->mVertices[0];

    SoftVertexPool *positions = createSoftVertexPool(SVT_TETRA_POSITION_VERTEX);
    SoftVertexPool *normals   = createSoftVertexPool(SVT_TETRA_POSITION_VERTEX);
    SoftVertexPool *texels    = createSoftVertexPool(SVT_TETRA_POSITION_VERTEX);

    NxU32 *translation = new NxU32[vcount];
    memset(translation,0,sizeof(NxU32)*vcount);

    for (NxU32 i=0; i<vcount; i++)
    {
      const TetraVertex &tv = vertices[i];
      NxU32 t = getSoftVertexIndex(positions,&tv.mPos.x);
      translation[t] = i; // translate back to the original graphics index
      getSoftVertexIndex(normals,&tv.mNormal.x);
      NxVec3 texel(tv.mTexel[0],tv.mTexel[1],0);
      getSoftVertexIndex(texels,&texel.x);
    }

    if ( 1 )
    {
      NxU32 vcount,ncount,tcount;
      getSoftVertexPoolBuffer(positions,vcount);
      getSoftVertexPoolBuffer(normals,ncount);
      getSoftVertexPoolBuffer(texels,tcount);
      fprintf(fph,"### Contains %d unique positions.\r\n", vcount );
      fprintf(fph,"### Contains %d unique normals.\r\n", ncount );
      fprintf(fph,"### Contains %d unique texture coordinates.\r\n", tcount);
    }

    fprintf(fph,"mtllib %s\r\n", temp );

    // Print the positions
    if ( 1 )
    {
      NxU32 count=0;
      NxVec3 *p = (NxVec3 *)getSoftVertexPoolBuffer(positions,count);
      for (NxU32 i=0; i<count; i++)
      {
        fprintf(fph,"v %10s %10s %10s\r\n", fstring(p->x), fstring(p->y), fstring(p->z) );
        p++;
      }
      fprintf(fph,"\r\n");
      fprintf(fph,"##################################\r\n");
      fprintf(fph,"\r\n");
    }

    // Print the normals
    if ( 1 )
    {
      NxU32 count=0;
      NxVec3 *p = (NxVec3 *)getSoftVertexPoolBuffer(normals,count);
      for (NxU32 i=0; i<count; i++)
      {
        fprintf(fph,"vn %10s %10s %10s\r\n", fstring(p->x), fstring(p->y), fstring(p->z) );
        p++;
      }
      fprintf(fph,"\r\n");
      fprintf(fph,"##################################\r\n");
      fprintf(fph,"\r\n");
    }
    // Print the texture co-ordinates
    if ( 1 )
    {
      NxU32 count=0;
      NxVec3 *p = (NxVec3 *)getSoftVertexPoolBuffer(texels,count);
      for (NxU32 i=0; i<count; i++)
      {
        fprintf(fph,"vt %10s %10s\r\n", fstring(p->x), fstring(p->y) );
        p++;
      }
      fprintf(fph,"\r\n");
      fprintf(fph,"##################################\r\n");
      fprintf(fph,"\r\n");
    }

    for (NxU32 i=0; i<model->mSections.size(); i++)
    {
      const TetraModelSection *ms = model->mSections[i];
      const char *matname = 0;
      if ( ms->mMaterial )
        matname = ms->mMaterial->mName;

      if ( matname )
        fprintf(fph,"usemtl %s\r\n", matname );

      NxU32 tcount = ms->mIndices.size()/3;
      const NxU32 *idx = &ms->mIndices[0];
      for (NxU32 i=0; i<tcount; i++)
      {
        NxU32 i1 = *idx++;
        NxU32 i2 = *idx++;
        NxU32 i3 = *idx++;

        const TetraVertex &v1 = vertices[i1];
        const TetraVertex &v2 = vertices[i2];
        const TetraVertex &v3 = vertices[i3];

        NxU32 pi1 = getSoftVertexIndex(positions,&v1.mPos.x);
        NxU32 pi2 = getSoftVertexIndex(positions,&v2.mPos.x);
        NxU32 pi3 = getSoftVertexIndex(positions,&v3.mPos.x);

        NxU32 ni1 = getSoftVertexIndex(normals,&v1.mNormal.x);
        NxU32 ni2 = getSoftVertexIndex(normals,&v2.mNormal.x);
        NxU32 ni3 = getSoftVertexIndex(normals,&v3.mNormal.x);

        NxVec3 t1( v1.mTexel[0], v1.mTexel[1], 0 );
        NxVec3 t2( v2.mTexel[0], v2.mTexel[1], 0 );
        NxVec3 t3( v3.mTexel[0], v3.mTexel[1], 0 );

        NxU32 ti1 = getSoftVertexIndex(texels,&t1.x);
        NxU32 ti2 = getSoftVertexIndex(texels,&t2.x);
        NxU32 ti3 = getSoftVertexIndex(texels,&t3.x);


        fprintf(fph,"f %d/%d/%d  %d/%d/%d  %d/%d/%d\r\n", pi1+1, ti1+1, ni1+1, pi2+1, ti2+1, ni2+1, pi3+1, ti3+1, ni3+1 );

      }

      releaseSoftVertexPool(normals);
      releaseSoftVertexPool(texels);
      normals = 0;
      texels = 0;

    }

		fclose(fph);


    TetraMesh *tm = model->mTetraMesh;
    if ( tm )
    {
      const char *saveName = oname;
      if  ( gFileInterface )
        saveName = gFileInterface->getSoftFileName(tname,false);

      FILE *fph = fopen(saveName,"wb");
      if ( fph )
      {

        NxU32 tcount            = tm->mIndices.size()/4;
        const NxU32 *idx        = &tm->mIndices[0];
        const NxVec3 *pos       = &tm->mPositions[0];
        const NxU8 *deletions   = tm->mDeletions;
        NxU32 vcount            = tm->mPositions.size();
        NxU32 lcount            = 0;

        getSoftVertexPoolBuffer(positions,lcount);

        fprintf(fph,"### Tetrahedral mesh exported from the SoftBody editor written by John W. Ratcliff\r\n");
        fprintf(fph,"### Links map to the graphics file '%s'\r\n", oname );
        fprintf(fph,"### Contains %d vertices\r\n", vcount );
        fprintf(fph,"### Contains %d tetrahedra\r\n", tcount );
        fprintf(fph,"### Contains %d links.\r\n", lcount );

        for (NxU32 i=0; i<vcount; i++)
        {
          const NxVec3 &p = pos[i];
          fprintf(fph,"v %10s %10s %10s\r\n", fstring( p.x), fstring(p.y), fstring(p.z) );
        }
        for (NxU32 i=0; i<tcount; i++)
        {
          NxU32 i1 = *idx++;
          NxU32 i2 = *idx++;
          NxU32 i3 = *idx++;
          NxU32 i4 = *idx++;
          if ( !deletions || deletions[i] == 0 )
            fprintf(fph,"t %d %d %d %d\r\n", i1, i2, i3, i4 );
        }

        getSoftVertexPoolBuffer(positions,vcount);

        if ( vcount )
        {
          const TetraVertex *vertices = &model->mVertices[0];
          for (NxU32 i=0; i<vcount; i++)
          {
            NxU32 gindex = translation[i]; // translate to the graphics index.
            const TetraVertex &tv = vertices[gindex];
            fprintf(fph,"l %d %s %s %s\r\n", tv.mIndex, fstring(tv.mBarycentric.x), fstring(tv.mBarycentric.y), fstring(tv.mBarycentric.z));
          }
        }
        fclose(fph);
      }
    }
    delete translation;
    releaseSoftVertexPool(positions);
    positions = 0;
  }
  else
  {
    if ( gFileInterface )
    {
      char scratch[512];
      sprintf(scratch,"Failed to open file '%s' for write access.\r\n", saveName);
      gFileInterface->softMessageReport(scratch);
    }
  }

  return ret;
}

static void ezVert(const TetraVertex &v,FILE *fph)
{
  fprintf(fph,"          %s %s %s    %s %s %s    %s %s    %s %s,\r\n",fstring(v.mPos[0]),fstring(v.mPos[1]),fstring(v.mPos[2]),
      fstring(v.mNormal[0]),fstring(v.mNormal[1]),fstring(v.mNormal[2]),
      fstring(v.mTexel[0]),fstring(v.mTexel[1]),
      fstring(v.mTexel[0]),fstring(v.mTexel[1]) );
}

bool saveSoftBodyEZM(const char *fname,TetraModel *model) // save as an EZ mesh file with TETs embedded in it.
{
  bool ret = false;

  const char *saveName = fname;
  if  ( gFileInterface )
    saveName = gFileInterface->getSoftFileName(fname,false);
	if ( !saveName) return false;

  FILE *fph = fopen(saveName,"wb");

  if ( fph )
  {
    fprintf(fph,"<?xml version=\"1.0\"?>\r\n");
    fprintf(fph,"  <SceneGraph name=\"%s\">\r\n", fname);
    fprintf(fph,"    <Mesh name=\"%s\">\r\n", fname );

    const TetraVertex *vertices = &model->mVertices[0];

    for (NxU32 i=0; i<model->mSections.size(); i++)
    {
      const TetraModelSection *ms = model->mSections[i];
      const char *matname = "white.tga";
      if ( ms->mMaterial )
        matname = ms->mMaterial->mTexture;

      fprintf(fph,"      <MeshSection material=\"%s\">\r\n", matname );


      SoftVertexPool *pool = createSoftVertexPool(SVT_TETRA_VERTEX);
      NxArray< NxU32 > indices;

      NxU32 tcount = ms->mIndices.size()/3;
      const NxU32 *idx = &ms->mIndices[0];
      for (NxU32 i=0; i<tcount; i++)
      {
        NxU32 i1 = *idx++;
        NxU32 i2 = *idx++;
        NxU32 i3 = *idx++;

        const TetraVertex &v1 = vertices[i1];
        const TetraVertex &v2 = vertices[i2];
        const TetraVertex &v3 = vertices[i3];

        i1 = getSoftVertexIndex(pool,&v1);
        i2 = getSoftVertexIndex(pool,&v2);
        i3 = getSoftVertexIndex(pool,&v3);

        indices.push_back(i1);
        indices.push_back(i2);
        indices.push_back(i3);

      }

      if ( 1 )
      {
        NxU32 vcount;
        TetraVertex *vertices = (TetraVertex *)getSoftVertexPoolBuffer(pool,vcount);
        fprintf(fph,"      <vertexbuffer count=\"%d\" ctype=\"fff fff ff ff\" semantic=\"position normal texcoord texcoord\">\r\n",vcount);
        for (NxU32 i=0; i<vcount; i++)
        {
          TetraVertex &v = vertices[i];
          ezVert(v,fph);
        }
        fprintf(fph,"      </vertexbuffer>\r\n");
        NxU32 icount = indices.size();
        NxU32 tcount = icount/3;
        fprintf(fph,"      <indexbuffer count=\"%d\">\r\n", tcount );
        NxU32 *idx = &indices[0];
        for (NxU32 i=0; i<tcount; i++)
        {
          NxU32 i1 = *idx++;
          NxU32 i2 = *idx++;
          NxU32 i3 = *idx++;
          fprintf(fph,"          %d %d %d,\r\n", i1, i2, i3 );
        }
        fprintf(fph,"        </indexbuffer>\r\n");

      }

      releaseSoftVertexPool(pool);
      pool = 0;

      fprintf(fph,"      </MeshSection>\r\n");

    }

    TetraMesh *tm = model->mTetraMesh;
    if ( tm )
    {

      NxU32 tcount            = tm->mIndices.size()/4;
      const NxU32 *idx        = &tm->mIndices[0];
      const NxVec3 *positions = &tm->mPositions[0];
      const NxU8 * deletions  = tm->mDeletions;

      NxU32 otcount = 0;
      for (NxU32 i=0; i<tcount; i++)
      {
        if ( !deletions || deletions[i] == 0 ) otcount++;
      }

      fprintf(fph,"      <TetraMesh count=\"%d\" ctype=\"fff  fff  fff  fff\" semantic=\"position position position position\">\r\n", otcount );

      for (NxU32 i=0; i<tcount; i++)
      {
        NxU32 i1 = *idx++;
        NxU32 i2 = *idx++;
        NxU32 i3 = *idx++;
        NxU32 i4 = *idx++;

        const NxVec3 &p1 = positions[i1];
        const NxVec3 &p2 = positions[i2];
        const NxVec3 &p3 = positions[i3];
        const NxVec3 &p4 = positions[i4];

        if ( !deletions || deletions[i] == 0 )
        {
          fprintf(fph,"          %s %s %s  %s %s %s  %s %s %s  %s %s %s\r\n",
            fstring( p1.x ), fstring(p1.y), fstring(p1.z),
            fstring( p2.x ), fstring(p2.y), fstring(p2.z),
            fstring( p3.x ), fstring(p3.y), fstring(p3.z),
            fstring( p4.x ), fstring(p4.y), fstring(p4.z) );
        }

      }

      fprintf(fph,"      </TetraMesh>\r\n");

    }


    fprintf(fph,"    </Mesh>\r\n");



    fprintf(fph,"  </SceneGraph>\r\n");

		fclose(fph);

  }
  else
  {
    if ( gFileInterface )
    {
      char scratch[512];
      sprintf(scratch,"Failed to open file '%s' for write access.\r\n", saveName);
      gFileInterface->softMessageReport(scratch);
    }
  }

  return ret;
}


void concatFile(char *dest,const char *source,const char *extension) // adds a different file name extension.
{
  strcpy(dest,source);
  char *dot = strchr(dest,'.');
  char *lastdot = dot;
  while ( dot )
  {
    lastdot = dot;
    dot = strchr(lastdot+1,'.');
  }
  if ( lastdot )
  {
    *lastdot = 0;
  }
  strcat(dest,extension);
}

static bool isSep(char c)
{
	if ( c == ',' || c == 44 || c == 39 || c == '&' || c == '=' || c <= 32 || c > 126 )
		return true;
	return false;
}

const char *getQuoteString(const char *str) // returns the source string in quotes if it contains seperator characters (space,tab,comma)
{
	const char *ret = str;

	if ( *str != 34 )
	{
    const char *scan = str;
    while ( *scan )
    {
    	if ( isSep(*scan) )
    	{
    		sprintf(data,"\"%s\"",str);
    		ret = data;
    		break;
    	}
    	scan++;
    }
  }

  return ret;
}

};
