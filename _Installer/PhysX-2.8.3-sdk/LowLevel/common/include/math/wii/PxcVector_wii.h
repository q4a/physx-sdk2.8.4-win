#ifndef PXC_VECTOR_WII_H
#define PXC_VECTOR_WII_H

#ifdef __PPCGEKKO__
#include <string.h>

#include "px_config.h"
#include "PxcMath.h"


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

	//! Bigger than, all components
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool operator>(const PxcVector& other) const
	{
		return (x>other.x && y>other.y && z>other.z);
	}

	//! Return negative vector
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator-() const
	{
		PxcVector retvec;
		register const PxcVector* p = this;
		register PxcVector* pOut = &retvec;
		register __vec2x32float__ a, b;
#pragma optimizewithasm off		
		asm
		{
			psq_l	a, 0(p), 0, 0;
			ps_neg	b, a;
			psq_st	b, 0(pOut), 0, 0;

			psq_l	a, 8(p), 1, 0;
			ps_neg	b, a;
			psq_st	b, 8(pOut), 1, 0;
		}
#pragma optimizewithasm on		
		return retvec;
	}


	// c = a<op>b operators

	//! Add
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator+(const PxcVector& other) const
	{
		PxcVector temp;
		VECAdd(&v, &other.v, &temp.v);
		return temp;
	}

	//! Subtract
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator-(const PxcVector& other) const
	{
		PxcVector temp;
		VECSubtract(&v, &other.v, &temp.v);
		return temp;
	}

	//! Scalar multiplication
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator*(PxFloat scalar) const
	{
		PxcVector temp;
		VECScale(&v, &temp.v, scalar);
		return temp;
	}

	friend PxcVector operator*(PxFloat, const PxcVector&);

	//! Scalar division
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector operator/(PxFloat scalar) const
	{
		PxFloat oneOverScalar = 1.0f / scalar;
		PxcVector temp;
		VECScale(&v, &temp.v, oneOverScalar);
		return temp;
	}

	// a <op>= b operators

	//! Equals-add
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void operator+=(const PxcVector& other)
	{
		VECAdd(&v, &other.v, &v);
	}

	//! Equals-sub
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void operator-=(const PxcVector& other)
	{
		VECSubtract(&v, &other.v, &v);
	}

	//! Equals scalar multiplication
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void operator*=(PxFloat scalar)
	{
		VECScale(&v, &v, scalar);
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
		return VECDotProduct(&v, &other.v);
	}

	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector multiplyElts(const PxcVector& other) const
	{
		return PxcVector(x*other.x, y*other.y, z*other.z);
	}

	//! Cross product
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector cross(const PxcVector& other) const
	{
		PxcVector temp;
		VECCrossProduct(&v, &other.v, &temp.v);
		return temp;
	}

	//! Get a normalized vector parallel to this
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector getNormalized() const
	{
		PxFloat ll = magnitudeSquared();
		if (ll > 0.0f) 
		{
			PxcVector temp;
			VECNormalize(&v, &temp.v);
			return temp;
		}
		return PxcVector(0.0f, 0.0f, 0.0f);
	}

	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void operator=(const PxcVector& other)
	{
		x = other.x; y = other.y; z = other.z;
	}

	//! Normalizes the vector and returns the original length
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat normalize()
	{
		PxFloat ll = magnitude();
		if (ll > 0.0f) 
		{
			PxFloat l = ll;
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
		return VECSquareDistance(&v, &other.v);
	}

	//! (Absolute) distance to another point
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat getDistanceTo(const PxcVector& other) const
	{
		return VECDistance(&v, &other.v);
	}

	//! Magnitude (length)
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat magnitude() const
	{
		return VECMag(&v);
	}

	//! Squared magnitude (length)
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat magnitudeSquared() const
	{
		return VECSquareMag(&v);
	}

	//! Return vector with element wise min(this,other)
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector minElts(const PxcVector& other) const
	{
		return PxcVector(PxcMin(other.x, this->x), PxcMin(other.y, this->y), PxcMin(other.z, this->z));
	}

	//! Return vector with element wise max(this,other)
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector maxElts(const PxcVector& other) const
	{
		return PxcVector(PxcMax(other.x, this->x), PxcMax(other.y, this->y), PxcMax(other.z, this->z));
	}

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
	union{
		Vec v;
		struct{
			PxFloat x,y,z;
		};
	};
	
#ifndef __SPU__
#pragma PXD_POP_PACK
#endif
};



// Free operations on above..
// scalar*vector


CUDA_CALLABLE PXD_FORCE_INLINE PxcVector operator*(PxFloat scalar, const PxcVector& v)
{
	return PxcVector(v.x*scalar, v.y*scalar, v.z*scalar);
}


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
