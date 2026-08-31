#ifndef __FPUGUARD__
#define __FPUGUARD__

#include "Nx.h"

/*
Description: Class which sets (and resets) the FPU control word in the constructor and descructor.

Threading: It should be possible to create multiple instances of this class on seperate threads to control
the precision on each thread.
*/
class NxFPUGuard
	{
	public:
		NxFPUGuard();
		~NxFPUGuard();


		//Setup the FPU with appropriate rounding modes etc for NovodeX.
		void ResetFPU();

	private:
#if defined(__APPLE__)
		NxU32   mControlWord;
#else
		NxU16	mControlWord;
#endif
	};



#endif
