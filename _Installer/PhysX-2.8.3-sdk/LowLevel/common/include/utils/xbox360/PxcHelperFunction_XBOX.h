#ifndef PXC_HELPERFUNCTION_XBOX_H
#define PXC_HELPERFUNCTION_XBOX_H

template<>
PXD_FORCE_INLINE void PxcSort(PxdF64& x, PxdF64& y)
{
	PxdF64 newX = fpmin(x, y);
	PxdF64 newY = fpmax(x, y);
	x=newX;
	y=newY;
}

template<>
PXD_FORCE_INLINE void PxcSort(PxFloat& x, PxFloat& y)
{
	PxdF64 newX = fpmin(x, y);
	PxdF64 newY = fpmax(x, y);
	x=(PxFloat)newX;
	y=(PxFloat)newY;
}

#endif
