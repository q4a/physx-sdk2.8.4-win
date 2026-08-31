#ifndef NX_PHYSICS_NP_SOFTBODYMESH
#define NX_PHYSICS_NP_SOFTBODYMESH

/*----------------------------------------------------------------------------*\
|
| NVIDIA PhysX Technology
|
| www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "ReadWriteMutex.h"
#include "NxSoftBodyMesh.h"

// soft bodies only exists in the Np - Nx layer, below they are cloths
class NvClothMesh;

class NpSoftBodyMesh : public NxSoftBodyMesh, public NxFoundation::NxAllocateable, public ReadWriteMutex
{
public:
	NX_INLINE						NpSoftBodyMesh(NvClothMesh* nvClothMesh) 
										: nvClothMesh(nvClothMesh), referenceCounter(0)  
									{};
	virtual							~NpSoftBodyMesh();

	bool							saveToDesc(NxSoftBodyMeshDesc& desc) const;
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
	NxU32							referenceCounter;	// number of soft body instances referencing this instance
};

#endif
