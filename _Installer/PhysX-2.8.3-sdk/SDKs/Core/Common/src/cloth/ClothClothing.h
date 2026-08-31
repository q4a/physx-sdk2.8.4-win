#ifndef CLOTH_CLOTHING_H
#define CLOTH_CLOTHING_H

#include "Physics.h"
#include "NxFoundation.h"
#include "Array.h"
#include "NxClothDesc.h"
#include "NxSoftBodyDesc.h"

class ClothMesh;

#if defined (__CELLOS_LV2__)
#include "ps3/ClothClothing_PS3.h"
#else
#include "ClothClothing.shared.h"
#endif


// ------------------------------------------------------------------------------

class ClothClothing {
public: 
	ClothClothing();
	~ClothClothing() {};

#if defined (__CELLOS_LV2__)
	// used for SPU
	uintptr_t prepareTaskArgs(ClothClothingPointers& data, NxU32 numParticles);
#endif

	// constraining to animated mesh
	void setConstrainPositions(void* buffer, NxU32 numParticles, NxU32 byteStride, 
		NxFoundation::NxArraySDK<NxU32> &permutationTable);
	void setConstrainNormals(void* buffer, NxU32 numParticles, NxU32 byteStride,
		NxFoundation::NxArraySDK<NxU32> &permutationTable);
	void setClothConstrainCoefficients(const NxClothConstrainCoefficients *coefficients, NxU32 numParticles, NxU32 byteStride,
		NxFoundation::NxArraySDK<NxU32> &permutationTable);
	void setSoftBodyConstrainCoefficients(const NxSoftBodyConstrainCoefficients *coefficients, NxU32 numParticles, NxU32 byteStride,
		NxFoundation::NxArraySDK<NxU32> &permutationTable);

	void constrainPositions(NxU32 numParticles, void *positions, void *newPositions, NxU32 posByteStride,
							void* flags, NxU32 flagsByteStride, float frictionCoefficient, float gridSpacing);

	bool testPositions(NxU32 numParticles, const void *positions, NxU32 posByteStride);

	bool dataReady(NxU32 numParticles) {
		return 
			mConstrainPositions.size() == numParticles &&
			mConstrainCoefficients.size() == numParticles;
	}

	// anti stretching
	void initStretchingProjection(const ClothMesh *clothMesh, NxU32 numParticles, const NxVec3 &gravity, bool isSoftBody);
	void updateStretchingProjection();

	void projectHardStretchingLimit(NxU32 numParticles, void *positions, void *newPositions, NxU32 posByteStride, NxReal hardStretchLimitationFactor);

	void removeParticleStretchingLinks(int particleNr);	// for tearing

	void untangle(NxU32 numParticles, void *positions, void *newPositions, NxU32 posByteStride, const void *origPositions, NxU32 origByteStride);

	const NxFoundation::NxArraySDK<ClothingProjectionConstraint> &getClothingProjectionConstraints() const {
		return mProjectionConstraints;
	}

	// for visualization
	const NxFoundation::NxArraySDK<int> &getSelfCollisionVertices() const {
		return mSelfCollisionVertices;
	}

	NX_INLINE static float invSqrt(float input)
	{
		const float halfInput = 0.5f * input;
		int         i     = *(int*)&input;

		i = 0x5f375a86 - ( i >> 1 );
		input = *(float*) & i;
		input = input * ( 1.5f - halfInput * input * input);
		return input;
	}

private:
	void updateHeap(NxU32 i);

	// Constraints for clothing, only allocated if user calls setConstrainPositions at least once
	NxFoundation::NxArraySDK<NxVec3> mConstrainPositions;
	NxFoundation::NxArraySDK<NxVec3> mConstrainNormals;
	NxFoundation::NxArraySDK<NxClothConstrainCoefficients> mConstrainCoefficients;

	bool projectPointBiasedSphere(const NxVec3 &center, const NxVec3& normal, float normalDist, float tangentialDist, NxVec3 &pos);

	// clothing anti stretching projection
	struct ClothingProjectionHeapEntry {
		bool operator < (const ClothingProjectionHeapEntry &c) const {
			return totalDist < c.totalDist;
		}
		NxF32 totalDist, edgeLength, traversalLength;
		NxI32 parent, child;
	};
	NxFoundation::NxArraySDK<ClothingProjectionHeapEntry> mClothingProjectionHeap;

	NxFoundation::NxArraySDKAlign<NxU32,16> mClothingProjectionFirstEdge;
	NxFoundation::NxArraySDKAlign<ClothingProjectionEdge,16> mClothingProjectionEdges;
	NxFoundation::NxArraySDKAlign<bool,16> mClothingProjectionBorderVertex;
	NxFoundation::NxArraySDK<bool> mClothingParticleMarked;

	NxFoundation::NxArraySDK<ClothingProjectionConstraint> mProjectionConstraints;
	NxFoundation::NxArraySDK<NxVec3> mVelocityUpdate;

	bool mStretchingProjectionInitialized;
	bool mProcessForward;
	bool mIsSoftBody;

	// untangling
	NxFoundation::NxArraySDK<NxU32> mShapeMatchingSubset;
	NxFoundation::NxArraySDK<NxVec3> mShapeMatchingDisplacements;

	// for debug rendering
	NxFoundation::NxArraySDK<int> mSelfCollisionVertices;

#if defined(__CELLOS_LV2__)
	uint32_t mMaxEdges;
#endif
};


#endif
