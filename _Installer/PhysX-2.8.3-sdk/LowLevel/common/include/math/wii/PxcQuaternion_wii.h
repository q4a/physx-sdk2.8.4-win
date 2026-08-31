#ifndef PXC_QUATERNION_WII_H
#define PXC_QUATERNION_WII_H

#ifdef __PPCGEKKO__

#include "px_config.h"

#include "PxcMath.h"
#include "PxcVector.h"

/*!
Basic quaternion.

Do not define and use any Constructors, the c++ standard does not allow it in unions:


11A union member shall not be of a class type (or  array  thereof)  that
  has a non-trivial constructor.

(this class is used in unions in the SolverConstraints and elsewhere)

dsequeira: this is just a temporary fix - the right thing is to take the classes with
constructors out of the unions, _not_ to take the constructors out of the classes - 
several important idioms (i.e. initializer lists and RVO) require constructors to work.)

Make sure to have same physical layout as PxdQuaternion
*/
class PxcQuaternion
{
	friend class PxcTransform;

public:
	//! Default constructor
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcQuaternion()
	{}

	//! Copy constructor
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcQuaternion(const PxcQuaternion& other)
		: v(other.v), w(other.w)
	{}

	//! Construct from four scalars
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcQuaternion(PxFloat x, PxFloat y, PxFloat z, PxFloat w)
		: v(x,y,z), w(w)
	{}

	//! Construct from PxdQuaternion
	explicit CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcQuaternion(const PxdQuaternion& other)
		: v(other.x,other.y,other.z), w(other.w)
	{}

	//! Construct from PxcVector + w
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcQuaternion(const PxcVector& v, PxFloat w)
		: v(v), w(w)
	{}

private:

	//! Construct from float[4]
	explicit CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcQuaternion(PxFloat values[])
		: v(values),w(values[3])
	{}


	// NOTE: no constructors for gcc

public:

	// dsequeira: added factory methods to replace the constructors (temporary!!)

	static PxcQuaternion create(PxFloat x, PxFloat y, PxFloat z, PxFloat w)
	{
		PxcQuaternion q;
		q.v.set(x,y,z);
		q.w = w;
		return q;
	}

	static PxcQuaternion create(const PxcVector &v, PxFloat w)
	{
		PxcQuaternion q;
		q.v = v;
		q.w = w;
		return q;
	}



	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void fromAngleAxis(PxFloat Angle, const PxcVector & axis)			// set the NxQuat by Angle-axis (see AA constructor)
	{
		QUATRotAxisRad(&quat, &axis.v, MTXDegToRad(Angle));
	}

	//! Construct from four scalars
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void set(PxFloat xOther, PxFloat yOther, PxFloat zOther, PxFloat wOther)
	{
		v.set(xOther,yOther,zOther);
		w = wOther;
	}

	//! Construct from PxcVector + w
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void set(const PxcVector& v, PxFloat wOther)
	{
		this->v = v;
		w = wOther;
	}

	//! Construct from PxdQuaternion
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void set(const PxdQuaternion& other)
	{
		v.set(other.x,other.y,other.z);
		w = other.w;
	}

	/* Cast-operator, there are very evil and rely on the fact that
	PxcQuaternion & PxdQuaternion have same memory layout
	*/
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE operator PxdQuaternion const& () const
	{
		return *reinterpret_cast<const PxdQuaternion*>(&v);
	}

	//! Set from axis+angle
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void setAxisAngle(const PxcVector& axis, PxFloat angle)
	{
		QUATRotAxisRad(&quat, &axis.v, angle);
	}


	//Operators

	//! Equality operator
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool operator==(const PxcQuaternion& other)
	{
		return (v.x==other.v.x && v.y==other.v.y && v.z==other.v.z && w==other.w);
	}

	//! Inequality operator
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool operator!=(const PxcQuaternion& other)
	{
		return (v.x!=other.v.x || v.y!=other.v.y || v.z!=other.v.z || w!=other.w);
	}

	//! Smaller than, all components
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool operator<(const PxcQuaternion& other)
	{
		return (v.x<other.v.x && v.y<other.v.y && v.z<other.v.z && w<other.w);
	}

	//! Bigger than, all components
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool operator>(const PxcQuaternion& other)
	{
		return (v.x>other.v.x && v.y>other.v.y && v.z>other.v.z && w>other.w);
	}

	//! Return negative
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcQuaternion operator-() const
	{
		return PxcQuaternion(-v, -w);
	}

	// c = a<op>b operators

	//! Add
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcQuaternion operator+(const PxcQuaternion& other) const
	{
		PxcQuaternion temp;
		QUATAdd(&quat, &other.quat, &temp.quat);
		return temp;
	}

	//! Subtract
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcQuaternion operator-(const PxcQuaternion& other) const
	{
		PxcQuaternion temp;
		QUATSubtract(&quat, &other.quat, &temp.quat);
		return temp;	
	}

