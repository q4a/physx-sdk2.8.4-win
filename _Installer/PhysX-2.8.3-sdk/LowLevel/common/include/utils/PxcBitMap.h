#ifndef PXC_BITMAP_H
#define PXC_BITMAP_H

#include "px_config.h"
#include "PxcFastMemory.h"
#include "PxcHelperFunction.h"
#include "PxnError.h"

#include "PxcNoCopy.h"

/*!
Hold a bitmap with operations to set,clear or check given bit.

We inhibit copy to prevent unintentional copies. If a copy is desired copy() should be used or
alternativly a copy constructor implimented.
*/
class PxcBitMap : public PxcNoCopy
{
public:
	PxcBitMap();
	~PxcBitMap();

	PXD_FORCE_INLINE void			set(PxU32 index)
	{
		extend(index);
		mMap[index>>5] |= 1<<(index&31);
	}

	PXD_FORCE_INLINE bool			checkSet(PxU32 index)
	{
		extend(index);
		PxU32 mask = 1<<(index&31);
		index >>= 5;
		bool result = (mMap[index] & mask)!=0;	// PT: performance warning!

		mMap[index] |= mask;

		return result;
	}
	
	PXD_FORCE_INLINE void			clear(PxU32 index)
	{
		extend(index);
		mMap[index>>5] &= ~(1<<(index&31));
	}

	PXD_FORCE_INLINE bool			check(PxU32 index) const
	{
		if((index>>5) >= mWordCount)
			return false;

		return (mMap[index>>5]&(1<<(index&31)))!=0;	// PT: performance warning!
	}

	PXD_INLINE bool			checkAllSet(PxU32 index, PxU32 count) const
	{
		PxU32 setCount = 0;
		for(PxU32 i=0; i<count; i++)
		{
			if(check(index+i))
				setCount++;
				
		}

		return (setCount == count);
	}

	PXD_INLINE bool			checkAllClear(PxU32 index, PxU32 count) const
	{
		PxU32 setCount = 0;
		for(PxU32 i=0; i<count; i++)
		{
			if(check(index+i))
				setCount++;
				
		}

		return (setCount == 0);
	}

	// Special optimized versions, when you _know_ your index is in range
	PXD_FORCE_INLINE void			setFast(PxU32 index)
	{
		PXN_ASSERT(index<mWordCount*32);
		mMap[index>>5] |= 1<<(index&31);
	}

	PXD_FORCE_INLINE void			clearFast(PxU32 index)
	{
		PXN_ASSERT(index<mWordCount*32);
		mMap[index>>5] &= ~(1<<(index&31));
	}

	PXD_FORCE_INLINE bool			checkFast(PxU32 index) const
	{
		PXN_ASSERT(index<mWordCount*32);
		return (mMap[index>>5]&(1<<(index&31)))!=0;	// PT: performance warning!
	}
	
	PXD_FORCE_INLINE PxU32			getNibbleFast(PxU32 nibIndex) const
	{
		PxU32 bitIndex = nibIndex << 2;

		PXN_ASSERT(bitIndex < mWordCount*32);

		return (mMap[bitIndex >> 5] >> (bitIndex & 31)) & 0xf;
	}

	PXD_FORCE_INLINE void			andNibbleFast(PxU32 nibIndex, PxU32 mask)
	{
		//TODO: there has to be a faster way...
		PxU32 bitIndex = nibIndex << 2;
		PxU32 shift = (bitIndex & 31);
		PxU32 nibMask = 0xf << shift;
		
		PXN_ASSERT(bitIndex < mWordCount*32);

		mMap[bitIndex >> 5] &= ((mask << shift) | ~nibMask);
	}

	PXD_FORCE_INLINE void			orNibbleFast(PxU32 nibIndex, PxU32 mask)
	{
		PXN_ASSERT(!(mask & ~0xf)); //check extra bits are not set

		PxU32 bitIndex = nibIndex << 2;
		PxU32 shift = bitIndex & 31;

		PXN_ASSERT(bitIndex < mWordCount*32);

		mMap[bitIndex >> 5] |= (mask << shift);
	}

	PXD_INLINE PxU32 getSetBitCount()		const
	{
		PxU32 bitCount = 0;

		// NOTE: we can probably do this faster, since the last steps in PxcBitCount32 can be defered to
		// the end of the seq. + 64/128bits at a time + native bit counting instructions(360 is fast non micro code).

		for(PxUInt i=0; i<mWordCount; i++)
			bitCount += PxcBitCount32(mMap[i]);

		return bitCount;
	}

	
	void			copy(const PxcBitMap &a);

	void			orBitmap(const PxcBitMap &bmap);
	void			xorBitmaps(const PxcBitMap &bmapA, const PxcBitMap &bmapB);
	void			andNotBitmaps(const PxcBitMap &bmapA, const PxcBitMap &bmapB);
	void			norBitmaps(const PxcBitMap &bmapA, const PxcBitMap &bmapB);

	PxU32			getHighestSetBit()		const;

	void			reset();
	void			reset(PxU32 index);
	
