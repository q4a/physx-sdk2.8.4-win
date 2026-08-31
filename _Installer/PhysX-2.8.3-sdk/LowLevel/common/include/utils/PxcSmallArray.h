#ifndef PXC_SMALLARRAY_H
#define PXC_SMALLARRAY_H

#include "px_config.h"
#include "PxnMemory.h"

#include "PxcFastMemory.h"
#include "PxcPoolMalloc.h"
#include "PxcBitMap.h"


class PxcSmallIndexArray
{
public:

	PxcSmallIndexArray()
	{
		mData = NULL;
		mSize = 0xffFF;
		mCapacity = 0xffFF;
	}

	~PxcSmallIndexArray()
	{
		if(mData != NULL)
			PxcPoolFree(mData, sizeof(PxU16) * mCapacity);
	}

	PXD_FORCE_INLINE void insert(PxU16 val)
	{
		//optimization to avoid cache miss on 1 or 2 entries.
		if(mData == NULL)
		{
			if(mSize == 0xffFF)	mSize = val;
			else if(mCapacity == 0xffFF) mCapacity = val;
			else
			{
				//time for an array
				mData = (PxU16 *)PxcPoolMalloc(sizeof(PxU16) * 4);

				mData[0] = mSize;
				mData[1] = mCapacity;
				mData[2] = val;

				mSize = 3;
				mCapacity = 4;
			}

			return;
		}
		else
		{
			if(mSize >= mCapacity)
				grow();
			mData[mSize++] = val;
		}
	}

	PXD_FORCE_INLINE void insert(const PxcSmallIndexArray &array)
	{
		//OPT: we already know how many elements we are inserting.
		if(array.mData == NULL)
		{
			if(array.mSize != 0xffFF) insert(array.mSize);
			if(array.mCapacity != 0xffFF) insert(array.mCapacity);
		}
		else
		{
			for(PxU32 i=0; i<array.mSize; i++)
				insert(array.mData[i]);
		}
	}

	PXD_FORCE_INLINE void removeElement(PxU16 val)
	{
		if(mData == NULL)
		{
			if(mSize == val) mSize = 0xffFF;
			else if(mCapacity == val) mCapacity = 0xffFF;
		}
		else
		{
			//OPT: check size and collapse to using only mSize/mCapacity when we are <=2 entries.
			for(PxU32 i=0; i<mSize; i++)
			{
				if(mData[i] == val)
				{
					PxcSwap(mData[i], mData[mSize-1]);
					mSize--;
					return;
				}
			}
		}
	}

	PXD_FORCE_INLINE void removeElements(const PxcSmallIndexArray &array)
	{
		//OPT: we already know how many elements we are inserting.
		if(array.mData == NULL)
		{
			if(array.mSize != 0xffFF) removeElement(array.mSize);
			if(array.mCapacity != 0xffFF) removeElement(array.mCapacity);
		}
		else
		{
			for(PxU32 i=0; i<array.mSize; i++)
				removeElement(array.mData[i]);
		}
	}


	PXD_FORCE_INLINE void clear()
	{
		if(mData == NULL)
		{
			mSize = 0xffFF;
			mCapacity = 0xffFF;
		}
		else
			mSize = 0;
	}

	PXD_FORCE_INLINE void setBitMapBits(PxcBitMap &bmap)
	{
		if(mData == NULL)
		{
			if(mSize != 0xffFF) bmap.set(mSize);
			if(mCapacity != 0xffFF) bmap.set(mCapacity);
		}
		else
		{
			//OPT: check bitmap size and use setFast()
			for(PxU32 i=0; i<mSize; i++)
				bmap.set(mData[i]);
		}
	}

private:

	PXD_INLINE void grow()
	{
		PxU16 oldCapacity = mCapacity;

		mCapacity *= 2;
		PxU16 *newData = (PxU16 *)PxcPoolMalloc(mCapacity * sizeof(PxU16));

		PxcMemcpy(newData, mData, sizeof(PxU16) * mSize);
		
		PxcPoolFree(mData, sizeof(PxU16) * oldCapacity);
		mData = newData;
	}

	PxU16 *mData;
	PxU16 mSize;
	PxU16 mCapacity;

};

#endif

