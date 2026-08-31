#ifndef PXC_SET_H
#define PXC_SET_H

#include <string.h>
#include "px_config.h"
#include "PxnMemory.h"
#include "PxcFastMemory.h"

/*!
A template class for handling sets. A set is a sorted array where
all elements are unique. The interface to a set is very similar to
the interface of an Array. Please consult the Array documentation for
usage.

A set is optimized for concatenation and intersection with other
sets and it is also a very efficient way of keeping a list of unique
elements.
*/
template <class T> 
class PxcSet
{
public:
	static const int SET_DEFAULT_SIZE = 4;

	/*!
	Default Set constructor.
	Allocates enough room for SET_DEFAULT_SIZE elements.
	*/
	PxcSet() :
		mData(0), mCapacity(SET_DEFAULT_SIZE), mSize(0)
	{
		if (mCapacity > 0)
		{
			mData = reinterpret_cast<T*> (PXN_MALLOC(sizeof(T)*mCapacity));
		}
	}

	/*!
	Set constructor.
	Allocates enough room for size elements. 

	\param size
	Size of set to be defined. 
	*/
	explicit PxcSet(PxUInt size) :
		mData(0), mCapacity(size), mSize(0)
	{
		if (mCapacity > 0)
		{
			mData = reinterpret_cast<T*> (PXN_MALLOC(sizeof(T)*mCapacity));
		}
	}

	/*!
	Set copy constructor.

	\param s
	The Set that will be copied.
	*/
	PxcSet(const PxcSet<T>& s) :
		mData(0), mCapacity(s.mCapacity), mSize(s.mSize)
	{
		if(mCapacity > 0)
		{
			mData = reinterpret_cast<T*> (PXN_MALLOC(sizeof(T)*mCapacity));
			PxcMemcpy(mData, s.mData, mCapacity*sizeof(T));
		}
	}

	/*!
	Default destructor.	
	*/
	~PxcSet() {
		PXN_FREE(mData);
	}

	/*!
	Set assignment operator.
	\param s
	The set that will be copied
	\return
	A reference to the assigned set.
	*/
	PXD_INLINE const PxcSet<T>& operator= (const PxcSet<T>& s) 
	{
		if(&s == this) 
			return *this;

		mSize = s.mSize;
		
		if(mCapacity < mSize) 
		{
			mCapacity = s.mCapacity;
			PXN_FREE(mData);
			mData = reinterpret_cast<T*> (PXN_MALLOC(sizeof(T)*mCapacity));
		}
		if(mSize > 0)
			PxcMemcpy(mData, s.mData, mSize*sizeof(T));

		return *this;
	}

	/*!
	Return an element from this set. Operation is O(1).
	\param i
	The index of the element that will be returned.
	\return
	Element i in the set.
	*/
	PXD_INLINE const T& get(PxUInt i) const 
	{
		return mData[i];
	}

	/*!
	Return an element from this set. Operation is O(1).
	\param i
	The index of the element that will be returned.
	\return
	Element i in the set.
	*/
	PXD_INLINE T& get(PxUInt i)
	{
		return mData[i];
	}

	/*!
	Set indexing operator.
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
	Set indexing operator.
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
	Returns the sets array representation.
	\return
	The sets representation.
	*/
	PXD_INLINE const T* getPtr() const 
	{
		return mData;
	}

	/*!
	Returns the number of entries in the set. This can, and probably will,
	differ from the array size.
	\return
	The number of of entries in the set.
	*/
	PXD_INLINE PxUInt getSize() const 
	{
		return mSize;
	}

	/*!
	Clears the set.
	*/
	PXD_INLINE void clear() 
	{
		mSize=0;
	}

	/*!
	Clear the set and release the memory
	*/
	PXD_INLINE void release()
	{
		mSize = 0;
		mCapacity = 0;
		PXN_FREE(mData);
		mData = 0;
	}


	/*!
	Return index of a specific element.

	\param a  
	The element to search for.
	\return
	The index of this element, or -1 if not found.
	*/
	PXD_INLINE PxUInt getIndexOf(const T& a) const
	{
		PxI32 l=0;
		PxI32 r=mSize-1;
		PxI32 i;
		for(;r>=l;) 
		{
			i=(l+r)/2;
			if(mData[i]>a) 
			{
				r=i-1;
			} 
			else 
			{
				if(mData[i]==a) 
				{
					return i;
				}
				l=i+1;
			}
		}
		return -1;
	}

	/*!
	Determine if this set contains a specific element.

	\param a
	The element
	\return
	True if the element is in the set.
	False if the element is not in the set.
	*/
	PXD_INLINE bool contains(const T& a)
	{
		return getIndexOf(a)!=-1;
	}

	/*!
	Insert a element to the set if it does not already exists within it. 
	If this Set is full it will be expanded to hold the new element. 

	\param a
	The element that will be added to this set.
	\param index
	Optional pointer to index where element is inserted.
	\return
	True if the element was inserted.
	False if the element was already in the set.
	*/
	PXD_INLINE bool insert(const T& a, PxUInt* index = NULL) 
	{
		int l=0;
		int r=mSize-1;
		int i;
		for(;r>=l;) 
		{
			i=(l+r)/2;
			if(mData[i]>a)
			{
				r=i-1;
			} 
			else 
			{
				if(mData[i]==a)
				{
					if (index) 
						*index=i;

					return false;
				}
				l=i+1;
			}
		}

		//move the elements [l count-1] to [l+1 count]
		if(mCapacity<=mSize) 
			grow(1+mCapacity*2);

		if (mSize-l > 0) 
			PxcMemMove(&mData[l+1],&mData[l],(mSize-l)*sizeof(T));

		mData[l]=a;
		mSize++;

		if (index)
			*index=l;

		return true;
	}

