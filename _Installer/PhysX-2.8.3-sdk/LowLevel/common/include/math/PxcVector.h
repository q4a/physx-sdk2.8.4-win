#ifndef PXC_VECTOR_H
#define PXC_VECTOR_H


#include <string.h>

#include "px_config.h"
#include "PxcMath.h"

#ifdef PXD_PLATFORM_WII
#include "wii\PxcVector_wii.h"
#elif defined(PXD_PLATFORM_WIIU)
#include "wiiu\PxcVector_wiiu.h"
#else

/*!
Simple R3 vector with basic operations

Do not define and use any Constructors, the c++ standard does not allow it in unions:

11A union member shall not be of a class type (or  array  thereof)  that
  has a non-trivial constructor.

(this class is used in unions in the SolverConstraints and elsewhere)

dsequeira: this is just a temporary fix - the right thing is to take the classes with
constructors out of the unions, _not_ to take the constructors out of the classes - 
several important idioms (i.e. initializer lists and RVO) require constructors to work.)

*/

class PxcVector
{

#if defined (PXD_COMPILER_VC)

	friend class PxcQuaternion;

public:

	//! Construct from a single value
	explicit CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector(PxFloat value)
		: x(value), y(value), z(value)
	{}

	////! Construct from three scalars
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector(PxFloat x, PxFloat y, PxFloat z)
		: x(x), y(y), z(z)
	{}

	////! Construct from PxdVector
	explicit CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector(const PxdVector& other)
		: x(other.x), y(other.y), z(other.z)
	{}

	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector(const PxcVector& other)
		: x(other.x), y(other.y), z(other.z)
	{}
	//! Default constructor
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector()
	{}

private:
	////! Construct from float[3]
	explicit CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector(const PxFloat* values)
		: x(values[0]), y(values[1]), z(values[2])
	{}



#endif

// NOTE: no constructors for gcc

public:

	// dsequeira: added factory methods to replace the constructors (temporary!!)

	static PxcVector create(PxFloat x, PxFloat y, PxFloat z)
	{
		PxcVector v;
		v.set(x,y,z);
		return v;
	}

	//! Construct from a single value
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void set(PxFloat value)
	{
		x = y = z = value;
	}

	/* Cast-operator, there are very evil and rely on the fact that
	PxcVector & PxdVector have same memory layout
	*/
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE operator PxdVector const& () const
	{
		return *reinterpret_cast<const PxdVector*>(&x);
	}

	//Operators
	//! Equality operator
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool operator==(const PxcVector& other) const
	{
		return (x==other.x && y==other.y && z==other.z);
	}

	//! Inequality operator
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool operator!=(const PxcVector& other) const
	{
		return (x!=other.x || y!=other.y || z!=other.z);
	}

	//! Smaller than, all components
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool operator<(const PxcVector& other) const
	{
		return (x<other.x && y<other.y && z<other.z);
	}
	
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool operator<=(const PxcVector& other) const
	{
		return (x<=other.x && y<=other.y && z<=other.z);
	}

	//! Bigger than, all components
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool operator>(const PxcVector& other) const
	{
		return (x>other.x && y>other.y && z>other.z);
	}

	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool operator>=(const PxcVector& other) const
	{
		return (x>=other.x && y>=other.y && z>=other.z);
	}

#if defined (PXD_COMPILER_VC)

	//! Return negative vector
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator-() const
	{
		return PxcVector(-x, -y, -z);
	}


	// c = a<op>b operators

	//! Add
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator+(const PxcVector& other) const
	{
		return PxcVector(x+other.x, y+other.y, z+other.z);
	}

	//! Subtract
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator-(const PxcVector& other) const
	{
		return PxcVector(x-other.x, y-other.y, z-other.z);
	}

	//! Scalar multiplication
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator*(PxFloat scalar) const
	{
		return PxcVector(x*scalar, y*scalar, z*scalar);
	}

	friend PxcVector operator*(PxFloat, const PxcVector&);

	//! Scalar division
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator/(PxFloat scalar) const
	{
		PxFloat oneOverScalar = 1.0f / scalar;
		return PxcVector(x*oneOverScalar, y*oneOverScalar, z*oneOverScalar);
	}

#elif defined (PXD_COMPILER_GNUC)

	//! Return negative vector
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator-() const
	{
		PxcVector retvec;
		retvec.x = -x;
		retvec.y = -y;
		retvec.z = -z;
		return retvec;
	}

	// c = a<op>b operators

	//! Add
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator+(const PxcVector& other) const
	{
		PxcVector temp;
		temp.set(x+other.x, y+other.y, z+other.z);
		return temp;
	}

