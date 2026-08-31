//  PxUtils.h
//
//  Created by Julius Trinkunas on 11/12/12.
//

#ifndef PXUTILS_H
#define PXUTILS_H


const PxdVector kPxdVectorZero = {0, 0, 0};


NX_INLINE const PxdVector& NxToPxd(const NxVec3& vec) {
	return (const PxdVector&)vec;
};

NX_INLINE const PxdQuaternion& NxToPxd(const NxQuat& quat) {
	return (const PxdQuaternion&)quat;
};

NX_INLINE const NxVec3& PxdToNx(const PxdVector& vec) {
	return (const NxVec3&)vec;
};


#endif // PXUTILS_H
