#include "ARMV8M.h"


.section .vector, "ax"

.global _vector
_vector:
	.dc.a	_stack
	.dc.a	_start + 1 // Additional +1 indicates target is T32
	.dc.a	_nmiHandler + 1
	.dc.a	_hardFaultHandler + 1
	.dc.a	_memManageFaultHandler + 1
	.dc.a	_busFaultHandler + 1
	.dc.a	_usageFaultHandler + 1
	.dc.a	_secureFaultHandler + 1
	.dc.a	_reservedHandler + 1
	.dc.a	_reservedHandler + 1
	.dc.a	_reservedHandler + 1
	.dc.a	_svCallHandler + 1
	.dc.a	_debugMonitorHandler + 1
	.dc.a	_reservedHandler + 1
	.dc.a	_pendSvHandler + 1
	.dc.a	_sysTickHandler + 1
	.dc.a	_extInterruptHandler + 1 // IRQ 0
	.dc.a	_extInterruptHandler + 1 // IRQ 1
	.dc.a	_extInterruptHandler + 1 // IRQ 2
	.dc.a	_extInterruptHandler + 1 // IRQ 3
	.dc.a	_extInterruptHandler + 1 // IRQ 4
	.dc.a	_extInterruptHandler + 1 // IRQ 5
	.dc.a	_extInterruptHandler + 1 // IRQ 6
	.dc.a	_extInterruptHandler + 1 // IRQ 7
	.dc.a	_extInterruptHandler + 1 // IRQ 8
	.dc.a	_extInterruptHandler + 1 // IRQ 9
	.dc.a	_extInterruptHandler + 1 // IRQ 10
	.dc.a	_extInterruptHandler + 1 // IRQ 11
	.dc.a	_extInterruptHandler + 1 // IRQ 12
	.dc.a	_extInterruptHandler + 1 // IRQ 13
	.dc.a	_extInterruptHandler + 1 // IRQ 14
	.dc.a	_extInterruptHandler + 1 // IRQ 15
	.dc.a	_extInterruptHandler + 1 // IRQ 16
	.dc.a	_extInterruptHandler + 1 // IRQ 17
	.dc.a	_extInterruptHandler + 1 // IRQ 18
	.dc.a	_extInterruptHandler + 1 // IRQ 19
	.dc.a	_extInterruptHandler + 1 // IRQ 20
	.dc.a	_extInterruptHandler + 1 // IRQ 21
	.dc.a	_extInterruptHandler + 1 // IRQ 22
	.dc.a	_extInterruptHandler + 1 // IRQ 23
	.dc.a	_extInterruptHandler + 1 // IRQ 24
	.dc.a	_extInterruptHandler + 1 // IRQ 25
	.dc.a	_extInterruptHandler + 1 // IRQ 26
	.dc.a	_extInterruptHandler + 1 // IRQ 27
	.dc.a	_extInterruptHandler + 1 // IRQ 28
	.dc.a	_extInterruptHandler + 1 // IRQ 29
	.dc.a	_extInterruptHandler + 1 // IRQ 30
	.dc.a	_extInterruptHandler + 1 // IRQ 31
	.dc.a	_extInterruptHandler + 1 // IRQ 32
	.dc.a	_extInterruptHandler + 1 // IRQ 33
	.dc.a	_extInterruptHandler + 1 // IRQ 34
	.dc.a	_extInterruptHandler + 1 // IRQ 35
	.dc.a	_extInterruptHandler + 1 // IRQ 36
	.dc.a	_extInterruptHandler + 1 // IRQ 37
	.dc.a	_extInterruptHandler + 1 // IRQ 38
	.dc.a	_extInterruptHandler + 1 // IRQ 39
	.dc.a	_extInterruptHandler + 1 // IRQ 40
	.dc.a	_extInterruptHandler + 1 // IRQ 41
	.dc.a	_extInterruptHandler + 1 // IRQ 42
	.dc.a	_extInterruptHandler + 1 // IRQ 43
	.dc.a	_extInterruptHandler + 1 // IRQ 44
	.dc.a	_extInterruptHandler + 1 // IRQ 45
	.dc.a	_extInterruptHandler + 1 // IRQ 46
	.dc.a	_extInterruptHandler + 1 // IRQ 47
	.dc.a	_extInterruptHandler + 1 // IRQ 48
	.dc.a	_extInterruptHandler + 1 // IRQ 49
	.dc.a	_extInterruptHandler + 1 // IRQ 50
	.dc.a	_extInterruptHandler + 1 // IRQ 51
	.dc.a	_extInterruptHandler + 1 // IRQ 52
	.dc.a	_extInterruptHandler + 1 // IRQ 53
	.dc.a	_extInterruptHandler + 1 // IRQ 54
	.dc.a	_extInterruptHandler + 1 // IRQ 55
	.dc.a	_extInterruptHandler + 1 // IRQ 56
	.dc.a	_extInterruptHandler + 1 // IRQ 57
	.dc.a	_extInterruptHandler + 1 // IRQ 58
	.dc.a	_extInterruptHandler + 1 // IRQ 59
	.dc.a	_extInterruptHandler + 1 // IRQ 60
	.dc.a	_extInterruptHandler + 1 // IRQ 61
	.dc.a	_extInterruptHandler + 1 // IRQ 62
	.dc.a	_extInterruptHandler + 1 // IRQ 63
	.dc.a	_extInterruptHandler + 1 // IRQ 64
	.dc.a	_extInterruptHandler + 1 // IRQ 65
	.dc.a	_extInterruptHandler + 1 // IRQ 66
	.dc.a	_extInterruptHandler + 1 // IRQ 67
	.dc.a	_extInterruptHandler + 1 // IRQ 68
	.dc.a	_extInterruptHandler + 1 // IRQ 69
	.dc.a	_extInterruptHandler + 1 // IRQ 70
	.dc.a	_extInterruptHandler + 1 // IRQ 71
	.dc.a	_extInterruptHandler + 1 // IRQ 72
	.dc.a	_extInterruptHandler + 1 // IRQ 73
	.dc.a	_extInterruptHandler + 1 // IRQ 74
	.dc.a	_extInterruptHandler + 1 // IRQ 75
	.dc.a	_extInterruptHandler + 1 // IRQ 76
	.dc.a	_extInterruptHandler + 1 // IRQ 77
	.dc.a	_extInterruptHandler + 1 // IRQ 78
	.dc.a	_extInterruptHandler + 1 // IRQ 79
	.dc.a	_extInterruptHandler + 1 // IRQ 80
	.dc.a	_extInterruptHandler + 1 // IRQ 81
	.dc.a	_extInterruptHandler + 1 // IRQ 82
	.dc.a	_extInterruptHandler + 1 // IRQ 83
	.dc.a	_extInterruptHandler + 1 // IRQ 84
	.dc.a	_extInterruptHandler + 1 // IRQ 85
	.dc.a	_extInterruptHandler + 1 // IRQ 86
	.dc.a	_extInterruptHandler + 1 // IRQ 87


