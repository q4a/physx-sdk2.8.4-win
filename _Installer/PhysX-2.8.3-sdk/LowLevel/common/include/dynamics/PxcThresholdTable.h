#ifndef PXS_THRESHOLDTABLE_H
#define PXS_THRESHOLDTABLE_H

#include "PxcBaseThresholdTable.h"

template <class BodyPtr>
class PxcThresholdTable : public PxcBaseThresholdTable<BodyPtr>
{
public:

	void init(PxU32 bodyPairCount)
	{
		PxcBaseThresholdTable<BodyPtr>::initTable(bodyPairCount);
	}

	void addForce(BodyPtr body0, BodyPtr body1, PxFloat normalForce)
	{
		PxFloat t0 = body0 ? body0->getReportThreshold() : PXD_FLOAT_MAX;
		PxFloat t1 = body1 ? body1->getReportThreshold() : PXD_FLOAT_MAX;
		addForceInternal(body0, body1, normalForce, PxcMin<float>(t0,t1));
	}

	template <class BodyPair>
	void getThresholdPairs(PxcArray<BodyPair> &accumulator)
	{
		getThresholdPairsInternal(accumulator);
	}
};

#endif
