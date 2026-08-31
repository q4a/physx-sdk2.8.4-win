#ifndef CLOTH_METAL_H
#define CLOTH_METAL_H

#include "Physics.h"
#include "NxFoundation.h"
#include "Array.h"

class NvShape;
class NvBody;
class NxActor;

// ------------------------------------------------------------------------------

class ClothMetal {
public: 
	ClothMetal();
	~ClothMetal();

	bool setCoreShape(NvShape *coreShape, NxReal velocityThreshold, 
				NxReal penetrationDepth, NxReal maxDeformationDistance);
	void removeCoreShape() { mCoreShape = NULL; }
	bool isActivated() const { return mCoreShape != NULL; }
	bool isSleeping() const;
	NvBody* getBody() const;
	NxActor* getActor() const;
	NxReal getPenetrationDepth() { return mPenetrationDepth; }

	void timeStep(NxReal dt, bool &update, NxVec3 &offset);
	void updateCoreShape(void* positions, NxU32 numPoints, NxU32 strideBytes, NxReal halfThickness);

	bool coreContains(NvShape *shape);

	void getWorldBounds(NxBounds3 &bounds) const;

private:
	bool updateNeeded(NxReal dt);
	void updateCompound(void* positions, NxU32 numPoints, NxU32 strideBytes);
	void limitDeformation(void* originalPositions, NxU32 numPoints, NxU32 strideBytes, NxVec3 offset);

	// metal representation
	NvShape* mCoreShape;
	NxReal   mImpulseThresholdSquared;
	NxReal   mPenetrationDepth;
	NxReal   mMaxDeformationDistance;

	// deformation data
	NxVec3    mCollisionOffset;
	int       mUpdateCounter;
	NxBounds3 mBounds;
	NxFoundation::NxArraySDK<NxVec3> mUndeformedPositions;	// for limited destruction

	// for compound shapes placement
	NxFoundation::NxArraySDK<NxBounds3> mGrid;
	NxFoundation::NxArraySDK<NxVec3> mShapeCenters;
	NxFoundation::NxArraySDK<NxVec3> mShapeRadii;
	NxFoundation::NxArraySDK<NxReal> mShapeMinDist2;
};


#endif
