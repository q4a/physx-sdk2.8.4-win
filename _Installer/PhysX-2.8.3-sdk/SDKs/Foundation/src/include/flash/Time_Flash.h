#ifndef NX_FOUNDATION_TIME_FLASH
#define NX_FOUNDATION_TIME_FLASH
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
	second GetTimeSeconds();
	};
	}
#endif
