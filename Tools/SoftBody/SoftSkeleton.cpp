#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "SoftSkeleton.h"
#include "NXU_helper.h"
#include "NXU_GraphicsMesh.h"
#include "TetraGraphics.h"

namespace SOFTBODY
{

class SoftBone
{
public:
	SoftBone(const NXU::NxuBone &b,unsigned int acount,NxActor **alist,SoftBone *parent)
  {
  	mParent = parent;
		mActor = 0;

		for (unsigned int i=0; i<acount; i++)
		{
			NxActor *a = alist[i];
			const char *name = a->getName();
			if ( name )
			{
				if ( strcmp(name, b.mName) == 0 )
				{
					mActor = a;
					break;
				}
			}
		}

		NxQuat q( b.mOrientation );
		mLocalTransform.M.fromQuat(q);
		mLocalTransform.t.set( b.mPosition );

		if ( mActor )
		{
			mGlobalTransform = mActor->getGlobalPose();
		}
		else
		{
			if ( mParent )
			{
  			mGlobalTransform = parent->mGlobalTransform * mLocalTransform;
			}
  		else
			{
  			mGlobalTransform = mLocalTransform;
			}
		}

		mGlobalTransform.getInverse(mComposite);
  }

	const float * getTransform(void)
	{
		if ( mActor )
		{
			mGlobalTransform = mActor->getGlobalPose();
		}
		else if ( mParent )
		{
 			mGlobalTransform = mParent->mGlobalTransform * mLocalTransform;
		}

		NxMat34 t = mGlobalTransform*mComposite;
		t.M.getColumnMajorStride4(mMatrix);
		mMatrix[12] = t.t.x;
		mMatrix[13] = t.t.y;
		mMatrix[14] = t.t.z;

		return mMatrix;
	}

	SoftBone  *mParent;
	NxMat34  	 mGlobalTransform;
	NxMat34    mLocalTransform;
	NxMat34    mComposite;
  NxActor 	*mActor;
  float    	 mMatrix[16];  // the deburst bone matrix in 4x4 D3D style format.
};

class SoftSkeleton
{
 public:
  SoftSkeleton(unsigned int acount,NxActor **alist,NXU::NxuSkeleton *skeleton)
  {
  	int bcount = skeleton->GetNxuBoneCount();
  	mTransforms = new const float*[bcount];
  	for (int i=0; i<bcount; i++)
  	{
			const NXU::NxuBone &b = skeleton->GetNxuBone(i);
  		SoftBone *parent = 0;
  		if ( b.mParentIndex >= 0 && b.mParentIndex < (int)mBones.size() )
  		{
  			parent = mBones[b.mParentIndex];
  		}
			SoftBone *sb = new SoftBone(b,acount,alist,parent);
			mBones.push_back(sb);
  	}

  }

  ~SoftSkeleton(void)
  {
  	for (unsigned int i=0; i<mBones.size(); i++)
  	{
  		SoftBone *sb = mBones[i];
  		delete sb;
  	}
  	delete mTransforms;
  }

	void debugRender(void)
	{
	}

  const float **getTransforms(void) const
  {
  	for (NxU32 i=0; i<mBones.size(); i++)
  	{
  		SoftBone *sb = mBones[i];
  		mTransforms[i] = sb->getTransform();
  	}
    return mTransforms;
  };

  NxArray< SoftBone *>    mBones;
  const float           **mTransforms;
};

// create a bone mapping skeleton between a list of actors and the bones in a graphics skeleton.
SoftSkeleton * createSoftSkeleton(unsigned int acount,NxActor **alist,NXU::NxuSkeleton *skeleton)
{
	SoftSkeleton *ret = 0;

	ret = new SoftSkeleton(acount,alist,skeleton);

  return ret;
}

const float ** getTransforms(SoftSkeleton *sk) // synchronizes the actors with	graphics skeleton and rebuilds the graphics transforms.
{
	const float **ret = 0;

	if ( sk )
		ret = sk->getTransforms();

  return ret;
}

void           releaseSoftSkeleton(SoftSkeleton *sk)
{
	delete sk;
}

void           debugRender(SoftSkeleton *sk)
{
	sk->debugRender();
}

}; // End of namespace

