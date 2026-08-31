#ifndef PXC_PLATFORMINCCLUDE_WIN_H
#define PXC_PLATFORMINCCLUDE_WIN_H

//turn off as much as we can for windows. All we really need is the thread functions(critical sections/Interlocked* etc)
#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOUSER
#define NONLS
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define WIN32_LEAN_AND_MEAN
// _WIN32_WINNT >= 0x0500 needed for spin crit sect, we only support >= WinXP 
//#if !defined(_WIN32_WINNT) || (_WIN32_WINNT != 0x0500)
//    #error "_WIN32_WINNT must be defined to 0x0500"
//#endif
#include <windows.h>
#ifdef PXD_SUPPORT_SSE
	#include <xmmintrin.h>
#endif

#endif
