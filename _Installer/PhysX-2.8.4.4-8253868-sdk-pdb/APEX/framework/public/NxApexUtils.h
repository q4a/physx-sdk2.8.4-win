#include "NxApex.h"
#ifndef __NX_APEX_UTILS_H__
#define __NX_APEX_UTILS_H__
/*
 * Copyright 2009-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

/*!
\file
\brief Misc utility classes
*/

#include "NxPlane.h"
#include "PxMath.h"
#include "NxFromPx.h"

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
\brief Linear interpolation
*/
class NxLerp : public NxInterpolator
{
public:
	virtual physx::PxF32 interpolate(physx::PxF32 inCurrent, physx::PxF32 inMin, physx::PxF32 inMax, physx::PxF32 outMin, physx::PxF32 outMax)
	{
		/*
		our return value is to be to the range outMin:outMax as
		inCurrent is to the range inMin:inMax -- and it is legal for it to lie outside the range!

		first convert everything to floats.
		then figure out the slope of the line via a division:
		*/

		if (inMin == inMax)  //special case to avoid divide by zero.
		{
			return outMin;
		}

		return ((inCurrent - inMin) / (inMax - inMin)) * (outMax - outMin) + outMin;
	}
};


/**
\brief Method by which chunk mesh collision hulls are generated.
*/
struct NxConvexHullMethod
{
	enum Enum
	{
		USE_6_DOP,
		USE_10_DOP_X,
		USE_10_DOP_Y,
		USE_10_DOP_Z,
		USE_14_DOP_XY,
		USE_14_DOP_YZ,
		USE_14_DOP_ZX,
		USE_18_DOP,
		USE_26_DOP,
		WRAP_GRAPHICS_MESH,

		COUNT
	};
};


/*
	Global utilities
 */


/**
\brief Stores the info needed for the cofactor matrix of a 4x4 homogeneous transformation matrix (implicit last row = 0 0 0 1)
*/
class NxCof44
{
public:
	/**
	\param [in] m can be an arbitrary homogeneoous transformation matrix
	*/
	NxCof44( const PxMat44 & m )
	{
		_block33(0,0) = m(1,1)*m(2,2)-m(1,2)*m(2,1);
		_block33(0,1) = m(1,2)*m(2,0)-m(1,0)*m(2,2);
		_block33(0,2) = m(1,0)*m(2,1)-m(1,1)*m(2,0);
		_block33(1,0) = m(2,1)*m(0,2)-m(2,2)*m(0,1);
		_block33(1,1) = m(2,2)*m(0,0)-m(2,0)*m(0,2);
		_block33(1,2) = m(2,0)*m(0,1)-m(2,1)*m(0,0);
		_block33(2,0) = m(0,1)*m(1,2)-m(0,2)*m(1,1);
		_block33(2,1) = m(0,2)*m(1,0)-m(0,0)*m(1,2);
		_block33(2,2) = m(0,0)*m(1,1)-m(0,1)*m(1,0);
		_block13 = _block33.transformTranspose( -m.getPosition() );
		_44 = _block33(0,0)*m(0,0)+_block33(0,1)*m(0,1)+_block33(0,2)*m(0,2);

		if (_44 < 0)
		{
			// det is < 0, we need to negate all values
			// The Cov Matrix divided by the determinant is the same as the inverse transposed of an affine transformation
			// For rotation normals, dividing by the determinant is useless as it gets renormalized afterwards again.
			// If the determinant is negative though, it is important that all values are negated to get the right results.
			_block33 *= -1;
			_block13 *= -1;
			_44 = -_44;
		}
	}

	/**
	\param [in] rt must be pure (proper) rotation and translation
	\param [in] s is any diagonal matrix (typically scale).
	\note The combined transform is assumed to be (rt)*s, i.e. s is applied first
	*/
	NxCof44( const PxMat44& rt, const PxVec3 s )
	{
		const PxVec3 cofS( s.y*s.z, s.z*s.x, s.x*s.y );
		_block33(0,0) = rt(0,0)*cofS.x;
		_block33(0,1) = rt(0,1)*cofS.y;
		_block33(0,2) = rt(0,2)*cofS.z;
		_block33(1,0) = rt(1,0)*cofS.x;
		_block33(1,1) = rt(1,1)*cofS.y;
		_block33(1,2) = rt(1,2)*cofS.z;
		_block33(2,0) = rt(2,0)*cofS.x;
		_block33(2,1) = rt(2,1)*cofS.y;
		_block33(2,2) = rt(2,2)*cofS.z;
		_block13 = _block33.transformTranspose( -rt.getPosition() );
		_44 = cofS.x*s.x;

		if (_44 < 0)
		{
			// det is < 0, we need to negate all values, see above
			_block33 *= -1;
			_block13 *= -1;
			_44 = -_44;
		}
	}

	/**
	\brief Transforms a plane equation correctly even when the transformation is not a rotation
	\note If the transformation is not a rotation then the length of the plane's normal vector is not preserved in general.
	*/
	void transform( const NxPlane& src, NxPlane& dst ) const
	{
		const PxVec3 srcNormal( src.normal.x, src.normal.y, src.normal.z );
		const PxVec3 dstNormal = _block33.transform( srcNormal );
		dst.normal.set( dstNormal.x, dstNormal.y, dstNormal.z );
		dst.d = (_block13|srcNormal) + _44*src.d;
	}

	/**
	\brief Transforms a normal correctly even when the transformation is not a rotation
	\note If the transformation is not a rotation then the normal's length is not preserved in general.
	*/
	const PxMat33&	getBlock33() const { return _block33; }

	/**
	\brief The determinant of the original matrix.
	*/
	PxF32			getDeterminant() const { return _44; }

private:
	PxMat33	_block33;
	PxVec3	_block13;
	PxF32	_44;
};

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_APEX_UTILS_H__
