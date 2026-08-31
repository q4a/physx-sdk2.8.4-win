#ifndef PXC_UNIONFIND_H
#define PXC_UNIONFIND_H

#include "px_config.h"

class PxcUnionFind
{
public:
	PxcUnionFind()
		: mEntryParent(0), mMaxEntries(0)
	{
	}

	~PxcUnionFind();

	//! Initialize for a find using max numEntries
	void init(PxU32 numEntries);

	//! Find the root for given parent
	PXD_INLINE PxU32 find(PxU32 index)
	{
		if(mEntryParent[index] != index)
			return mEntryParent[index] = find(mEntryParent[index]);

		return mEntryParent[index];
	}

	//! Union two subsets
	PXD_INLINE void join(PxU32 index1, PxU32 index2)
	{
		PxU32 p1 = find(index1);
		PxU32 p2 = find(index2);

		if(p1 != p2)
		{
			if(p1 < p2)
			{
				//make p2 child of p1
				mEntryParent[p2] = p1;
				//mGroupEntryCount[p1]++;
			}
			else
			{
				//make p1 child of p2
				mEntryParent[p1] = p2;
				//mGroupEntryCount[p2]++;
			}
		}
	}



private:
	PxU32* mEntryParent;
	//PxU32 *mGroupEntryCount;
	PxU32 mMaxEntries;
};


#endif
