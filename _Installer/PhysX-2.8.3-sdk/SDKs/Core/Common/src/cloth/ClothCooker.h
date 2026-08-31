#ifndef CLOTH_COOKER_H
#define CLOTH_COOKER_H

/*----------------------------------------------------------------------------*\
|
| NVIDIA PhysX Technology
|
| www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "NxFoundation.h"
#include "ClothMesh.h"
#include "Array.h"

// ---------------------------------------------------------------------------------------
class ClothCooker {
public: 
	ClothCooker(
				DeformableType deformableType,
				bool tearable,
				const NxFoundation::NxArraySDK<NxVec3> &vertices, 
				const NxFoundation::NxArraySDK<ClothGlobalInternalConstraint> &constraints,
				NxFoundation::NxArraySDK<NxU32> &permutationTable,
				NxFoundation::NxArraySDK<NxU32> &firstPacketOfPhase,
				NxFoundation::NxArraySDK<ClothSolverPacket*> &solverPackets
				) :
			mDeformableType(deformableType),
			mTearable(tearable),
			mVertices(vertices), mConstraints(constraints),
			mPermutationTable(permutationTable),
			mFirstPacketOfPhase(firstPacketOfPhase),
			mSolverPackets(solverPackets),
			mNumPackets(0), mNumPhases(0)
		{};
	~ClothCooker() {};

	void generatePackets();
	void generatePermutationTable();

	static const int maxPhases = 32;

private:
	// vertex -> adjacent constraints map for speeding up workpacket generation
	void generateConstraintsOfVerticesList();
	NxFoundation::NxArraySDK<NxU32> mNumConstraintsOfVertex;
	NxFoundation::NxArraySDK<NxU32> mFirstConstraintOfVertex;
	NxFoundation::NxArraySDK<NxU32> mConstraintsOfVertices;

	void checkConstraint(
		int constraintNr, int currentPacket, NxFoundation::NxArraySDK<int> *currentPacketOfVertex, 
		const NxBounds3 &currentPacketBounds, int &bestVal, NxReal &bestS, int &bestNr);

	DeformableType mDeformableType;
	bool mTearable;	
	NxFoundation::NxArraySDK<int> mPacketOfVertex[maxPhases];
	int mNumPackets;
	int mNumPhases;

	const NxFoundation::NxArraySDK<NxVec3> &mVertices;
	const NxFoundation::NxArraySDK<ClothGlobalInternalConstraint> &mConstraints; 

	NxFoundation::NxArraySDK<NxU32> &mPermutationTable;
	NxFoundation::NxArraySDK<NxU32> &mFirstPacketOfPhase;
	NxFoundation::NxArraySDK<ClothSolverPacket*> &mSolverPackets;
};


#endif
