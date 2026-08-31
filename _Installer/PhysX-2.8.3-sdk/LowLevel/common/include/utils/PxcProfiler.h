#ifndef PXC_PROFILER_H
#define PXC_PROFILER_H

#include "PxcProfilerClock.h"
#include "PxnMemory.h"
#if AG_PERFMON
#include "AgPerfMonPhysX.h"
#endif // AG_PERFMON

#define PXC_ENABLE_PROFILER

class PxcProfiler
{
public:
	struct ProfileZone;

	PXD_INLINE PxcProfiler()
		: mProfilerZones(0), mNumZones(0)
	{
		PxcProfilerClock::initClock();
	}

	~PxcProfiler()
	{
		PXN_FREE(mProfilerZones);
	}

	PXD_INLINE void initZones(PxUInt numZones)
	{
		//Setup zones
		PXN_FREE(mProfilerZones);

		mNumZones = numZones;
		mProfilerZones = reinterpret_cast<ProfileZone*>(PXN_MALLOC(mNumZones*sizeof(ProfileZone)));

		PxcMemset(mProfilerZones, 0, mNumZones*sizeof(ProfileZone));
	}

	PXD_INLINE void setZoneInfo(PxUInt index, const char* name, PxU32 parent = (PxU32)-1)
	{
		if(index < mNumZones)
		{
			mProfilerZones[index].mName = name;
			mProfilerZones[index].mParentIndex = parent;
#if AG_PERFMON
			mProfilerZones[index].mIndex = gPerfUtils->registerEvent(name);
#endif // AG_PERFMON
			mProfilerZones[index].mIdent = reinterpret_cast<size_t>(this);
		}
	}

	PXD_INLINE void resetZones()
	{
		for(PxUInt i = 0; i < mNumZones; ++i)
		{
			mProfilerZones[i].mTicks = 0;
		}
	}
	
	PXD_INLINE ProfileZone& getZone(PxUInt index)
	{
		if(index < mNumZones)
		{
			return mProfilerZones[index];
		}

		return mScrapZone;
	}


	PXD_INLINE PxUInt getNumZones() const
	{
		return mNumZones;
	}

	PXD_INLINE void getProfileData(PxdProfileZone* data, bool reset = true)
	{
		for(PxUInt i = 0; i < mNumZones; i++)
		{
			const PxcProfiler::ProfileZone& zone = mProfilerZones[i];
			data[i].Name = zone.mName;
			data[i].parentIndex = zone.mParentIndex;
			if(zone.mTicks > ((PxU32)~0))
				data[i].totalTime = (PxU32)~0;
			else
				data[i].totalTime = (PxU32)zone.mTicks;
		}

		if(reset)
			resetZones();
	}

	struct ProfileZone
	{
		PxU64		mTicks;
		const char*	mName;
		PxU32		mIndex;
		PxU32		mParentIndex;
		size_t		mIdent;
	};

	struct ProfileZoneCounter
	{
		PXD_INLINE ProfileZoneCounter(ProfileZone& zone)
			: mZone(zone)
		{
#if AG_PERFMON
			gPerfUtils->startEvent(zone.mIndex);
#endif // AG_PERFMON
			mStartTicks = PxcProfilerClock::getTicks();
		}

		PXD_INLINE ~ProfileZoneCounter()
		{
			PxU64 endTicks = PxcProfilerClock::getTicks();
			PxU64 diff = endTicks-mStartTicks;
			mZone.mTicks += diff;
#if AG_PERFMON
			gPerfUtils->stopEvent(mZone.mIndex);
#endif // AG_PERFMON
		}

		ProfileZone&	mZone;
		PxU64			mStartTicks;

	};

private:
	ProfileZone*	mProfilerZones;
	ProfileZone		mScrapZone;
	PxUInt			mNumZones;
};

#ifdef PXC_ENABLE_PROFILER
#define PXC_PROFILE(_Profiler, _ZoneId) PxcProfiler::ProfileZoneCounter zoneProf ## _ZoneId ((_Profiler).getZone(_ZoneId))
#else
#define PXC_PROFILE(_Profiler, _ZoneId)
#endif

#define PXC_PROFILE_SET_ZONE_NAME(_Profiler, _ZoneId) _Profiler.setZoneInfo(_ZoneId, #_ZoneId )
#define PXC_PROFILE_SET_ZONE_NAME2(_Profiler, _ZoneId, _Parent) _Profiler.setZoneInfo(_ZoneId, #_ZoneId, _Parent)

#endif
