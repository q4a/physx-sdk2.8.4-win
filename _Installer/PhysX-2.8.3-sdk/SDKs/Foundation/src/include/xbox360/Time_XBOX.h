#ifndef NX_FOUNDATION_TIME_XBOX
#define NX_FOUNDATION_TIME_XBOX
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

namespace NxFoundation
	{

typedef double second;

class Time
	{
	second  lstTimeS;

	public:
	Time();
	/**
	returns milliseconds elapsed since last call.
	*/
	second GetElapsedSeconds();
	second PeekElapsedSeconds();
	private:
	static double GetTimeTicks();
	static second GetClockFrequency();
	};
	}
#endif
