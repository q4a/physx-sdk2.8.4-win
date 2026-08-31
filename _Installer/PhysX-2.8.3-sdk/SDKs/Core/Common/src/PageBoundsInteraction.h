#ifndef NX_COLLISION_PAGEBOUNDSINTERACTION
#define NX_COLLISION_PAGEBOUNDSINTERACTION
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "PxScene.h"
#include "ShapeInstancePairLL.h"
#include "NxArray.h"

class TriangleMeshShape;

// TODO:	Let the TriangleMeshShape contain a page state time stamp which the PBIs can monitor and early exit in the
//			updatePageMapping to save time when called multiple times because of different events.

class PageBoundsInteraction : public ShapeInstancePairLL
{
public:
	PageBoundsInteraction(TriangleMeshShape& meshShape, Shape &shape, ActorPair& aPair);
	virtual ~PageBoundsInteraction();

	virtual void initialize();
	virtual void destroy();

	void onPageOverlapCreated(NxU32 pageIndex);
	void onPageOverlapRemoved(NxU32 pageIndex);
	// Should not be called between BP and solver
	void unmapPagesNow();

	// Called with true argument if and only if unmapping is ok, 
	// i.e. not between broadphase and solver.
	void updatePageMapping(bool unmapNow=false);

	NX_INLINE const TriangleMeshShape* getMeshShape() const;	
	NX_INLINE TriangleMeshShape* getMeshShape();
	NX_INLINE const Shape* getShape() const;
	NX_INLINE NxU32 getOverlapCount() const;

	// These are called when pages are mapped/unmapped to card
	NX_INLINE void onPageInstanceRemoved(NxU32 pageIndex);
	NX_INLINE void onPageInstanceCreated(NxU32 pageIndex);

protected:
	virtual void beforeManagerDestroyed();
	virtual void afterManagerCreated();

	bool mapPage(NxU32 pageIndex);
	bool unmapPage(NxU32 pageIndex);
	bool isHwReady() const;

	enum PbiFlag
	{
		PBI_HAS_PENDING_UNMAP				= (1<<0),
		PBI_IS_IN_HW_MODE					= (1<<1),
	};

	enum PageFlag
	{
		PAGE_HAS_OVERLAP					= (1<<0),
		PAGE_IS_MAPPED_TO_MANAGER			= (1<<1),
	};

	NX_INLINE bool readPageFlag(NxU32 pageIndex, PageFlag flag) const;
	NX_INLINE void raisePageFlag(NxU32 pageIndex, PageFlag flag);
	NX_INLINE void clearPageFlag(NxU32 pageIndex, PageFlag flag);
	NX_INLINE bool readFlag(PbiFlag flag) const;
	NX_INLINE void raiseFlag(PbiFlag flag);
	NX_INLINE void clearFlag(PbiFlag flag);

	struct PageData {
		NxU32 flags;
	};
	NxU32 mFlags;
	NxU32 mOverlapCount;
	NxFoundation::NxArraySDK<PageData> mPages;
};

//////////////////////////////////////////////////////////////////////////

NX_INLINE const TriangleMeshShape* PageBoundsInteraction::getMeshShape() const 
{
	return (TriangleMeshShape*)&getShape0();
}

NX_INLINE TriangleMeshShape* PageBoundsInteraction::getMeshShape()
{
	return (TriangleMeshShape*)&getShape0();
}

NX_INLINE const Shape* PageBoundsInteraction::getShape() const 
{ 
	return &getShape1(); 
}

NX_INLINE NxU32 PageBoundsInteraction::getOverlapCount() const
{
	return mOverlapCount;
}

NX_INLINE void PageBoundsInteraction::onPageInstanceCreated(NxU32 /*pageIndex*/)
{
	// Should not be called between BP and solver
	updatePageMapping(true);
}

NX_INLINE void PageBoundsInteraction::onPageInstanceRemoved(NxU32 /*pageIndex*/)
{
	// Should not be called between BP and solver
	updatePageMapping(true);
}

NX_INLINE bool PageBoundsInteraction::readPageFlag(NxU32 pageIndex, PageFlag flag) const
{
	return (mPages[pageIndex].flags & flag) != 0;
}

NX_INLINE void PageBoundsInteraction::raisePageFlag(NxU32 pageIndex, PageFlag flag)
{
	mPages[pageIndex].flags |= flag;
}

NX_INLINE void PageBoundsInteraction::clearPageFlag(NxU32 pageIndex, PageFlag flag)
{
	mPages[pageIndex].flags &= ~flag;
}

NX_INLINE bool PageBoundsInteraction::readFlag(PbiFlag flag) const
{
	return (mFlags & flag) != 0;
}

NX_INLINE void PageBoundsInteraction::raiseFlag(PbiFlag flag)
{
	mFlags |= flag;
}

NX_INLINE void PageBoundsInteraction::clearFlag(PbiFlag flag)
{
	mFlags &= ~flag;
}

#endif
