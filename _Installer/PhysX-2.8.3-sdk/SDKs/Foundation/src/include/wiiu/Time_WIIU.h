#ifndef NX_FOUNDATION_TIME_WIIU
#define NX_FOUNDATION_TIME_WIIU
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

#include <cafe.h>

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
