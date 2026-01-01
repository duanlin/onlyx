#ifndef SYNCLOCK_H
#define SYNCLOCK_H


#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	SynQualityLost = 0,
	
	SynQualityGood = 0b01,
	SynQualityFail = 0b10,
	SynQualityMiss = 0b11
	
} SynQuality;

typedef struct
{
	uint32_t ns;
	SynQuality quality;
	
} SynTimeStamp;

typedef struct
{
	uint32_t edge;
	uint32_t width;
	
	SynQuality quality;
	
} SynClock;


extern SynClock gSynClock;


int initSynClock(void);

int updateSynClock(void);

int getSynStamp(SynTimeStamp* synStamp);

uint32_t usClock(void);
uint32_t msClock(void);


#ifdef __cplusplus
}
#endif


#endif
