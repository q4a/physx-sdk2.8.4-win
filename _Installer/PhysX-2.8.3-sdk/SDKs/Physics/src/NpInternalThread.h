#ifndef __NP_INTERNAL_THREAD__
#define __NP_INTERNAL_THREAD__

#include "NxThread.h"
#include "Allocateable.h"

class NpScene;

class NpInternalThread : public NxThread, public NxFoundation::NxAllocateable
	{
	NpScene *owner;
	bool backgroundThread;

	public:
		NpInternalThread(NpScene *own, bool bkThread)
			{
			owner=own;
			backgroundThread=bkThread;
			}

	protected:
		virtual void execute();

	};

#endif
