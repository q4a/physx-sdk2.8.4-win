#ifndef PXC_AABB_H
#define PXC_AABB_H

#include "PxnError.h"
#include "PxcVector.h"
#include "PxcMatrix33.h"


/*!
AABB with basics operations
*/
class PxcAabb
{
public:

	CUDA_CALLABLE_MEMBER PXD_INLINE PxcAabb();

	//! Construct from min, max
	CUDA_CALLABLE_MEMBER PXD_INLINE PxcAabb(const PxcVector& min, const PxcVector& max);


	CUDA_CALLABLE_MEMBER PXD_INLINE ~PxcAabb();

	/**
	\brief Sets empty to true
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void setEmpty();

	/**
	\brief Sets infinite bounds
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void setInfinite();
	
	/**
	\brief low level assignment.

	\param minx Minimum X value
	\param miny Minimum Y value
	\param minz Minimum Z value
	\param maxx Maximum X value
	\param maxy Maximum Y value
	\param maxz Maximum Z value
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void set(PxFloat minx, PxFloat miny, PxFloat minz, PxFloat maxx, PxFloat maxy,PxFloat maxz);

	/**
	\brief vector assignment.

	\param min Minimum point of bounds.
	\param max Maximum point of bounds.
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void set(const PxcVector& min, const PxcVector& max);

	/**
	\brief expands the volume to include v

	\param v Point to expand to.
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void include(const PxcVector& v);

	/**
	\brief sets this to the union of this and b2.

	\param b2 Bounds to perform union with.
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void combine(const PxcAabb& b2);

	/**
	\brief sets this to the AABB of the OBB passed.

	\param orientation Orientation of the OBB.
	\param translation Translation of the OBB.
	\param halfDims radii of the OBB.
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void boundsOfOBB(const PxcMatrix33& orientation, const PxcVector& translation, const PxcVector& halfDims);

	/**
	\brief transforms this volume as if it was an axis aligned bounding box, and then assigns the results' bounds to this.

	\param orientation Orientation to apply.
	\param translation Translation to apply(applied after orientation transform)
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void transform(const PxcMatrix33& orientation, const PxcVector& translation);
	
	CUDA_CALLABLE_MEMBER PXD_INLINE bool isEmpty() const;

	/**
	\brief indicates whether the intersection of this and b is empty or not.

	\param b Bounds to test for intersection.
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE bool intersects(const PxcAabb& b) const;

	/**
	\brief indicates whether the intersection of this and b is empty or not in the plane orthogonal to the axis passed (X = 0, Y = 1 or Z = 2).

	\param b Bounds to test for intersection.
	\param axisToIgnore Axis to ignore when performing the intersection test.
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE bool intersects2D(const PxcAabb& b, unsigned axisToIgnore) const;

	/**
	\brief indicates if these bounds contain v.

	\param v Point to test against bounds.
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE bool contain(const PxcVector& v) const;

	/**
	\brief returns the center of this axis aligned box.

	\param center The center of the bounds.
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void getCenter(PxcVector& center) const;

	/**
	\brief returns the dimensions (width/height/depth) of this axis aligned box.

	\param dims The dimensions of the bounds.
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void getDimensions(PxcVector& dims) const;

	/**
	\brief returns the extents, which are half of the width/height/depth.

	\param extents The extents/radii of the bounds.
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void getExtents(PxcVector& extents) const;

	/**
	\brief setups an AABB from center & extents vectors.

	\param c Center vector
	\param e Extents vector
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void setCenterExtents(const PxcVector& c, const PxcVector& e);

	/**
	\brief scales the AABB.

	\param scale Factor to scale AABB by.
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void scale(PxFloat scale);

	/** 
	fattens the AABB in all 3 dimensions by the given distance. 
	*/
	CUDA_CALLABLE_MEMBER PXD_INLINE void fatten(PxFloat distance);


	//CUDA_CALLABLE_MEMBER PXD_INLINE void combine(PxFloat extension);

	PxcVector min, max;
};


CUDA_CALLABLE_MEMBER PXD_INLINE PxcAabb::PxcAabb()
{
	// Default to empty boxes for compatibility TODO: PT: remove this if useless
	setEmpty();
}

CUDA_CALLABLE_MEMBER PXD_INLINE PxcAabb::PxcAabb(const PxcVector& min, const PxcVector& max) 
	: min(min), max(max)
{

}