	//! Multiply
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcQuaternion operator*(const PxcQuaternion& other) const
	{
		PxcQuaternion temp;
		QUATMultiply(&quat, &other.quat, &temp.quat);
		return temp;
	}

	//! Assignment operator
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void operator=(const PxcQuaternion& other)
	{
		v = other.v;
		w = other.w;
	}

	// a <op>= b operators

	//! Equals-add
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void operator+=(const PxcQuaternion& other)
	{
		QUATAdd(&quat, &other.quat, &quat);
	}

	//! Equals-sub
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void operator-=(const PxcQuaternion& other)
	{
		QUATSubtract(&quat, &other.quat, &quat);
	}

	//! Equals-multiply
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void operator*=(const PxcQuaternion& other)
	{
		QUATMultiply(&quat, &other.quat, &quat);
	}

	//! Element access
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat operator[](PxUInt index) const
	{
		return (&v.x)[index];
	}

	//! Element access
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat& operator[](PxUInt index)
	{
		return (&v.x)[index];
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
		return QUATDotProduct(&quat, &quat);
	}

	//! Rotate vector by quaternion
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector rotate(const PxcVector& src) const
	{
		return src * (2.0f*w*w-1.0f) + v.cross(src)*w*2.0f + v*v.dot(src)*2.0f;
	}

	//! Rotate vector by quaternion
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector rotateInv(const PxcVector& src) const
	{
		return src * (2.0f*w*w-1.0f) - v.cross(src)*w*2.0f + v*v.dot(src)*2.0f;
	}

	//! Return inverse quaternion
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcQuaternion getConjugate() const
	{
		return PxcQuaternion(-v, w);
	}

	//! Return a normalized version of this quaternion
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcQuaternion getNormalized() const
	{
		PxcQuaternion temp;
		QUATNormalize(&quat, &temp.quat);
		return temp;
	}

	//! A faster (but somewhat less accurate) normalization for almost-normalized quaternions
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcQuaternion getFastNormalized() const
	{
		PxFloat s = 0.5f*(3-magnitudeSquared());
		PxcQuaternion temp;
		QUATScale(&quat, &temp.quat, s);
		return temp;
	}

	//! Dot quaternion with other quaternion
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat dot(const PxcQuaternion& q)
	{
		return QUATDotProduct(&quat, &q.quat);
	}

	//! Set to identity quaternion
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void setIdentity()
	{
		v.setZero();
		w = 1.0f;
	}

	//! Return boolean indicating if quaternion is identity rotation
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool isIdentity() const
	{
		return v.isZero() && PxcAbs(w) == 1.0f;
	}

	//! Retrieve the first basis vector
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector getBasisVector0() const
	{
		return PxcVector(
			1.0f - 2.0f*(v.y*v.y + v.z*v.z), 
			2.0f*(v.x*v.y + v.z*w),
			2.0f*(v.x*v.z - v.y*w)
			);
	}

	//! Retrieve the second basis vector
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector getBasisVector1() const
	{
		return PxcVector(
			2.0f*(v.x*v.y - v.z*w),
			1.0f - 2.0f*(v.x*v.x + v.z*v.z), 
			2.0f*(v.y*v.z + v.x*w)
			);
	}

	//! Retrieve the third basis vector
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector getBasisVector2() const
	{
		return PxcVector(
			2.0f*(v.x*v.z + v.y*w),
			2.0f*(v.y*v.z - v.x*w),
			1.0f - 2.0f*(v.x*v.x + v.y*v.y) 
			);
	}

	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void getBasisVectors(PxcVector& basis0, PxcVector& basis1, PxcVector& basis2) const
	{
		const PxFloat x2 = v.x + v.x;
		const PxFloat y2 = v.y + v.y;
		const PxFloat z2 = v.z + v.z;

		const PxFloat xx = x2*v.x;
		const PxFloat yy = y2*v.y;
		const PxFloat zz = z2*v.z;

		const PxFloat xy = x2*v.y;
		const PxFloat xz = x2*v.z;
		const PxFloat xw = x2*w;

		const PxFloat yz = y2*v.z;
		const PxFloat yw = y2*w;

		const PxFloat zw = z2*w;
		
		basis0.set(
			1.0f - (yy + zz), 
			(xy + zw),
			(xz - yw)
			);

		basis1.set(
			(xy - zw),
			1.0f - (xx + zz), 
			(yz + xw)
			);

		basis2.set(
			(xz + yw),
			(yz - xw),
			1.0f - (xx + yy) 
			);
	}


	// Data
#ifndef __SPU__
#pragma PXD_PUSH_PACK(1)
#endif // __SPU__
	union
	{
		struct
		{
			PxcVector v;
			float w;
		};
		Quaternion quat;
	};
#ifndef __SPU__
#pragma PXD_POP_PACK
#endif // __SPU__
};

#endif
#endif