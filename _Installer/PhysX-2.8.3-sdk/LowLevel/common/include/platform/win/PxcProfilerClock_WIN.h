#ifndef PXC_PROFILERCLOCK_WIN_H
#define PXC_PROFILERCLOCK_WIN_H

class PxcProfilerClock
{
public:
	static PXD_INLINE void initClock()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);

		mTickToUs = li.QuadPart/(1000*1000);
	}

	static PXD_INLINE PxU64 getTicks()
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return li.QuadPart;
	}

	static PXD_INLINE PxU64 convertToUs(PxU64 ticks)
	{
		return ticks/mTickToUs;
	}
	
	static PxU64 mTickToUs; //tick to micro-seconds
};

#endif
