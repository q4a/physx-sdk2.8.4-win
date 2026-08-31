#ifndef CLOTH_HIERARCHY_H
#define CLOTH_HIERARCHY_H

#include "Physics.h"
#include "NxFoundation.h"
#include "NxClothMeshDesc.h"
#include "DebugRenderable.h"
#include "Array.h"
#include "NxStream.h"
#if defined(__CELLOS_LV2__)
#include "ps3/ClothHierarchy_PS3.h"
#else
#include "ClothHierarchy.shared.h"
#endif

namespace NxFoundation
{
class  DebugRenderable;
}

// ------------------------------------------------------------------------------

class ClothHierarchy {
public: 
	ClothHierarchy();
	~ClothHierarchy();
	void clear();

#if defined (__CELLOS_LV2__)
	// used for SPU
	uintptr_t prepareTaskArgs(ClothHierarchyPointers& data);
#endif

	bool isInitialized() { return mNumLevels > 0; }

	void multigridSolve(
		NxU32 numVertices,
		void *vertexPositions, NxU32 posByteStride,
		void *vertexFlags, NxU32 flagsByteStride,
		void *vertexInvMasses, NxU32 invMassByteStride,
		NxU32 numIterations,
		bool directDistance, bool oneSided, 
		float distanceScaling
		);

	void createHierarchy(
		NxU32 numLevels,
		NxU32 numConstraints, 
		void *constraintIndex0, void *constraintIndex1, NxU32 indexByteStride,
		void *restDistances, NxU32 restDistanceByteStride,
		NxU32 numVertices,
		void *vertexPositions, NxU32 posByteStride
		);

	// used when the cloth is torn
	void reduceMultigridDatastructures(int particleNr);

	// for visualization
	const NxFoundation::NxArraySDKAlign<ClothHierarchyLevelConstraint,16> &getLevelConstraints(int level) {
		return mLevelConstraints[level];
	}

	void copyFrom(ClothHierarchy &hierarchy);
	void permute(const NxFoundation::NxArraySDK<NxU32> &permutation);

	void save(NxStream& stream, bool platformMismatch);
	bool load(const NxStream& stream, bool platformMismatch);

	int  getNumLevels() const { return mNumLevels; }

private:
	void projectDistanceConstraint(int pNr0, int pNr1, float distance, float stiffness, bool oneSided);
	void solveLevel(int level, int numIters, bool directDistance, bool oneSided, float distanceScaling);
	void prolongLevel(int level, bool absolute = false);
	void addLevelConstraint(int level, const ClothHierarchyLevelConstraint &lc);
	void multigridDatastructuresCheck();

	// final datastructures stored in the cooked mesh
	// used by the solver
	int mNumLevels;
	int mLevelItersFactor;
	NxFoundation::NxArraySDKAlign<ClothHierarchyLevelConstraint,16> mLevelConstraints[NX_CLOTH_MAX_HIERARCHY_LEVELS];
	NxFoundation::NxArraySDK<NxI32> mLevelParticles[NX_CLOTH_MAX_HIERARCHY_LEVELS];
	NxFoundation::NxArraySDKAlign<ClothHierarchyPerParticleInfo,16> mPerParticleInfos;
	NxFoundation::NxArraySDKAlign<NxI32,16> mParents;
	NxFoundation::NxArraySDKAlign<NxReal,16> mParentWeights;
	NxFoundation::NxArraySDKAlign<ClothHierarchyParticleConstraintLink,16> mConstraintLinks;

	// auxiliaries
	NxU32 mNumVertices;
	void *mVertexPositions;
	NxU32 mPosByteStride;
	void *mVertexFlags;
	NxU32 mFlagsByteStride;
	void *mVertexInvMasses;
	NxU32 mInvMassByteStride;

	NxU32 mNumConstraints;
	void *mConstraintIndex0;
	void *mConstraintIndex1; 
	NxU32 mIndexByteStride;
	void  *mRestDistances;
	NxU32 mRestDistanceByteStride;

	NxFoundation::NxArraySDK<NxVec3> mPreCorrectedPositions;

	// accessors
	inline NxVec3& vertexPos(int i) { return *(NxVec3*)((NxU8*)mVertexPositions + i * mPosByteStride); }
	inline NxU32&  index0(int i) { return *(NxU32*)((NxU8*)mConstraintIndex0 + i * mIndexByteStride); }
	inline NxU32&  index1(int i) { return *(NxU32*)((NxU8*)mConstraintIndex1 + i * mIndexByteStride); }
	inline NxReal& restDist(int i) { return *(NxReal*)((NxU8*)mRestDistances + i * mRestDistanceByteStride); }
	inline NxReal& vertexInvMass(int i) { return *(NxReal*)((NxU8*)mVertexInvMasses + i * mInvMassByteStride); }
	inline NxU32&  vertexFlags(int i) { return *(NxU32*)((NxU8*)mVertexFlags + i * mFlagsByteStride); }
};

// ------------------------------------------------------------------------------
#endif
