#ifndef PXC_ARRAY_H
#define PXC_ARRAY_H

#include "px_config.h"
#include "PxnMemory.h"

#include <new>

#include "PxcFastMemory.h"
#include "PxcStack.h"

template<class T, class C> class PxcArrayQuickSort;

/*!
Templated array.

An array is a sequencial container.
*/
template<class T>
class PxcArray
{
public:
	/*!
	Default array constructor.
	Initialize an empty array
	*/
	PXD_INLINE PxcArray() : 
		mData(0), mCapacity(0), mSize(0)
	{}

	/*!
	Initialize array with given length
	*/
	PXD_INLINE  explicit PxcArray(PxUInt capacity) : 
		mData(0), mCapacity(0), mSize(0)
	{
		grow(capacity);
	}

	/*!
	Copy-constructor. Copy all entries from other array
	*/
	PXD_INLINE PxcArray(const PxcArray<T>& t) :
		mData(0), mCapacity(t.mCapacity), mSize(t.mSize)
	{
		if(mCapacity > 0)
		{
			mData = reinterpret_cast<T*> (PXN_MALLOC(sizeof(T)*mCapacity));
			for(PxUInt i = 0; i < mCapacity; i++)
				new (mData+i) T(t.mData[i]);
		}
	}

	/*!
	Default destructor
	*/
	PXD_INLINE ~PxcArray()
	{
		for(PxUInt i = 0; i < mCapacity; i++)
			mData[i].~T();

		PXN_FREE(mData);
	}

	/*!
	Assignment operator.
	Copy content (deep-copy)
	*/
	PXD_INLINE const PxcArray<T>& operator= (const PxcArray<T>& t)
	{
		if(&t == this)
			return *this;

		mSize = t.mSize;
		if(mCapacity < t.mCapacity)
		{
			mCapacity = t.mCapacity;
			PXN_FREE(mData);
			mData = reinterpret_cast<T*> (PXN_MALLOC(sizeof(T)*mCapacity));
			//Construct unused ones
			for(PxUInt i = mSize; i < mCapacity; i++)
				new (mData+i) T;
		}

		if(mSize > 0)
		{
			PxcMemcpy(mData, t.mData, mSize*sizeof(T)); // misspelled?
		}

		return *this;
	}

	/*!
	Return an element from this array. Operation is O(1).
	\param i
	The index of the element that will be returned.
	\return
	Element i in the array.
	*/
	PXD_INLINE const T& get(PxUInt i) const 
	{
		return mData[i];
	}

	/*!
	Return an element from this array. Operation is O(1).
	\param i
	The index of the element that will be returned.
	\return
	Element i in the array.
	*/
	PXD_INLINE T& get(PxUInt i)
	{
		return mData[i];
	}

	/*!
	Array indexing operator.
	\param i
	The index of the element that will be returned.
	\return
	The element i in the array.
	*/
	PXD_INLINE const T& operator[] (PxUInt i) const 
	{
		return get(i);
	}

	/*!
	Array indexing operator.
	\param i
	The index of the element that will be returned.
	\return
	The element i in the array.
	*/
	PXD_INLINE T& operator[] (PxUInt i) 
	{
		return get(i);
	}

	/*!
	Returns the plain array representation.
	\return
	The sets representation.
	*/
	PXD_INLINE const T* getPtr() const 
	{
		return mData;
	}

	/*!
	Returns the plain array representation.
	\return
	The sets representation.
	*/
	PXD_INLINE T* getPtr() 
	{
		return mData;
	}

	/*!
	Returns the number of entries in the array. This can, and probably will,
	differ from the array size.
	\return
	The number of of entries in the array.
	*/
	PXD_INLINE PxUInt getSize() const 
	{
		return mSize;
	}

	/*!
	Clears the array.
	*/
	PXD_INLINE void clear() 
	{
		mSize=0;
	}

