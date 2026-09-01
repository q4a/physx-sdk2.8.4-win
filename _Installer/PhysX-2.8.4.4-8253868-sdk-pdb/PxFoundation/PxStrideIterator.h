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

#ifndef PX_FOUNDATION_PXSTRIDEITERATOR_H
#define PX_FOUNDATION_PXSTRIDEITERATOR_H

#include "PxAssert.h"

/** \addtogroup foundation
  @{
*/

#include "PxAssert.h"

namespace physx
{
namespace pubfnd2
{

PX_PUSH_PACK_DEFAULT

    template<typename T>
    class PxStrideIterator
    {
        template <typename X>
        struct StripConst
        {
            typedef X Type;
        };

        template <typename X>
        struct StripConst<const X>
        {
            typedef X Type;
        };

    public:
        explicit PX_INLINE PxStrideIterator(T* ptr = NULL, PxU32 stride = sizeof(T)) :
            mPtr(ptr), mStride(stride)
        {
            PX_ASSERT(mStride == 0 || sizeof(T) <= mStride);
        }

        PX_INLINE PxStrideIterator(const PxStrideIterator<typename StripConst<T>::Type>& strideIterator) :
            mPtr(strideIterator.ptr()), mStride(strideIterator.stride())
        {
            PX_ASSERT(mStride == 0 || sizeof(T) <= mStride);
        }

        PX_INLINE T* ptr() const
        {
            return mPtr;
        }

        PX_INLINE PxU32 stride() const
        {
            return mStride;
        }

        PX_INLINE T& operator*() const
        {
            return *mPtr;
        }

        PX_INLINE T* operator->() const
        {
            return mPtr;
        }

        PX_INLINE T& operator[](int i) const
        {
            return *byteAdd(mPtr, i * stride());
        }

        // preincrement
        PX_INLINE PxStrideIterator& operator++()
        {
            mPtr = byteAdd(mPtr, stride());
            return *this;
        }

        // postincrement
        PX_INLINE PxStrideIterator operator++(int)
        {
            PxStrideIterator tmp = *this;
            mPtr = byteAdd(mPtr, stride());
            return tmp;
        }

        // predecrement
        PX_INLINE PxStrideIterator& operator--()
        {
            mPtr = byteSub(mPtr, stride());
            return *this;
        }

        // postdecrement
        PX_INLINE PxStrideIterator operator--(int)
        {
            PxStrideIterator tmp = *this;
            mPtr = byteSub(mPtr, stride());
            return tmp;
        }

        PX_INLINE PxStrideIterator& operator+=(int i)
        {
            mPtr = byteAdd(mPtr, i * stride());
            return *this;
        }

        PX_INLINE PxStrideIterator operator+(int i) const
        {	
            return PxStrideIterator(byteAdd(mPtr, i * stride()), stride());
        }

        PX_INLINE PxStrideIterator& operator-=(int i)
        {
            mPtr = byteSub(mPtr, i * stride());
            return *this;
        }

        PX_INLINE PxStrideIterator operator-(int i) const
        {
            return PxStrideIterator(byteSub(mPtr, i * stride()), stride());
        }

        // iterator difference
        PX_INLINE int operator-(const PxStrideIterator& other) const
        {
            PX_ASSERT(isCompatible(other));
            int byteDiff = static_cast<int>(reinterpret_cast<const PxU8*>(mPtr) - reinterpret_cast<const PxU8*>(other.mPtr));
            return byteDiff / static_cast<int>(stride());
        }

        PX_INLINE bool operator==(const PxStrideIterator& other) const
        {
            PX_ASSERT(isCompatible(other));
            return mPtr == other.mPtr;
        }

        PX_INLINE bool operator!=(const PxStrideIterator& other) const
        {
            PX_ASSERT(isCompatible(other));
            return mPtr != other.mPtr;
        }

        PX_INLINE bool operator<(const PxStrideIterator& other) const
        {
            PX_ASSERT(isCompatible(other));
            return mPtr < other.mPtr;
        }

        PX_INLINE bool operator>(const PxStrideIterator& other) const
        {
            PX_ASSERT(isCompatible(other));
            return mPtr > other.mPtr;
        }

        PX_INLINE bool operator<=(const PxStrideIterator& other) const
        {
            PX_ASSERT(isCompatible(other));
            return mPtr <= other.mPtr;
        }

        PX_INLINE bool operator>=(const PxStrideIterator& other) const
        {
            PX_ASSERT(isCompatible(other));
            return mPtr >= other.mPtr;
        }

    private:
        PX_INLINE static T* byteAdd(T* ptr, PxU32 bytes) 
        { 
            return const_cast<T*>(reinterpret_cast<const T*>(reinterpret_cast<const PxU8*>(ptr) + bytes));
        }

        PX_INLINE static T* byteSub(T* ptr, PxU32 bytes) 
        { 
            return const_cast<T*>(reinterpret_cast<const T*>(reinterpret_cast<const PxU8*>(ptr) - bytes));
        }

        PX_INLINE bool isCompatible(const PxStrideIterator& other) const
        {
            int byteDiff = static_cast<int>(reinterpret_cast<const PxU8*>(mPtr) - reinterpret_cast<const PxU8*>(other.mPtr));
            return (stride() == other.stride()) && (abs(byteDiff) % stride() == 0);
        }

        T* mPtr;
        PxU32 mStride;
    };

    template<typename T>
    PX_INLINE PxStrideIterator<T> operator+(int i, PxStrideIterator<T> it)
    {
        it += i;
        return it;
    }

	template<typename T>
	PX_INLINE PxStrideIterator<T> makeIterator(T* ptr, PxU32 stride = sizeof(T))
	{
		return PxStrideIterator<T>(ptr, stride);
	}

	template<typename T>
	PX_INLINE PxStrideIterator<const T> makeIterator(const T* ptr, PxU32 stride = sizeof(T))
	{
		return PxStrideIterator<const T>(ptr, stride);
	}

PX_POP_PACK

} // namespace pubfnd2
} // namespace physx

 /** @} */
#endif
