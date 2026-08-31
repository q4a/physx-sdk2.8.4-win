#ifndef NX_SWEEP_TESTS_H
#define NX_SWEEP_TESTS_H

	#include "Nx.h"
	#include "NxBox.h"
	#include "NxCapsule.h"
	class NxVec3;

	enum SweepType
	{
		SWEPT_BOX,
		SWEPT_CAPSULE,

		SWEPT_UNDEFINED
	};

	// PT: the names have been changed to avoid link errors in the X360 version. It shouldn't happen but for some reason, it does.
	class SDK_SweptVolume
	{
		public:
							SDK_SweptVolume()	: type(SWEPT_UNDEFINED)	{}
							~SDK_SweptVolume()	{}

				SweepType	type;
	};

	class SDK_SweptBox : public SDK_SweptVolume
	{
		public:
							SDK_SweptBox()	{ type = SWEPT_BOX;	}
							~SDK_SweptBox()	{}

				NxBox		box;
	};

	class SDK_SweptCapsule : public SDK_SweptVolume
	{
		public:
							SDK_SweptCapsule()	{ type = SWEPT_CAPSULE;	}
							~SDK_SweptCapsule()	{}

				NxCapsule	capsule;
	};

	class Shape;
	struct NxSweepQueryHit;
	typedef bool (*SweepFunc) (const Shape*, const SDK_SweptVolume&, const NxVec3&, NxSweepQueryHit&);
	extern const SweepFunc gSweepMap[][10];

#endif
