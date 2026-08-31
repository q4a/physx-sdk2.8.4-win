#ifndef PXC_POOLMALLOC_H
#define PXC_POOLMALLOC_H

#include "PxnMemory.h"

#include "PxcPool.h"

void PxcPoolMallocInit();
void PxcPoolMallocTerm();

void* PxcPoolMalloc(size_t size);
void PxcPoolFree(void *ptr, size_t size);

#endif