	/////////////////////////////////////////////////////////////////////////
	/*!
	Adds one element to the end of the array. Operation is O(1).
	\param a
	The element that will be added to this array.
	\param index
	Optional pointer to index where element is inserted.
	*/
	/////////////////////////////////////////////////////////////////////////
	PXD_INLINE T& insert(const T& a, PxUInt* index = NULL) 
	{
		if(mCapacity<=mSize) 
			grow(mCapacity*2+1);
		
		if (index) 
			*index = mSize;

		new((void*)(mData + mSize)) T(a);

		return mData[mSize++];
	}

	/////////////////////////////////////////////////////////////////////////
	/*!
	Returns the element at the end of the array. Only legal if the array is non-empty.
	*/
	/////////////////////////////////////////////////////////////////////////
	PXD_INLINE T& pop() 
	{
		return mData[--mSize];
	}


	/////////////////////////////////////////////////////////////////////////
	/*!
	Construct one element at the end of the array. Operation is O(1).
	*/
	/////////////////////////////////////////////////////////////////////////
	PXD_INLINE T& insert()
	{
		if(mCapacity<=mSize) 
			grow(mCapacity*2+1);

		return mData[mSize++];
	}

	/////////////////////////////////////////////////////////////////////////
	/*!
	Subtracts the element on position i from the array and replace it with
	the last element.
	Operation is O(1)
	\param i
	The position of the element that will be subtracted from this array.
	\return
	The element that was removed.
	*/
	/////////////////////////////////////////////////////////////////////////
	PXD_INLINE T removeFast(PxUInt i) 
	{
		T tmp = mData[i];
		mData[i] = mData[--mSize];
		return tmp;
	}

	// We need a renamed version or it clashes when we have an array of PxUInt
	PXD_INLINE T removeIndexFast(PxUInt i) 
	{
		T tmp = mData[i];
		mData[i] = mData[--mSize];
		return tmp;
	}

	/////////////////////////////////////////////////////////////////////////
	/*!
	Subtracts the element on position i from the array. Shift the entire
	array one step.
	Operation is O(n)
	\param i
	The position of the element that will be subtracted from this array.
	\return
	The element that was removed.
	*/
	/////////////////////////////////////////////////////////////////////////
	PXD_INLINE T remove(PxUInt i) 
	{
		T tmp = mData[i];
		
		PxcMemMove(&mData[i],&mData[i+1],(mSize-i-1)*sizeof(T));
		
		mSize--;

		return tmp;
	}


	/////////////////////////////////////////////////////////////////////////
	/*!
	Remove all occurrences of element a from the array.
	Operation is O(n)
	\param a
	The element that will be subtracted from this array.
	\return
	True if one or more elements were removed, false otherwise.
	*/
	/////////////////////////////////////////////////////////////////////////
	PXD_FORCE_INLINE bool removeFast(const T& a) 
	{
		return removeElementFast(a);
	}

	//we need a renamed version or it conflicts with the index version for an int array(grrr).
	PXD_INLINE bool removeElementFast(const T& a)
	{
		PxU32 i;
		PxU32 low=0;

		bool ret=false;

		do{
			for(i = low; i < mSize; i++)
			{
				if(mData[i] == a) 
					break;
			}

			if(i < mSize)
			{
				removeFast(i);
				ret=true;
				low=i;
			}
		} while(i<mSize);

		return ret;
	}

	/////////////////////////////////////////////////////////////////////////
	/*!
	Remove all occurrences of element a from the array.
	Operation is O(n*n)!
	\param a
	The element that will be subtracted from this array.
	\return
	True if one or more elements were removed, false otherwise.
	*/
	/////////////////////////////////////////////////////////////////////////
	PXD_INLINE bool remove(const T& a) 
	{
		PxU32 i;
		PxU32 low=0;

		bool ret=false;

		do{
			for(i = low; i < mSize; i++)
			{
				if(mData[i] == a) 
					break;
			}

			if(i < mSize)
			{
				remove(i);
				ret=true;
				low=i;
			}
		} while(i<mSize);

		return ret;
	}

	/////////////////////////////////////////////////////////////////////////
	/*!
	Given an element in the array, determine its index
	*/
	/////////////////////////////////////////////////////////////////////////
	PXD_INLINE PxUInt getIndex(const T& a)
	{
		return (PxUInt)(&a - mData);
	}

