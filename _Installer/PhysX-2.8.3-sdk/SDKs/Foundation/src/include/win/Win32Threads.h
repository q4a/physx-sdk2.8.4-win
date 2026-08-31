#pragma once


#if !defined(TLS_OUT_OF_INDEXES)
#define TLS_OUT_OF_INDEXES ((DWORD)0xFFFFFFFF)
#endif


namespace win32
{
	DWORD WINAPI TlsAlloc();
	BOOL WINAPI TlsFree(DWORD index);
	LPVOID WINAPI TlsGetValue(DWORD index);
	BOOL WINAPI TlsSetValue(DWORD index, LPVOID value);

	HANDLE WINAPI CreateThread(LPTHREAD_START_ROUTINE function, LPVOID parameter);
	VOID WINAPI ExitThread(DWORD exitCode);
	BOOL WINAPI TerminateThread(HANDLE thread, DWORD exitCode);

	VOID WINAPI Sleep(DWORD milliseconds);
}
