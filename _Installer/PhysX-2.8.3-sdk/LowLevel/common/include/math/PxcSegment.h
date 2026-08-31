#ifndef PXC_SEGMENT_H
#define PXC_SEGMENT_H

#include "px_config.h"
#include "PxcVector.h"

/*!
Line segment geometry
In some cases this structure will be used to represent the infinite line that passes point0 and point1.
*/
class PxcSegment
{
public:
	//! Default constructor
	PXD_INLINE PxcSegment()
	{}

	//! Construct from origin and direction
	PXD_INLINE PxcSegment(const PxcVector& point0, const PxcVector& point1)
		: point0(point0), point1(point1)
	{}

	//! Copy constructor
	PXD_INLINE PxcSegment(const PxcSegment& other)
		: point0(other.point0), point1(other.point1)
	{}

	//! Assignment operator
	PXD_INLINE PxcSegment& operator=(const PxcSegment& other)
	{
		point0=other.point0; 
		point1=other.point1; 
		
		return *this;
	}

	//! Equality operator
	PXD_INLINE bool operator==(const PxcSegment& other) const
	{
		return (point0==other.point0 && point1==other.point1);
	}

	//! Inequality operator
	PXD_INLINE bool operator!=(const PxcSegment& other) const
	{
		return (point0!=other.point0 || point1!=other.point1);
	}

	//! Return the vector from point0 to point1
	PXD_INLINE PxcVector direction() const
	{
		return (point1-point0);
	}

	//! Return the square of the length of vector from point0 to point1
	PXD_INLINE PxFloat lengthSquared() const
	{
		return ((point1-point0).magnitudeSquared());
	}

	//! Return the length of vector from point0 to point1
	PXD_INLINE PxFloat length() const
	{
		return ((point1-point0).magnitude());
	}

	//! Return the point at parameter t along the line: point0 + t*(point1-point0)
	PXD_INLINE PxcVector getPointAt(PxFloat t) const
	{
		return (point1-point0)*t + point0;
	}

	// Data
#pragma PXD_PUSH_PACK(1)
	PxcVector	point0;
	PxcVector	point1;
#pragma PXD_POP_PACK

};

#endif
