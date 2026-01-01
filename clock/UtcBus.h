#ifndef UTCBUS_H
#define UTCBUS_H


#include <stdbool.h>
#include <stdint.h>

#include "UtcClock.h"


#define UTCBUS_INTERRUPT_PRIORITY 0x20

#define UTCBUS_ROUTINE_FREQUENCY_HZ 5000
#define UTCBUS_MAINTASK_MOD_OFFSET 1
#define UTCBUS_EDGE_MAX_DIFF_US 150

#define UTCBUS_INFO_TIMEOUT_MS 3


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct
{
	unsigned busRxDelayNs;
	
	// Bus edge
	uint32_t busEdgeTime;
	uint32_t busEdgeWidth;
	bool busEdgeValid;
	bool busWidthValid;
	
	// Bus info
	uint32_t busUtcSecond;
	bool busLSP, busLS;
	UtcQuality busQuality;
	bool busInfoValid;
	
	// Statistic
	uint64_t busEdgeRecvCount;
	uint64_t busEdgeTimeoutCount;
	uint64_t busEdgeErrorCount;
	
	uint64_t busInfoRecvCount;
	uint64_t busInfoTimeoutCount;
	uint64_t busInfoErrorCount;
	
} UtcBus;


extern UtcBus gUtcBus;


int initUtcBus(void);

int utcBusRoutine(unsigned mainCounter, bool synchronized);

int utcBusInfoRecv(uint32_t utcSecond, bool LSP, bool LS, UtcQuality quality);


#ifdef __cplusplus
}
#endif


#endif
