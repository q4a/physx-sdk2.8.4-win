// This code contains NVIDIA Confidential Information and is disclosed 
// under the Mutual Non-Disclosure Agreement.
//
// Notice
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES 
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO 
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT, 
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable. 
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such 
// information or for any infringement of patents or other rights of third parties that may 
// result from its use. No license is granted by implication or otherwise under any patent 
// or patent rights of NVIDIA Corporation. Details are subject to change without notice. 
// This code supersedes and replaces all information previously supplied. 
// NVIDIA Corporation products are not authorized for use as critical 
// components in life support devices or systems without express written approval of 
// NVIDIA Corporation.
//
// Copyright © 2009 NVIDIA Corporation. All rights reserved.
// Copyright © 2002-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright © 2001-2006 NovodeX. All rights reserved.   


#ifndef PX_FOUNDATION_PXQUAT_H
#define PX_FOUNDATION_PXQUAT_H

/** \addtogroup foundation
@{
*/

#include "PxVec3.h"

namespace physx
{
namespace pubfnd2
{

PX_PUSH_PACK_DEFAULT

/**
\brief This is a quaternion class. For more information on quaternion mathematics
consult a mathematics source on complex numbers.

*/

class PxQuat
{
public:
	/**
	\brief Default constructor, does not do any initialization.
	*/
	PX_CUDA_MEMBER PX_INLINE PxQuat()	{	}


	/**
	\brief Constructor.  Take note of the order of the elements!
	*/
	PX_CUDA_MEMBER PX_INLINE PxQuat(PxReal nx, PxReal ny, PxReal nz, PxReal nw) : x(nx),y(ny),z(nz),w(nw) {}

	/**
	\brief Creates from angle-axis representation.

	Axis must be normalized!

	Angle is in radians!

	<b>Unit:</b> Radians
	*/
	PX_CUDA_MEMBER PX_INLINE PxQuat(PxReal angleRadians, const PxVec3& unitAxis)
	{
		PX_ASSERT(PxAbs(1.0f-unitAxis.magnitude())<1e-3f);
		const PxReal a = angleRadians * 0.5f;
		const PxReal s = PxSin(a);
		w = PxCos(a);
		x = unitAxis.x * s;
		y = unitAxis.y * s;
		z = unitAxis.z * s;
	}

	/**
	\brief Copy ctor.
	*/
	PX_CUDA_MEMBER PX_INLINE PxQuat(const PxQuat& v): x(v.x), y(v.y), z(v.z), w(v.w) {}

	/**
	\brief Creates from orientation matrix.

	\param[in] m Rotation matrix to extract quaternion from.
	*/
	PX_CUDA_MEMBER PX_INLINE explicit PxQuat(const PxMat33& m); /* defined in PxMat33.h */

	/**
	\brief returns true if all elements are finite (not NAN or INF, etc.)
	*/
	PX_CUDA_MEMBER bool isFinite() const
	{
		return PxIsFinite(x) 
			&& PxIsFinite(y) 
			&& PxIsFinite(z)
			&& PxIsFinite(w);
	}

	/**
	\brief returns true if finite and magnitude is close to unit
	*/

	PX_CUDA_MEMBER bool isValid() const
	{
		const PxReal unitTolerance = PxReal(1e-4);
		return isFinite() && PxAbs(magnitude()-1)<unitTolerance;
	}

	PX_CUDA_MEMBER PX_INLINE void toRadiansAndUnitAxis(PxReal& angle, PxVec3& axis) const
	{
		const PxReal quatEpsilon = (PxReal(1.0e-8f));
		const PxReal s2 = x*x+y*y+z*z;
		if(s2<quatEpsilon*quatEpsilon)  // can’t extract a sensible axis
		{
			angle = 0;
			axis = PxVec3(1,0,0);
		}
		else
		{
			const PxReal s = PxRecipSqrt(s2);
			axis = PxVec3(x,y,z) * s; 
			angle = w<quatEpsilon ? PxPi : PxAtan2(s2*s, w) * 2;
		}

	}

	/**
	\brief Gets the angle between this quat and the identity quaternion.

	<b>Unit:</b> Radians
	*/
	PX_CUDA_MEMBER PX_INLINE PxReal getAngle() const
	{
		return PxAcos(w) * PxReal(2);
	}


