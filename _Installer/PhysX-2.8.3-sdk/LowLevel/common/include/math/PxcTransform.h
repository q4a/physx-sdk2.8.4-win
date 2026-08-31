#ifndef PXC_TRANSFORM_H
#define PXC_TRANSFORM_H

#include "px_config.h"

#include "PxcQuaternion.h"
#include "PxcVector.h"

/*!
Transform (translation+rotation) represented as a vector and a quaternion.

T = [translate, rotate]

Make sure to match data-format of PxdTransform
*/
class PxcTransform
{
public:
	//! Default constructor
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcTransform()
	{}

	//! Construct from PxdTransform
	explicit CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcTransform(const PxdTransform& other)
	//	: /*translation(other.translation), rotation(other.rotation)*/
	{
		rotation.set(other.rotation);
		translation.x = other.translation.x;
		translation.y = other.translation.y;
		translation.z = other.translation.z;
	}


	//! Construct from PxcVector + PxcQuaternion
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcTransform(const PxcVector& t, const PxcQuaternion& r)
		: translation(t), rotation(r)
	{}

	//! Copy constructor
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcTransform(const PxcTransform& other)
		: translation(other.translation), rotation(other.rotation)
	{}

	//! Assignment operator
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE const PxcTransform& operator=(const PxcTransform& other)
	{
		translation = other.translation;
		rotation = other.rotation;
		return *this;
	}

	/* Cast-operator, there are very evil and rely on the fact that
	PxcTransform & PxdTransform have same memory layout
	*/
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE operator PxdTransform const& () const
	{
		return *reinterpret_cast<const PxdTransform*>(&translation.x);
	}

	//! Return inverse transform
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcTransform getInverse() const
	{
		return PxcTransform(rotation.rotateInv(-translation), rotation.getConjugate());
	}

	//! Rotate vector with transform (not taking translation into account)
	CUDA_CALLABLE_MEMBER CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector rotate(const PxcVector& src) const
	{
		// v' = rotation*v
		return rotation.rotate(src);
	}

	//! Rotate vector with inverse transform (not taking translation into account)
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector rotateInv(const PxcVector& src) const
	{
		// v' = rotation^-1*v
		return rotation.rotateInv(src);
	}

	//! Transform vector with transform
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector transform(const PxcVector& src) const
	{
		// v' = rotation*v + translation
		return rotation.rotate(src) + translation;
	}

	//! Transform vector with inverse transform
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector transformInv(const PxcVector& src) const
	{
		// v' = rotation^-1(v-translate)
		return rotation.rotateInv(src-translation);
	}

	//! Transform transform to parent
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcTransform transform(const PxcTransform& src) const
	{
		// src = [srct, srcr] -> [r*srct + t, r*srcr]
		return PxcTransform(rotation.rotate(src.translation) + translation,
			rotation*src.rotation);
	}

	//! Transform transform from parent
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcTransform transformInv(const PxcTransform& src) const
	{
		// src = [srct, srcr] -> [r^-1*(srct-t), r^-1*srcr]
		PxcQuaternion rinv = rotation.getConjugate();
		return PxcTransform(rinv.rotate(src.translation - translation),
			rinv*src.rotation);
	}

	//! Retrieve the first basis vector
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector getBasisVector0() const
	{
		return rotation.getBasisVector0();
	}

	//! Retrieve the second basis vector
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector getBasisVector1() const
	{
		return rotation.getBasisVector1();
	}

	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void getBasisVectors(PxcVector& base0, PxcVector& base1, PxcVector& base2) const 
	{
		return rotation.getBasisVectors(base0, base1, base2);
	}

	//! Retrieve the third basis vector
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector getBasisVector2() const
	{
		return rotation.getBasisVector2();
	}

	//! Computes the maximum delta to another transform
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat computeMaxComponentDelta(const PxcTransform& t) const
	{
		PxFloat tmp, delta = 0.0f;
		tmp = PxcAbs(t.translation.x - translation.x);	if(tmp>delta)	delta=tmp;
		tmp = PxcAbs(t.translation.y - translation.y);	if(tmp>delta)	delta=tmp;
		tmp = PxcAbs(t.translation.z - translation.z);	if(tmp>delta)	delta=tmp;

		tmp = PxcAbs(t.rotation.v.x - rotation.v.x);	if(tmp>delta)	delta=tmp;
		tmp = PxcAbs(t.rotation.v.y - rotation.v.y);	if(tmp>delta)	delta=tmp;
		tmp = PxcAbs(t.rotation.v.z - rotation.v.z);	if(tmp>delta)	delta=tmp;

		tmp = PxcAbs(t.rotation.w - rotation.w);		if(tmp>delta)	delta=tmp;
		return delta;
	}

#ifndef __SPU__
#pragma PXD_PUSH_PACK(1)
#endif
	// Data
	PxcVector translation;
	PxcQuaternion rotation;
#ifndef __SPU__
#pragma PXD_POP_PACK
#endif
};

#endif
