// dsequeira: I expect these have to be inlined on SPU, elsewhere they shouldn't be.

#if defined(__CELLOS_LV2__) && defined(__SPU__)
#include <spu_printf.h>

PX_FORCE_INLINE void PxcOutputDebugString(const char * str)
{
	spu_printf(str);
}

PX_FORCE_INLINE void reportError(const char* error)
{
	spu_printf("Internal error: %s: \n",error);
	PX_ASSERT(0);
}
#endif