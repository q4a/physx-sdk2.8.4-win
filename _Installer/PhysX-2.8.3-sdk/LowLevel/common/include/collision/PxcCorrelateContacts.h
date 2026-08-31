#ifndef PXC_CORRELATECONTACTS_H
#define PXC_CORRELATECONTACTS_H

#include "PxcArray.h"
#include "PxcVector.h"
#include "PxcMatrix34.h"

extern const PxFloat PXC_SAME_NORMAL;

struct PxcPersistentContact
{
	PxcVector		mWorldPoint;
	PxcVector		mLocalPoint[2];
	PxcVector		mLocalNormal[2];
	PxcVector		mError;
	PxcQuaternion	mWorldContactFrame;
	PxU32			mFeature0;
	PxU32			mFeature1;
	bool			mContactTouched;
	bool			mIgnoreContact;
};

struct PxcTemporaryContact
{
	PxcVector	mWorldPoint;
	PxcVector	mWorldNormal;
	PxFloat		mSeparation;
	PxU32		mFeature0;
	PxU32		mFeature1;
};

PxcQuaternion PxcComputeQuatFromNormal(const PxcVector& n);

void PxcCorrelateContacts(const PxcMatrix34& bodyFrame0, 
						  const PxcMatrix34& bodyFrame1,
						  PxcArray<PxcPersistentContact>& persistentContacts,
						  const PxcArray<PxcTemporaryContact>& temporaryContacts,
  						  PxFloat corrEpsilon,
						  PxFloat skinWidth);


#endif
