/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
#ifndef NX_FOUNDATION_TIME_PS3
#define NX_FOUNDATION_TIME_PS3
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


