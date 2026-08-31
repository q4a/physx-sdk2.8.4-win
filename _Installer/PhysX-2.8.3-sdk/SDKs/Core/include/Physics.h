#ifndef NX_PHYSICS_PHYSICS
#define NX_PHYSICS_PHYSICS
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Nxp.h"

/* COMMENT(djs): THIS FILE SHOULD NOT BE PART OF THE CORE API !!!! */

#define NX_RELEASE(x)	\
	if(x)				\
		{				\
		x->release();	\
		x = NULL;		\
		}

class NxUserAllocator;

//extern /*NXF_DLL_EXPORT*/ NxUserAllocator * nxFoundationSDKAllocator;
//NXF_DLL_EXPORT NxUserAllocator * GetAllocator();


#define	NX_WORLD_BODY

//Enable the SSE code path(note it is dynamically chosen so will still run on non SSE systems)
//Relies on NX_WORLD_BODY, x86-32bit(maybe x64 works?) and a recent version of MSVC.
#if (_MSC_VER>=1300) && defined(NX_WORLD_BODY) && defined(WIN32)
#define NX_SSE_PATH 
#endif

//Note parallel raycasts rely on bp double buffering
#define NX_RAYCAST_THREADING

#define OBSERVABLE_BODY				// can't get rid of them for now, because of BodyEffectors

//#define NX_LEGACY_LOCAL_FORCE		//for compatibility with space-bug

//#define UNDEFINE_ME_BEFORE_SHIPPING
#ifdef UNDEFINE_ME_BEFORE_SHIPPING
	//#define USE_PROFILER	// WARNING: profiling has a significant speed hit (we now profile very low-level stuff)
	//#define DEBUG_DETERMINISM	// WARNING: has a significant speed hit. Causes us to do direct file system access.
#endif

//#define CULLING_TWEAK	5.0f

//some internal macros:

/*
param checking. Usage only in API methods!!: 

variable args:

ASSERT_PARAM_BEGIN(mass > 0) "Body::setMass: mass is %f, should be positive!", mass PARAM_END 

one arg:

ASSERT_PARAM(mass > 0, "Body::setMass: mass should be positive!");    //returns void
ASSERT_PARAM_R0(mass > 0, "Body::setMass: mass should be positive!"); //returns 0


make sure API method execution is atomic!!  I.e. put all of these right at the start of the method.
The API calls should not fail once these checks succeed. (if this is always possible is another question)
*/

#define ASSERT_PARAM_BEGIN(exp)			{if (!(exp)) { 	NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_PARAMETER, __FILE__, __LINE__, 0, 
#define PARAM_END						); return; }}
#define ASSERT_PARAM(exp,msg)			{if (!(exp)) { 	NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_PARAMETER, __FILE__, __LINE__, 0, msg); return; }}
#define ASSERT_PARAM_R0(exp,msg)		{if (!(exp)) { 	NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_PARAMETER, __FILE__, __LINE__, 0, msg); return 0; }}
#define ASSERT_PARAM_RX(exp,msg, rval)	{if (!(exp)) { 	NxFoundation::FoundationSDK::getInstance().error(NXE_INVALID_PARAMETER, __FILE__, __LINE__, 0, msg); return rval; }}

#define NOT_AVAILABLE(msg)				{	NxFoundation::FoundationSDK::getInstance().error(NXE_DB_WARNING, __FILE__, __LINE__, 0, msg); return; }
#define NOT_AVAILABLE_R0(msg)			{	NxFoundation::FoundationSDK::getInstance().error(NXE_DB_WARNING, __FILE__, __LINE__, 0, msg); return 0; }

#if defined(_DEBUG) || defined(NX_CHECKED)
// These macros check for invalid inputs and prevent them from tainting the SDK
//#define NXCHK_VALID_VECTOR(vec, func) if(!NxMath::isFinite(vec.x) || !NxMath::isFinite(vec.y) || !NxMath::isFinite(vec.z)) {NxFoundation::FoundationSDK::error(NXE_INVALID_PARAMETER, __FILE__, __LINE__, 0, "%s: non-finite argument in function call!", #func); return;}
//#define NXCHK_VALID_QUAT(quat, func) if(!NxMath::isFinite(quat.x) || !NxMath::isFinite(quat.y) || !NxMath::isFinite(quat.z) || !NxMath::isFinite(quat.w)) {NxFoundation::FoundationSDK::error(NXE_INVALID_PARAMETER, __FILE__, __LINE__, 0, "%s: non-finite argument in function call!", #func); return;}
//#define NXCHK_VALID_MATRIX(m, func) if(!NxMath::isFinite(m(0,0)) || !NxMath::isFinite(m(0,1)) || !NxMath::isFinite(m(0,2)) || !NxMath::isFinite(m(1,0)) || !NxMath::isFinite(m(1,1)) || !NxMath::isFinite(m(1,2)) || !NxMath::isFinite(m(2,0)) || !NxMath::isFinite(m(2,1)) || !NxMath::isFinite(m(2,2))) {NxFoundation::FoundationSDK::error(NXE_INVALID_PARAMETER, __FILE__, __LINE__, 0, "%s: non-finite argument in function call!", #func); return;}