	const PxU32*	getWords()				const	{	return mMap;		}
	PxU32*			getWords()						{	return mMap;		}
	PxU32			getWordCount()			const	{	return mWordCount;	}

private:
	PxU32*			mMap;	//one bit per index
	PxU32			mWordCount;

	void			extend(PxU32 index);
	friend class PxcBitMapIterator;
	friend class PxcBitMapIteratorOld;
	friend class PxcBitMapIteratorOld2;
};


/*!
Iterate over indices in a bitmap

This is the old iterator for comparison. Use the one below.
TODO: remove eventually.
*/
class PxcBitMapIteratorOld
{
public:
	static const PxU32 DONE = 0xffffffff;

	PXD_FORCE_INLINE PxcBitMapIteratorOld(const PxcBitMap &map)
		: mBitMap(map)
	{
		reset();
	}

	PXD_FORCE_INLINE PxU32		getNext()
	{
		for(; i < mBitMap.mWordCount; i++)
		{
			PxU32 cachedMap = mBitMap.mMap[i];
			if(!cachedMap) 
			{
				j = DONE; //restart
				continue;
			}
			j++;
			
			PxU32 tmp = cachedMap >> j;//move variable shift out of the loop(microcoded instruction on 360)
			
			for(;j < 32; j++)
			{
				
				if(tmp & 0x01)
					return (i<<5)+j;
				else if(!tmp)
					break;

				tmp = tmp >> 1;
			}

			j = DONE;
		}
		return DONE;
	}

	PXD_FORCE_INLINE void		reset()
	{
		i = 0;
		j = DONE;
	}
private:
	PxU32 i, j;
	const PxcBitMap& mBitMap;
};

/*!
Iterate over indices in a bitmap

This iterator is good because it compltely removes the need for variable length shifts.
*/
class PxcBitMapIteratorOld2
{
public:
	static const PxU32 DONE = 0xffffffff;

	PXD_FORCE_INLINE PxcBitMapIteratorOld2(const PxcBitMap &map)
		: mBitMap(map)
	{
		reset();
	}

	PXD_FORCE_INLINE PxU32		getNext()
	{
		do
		{
			if(cachedBits)//perhaps we should remove this if and let the below loop drop out.
			{
				for(;j < 32; j++)
				{
					if(cachedBits & 0x01)
					{
						PxU32 rv = (i<<5)+j;
						
						//move to next bit.
						j++;
						cachedBits = cachedBits >> 1;

						return rv;
					}
					else if(!cachedBits)
						break;

					cachedBits = cachedBits >> 1;
				}
			}

			i++;
			if(i >= mBitMap.mWordCount)
			{
				return DONE;
			}

			j = 0;
			cachedBits = mBitMap.mMap[i];
			
		}while(true);

		return DONE;
	}

	PXD_FORCE_INLINE void		reset()
	{
		i = 0;
		j = 0;
		if(mBitMap.mWordCount>0)
			cachedBits = mBitMap.mMap[0];
		else
			cachedBits = 0;

	}
private:
	PxU32 i, j, cachedBits;
	const PxcBitMap &mBitMap;
};

/*!
Iterate over indices in a bitmap

This iterator is good because it finds the set bit without looping over the cached bits upto 31 times.
However it does require a variable shift.
*/
class PxcBitMapIterator
{
public:
	static const PxU32 DONE = 0xffffffff;

	PXD_FORCE_INLINE PxcBitMapIterator(const PxcBitMap &map)
		:
#ifdef _DEBUG
	oldIterator(map),
#endif
		mBitMap(map)
	{
		reset();
	}

	PXD_FORCE_INLINE PxU32		getNext()
	{
#ifdef _DEBUG
		PxU32 checkIndex = oldIterator.getNext();
#endif
		do
		{
			if(cachedBits)
			{
				PxU32 bitIndex = PxcLowestSetBit(cachedBits);
				
				cachedBits = cachedBits >> bitIndex;//shifting by 32 is undefined.
				j += bitIndex;
				
				PxU32 rv = (i<<5) + j;

#ifdef _DEBUG
				PXN_ASSERT(checkIndex == rv);
#endif
				//move to next
				cachedBits = cachedBits >> 1;
				j++;

				return rv;
			}

			i++;
			if(i >= mBitMap.mWordCount)
			{
#ifdef _DEBUG
				PXN_ASSERT(checkIndex == DONE);
#endif
				return DONE;
			}

			j = 0;
			cachedBits = mBitMap.mMap[i];
			
		}while(true);

		return DONE;
	}

	PXD_FORCE_INLINE void		reset()
	{
#ifdef _DEBUG
		oldIterator.reset();
#endif
		i = 0;
		j = 0;
		if(mBitMap.mWordCount>0)
			cachedBits = mBitMap.mMap[0];
		else
			cachedBits = 0;

	}
private:
	PxU32 i, j, cachedBits;
	const PxcBitMap &mBitMap;
#ifdef _DEBUG
	PxcBitMapIteratorOld oldIterator;
#endif
};

#endif
