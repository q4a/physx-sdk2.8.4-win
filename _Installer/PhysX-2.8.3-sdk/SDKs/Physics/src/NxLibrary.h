/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#ifndef NX_PHYSICS_NXLIBRARY_H
#define NX_PHYSICS_NXLIBRARY_H

#include "Physics.h"

#if   defined(WIN32)
#define NOMINMAX
#include <windows.h>
#elif defined(LINUX)
#include <dlfcn.h>
#endif

/**
Class for making loading shared libraries abstract and simple.
*/
class NxLibrary
	{
	public:
	/**
    Default Constructor.
    */
	NxLibrary(void);
	
	/**
    Constructor. Send path to shared library.
    */
	NxLibrary(const char *path);
	
	/**
    Destructor.
    */
	~NxLibrary(void);
	
	/**
    Open Library. Send path to shared library.
    */
	bool open(const char *path);
	
	/**
    Close Library.
    */
	void close(void);
	
	/**
    Returns true if the library is loaded.
    */
	bool isOpen(void);
	
	/**
    Get the address of a symbol. Send name.
    */
	void *getSymbol(const char *name);
	
	private:
	#if   defined(WIN32)
	HMODULE   library;
	#elif defined(LINUX)
	void     *library;
	#endif
	};

#endif