	//////////////////////////////////////////////////////////////////////////
	/*!
	Resize array
	*/
	//////////////////////////////////////////////////////////////////////////
	PXD_INLINE void setSize(const PxUInt size)
	{
		if(size >= mCapacity)
			grow(size);

		mSize = size;
	}

	//////////////////////////////////////////////////////////////////////////
	/*!
	Resize array, and set new elements to given value
	*/
	//////////////////////////////////////////////////////////////////////////
	PXD_INLINE void setSize(const PxUInt size, const T& a)
	{
		if(size >= mCapacity)
			grow(size);

		//Initialize new elements
		for(PxUInt i = mSize; i < size; i++)
			mData[i] = a;

		mSize = size;
	}

	//////////////////////////////////////////////////////////////////////////
	/*!
	Ensure that the array has at least size capacity.
	*/
	//////////////////////////////////////////////////////////////////////////
	PXD_INLINE void reserve(const PxUInt size)
	{
		if(size >= mCapacity)
			grow(size);
	}

	//////////////////////////////////////////////////////////////////////////
	/*!
	Query the capacity(allocated mem) for the array.
	*/
	//////////////////////////////////////////////////////////////////////////
	PXD_FORCE_INLINE PxUInt getCapacity()
	{
		return mCapacity;
	}

private:
	/*!
	Resizes the available memory for the array.

	\param capacity
	The number of entries that the set should be able to hold.
	*/	
	PXD_INLINE void grow(PxUInt capacity) 
	{
		if(this->mCapacity < capacity) {
			T* newData = reinterpret_cast<T*>(PXN_MALLOC(capacity*sizeof(T)));
			PXN_ASSERT(newData != NULL);

			// dsequeira: these casts are required as a workaround for the 2.0 beta compiler.
			// bug (NVBugs 413813)
			PxcMemcpy(static_cast<void *>(newData), static_cast<void *>(mData), mSize*sizeof(T));

			PXN_FREE(mData);
			mData = newData;

			//initialize new entries
			for(PxUInt i = mSize; i < capacity; i++)
			{
				new ((void*)(mData+i)) T;
			}
			mCapacity = capacity;
		}
	}

	T*			mData;
	PxUInt		mCapacity;
	PxUInt		mSize;

	template<class, class> friend class PxcArrayQuickSort;
};

