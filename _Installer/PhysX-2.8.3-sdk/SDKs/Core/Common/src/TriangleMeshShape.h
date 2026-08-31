#ifndef NX_COLLISION_TRIANGLEMESHSHAPE
#define NX_COLLISION_TRIANGLEMESHSHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/


#include "NxSimpleTriangleMesh.h"
#include "NxTriangleMeshShape.h"
#include "TriangleMesh.h"
#include "Shape.h"
#include "Opcode.h"
#include "InternalTriangleMesh.h"
#include "Scene.h"


#undef INC_SCENE_STAT
#undef DEC_SCENE_STAT
#ifdef NX_ENABLE_SCENE_STATS2
#undef ADD_TO_SCENE_STATS
#undef INC_SCENE_STAT
#undef DEC_SCENE_STAT
#define ADD_TO_SCENE_STATS(x, y) getOwnerSceneFast()->stats2.addToStat(x, y)
#define INC_SCENE_STAT(x) getOwnerSceneFast()->stats2.incStat(x)
#define DEC_SCENE_STAT(x) getOwnerSceneFast()->stats2.decStat(x)
#else
#define ADD_TO_SCENE_STATS(x, y)
#define INC_SCENE_STAT(x)
#define DEC_SCENE_STAT(x)
#endif

class NxTriangleMeshShapeDesc;

//namespace NxCollision {

class TriangleMesh;
class NpTriangleMesh;
class NvTriangleMesh;
namespace Opcode
{
struct AABBCache;
}

class PageBoundsElement;

class TriangleMeshShape : public Shape
	{
	protected:
									TriangleMeshShape(const NvShapeDesc & desc, Scene *, CoreActor& actor, NxU32 uid);
	virtual							~TriangleMeshShape();

	public:

	// Shape interface
	virtual		Shape&				getShape()			{ return *this;	}
	virtual		const Shape&		getShape()	const	{ return *this;	}
	virtual		operator			Shape&()			{ return *this;	}
	virtual		void				visualize(NxFoundation::DebugRenderable& renderer)													const;
	virtual		bool				raycast(const NxRay& worldRay, NxReal maxDist, NxU32 hintFlags, NxRaycastHit& hit, bool firstHit)	const;
	virtual		bool				checkOverlapSphere(const NxSphere& worldSphere)														const;
	virtual		bool				checkOverlapOBB(const NxBox& worldBox)																const;
	virtual		bool				checkOverlapAABB(const NxBounds3& worldBounds)														const;
	virtual		bool				checkOverlapCapsule(const NxCapsule& worldCapsule)													const;
	virtual		void				removeFromBroadPhase();
	virtual		bool				hasSpecialBroadphase() const { return true; }

#ifdef SUPPORT_INTERNAL_RADIUS
	virtual		bool				getInternalRadius(NxReal& radius, const NxVec3& dir)												const;
#endif
	virtual		void				computeLocalBounds(NxBounds3& bounds)																const;
	//~Shape interface

				bool				overlapAABBTriangles(const NxBounds3 bounds, NxU32 flags, NxUserEntityReport<NxU32>* callback)		const;

	virtual		NxU32				getTriangle(NxTriangle& triangle, NxTriangle* edgeTri, NxU32* returnedFlags, NxTriangleID triangleIndex, bool worldSpaceTranslation=true, bool worldSpaceRotation=true) const;

				NpTriangleMesh*		getTriangleMesh();//TODO: make this inline again if possible after refactor
				NvTriangleMesh*		getNvTriangleMesh();
				const NvTriangleMesh*getNvTriangleMesh() const;
				void				setTriangleMesh(NvTriangleMesh*);

	NX_INLINE	const TriangleMesh&	getTriangleMeshFast()	const	{ return *triangleMesh;	}
	NX_INLINE	TriangleMesh&		getTriangleMeshFast()			{ return *triangleMesh;	}
	NX_INLINE	void				setTriangleMeshFast(TriangleMesh* tm)	{ triangleMesh = tm;	}

	NX_INLINE	NxU32				getMeshFlags()			const	{ return meshFlags;		}
	NX_INLINE	void				setMeshFlags(NxU32 f);
	NX_INLINE	NxMeshPagingMode	getMeshPagingMode()		const	{ return pagingMode;	}
	NX_INLINE	void				setMeshPagingMode(NxMeshPagingMode f)		{ pagingMode = f;		}
#ifdef NX_SUPPORT_MESH_SCALE
	NX_INLINE	NxReal				getScale()				const	{ return scale;			}
	NX_INLINE	void				setScale(NxReal	s)				{ scale = s;			}
#endif

	NX_INLINE	void				vertex2World(const NxPoint& localVertex, NxVec3& worldVertex) const
									{
									const NxMat34 &absPose = getAbsPoseFast();
									worldVertex = absPose.t + absPose.M * localVertex;
									}

	private:
				bool				overlapAABBTriangles(const NxBounds3 bounds, NxU32 flags, NxU32& nb, const NxU32*& indices, AABBQuery&, Opcode::AABBCache&)	const;

				TriangleMesh*		triangleMesh;	//!< Actual triangle mesh data
				NxU32				meshFlags;		//!< Combination of NxTriangleMeshShape::MeshFlag(s)
				NxMeshPagingMode	pagingMode;		//!< Mesh paging mode
#ifdef NX_SUPPORT_MESH_SCALE
				NxReal				scale;			//!< Scale factor for this instance TODO: non-uniform scale ?
#endif
	// Internal methods
	virtual		void				computeWorldBounds(NxBounds3&)	const;
	virtual		void				computeWorldSphere(NxSphere&)	const;
	virtual		void				computeLocalSphere(NxSphere&)	const;

	class PageData
	{
	public:
		PxdShape			bodyShape;
		PageBoundsElement*	boundsElement;
		NxI32				refCount;
		bool				unmapValid;
		bool				mapped;			// When a page is about to be unmapped, the bodyShape is ok, but the page should not be valid to bind by PBIs
	};

				NxFoundation::NxArraySDK<PageData>	pages;
				bool				needUnmapping;
				bool				pagesValid;					// Pages are created
	public:
	virtual		void				updateBroadPhase();
	virtual		void				updateLowLevelShapeTransform();
				void				updatePageTransforms(const NxQP& qp);
	NX_INLINE	PxdShape			getPageBodyShape(NxU32 pageIndex) const { return pages[pageIndex].bodyShape; }
				NxBounds3			getPageWorldBounds(NxU32 pageIndex) const;
	NX_INLINE	PageBoundsElement*	getPageBoundsElement(NxU32 pageIndex) const { return pages[pageIndex].boundsElement; }
				
	NX_INLINE	NxU32				getPageCount() const { return pages.size(); }

	virtual		bool				mapPageInstance(NxU32 pageIndex);
	virtual		void				unmapPageInstance(NxU32 pageIndex);
	virtual		bool				isPageInstanceMapped(NxU32 pageIndex) const;
				bool				mapPageInstanceFast(NxU32 pageIndex);
				void				unmapPageInstanceFast(NxU32 pageIndex);
	NX_INLINE	bool				isPageInstanceMappedFast(NxU32 pageIndex) const;
	NX_INLINE	void				incrementPageReference(NxU32 pageIndex, bool userRef = false);
	NX_INLINE	void				decrementPageReference(NxU32 pageIndex, bool userRef = false);
				void				unmapPages();
				void				recreatePages();
				void				destroyPages();
				void				onTriggerChange(bool on);
	NX_INLINE	void				onBroadPhaseInsert() { recreatePages(); }

	
	virtual		void				release();

	friend class Scene;	// For factory function
	};
