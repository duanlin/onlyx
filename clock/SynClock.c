#include <string.h>

#include "SynClock.h"

#include "UniTimeX.h"



SynClock gSynClock;


static volatile uint32_t sUpdatedUsClock;
static volatile uint32_t sUpdatedUsTime;

static volatile uint32_t sUpdatedMsClock;
static volatile uint32_t sUpdatedMsTime;


int initSynClock(void)
{
	memset(&gSynClock, 0, sizeof(gSynClock));
	gSynClock.edge = getUniTimeX();
	gSynClock.width = UNITIMEX_COUNTER_FREQUENCY_HZ;
	
	sUpdatedUsClock = 0;
	sUpdatedUsTime = getUniTimeX();
	
	sUpdatedMsClock = 0;
	sUpdatedMsTime = getUniTimeX();
	
	return 0;
}

int updateSynClock(void)
{
	uint32_t now;
	
	unsigned elapse;
	unsigned remain;
	
	now = getUniTimeX();
	
	//
	// SYN clock
	//
	if(now - gSynClock.edge >= gSynClock.width)
		gSynClock.edge += gSynClock.width;
	
	//
	// UsClock and UsClock
	//
	elapse = now - sUpdatedUsTime;
	if(elapse / (UNITIMEX_COUNTER_FREQUENCY_HZ / 1000000))
	{
		sUpdatedUsClock += elapse / (UNITIMEX_COUNTER_FREQUENCY_HZ / 1000000);
		remain = elapse % (UNITIMEX_COUNTER_FREQUENCY_HZ / 1000000);
		
		sUpdatedUsTime = now - remain;
	}
	
	elapse = now - sUpdatedMsTime;
	if(elapse / (UNITIMEX_COUNTER_FREQUENCY_HZ / 1000))
	{
		sUpdatedMsClock += elapse / (UNITIMEX_COUNTER_FREQUENCY_HZ / 1000);
		remain = elapse % (UNITIMEX_COUNTER_FREQUENCY_HZ / 1000);
		
		sUpdatedMsTime = now - remain;
	}
	
	return 0;
}

int getSynStamp(SynTimeStamp* synStamp)
{
	uint32_t edge;
	uint32_t width;
	SynQuality quality;
	
	unsigned elapse;
	
	// Interruptable locking
	do {
		edge = gSynClock.edge;
		width = gSynClock.width;
		quality = gSynClock.quality;
		
	} while((edge != gSynClock.edge) || (width != gSynClock.width) || (quality != gSynClock.quality));
	
	elapse = getUniTimeX() - edge;
	
	synStamp->ns = 1000000000.0 / width * elapse;
	synStamp->ns = synStamp->ns % 1000000000;
	synStamp->quality = quality;
	
	return 0;
}

uint32_t usClock(void)
{
	uint32_t updatedClock;
	uint32_t updatedTime;
	
	unsigned elapse;
	
	// Interruptable locking
	do {
		updatedClock = sUpdatedUsClock;
		updatedTime = sUpdatedUsTime;
		
	} while((updatedClock != sUpdatedUsClock) || (updatedTime != sUpdatedUsTime));
	
	elapse = getUniTimeX() - updatedTime;
	
	return updatedClock + elapse / (UNITIMEX_COUNTER_FREQUENCY_HZ / 1000000);
}

uint32_t msClock(void)
{
	uint32_t updatedClock;
	uint32_t updatedTime;
	
	unsigned elapse;
	
	// Interruptable locking
	do {
		updatedClock = sUpdatedMsClock;
		updatedTime = sUpdatedMsTime;
		
	} while((updatedClock != sUpdatedMsClock) || (updatedTime != sUpdatedMsTime));
	
	elapse = getUniTimeX() - updatedTime;
	
	return updatedClock + elapse / (UNITIMEX_COUNTER_FREQUENCY_HZ / 1000);
}
