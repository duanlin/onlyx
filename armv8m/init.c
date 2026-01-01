#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "trap.h"

#include "ARMV8M.h"
#include "BitCodec.h"


int init(void)
{
	unsigned i;
	
	//
	// MPU initialization
	//
	extern uint8_t __text_vmpos[], __text_vmend[];
	extern uint8_t __rodata_vmpos[], __rodata_vmend[];
	extern uint8_t __rwdata_vmpos[], __rwdata_vmend[];
	extern uint8_t __bss_vmpos[], __bss_vmend[];
	extern uint8_t __stack_vmpos[], __stack_vmend[];
	extern uint8_t __heap_vmpos[], __heap_vmend[];
	
	uint32_t mpu_ctrl;
	
	//
	// MPU_MAIR0 MPU Memory Attribute Indirection Register 0
	//
	uint32_t mpu_mair0;
	
	mpu_mair0 = REG32_GET(ARMV8M_MPU_MAIR0);
	// Attr1 [15:8] Attribute memory
	// 0b01000100 Normal memory Non-cacheable
	// Attr0 [7:0] Attribute Device
	// 0b00000000 Device-nGnRnE
	BIT_SET_RANGE(mpu_mair0, 7, 0, 0b00000000);
	BIT_SET_RANGE(mpu_mair0, 15, 8, 0b01000100);
	REG32_SET(ARMV8M_MPU_MAIR0, mpu_mair0);
	
	// Region never access
	// 0x0 - 0x800_0000
	
	// Region read/write
	// 0x800_0000 - 0x2000_0000
	// Attribute device
	REG32_SET(ARMV8M_MPU_RNR, 0);
	REG32_SET(ARMV8M_MPU_RBAR, 0x8000000 | 0b011);
	REG32_SET(ARMV8M_MPU_RLAR, ((0x20000000 - 1) & ~0b11111) | 0b10001);
	
	// Region read/execute
	// __text_vmpos - __text_vmend
	// Attribute memory
	REG32_SET(ARMV8M_MPU_RNR, 1);
	REG32_SET(ARMV8M_MPU_RBAR, (uintptr_t)__text_vmpos | 0b110);
	REG32_SET(ARMV8M_MPU_RLAR, (((uintptr_t)__text_vmend - 1) & ~0b11111) | 0b00011);
	
	// Region read
	// __rodata_vmpos - __rodata_vmend
	// Attribute memory
	REG32_SET(ARMV8M_MPU_RNR, 2);
	REG32_SET(ARMV8M_MPU_RBAR, (uintptr_t)__rodata_vmpos | 0b111);
	REG32_SET(ARMV8M_MPU_RLAR, (((uintptr_t)__rodata_vmend - 1) & ~0b11111) | 0b10011);
	
	// Region read/write
	// __rwdata_vmpos - __rwdata_vmend
	// Attribute memory
	REG32_SET(ARMV8M_MPU_RNR, 3);
	REG32_SET(ARMV8M_MPU_RBAR, (uintptr_t)__rwdata_vmpos | 0b011);
	REG32_SET(ARMV8M_MPU_RLAR, (((uintptr_t)__rwdata_vmend - 1) & ~0b11111) | 0b10011);
	
	// Region read/write
	// __bss_vmpos - __bss_vmend
	// Attribute memory
	REG32_SET(ARMV8M_MPU_RNR, 4);
	REG32_SET(ARMV8M_MPU_RBAR, (uintptr_t)__bss_vmpos | 0b011);
	REG32_SET(ARMV8M_MPU_RLAR, (((uintptr_t)__bss_vmend - 1) & ~0b11111) | 0b10011);
	
	// Region read/write
	// __stack_vmpos - __stack_vmend
	// Attribute memory
	REG32_SET(ARMV8M_MPU_RNR, 5);
	REG32_SET(ARMV8M_MPU_RBAR, (uintptr_t)__stack_vmpos | 0b011);
	REG32_SET(ARMV8M_MPU_RLAR, (((uintptr_t)__stack_vmend - 1) & ~0b11111) | 0b10011);
	
	// Region read/write
	// __heap_vmpos - __heap_vmend
	// Attribute memory
	REG32_SET(ARMV8M_MPU_RNR, 6);
	REG32_SET(ARMV8M_MPU_RBAR, (uintptr_t)__heap_vmpos | 0b011);
	REG32_SET(ARMV8M_MPU_RLAR, (((uintptr_t)__heap_vmend - 1) & ~0b11111) | 0b10011);
	
	// Region read/write
	// 0x20020000 - 0x1_0000_0000
	// Attribute device
	REG32_SET(ARMV8M_MPU_RNR, 7);
	REG32_SET(ARMV8M_MPU_RBAR, 0x20020000 | 0b011);
	REG32_SET(ARMV8M_MPU_RLAR, (0xFFFFFFFF & ~0b11111) | 0b10001);
	
	// MPU_CTRL MPU Control Register
	mpu_ctrl = REG32_GET(ARMV8M_MPU_CTRL);
	// PRIVDEFENA [2] Privileged default enable.
	// 0b0 Use of the system address map disabled. Any instruction or data access that does not access a defined memory region faults.
	BIT_SET(mpu_ctrl, 2, 0b0);
	// HFNMIENA [1] HardFault, NMI enable. Controls whether handlers executing with a requested execution priority of less than 0 access memory with the MPU enabled or disabled.
	// 0b1 MPU enabled for these handlers.
	BIT_SET(mpu_ctrl, 1, 0b1);
	// ENABLE [0] 0b1 enable.
	BIT_SET(mpu_ctrl, 0, 0b1);
	REG32_SET(ARMV8M_MPU_CTRL, mpu_ctrl);
	
	//
	// Interrupt initialization
	//
	uint32_t ccr;
	
	for(i = 0; i < TRAP_MAX_IRQ_COUNT; i ++)
		gInterruptInfo[i].handler = NULL;
	
	gSysCallHandler = NULL;
	
	//
	// NVIC Nested Vectored Interrupt Controller
	//
	// Disable all IRQs
	for(i = 0; i < TRAP_MAX_IRQ_COUNT; i ++)
		disableInterrupt(i);
	
	// Clean pending
	for(i = 0; i < 16; i ++)
		// NVIC_ICPRn Interrupt Clear Pending Register, n = 0 - 15
		REG32_SET(ARMV8M_NVIC_ICPR0 + i * 4, 0xFFFFFFFF);
	
	// CCR Configuration and Control Register
	ccr = REG32_GET(ARMV8M_SCB_CCR);
	// BP [18]
	// 0b1 Program flow prediction enabled for the selected Security state
	BIT_SET(ccr, 18, 0b1);
	// IC [17]
	// 0b1 Instruction caches enabled for the selected Security state
	//BIT_SET(ccr, 17, 0b1);
	// DC [16]
	// 0b1 Data caching enabled
	//BIT_SET(ccr, 16, 0b1);
	REG32_SET(ARMV8M_SCB_CCR, ccr);
	
	//
	// bss zero initialization
	//
	memset(__bss_vmpos, 0, __bss_vmend - __bss_vmpos);
	
	return 0;
}
