#ifndef CORE_H
#define CORE_H


#include <stdint.h>


#define CORE_NAME "Cortex-A55"
#define CORE_ARCH "ARMv8-A"

//
// Caches for Cortex-A55
//
#define ICACHE_LINE_SIZE 64
#define DCACHE_LINE_SIZE 64

#define ICACHE_UPDATE() asm volatile("ic	IALLU\n" "isb	SY\n")
#define ICACHE_UPDATE_LINE(ADDR) asm volatile("ic	IVAU, %0\n" "isb	SY\n": : "r"((uintptr_t)(ADDR)) : )

static inline void ICACHE_UPDATE_RANGE(void* ADDR, unsigned SIZE)
{
	uintptr_t va;
	
	for(va = (uintptr_t)ADDR; va < (uintptr_t)ADDR + SIZE; va += ICACHE_LINE_SIZE)
		asm volatile("ic	IVAU, %0\n": : "r"(va) : );
	
	asm volatile("isb	SY\n");
}

static inline void DCACHE_FLUSH()
{
	unsigned way, set;
	
	// Level = 0
	for(way = 0; way < 4; way ++) for(set = 0; set < 128; set ++)
		asm volatile("dc	CSW, %0\n": : "r"((way << 30) | (set << 6)) : );
	
	asm volatile("isb	SY\n");
}

#define DCACHE_FLUSH_LINE(ADDR) asm volatile("dc	CVAP, %0\n" "isb	SY\n": : "r"((uintptr_t)(ADDR)) : )

static inline void DCACHE_FLUSH_RANGE(void* ADDR, unsigned SIZE)
{
	uintptr_t va;
	
	for(va = (uintptr_t)ADDR; va < (uintptr_t)ADDR + SIZE; va += DCACHE_LINE_SIZE)
		asm volatile("dc	CVAP, %0\n": : "r"(va) : );
	
	asm volatile("isb	SY\n");
}

static inline void DCACHE_UPDATE()
{
	unsigned way, set;
	
	// Level = 0
	for(way = 0; way < 4; way ++) for(set = 0; set < 128; set ++)
		asm volatile("dc	ISW, %0\n": : "r"((way << 30) | (set << 6)) : );
	
	asm volatile("isb	SY\n": : : "memory");
}

#define DCACHE_UPDATE_LINE(ADDR) asm volatile("dc	IVAC, %0\n" "isb	SY\n": : "r"((uintptr_t)(ADDR)) : "memory")

static inline void DCACHE_UPDATE_RANGE(void* ADDR, unsigned SIZE)
{
	uintptr_t va;
	
	for(va = (uintptr_t)ADDR; va < (uintptr_t)ADDR + SIZE; va += DCACHE_LINE_SIZE)
		asm volatile("dc	IVAC, %0\n": : "r"(va) : );
	
	asm volatile("isb	SY\n": : : "memory");
}

//
// Barriers for Cortex-A55
//
#define MEMORY_BARRIER() asm volatile("dmb	SY\n": : : "memory")

#define INSTRUCTION_BARRIER() asm volatile("isb	SY\n")


#endif
