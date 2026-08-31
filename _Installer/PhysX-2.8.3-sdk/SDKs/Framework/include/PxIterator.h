#ifndef NX_FRAMEWORK_PXITERATOR
#define NX_FRAMEWORK_PXITERATOR

#include "Array.h"
#include "NxPrefetch.h"

template<class T> 
class PxIterator
{
private:
	T* firstData;
	T* lastData;
	T* current;
public:
	PxIterator(T* firstData, T* lastData) 
		: firstData(firstData), lastData(lastData), current(firstData) 
	{
	}

	PxIterator<T>& operator++()
	{
		++current;
		return *this;
	}

	T* operator*()
	{
		return current;
	}

	T* begin() const
	{
		return firstData;
	}

	T* end() const
	{
		return lastData;
	}

	void skip(NxU32 steps)
	{
		current += steps;
		if (current>end())
			current = end();
	}

	T* operator*() const 
	{ 
		return current; 
	}

	void reset() 
	{ 
		current = firstData; 
	}

	NxU32 size() const 
	{ 
		return (NxU32)(lastData-firstData); 		
	}

	void copy(void* destination) const 
	{ 
		if(size()>0)
			memcpy(destination, firstData, sizeof(T)*size()); 
	}

	
	NX_INLINE void prefetchContents1(NxU32 prefOffset)
	{
		if((current+prefOffset) < lastData)
			NxPrefetch(current[prefOffset], 0);
	}
	NX_INLINE void prefetchContents2(NxU32 prefOffset)
	{
		if((current+prefOffset) < lastData)
			NxPrefetch2(current[prefOffset]);
	}
	NX_INLINE void prefetchContents3(NxU32 prefOffset)
	{
		if((current+prefOffset) < lastData)
			NxPrefetch3(current[prefOffset]);
	}
	NX_INLINE void prefetchContents4(NxU32 prefOffset)
	{
		if((current+prefOffset) < lastData)
			NxPrefetch4(current[prefOffset]);
	}
	
	NX_INLINE void prefetchContents5(NxU32 prefOffset)
	{
		if((current+prefOffset) < lastData)
			NxPrefetch5(current[prefOffset]);
	}	

};


template<class T> 
class PxBackwardIterator
{
private:
	T* firstData;
	T* lastData;
	T* current;
public:
	PxBackwardIterator(T* firstData, T* lastData) 
		: firstData(firstData), lastData(lastData), current(firstData) 
	{
	}

	PxBackwardIterator(const PxIterator<T>& fwd) 
		: firstData(fwd.end()-1), lastData(fwd.begin()-1), current(fwd.end()-1) 
	{
	}

	PxBackwardIterator<T>& operator++()
	{
		--current;
		return *this;
	}

	T* operator*()
	{
		return current;
	}

	T* first() const
	{
		return firstData;
	}

	T* end() const
	{
		return lastData;
	}

	T* operator*() const 
	{ 
		return current; 
	}

	void reset() 
	{ 
		current = firstData; 
	}

	NxU32 size() const 
	{ 
		return (NxU32)(firstData-lastData); 		
	}
};

#endif //NX_FRAMEWORK_PXITERATOR
