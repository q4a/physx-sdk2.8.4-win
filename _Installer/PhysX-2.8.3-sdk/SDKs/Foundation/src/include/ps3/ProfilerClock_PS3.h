/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
#ifndef NX_FOUNDATION_PROFILER_CLOCK_PS3
#define NX_FOUNDATION_PROFILER_CLOCK_PS3

namespace NxFoundation
{

#include <sys/time_util.h>
#include <sys/sys_time.h>

class ProfilerClock
	{
	public:

	static NX_INLINE void initClock()
	{
		baseTime = sys_time_get_timebase_frequency();
	}

	//!	This function starts recording the number of cycles elapsed.
	//!	\param		val		[out] address of a 32 bits value where the system should store the result.
	static NX_INLINE void startClock(NxU64& val)
	{
		uint64_t currentTime;
		SYS_TIMEBASE_GET(currentTime);
		val = currentTime;
	}

	//!	This function ends recording the number of cycles elapsed.
	//!	\param		val		[out] address to store the number of cycles elapsed since the last StartProfile.
	static NX_INLINE void stopClock(NxU64& val)
	{
		uint64_t currentTime;
		SYS_TIMEBASE_GET(currentTime);
		val = currentTime-val;
	}

	// TODO: convert to an appropriate value for user display(eg micro seconds)
	static NX_INLINE NxU64 convert(NxU64 interval)
	{
		return (1000000 * interval) / baseTime;
	}

	static NX_INLINE void startCounter(NxU32 &val)
	{
		// Disabled this zeroing code, just in case someone wants to set some magic value for now
		//val=0;
	}
	static NX_INLINE void stopCounter(NxU32 &val)
	{
		// Disabled this zeroing code, just in case someone wants to set some magic value for now
		//val=0;
	}

	static NxU64 baseTime;		//its ok for this to be static as its a machine property.
	};
	
}//end namespace

#endif


