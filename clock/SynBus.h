#ifndef SYNBUS_H
#define SYNBUS_H


#include <stdbool.h>
#include <stdint.h>


#define SYNBUS_INTERRUPT_PRIORITY 0x20

#define SYNBUS_ROUTINE_FREQUENCY_HZ 5000
#define SYNBUS_MAINTASK_MOD_OFFSET 0
#define SYNBUS_EDGE_MAX_DIFF_US 100


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct
{
	unsigned busRxDelayNs;
	
	uint32_t edgeTime;
	uint32_t edgeWidth;
	bool edgeValid;
	bool widthValid;
	
	// Statistics
	uint64_t busEdgeRecvCount;
	uint64_t busEdgeTimeoutCount;
	uint64_t busEdgeErrorCount;
	
} SynBus;


extern SynBus gSynBus;


int initSynBus(void);

int synBusRoutine(void);


#ifdef __cplusplus
}
#endif


#endif
