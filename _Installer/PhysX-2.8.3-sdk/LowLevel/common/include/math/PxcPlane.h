#ifndef PXC_PLANE_H
#define PXC_PLANE_H

#include "px_config.h"
#include "PxcVector.h"
#include "PxcMatrix34.h"

/*!
Plane geometry
*/
class PxcPlane
{
public:
	//! Default constructor
	CUDA_CALLABLE_MEMBER PXD_INLINE PxcPlane()
	{}

	//! Construct from normal and d
	CUDA_CALLABLE_MEMBER PXD_INLINE PxcPlane(const PxcVector& normal, PxFloat d)
		: normal(normal), d(d)
	{}

	//! Construct from three scalars (for normal) and d
	CUDA_CALLABLE_MEMBER PXD_INLINE PxcPlane(PxFloat x, PxFloat y, PxFloat z, PxFloat d)
		: /*normal(x, y, z),*/ d(d)
	{
		normal.set(x, y, z);
	}

	//! Construct from float[3] (for normal) and a float for d
	CUDA_CALLABLE_MEMBER PXD_INLINE PxcPlane(const PxFloat* normalOther, PxFloat d)
		: /*normal(normal),*/ d(d)
	{
		normal.set(normalOther);
	}

	//! Construct from three points
	CUDA_CALLABLE_MEMBER PXD_INLINE PxcPlane(const PxcVector& p0, const PxcVector& p1, const PxcVector& p2)
	{
		PxcVector e0 = p1 - p0;
		PxcVector e1 = p2 - p0;
		normal = e0.cross(e1).getNormalized();

		d = -normal.dot(p0);
	}

	//! Construct from point unit normal
	CUDA_CALLABLE_MEMBER PXD_INLINE PxcPlane(const PxcVector& normal, const PxcVector& point) : normal(normal)
	{
		d = -normal.dot(point);
	}

	//! Construct from float[4] (normal x .. y .. z, d)
	explicit CUDA_CALLABLE_MEMBER PXD_INLINE PxcPlane(const PxFloat* values)
		: /*normal(values),*/ d(values[3])
	{
		normal.set(values);
	}

	//! Equality operator
	PXD_INLINE bool operator==(const PxcPlane& other) const
	{
		// this is not true but the real deal is involved
		return (normal==other.normal && d==other.d);
	}

	//! Inequality operator
	PXD_INLINE bool operator!=(const PxcPlane& other) const
	{
		// this is not true but the real deal is involved
		return (normal!=other.normal || d!=other.d);
	}

	PXD_FORCE_INLINE void operator=(const PxcPlane& other)
	{
		normal = other.normal;
		d = other.d;
	}

	PXD_INLINE PxFloat signedDistanceHessianNormalForm(const PxcVector& point) const
	{
		return normal.dot(point) + d;
	}

	PXD_INLINE PxFloat signedDistance(const PxcVector& point) const
	{
		PxFloat lenSq = normal.magnitudeSquared();
		if (PxcAbs(lenSq) > PXD_FLOAT_EPSILON)
		{
			if (PxcAbs(lenSq - 1.0f) <= PXD_FLOAT_EPSILON) 
			{
				return signedDistanceHessianNormalForm(point);
			}
			return signedDistanceHessianNormalForm(point) / PxcSqrt(lenSq);
		}
		return PXD_FLOAT_MAX; // a no-win situation
	}

	PXD_INLINE PxFloat distance(const PxcVector& point) const
	{
		return PxcAbs(signedDistance(point));
	}

	PXD_INLINE PxcVector projectPoint(const PxcVector& point) const
	{
		PxcVector p = point;
		PxFloat dist = signedDistanceHessianNormalForm(p);
		p = p - (normal * dist);
		return p;
	}

	PXD_INLINE PxcVector projectVector(const PxcVector& vector) const
	{
		PxcVector v = vector;
		PxFloat dist = normal.dot(v);
		v = v - (normal * dist);
		return v;
	}

	PXD_INLINE PxcPlane getTransformed(const PxcMatrix34& transform) const
	{
		PxcVector newN = transform.rotate(normal);
		return PxcPlane(newN, d - transform.base3.dot(newN));
	}

	PXD_INLINE PxcPlane getTransformed(const PxcTransform& transform) const
	{
		PxcVector newN = transform.rotate(normal);
		return PxcPlane(newN, d - transform.translation.dot(newN));
	}

	// Data
	// Coding convention on members broken because they are public.
#pragma PXD_PUSH_PACK(1)
	PxcVector	normal;
	PxFloat		d;
#pragma PXD_POP_PACK

};

#endif
