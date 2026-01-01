#ifndef MAINTASK_H
#define MAINTASK_H


#include <stdbool.h>

#include "chip.h"
#include "timer.h"


#define MAINTASK_TIMER Timer5
#define MAINTASK_IRQ IrqTimer5

#define MAINTASK_FREQUENCY_HZ 10000


#ifdef __cplusplus
extern "C"
{
#endif


typedef int (*UserMain)(unsigned mainCounter, bool synchronized, void* arg);


int initMainTask(void);

int registerUserMain(UserMain routine, void* arg);

int postMainRoutine(void);


#ifdef __cplusplus
}
#endif


#endif
