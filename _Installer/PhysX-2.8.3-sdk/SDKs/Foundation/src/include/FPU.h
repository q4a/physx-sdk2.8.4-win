#ifndef NX_FOUNDATION_FPU
#define NX_FOUNDATION_FPU

#include "Nx.h"

//Integer representation of a floating-point value.
#define NX_IR(x)			((NxU32&)(x))

//Floating-point representation of a integer value.
#define NX_FR(x)			((NxF32&)(x))

//Absolute integer representation of a floating-point value
#define NX_AIR(x)			(NX_IR(x)&0x7fffffff)


#endif