CUDA_CALLABLE_MEMBER PXD_INLINE PxcAabb::~PxcAabb()
{

}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::setEmpty()
{
	// We know use this particular pattern for empty boxes
	set(PXD_FLOAT_MAX, PXD_FLOAT_MAX, PXD_FLOAT_MAX,
		-PXD_FLOAT_MAX, -PXD_FLOAT_MAX, -PXD_FLOAT_MAX);
}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::setInfinite()
{
	set(-PXD_FLOAT_MAX, -PXD_FLOAT_MAX, -PXD_FLOAT_MAX,
		PXD_FLOAT_MAX, PXD_FLOAT_MAX, PXD_FLOAT_MAX);
}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::set(PxFloat minx, PxFloat miny, PxFloat minz, PxFloat maxx, PxFloat maxy,PxFloat maxz)
{
	min.set(minx, miny, minz);
	max.set(maxx, maxy, maxz);
}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::set(const PxcVector& _min, const PxcVector& _max)
{
	min = _min;
	max = _max;
}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::include(const PxcVector& v)
{
	max = max.maxElts(v);
	min = min.minElts(v);
}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::combine(const PxcAabb& b2)
{
	// - if we're empty, min = MAX,MAX,MAX => min will be b2 in all cases => it will copy b2, ok
	// - if b2 is empty, the opposite happens => keep us unchanged => ok
	// => same behavior as before, automatically
	min = min.minElts(b2.min);
	max = max.maxElts(b2.max);
}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::boundsOfOBB(const PxcMatrix33& orientation, const PxcVector& translation, const PxcVector& halfDims)
{
	PxFloat dimx = halfDims[0];
	PxFloat dimy = halfDims[1];
	PxFloat dimz = halfDims[2];

	PxFloat x = PxcAbs(orientation(0,0) * dimx) + PxcAbs(orientation(0,1) * dimy) + PxcAbs(orientation(0,2) * dimz);
	PxFloat y = PxcAbs(orientation(1,0) * dimx) + PxcAbs(orientation(1,1) * dimy) + PxcAbs(orientation(1,2) * dimz);
	PxFloat z = PxcAbs(orientation(2,0) * dimx) + PxcAbs(orientation(2,1) * dimy) + PxcAbs(orientation(2,2) * dimz);

	set(-x + translation[0], -y + translation[1], -z + translation[2], x + translation[0], y + translation[1], z + translation[2]);
}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::transform(const PxcMatrix33& orientation, const PxcVector& translation)
{
	// convert to center and extents form
	PxcVector center, extents;
	getCenter(center);
	getExtents(extents);

	center = orientation.transform(center) + translation;
	boundsOfOBB(orientation, center, extents);
}

CUDA_CALLABLE_MEMBER PXD_INLINE bool PxcAabb::isEmpty() const
{
	// Consistency condition for (Min, Max) boxes: min < max
	// TODO: PT: should we test against the explicit pattern ?
	if(min.x < max.x)	return false;
	if(min.y < max.y)	return false;
	if(min.z < max.z)	return false;
	return true;
}

CUDA_CALLABLE_MEMBER PXD_INLINE bool PxcAabb::intersects(const PxcAabb& b) const
{
	if ((b.min.x > max.x) || (min.x > b.max.x)) return false;
	if ((b.min.y > max.y) || (min.y > b.max.y)) return false;
	if ((b.min.z > max.z) || (min.z > b.max.z)) return false;
	return true;
}

CUDA_CALLABLE_MEMBER PXD_INLINE bool PxcAabb::intersects2D(const PxcAabb& b, unsigned axis) const
{
	// TODO: PT: could be static and like this:
	// static unsigned i[3] = { 1,2,0,1 };
	// const unsigned ii = i[axis];
	// const unsigned jj = i[axis+1];
	const unsigned i[3] = { 1,0,0 };
	const unsigned j[3] = { 2,2,1 };
	const unsigned ii = i[axis];
	const unsigned jj = j[axis];
	if ((b.min[ii] > max[ii]) || (min[ii] > b.max[ii])) return false;
	if ((b.min[jj] > max[jj]) || (min[jj] > b.max[jj])) return false;
	return true;
}

CUDA_CALLABLE_MEMBER PXD_INLINE bool PxcAabb::contain(const PxcVector& v) const
{
	if ((v.x < min.x) || (v.x > max.x)) return false;
	if ((v.y < min.y) || (v.y > max.y)) return false;
	if ((v.z < min.z) || (v.z > max.z)) return false;
	return true;
}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::getCenter(PxcVector& center) const
{
	center = min + max;
	center *= PxFloat(0.5);
}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::getDimensions(PxcVector& dims) const
{
	dims = max - min;
}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::getExtents(PxcVector& extents) const
{
	extents = max - min;
	extents *= PxFloat(0.5);
}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::setCenterExtents(const PxcVector& c, const PxcVector& e)
{
	min = c - e;
	max = c + e;
}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::scale(PxFloat scale)
{
	min *= scale;
	max *= scale;
}

CUDA_CALLABLE_MEMBER PXD_INLINE void PxcAabb::fatten(PxFloat distance)
{
	min.x -= distance;
	min.y -= distance;
	min.z -= distance;

	max.x += distance;
	max.y += distance;
	max.z += distance;
}

#endif
