#ifndef __NPHASE_CONTEXTGRAB__
#define __NPHASE_CONTEXTGRAB__

#include "NPhaseCore.h"
#include "NPhaseContext.h"
#include "Scene.h"

// This class acquires an NPhaseContext from the scene on construction and returns in on destruction.
//It is effectivly a smart pointer/
class NPhaseContextGrab
{
public:

	NPhaseContextGrab(const Scene *scene) : mScene(scene)
	{
		context = scene->getNPhaseCore()->getContext();
	}
	~NPhaseContextGrab()
	{
		mScene->getNPhaseCore()->putContext(context);
	}

	NX_INLINE	NPhaseContext *operator->()
	{
		return context;
	}

	NX_INLINE	const NPhaseContext *operator->() const
	{
		return context;
	}

	NX_INLINE	NPhaseContext &operator*()
	{
		return *context;
	}

	NX_INLINE	const NPhaseContext &operator*() const
	{
		return *context;
	}


private:
	NPhaseContext *context;
	const Scene *mScene;
};

#endif
