#ifndef NX_PHYSICS_INTERVAL_SOLVE
#define NX_PHYSICS_INTERVAL_SOLVE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Interval.h"

//#define INTERVAL_USE_SCREWINGS		//This now also gets used in Body.cpp, so if you enable it you have to put it into Nxp.h

#ifdef INTERVAL_USE_SCREWINGS
#include "Screwing.h"
#endif

//#define DANIEL_CCD	// enable/disable CCD code of DG

struct INCLUSIONCREATE
	{
		NX_INLINE		INCLUSIONCREATE() : mFlipNormal(0)	{}
#ifdef INTERVAL_USE_SCREWINGS
		Screwing	mScrewing0;
		Screwing	mScrewing1;
#else
		//object 1 is moving with below params, object 2 is stationary:
		NxVec3			linearMotion, angularMotion, pivot;
#endif
		NX_BOOL		mFlipNormal;
	};

class ContinuousTest
	{
		public:
		NX_INLINE					ContinuousTest()	{}
		NX_INLINE					~ContinuousTest()	{}

		virtual	bool			Init(const INCLUSIONCREATE* create);
#ifdef INTERVAL_USE_SCREWINGS
		NX_INLINE	const Screwing&	GetScrewing0()	const	{ return mScrew0;	}
		NX_INLINE	const Screwing&	GetScrewing1()	const	{ return mScrew1;	}
#endif
		NX_INLINE	NxU32			GetNbIter()		const	{ return mNbIter;	}

		virtual	void			Inclusion(Interval& dst, Interval& src)	= 0;

		virtual	NX_BOOL			ValidateSolution(NxReal collision_time)	= 0;
		virtual	NxReal			ComputeEpsilon()						= 0;

		NX_BOOL					Solve(NxReal current_collision_time, Interval & solution);

//				RigidContact	mContact;
//				Contacts		mContacts;
//A				Container		mContactData;

		protected:
				NX_BOOL			_Solve(const Interval& it,  Interval & solution);
#ifdef INTERVAL_USE_SCREWINGS
		// Screwing from object 0
				Screwing		mScrew0;

		// Screwing from object 1
				Screwing		mScrew1;
		// Precomputed data
				NxMat34		m0to1;
#else
				//object 1 is moving with below params, object 2 is stationary:
				NxVec3			linearMotion, angularMotion, pivot;
#endif

		// Solution
//				Interval		mFirstTime;
//NxReal mExpectedBest;
		// Stats
				NxReal			timeEpsilon;	//iteration stops if temporal uncertainty is smaller than this
				NxReal			spaceEpsilon;	//iteration stops if spatial distance uncertainty is smaller than this. Preferable to time uncertainty, but convergence is not so predictable.
				NxU32			mNbIter;
				NX_BOOL			mValidSolution;
				NX_BOOL			mFlipNormal;
	};

struct FOURPOINTCREATE : public INCLUSIONCREATE
	{
		NxVec3		mWA;		//point	// GLOBALPOS ??
		NxVec3		mWB;		//triangle vertices
		NxVec3		mWC;
		NxVec3		mWD;
		NxVec3		ext;		//for edge-edge, this is the normal of the static edge, for backface culling.
	};

class VertexFaceTest : public ContinuousTest
	{
		public:
		NX_INLINE					VertexFaceTest()	{}
		NX_INLINE					~VertexFaceTest()	{}

		virtual	bool			Init(const INCLUSIONCREATE* create);
		virtual void			Inclusion(Interval& dst, Interval& src);
		virtual	NX_BOOL			ValidateSolution(NxReal collision_time);
		virtual	NxReal			ComputeEpsilon();

		protected:
		// Original vertices in world space
				NxVec3			mWA, mWB, mWC, mWD;
#ifdef INTERVAL_USE_SCREWINGS
				NxVec3			mSA, mSB, mSC, mSD;

		// Current vertices in a common screw space
		mutable	IntervalPoint	mPA,mPB,mPC,mPD;
#else
		NxVec3			trigNormal;		//precomputed un-normalized trig normal: (mPC-mPB)^(mPD-mPB)
		NxVec3			vertexAtStartOfLastInterval;		//a cache

#endif
	};

class EdgeEdgeTest : public ContinuousTest
	{
		public:
		NX_INLINE					EdgeEdgeTest()		{}
		NX_INLINE					~EdgeEdgeTest()		{}

		virtual	bool			Init(const INCLUSIONCREATE* create);
		virtual void			Inclusion(Interval& dst, Interval& src);
		virtual	NX_BOOL			ValidateSolution(NxReal collision_time);
		virtual	NxReal			ComputeEpsilon();

		//protected:	//temp commented
		// Original vertices in world space
		NxVec3		mWA, mWB, mWC, mWD;

		// 'normalized' B and D, unit distance from A resp. C:
		NxVec3		mWB2, mWD2;
		NxReal		edge1Length;		//length of mWA,mWB
#ifdef INTERVAL_USE_SCREWINGS
#error Never tested the edge-edge version of screwing since I ported this to Nx.
				NxVec3			mSA, mSB, mSC, mSD;

		// Current vertices in a common screw space
		mutable	IntervalPoint	mPA,mPB,mPC,mPD;
#else
		NxVec3 vertexAtStartOfLastInterval[2];		//a cache
		NxVec3 staticEdgeNormal;					//normal of the static edge, for backface culling.
#endif
	};

#if 0	//TODO: later
struct SPHERESPHERECREATE : public INCLUSIONCREATE
	{
		NxVec3		mWA;
		NxVec3		mWB;
	};

class SphereSphereTest : public ContinuousTest
	{
		public:
		NX_INLINE					SphereSphereTest()	{}
		NX_INLINE					~SphereSphereTest()	{}

		virtual	bool			Init(const INCLUSIONCREATE* create);
		virtual void			Inclusion(Interval& dst, Interval& src);
		virtual	NX_BOOL			ValidateSolution(NxReal collision_time);
		virtual	NxReal			ComputeEpsilon();

//		protected:
				NxReal			mRadius0, mRadius1;

		// Original vertices in world space
				NxVec3			mWA, mWB;

				NxVec3			mSA, mSB;
	};
#endif
#endif NX_PHYSICS_INTERVAL_SOLVE
