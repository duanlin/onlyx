#ifndef TRAP_H
#define TRAP_H


#include <stdint.h>

#include "chip.h"


#define TRAP_MAX_IRQ_COUNT 88


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct
{
	// State context
	//
	union { uint32_t r0; uint32_t a1; };
	union { uint32_t r1; uint32_t a2; };
	union { uint32_t r2; uint32_t a3; };
	union { uint32_t r3; uint32_t a4; };
	union { uint32_t r12; uint32_t IP; };
	union { uint32_t r14; uint32_t LR; };
	
	uint32_t PC;
	uint32_t PSR;
	
	// FP context
	//
	union { double d0; struct { float s0; float s1; }; };
	union { double d1; struct { float s2; float s3; }; };
	union { double d2; struct { float s4; float s5; }; };
	union { double d3; struct { float s6; float s7; }; };
	union { double d4; struct { float s8; float s9; }; };
	union { double d5; struct { float s10; float s11; }; };
	union { double d6; struct { float s12; float s13; }; };
	union { double d7; struct { float s14; float s15; }; };
	
	uint32_t FPSCR;
	uint32_t VPR;
	
} TrapContext;

typedef int (*InterruptHandler)(void* arg);

typedef struct
{
	InterruptHandler handler;
	void* arg;
	
} InterruptInfo;


typedef int (*SysCallHandler)(TrapContext* context);


extern InterruptInfo gInterruptInfo[TRAP_MAX_IRQ_COUNT];

extern SysCallHandler gSysCallHandler;


int registerInterrupt(Irq irq, InterruptHandler handler, void* arg, uint8_t priority);
int disableInterrupt(Irq irq);

int setInterruptPending(Irq irq);

int registerSysCallHandler(SysCallHandler handler);

int hardFaultHandler(const TrapContext* context, uint32_t hfsr, uint32_t cfsr);
int memManageFaultHandler(const TrapContext* context, uint8_t mmfsr, uint32_t mmfar);
int busFaultHandler(const TrapContext* context, uint8_t bfsr, uint32_t bfar);
int usageFaultHandler(const TrapContext* context, uint32_t ufsr);
int svCallHandler(TrapContext* context);
int extInterruptHandler(Irq irq);


#ifdef __cplusplus
}
#endif


#endif
