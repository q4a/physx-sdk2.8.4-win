#ifndef OPC_ICE_HOOK_H
#define OPC_ICE_HOOK_H

// Should be included by Opcode.h if needed

	#define ICE_DONT_CHECK_COMPILER_OPTIONS

	#define	PARALLEL_TOLERANCE	1e-02f

	// From Windows...
	typedef int                 BOOL;
	#ifndef FALSE
	#define FALSE               0
	#endif

	#ifndef TRUE
	#define TRUE                1
	#endif

	#include <stdio.h>
	#include <stdlib.h>
	#include <assert.h>
	#include <string.h>
	#include <float.h>
	#include <math.h>

#ifdef WIN32
	#include "win/OPC_WIN.h"
#elif LINUX
	#include "linux/OPC_LINUX.h"  
#elif defined(__CELLOS_LV2__)
	#include "ps3/OPC_PS3.h"  
#elif defined(_XBOX)
	#include "xbox360/OPC_XBOX.h" 
	//! Use SIMD on xbox360
	#define OPC_SUPPORT_SIMD
	#define ICE32
#endif

	#ifndef ASSERT
		#define ASSERT(x) { if (x) ; else NxOpcodeError (#x, __FILE__, __LINE__); }
	#endif

	#define ICE_COMPILE_TIME_ASSERT(exp)	typedef char ICE_Dummy[ (exp) ? 1 : -1 ]

	#define	Log				{}
	inline const char*	_F(const char* format, ...){ return NULL; }
	//by Adam:
	extern bool NxOpcodeError(const char *message, const char *, unsigned );
	#define	SetIceError(a,b)		NxOpcodeError(a,__FILE__, __LINE__)

	#define	EC_OUTOFMEMORY	"Out of memory"

	#include "./Ice/IcePreprocessor.h"
	#undef ICECORE_API
	#define ICECORE_API	OPCODE_API

	#include "./Ice/IceTypes.h"
	#include "./Ice/IceMemoryMacros.h"

	#define USE_MINMAX
	#define PROFILE_MARK(x)
	typedef char* LPSTR;

	namespace IceCore
	{
		#include "./Ice/IceAllocator.h"
		#include "./Ice/IceFPU.h"
		#include "./Ice/IceUtils.h"
		#include "./Ice/IceContainer.h"
		#include "./Ice/IcePairs.h"
		#include "./Ice/IceRevisitedRadix.h"
		#include "./Ice/IceRandom.h"
		#include "./Ice/IcePairManager.h"
		#include "./Ice/IceCustomArray.h"
		#include "./Ice/IceFlagControl.h"
		#include "./Ice/IceHandleManager.h"
		#include "./Ice/IceSignature.h"
		#include "./Ice/IceCodeSize.h"
		#include "./Ice/IceStream.h"
		#include "./Ice/IceSerialize.h"
	}
	using namespace IceCore;

	#define ICEMATHS_API	OPCODE_API
	namespace IceMaths
	{
		#include "./Ice/IceAxes.h"
		#include "./Ice/IcePoint.h"
		#include "./Ice/IceHPoint.h"
		#include "./Ice/IceMatrix3x3.h"
		#include "./Ice/IceMatrix4x4.h"
		#include "./Ice/IcePlane.h"
		#include "./Ice/IceRay.h"
		#include "./Ice/IceIndexedTriangle.h"
		#include "./Ice/IceTriangle.h"
		#include "./Ice/IceTrilist.h"
		#include "./Ice/IceAABB.h"
		#include "./Ice/IceOBB.h"
		#include "./Ice/IceBoundingSphere.h"
		#include "./Ice/IceSegment.h"
		#include "./Ice/IceLSS.h"
		#include "./Ice/IceZCurve.h"
		#include "./Ice/IcePolygon.h"
	}
	using namespace IceMaths;

	#include "OPC_Simd.h"

	#define CONTACT_API
	namespace Ctc
	{
		#include "./Ice/CTC_PlaneAABBOverlap.h"
		#include "./Ice/CTC_RayAABBOverlap.h"
		#include "./Ice/CTC_SphereAABBOverlap.h"
		#include "./Ice/CTC_SegmentOBBDistance.h"
		#include "./Ice/CTC_PointOBBDistance.h"
	}
	using namespace Ctc;
#endif
