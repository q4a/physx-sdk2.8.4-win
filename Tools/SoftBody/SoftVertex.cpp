#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>
#include <set>

#include "TetraMesh.h"
#include "SoftVertex.h"

namespace SOFTBODY
{

template <class Type> class VertexLess
{
public:
	typedef std::vector< Type > VertexVector;

	bool operator()(int v1,int v2) const;

	static void SetSearch(const Type& match,VertexVector *list)
	{
		mFind = match;
		mList = list;
	};

private:
	const Type& Get(int index) const
	{
		if ( index == -1 ) return mFind;
		VertexVector &vlist = *mList;
		return vlist[index];
	}
	static Type mFind; // vertice to locate.
	static VertexVector  *mList;
};

template <class Type> class VertexPool
{
public:
	typedef std::set<int, VertexLess<Type> > VertexSet;
	typedef std::vector< Type > VertexVector;

	int GetVertex(const Type& vtx)
	{
		VertexLess<Type>::SetSearch(vtx,&mVtxs);
		VertexSet::iterator found;
		found = mVertSet.find( -1 );
		if ( found != mVertSet.end() )
		{
			return *found;
		}
		int idx = (int)mVtxs.size();
		mVtxs.push_back( vtx );
		mVertSet.insert( idx );
		return idx;
	};

	const float * GetPos(int idx) const
	{
		return &mVtxs[idx].mPos.x;
	}

	const Type& Get(int idx) const
	{
		return mVtxs[idx];
	};

	unsigned int GetSize(void) const
	{
		return mVtxs.size();
	};

	void Clear(int reservesize)  // clear the vertice pool.
	{
		mVertSet.clear();
		mVtxs.clear();
		mVtxs.reserve(reservesize);
	};

	const VertexVector& GetVertexList(void) const { return mVtxs; };

	void Set(const Type& vtx)
	{
		mVtxs.push_back(vtx);
	}

	unsigned int GetVertexCount(void) const
	{
		return mVtxs.size();
	};


	Type * GetBuffer(void)
	{
		return &mVtxs[0];
	};

private:
	VertexSet      mVertSet; // ordered list.
	VertexVector   mVtxs;  // set of vertices.
};


TetraVertex VertexLess<TetraVertex>::mFind;
std::vector<TetraVertex > *VertexLess<TetraVertex>::mList=0;

TetraGraphicsVertex VertexLess<TetraGraphicsVertex>::mFind;
std::vector<TetraGraphicsVertex > *VertexLess<TetraGraphicsVertex>::mList=0;

TetraDeformVertex VertexLess<TetraDeformVertex>::mFind;
std::vector<TetraDeformVertex > *VertexLess<TetraDeformVertex>::mList=0;

NxVec3      VertexLess<NxVec3>::mFind;
std::vector<NxVec3> *VertexLess<NxVec3>::mList=0;

enum RDIFF
{
  RD_EQUAL,
  RD_LESS,
  RD_GREATER
};

static RDIFF relativeDiff(const NxVec3 &a,const NxVec3 &b,float magnitude)
{
  RDIFF ret = RD_EQUAL;

  float m2 = magnitude*magnitude;
  float dx = a.x-b.x;
  float dy = a.y-b.y;
  float dz = a.z-b.z;
  float d2 = (dx*dx)+(dy*dy)+(dz*dz);

  if ( d2 > m2 )
  {
         if ( a.x < b.x ) ret = RD_LESS;
    else if ( a.x > b.x ) ret = RD_GREATER;
    else if ( a.y < b.y ) ret = RD_LESS;
    else if ( a.y > b.y ) ret = RD_GREATER;
    else if ( a.z < b.z ) ret = RD_LESS;
    else if ( a.z > b.z ) ret = RD_GREATER;
  }
  return ret;
}



bool VertexLess<TetraVertex>::operator()(int v1,int v2) const
{
  bool ret = false;


	const TetraVertex& a = Get(v1);
	const TetraVertex& b = Get(v2);

  RDIFF d = relativeDiff(a.mPos,b.mPos,0.00001f);
  if ( d == RD_EQUAL )
  {
    d = relativeDiff(a.mNormal,b.mNormal,0.05f);
    if ( d == RD_EQUAL )
    {
      NxVec3 v1( a.mTexel[0], a.mTexel[1], 0 );
      NxVec3 v2( b.mTexel[0], b.mTexel[1], 0 );
      d = relativeDiff( v1, v2, 0.001f );
    }
  }

  if ( d == RD_LESS ) ret = true;

  return ret;
};

bool VertexLess<TetraGraphicsVertex>::operator()(int v1,int v2) const
{
  bool ret = false;


	const TetraGraphicsVertex& a = Get(v1);
	const TetraGraphicsVertex& b = Get(v2);

  RDIFF d = relativeDiff(a.mPos,b.mPos,0.00001f);
  if ( d == RD_EQUAL )
  {
    d = relativeDiff(a.mNormal,b.mNormal,0.05f);
    if ( d == RD_EQUAL )
    {
      NxVec3 v1( a.mTexel[0], a.mTexel[1], 0 );
      NxVec3 v2( b.mTexel[0], b.mTexel[1], 0 );
      d = relativeDiff( v1, v2, 0.001f );
    }
  }

  if ( d == RD_LESS ) ret = true;

  return ret;
};

bool VertexLess<TetraDeformVertex>::operator()(int v1,int v2) const
{
  bool ret = false;


	const TetraDeformVertex& a = Get(v1);
	const TetraDeformVertex& b = Get(v2);

  RDIFF d = relativeDiff(a.mPos,b.mPos,0.00001f);
  if ( d == RD_EQUAL )
  {
    d = relativeDiff(a.mNormal,b.mNormal,0.05f);
    if ( d == RD_EQUAL )
    {
      NxVec3 v1( a.mTexel1[0], a.mTexel1[1], 0 );
      NxVec3 v2( b.mTexel1[0], b.mTexel1[1], 0 );
      d = relativeDiff( v1, v2, 0.001f );
      if ( d == RD_EQUAL )
      {
        NxVec3 v1( a.mTexel2[0], a.mTexel2[1], 0 );
        NxVec3 v2( b.mTexel2[0], b.mTexel2[1], 0 );
        d = relativeDiff( v1, v2, 0.001f );
      }
    }
  }

  if ( d == RD_LESS ) ret = true;

  return ret;
};

bool VertexLess<NxVec3>::operator()(int v1,int v2) const
{
  bool ret = false;

	const NxVec3& a = Get(v1);
	const NxVec3& b = Get(v2);

  RDIFF d = relativeDiff(a,b,0.00001f);
  if ( d == RD_LESS ) ret = true;

  return ret;
};


class SoftVertexPool
{
public:
  SoftVertexPool(SoftVertexType type)
  {
  	mPositionPool = 0;
  	mGraphicsPool = 0;
  	mDeformPool   = 0;
		mTetraGraphicsVertex = 0;

  	mType         = type;

  	switch ( type )
  	{
  		case SVT_TETRA_VERTEX:
  			mGraphicsPool = new VertexPool< TetraVertex >;
  			break;
  		case SVT_TETRA_POSITION_VERTEX:
  			mPositionPool = new VertexPool< NxVec3 >;
  			break;
  		case SVT_TETRA_DEFORM_VERTEX:
  			mDeformPool = new VertexPool< TetraDeformVertex >;
  			break;
  		case SVT_TETRA_GRAPHICS_VERTEX:
  			mTetraGraphicsVertex = new VertexPool< TetraGraphicsVertex >;
  			break;
  	}
  }

