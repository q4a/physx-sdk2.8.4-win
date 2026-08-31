#ifndef NX_COLLISION_PAGEBOUNDSELEMENT
#define NX_COLLISION_PAGEBOUNDSELEMENT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "CoreElement.h"
#include "TriangleMeshShape.h"
#include "PageBoundsInteraction.h"

class Scene;
class Shape;
class ShapeInstancePairLL;


class PageBoundsElement : public CoreElement
{
public:
	PageBoundsElement(TriangleMeshShape &meshShape, NxU32 pageIndex, NxU32 broadphaseGroup);
	virtual void initialize();
	virtual void destroy();

	NX_INLINE bool canDeactivate() const { return true; }
	virtual bool isActive() const { return false; }
	NX_INLINE TriangleMeshShape& getMeshShape() const { return mMeshShape; }
	NX_INLINE NxU32 getPageIndex() const { return mPageIndex; }
	NX_INLINE void updateBroadPhase();
	NX_INLINE void onPageInstanceCreated();
	NX_INLINE void onPageInstanceRemoved();
	NX_INLINE void onPageOverlapCreated(PageBoundsInteraction* pbi);
	NX_INLINE void onPageOverlapRemoved(PageBoundsInteraction* pbi);
	NX_INLINE NxU32 getPageOverlapCount() const;
	NX_INLINE PageBoundsInteraction* getPageOverlap(NxU32 index) const;

protected:
private:
	virtual ~PageBoundsElement();
	NxU32 mPageIndex;
	TriangleMeshShape& mMeshShape;
	NxFoundation::NxArraySDK<PageBoundsInteraction*> mOverlapList;
};

////////////////////////////////////////////////////////////////////////////
NX_INLINE void PageBoundsElement::updateBroadPhase()
{
	NxBounds3 bounds = mMeshShape.getPageWorldBounds(mPageIndex);
	NxVec3 vec1;
	bounds.getCenter(vec1);
	PxdVector llVec;
	llVec.x = vec1.x; llVec.y = vec1.y; llVec.z = vec1.z;
	if (getLowLevelVolume() != 0)
		PxdVolumeSetVec(getLowLevelVolume(), PXD_VOLUME_ORIGIN, llVec);
	NxVec3 vec2;
	bounds.getExtents(vec2);
	llVec.x = vec2.x; llVec.y = vec2.y; llVec.z = vec2.z;
	if (getLowLevelVolume() != 0)
		PxdVolumeSetVec(getLowLevelVolume(), PXD_VOLUME_EXTENT, llVec);
}

NX_INLINE void PageBoundsElement::onPageInstanceRemoved()
{
	// SFD: dangerous loop, if the array gets updated
	NxU32 nbOverlaps = getPageOverlapCount();
	for (NxU32 i=0; i<nbOverlaps; ++i)
		getPageOverlap(i)->onPageInstanceRemoved(getPageIndex());
	NX_ASSERT(nbOverlaps == getPageOverlapCount());
}

NX_INLINE void PageBoundsElement::onPageInstanceCreated()
{
	// This should only be called when unbinding of managers is ok
	// Not between broadphase and solver
	// SFD: dangerous loop, if the array gets updated
	NxU32 nbOverlaps = getPageOverlapCount();
	for (NxU32 i=0; i<nbOverlaps; ++i)
		getPageOverlap(i)->onPageInstanceCreated(getPageIndex());
	NX_ASSERT(nbOverlaps == getPageOverlapCount());
}

NX_INLINE void PageBoundsElement::onPageOverlapCreated(PageBoundsInteraction* pbi)
{
	mOverlapList.pushBack(pbi);
}

NX_INLINE void PageBoundsElement::onPageOverlapRemoved(PageBoundsInteraction* pbi)
{
	// TODO: Shrink on ratio
	bool found = mOverlapList.deleteEntry(pbi);
}

NX_INLINE NxU32 PageBoundsElement::getPageOverlapCount() const
{
	return mOverlapList.size();
}

NX_INLINE PageBoundsInteraction* PageBoundsElement::getPageOverlap(NxU32 index) const
{
	NX_ASSERT(index < mOverlapList.size());
	return mOverlapList[index];
}

#endif
