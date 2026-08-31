#ifndef NX_PHYSICS_NP_CLOTHMESH
#define NX_PHYSICS_NP_CLOTHMESH

/*----------------------------------------------------------------------------*\
|
| NVIDIA PhysX Technology
|
| www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "ReadWriteMutex.h"
#include "NxClothMesh.h"

class NvClothMesh;

class NpClothMesh : public NxClothMesh, public NxFoundation::NxAllocateable, public ReadWriteMutex
{
public:
	NX_INLINE						NpClothMesh(NvClothMesh* nvClothMesh) 
										: nvClothMesh(nvClothMesh), referenceCounter(0)  
									{};
	virtual							~NpClothMesh();

	bool							saveToDesc(NxClothMeshDesc& desc) const;
	bool							load(const NxStream& stream);

	NX_INLINE	const NvClothMesh*	getNvClothMeshConst() const	{ return nvClothMesh; }
	NX_INLINE	NvClothMesh*		getNvClothMesh() { return nvClothMesh; }

	NxU32							getReferenceCount() const { return referenceCounter; }

	void							increaseReferenceCounter() { referenceCounter++; }
	bool							decreaseReferenceCounter();

	bool							loadFast(const NxStream& stream); // Not locking scenes

protected:
	NvClothMesh* nvClothMesh;
private:
	NxU32							referenceCounter;	// number of Cloth instances referencing this instance
};

#endif
