#ifndef NX_FOUNDATION_PROFILER_CLOCK_WIIU
#define NX_FOUNDATION_PROFILER_CLOCK_WIIU

namespace NxFoundation
{
/***********************************************/
#if (defined(__PPCGEKKO__) || defined(CAFE)) && !defined(NX_PROFILE_RDTSC)	
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
		val = NxU64(OSGetTime());
		}

	//!	This function ends recording the number of cycles elapsed.
	//!	\param		val		[out] address to store the number of cycles elapsed since the last StartProfile.
	static NX_INLINE void stopClock(NxU64& val)
		{
		val = NxU64(OSGetTime()) - val;
		}

	static NX_INLINE NxU64 convert(NxU64 interval)
		{
		return NxU64(OSTicksToMicroseconds(OSTime(interval)));
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
#endif
}//end namespace

#endif
