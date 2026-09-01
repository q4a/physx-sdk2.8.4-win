#ifndef __NX_CURVE_H__
#define __NX_CURVE_H__
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
#include "NxApex.h"
#include "NxParameterized.h"

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
	A trivial templatized math vector type.
*/
template <typename T>
struct NxVec2T
{
	NxVec2T(T _x=T(), T _y=T()) : x(_x), y(_y) { }

	T x, y;

	T& operator[](physx::PxU32 ndx)
	{
		PX_ASSERT(ndx < 2);
		return ((T*)&x)[ndx];
	}
};

// Some useful typedefs
typedef NxVec2T<physx::PxF32> NxVec2R;

/**
	The NxCurve is a class for storing control points on a curve and evaluating the results later.
*/
class NxCurve
{
public:
	virtual ~NxCurve() {}

	/**
		Retrieve the output Y for the specified input x, based on the properties of the stored curve described
		by mControlPoints.
	*/
	virtual physx::PxF32 evaluate(physx::PxF32 x) const = 0;

	/**
		Add a control point to the list of control points, returning the index of the new point.
	*/
	virtual physx::PxU32 addControlPoint(const NxVec2R& controlPoint) = 0;

	/**
		Add a control points to the list of control points.  Assuming the 
		hPoints points to a list of vec2s
	*/
	virtual void addControlPoints(::NxParameterized::Interface *param, ::NxParameterized::Handle &hPoints) = 0;

	/**
		Locates the control points that contain x, placing the resulting control points in the two
		out parameters. Returns true if the points were found, false otherwise. If the points were not
		found, the output variables are untouched
	*/
	virtual bool calculateControlPoints(physx::PxF32 x, NxVec2R& outXPoints, NxVec2R& outYPoints) const = 0;

	/**
		Locates the first control point with x larger than xValue or the nimber of control points if such point doesn't exist
	*/
	virtual physx::PxU32 calculateFollowingControlPoint(physx::PxF32 xValue) const = 0;

	///get the array of control points
	virtual const NxVec2R* getControlPoints( physx::PxU32 &outCount ) const = 0;
};

PX_POP_PACK

}} // namespace apex

#endif /* __NX_CURVE_H__ */
