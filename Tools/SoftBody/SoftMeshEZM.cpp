#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <float.h>
#include <math.h>

#include <set>

#include "SoftMeshEZM.h"
#include "NXU_TinyXML.h"
#include "NXU_Asc2Bin.h"
#include "TetraGraphics.h"
#include "NXU_GraphicsMesh.h"

namespace NXU
{

class MeshEZM : public NxuGraphicsInterface
{
public:
	MeshEZM(SOFTBODY::SoftMeshInterface *smi)
	{
		mSoftMesh = smi;
	}


	void NodeMaterial(const char *name,const char *info)
	{
		SOFTBODY::TetraMaterial mat(name);
		strcpy(mat.mTexture,name);
		mSoftMesh->softMeshMaterial(mat);
	}

	void NodeMesh(const char *name,const char *info)
	{
	}



	void NodeTriangleList(int vcount,const NxuVertex *vertex,int icount,const unsigned short *indices)
	{
		unsigned int tcount = icount/3;

		for (unsigned int i=0; i<tcount; i++)
		{
			unsigned int i1 = *indices++;
			unsigned int i2 = *indices++;
			unsigned int i3 = *indices++;

			const NxuVertex v1 = vertex[i1];
			const NxuVertex v2 = vertex[i2];
			const NxuVertex v3 = vertex[i3];

			NodeTriangle(&v1,&v2,&v3);


		}
	}

	void NodeTriangleList(int vcount,const NxuDeformVertex *vertex,int icount,const unsigned short *indices)
	{
		unsigned int tcount = icount/3;

		for (unsigned int i=0; i<tcount; i++)
		{
			unsigned int i1 = *indices++;
			unsigned int i2 = *indices++;
			unsigned int i3 = *indices++;

			const NxuDeformVertex v1 = vertex[i1];
			const NxuDeformVertex v2 = vertex[i2];
			const NxuDeformVertex v3 = vertex[i3];

			NodeTriangle(&v1,&v2,&v3);

		}
	}

	void get(SOFTBODY::TetraGraphicsVertex &g,const NxuVertex &v)
	{
		g.mPos[0] = v.mPos[0];
		g.mPos[1] = v.mPos[1];
		g.mPos[2] = v.mPos[2];
		g.mNormal[0] = v.mNormal[0];
		g.mNormal[1] = v.mNormal[1];
		g.mNormal[2] = v.mNormal[2];
		g.mTexel[0] = v.mTexel1[0];
		g.mTexel[1] = v.mTexel1[1];
	}

	void NodeTriangle(const NxuVertex *v1,const NxuVertex *v2,const NxuVertex *v3)
	{
		SOFTBODY::TetraGraphicsVertex g1,g2,g3;

    get(g1,*v1);
    get(g2,*v2);
    get(g3,*v3);

	  mSoftMesh->softMeshTriangle(g1,g2,g3);
	}

  void NodeTetrahedron(const float *p1,const float *p2,const float *p3,const float *p4)
  {
    mSoftMesh->softMeshTetrahedron(p1,p2,p3,p4);
  }

	void NodeTriangle(const NxuDeformVertex *dv1,const NxuDeformVertex *dv2,const NxuDeformVertex *dv3)
	{
		NxuVertex v1(*dv1);
		NxuVertex v2(*dv2);
		NxuVertex v3(*dv3);
		NodeTriangle(&v1,&v2,&v3);
	}

	// add a skeleton
	void NodeSkeleton(const NxuSkeleton *skeleton)
	{
	}

	void NodeAnimation(const NxuAnimation *animation)
	{
	}

	void NodeInstance(const char *name,const float *transform)
	{
	}

	bool loadEZM(const char *fname)
	{
		bool ret = false;

		if ( SOFTBODY::gFileInterface )
			fname = SOFTBODY::gFileInterface->getSoftFileName(fname,true);

    ret = NxuLoadGraphicsMesh(fname,this);

    return ret;
  }

private:
	SOFTBODY::SoftMeshInterface *mSoftMesh;

};


}; // END of NXU namespace

namespace SOFTBODY
{

bool loadSoftMeshEZM(const char *oname,SoftMeshInterface *smi)
{
	bool ret = false;

	NXU::MeshEZM ezm(smi);

  ret = ezm.loadEZM(oname);

  return ret;
}


};
