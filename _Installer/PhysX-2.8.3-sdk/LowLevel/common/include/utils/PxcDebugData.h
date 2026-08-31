#ifndef PXC_DEBUG_DATA_H
#define PXC_DEBUG_DATA_H

#include "px_config.h"
#include "px_context.h"
#include "PxcArray.h"
#include "PxcAabb.h"

class PxcVector;

#ifdef PXD_SUPPORT_DEBUG_RENDERER

class PxcDebugData
{
public:
	PXD_INLINE PxU32					getNbPoints()		const	{ return mPointsArray.getSize();	}
	PXD_INLINE const PxDebugPoint*		getPoints()			const	{ return mPointsArray.getPtr();		}

	PXD_INLINE PxU32					getNbLines()		const	{ return mLinesArray.getSize();		}
	PXD_INLINE const PxDebugLine*		getLines()			const	{ return mLinesArray.getPtr();		}

	PXD_INLINE PxU32					getNbTriangles()	const	{ return mTrianglesArray.getSize();	}
	PXD_INLINE const PxDebugTriangle*	getTriangles()		const	{ return mTrianglesArray.getPtr();	}

	PXD_INLINE PxU32					getNbAABBs()		const	{ return mAABBsArray.getSize();		}
	PXD_INLINE const PxDebugAABB*		getAABBs()			const	{ return mAABBsArray.getPtr();		}

	PXD_INLINE void addPoint(const PxcVector& p, PxU32 color)
	{
		PxDebugPoint tmp;
		tmp.p		= (PxdVector&)p;
		tmp.color	= color;
		mPointsArray.insert(tmp);
	}

	PXD_INLINE void	addLine(const PxcVector& p0, const PxcVector& p1, PxU32 color)
	{
		PxDebugLine tmp;
		tmp.p0		= (PxdVector&)p0;
		tmp.p1		= (PxdVector&)p1;
		tmp.color	= color;
		mLinesArray.insert(tmp);
	}

	PXD_INLINE void	addTriangle(const PxcVector& p0, const PxcVector& p1, const PxcVector& p2, PxU32 color)
	{
		PxDebugTriangle tmp;
		tmp.p0		= (PxdVector&)p0;
		tmp.p1		= (PxdVector&)p1;
		tmp.p2		= (PxdVector&)p2;
		tmp.color	= color;
		mTrianglesArray.insert(tmp);
	}

	PXD_INLINE void	addAABB(const PxcAabb& aabb, PxU32 color)
	{
		PxDebugAABB tmp;
		tmp.min		= (PxdVector&)aabb.min;
		tmp.max		= (PxdVector&)aabb.max;
		tmp.color	= color;
		mAABBsArray.insert(tmp);
	}

	// todo?:
	// addOBB
	// addArrow
	// addBasis
	// addCircle

	PXD_INLINE void clear()
	{
		mPointsArray.clear();
		mLinesArray.clear();
		mTrianglesArray.clear();
		mAABBsArray.clear();
	}

private:
	PxcArray<PxDebugPoint>		mPointsArray;
	PxcArray<PxDebugLine>		mLinesArray;
	PxcArray<PxDebugTriangle>	mTrianglesArray;
	PxcArray<PxDebugAABB>		mAABBsArray;
};

#endif

#endif