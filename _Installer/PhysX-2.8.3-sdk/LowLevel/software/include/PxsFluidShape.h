#ifndef PXS_FLUIDSHAPE_H
#define PXS_FLUIDSHAPE_H

#include "PxsFluidConfig.h"
#ifdef PXS_ENABLE_FLUIDS

#include "PxnFluidShape.h"

#include "PxcTransform.h"
#include "PxcSmallArray.h"
#include "PxcListNode.h"
#include "PxcGridCellVector.h"

#include "PxsFluidSpatialHash.h"

// For the inlining
#include "PxsFluid.h"

class PxsContext;
class PxsFluidBodyContactManager;
struct PxsFluidPacket;

class PxsFluidShape : public PxnFluidShape
{
public:
	typedef PxcListNode<PxsFluidBodyContactManager>	PxsFluidBodyContactManagerListNode;


											PxsFluidShape(PxsContext* context, PxU32 index);
	virtual									~PxsFluidShape();

			void							init(PxsFluid* fluid, const PxsFluidPacket* packet);

	// Implements PxnFluidShape
	virtual PxnFluid*						getFluidV()							const;

	virtual	const PxdVector&				getOriginV()						const;

	virtual	const PxdVector&				getExtentV()						const;

	virtual	PxU16							getParticleOffsetV()				const;

	virtual	PxU16							getParticleCountV()					const;

	virtual void							setUserDataV(void* data);
	virtual void*							getUserDataV()						const;

	virtual PxdHandle						getHandleV()						const;
	virtual void							destroyV();
	//~Implements PxnFluidShape

	PXD_FORCE_INLINE	PxU32				getIndex()							const	{ return mIndex;			}
	PXD_FORCE_INLINE	PxsFluid*			getFluid()									{ return mFluid;			}
	PXD_FORCE_INLINE	const PxsFluid*		getFluid()							const	{ return mFluid;			}
	PXD_FORCE_INLINE    void				setFluidPacket(const PxsFluidPacket* packet)	{ PXN_ASSERT(packet); mPacket = packet; }
	PXD_FORCE_INLINE	const PxsFluidPacket* getFluidPacket()					const	{ return mPacket;			}
	PXD_FORCE_INLINE	PxcGridCellVector	getFluidPacketCoordinates()			const	{ return mPacketCoordinates; }

	// Add contact manager to list.
	PXD_INLINE			void				addToContactManagerList(PxsFluidBodyContactManagerListNode& cm)
											{
												cm.insertAfter(&mContactManagerList);
											}

	PXD_INLINE			PxsFluidBodyContactManagerListNode& getContactManagerList()
											{
												return mContactManagerList;
											}

	PXD_INLINE			void				computeBounds(PxcVector& origin, PxcVector& extent)
											{
												origin = mOrigin;
												extent = mExtent;
											}

private:
						PxsContext*			mContext;
						PxU32				mIndex;

						PxsFluid*			mFluid;
						PxcVector			mOrigin;
						PxcVector			mExtent;

						PxcGridCellVector	mPacketCoordinates;
						const PxsFluidPacket* mPacket;

						void*				mUserData;

						// Pointer to first contact manager
						PxsFluidBodyContactManagerListNode mContactManagerList;
};


#endif	// PXS_ENABLE_FLUIDS

#endif
