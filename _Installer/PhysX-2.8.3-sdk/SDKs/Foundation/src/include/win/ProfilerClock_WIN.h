#ifndef NX_FOUNDATION_PROFILER_CLOCK_WIN
#define NX_FOUNDATION_PROFILER_CLOCK_WIN

#define NOMINMAX
#include <windows.h>

//#define NX_PROFILE_RDTSC

namespace NxFoundation
{

/***********************************************/
#if defined(WIN32) && !defined(NX_PROFILE_RDTSC)

class ProfilerClock
	{
	public:

	static NX_INLINE void initClock()
		{
		LARGE_INTEGER li;

		QueryPerformanceFrequency(&li);
		ticksToMS=li.QuadPart/(1000*1000);
		}

	//!	This function starts recording the number of cycles elapsed.
	//!	\param		val		[out] address of a 32 bits value where the system should store the result.
	static NX_INLINE void startClock(NxU64& val)
		{
		/*
		** TODO: we should handle the overhead of QPC properly if we are profiling very small code sections.
		*/
		LARGE_INTEGER li;

		QueryPerformanceCounter(&li);
		val=li.QuadPart;
		}

	//!	This function ends recording the number of cycles elapsed.
	//!	\param		val		[in,out] address to store the number of cycles elapsed since the last StartProfile.
	//								holds the time reported at the start.
	static NX_INLINE void stopClock(NxU64& val)
		{
		LARGE_INTEGER li;

		QueryPerformanceCounter(&li);
		val=li.QuadPart-val;
		}

	//convert to micro seconds
	static NX_INLINE NxU64 convert(NxU64 interval)
		{
		return interval/ticksToMS;
		}

	static NX_INLINE void startCounter(NxU32 &val)
		{
		}
	static NX_INLINE void stopCounter(NxU32 &val)
		{
		}

	static NxU64 ticksToMS;

	static NxU64 baseTime;		//its ok for this to be static as its a machine property.
	};

/*********************************************/
#elif defined(WIN32) && defined(NX_PROFILE_RDTSC)

class ProfilerClock
	{
	public:

	//! This function initializes the profiler by counting the cpuid overhead.
	//! This is done 3 times on purpose, since cpuid takes a longer time to execute the first times it's called.
	//! "cpuid" is used before rdtsc to prevent out-of-sequence execution from producing wrong results.
	//! For more details, read Intel's application notes "Using the RDTSC instruction for performance monitoring".

	static NX_INLINE void initClock()
		{
		/*
		** TODO: update this to take into account the(small) overhead of storing to 64bit
		*/
		NxU32 cyc, base;

		_asm
			{
			cpuid
			rdtsc
			mov		cyc, eax
			cpuid
			rdtsc
			sub		eax, cyc
			mov		base, eax

			cpuid
			rdtsc
			mov		cyc, eax
			cpuid
			rdtsc
			sub		eax, cyc
			mov		base, eax

			cpuid
			rdtsc
			mov		cyc, eax
			cpuid
			rdtsc
			sub		eax, cyc
			mov		base, eax
			}
			baseTime = base;
		}

	//!	This function starts recording the number of cycles elapsed.
	//!	\param		val		[out] address of a 32 bits value where the system should store the result.
	static NX_INLINE void startClock(NxU64& val)
		{
		NxU32 valLow,valHigh;

		__asm
			{
			cpuid
			rdtsc
			mov		[valLow], eax
			mov		[valHigh],edx
			}
		val=(((NxU64)valHigh)<<32) | valLow;
		}

	//!	This function ends recording the number of cycles elapsed.
	//!	\param		val		[out] address to store the number of cycles elapsed since the last StartProfile.
	static NX_INLINE void stopClock(NxU64& val)
		{
		NxU32 valLow,valHigh;

		__asm
			{
			cpuid
			rdtsc
			mov		[valLow], eax
			mov		[valHigh],edx
			}

		NxU64 val2=(((NxU64)valHigh)<<32) | valLow;
		val=val2-val-baseTime;
		}

	// TODO: convert to an appropriate value for user display(eg micro seconds)
	static NX_INLINE NxU64 convert(NxU64 interval)
		{
		return interval;
		}

	static NX_INLINE void startCounter(NxU32 &val)
		{
		val=0;
		}
	static NX_INLINE void stopCounter(NxU32 &val)
		{
		val=0;
		}

		//this is the overhead of cpuid
		static NxU64 baseTime;		//its ok for this to be static as its a machine property.
	};
	
#endif

}//end namespace

#endif