  ~SoftVertexPool(void)
  {
  	delete mPositionPool;
  	delete mGraphicsPool;
  	delete mDeformPool;
		delete mTetraGraphicsVertex;
  }


  unsigned int getSoftVertexIndex(const void *vertex)
  {
  	unsigned int ret = 0;

		switch ( mType )
		{
			case SVT_TETRA_POSITION_VERTEX:
				{
	  		const NxVec3 *v = (const NxVec3 *) vertex;
				ret = mPositionPool->GetVertex(*v);
				}
				break;
			case SVT_TETRA_VERTEX:
				{
	  		const TetraVertex *tv = (const TetraVertex *) vertex;
				ret = mGraphicsPool->GetVertex(*tv);
				}
				break;
			case SVT_TETRA_DEFORM_VERTEX:
				{
	  		const TetraDeformVertex *tv = (const TetraDeformVertex *) vertex;
				ret = mDeformPool->GetVertex(*tv);
				}
				break;
			case SVT_TETRA_GRAPHICS_VERTEX:
				{
	  		const TetraGraphicsVertex *tv = (const TetraGraphicsVertex *) vertex;
				ret = mTetraGraphicsVertex->GetVertex(*tv);
				}
				break;
  	}

  	return ret;
  }


  void *       getSoftVertexPoolBuffer(unsigned int &vcount)
  {
  	void *ret = 0;

		switch ( mType )
		{
		case SVT_TETRA_POSITION_VERTEX:
			vcount = mPositionPool->GetVertexCount();
    		if(vcount) ret = mPositionPool->GetBuffer();
    		break;
    	case SVT_TETRA_VERTEX:
			vcount = mGraphicsPool->GetVertexCount();
    		if(vcount) ret = mGraphicsPool->GetBuffer();
    		break;
    	case SVT_TETRA_DEFORM_VERTEX:
			vcount = mDeformPool->GetVertexCount();
    		if(vcount) ret = mDeformPool->GetBuffer();
    		break;
    	case SVT_TETRA_GRAPHICS_VERTEX:
			vcount = mTetraGraphicsVertex->GetVertexCount();
    		if(vcount) ret = mTetraGraphicsVertex->GetBuffer();
    		break;
  	}

  	return ret;
  }

	SoftVertexType                     mType;
  VertexPool< NxVec3 >              *mPositionPool;
  VertexPool< TetraVertex >         *mGraphicsPool;
  VertexPool< TetraDeformVertex >   *mDeformPool;
  VertexPool< TetraGraphicsVertex > *mTetraGraphicsVertex;

};

SoftVertexPool *       createSoftVertexPool(SoftVertexType type)
{
	SoftVertexPool *ret = new SoftVertexPool(type);
	return ret;
}

bool         releaseSoftVertexPool(SoftVertexPool *vpool)
{
	bool ret = false;
	if ( vpool )
	{
		ret = true;
		delete vpool;
	}
	return ret;
}

unsigned int getSoftVertexIndex(SoftVertexPool *vpool,const void *vertex)
{
  unsigned int ret = 0;
  if ( vpool )
	  ret = vpool->getSoftVertexIndex(vertex);
  return ret;
}

void *       getSoftVertexPoolBuffer(SoftVertexPool *vpool,unsigned int &vcount)
{
  void * ret = 0;
  vcount = 0;
  if ( vpool )
	  ret = vpool->getSoftVertexPoolBuffer(vcount);
  return ret;
}

};