//	}

////////////////////////////////////////////////////////////////////////////
NX_INLINE bool TriangleMeshShape::isPageInstanceMappedFast(NxU32 pageIndex) const
{
	NX_ASSERT(pageIndex<pages.size());
	return pages[pageIndex].mapped;
}

NX_INLINE void TriangleMeshShape::incrementPageReference(NxU32 pageIndex, bool userRef)
{
	// This is safe to call even between broadphase and solver
	NX_ASSERT(pageIndex<pages.size());
	
	if (!userRef)
		++pages[pageIndex].refCount;
	else
		pages[pageIndex].refCount |= (1<<30);

	if ((pagingMode == NX_MESH_PAGING_AUTO) && pages[pageIndex].refCount>0 && !isPageInstanceMappedFast(pageIndex)) // Auto
		mapPageInstanceFast(pageIndex);

	// The user reference failed to map the page. The ref count should be voided.
	if (userRef && !isPageInstanceMappedFast(pageIndex))
		pages[pageIndex].refCount &= ~(1<<30);
}

NX_INLINE void TriangleMeshShape::decrementPageReference(NxU32 pageIndex, bool userRef)
{
	// This is safe to call even between broadphase and solver
	NX_ASSERT(pageIndex<pages.size());

	if (!userRef)
		--pages[pageIndex].refCount;
	else
		pages[pageIndex].refCount &= ~(1<<30);

	if ((pagingMode == NX_MESH_PAGING_AUTO) && pages[pageIndex].refCount==0) // Auto
	{
		if (!needUnmapping)
		{
			// We need to make sure that unmapping of pages is done at a safe place
			pages[pageIndex].unmapValid = true;
			getOwnerSceneFast()->addToPageUnmapList(this);
			needUnmapping = true;
		}
	}
}

NX_INLINE void TriangleMeshShape::setMeshFlags(NxU32 f)			
{ 
	meshFlags = f;
}
////////////////////////////////////////////////////////////////////////////

#undef INC_SCENE_STAT
#undef DEC_SCENE_STAT
#undef ADD_TO_SCENE_STATS

#endif
