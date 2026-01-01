#ifndef UART_H
#define UART_H


#include <stdint.h>

#include "trap.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	Uart0 = 0,
	
	UartCount
	
} Uart;

typedef int (*UartRxHandler)(uint8_t byte, void* arg);


int initUart(void);

int registerUartHandler(Uart uart, UartRxHandler rxHandler, void* arg, uint8_t priority);
int disableUargHandler(Uart uart);

unsigned uartTxSize(Uart uart);
int uartTxByte(Uart uart, uint8_t byte);


#ifdef __cplusplus
}
#endif


#endif