//!	Checks a String
//!	\param		string	[in] string pointer
//!	\param		maxSize	[in] max string length to check
NX_INLINE bool NxCheckString(const char* string, NxU32 maxSize)		
	{ 
	if(string == NULL)
		return true;

	for(NxU32 i=0;i<=maxSize;i++)
		{
		if(string[i] == '\0')
			return true;
		}

	return false;
	}

// These macros check for invalid inputs but pass them into the SDK anyway
#define NXCHK_DEFAULT_STRING_SIZE 4096
#define NXCHK_VALID_FLOAT(f, func) if(!NxMath::isFinite(f)) NxFoundation::FoundationSDK::error(NXE_INVALID_PARAMETER, __FILE__, __LINE__, 0, "%s: Parameter should be a valid float!", #func)
#define NXCHK_VALID_VECTOR(vec, func) if(!NxMath::isFinite(vec.x) || !NxMath::isFinite(vec.y) || !NxMath::isFinite(vec.z)) NxFoundation::FoundationSDK::error(NXE_INVALID_PARAMETER, __FILE__, __LINE__, 0, "%s: Parameter should be a valid vector!", #func)
#define NXCHK_VALID_QUAT(quat, func) if(!NxMath::isFinite(quat.x) || !NxMath::isFinite(quat.y) || !NxMath::isFinite(quat.z) || !NxMath::isFinite(quat.w)) NxFoundation::FoundationSDK::error(NXE_INVALID_PARAMETER, __FILE__, __LINE__, 0, "%s: Parameter should be a valid quaternion!", #func)
#define NXCHK_VALID_MATRIX(m, func) if(!NxMath::isFinite(m(0,0)) || !NxMath::isFinite(m(0,1)) || !NxMath::isFinite(m(0,2)) || !NxMath::isFinite(m(1,0)) || !NxMath::isFinite(m(1,1)) || !NxMath::isFinite(m(1,2)) || !NxMath::isFinite(m(2,0)) || !NxMath::isFinite(m(2,1)) || !NxMath::isFinite(m(2,2))) NxFoundation::FoundationSDK::error(NXE_INVALID_PARAMETER, __FILE__, __LINE__, 0, "%s: Parameter should be a valid matrix!", #func)
#define NXCHK_VALID_STRING_SIZE(string, maxSize, func) if(!NxCheckString(string, maxSize)) NxFoundation::FoundationSDK::error(NXE_INVALID_PARAMETER, __FILE__, __LINE__, 0, "%s: Parameter should be a valid string!", #func)
#define NXCHK_VALID_STRING(string, func) NXCHK_VALID_STRING_SIZE(string, NXCHK_DEFAULT_STRING_SIZE, func)

// The following macro asserts an expression and passes an error message to the error stream if the assertion fails
#define NXCHK_ASSERT(expr, err_type, err_str) if (!(expr)) NxFoundation::FoundationSDK::error(err_type, __FILE__, __LINE__, 0, err_str)
#else
// These macros don't check inputs at all
#define NXCHK_VALID_FLOAT(f, func)
#define NXCHK_VALID_VECTOR(vec, func)
#define NXCHK_VALID_QUAT(quat, func) 
#define NXCHK_VALID_MATRIX(m, func)
#define NXCHK_VALID_STRING_SIZE(string, maxSize, func)
#define NXCHK_VALID_STRING(string, func)

// No special assertions in release build
#define NXCHK_ASSERT(expr, err_type, err_str)
#endif

//!	Clears a buffer.
//!	\param		addr	[in] buffer address
//!	\param		size	[in] buffer length
NX_INLINE void NxZeroMemory(void* addr, NxU32 size)		{ memset(addr, 0, size);	}

// PT: this used to be replicated everywhere in the code, causing bugs to sometimes reappear (e.g. TTP 3587).
// It is better to define it in a header and use the same constant everywhere. The original value (1e-05f)
// caused troubles (e.g. TTP 1705, TTP 306).
#define PARALLEL_TOLERANCE	1e-02f

// #define NX_DISABLE_VISUALIZATION

#endif //#define NX_PHYSICS_PHYSICS
