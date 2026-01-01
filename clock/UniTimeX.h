#ifndef UNITIMEX_H
#define UNITIMEX_H


#define UNITIMEX_COUNTER_FREQUENCY_HZ 100000000


#include <stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif


int initUniTimeX(void);

uint32_t getUniTimeX(void);
uint64_t timeCounter(void);


#ifdef __cplusplus
}
#endif


#endif
