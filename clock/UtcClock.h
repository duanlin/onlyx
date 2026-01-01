#ifndef UTCCLOCK_H
#define UTCCLOCK_H


#include <stdbool.h>
#include <stdint.h>


#define UTC_SECONDS_PER_1_DAY (3600 * 24)
#define UTC_SECONDS_PER_4_YEAR ((uint32_t)UTC_SECONDS_PER_1_DAY * 365 * 4 + UTC_SECONDS_PER_1_DAY)
#define UTC_SECONDS_PER_100_YEAR ((uint32_t)UTC_SECONDS_PER_4_YEAR * 25 - UTC_SECONDS_PER_1_DAY)
#define UTC_SECONDS_PER_400_YEAR ((uint64_t)UTC_SECONDS_PER_100_YEAR * 4 + UTC_SECONDS_PER_1_DAY)

#define UTC_SECONDS_AT_2000 ((uint32_t)946684800)


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	UtcQualityLost = 0,
	
	UtcQualityMiss,
	UtcQualityPoor,
	UtcQualityGood
	
} UtcQuality;

typedef struct
{
	uint32_t second;
	
	uint16_t ms;
	uint16_t us;
	
	bool leap;
	
	UtcQuality quality;
	
} UtcTimeStamp;

typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t ms;
	uint16_t us;
	
	UtcQuality quality;
	
} UtcDateStamp;

typedef struct
{
	uint32_t second;
	
	uint32_t edge;
	uint32_t width;
	
	bool LSP, LS;
	UtcQuality quality;
	
	uint32_t adjustTime;
	
} UtcClock;


extern UtcClock gUtcClock;


int initUtcClock(void);

int updateUtcClock(void);

int getUtcStamp(UtcTimeStamp* timeStamp);

static inline bool isLeapYear(uint16_t year)
{
	if(year % 4 == 0)
	{
		if(year % 100 == 0)
		{
			if(year % 400 == 0)
			{
				if(year % 3200 == 0)
					return false;
				
				return true;
			}
			
			return false;
		}
		
		return true;
	}
	
	return false;
}

int toDateStamp(const UtcTimeStamp* timeStamp, UtcDateStamp* dateStamp, signed timeZoneOffsetMinutes);
int toTimeStamp(const UtcDateStamp* dateStamp, signed timeZoneOffsetMinutes, UtcTimeStamp* timeStamp);


#ifdef __cplusplus
}
#endif


#endif
