/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
#ifndef CLOTH_SPU_H
#define CLOTH_SPU_H
#include "CellSPU.h"
#include "CellSPURSSupport.h"

#include "NxMath.h"

#include "gen/NxU8.gen.h"
#include "gen/NxI16.gen.h"
#include "gen/NxU16.gen.h"
#include "gen/NxI32.gen.h"
#include "gen/NxU32.gen.h"
#include "gen/NxReal.gen.h"
#include "gen/NxShapeType.gen.h"

#include "NxMat34.spu.h"
#include "NxBounds3.spu.h"

#include "ClothMesh.shared.h"

#include "ps3/ClothCollision_PS3.h"

#include "CellClothConfig.h"

#include "ps3/ClothHierarchy_PS3.h"
#include "NxClothConstrainCoefficients.spu.h"
#include "ps3/ClothClothing_PS3.h"


#include "ps3/Cloth_PS3.h"
#include "Cloth.shared.h"

#include "NxClothFlag.spu.h"

#include <float.h>
#include "gen/NxRay.gen.h"

#include "ShapeMatcher.spu.h"

#define SPUD_ASSERT(X)		(void)0
#include "CellPhysXMemoryManager.h"
#include "px_config.h"
#include "px_types.h"
#include "SPU_SolverShared/CellDataStream.h"

#define SPU_CLOTH_MAX_BUFFER_SIZE SPU_CLOTH_MAX_PARTICLES*sizeof(SelfCollisionParticle) // 57344-1024 // = largest data that needs to fit in full buffer: SPU_CLOTH_MAX_PARTICLES*sizeof(SelfCollisionParticle)
#define SPU_CLOTH_MAX_COLL_SHAPES (2*DOUBLEBUFFERSIZE)/sizeof(CellClothShape);

#define DOUBLEBUFFERSIZE 8*1024 // reduced buffer size so more buffers can be used for hierarchy stuff
#define ADDITIONALBUFFERSIZE SPU_CLOTH_MAX_BUFFER_SIZE-(2*DOUBLEBUFFERSIZE)

#define MAXCONSTRAINTSPERBUFFER DOUBLEBUFFERSIZE/sizeof(ClothGlobalInternalConstraint)
#define MAXATTACHMENTSPERBUFFER DOUBLEBUFFERSIZE/sizeof(NxMat34)
#define MAXHCONSTRAINTSPERBUFFER DOUBLEBUFFERSIZE/sizeof(ClothHierarchyLevelConstraint) & ~3

class ClothSPU
{
	friend class ClothManager;
public:
	/*inline*/ void process(uintptr_t clothPointersEA);
private:
	/*inline*/ void solverStep(NxReal dt);
	/*inline*/ void projectInternalConstraintsDB();
	/*inline*/ void computeNewState(NxReal dt);
	/*inline*/ void intraSelfCollision(NxReal dt);
	/*inline*/ int cellIndex(int phase, NxVec3 &pos, NxReal cellW, NxReal invCellW, NxReal h);
	///*inline*/ int cellIndex(int phase, Vector3 &pos, NxReal cellW, NxReal invCellW, NxReal h);
	/*inline*/ void resetAttachmentImpulses();
	/*inline*/ void dampCOM();
	/*inline*/ void adhereToAttachments(NxReal dt);
	/*inline*/ void adhereToAttachmentsInnerLoop(void* attachments, uint32_t attachmentCount);
	/*inline*/ void updateInertia(NxMat33 &I, const NxVec3 &r, NxReal m);
	/*inline*/ void collisionDetection(const NxVec3 &offset = NxVec3(0.0f));
	/*inline*/ void projectInternalClothConstraints(void* constraints, uint32_t numConstraints);
	/*inline*/ void projectInternalSoftBodyConstraints(void* constraints, uint32_t numConstraints);
	
	/*inline*/ void projectAttachmentConstraints(void* attachments, uint32_t numAttachments);

	/*inline*/ void projectDoubleBuffered(
		uint32_t loadEA, 
		uint32_t sizeOfElt, 
		uint32_t numElts, 
		void (ClothSPU::*projection) (void*, uint32_t)); 
	/*inline*/ void projectContacts(NxReal dt);

	/*inline*/ void DataToEA();

	// hierarchy
	ClothHierarchyPointers CELL_ALIGN(16,hierarchyPointers);
	NxVec3* mPreCorrectedPositions;
	ClothHierarchyPerParticleInfo* mPerParticleInfos;
	bool isHierarchyInitialized() { return hierarchyPointers.numHierarchyLevels > 0; }
	void multigridSolve(
		NxU32 numVertices,
		void *vertexPositions, NxU32 posByteStride,
		void *vertexFlags, NxU32 flagsByteStride,
		void *vertexInvMasses, NxU32 invMassByteStride,
		NxU32 numIterations,
		bool directDistance, bool oneSided, 
		float distanceScaling,
		void* buf0, void* buf1,
		void* buffer);
	void solveLevel(const unsigned int level, const int numIters, const bool directDistance, const bool oneSided, const float distanceScaling, LoaderR& loader);
	void prolongLevel(const unsigned int level,const  void* parentBuf,const  void* weigthBuf,const  bool absolute = false);
	void projectDistanceConstraint(const int pNr0,const  int pNr1,const  float distance,const  float stiffness,const  bool oneSided);
	void reduceMultigridDatastructures(int particleNr);
	/*inline*/ void removeConstraints(int firstConstraintLink,uint32_t constraintLinksEA);

	// clothing
	ClothClothingPointers CELL_ALIGN(16,clothingPointers);
	void projectHardStretchingLimit(NxU32 numParticles, void *positions, void *newPositions, NxU32 posByteStride, NxReal hardStretchLimitationFactor);
	void untangle(NxU32 numParticles, void *positions, void *newPositions, NxU32 posByteStride, const void *origPositions, NxU32 origByteStride);
	void removeParticleStretchingLinks(int particleNr);
	/*inline*/ static float invSqrt(float input)
	{
		const float halfInput = 0.5f * input;
		int         i     = *(int*)&input;

		i = 0x5f375a86 - ( i >> 1 );
		input = *(float*) & i;
		input = input * ( 1.5f - halfInput * input * input);
		return input;
	}


	// temp data
	uint32_t CELL_ALIGN(16,particleSize);
	bool mAdhered;
	NxMat34 avgTrans;
	NxU32 numAttachments;	

	CellClothPointers CELL_ALIGN(16,clothPointers);
	ClothInfo CELL_ALIGN(16,clothInfo);
	ClothParticle CELL_ALIGN(16,mParticles[SPU_CLOTH_MAX_PARTICLES]);
	char CELL_ALIGN(16,mDoubleBuffer[2][DOUBLEBUFFERSIZE]);
	char CELL_ALIGN(16,mAdditionalBuffer[ADDITIONALBUFFERSIZE]);
	//char mDoubleBuffer[2][DOUBLEBUFFERSIZE];
	//char mAdditionalBuffer[ADDITIONALBUFFERSIZE];

	// aux helper
	void printNewPos(int frame);
	void printNewPos(int pNr, int line);
};

#endif


