#ifndef PXC_CACHEDTRANSFORMS_H
#define PXC_CACHEDTRANSFORMS_H

#include "PxcTransform.h"
#include "PxcMatrix34.h"
#include "PxcSimd.h"

/*!
Class to hold transforms which are cached and provided to the contact generation routines.
*/
class PxcCachedTransforms
{
public:

	PXD_FORCE_INLINE PxcCachedTransforms() {}

	PXD_FORCE_INLINE PxcCachedTransforms& operator=(const PxcCachedTransforms& a)
	{
		// Use memcpy?
		mShapeToWorld = a.mShapeToWorld;
		mShapeToWorldMatrix = a.mShapeToWorldMatrix;

		mBodyToWorld = a.mBodyToWorld;

		return *this;
	}

	// for dynamic shapes.

	PXD_FORCE_INLINE void set(const PxcTransform& shapeToBody, const PxcTransform& bodyToWorld)
	{
		mShapeToWorld = bodyToWorld.transform(shapeToBody);
		mShapeToWorldMatrix = PxcMatrix34(mShapeToWorld);
		mBodyToWorld = PxcMatrix34(bodyToWorld);
	}

	//HostCm uses this.
	PXD_FORCE_INLINE void set(const PxcTransform& shapeToWorld, const PxcMatrix34& bodyToWorld)
	{
		mShapeToWorld = shapeToWorld;
		mShapeToWorldMatrix = PxcMatrix34(shapeToWorld);
		mBodyToWorld = bodyToWorld;
	}

	// for static shapes.
	PXD_FORCE_INLINE void set(const PxcTransform& shapeToWorld)
	{
		mShapeToWorld = shapeToWorld;
		mShapeToWorldMatrix = PxcMatrix34(shapeToWorld);
		mBodyToWorld.setIdentity();
	}

	PXD_FORCE_INLINE const PxcTransform& getShapeToWorld() const
	{
		return mShapeToWorld;
	}

	PXD_FORCE_INLINE const PxcMatrix34& getShapeToWorldMatrix() const
	{
		return mShapeToWorldMatrix;
	}

	PXD_FORCE_INLINE const PxcMatrix34& getBodyToWorldMatrix() const
	{
		return mBodyToWorld;
	}

private:

	PxcTransform	mShapeToWorld;
	PxcMatrix34		mShapeToWorldMatrix;

	PxcMatrix34		mBodyToWorld;
};

#endif
