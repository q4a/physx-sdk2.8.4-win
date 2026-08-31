#ifndef NX_COLLISION_NP_HEIGHTFIELD
#define NX_COLLISION_NP_HEIGHTFIELD
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxFoundation.h"
#include "NxHeightField.h"
#include "Allocateable.h"
#include "RefCountable.h"

class NvHeightField;
class NxHeightFieldDesc;

class NpHeightField : public NxHeightField, public RefCountable, public NxFoundation::NxAllocateable
	{
	
	public:
										NpHeightField(NvHeightField* heightField);
	virtual								~NpHeightField();

	// Implements NxHeightField
	virtual		bool					saveToDesc(NxHeightFieldDesc&)	const;
	virtual		bool					loadFromDesc(const NxHeightFieldDesc&);
    virtual		NxU32					saveCells(void * destBuffer, NxU32 destBufferSize) const;

	NX_INLINE	NvHeightField*			getHeightField()					{ return mHeightField;	}
	NX_INLINE	const NvHeightField*	getHeightFieldConst()		const	{ return mHeightField;	}

	virtual		NxU32					getNbRows()					const;
	virtual		NxU32					getNbColumns()				const;
	virtual		NxHeightFieldFormat		getFormat()					const;
	virtual		NxU32					getSampleStride()			const;
	virtual		NxReal					getVerticalExtent()			const;
	virtual		NxReal					getThickness()				const;
	virtual		NxReal					getConvexEdgeThreshold()	const;
	virtual		NxU32					getFlags()					const;

	virtual		NxReal					getHeight(NxReal x, NxReal z) const;
	virtual		const void*				getCells()					const;
	virtual		void					updateBlock(int xBase, int yBase, int width, int height, int rowStride, void* data);
	virtual		NxU32					getReferenceCount()					{ return getNumRefs(); }

				void					sendToVRD(bool create, bool fromCtor);
				void					removeFromVRD();
	private:
				NvHeightField*			mHeightField;
	};

#endif