	//! Subtract
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator-(const PxcVector& other) const
	{
		PxcVector temp;
		temp.set(x-other.x, y-other.y, z-other.z);
		return temp;
	}

	//! Scalar multiplication
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator*(PxFloat scalar) const
	{
		PxcVector temp;
		temp.set(x*scalar, y*scalar, z*scalar);
		return temp;
	}

	friend PxcVector operator*(PxFloat, const PxcVector&);

	//! Scalar division
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator/(PxFloat scalar) const
	{
		PxFloat oneOverScalar = 1.0f / scalar;

		PxcVector temp;
		temp.set(x*oneOverScalar, y*oneOverScalar, z*oneOverScalar);
		return temp;
	}


#endif

	// a <op>= b operators

	//! Equals-add
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void operator+=(const PxcVector& other)
	{
		x+=other.x; y+=other.y; z+=other.z;
	}

	//! Equals-sub
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void operator-=(const PxcVector& other)
	{
		x-=other.x; y-=other.y; z-=other.z;
	}

	//! Equals scalar multiplication
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void operator*=(PxFloat scalar)
	{
		x*=scalar; y*=scalar; z*=scalar;
	}

	//! Element access
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat operator[](PxUInt index) const
	{
		return (&x)[index];
	}

	//! Element access
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat& operator[](PxUInt index)
	{
		return (&x)[index];
	}

	//Vector operations

	//! Dot operation
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat dot(const PxcVector& other) const
	{
		return x*other.x + y*other.y + z*other.z;
	}

#if defined (PXD_COMPILER_VC) || defined(PXD_COMPILER_CW)

	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector multiplyElts(const PxcVector& other) const
	{
		return PxcVector(x*other.x, y*other.y, z*other.z);
	}

	//! Cross product
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector cross(const PxcVector& other) const
	{
		return PxcVector(y*other.z - z*other.y,
						 z*other.x - x*other.z,
						 x*other.y - y*other.x);
	}

	//! Get a normalized vector parallel to this
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector getNormalized() const
	{
		PxFloat ll = magnitudeSquared();
		if (ll > 0.0f) 
		{
			PxFloat s = 1.0f / PxcSqrt(ll);
			return PxcVector(x*s, y*s, z*s);
		}
		return PxcVector(0.0f);
	}

	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void operator=(const PxcVector& other)
	{
		x = other.x; y = other.y; z = other.z;
	}

#elif defined (PXD_COMPILER_GNUC)

	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector multiplyElts(const PxcVector& other) const
	{
		PxcVector temp;
		temp.set(x*other.x, y*other.y, z*other.z);
		return temp;
	}

	//! Cross product
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector cross(const PxcVector& other) const
	{
		PxcVector temp;
		temp.set(y*other.z - z*other.y, z*other.x - x*other.z, x*other.y - y*other.x);
		return temp;
	}

	//! Get a normalized vector parallel to this
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector getNormalized() const
	{
		PxFloat ll = magnitudeSquared();
		if (ll > 0.0f) 
		{
			PxFloat s = 1.0f / PxcSqrt(ll);
			PxcVector temp;
			temp.set(x*s, y*s, z*s);
			return temp;
		}
		PxcVector temp;
		temp.set(0.0f);
		return temp;
	}

#endif

	//! Normalizes the vector and returns the original length
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat normalize()
	{
		PxFloat ll = magnitudeSquared();
		if (ll > 0.0f) 
		{
			PxFloat l = PxcSqrt(ll);
			PxFloat s = 1.0f / l;
			x*=s; y*=s; z*=s;
			return l;
		}
		x = 1.0f; y=z=0.0f;
		return 1.0f;
	}

	//! Square of the distance to another point
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat getSquaredDistanceTo(const PxcVector& other) const
	{
		PxFloat acc = 0;
		PxFloat delta;
		delta = x-other.x;
		acc += delta*delta;
		delta = y-other.y;
		acc += delta*delta;
		delta = z-other.z;
		acc += delta*delta;
		return acc;
	}

	//! (Absolute) distance to another point
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat getDistanceTo(const PxcVector& other) const
	{
		return PxcSqrt(getSquaredDistanceTo(other));
	}

	//! Magnitude (length)
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat magnitude() const
	{
		//TODO Add real sqrt
		return PxcSqrt(magnitudeSquared());
	}

	//! Squared magnitude (length)
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat magnitudeSquared() const
	{
		return x*x + y*y + z*z;
	}
#if defined (PXD_COMPILER_VC)
     //! Return vector with element wise min(this,other)
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector minElts(const PxcVector& other) const
	{
	    return  PxcVector( PxcMin(other.x, this->x), PxcMin(other.y, this->y), PxcMin(other.z, this->z) );
	}
	  