	/**
	\brief Gets the angle between this quat and the argument

	<b>Unit:</b> Radians
	*/
	PX_CUDA_MEMBER PX_INLINE PxReal getAngle(const PxQuat& q) const
	{
		return PxAcos(dot(q)) * PxReal(2);
	}


	/**
	\brief This is the squared 4D vector length, should be 1 for unit quaternions.
	*/
	PX_CUDA_MEMBER PX_INLINE PxReal magnitudeSquared() const
	{
		return x*x + y*y + z*z + w*w;
	}

	/**
	\brief returns the scalar product of this and other.
	*/
	PX_CUDA_MEMBER PX_INLINE PxReal dot(const PxQuat& v) const
	{
		return x * v.x + y * v.y + z * v.z  + w * v.w;
	}

	PX_CUDA_MEMBER PX_INLINE PxQuat getNormalized() const
	{
		const PxReal s = 1.0f/magnitude();
		return PxQuat(x*s, y*s, z*s, w*s);
	}


	PX_CUDA_MEMBER PX_INLINE float magnitude() const
	{
		return PxSqrt(magnitudeSquared());
	}

	//modifiers:
	/**
	\brief maps to the closest unit quaternion.
	*/
	PX_CUDA_MEMBER PX_INLINE PxReal normalize()											// convert this PxQuat to a unit quaternion
	{
		const PxReal mag = magnitude();
		if (mag)
		{
			const PxReal imag = PxReal(1) / mag;

			x *= imag;
			y *= imag;
			z *= imag;
			w *= imag;
		}
		return mag;
	}

	/*
	\brief returns the conjugate.

	\note for unit quaternions, this is the inverse.
	*/
	PX_CUDA_MEMBER PX_INLINE PxQuat getConjugate() const
	{
		return PxQuat(-x,-y,-z,w);
	}

	/*
	\brief returns imaginary part.
	*/
	PX_CUDA_MEMBER PX_INLINE PxVec3& getImaginaryPart()
	{
		return reinterpret_cast<PxVec3&>(x);
	}

	/*
	\brief returns imaginary part.
	*/
	PX_CUDA_MEMBER PX_INLINE const PxVec3& getImaginaryPart() const
	{
		return reinterpret_cast<const PxVec3&>(x);
	}


	/** brief computes rotation of x-axis */
	PX_CUDA_MEMBER PX_FORCE_INLINE PxVec3 getBasisVector0()	const
	{	
//		return rotate(PxVec3(1,0,0));
		const PxF32 x2 = x*2.0f;
		const PxF32 w2 = w*2.0f;
		return PxVec3(	(w * w2) - 1.0f + x*x2,
						(z * w2)        + y*x2,
						(-y * w2)       + z*x2);
	}
	
	/** brief computes rotation of y-axis */
	PX_CUDA_MEMBER PX_FORCE_INLINE PxVec3 getBasisVector1()	const 
	{	
//		return rotate(PxVec3(0,1,0));
		const PxF32 y2 = y*2.0f;
		const PxF32 w2 = w*2.0f;
		return PxVec3(	(-z * w2)       + x*y2,
						(w * w2) - 1.0f + y*y2,
						(x * w2)        + z*y2);
	}


	/** brief computes rotation of z-axis */
	PX_CUDA_MEMBER PX_FORCE_INLINE PxVec3 getBasisVector2() const	
	{	
//		return rotate(PxVec3(0,0,1));
		const PxF32 z2 = z*2.0f;
		const PxF32 w2 = w*2.0f;
		return PxVec3(	(y * w2)        + x*z2,
						(-x * w2)       + y*z2,
						(w * w2) - 1.0f + z*z2);
	}

	/**
	rotates passed vec by this (assumed unitary)
	*/
	PX_CUDA_MEMBER PX_INLINE const PxVec3 rotate(const PxVec3& v) const
	{
		const PxF32 vx = 2.0f*v.x;
		const PxF32 vy = 2.0f*v.y;
		const PxF32 vz = 2.0f*v.z;
		const PxF32 w2 = w*w-0.5f;
		const PxF32 dot2 = (x*vx + y*vy +z*vz);
		return PxVec3
		(
			(vx*w2 + (y * vz - z * vy)*w + x*dot2), 
			(vy*w2 + (z * vx - x * vz)*w + y*dot2), 
			(vz*w2 + (x * vy - y * vx)*w + z*dot2)
		);
		/*
		const PxVec3 qv(x,y,z);
		return (v*(w*w-0.5f) + (qv.cross(v))*w + qv*(qv.dot(v)))*2;
		*/
	}

