#ifndef NX_COLLISION_FILTERING
#define NX_COLLISION_FILTERING
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

	class Shape;
	class NxUserContactReport;
	class PairManager;
	struct userPair;



typedef void	(*FilterFunction)	(NxGroupsMask& results, const NxGroupsMask& mask0, const NxGroupsMask& mask1);
extern FilterFunction const gTable[];
/*
NX_INLINE bool gFilterFunction(const NxGroupsMask& g0, const NxGroupsMask& g1, const NxGroupsMask& k0, const NxGroupsMask& k1, NxFilterOp op0, NxFilterOp  op1, NxFilterOp op2, bool b)
	{
	NxGroupsMask g0k0;	gTable[op0](g0k0, g0, k0);
	NxGroupsMask g1k1;	gTable[op1](g1k1, g1, k1);
	NxGroupsMask final;	gTable[op2](final, g0k0, g1k1);

	bool r = final.bits0 || final.bits1 || final.bits2 || final.bits3;
	return r==b;
	}
*/
NX_INLINE NX_BOOL Scene::filterFunction(const NxGroupsMask& g0, const NxGroupsMask& g1) const
	{
#ifdef TIWAK_GR3
    // Since we always use the gSWAP_AND operator, we want to avoid the use of
    // a function pointer that flushes the code instructions pipline on Xenon
    NxGroupsMask final;
    final.bits0 = g0.bits0 & g1.bits2;
    final.bits1 = g0.bits1 & g1.bits3;
    final.bits2 = g0.bits2 & g1.bits0;
    final.bits3 = g0.bits3 & g1.bits1;
#else // TIWAK_GR3
	NxGroupsMask g0k0;	gTable[filterOp0](g0k0, g0, bitsK0);
	NxGroupsMask g1k1;	gTable[filterOp1](g1k1, g1, bitsK1);
	NxGroupsMask final;	gTable[filterOp2](final, g0k0, g1k1);
#endif // TIWAK_GR3

	bool r = final.bits0 || final.bits1 || final.bits2 || final.bits3;
	return r==filterBool;


	//return gFilterFunction(g0, g1, bitsK0, bitsK1, filterOp0, filterOp1, filterOp2, filterBool);
	}

	#define IMPLEMENT_NEW_FILTERING															\
	/* New filtering - shape can't be a compound at this point */							\
		if(groupsMask && !filterFunction(*groupsMask, currentShape->getGroupsMaskFast()))	\
			continue;

#endif