.align 9
_nmiHandler:
	push	{r4, lr}
	
	// int nmiHandler(void)
	bl	nmiHandler
	cmp	r0, #0
	bne	_halt
	
	pop	{r4, pc}

_hardFaultHandler:
	mov	r0, sp
	
	ldr	r1, =ARMV8M_SCB_HFSR
	ldr	r1, [r1]
	
	ldr	r2, =ARMV8M_SCB_CFSR
	ldr	r2, [r2]
	
	// int hardFaultHandler(const TrapContext* context, uint32_t hfsr, uint32_t cfsr)
	bl	hardFaultHandler
	cmp	r0, #0
	bne	_halt
	
	bx	lr

_memManageFaultHandler:
	mov	r0, sp
	
	ldr	r1, =ARMV8M_SCB_CFSR
	ldrb	r1, [r1]
	
	ldr	r2, =ARMV8M_SCB_MMFAR
	ldr	r2, [r2]
	
	// int memManageFaultHandler(const TrapContext* context, uint8_t mmfsr, uint32_t mmfar)
	bl	memManageFaultHandler
	cmp	r0, #0
	bne	_halt
	
	bx	lr

_busFaultHandler:
	mov	r0, sp
	push	{r4, lr}
	
	ldr	r1, =ARMV8M_SCB_CFSR
	ldrb	r1, [r1, #+1]
	
	ldr	r2, =ARMV8M_SCB_BFAR
	ldr	r2, [r2]
	
	// int busFaultHandler(const TrapContext* context, uint8_t bfsr, uint32_t bfar)
	bl	busFaultHandler
	cmp	r0, #0
	bne	_halt
	
	bx	lr

_usageFaultHandler:
	mov	r0, sp
	
	ldr	r1, =ARMV8M_SCB_CFSR
	ldrh	r1, [r1, #+2]
	
	// int usageFaultHandler(const TrapContext* context, uint32_t ufsr)
	bl	usageFaultHandler
	cmp	r0, #0
	bne	_halt
	
	bx	lr

_secureFaultHandler:
	mov	r0, sp
	
	ldr	r1, =ARMV8M_SAU_SFSR
	ldr	r1, [r1]
	
	ldr	r2, =ARMV8M_SAU_SFAR
	ldr	r2, [r2]
	
	// int secureFaultHandler(const TrapContext* context, uint32_t sfsr, uint32_t sfar)
	bl	secureFaultHandler
	cmp	r0, #0
	bne	_halt
	
	bx	lr

_svCallHandler:
	mov	r0, sp
	push	{r4, lr}
	
	// int svCallHandler(TrapContext* context)
	bl	svCallHandler
	cmp	r0, #0
	bne	_halt
	
	pop	{r4, pc}

_debugMonitorHandler:
	push	{r4, lr}
	
	// int debugMonitorHandler(void)
	bl	debugMonitorHandler
	cmp	r0, #0
	bne	_halt
	
	pop	{r4, pc}

_pendSvHandler:
	push	{r4, lr}
	
	// int pendSvHandler(void)
	bl	pendSvHandler
	cmp	r0, #0
	bne	_halt
	
	pop	{r4, pc}

_sysTickHandler:
	push	{r4, lr}
	
	// int sysTickHandler(void)
	bl	sysTickHandler
	cmp	r0, #0
	bne	_halt
	
	pop	{r4, pc}
	
_extInterruptHandler:
	push	{r4, lr}
	
	mrs	r0, IPSR
	cmp	r0, #16
	blt	_halt
	
	sub	r0, r0, #16
	
	// int extInterruptHandler(unsigned irq)
	bl	extInterruptHandler
	cmp	r0, #0
	bne	_halt
	
	pop	{r4, pc}

_reservedHandler:
	b	.

_halt:
	b	.


.weak nmiHandler
nmiHandler:
	b	.

.weak hardFaultHandler
hardFaultHandler:
	b	.

.weak memManageFaultHandler
memManageFaultHandler:
	b	.

.weak busFaultHandler
busFaultHandler:
	b	.

.weak usageFaultHandler
usageFaultHandler:
	b	.

.weak secureFaultHandler
secureFaultHandler:
	b	.

.weak svCallHandler
svCallHandler:
	b	.

.weak debugMonitorHandler
debugMonitorHandler:
	b	.

.weak pendSvHandler
pendSvHandler:
	b	.

.weak sysTickHandler
sysTickHandler:
	b	.

.global sysCall
sysCall:
	svc	#0
	bx	lr
