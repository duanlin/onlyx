#ifndef TIMER_H
#define TIMER_H


#include <stdint.h>

#include "trap.h"


#define TIMER_FREQUENCY_HZ 100000000


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	Timer5 = 0,
	
	TimerCount
	
} Timer;


int initTimer(void);

int restartTimer(Timer timer, uint64_t interval);
int setTimerInterval(Timer timer, uint64_t interval);

int registerTimerInterrupt(Timer timer, uint64_t interval, InterruptHandler handler, void* arg, uint8_t priority);
int disableTimerInterrupt(Timer timer);


#ifdef __cplusplus
}
#endif


#endif
