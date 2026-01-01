#ifndef TRAP_H
#define TRAP_H


#include <stdint.h>


#define TRAP_MAX_INTID_COUNT 283


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct
{
	uintptr_t SP;
	uint64_t ELR;
	
	union { struct { double v0d0; double v0d1; }; struct { float v0s0; float v0s1; float v0s2; float v0s3; }; };
	union { struct { double v1d0; double v1d1; }; struct { float v1s0; float v1s1; float v1s2; float v1s3; }; };
	union { struct { double v2d0; double v2d1; }; struct { float v2s0; float v2s1; float v2s2; float v2s3; }; };
	union { struct { double v3d0; double v3d1; }; struct { float v3s0; float v3s1; float v3s2; float v3s3; }; };
	union { struct { double v4d0; double v4d1; }; struct { float v4s0; float v4s1; float v4s2; float v4s3; }; };
	union { struct { double v5d0; double v5d1; }; struct { float v5s0; float v5s1; float v5s2; float v5s3; }; };
	union { struct { double v6d0; double v6d1; }; struct { float v6s0; float v6s1; float v6s2; float v6s3; }; };
	union { struct { double v7d0; double v7d1; }; struct { float v7s0; float v7s1; float v7s2; float v7s3; }; };
	union { struct { double v8d0; double v8d1; }; struct { float v8s0; float v8s1; float v8s2; float v8s3; }; };
	union { struct { double v9d0; double v9d1; }; struct { float v9s0; float v9s1; float v9s2; float v9s3; }; };
	union { struct { double v10d0; double v10d1; }; struct { float v10s0; float v10s1; float v10s2; float v10s3; }; };
	union { struct { double v11d0; double v11d1; }; struct { float v11s0; float v11s1; float v11s2; float v11s3; }; };
	union { struct { double v12d0; double v12d1; }; struct { float v12s0; float v12s1; float v12s2; float v12s3; }; };
	union { struct { double v13d0; double v13d1; }; struct { float v13s0; float v13s1; float v13s2; float v13s3; }; };
	union { struct { double v14d0; double v14d1; }; struct { float v14s0; float v14s1; float v14s2; float v14s3; }; };
	union { struct { double v15d0; double v15d1; }; struct { float v15s0; float v15s1; float v15s2; float v15s3; }; };
	union { struct { double v16d0; double v16d1; }; struct { float v16s0; float v16s1; float v16s2; float v16s3; }; };
	union { struct { double v17d0; double v17d1; }; struct { float v17s0; float v17s1; float v17s2; float v17s3; }; };
	union { struct { double v18d0; double v18d1; }; struct { float v18s0; float v18s1; float v18s2; float v18s3; }; };
	union { struct { double v19d0; double v19d1; }; struct { float v19s0; float v19s1; float v19s2; float v19s3; }; };
	union { struct { double v20d0; double v20d1; }; struct { float v20s0; float v20s1; float v20s2; float v20s3; }; };
	union { struct { double v21d0; double v21d1; }; struct { float v21s0; float v21s1; float v21s2; float v21s3; }; };
	union { struct { double v22d0; double v22d1; }; struct { float v22s0; float v22s1; float v22s2; float v22s3; }; };
	union { struct { double v23d0; double v23d1; }; struct { float v23s0; float v23s1; float v23s2; float v23s3; }; };
	union { struct { double v24d0; double v24d1; }; struct { float v24s0; float v24s1; float v24s2; float v24s3; }; };
	union { struct { double v25d0; double v25d1; }; struct { float v25s0; float v25s1; float v25s2; float v25s3; }; };
	union { struct { double v26d0; double v26d1; }; struct { float v26s0; float v26s1; float v26s2; float v26s3; }; };
	union { struct { double v27d0; double v27d1; }; struct { float v27s0; float v27s1; float v27s2; float v27s3; }; };
	union { struct { double v28d0; double v28d1; }; struct { float v28s0; float v28s1; float v28s2; float v28s3; }; };
	union { struct { double v29d0; double v29d1; }; struct { float v29s0; float v29s1; float v29s2; float v29s3; }; };
	union { struct { double v30d0; double v30d1; }; struct { float v30s0; float v30s1; float v30s2; float v30s3; }; };
	union { struct { double v31d0; double v31d1; }; struct { float v31s0; float v31s1; float v31s2; float v31s3; }; };
	uint32_t FPSR;
	
	uint32_t SPSR;
	union { uint64_t r0; uint64_t x0; };
	union { uint64_t r1; uint64_t x1; };
	union { uint64_t r2; uint64_t x2; };
	union { uint64_t r3; uint64_t x3; };
	union { uint64_t r4; uint64_t x4; };
	union { uint64_t r5; uint64_t x5; };
	union { uint64_t r6; uint64_t x6; };
	union { uint64_t r7; uint64_t x7; };
	union { uint64_t r8; uint64_t x8; };
	union { uint64_t r9; uint64_t x9; };
	union { uint64_t r10; uint64_t x10; };
	union { uint64_t r11; uint64_t x11; };
	union { uint64_t r12; uint64_t x12; };
	union { uint64_t r13; uint64_t x13; };
	union { uint64_t r14; uint64_t x14; };
	union { uint64_t r15; uint64_t x15; };
	union { uint64_t r16; uint64_t x16; };
	union { uint64_t r17; uint64_t x17; uint64_t IP1; };
	union { uint64_t r18; uint64_t x18; uint64_t IP0; };
	
	union { uint64_t r29; uint64_t x29; uint64_t FP; };
	union { uint64_t r30; uint64_t x30; uint64_t LR; };
	
} TrapContext;

typedef int (*InterruptHandler)(void* arg);

typedef struct
{
	InterruptHandler handler;
	void* arg;
	
} InterruptInfo;

typedef enum
{
	InterruptTrigNone = 0,
	
	InterruptTrigEdge,
	InterruptTrigLevel
	
} InterruptTrig;

typedef int (*SysCallHandler)(TrapContext* context);


extern InterruptInfo gInterruptInfo[TRAP_MAX_INTID_COUNT];

extern SysCallHandler gSysCallHandler;


int registerInterrupt(unsigned INTID, InterruptHandler handler, void* arg, uint8_t priority, InterruptTrig trig);
int disableInterrupt(unsigned INTID);

int setInterruptPending(unsigned INTID);

int registerSysCallHandler(SysCallHandler handler);

int synExceptionHandler(TrapContext* context, uint64_t ESR, uintptr_t FAR);
int intExceptionHandler(const TrapContext* context, unsigned INTID);
int errExceptionHandler(const TrapContext* context, uint64_t ESR, uintptr_t FAR);


#ifdef __cplusplus
}
#endif


#endif
