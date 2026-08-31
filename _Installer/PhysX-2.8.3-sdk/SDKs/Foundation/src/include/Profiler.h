#ifndef NX_FOUNDATION_PROFILER
#define NX_FOUNDATION_PROFILER
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Allocateable.h"
#include "Array.h"
#include "NxProfiler.h"

#include "FoundationSDK.h"
#include "NxThread.h"
#include "NxMutex.h"
#include "ProfilerClock.h"

#if AG_PERFMON
#include "AgPerfMonPhysX.h"
#endif // AG_PERFMON

#define NX_FASTPROF


// In foundation so we can use it in the Core and NxPhysics.
namespace NxFoundation
	{

struct ProfileName
	{
	ProfileName() : name(0), callCount(0) {}

	NX_INLINE void reset() 
		{
		parent = 0;
		callCount = 0;
		time = 0;
		recursionLevel = 0;
		start = 0;
		order = 0;
		childTime = 0;
		counter = 0;
		}

	const char * name;
	NxU32 parent;		//first time parent's index.
	NxU32 callCount;
	NxU64 time;
	NxU64 childTime;	//children add their times here.
	NxU32 recursionLevel;
	NxU64 start;	//used as scratch for time the clock started.
	NxU32 order;	//order it was first called.
	NxU32 counter; //Additional performance counter(eg L2 cache misses)
	};

class Profiler : public NxAllocateable
	{

	public:
	Profiler();
	
	NxProfilerData * getProfileData(bool reset);

	NX_INLINE		void			startProfile(NxU32 name)
		{
		if (name < names.size()-1)//guard against sloppy usage
			{
			// Keep track of recursion level
#ifdef NX_FASTPROF
			ProfileName & z = fastNames[name];
#else
			ProfileName & z = names[name];
#endif
			if (z.name == NULL)	//don't spend time on inactive zones.
				return;

			if (z.callCount == 0)	//only 1st time if called from several places.
				{
				z.order = nRecords++;
				z.recursionLevel= recursionLevel;
#ifdef NX_FASTPROF
				z.parent = *fastStack;
#else
				z.parent = activeStack.back();
#endif
				}
			else
				{
#ifdef NX_FASTPROF
				if (z.parent != *fastStack)
#else
				if (z.parent != activeStack.back())
#endif
					z.parent = 0xffffffff;				//multiparent == no parent!
				}
			recursionLevel++;
#ifdef NX_FASTPROF
			*(++fastStack) = name;
#else
			activeStack.pushBack(name);
#endif
			clock.startCounter(z.counter);
			clock.startClock(z.start);
			}
		}

	NX_INLINE		void			endProfile(NxU32 name)
		{
		if (name < names.size()-1)//guard against sloppy usage
			{
#ifdef NX_FASTPROF
			ProfileName & z = fastNames[name];
#else
			ProfileName & z = names[name];
#endif
			if (z.name == NULL)	//don't spend time on inactive zones.
				return;

			clock.stopClock(z.start);
			clock.stopCounter(z.counter);

			--recursionLevel;
			z.callCount ++;
			z.time += z.start;	//save value, it will be overwritten by next startProfile().
#ifdef NX_FASTPROF
			fastStack--;
			fastNames[*fastStack].childTime += z.start;
#else
			activeStack.popBack();
			names[activeStack.back()].childTime += z.start;
#endif
			}
		}

	NX_INLINE		void		allocateNames(NxU32 numNames)
		{
		names.resize(numNames+1);	//need 1 more for scratch
		for(NxU32 i=0;i<names.size();i++)
			{
			ProfileName & z = names[i];
			z.reset();
			z.name = 0;
			}

#ifdef NX_FASTPROF
		fastNames = numNames ? names.begin() : 0;
#endif
		activeStack[0] = numNames;	//have the active stack point to it.
		}

	NX_INLINE		void		createName(const char * n, NxU32 slotNum)
		{
		if (slotNum < names.size())
			{
			ProfileName & pn = names[slotNum];
			pn.name = n;
			pn.reset();
			}
		}
	NX_INLINE		ProfileName*	getProfileName(NxU32 name)
		{
		if (name < names.size()-1)//guard against sloppy usage
			{
#ifdef NX_FASTPROF
			ProfileName & z = fastNames[name];
#else
			ProfileName & z = names[name];
#endif
			return &z;
			}
		else
			return NULL;
		}
	private:

	ProfilerClock clock;

	NxArraySDK<ProfileName> names;
	NxArraySDK< ::NxProfileZone> profData;
	NxArraySDK<NxU32> activeStack;	//index of name currently active.  Only used to detect multi-parent situations that screw up the self-time computation.
	NxProfilerData rval;

#ifdef NX_FASTPROF
	ProfileName * fastNames;
	NxU32 * fastStack;			//points to top of stack
#endif
	
	NxU32 recursionLevel, nRecords;
	};

/*******************/

class ProfilerManager : public NxFoundation::NxAllocateable
{
public:

    ProfilerManager()
    {
        profilerTls=NxTlsAlloc();
    }
    ~ProfilerManager()
    {
        for(NxU32 i=0;i<profList.size();i++)
            NX_DELETE_SINGLE(profList[i]);

        profList.clear();

        NxTlsFree(profilerTls);
    }

    //must be called before calling startProfiler() and createName()
    NX_INLINE void allocateNames(NxU32 nNames)
    {
        names.resize(nNames);
#if AG_PERFMON
        eventIds.resize(nNames);
#endif // AG_PERFMON
        for(NxU32 i=0;i<names.size();i++)
        {
            names[i]=NULL;
#if AG_PERFMON
            eventIds[i] = AG_INVALID_EVENT_ID;
#endif // AG_PERFMON
        }
    }
    //must be called before calling startProfiler()
    NX_INLINE void createName(const char * n, NxU32 slotNum)
    {
        names[slotNum]=n;
#if AG_PERFMON
		eventIds[slotNum] = gPerfUtils->registerEvent(n);
#endif // AG_PERFMON

    }


    NX_INLINE void startProfile(NxU32 name)
    {
        getPerThreadProfiler()->startProfile(name);
    }
    NX_INLINE void endProfile(NxU32 name)
    {
        getPerThreadProfiler()->endProfile(name);
    }

    //Note: we assume all threads are suspended(ie not adding profile zones) when this is called.
    NxProfilerData * getProfileData(bool reset);

    Profiler *getPerThreadProfiler()
    {
        Profiler *prof=(Profiler *)NxTlsGet(profilerTls);
        if(prof!=NULL)
            return prof;
        else
            return addPerThreadProfiler();
    }

#if AG_PERFMON
    NxArraySDK<AgEventID> eventIds;
    int threadId;
#endif // AG_PERFMON

private:

    Profiler *addPerThreadProfiler();

    NxMutex profilerMutex;
    //>>>>
    NxArraySDK<Profiler *> profList;
    NxArraySDK<const char *> names;

    //these store the merged profiler data from all threads.
    NxArraySDK< ::NxProfileZone> profData;
    NxProfilerData rval;
    //<<<<

    NxTLSId profilerTls;
};

/*************************/
class ProfileZone
{
public:
    NX_INLINE ProfileZone(NxU32 nIndex, ProfilerManager & cp) : 
      nameIndex(nIndex), currProfiler(cp), mThreadID(0)
      {
          currProfiler.startProfile(nIndex);
#if AG_PERFMON 
		  gPerfUtils->startEvent(currProfiler.eventIds[nIndex]);
#endif // AG_PERFMON
      }
      NX_INLINE ~ProfileZone()
      {
          currProfiler.endProfile(nameIndex);
#if AG_PERFMON
		  gPerfUtils->stopEvent(currProfiler.eventIds[nameIndex]);
#endif // AG_PERFMON
      }
private:
    NxU32 nameIndex;
    ProfilerManager & currProfiler;	//unfortunately we can't use a global as each scene gets its own profiler.
    NxU32           mThreadID;
};

    };	//end namespace
	
//wrap in macro so we can easily disable profiling and not get the strings stored:

#ifdef NX_ENABLE_PROFILER
#define NX_PROFILE_ZONE(name, cp) NxFoundation::ProfileZone profileZone(name, cp);
#else
#define NX_PROFILE_ZONE(name, cp) 
#endif
//this one is always on, used sparingly for PPU LOD related timings
#define NX_PROFILE_ZONE_IMMUTABLE(name, cp) NxFoundation::ProfileZone profileZone(name, cp);



#endif
