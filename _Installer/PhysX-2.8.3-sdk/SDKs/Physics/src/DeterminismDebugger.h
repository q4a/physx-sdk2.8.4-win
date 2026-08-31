#ifndef NX_PHYSICS_NXDETERMINISM_DEBUGGER
#define NX_PHYSICS_NXDETERMINISM_DEBUGGER
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Physics.h"
#include "FPU.h"

/**
 Utility to help debug determinism bugs
 Not in foundation because it does direct file access, so it gets disabled in official builds.

 The assumption is that the begin()/end() pair will be called at least twice in sequence, and a bunch of 
 value() calls are made between them. It is ensured that the first sequence consists of the same values as the
 second and subsequent sequences.

 If this is not the case an error is reported.
*/

class DeterminismDebugger
	{
	public:
	static void begin(/*const char * fname, bool read*/);
	static void end();

	static void checkPoint(const char * file, int line);
	static void value(NxU32 v);
	NX_INLINE static void value(NxF32 v) 
		{ 
		value(NX_IR(v)); 
		}
	NX_INLINE static void value(const NxVec3 & v) 
		{ 
		value(NX_IR(v.x)); 
		value(NX_IR(v.y)); 
		value(NX_IR(v.z)); 
		}
	NX_INLINE static void value(const NxQuat & v) 
		{ 
		value(NX_IR(v.x));
		value(NX_IR(v.y)); 
		value(NX_IR(v.z)); 
		value(NX_IR(v.w)); 
		}
	NX_INLINE static void value(const NxMat33 & v) 
		{ 
		value(NX_IR(v(0,0))); 
		value(NX_IR(v(0,1))); 
		value(NX_IR(v(0,2))); 

		value(NX_IR(v(1,0))); 
		value(NX_IR(v(1,1))); 
		value(NX_IR(v(1,2))); 

		value(NX_IR(v(2,0))); 
		value(NX_IR(v(2,1))); 
		value(NX_IR(v(2,2))); 
		}
	NX_INLINE static void value(const NxMat34 & v) 
		{ 
		value(NX_IR(v.M)); 
		value(NX_IR(v.t)); 
		}
	};



#ifdef DEBUG_DETERMINISM
#define CHECK_DETERMINISM(x) DeterminismDebugger::checkPoint(__FILE__, __LINE__), DeterminismDebugger::value(x)
#else
#define CHECK_DETERMINISM(x) {}
#endif	//ifdef DEBUG_DETERMINISM

#endif