	/*!
	Remove one element from the set, if it exists.
	\param a
	The element that will be subtracted from this set.
	\return
	True if the element was removed. False if no 
	element was found.
	*/
	PXD_INLINE bool remove(const T& a) 
	{
		int l=0;
		int r=mSize-1;
		int i;
		for(;r>=l;) 
		{
			i=(l+r)/2;
			if(mData[i]>a) 
			{
				r=i-1;
			}
			else 
			{
				if(mData[i]==a) 
				{
					PxcMemMove(&mData[i],&mData[i+1],(mSize-i-1)*sizeof(T));
					mSize--;
					return true;
				}
				l=i+1;
			}
		}
		return false;
	}

	/*!
	Subtracts the element on position i from the set.
	Operation is O(n). Fastest removal on high index elements.
	\param i
	The position of the element that will be subtracted from this set.
	\return
	The element that was removed.
	*/
	PXD_INLINE T removeElement(PxUInt i) 
	{
		T tmp = mData[i];
		PxcMemMove(mData+i,mData+i+1,(mSize-i-1)*sizeof(T));
		mSize--;
		return tmp;
	}

	/*!
	Insert a given set to this set, more formaly called a union between the sets.
	\param s
	The set that will be added to this one.
	*/
	//PXD_INLINE void insert(const PxcSet& other) 
	//{
	//	if(other.getSize() == 0)
	//		return;

	//	//Make room for this set + this set and other set
	//	grow(2*getSize() + other.getSize());
	//	
	//	T* data2 = data1 + getSize();

	//	PxUInt i=0, j=0, k=0; //index in local, forigin and destination arrays
	//	
	//	if(mSize == 0)
	//	{
	//		*this = other;
	//		return;
	//	}
	//	while(true) 
	//	{
	//		if(mData[i] < other.get(j)) 
	//		{
	//			//local smaller, copy over local
	//			data2[k++] = mData[i++];
	//		} 
	//		else if(other.get(j) < mData[i]) 
	//		{
	//			//other smaller, copy other
	//			data2[k++] = other.get(j++);
	//		} 
	//		else //same, use local
	//		{
	//			data2[k++]=data1[i++];
	//			j++;
	//		}

	//		if(i==mSize) //all local done, copy other
	//		{
	//			PxcMemcpy(data2+k, other.getPtr()+j, (other.getSize()-j)*sizeof(T));
	//			k+=other.getSize()-j;
	//			break;
	//		}
	//		else if(j==other.getSize()) //all other done, copy local
	//		{
	//			PxcMemcpy(data2+k, getPtr()+i, (getSize()-i)*sizeof(T));
	//			k+=getSize()-i;
	//			break;
	//		}
	//	}
	//	PxcMemMove(mData,data2,k*sizeof(T));
	//	mSize = k;
	//}


	/////////////////////////////////////////////////////////////////////////
	/*!
	Remove a given set from this set, more formaly called a difference between
	this set and the given one.
	\param s
	The given set.
	*/
	/////////////////////////////////////////////////////////////////////////
	//PXD_INLINE void remove(const PxcSet& other) 
	//{
	//	if(getSize() == 0 || other.getSize() == 0) 
	//		return;

	//	//Make room for twice this set
	//	grow(2*getSize());
	//	T* data2 = mData + getSize();

	//	PxUInt i=0, j=0, k=0; //index in local, foreign and destination array
	//	
	//	while(true) 
	//	{
	//		if(mData[i] < other.get(j)) 
	//		{
	//			data2[k++]=data1[i++];
	//		} 
	//		else 
	//		{
	//			if(mData[i] == other.get(j)) //same, skip this
	//				i++;

	//			j++;
	//		}

	//		if(i == mSize) 
	//			break; //no more in local

	//		if(j == other.getSize())  //no more in foreign, copy local
	//		{
	//			PxcMemcpy(data2+k, mData+i, (mSize-i)*sizeof(T));
	//			k+=mSize-i;
	//			break;
	//		}
	//	}
	//	PxcMemMove(mData,data2,k*sizeof(T));
	//	mSize = k;
	//}
private:

	/*!
	Resizes the available memory for the array representing the set.

	\param capacity
	The number of entries that the set should be able to hold.
	*/	
	PXD_INLINE void grow(PxUInt capacity) 
	{
		if(this->mCapacity < capacity) {
			T* newData = reinterpret_cast<T*>(PXN_MALLOC(capacity*sizeof(T)));
			PxcMemcpy(newData, mData, mSize*sizeof(T));
			
			PXN_FREE(mData);
			mData = newData;

			//initialize new entries
			for(PxUInt i = mCapacity; i < capacity; i++)
			{
				new ((void*)(mData+i)) T;
			}
			mCapacity = capacity;
		}
	}

	T*					mData;
	PxUInt				mCapacity;
	PxUInt				mSize;
};

#endif
