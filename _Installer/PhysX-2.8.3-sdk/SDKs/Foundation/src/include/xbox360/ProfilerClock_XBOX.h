#ifndef NX_FOUNDATION_PROFILER_CLOCK_XBOX360
#define NX_FOUNDATION_PROFILER_CLOCK_XBOX360

#include <NxXBOX.h>

//#define NX_PROFILE_RDTSC

/*

To enable L2 cache miss profiling you must enable NX_ENABLE_PROFILER_COUNTER, which adds
an additional counter member to the profiling data(in NxProfiler).

Then enable NX_XBOX_L2_COUNTER plus link with libpmcpb.lib or libpmcpbd.lib.
*/
#if defined(_XBOX) && defined(NX_ENABLE_PROFILER_COUNTER)
//#define NX_XBOX_L2_COUNTER
#endif

#ifdef NX_XBOX_L2_COUNTER
#include <pmcpbsetup.h>
#endif

namespace NxFoundation
{

/***********************************************/

class ProfilerClock
	{
	public:

	static NX_INLINE void initClock()
		{
		LARGE_INTEGER li;

		QueryPerformanceFrequency(&li);
		ticksToMS=li.QuadPart/(1000*1000);

#ifdef NX_XBOX_L2_COUNTER
	//Setup xbox performance counter unit and select a default set of counters(this sets the counters on all cores
	/*
	TODO: We should probably callibrate for timing overhead and also allocate from non cached memory to eliminate
	cache misses due to profiling.
	*/
    PMCInstallSetup(&PMCDefaultSetups[PMC_SETUP_OVERVIEW_PB0T0]);
	PMCResetCounters();
	
	// Start up the Performance Monitor Counters
	//TODO: we should probably stop them when done
    PMCStart();
#endif
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
#ifdef NX_XBOX_L2_COUNTER
		PMCState             pmcState;

		PMCStop();//timers must be stopped to read them.
	    
		// Get the four counters.
		PMCGetCounters(&pmcState);

		val=(NxU32)pmcState.pmc[14];//magic value of 14 is L2 cache misses(see FastCPU sample)

		PMCStart();
#endif
		}
	static NX_INLINE void stopCounter(NxU32 &val)
		{
#ifdef NX_XBOX_L2_COUNTER
		PMCState             pmcState;

		PMCStop();//timers must be stopped to read them.
	    
		// Get the four counters.
		PMCGetCounters(&pmcState);

		val=(NxU32)pmcState.pmc[14] - val;//magic value of 14 is L2 cache misses(see FastCPU sample)

		PMCStart();
#endif
		}

	static NxU64 ticksToMS;

	static NxU64 baseTime;		//its ok for this to be static as its a machine property.
	};


	
}//end namespace

#endif