	/**
	inverse rotates passed vec by this (assumed unitary)
	*/
	PX_CUDA_MEMBER PX_INLINE const PxVec3 rotateInv(const PxVec3& v) const
	{
		const PxF32 vx = 2.0f*v.x;
		const PxF32 vy = 2.0f*v.y;
		const PxF32 vz = 2.0f*v.z;
		const PxF32 w2 = w*w-0.5f;
		const PxF32 dot2 = (x*vx + y*vy +z*vz);
		return PxVec3
		(
			(vx*w2 - (y * vz - z * vy)*w + x*dot2), 
			(vy*w2 - (z * vx - x * vz)*w + y*dot2), 
			(vz*w2 - (x * vy - y * vx)*w + z*dot2)
		);
//		const PxVec3 qv(x,y,z);
//		return (v*(w*w-0.5f) - (qv.cross(v))*w + qv*(qv.dot(v)))*2;
	}

	/**
	\brief Assignment operator
	*/
	PX_CUDA_MEMBER PX_FORCE_INLINE	PxQuat&	operator=(const PxQuat& p)			{ x = p.x; y = p.y; z = p.z; w = p.w;	return *this;		}

	PX_CUDA_MEMBER PX_INLINE PxQuat& operator*= (const PxQuat& q)
	{
		const PxReal tx = w*q.x + q.w*x + y*q.z - q.y*z;
		const PxReal ty = w*q.y + q.w*y + z*q.x - q.z*x;
		const PxReal tz = w*q.z + q.w*z + x*q.y - q.x*y;

		w = w*q.w - q.x*x - y*q.y - q.z*z;
		x = tx;
		y = ty;
		z = tz;

		return *this;
	}

	PX_CUDA_MEMBER PX_FORCE_INLINE PxQuat& operator+= (const PxQuat& q)
	{
		x+=q.x;
		y+=q.y;
		z+=q.z;
		w+=q.w;
		return *this;
	}

	PX_CUDA_MEMBER PX_FORCE_INLINE PxQuat& operator-= (const PxQuat& q)
	{
		x-=q.x;
		y-=q.y;
		z-=q.z;
		w-=q.w;
		return *this;
	}

	PX_CUDA_MEMBER PX_FORCE_INLINE PxQuat& operator*= (const PxReal s)
	{
		x*=s;
		y*=s;
		z*=s;
		w*=s;
		return *this;
	}

	/** quaternion multiplication */
	PX_CUDA_MEMBER PX_INLINE PxQuat operator*(const PxQuat& q) const
	{
		return PxQuat(w*q.x + q.w*x + y*q.z - q.y*z,
					  w*q.y + q.w*y + z*q.x - q.z*x,
					  w*q.z + q.w*z + x*q.y - q.x*y,
					  w*q.w - x*q.x - y*q.y - z*q.z);
	}

	/** quaternion addition */
	PX_CUDA_MEMBER PX_FORCE_INLINE PxQuat operator+(const PxQuat& q) const
	{
		return PxQuat(x+q.x,y+q.y,z+q.z,w+q.w);
	}

	/** quaternion subtraction */
	PX_CUDA_MEMBER PX_FORCE_INLINE PxQuat operator-() const
	{
		return PxQuat(-x,-y,-z,-w);
	}


	PX_CUDA_MEMBER PX_FORCE_INLINE PxQuat operator-(const PxQuat& q) const
	{
		return PxQuat(x-q.x,y-q.y,z-q.z,w-q.w);
	}


	PX_CUDA_MEMBER PX_FORCE_INLINE PxQuat operator*(PxReal r) const
	{
		return PxQuat(x*r,y*r,z*r,w*r);
	}

	static PX_CUDA_MEMBER PX_INLINE PxQuat identity() { return PxQuat(0,0,0,1); }

	/** DEPRECATED FUNCTIONS */
	PX_DEPRECATED PxQuat operator!() const { return getConjugate(); }

	/** the quaternion elements */
	PxReal x,y,z,w;
};

PX_POP_PACK

} // namespace pubfnd2
} // end namespace physx

/** @} */
#endif
