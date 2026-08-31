#ifndef NX_PHYSICS_NP_EFFECTOR
#define NX_PHYSICS_NP_EFFECTOR
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxEffector.h"
#include "ReadWriteMutex.h"

class NpEffector : public ReadWriteMutex
	{
	public:
	virtual ~NpEffector();
	void			setName(const char* name);
	const char*		getName()			const;
	};


#endif
