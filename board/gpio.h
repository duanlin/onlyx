#ifndef GPIO_H
#define GPIO_H


#include <stdint.h>


#define GPIO_CHANNEL_COUNT 16


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	GpioA = 0,
	GpioB,
	GpioC,
	GpioD,
	GpioE,
	GpioF,
	GpioG,
	
	GpioCount
	
} Gpio;

typedef enum
{
	GpioStateHiZ = 0,
	GpioStateLow,
	GpioStateHigh,
	GpioStateErr
	
} GpioState;


int initGpio(void);

int getGpioInput(Gpio gpio, uint16_t* status);
int setGpioOutput(Gpio gpio, unsigned channel, GpioState state);


#ifdef __cplusplus
}
#endif


#endif
