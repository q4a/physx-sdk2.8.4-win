#ifndef PXC_LISTNODE_H
#define PXC_LISTNODE_H

#include "PxcNoCopy.h"
/*!
List-node in double-linked list.
*/
template <class T>
class PxcListNode : public PxcNoCopy_::NoCopy
{
public:
	PxcListNode(T* val)
	{
		mPrev = this; 
		mNext = this;
		mValue = val;
	}

	T* value()
	{
		return mValue;
	}

	bool isEmpty()	const
	{
		return mPrev == this; 
	}
	
	void unlink()
	{
		mPrev->mNext = mNext; 
		mNext->mPrev = mPrev; 
		mNext = this;
		mPrev = this;
	}
	
	void insertAfter(PxcListNode<T>* prev)
	{
		mPrev = prev; 
		mNext = prev->mNext; 
		mPrev->mNext = this;
		mNext->mPrev = this;
	} 

	void insertBefore(PxcListNode<T>* next)
	{
		mNext = next;
		mPrev = next->mPrev;
		mPrev->mNext = this;
		mNext->mPrev = this;
	}

	PxcListNode<T>* next()
	{
		return mNext;
	}
	
	PxcListNode<T>* prev()
	{
		return mPrev;
	}

private:
	PxcListNode<T> *mPrev, *mNext;
	T* mValue;
};
#endif

