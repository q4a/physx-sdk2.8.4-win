#ifndef PXC_PROFILERCLOCK_WII_H
#define PXC_PROFILERCLOCK_WII_H

class PxcProfilerClock
{
public:
	static PXD_INLINE void initClock()
	{
	}

	static PXD_INLINE PxU64 getTicks()
	{
		return OSGetTime();
	}

	static PXD_INLINE PxU64 convertToUs(PxU64 ticks)
	{
		return OSTicksToMicroseconds(ticks);
	}
};

#endif