	 //! Return vector with element wise max(this,other)
	 CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector maxElts(const PxcVector& other) const
	 {
	 	return PxcVector( PxcMax(other.x, this->x), PxcMax(other.y, this->y), PxcMax(other.z, this->z) );
	 }

#elif defined ( PXD_COMPILER_GNUC )
	//! Return vector with element wise min(this,other)
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector minElts(const PxcVector& other) const
	{
		PxcVector	v;

		v.set( PxcMin(other.x, this->x), PxcMin(other.y, this->y), PxcMin(other.z, this->z) );
		return v;
	}

	//! Return vector with element wise max(this,other)
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector maxElts(const PxcVector& other) const
	{
		PxcVector	v;
		
		v.set( PxcMax(other.x, this->x), PxcMax(other.y, this->y), PxcMax(other.z, this->z) );
		return v;
	}
#endif
	//! Set vector to 0 <0,0,0>
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void setZero()
	{
		x = y = z = 0.0f;
	}

	//! Return true if all components are zero
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool isZero() const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	// PxdVector interface
	//! Set a PxdVector to the content of this vector
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void get(PxdVector& other) const
	{
		other.x = x; other.y = y; other.z = z;
	}

	//! Fill this vector from a PxdVector
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void set(const PxdVector& other)
	{
		x = other.x; y = other.y; z = other.z;
	}

	//! Set the components of this vector from 3 floats
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void set(PxFloat x, PxFloat y, PxFloat z)
	{
		this->x = x; this->y = y; this->z = z;
	}

	//! Set the components of this vector from float array
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void set(const PxFloat* values)
	{
		this->x = values[0]; this->y = values[1]; this->z = values[2];
	}
	
	PXD_FORCE_INLINE bool isFinite() const 
	{
		return (PxcIsFinite(x) && PxcIsFinite(y) && PxcIsFinite(z));
	}

#ifndef __SPU__
#pragma PXD_PUSH_PACK(1)
#endif
	PxFloat x,y,z;

#ifndef __SPU__
#pragma PXD_POP_PACK
#endif
};



// Free operations on above..
// scalar*vector

#if defined (PXD_COMPILER_VC)

CUDA_CALLABLE PXD_FORCE_INLINE PxcVector operator*(PxFloat scalar, const PxcVector& v)
{
	return PxcVector(v.x*scalar, v.y*scalar, v.z*scalar);
}

#elif defined (PXD_COMPILER_GNUC)

CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator*(PxFloat scalar, const PxcVector& v)
{
	PxcVector temp;
	temp.set(v.x*scalar, v.y*scalar, v.z*scalar);
	return temp;
}

#endif

/*!
Combination of two R3 vectors.

*/
class PxcSpatialVector
{
public:
	//! Default constructor
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcSpatialVector()
	{}

	//! Construct from PxdSpatialVector
	explicit CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcSpatialVector(const PxdSpatialVector& other)
	{
		linear.set(other.linear);
		angular.set(other.angular);
	}

	//! Construct from two PxcVectors
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcSpatialVector(const PxcVector& linear, const PxcVector& angular)
		: linear(linear), angular(angular)
	{
	}

	//! Construct from two PxdVectors
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcSpatialVector(const PxdVector& linearOther, const PxdVector& angularOther)
	{
		linear.set(linearOther);
		angular.set(angularOther);
	}

	//! Copy constructor
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcSpatialVector(const PxcSpatialVector& other)
		: linear(other.linear), angular(other.angular)
	{}

	//! Assignment operator
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcSpatialVector& operator=(const PxcSpatialVector& other)
	{
		linear = other.linear;
		angular = other.angular;
		return *this;
	}

	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE ~PxcSpatialVector()
	{}


	/* Cast-operator, there are very evil and rely on the fact that
	PxcSpatialVector & PxdSpatialVector have same memory layout
	*/
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE operator PxdSpatialVector const& () const
	{
		return *reinterpret_cast<const PxdSpatialVector*>(&linear.x);
	}

	// PxdVector interface
	//! Set a PxdVector to the content of this vector
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void get(PxdSpatialVector& other) const
	{
		linear.get(other.linear);
		angular.get(other.angular);
	}

	//! Fill this vector from a PxdVector
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void set(const PxdSpatialVector& other)
	{
		linear.set(other.linear);
		angular.set(other.angular);
	}

	//! Construct from two PxcVectors
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void set(const PxcVector& linearOther, const PxcVector& angularOther)
	{
		linear = linearOther;
		angular = angularOther;
	}

	//Ensure same data-layout as PxdSpatialVector
#ifndef __SPU__
#pragma PXD_PUSH_PACK(1)
#endif
	// Data
	PxcVector linear;
	PxcVector angular;
#ifndef __SPU__
#pragma PXD_POP_PACK
#endif
};

#endif
#endif
