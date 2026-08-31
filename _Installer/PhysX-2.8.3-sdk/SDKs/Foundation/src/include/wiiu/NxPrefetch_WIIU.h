#ifndef NX_FOUNDATION_PREFETCH_WIIU
#define NX_FOUNDATION_PREFETCH_WIIU

NX_INLINE void NxPrefetch(register void* ptr, register NxU32 offset)
{
	//DCTouchRange( ptr, offset);
}

// Prefetch utilities.
/////////////////////////////////////////////////////////////////////////////////////

NX_INLINE void NxPrefetch2(void *ptr)
{
	NxPrefetch(ptr, 0);
	NxPrefetch(ptr, 128);
}

NX_INLINE void NxPrefetch3(void *ptr)
{
	NxPrefetch(ptr, 0);
	NxPrefetch(ptr, 128);
	NxPrefetch(ptr, 256);
}

NX_INLINE void NxPrefetch4(void *ptr)
{
	NxPrefetch(ptr, 0);
	NxPrefetch(ptr, 128);
	NxPrefetch(ptr, 256);
	NxPrefetch(ptr, 384);
}

NX_INLINE void NxPrefetch5(void *ptr)
{
	NxPrefetch(ptr, 0);
	NxPrefetch(ptr, 128);
	NxPrefetch(ptr, 256);
	NxPrefetch(ptr, 384);
	NxPrefetch(ptr, 512);
}

template<class T>
NX_INLINE void NxPrefetchContents4(NxFoundation::NxArraySDK<T> &array, NxU32 index)
{
	if(index < array.size())
		NxPrefetch4(array[index]);
}

template<class T>
NX_INLINE void NxPrefetchContents5(NxFoundation::NxArraySDK<T> &array, NxU32 index)
{
	if(index < array.size())
		NxPrefetch5(array[index]);
}

template<class T>
NX_INLINE void NxPrefetchContents4(T** array, NxU32 index, NxU32 count)
{
	if(index < count)
		NxPrefetch4(array[index]);
}

#endif
