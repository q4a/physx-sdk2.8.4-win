#ifndef NX_PHYSICS_CLOTHMESH
#define NX_PHYSICS_CLOTHMESH

/*----------------------------------------------------------------------------*\
|
| NVIDIA PhysX Technology
|
| www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Allocateable.h"
#include "NvClothMesh.h"
#include "Array.h"
#include "DeformableMeshDesc.h"
#include "ClothHierarchy.h"

/*----------------------------------------------------------------------------*/

// cloth data sizes
// IMPORTANT! -> make sure these values are the same as in ClothPPUTypes.h!!!
#define CLOTH_PARTICLE_SIZE_WORD						0x0010
#define CLOTH_INTERNALCONSTRAINT_SIZE_WORD				0x0008
#define CLOTH_SOLVERPACKET_DATA_SIZE_WORD				0x03c0
#define CLOTH_SOLVERPACKET_TEARING_PARTICLES_CLOTH		25	
#define CLOTH_SOLVERPACKET_TEARING_PARTICLES_SOFTBODY	45

// cloth vertex flags
// IMPORTANT! -> make sure these values are the same as in ClothPPUTypes.h!!!
enum ClothVertexFlags
{
    CLOTHVERTEX_USER_DEFINED			= 0x2081,   // TEARABLE, ATTACHED and SECONDARY
};

// cloth solverpacket flags
// IMPORTANT! -> make sure these values are the same as in ClothPPUTypes.h!!!
enum ClothSolverPacketFlags {
    SOLVERPACKET_LASTPHASE    = 0x0001,
    SOLVERPACKET_CONTIGUOUS   = 0x0002,
};

// cloth internal constraints
// IMPORTANT! -> make sure these structures have the same memory layout as ClothInternal*ConstraintPPU in ClothPPUTypes.h!!!
struct ClothInternalConstraint {
	NxI16  vertNr[4];
	NxU32  _pads[6];
};

#include "ClothMesh.shared.h"

/*----------------------------------------------------------------------------*/
class ClothSolverPacket : public NxFoundation::NxAllocateable
{
public:
	ClothSolverPacket() { flags = 0; }
	NxU32 flags;
	NxFoundation::NxArraySDK<NxU32> vertices;
	NxFoundation::NxArraySDK<ClothInternalConstraint> constraints;
};


/*----------------------------------------------------------------------------*/

/*
	helper data structures used to speed up tearing
	(currently only used for softbodies)

	1. neighborBlocks maps vertex ids to a contigous blocks of indices of adjacent tetrahedra. 
	   'offset' gives the starting offset into the neighborIndices array
	   'size' gives the total number of adjacent tetrahedra
	2. neighborIndices contains as many tetrahedron indices as there are pairs of adjacent tetrahedra and vertices.
	   it is indexed by the neighborBlocks array
	   its size will not change if new vertices are created during tearing
    3. lastPhasePacketIndices maps vertex ids to the index of their last phase solver packet
	4. constraintRefs maps tetrahedron ids to the solver packet index where its corresponding constraint 
	   is stored (plus the index of said constraint)

	the worst case space usage of these arrays can be estimated:

	there are max 
		- 4 times as many vertices as tetras
		- 3 times as many tetras as vertices

	max neighborBlocks			=     numVertices * 8 bytes
	max neighborIndices			= 3 * numVertices * 4 bytes
	max lastPhasePacketIndices	=     numVertices * 4 bytes
	max constraintRefs			= 3 * numVertices * 8 bytes

	-> max 8 + 12 + 4 + 24 = 48 bytes = 12 u32s per vertex
*/


struct ClothNeighborBlock
{
	NxU32 offset;
	NxU32 size;
};

struct ClothConstraintReference
{
	NxU32 solverPacket;
	NxU32 constraint;
};

struct ClothTearingData
{
	NxFoundation::NxArraySDK<ClothNeighborBlock> neighborBlocks;
	NxFoundation::NxArraySDK<NxU32> neighborIndices;
	NxFoundation::NxArraySDK<NxU32> lastPhasePacketIndices;
	NxFoundation::NxArraySDK<ClothConstraintReference> constraintRefs;	
};

/*----------------------------------------------------------------------------*/

// flags used during vertex / constraint deletion (currently soft body only)

enum ClothVertexDelFlags
{
	CLOTH_VERTEX_DEL_HAS_VOLUME = 0x01,
	CLOTH_VERTEX_DEL_LOOSE = 0x02,
};

/*----------------------------------------------------------------------------*/

class ClothMesh : public NvClothMesh, public NxFoundation::NxAllocateable
{
public:
	ClothMesh() {};
	~ClothMesh();

public:
	/* implementation of NvClothMesh*/
	bool saveToDesc(DeformableMeshDesc& desc) const;
	bool load(const NxStream& stream);

	void setNpClothMesh(NpClothMesh* _npClothMesh) { npClothMesh = _npClothMesh; }
	NpClothMesh* getNpClothMesh() { return npClothMesh; }
	void setNpSoftBodyMesh(NpSoftBodyMesh* _npSoftBodyMesh) { npSoftBodyMesh = _npSoftBodyMesh; }
	NpSoftBodyMesh* getNpSoftBodyMesh() { return npSoftBodyMesh; }

	virtual DeformableType getDeformableType() { return deformableType; }

	/* end of implementation of NvClothMesh*/

	// these procedures are not visible to the user
	// the user has to use NxCookClothMesh
	bool loadFromDesc(const DeformableMeshDesc& desc);	
	bool save(NxStream& stream, bool platformMismatch);

public:	// data used by Cloth or Soft Body
	NxU32 getNumVertices() const { return vertices.size(); }
	NxU32 getNumTriangles() const { return indices.size()/3; }
	NxU32 getNumTetrahedra() const { return indices.size()/4; }

	// these arrays are all stored permuted and welded
	NxFoundation::NxArraySDK<NxVec3> vertices;
	NxFoundation::NxArraySDK<NxReal> vertexMasses;
	NxFoundation::NxArraySDK<NxU32> vertexFlags;

	// the indices are stored unpermuted and unwelded 
	// (otherwise information would be lost)
	NxFoundation::NxArraySDK<NxU32> indices;

	// permutation table contains reordering due to work pakets and welding
	NxFoundation::NxArraySDK<NxU32>	permutationTable;

	// hardware specific information

	NxFoundation::NxArraySDK<NxU32> firstPacketOfPhase;
	NxFoundation::NxArraySDK<ClothSolverPacket *> solverPackets;

	ClothHierarchy clothHierarchy;

	NxU32 getFlags() const { return flags; } 

private:
	void clear();

	bool loadTriangleMesh(const DeformableMeshDesc& desc);
	bool loadTetrahedralMesh(const DeformableMeshDesc& desc);

	void weldMesh(bool weldingEnabled, NxReal weldingDistance);
	NxFoundation::NxArraySDK<NxU32>	 weldingPermutationTable;
	NxFoundation::NxArraySDK<NxVec3> weldedVertices;
	
	void quickSortVertices(NxFoundation::NxArraySDK<int> &order, int l, int r);

	void generateConstraintsFromTriangles();
	void generateConstraintsFromTetrahedra();

	NxFoundation::NxArraySDK<ClothGlobalInternalConstraint> globalInternalConstraints;
	NpClothMesh* npClothMesh;
	NpSoftBodyMesh* npSoftBodyMesh;
	DeformableType deformableType;
	NxU32 flags;
	NxReal weldingDistance;
	NxU32 numHierarchyLevels;
};

/*----------------------------------------------------------------------------*/

#endif
