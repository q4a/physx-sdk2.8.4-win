#ifndef NX_FOUNDATION_PROFILER_CLOCK_LINUX
#define NX_FOUNDATION_PROFILER_CLOCK_LINUX

//#define NX_PROFILE_RDTSC

namespace NxFoundation
{

/***********************************************/
#if defined(LINUX) && defined(NX_PROFILE_RDTSC)

#pragma message("TODO: impliment NxProfiler")

class ProfilerClock
	{
	public:

	static NX_INLINE void initClock()
		{
		      //__asm__("cpuid\n\t"
			   //     "rdtsc\n\t"
			   //     "mov      %0,   eax\n\t"
			   //     "cpuid\n\t"
			   //     "rdtsc\n\t"
			   //     "sub      eax,  %0\n\t"
			   //     "mov      %1,   eax\n\t"

			   //     "cpuid\n\t"
			   //     "rdtsc\n\t"
			   //     "mov      %0,   eax\n\t"
			   //     "cpuid\n\t"
			   //     "rdtsc\n\t"
			   //     "sub      eax,  %0\n\t"
			   //     "mov      %1,   eax\n\t"

			   //     "cpuid\n\t"
			   //     "rdtsc\n\t"
			   //     "mov      %0,   eax\n\t"
			   //     "cpuid\n\t"
			   //     "rdtsc\n\t"
			   //     "sub      eax,  %0\n\t"
			   //     "mov      %1,   eax"
			   //     : :"0"(cyc), "1"(base)
			   //     :"%eax");
		}

	//!	This function starts recording the number of cycles elapsed.
	//!	\param		val		[out] address of a 32 bits value where the system should store the result.
	static NX_INLINE void startClock(NxU64& val)
		{
		  //asm("cpuid\n\t"
		//  "rdtsc\n\t"
		//	"mov    ebx,    val\n\t"
		//	"mov    (%ebx), eax");
		}

	//!	This function ends recording the number of cycles elapsed.
	//!	\param		val		[out] address to store the number of cycles elapsed since the last StartProfile.
	static NX_INLINE void stopClock(NxU64& val)
		{
				//asm("cpuid\n\t"
		//    "rdtsc\n\t"
		//	  "mov    ebx,    val\n\t"
		//	  "sub    eax,    (%ebx)\n\t"
		//	  "mov    (%ebx), eax");
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

	static NxU64 baseTime;		//its ok for this to be static as its a machine property.
	};


/*********************************************/
#else

#include <sys/time.h>

class ProfilerClock
	{
	public:

	static NX_INLINE void initClock()
		{
		}

	//!	This function starts recording the number of cycles elapsed.
	//!	\param		val		[out] address of a 32 bits value where the system should store the result.
	static NX_INLINE void startClock(NxU64& val)
		{
		  timeval time;
		  gettimeofday(&time, NULL);
		  val = time.tv_sec*1000000+time.tv_usec;
		}

	//!	This function ends recording the number of cycles elapsed.
	//!	\param		val		[out] address to store the number of cycles elapsed since the last StartProfile.
	static NX_INLINE void stopClock(NxU64& val)
		{
		  timeval time;
		  gettimeofday(&time, NULL);
		  val = time.tv_sec*1000000+time.tv_usec-val;
		}

	// TODO: convert to an appropriate value for user display(eg micro seconds)
	static NX_INLINE NxU64 convert(NxU64 interval)
		{
		return interval;
		}

	static NX_INLINE void startCounter(NxU32 &val)
		{
		}
	static NX_INLINE void stopCounter(NxU32 &val)
		{
		}
	static NxU64 baseTime;	
	};

/*********************************************/
#endif

}//end namespace

#endif
