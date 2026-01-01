#ifndef SYSHEAP_H
#define SYSHEAP_H


#include <stddef.h>


#define SYSHEAP_ALIGN_SIZE 16


#ifdef __cplusplus
extern "C"
{
#endif


int initSysHeap(void);

void* sysHeapAlloc(size_t size);
void sysHeapFree(void* ptr);


#ifdef __cplusplus
}
#endif


#endif
