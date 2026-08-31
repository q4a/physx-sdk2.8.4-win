#ifndef PXC_BOX_H
#define PXC_BOX_H

#include "px_config.h"
#include "PxcVector.h"
#include "PxcMatrix33.h"
#include "PxcMatrix34.h"

/**
Box geometry

The base member describes the world space orientation of the box.
The origin member gives the world space position of the box.
The extent gives the local space coordinates of the box corner in the positive octant.
Dimensions of the box are: 2*extent.
Transformation to world space is: worldPoint = base * localPoint + origin
Transformation to local space is: localPoint = T(base) * (worldPoint - origin)
Where T(M) denotes the transpose of M.
*/
class PxcBox
{
public:
	//! Default constructor
	PXD_INLINE PxcBox()
	{}

	//! Construct from center, extent and rotation
	PXD_INLINE PxcBox(const PxcVector& origin, const PxcVector& extent, const PxcMatrix33& base)
		: base(base), origin(origin), extent(extent)
	{}

	//! construct from a matrix(center and rotation) + extent
	PXD_INLINE PxcBox(const PxcMatrix34& mat, const PxcVector& extent)
		: base(PxcMatrix33(mat.base0, mat.base1, mat.base2)), origin(mat.base3), extent(extent)
	{}

	//! Copy constructor
	PXD_INLINE PxcBox(const PxcBox& other)
		: base(other.base), origin(other.origin), extent(other.extent)
	{}

	//! Assignment operator
	PXD_INLINE const PxcBox& operator=(const PxcBox& other)
	{
		base=other.base;
		origin=other.origin;
		extent=other.extent;

		return *this;
	}

	//! Equality operator
	PXD_INLINE bool operator==(const PxcBox& other) const
	{
		return (extent==other.extent && origin==other.origin && base==other.base);
	}

	//! Inequality operator
	PXD_INLINE bool operator!=(const PxcBox& other) const
	{
		return (extent!=other.extent || origin!=other.origin || base!=other.base);
	}

	/**
	Return the square of the minimum distance from the surface of the box to the given point.
	\param point The query point
	\param closest Set to coordinates of the closest point in local space
	*/
	PXD_INLINE PxFloat distanceToPointSquared(const PxcVector& point, PxcVector* closest=NULL) const
	{
		PxFloat dd = 0.0f;
		PxcVector local = transformPositionToLocal(point);
		int ax;
		for (ax=0; ax<3; ax++) 
		{
			if(local[ax] < -extent[ax])
			{
				PxFloat d = local[ax] + extent[ax];
				dd += d*d;
				local[ax] = -extent[ax];
			}
			else if(local[ax] > extent[ax])
			{
				float d = local[ax] - extent[ax];
				dd += d*d;
				local[ax] = extent[ax];
			}
		}
		if(closest)	*closest = local;
		return dd;
	}
	
	//! Return the minimum distance from the surface of the box to the given point.
	PxFloat distanceToPoint(const PxcVector& point, PxcVector* closest=NULL) const
	{
		// I am not sure who would call this but why not
		return PxcSqrt(distanceToPointSquared(point, closest));
	}
	
	//! Return the position vector transformed to the box local space
	PXD_INLINE PxcVector transformPositionToLocal(const PxcVector& position) const
	{
		return transformDirectionToLocal(position-origin);
	}

	//! Return the direction vector transformed to the box local space
	PXD_INLINE PxcVector transformDirectionToLocal(const PxcVector& direction) const
	{
		PxcVector vec;
		vec.x = direction.dot(base.base0);
		vec.y = direction.dot(base.base1);
		vec.z = direction.dot(base.base2);
		return vec;
		//return PxcVector(direction.dot(base.base0), direction.dot(base.base1), direction.dot(base.base2));
	}

	PXD_INLINE PxcVector computeAABBExtent() const
	{
		PxcVector aabbExtent; //(0,0,0);
		aabbExtent.x = 0;
		aabbExtent.y = 0;
		aabbExtent.z = 0;
		PxU32 i, j;
		for(i=0; i<3; i++)
			for (j=0; j<3; j++)
				aabbExtent[j] += PxcAbs(base[i][j]) * extent[i];
		return aabbExtent;
	}
	
	// Data

#pragma PXD_PUSH_PACK(1)
	PxcMatrix33	base;
	PxcVector	origin;
	PxcVector	extent;
#pragma PXD_POP_PACK
};

#endif
