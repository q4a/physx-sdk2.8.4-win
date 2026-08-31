#ifndef PXC_PROFILERCLOCK_LINUX_H
#define PXC_PROFILERCLOCK_LINUX_H

#include <sys/time.h>

// Note: To get decent precision gettimeofday() is used. Hence, no ticks are avaible, only us.

class PxcProfilerClock
{
public:
	static PXD_INLINE void initClock()
	{
	        //mTickToUs = CLOCKS_PER_SEC/(1000*1000);
	}

	static PXD_INLINE PxU64 getTicks()
	{
	        timeval time;
		gettimeofday(&time, NULL);
		return time.tv_sec*1000000+time.tv_usec;
                //return clock();
	}

	static PXD_INLINE PxU64 convertToUs(PxU64 ticks)
	{
	        //return ticks/mTickToUs;
	        return ticks;
	}

	static PxU64 mTickToUs; //tick to micro-seconds
};

#endif
