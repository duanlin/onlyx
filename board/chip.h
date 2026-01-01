#ifndef CHIP_H
#define CHIP_H


#define CHIP_NAME "GD32E503"

#define CORTEX_M33_CORE_COUNT 1

#define IRQ_INTID(IRQ) (16 + (IRQ))


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	// Exti
	IrqExti1 = 7,
	IrqExti3 = 9,
	
	// Timer
	IrqTimer5 = 54,
	
	// Usart
	IrqUsart0 = 37,
	
	// Can0
	IrqTxCan0 = 19,
	IrqRx0Can0 = 20,
	IrqRx1Can0 = 21,
	IrqEwmcCan0 = 22,
	
	// Can1
	IrqTxCan1 = 63,
	IrqRx0Can1 = 64,
	IrqRx1Can1 = 65,
	IrqEwmcCan1 = 66
	
} Irq;


#ifdef __cplusplus
}
#endif


#endif
