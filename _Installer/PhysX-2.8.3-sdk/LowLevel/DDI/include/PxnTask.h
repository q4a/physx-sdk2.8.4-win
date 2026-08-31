#ifndef PXN_TASK_H
#define PXN_TASK_H

#include "px_config.h"
#include "px_task.h"

class PxnTask
{
public:
	virtual PxdHandle			getHandleV()					const	= 0;
	virtual void				releaseV()								= 0;

	virtual PxdTaskStatus		waitV()									= 0;
	virtual PxdTaskStatus		getStatusV()					const	= 0;
};

#endif
