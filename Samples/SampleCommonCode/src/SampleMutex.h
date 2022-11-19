#ifndef SAMPLE_MUTEX_H
#define SAMPLE_MUTEX_H

#if defined(SN_TARGET_PSP2)
#	include "SampleMutex_PSP2.h"
#elif defined(WIN32)
#	include "SampleMutex_WIN.h"
#elif defined(_XBOX)
#	include "SampleMutex_XBOX360.h"
#elif defined(__CELLOS_LV2__)
#	include "SampleMutex_PS3.h"
#elif defined(__linux__) || defined(__APPLE__)
#	include "SampleMutex_LINUX.h"
#elif defined(__PPCGEKKO__)
#	include "SampleMutex_wii.h"
#endif

#endif  // SAMPLE_MUTEX_H