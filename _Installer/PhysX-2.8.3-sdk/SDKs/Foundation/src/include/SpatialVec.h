#ifndef NX_PHYSICS_SPATIALVEC
#define NX_PHYSICS_SPATIALVEC
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Nxf.h"
//namespace NxRigidBody
//	{

class SpatialVec
	{
	public:
	/**
	[ a ]
	[ b ]
	*/
	NxVec3 a, b;

	NX_INLINE SpatialVec()	{}
	NX_INLINE ~SpatialVec()	{}

	NX_INLINE SpatialVec(const NxVec3& a_, const NxVec3& b_) : a(a_), b(b_)	{}

	NX_INLINE void zero();

	/**
	returns the scalar product of 
	this and other.
	*/
	NX_INLINE NxReal dot(const SpatialVec &) const;


	/**
	this = -a
	*/
	NX_INLINE void setNegative(const SpatialVec &a);
	/**
	this = -this
	*/
	NX_INLINE void setNegative();
	/**
	this = a + b
	*/
	NX_INLINE void add(const SpatialVec &a, const SpatialVec &b);
	/**
	this = a - b
	*/
	NX_INLINE void subtract(const SpatialVec &a, const SpatialVec &b);
	/**
	this = a * b
	*/
	NX_INLINE void multiply(NxReal b, const SpatialVec &a);

	//operators
	NX_INLINE const SpatialVec &operator +=(const SpatialVec &);
	NX_INLINE const SpatialVec &operator -=(const SpatialVec &);
	NX_INLINE const SpatialVec &operator *=(NxReal);

	NX_INLINE NxReal& operator[](int index);
	NX_INLINE NxReal  operator[](int index) const;
	};



NX_INLINE void SpatialVec::zero()
	{
	a.zero();
	b.zero();
	}

NX_INLINE NxReal SpatialVec::dot(const SpatialVec &other) const
	{
	return a.dot(other.b) + b.dot(other.a);
	}

NX_INLINE void SpatialVec::setNegative(const SpatialVec &m)
	{
	a.setNegative(m.a);
	b.setNegative(m.b);
	}

NX_INLINE void SpatialVec::setNegative()
	{
	a.setNegative();
	b.setNegative();
	}

NX_INLINE void SpatialVec::add(const SpatialVec &m, const SpatialVec &n)
	{
	a.add(m.a,n.a);
	b.add(m.b,n.b);
	}

NX_INLINE void SpatialVec::subtract(const SpatialVec &m, const SpatialVec &n)
	{
	a.subtract(m.a,n.a);
	b.subtract(m.b,n.b);
	}

NX_INLINE void SpatialVec::multiply(NxReal n, const SpatialVec &m)
	{
	a.multiply(n, m.a);
	b.multiply(n, m.b);
	}

NX_INLINE const SpatialVec &SpatialVec::operator +=(const SpatialVec &m)
	{
	a += m.a;
	b += m.b;
	return *this;
	}

NX_INLINE const SpatialVec &SpatialVec::operator -=(const SpatialVec &m)
	{
	a -= m.a;
	b -= m.b;
	return *this;
	}

NX_INLINE const SpatialVec &SpatialVec::operator *=(NxReal n)
	{
	a *= n;
	b *= n;
	return *this;
	}

NX_INLINE NxReal & SpatialVec::operator[](int index)
	{
	return index < 3 ? a[index] : b[index-3];
	}

NX_INLINE NxReal SpatialVec::operator[](int index) const
	{
	return index < 3 ? a[index] : b[index-3];
	}

//}
#endif
