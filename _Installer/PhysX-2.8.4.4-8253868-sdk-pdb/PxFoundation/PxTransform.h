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

#ifndef PX_FOUNDATION_PXTRANSFORM_H
#define PX_FOUNDATION_PXTRANSFORM_H

#include "PxQuat.h"

namespace physx
{
namespace pubfnd2
{

PX_PUSH_PACK_DEFAULT

class PxTransform
{
public:
	// Important: Ordering of members is important. See Body::LLState
	PxQuat q;
	PxVec3 p;

	PX_FORCE_INLINE PxTransform(): q(0, 0, 0, 1), p(0, 0, 0)
	{
	}

	PX_FORCE_INLINE PxTransform(const PxVec3& p0, const PxQuat& q0): q(q0), p(p0) 
	{
	}

	PX_CUDA_MEMBER PX_FORCE_INLINE PxTransform operator*(const PxTransform& x) const
	{
		return transform(x);
	}

	PX_CUDA_MEMBER PX_FORCE_INLINE PxTransform getInverse() const
	{
		return PxTransform(q.rotateInv(-p),q.getConjugate());
	}


	PX_CUDA_MEMBER PX_FORCE_INLINE PxVec3 transform(const PxVec3& input) const
	{
		return q.rotate(input) + p;
	}

	PX_CUDA_MEMBER PX_FORCE_INLINE PxVec3 transformInv(const PxVec3& input) const
	{
		return q.rotateInv(input-p);
	}

	PX_CUDA_MEMBER PX_FORCE_INLINE PxVec3 rotate(const PxVec3& input) const
	{
		return q.rotate(input);
	}

	PX_CUDA_MEMBER PX_FORCE_INLINE PxVec3 rotateInv(const PxVec3& input) const
	{
		return q.rotateInv(input);
	}


//	PX_INLINE void setIdentity() {	p.setZero(); q.setIdentity(); }

	//! Transform transform to parent (returns compound transform: first src, then *this)
	PX_CUDA_MEMBER PX_FORCE_INLINE PxTransform transform(const PxTransform& src) const
	{
		// src = [srct, srcr] -> [r*srct + t, r*srcr]
		return PxTransform(q.rotate(src.p) + p, q*src.q);
	}

	/**
	\brief returns true if finite and q is a unit quaternion
	*/

	PX_CUDA_MEMBER bool isValid() const
	{
		return p.isFinite() && q.isFinite() && q.isValid();
	}


	/**
	\brief returns true if all elems are finite (not NAN or INF, etc.)
	*/
	PX_FORCE_INLINE bool isFinite() const { return p.isFinite() && q.isFinite(); }

	//! Transform transform from parent (returns compound transform: first src, then this->inverse)
	PX_CUDA_MEMBER PX_FORCE_INLINE PxTransform transformInv(const PxTransform& src) const
	{
		// src = [srct, srcr] -> [r^-1*(srct-t), r^-1*srcr]
		PxQuat qinv = q.getConjugate();
		return PxTransform(qinv.rotate(src.p - p), qinv*src.q);
	}

	static PX_FORCE_INLINE PxTransform identity() 
	{ 
		return PxTransform(); 
	}
};

PX_POP_PACK

} // namespace pubfnd2
} // end namespace physx

#endif
