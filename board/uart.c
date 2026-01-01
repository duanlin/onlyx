#include <stddef.h>
#include <string.h>

#include "uart.h"

#include "GD32E503.h"
#include "BitCodec.h"


typedef struct
{
	UartRxHandler rxHandler;
	void* arg;
	
} UartInterruptInfo;


static UartInterruptInfo sUartInterruptInfo[UartCount];


int initUart(void)
{
	memset(&sUartInterruptInfo, 0, sizeof(sUartInterruptInfo));
	
	// USART0
	// PA9 USART0_TX
	// PA10 USART0_RX
	//
	// GD32E503_GPIO_CTL
	// CTL9 [7:6] 0b10 AFIO Push-Pull
	// MD9 [5:4] 0b11 Output 50MHz
	REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL1), 7, 4, 0b1011);
	// GD32E503_GPIO_CTL
	// CTL10 [11:10] 0b10 Float
	// MD10 [9:8] 0b00 Input
	REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL1), 11, 8, 0b1000);
	
	// GD32E503_USART_CTL0
	REG32_SET(GD32E503_USART0(GD32E503_USART_CTL0), 0);
	// GD32E503_USART_CTL1
	REG32_SET(GD32E503_USART0(GD32E503_USART_CTL1), 0);
	// GD32E503_USART_CTL2
	REG32_SET(GD32E503_USART0(GD32E503_USART_CTL2), 0);
	// GD32E503_USART_CTL3
	REG32_SET(GD32E503_USART0(GD32E503_USART_CTL3), 0);
	// GD32E503_USART_GDCTL
	REG32_SET(GD32E503_USART0(GD32E503_USART_GDCTL), 0);
	
	// GD32E503_USART_STAT0
	REG32_SET(GD32E503_USART0(GD32E503_USART_STAT0), 0);
	// GD32E503_USART_STAT1
	REG32_SET(GD32E503_USART0(GD32E503_USART_STAT1), 0);
	
	// GD32E503_USART_CTL0
	// OVSMOD [15] 0b1 8
	REG32_SET_BIT(GD32E503_USART0(GD32E503_USART_CTL0), 15, 0b1);
	
	// GD32E503_USART_BAUD
	// USARTDIV = APB2 100MHz / (8 x 115200) = 108.5069
	// INTDIV [15:4] 108
	// FRADIV [3:0] 0.5069 / 0.0625 = 8
	REG32_SET_RANGE(GD32E503_USART0(GD32E503_USART_BAUD), 15, 4, 108);
	REG32_SET_RANGE(GD32E503_USART0(GD32E503_USART_BAUD), 3, 0, 8);
	
	// GD32E503_USART_CTL0
	// UEN [13] 0b1 Enable
	REG32_SET_BIT(GD32E503_USART0(GD32E503_USART_CTL0), 13, 0b1);
	// TEN [3] 0b1 Enable
	// REN [2] 0b1 Enable
	REG32_SET_RANGE(GD32E503_USART0(GD32E503_USART_CTL0), 3, 2, 0b11);
	
	return 0;
}

static int uartInterruptHandler(void* arg)
{
	Uart uart;
	
	uint8_t byte;
	
	uart = (Uart)arg;
	
	// GD32E503_USART_STAT0
	// RBNE [5]
	if(REG32_GET_BIT(GD32E503_USART0(GD32E503_USART_STAT0), 5))
	{
		byte = REG32_GET(GD32E503_USART0(GD32E503_USART_DATA));
		
		REG32_SET_BIT(GD32E503_USART0(GD32E503_USART_STAT0), 5, 0b0);
		
		if(sUartInterruptInfo[uart].rxHandler)
			sUartInterruptInfo[uart].rxHandler(byte, sUartInterruptInfo[uart].arg);
	}
	
	return 0;
}

int registerUartHandler(Uart uart, UartRxHandler rxHandler, void* arg, uint8_t priority)
{
	(void)uart;
	
	// GD32E503_USART_CTL0
	// RBNEIE [5] 0b0 Disable
	REG32_SET_BIT(GD32E503_USART0(GD32E503_USART_CTL0), 5, 0b0);
	
	// GD32E503_USART_STAT0
	REG32_SET(GD32E503_USART0(GD32E503_USART_STAT0), 0);
	
	// Register
	if(registerInterrupt(IrqUsart0, uartInterruptHandler, (void*)Uart0, priority))
		return -1;
	
	sUartInterruptInfo[uart].arg = arg;
	sUartInterruptInfo[uart].rxHandler = rxHandler;
	
	// GD32E503_USART_CTL0
	// RBNEIE [5] 0b1 Enable
	REG32_SET_BIT(GD32E503_USART0(GD32E503_USART_CTL0), 5, 0b1);
	
	return 0;
}

int disableUargHandler(Uart uart)
{
	// GD32E503_USART_CTL0
	// RBNEIE [5] 0b0 Disable
	REG32_SET_BIT(GD32E503_USART0(GD32E503_USART_CTL0), 5, 0b0);
	
	sUartInterruptInfo[uart].rxHandler = NULL;
	sUartInterruptInfo[uart].arg = NULL;
	
	switch(uart)
	{
	case Uart0:
		if(disableInterrupt(IrqUsart0))
			return -1;
		
		break;
		
	default:
		return -2;
	}
	
	return 0;
}

unsigned uartTxSize(Uart uart)
{
	// GD32E503_USART_STAT0
	// TBE [7]
	if(REG32_GET_BIT(GD32E503_USART0(GD32E503_USART_STAT0), 7))
		return 1;
	
	return 0;
}

int uartTxByte(Uart uart, uint8_t byte)
{
	(void)uart;
	
	REG32_SET(GD32E503_USART0(GD32E503_USART_DATA), byte);
	
	return 0;
}
