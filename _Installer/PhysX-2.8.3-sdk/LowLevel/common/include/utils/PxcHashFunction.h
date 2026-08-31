#ifndef PXC_HASHFUNCTION_H
#define PXC_HASHFUNCTION_H

#if defined(__flash__) || defined(__webgl__)
#include <stdint.h>
#endif

// Disable pointer trnc warning as we do this on purpose.
#ifdef PXD_COMPILER_VC
#pragma warning(push)
#pragma warning(disable: 4311)
#endif

/*!
Central definition of hash functions
*/

// Hash functions
template<class T>
PxUInt PxcHashFunction(const T& key)
{
	return (PxUInt)key;
}

// Thomas Wang's 32 bit mix
// http://www.cris.com/~Ttwang/tech/inthash.htm
template<>
PXD_FORCE_INLINE PxUInt PxcHashFunction<PxU32>(const PxU32& key)
{
	PxU32 k = key;
	k += ~(k << 15);
	k ^= (k >> 10);
	k += (k << 3);
	k ^= (k >> 6);
	k += ~(k << 11);
	k ^= (k >> 16);
	return (PxUInt)k;
}
template<>
PXD_FORCE_INLINE PxUInt PxcHashFunction<PxI32>(const PxI32& key)
{
	return PxcHashFunction<PxU32>((PxU32)key);
}

// Thomas Wang's 64 bit mix
// http://www.cris.com/~Ttwang/tech/inthash.htm
template<>
PXD_FORCE_INLINE PxUInt PxcHashFunction<PxU64>(const PxU64& key)
{
	PxU64 k = key;
	k += ~(k << 32);
	k ^= (k >> 22);
	k += ~(k << 13);
	k ^= (k >> 8);
	k += (k << 3);
	k ^= (k >> 15);
	k += ~(k << 27);
	k ^= (k >> 31);
	return (PxUInt)k;
}

// Helper for pointer hashing
template<int size>
PxUInt PxcPointerHash(void* ptr);

template<>
PXD_FORCE_INLINE PxUInt PxcPointerHash<4>(void* ptr)
{
	return PxcHashFunction<uintptr_t>(reinterpret_cast<uintptr_t>(ptr));
}


template<>
PXD_FORCE_INLINE PxUInt PxcPointerHash<8>(void* ptr)
{
	return PxcHashFunction<uintptr_t>(reinterpret_cast<uintptr_t>(ptr));
}

// Hash function for pointers
template<class T>
PXD_FORCE_INLINE PxUInt PxcHashFunction(T* key)
{
	return PxcPointerHash<sizeof(const void*)>(key);
}

#ifdef PXD_COMPILER_VC
#pragma warning(pop)
#endif

#endif
