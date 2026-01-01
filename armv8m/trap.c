#include <stdbool.h>
#include <stddef.h>

#include "trap.h"

#include "SysShell.h"

#include "ARMV8M.h"
#include "BitCodec.h"


InterruptInfo gInterruptInfo[TRAP_MAX_IRQ_COUNT];

SysCallHandler gSysCallHandler;


int registerInterrupt(Irq irq, InterruptHandler handler, void* arg, uint8_t priority)
{
	uint32_t nvic_itns;
	uint32_t nvic_ipr;
	
	if(irq >= TRAP_MAX_IRQ_COUNT)
		return -1;
	
	// NVIC_ICERn Interrupt Clear Enable Register
	REG32_SET(ARMV8M_NVIC_ICER0 + irq / 32 * 4, (uint32_t)0b01 << (irq % 32));
	// NVIC_ICPRn Interrupt Clear Pending Register
	REG32_SET(ARMV8M_NVIC_ICPR0 + irq / 32 * 4, (uint32_t)0b01 << (irq % 32));
	
	// NVIC_ITNSn Interrupt Target Non-secure Register
	nvic_itns = REG32_GET(ARMV8M_NVIC_ITNS0 + irq / 32 * 4);
	BIT_SET(nvic_itns, irq % 32, 0b1);
	REG32_SET(ARMV8M_NVIC_ITNS0 + irq / 32 * 4, nvic_itns);
	
	// NVIC_IPRn Interrupt Priority Register
	nvic_ipr = REG32_GET(ARMV8M_NVIC_IPR0 + irq / 4 * 4);
	BIT_SET_RANGE(nvic_ipr, irq % 4 * 8 + 7, irq % 4 * 8, priority);
	REG32_SET(ARMV8M_NVIC_IPR0 + irq / 4 * 4, nvic_ipr);
	
	gInterruptInfo[irq].handler = handler;
	gInterruptInfo[irq].arg = arg;
	
	// NVIC_ISERn Interrupt Set Enable Register
	REG32_SET(ARMV8M_NVIC_ISER0 + irq / 32 * 4, (uint32_t)0b01 << (irq % 32));
	
	return 0;
}

int disableInterrupt(Irq irq)
{
	if(irq >= TRAP_MAX_IRQ_COUNT)
		return -1;
	
	gInterruptInfo[irq].handler = NULL;
	gInterruptInfo[irq].arg = NULL;
	
	// NVIC_ICERn Interrupt Clear Enable Register
	REG32_SET(ARMV8M_NVIC_ICER0 + irq / 32 * 4, (uint32_t)0b01 << (irq % 32));
	
	return 0;
}

int setInterruptPending(Irq irq)
{
	if(irq >= TRAP_MAX_IRQ_COUNT)
		return -1;
	
	// NVIC_ISPRn Interrupt Set Pending Register
	REG32_SET(ARMV8M_NVIC_ISPR0 + irq / 32 * 4, (uint32_t)0b01 << (irq % 32));
	
	return 0;
}

int registerSysCallHandler(SysCallHandler handler)
{
	uint32_t scb_shpr2;
	
	// SHPR2 System Handler Priority Register 2
	// PRI_11 [31:24] SVCall Exception priority
	scb_shpr2 = REG32_GET(ARMV8M_SCB_SHPR2);
	BIT_SET_RANGE(scb_shpr2, 31, 24, 0x10); // 0x10 Second highest
	REG32_SET(ARMV8M_SCB_SHPR2, scb_shpr2);
	
	gSysCallHandler = handler;
	
	return 0;
}

static void printContext(const TrapContext* context)
{
	printk("-------------------------------------\n");
	printk("PC:    0x%08X  PSR:    0x%08X\n", context->PC, context->PSR);
	printk("r0 a1: 0x%08X  r3 a4:  0x%08X\n", context->r0, context->r3);
	printk("r1 a2: 0x%08X  r12 IP: 0x%08X\n", context->r1, context->r12);
	printk("r2 a3: 0x%08X  r14 LR: 0x%08X\n", context->r2, context->r14);
}

int hardFaultHandler(const TrapContext* context, uint32_t hfsr, uint32_t cfsr)
{
	uint16_t ufsr;
	uint8_t bfsr;
	uint8_t mmfsr;
	
	printk("\n-------------------------------------\n");
	printk("! Hard fault\n");
	
	printk("-------------------------------------\n");
	printk("HFSR: 0x%08X\n", hfsr);
	if(BIT_GET(hfsr, 30))
	{
		ufsr = BIT_GET_RANGE(cfsr, 31, 16);
		printk("UFSR: 0x%04X\n", ufsr);
		
		bfsr = BIT_GET_RANGE(cfsr, 15, 8);
		printk("BFSR: 0x%02X", bfsr);
		if(BIT_GET(bfsr, 7))
			printk("         BFAR: 0x%08X\n", REG32_GET(ARMV8M_SCB_BFAR));
		else
			printk("\n");
		
		mmfsr = BIT_GET_RANGE(cfsr, 7, 0);
		printk("MMFSR: 0x%02X", mmfsr);
		if(BIT_GET(mmfsr, 7))
			printk("        MMFAR: 0x%08X\n", REG32_GET(ARMV8M_SCB_MMFAR));
		else
			printk("\n");
	}
	
	printContext(context);
	
	// Halt
	while(true);
	
	return 0;
}

int memManageFaultHandler(const TrapContext* context, uint8_t mmfsr, uint32_t mmfar)
{
	printk("\n-------------------------------------\n");
	printk("! Memory fault\n");
	
	printk("-------------------------------------\n");
	printk("MMFSR: 0x%02X", mmfsr);
	if(BIT_GET(mmfsr, 7))
		printk("        MMFAR:  0x%08X\n", mmfar);
	else
		printk("\n");
	
	printContext(context);
	
	// Halt
	while(true);
	
	return 0;
}

int busFaultHandler(const TrapContext* context, uint8_t bfsr, uint32_t bfar)
{
	printk("\n-------------------------------------\n");
	printk("! Bus fault\n");
	
	printk("-------------------------------------\n");
	printk("BFSR: 0x%02X", bfsr);
	if(BIT_GET(bfsr, 7))
		printk("         BFAR: 0x%08X\n", REG32_GET(ARMV8M_SCB_BFAR));
	else
		printk("\n");
	
	printContext(context);
	
	// Halt
	while(true);
	
	return 0;
}

int usageFaultHandler(const TrapContext* context, uint32_t ufsr)
{
	printk("\n-------------------------------------\n");
	printk("! Usage fault\n");
	
	printk("-------------------------------------\n");
	printk("UFSR: 0x%04X\n", ufsr);
	
	printContext(context);
	
	// Halt
	while(true);
	
	return 0;
}

int svCallHandler(TrapContext* context)
{
	if(gSysCallHandler)
		gSysCallHandler(context);
	
	return 0;
}

int extInterruptHandler(Irq irq)
{
	if(irq >= TRAP_MAX_IRQ_COUNT)
		return -1;
	
	if(gInterruptInfo[irq].handler)
	{
		// Interrupt handle
		if(gInterruptInfo[irq].handler(gInterruptInfo[irq].arg))
			return -2;
	}
	
	return 0;
}
