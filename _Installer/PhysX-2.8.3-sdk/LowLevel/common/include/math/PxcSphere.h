#ifndef PXC_SPHERE_H
#define PXC_SPHERE_H

#include "px_config.h"
#include "PxcVector.h"
#include "PxdVector.h"

/*!
Sphere geometry
*/
class PxcSphere
{
public:
	//! Default constructor
	PXD_INLINE PxcSphere()
	{}

	//! Construct from center and radius
	PXD_INLINE PxcSphere(const PxcVector& center, PxFloat radius)
		: center(center), radius(radius)
	{}

	//! Construct from center and radius
	PXD_INLINE PxcSphere(const PxdVector& center, PxFloat radius)
		: center(center), radius(radius)
	{}

	//! Construct from three scalars (for center) and radius
	PXD_INLINE PxcSphere(PxFloat x, PxFloat y, PxFloat z, PxFloat radius)
		: center(x, y, z), radius(radius)
	{}

	//! Construct from float[3] (for center) and radius
	PXD_INLINE PxcVector(PxFloat center[], PxFloat radius)
		: center(center), radius(radius)
	{}

	//! Copy constructor
	PXD_INLINE PxcSphere(const PxcSphere& other)
		: center(other.center), radius(other.radius)
	{}

	//! Assignment operator
	PXD_INLINE PxcSphere& operator=(const PxcSphere& other)
	{
		center=other.center; 
		radius=other.radius; 
		return *this;
	}

	//! Equality operator
	PXD_INLINE bool operator==(const PxcSphere& other) const
	{
		return (center==other.center && radius==other.radius);
	}

	//! Inequality operator
	PXD_INLINE bool operator!=(const PxcSphere& other) const
	{
		return (center!=other.center || radius!=other.radius);
	}

	// Data
#pragma PXD_PUSH_PACK(1)
	PxcVector	center;
	PxFloat		radius;
#pragma PXD_POP_PACK
};

#endif
