#ifndef PXC_BITFIELD_H
#define PXC_BITFIELD_H

#include "px_config.h"

/*!
Hold a bit-field with operations to set, check and clear individual bits
*/
class PxcBitField
{
public:
	PxcBitField()
		: mData(0)
	{}


	PXD_INLINE void			set(PxU32 index)
	{
		mData |= (1<<index);
	}

	PXD_INLINE bool			checkSet(PxU32 index)
	{
		PxU32 mask = (1<<index);
		bool result = (mData & mask)!=0;	// PT: performance warning!

		mData |= mask;

		return result;
	}

	PXD_INLINE void			clear(PxU32 index)
	{
		mData &= ~(1<<index);
	}

	PXD_INLINE bool			check(PxU32 index) const
	{
		return (mData & (1<<index))!=0;	// PT: performance warning!
	} 

	PXD_INLINE void			reset()
	{
		mData = 0;
	}

private:
	PxU32 mData;
};

#endif
