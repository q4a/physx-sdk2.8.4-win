#ifndef PXC_HASHMAP_H
#define PXC_HASHMAP_H

#include "px_config.h"
#include "px_types.h"
#include "PxcListNode.h"
#include "PxcPool.h"

// It's annoying the functionality of this doesn't overlap with PxcHashSet more. 
// It would be nice to have iterators etc, but there is no time...
//
// Greenspun's Tenth Rule of Programming: any sufficiently complicated C or 
// Fortran program contains an ad hoc informally-specified bug-ridden slow 
// implementation of half of Common Lisp.
// 
// And so also with container libraries and STL... :-/


// WARNING, power of 2 reqd

template<class Key, class Data>
class PxcHashMap
{
public:
	struct Cell
	{
		Key					key;
		Data				data;
		PxcListNode<Cell>	node;
		Cell(const Key &k, const Data &d): key(k), data(d), node(this) {}
	};

	typedef PxcListNode<Cell> CellNode;

	class iterator
	{
	public:
		iterator(PxcHashMap & hashMap) : mHashMap(hashMap),mCurrentBucket(0),mCurrentCell(0) {}
		iterator(PxcHashMap & hashMap,PxU32 currentBucket) : mHashMap(hashMap),mCurrentBucket(currentBucket),mCurrentCell(0) {}

		void findNextNonEmptyBucket()
		{
			while(mCurrentBucket!=mHashMap.mBucketCount && mHashMap.mBucketArray[mCurrentBucket].isEmpty()) mCurrentBucket++;
			
			if(mCurrentBucket!=mHashMap.mBucketCount) mCurrentCell = mHashMap.mBucketArray[mCurrentBucket].next();
		}

		Cell& operator*()
		{
			return *mCurrentCell->value();
		}

		Cell* operator->()
		{
			return mCurrentCell->value();
		}

		iterator operator++()
		{
			CellNode * endCell = mHashMap.mBucketArray + mCurrentBucket;

			mCurrentCell = mCurrentCell->next();

			if(mCurrentCell==endCell)
			{
				mCurrentBucket++;

				findNextNonEmptyBucket();
			}

			return *this;
		}

		iterator operator++(int)
		{
		   iterator old(*this);
		   ++*this;
		   return old;
		}

		bool operator==(const iterator& other)
		{
			return mCurrentBucket==mHashMap.mBucketCount || (mCurrentBucket==other.mCurrentBucket && mCurrentCell==other.mCurrentCell);
		}

		bool operator!=(const iterator& other)
		{
			return !(mCurrentBucket==mHashMap.mBucketCount || (mCurrentBucket==other.mCurrentBucket && mCurrentCell==other.mCurrentCell));
		}

	private:
		PxcHashMap & mHashMap;
		PxU32 mCurrentBucket;
		CellNode * mCurrentCell;
	};

	PxcHashMap(PxU32 bucketCount): mBucketCount(bucketCount)
	{
		PXN_ASSERT((bucketCount & (bucketCount-1)) == 0); 
		mBucketArray = reinterpret_cast<CellNode *>(PXN_MALLOC(bucketCount * sizeof(CellNode)));
		for(PxU32 i=0;i<mBucketCount;i++)
			new(mBucketArray+i)CellNode(0);
	}

	~PxcHashMap()
	{
		PXN_FREE(mBucketArray);
	}

	void clear()
	{
		for(PxU32 i=0;i<mBucketCount;i++)
			new(mBucketArray+i)CellNode(0);

		mCellPool.clear();
	}

	// for when you know the key's not already in the hash
	Cell* insertFast(const Key& k, const Data& d)
	{
		PXN_ASSERT(!find(k));
		Cell* c = mCellPool.construct(k,d);
		PxU32 bucket = k.hash()&(mBucketCount-1);
		c->node.insertAfter(mBucketArray+bucket);
		return c;
	}

	Cell* find(const Key& k) const
	{
		PxU32 bucket = k.hash()&(mBucketCount-1);
		CellNode* hd = mBucketArray+bucket;
		for(CellNode* n = hd->next(); n!=hd; n = n->next())
		{
			if(n->value()->key == k)
				return n->value();
		}
		return 0;
	}

	Cell* insertReplace(const Key& k, const Data& d)
	{
		Cell* c = find(k);
		if(c)
		{
			c->data = d;
			return c;
		}
		else
			return insertFast(k,d);
	}

	void remove(Cell* c)
	{
		c->node.unlink();
		mCellPool.destroy(c);
	}

	void remove(const Key& k)
	{
		Cell* c = find(k);
		if(c)
			remove(c);
	}

	iterator begin()
	{
		iterator iter(*this);

		iter.findNextNonEmptyBucket();

		return iter;
	}

	iterator end()
	{
		return iterator(*this,mBucketCount);
	}

private:


	PxU32								mBucketCount;
	CellNode*							mBucketArray;
	PxcSimplePool<Cell>					mCellPool;
};


#endif
