#ifndef NX_COLLISION_NV_HEIGHTFIELD
#define NX_COLLISION_NV_HEIGHTFIELD
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"

class NxHeightFieldDesc;
class NpHeightField;

class NvHeightField
	{
	public:
	virtual		void						release() = 0;

	virtual		bool						saveToDesc(NxHeightFieldDesc&)	const = 0;
	virtual		bool						loadFromDesc(const NxHeightFieldDesc&) = 0;
    virtual		NxU32						saveCells(void * destBuffer, NxU32 destBufferSize) const = 0;

	virtual		NpHeightField*				getNpHeightField() = 0; 
	virtual		void						setNpHeightField(NpHeightField* hf) = 0;

	//only needed as long as we have to compute inertia tensors for bodies based on shapes:
	//virtual		MeshMassInfo				getMassInformation()						const = 0;		//returns 0 when tensor computation failed.

	virtual		NxU32						getNbRowsV()					const = 0;
	virtual		NxU32						getNbColumnsV()					const = 0;
	virtual		NxHeightFieldFormat			getFormatV()					const = 0;
	virtual		NxU32						getSampleStrideV()				const = 0;
	virtual		NxReal						getVerticalExtentV()			const = 0;
	virtual		NxReal						getThicknessV()					const = 0;
	virtual		NxReal						getConvexEdgeThresholdV()		const = 0;
	virtual		NxU32						getFlagsV()						const = 0;
	virtual		NxReal						getHeightV(NxReal x, NxReal z)	const = 0;
	virtual		const void*					getCellsV()						const = 0;
	virtual		void						updateBlock(int xBase, int yBase, int width, int height, int rowStride, void* data) = 0;

	protected:
				~NvHeightField() {}	//use release() plz.
	};

#endif
