#ifndef NX_FOUNDATION_ASSERT
#define NX_FOUNDATION_ASSERT

/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

/**
SDK-internal Assert macro.  This is a dependency of the FoundationSDK object because
the assert dialog needs to be routed to the user, so we don't have to include that sort
of GUI code in the sdks.  

For this reason it can only be used SDK side, in classes that can't be instanced by the user. 

User side code can use the ANSI assert().

This file is not called Assert.h to avoid conflict with standard assert.h header.
*/

	#ifndef NX_CUSTOM_ASSERT
		#define NX_CUSTOM_ASSERT
		#undef NX_ASSERT		//get rid of lame default assert in case it was already included
		#if defined(_DEBUG) || defined(NX_CHECKED)									\
			//! Custom ASSERT function. Various usages:
			//! ASSERT(condition)
			//! ASSERT(!"Not implemented")
			//! ASSERT(condition && "error text")
			
			
			#if defined(WIN32)
			#include <intrin.h>
			  #define NX_ASSERT(exp)																	\
			  {																						\
				  static bool NX_ASSERT_ignore = false;													\
				  if(!(exp) && !NX_ASSERT_ignore)															\
				  {																					\
				  if(NxFoundation::FoundationSDK::dbAssert(NXE_ASSERTION, __FILE__, __LINE__, &NX_ASSERT_ignore, #exp)) \
					  {																				\
						  __debugbreak();																\
					  }																				\
				  }																					\
			  }
			#elif defined(LINUX) && !defined(TEGRA)
			  #define NX_ASSERT(exp)												\
				{											\
				static bool NX_ASSERT_ignore = false; \
				if(!(exp) && !NX_ASSERT_ignore)										\
					{													\
					if(NxFoundation::FoundationSDK::dbAssert(NXE_ASSERTION, __FILE__, __LINE__, &NX_ASSERT_ignore, #exp)) 	\
						{  asm ( "int $3");				\
						}					\
					}						\
				}
			#elif defined(__APPLE__) || defined(__native_client__) || defined(TEGRA) || defined(__flash__) || defined(__webgl__)
            #define NX_ASSERT(exp)												\
                {											\
				static bool NX_ASSERT_ignore = false; \
				if(!(exp) && !NX_ASSERT_ignore)										\
					{													\
					if(NxFoundation::FoundationSDK::dbAssert(NXE_ASSERTION, __FILE__, __LINE__, &NX_ASSERT_ignore, #exp)) 	\
						{  assert((exp) || NX_ASSERT_ignore);				\
						}					\
					}						\
				}
			#elif defined(__CELLOS_LV2__)
				#define NX_ASSERT(exp)												\
				{											\
					static bool NX_ASSERT_ignore = false; \
					if(!(exp) && !NX_ASSERT_ignore)										\
					{													\
						if(NxFoundation::FoundationSDK::dbAssert(NXE_ASSERTION, __FILE__, __LINE__, &NX_ASSERT_ignore, #exp)) 	\
						{  assert((exp) || NX_ASSERT_ignore);				\
						}					\
					}						\
				}
			#elif defined(_XBOX)
			  #define NX_ASSERT(exp)																	\
				{																						\
					static bool NX_ASSERT_ignore = false;												\
					if(!(exp) && !NX_ASSERT_ignore)														\
					{																					\
						if(NxFoundation::FoundationSDK::dbAssert(NXE_ASSERTION, __FILE__, __LINE__, &NX_ASSERT_ignore, #exp)) \
						{																				\
							/*DebugBreak();*/ \
							assert((exp)); \
						}																				\
					}																					\
				}
			#elif defined(__PPCGEKKO__)
			  #define NX_ASSERT(exp)																	\
				{																						\
					static bool NX_ASSERT_ignore = false;												\
					if(!(exp) && !NX_ASSERT_ignore)														\
					{																					\
						if(NxFoundation::FoundationSDK::dbAssert(NXE_ASSERTION, __FILE__, __LINE__, &NX_ASSERT_ignore, #exp)) \
						{																				\
							assert(exp);																\
						}																				\
					}																					\
				}
			#elif defined(CAFE)
			  #define NX_ASSERT(exp)																	\
				{																						\
					static bool NX_ASSERT_ignore = false;												\
					if(!(exp) && !NX_ASSERT_ignore)														\
					{																					\
						if(NxFoundation::FoundationSDK::dbAssert(NXE_ASSERTION, __FILE__, __LINE__, &NX_ASSERT_ignore, #exp)) \
						{																				\
							assert(exp);																\
						}																				\
					}																					\
				}
			#endif						
		#else
			//! Leave the {} so that you can write this kind of things safely in release mode:
			//!	if(condition)	NX_ASSERT()
			#define NX_ASSERT(exp)	{}
		#endif


	#endif
#endif