template<class T, class C> class PxcArrayQuickSort
{
	
public:
	PxcArrayQuickSort()
	{
	}

	void sort(PxcArray<T> &array, C &comparitor)
	{
		sortInternal2(array.mData, 0, array.getSize()-1, comparitor);
#if 0
#ifdef _DEBUG

		//TEMP: check the sort worked.
		for(PxU32 i=1; i<array.getSize(); i++)
			PXN_ASSERT(comparitor.lessEqual(array[i-1], array[i]));
#endif
#endif
	}

private:

	//NOTE, this should be >= 3 since we need 3 elements when computing the median of 3.
	static const PxU32 SMALL_SORT_CUTOFF = 5;

	// This is a really really stupid quicksort for verification.
	// dsequeira: No kidding - what's wrong with the STL convention of just using < ??

	void sortInternal(T *array, PxI32 start, PxI32 end, C &comparator)
	{
		PxI32 i = start;
		PxI32 k = end;

		if((end - start) >= 1)
		{
			T pivot = array[start];

			while(k > i)
			{
				while(comparator.lessEqual(array[i], pivot) && (i <= end) && (k > i))
					i++;
				while(comparator.greater(array[k], pivot) && (k >= start) && (k >= i))
					k--;
				if(k > i)
					PxcSwap(array[i], array[k]);
			}

			PxcSwap(array[start], array[k]);

			sortInternal(array, start, k - 1, comparator);
			sortInternal(array, k + 1, end, comparator);
		}
	}



	PXD_FORCE_INLINE void median3(T *array, PxI32 first, PxI32 last, C &comparitor)
	{
		/*
		This creates sentinals because we know there is an element at the start
		lower(or equal) than the pivot and an element at the end greater(or equal) than the pivot.
		Plus the median of 3 reduces the chance of degenerate behavour.
		*/

		PxI32 mid = (first + last)/2;

		if(comparitor.greater(array[first], array[mid]))
			PxcSwap(array[first], array[mid]);

		if(comparitor.greater(array[first], array[last]))
			PxcSwap(array[first], array[last]);

		if(comparitor.greater(array[mid], array[last]))
			PxcSwap(array[mid], array[last]);

		//keep the pivot at last-1
        PxcSwap(array[mid], array[last-1]);
	}

	PXD_FORCE_INLINE PxI32 partition(T *array, PxI32 first, PxI32 last, C &comparitor)
	{
		//median of 3

		median3(array, first, last, comparitor);
		
		/*
		WARNING: using the line:

		T partValue = array[last-1];

		and changing the scan loops to:
		
		while(comparitor.greater(partValue, array[++i]));
		while(comparitor.greater(array[--j], partValue);

		triggers a compiler optimizer bug on xenon where it stores a double to the stack for partValue
		then loads it as a single...:-(
		*/

		PxI32 i = first; //we know first is less than pivot(but i gets pre incremented) 
		PxI32 j = last - 1;//pivot is in last-1 (but j gets pre decremented)

		while(true)
		{
			//scan
			while(comparitor.greater(array[last-1], array[++i]));
			while(comparitor.greater(array[--j], array[last-1]));
			
			if(i>=j) break;

			PXN_ASSERT(i<=last);
			PXN_ASSERT(j>=first);
			PxcSwap(array[i], array[j]);
		}
		//put the pivot in place

		PXN_ASSERT(i<=last);
		PXN_ASSERT((last-1)>=first);
		PxcSwap(array[i], array[last-1]);

#if 0
#ifdef _DEBUG
		//check for correct partitioning.
		for(PxI32 k=first; k<i; k++)
			PXN_ASSERT(comparitor.greaterEqual(array[i], array[k]));
		for(PxI32 k=i+1; k<=last; k++)
			PXN_ASSERT(comparitor.greaterEqual(array[k], array[i]));
#endif
#endif
		return i;
	}

	PXD_FORCE_INLINE void smallSort(T *array, PxI32 first, PxI32 last, C &comparitor)
	{
		//selection sort
		for(PxI32 i=first; i<last; i++)
		{
			PxI32 m = i;
			for(PxI32 j=i+1; j<=last; j++)
				if(comparitor.greater(array[m], array[j])) m = j;

			PxcSwap(array[m], array[i]);
		}
	}

	PXD_FORCE_INLINE void sortInternal2(T *array, PxI32 first, PxI32 last, C &comparitor)
	{

		PxI32 startFirst = first;
		PxI32 startLast = last;

		//TODO: pre reserve mStack space so we dont have to check for resize on every push.
		if(last > first)
		{
			while(true)
			{
				while(last > first)
				{
					if((last-first) < SMALL_SORT_CUTOFF)
					{
						PXN_ASSERT(first>=startFirst);
						PXN_ASSERT(last<=startLast);
		
						//Selection would be good for consoles.
						//cmpxhg could be used on PC.
						//(although maybe bus locks would be a problem).
						//insertion is good for nearly sorted though.

						smallSort(array, first, last, comparitor);
						break;
					}
					else
					{
						PXN_ASSERT(first>=startFirst);
						PXN_ASSERT(last<=startLast);

						PxI32 partIndex = partition(array, first, last, comparitor);

						//push the large sublist
						if((partIndex - first) > (last - partIndex))
						{
							mStack.push(first);
							mStack.push(partIndex-1);
							first = partIndex + 1;
						}
						else
						{
							mStack.push(partIndex+1);
							mStack.push(last);
							last = partIndex - 1;
						}
					}
				}

				if(mStack.empty())
					break;

				last = mStack.pop();
				first = mStack.pop();
			}
		}

	}

	//TODO: allow sharing of stacks between sorters. Plus allow fixed sized stacks(alloca).
	PxcStack<PxI32, 32> mStack;
};


#endif
