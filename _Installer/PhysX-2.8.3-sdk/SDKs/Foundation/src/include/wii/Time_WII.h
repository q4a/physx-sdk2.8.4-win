#ifndef NX_FOUNDATION_TIME_WII
#define NX_FOUNDATION_TIME_WII
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

/**
Time class.
*/

#include <dolphin.h>

namespace NxFoundation
	{

typedef double second;

class Time
	{
	OSTime	lstTime;
	public:
	Time();
	/**
	returns milliseconds elapsed since last call.
	*/
	second GetElapsedSeconds();
	second PeekElapsedSeconds();
	private:
	};
	}
#endif
