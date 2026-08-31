#ifndef PXC_STACK_H
#define PXC_STACK_H

#include "px_config.h"
#include "PxnMemory.h"
#include "PxcFastMemory.h"

template <class T, int increment>
class PxcStack
{
public:
	PxcStack(): mStack(0), mTop(0), mSize(0) {}
	~PxcStack() 
	{ 
		//should we check for non empty stack? Probably not...
		if(mStack != NULL) 
			PXN_FREE(mStack);
	}

	void push(T a)
	{
		if(mTop == mSize)
		{
			T* newStack = reinterpret_cast<T*>(PXN_MALLOC((mSize+increment) * sizeof(T)));
			if(mStack)
			{
				PxcMemcpy(newStack,mStack, sizeof(T)*mSize);
				PXN_FREE(mStack);
			}
			mStack = newStack;
			mSize += increment;
		}
		mStack[mTop++] = a;
	}

	T pop()
	{
		PXN_ASSERT(!empty());
		return mStack[--mTop];
	}

	bool empty()
	{
		return mTop==0;
	}


private:
	T* mStack;
	PxU32 mTop;//next free element.
	PxU32 mSize;
};


#endif
