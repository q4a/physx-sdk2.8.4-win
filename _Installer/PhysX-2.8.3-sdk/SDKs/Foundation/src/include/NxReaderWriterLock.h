#ifndef __NX_READERWRITERLOCK__
#define __NX_READERWRITERLOCK__

#include "NxMutex.h"

//#define NX_RAYCAST_READWRITE_LOCK

/*
Implements a full reader writer lock, i.e. multiple readers or a single writer.

Below is an implementation using just a mutex for non win32 platforms.

Disable the full reader/writer lock by default. Using the full reader writer lock
results in better performance when a lot of parallel raycasts are performed but is
more costly when only a few are performed and slows down serial raycasts.
*/


#ifdef NX_RAYCAST_READWRITE_LOCK
class NxReaderWriterLock
	{
	public:

		NX_INLINE NxReaderWriterLock()
			{
			hReaderEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
			NX_ASSERT(hReaderEvent!=NULL);

			hMutex = CreateEvent(NULL,FALSE,TRUE,NULL);
			NX_ASSERT(hMutex!=NULL);
			
			InitializeCriticalSection(&writerMutex);
			counter = -1;
			recursionCounter=0;
			}
		NX_INLINE ~NxReaderWriterLock()
			{
			if(hReaderEvent!=NULL)
				CloseHandle(hReaderEvent);
			if(hMutex!=NULL)
				CloseHandle(hMutex);
			DeleteCriticalSection(&writerMutex);
			}

		NX_INLINE void lockReader()
			{
			if(InterlockedIncrement(&counter) == 0)
                {
				WaitForSingleObject(hMutex, INFINITE);
				SetEvent(hReaderEvent);
				}
			
			WaitForSingleObject(hReaderEvent,INFINITE);
			}

		NX_INLINE void lockWriter()
			{
			EnterCriticalSection(&writerMutex);

			//we may already have the global mutex(really an event so we have to handle recursion ourselves)
			recursionCounter++;
			if(recursionCounter==1)
				WaitForSingleObject(hMutex, INFINITE);
			}
		NX_INLINE void unlockReader()
			{
			if(InterlockedDecrement(&counter) < 0)
				{
				ResetEvent(hReaderEvent);
				SetEvent(hMutex);
				}
			}
		NX_INLINE void unlockWriter()
			{
			recursionCounter--;
			if(recursionCounter==0)
				SetEvent(hMutex);

			LeaveCriticalSection(&writerMutex);
			}

	private:

		HANDLE hReaderEvent;
		HANDLE hMutex;
		CRITICAL_SECTION writerMutex;
		LONG counter;//count the number of readers in the lock.
		LONG recursionCounter;//handle recursive writer locking
	};
#else
/*
TODO: implement proper reader writer lock for other platforms, eg cell os has a native reader writer lock.
*/

class NxReaderWriterLock
	{
	public:

		NX_INLINE void lockReader()
			{
			lock.lock();
			}
		NX_INLINE void lockWriter()
			{
			lock.lock();
			}
		NX_INLINE void unlockReader()
			{
			lock.unlock();
			}
		NX_INLINE void unlockWriter()
			{
			lock.unlock();
			}

	private:

		NxMutex lock;
	};
#endif

class NxTakeReaderLock
	{
	public:

		NxTakeReaderLock(NxReaderWriterLock &l) : lock(l)
			{
			lock.lockReader();
			}
		~NxTakeReaderLock()
			{
			lock.unlockReader();
			}

	private:
		NxReaderWriterLock &lock;
	};

class NxTakeWriterLock
	{
	public:

		NxTakeWriterLock(NxReaderWriterLock &l) : lock(l)
			{
			lock.lockWriter();
			}
		~NxTakeWriterLock()
			{
			lock.unlockWriter();
			}

	private:
		NxReaderWriterLock &lock;
	};

#endif
