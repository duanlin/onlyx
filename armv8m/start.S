#include "ARMV8M.h"


.section .start, "ax"

.global _start
_start:
	// Thread mode expected
	mrs	r0, IPSR
	cmp	r0, #0
	bne	.
	
	// PRIMASK mask exception
	mov	r0, #0b1
	msr	PRIMASK, r0
	
	// Vector
	ldr	r0, =_vector
	ldr	r1, =ARMV8M_SCB_VTOR
	str	r0, [r1]
	
	// Stack
	ldr	r0, =_stack
	mov	sp, r0
	// MSPLIM Main Stack Pointer Limit Register
	ldr	r0, =__stack_vmpos
	msr	MSPLIM, r0
	
	//
	// CONTROL Control Register
	//
	mov	r0, #0
	// SPSEL [1] Stack-pointer select. Defines the stack pointer to be used.
	// 0b0 Use SP_main as the current stack.
	ldr	r1, =~0b10
	and	r0, r0, r1
	// nPRIV [0] Not privileged. Defines the execution privilege in Thread mode.
	// 0b0 Thread mode has privileged access.
	ldr	r1, =~0b1
	and	r0, r0, r1
	
	msr	CONTROL, r0
	
	//
	// CPPWR Coprocessor Power Control Register
	//
	ldr	r1, =ARMV8M_ICB_CPPWR
	ldr	r0, [r1]
	// SUS11 [23] 0b0 Same as SUS10.
	// SUS10 [21] 0b0 The SU10 field is accessible from both Security states.
	ldr	r1, =~0xA00000
	and	r0, r0, r1
	// SU11 [22] 0b0 Same as SU10.
	// SU10 [20] 0b0 The Floating-point and MVE state is not permitted to become UNKNOWN.
	ldr	r1, =~0x500000
	and	r0, r0, r1
	
	ldr	r1, =ARMV8M_ICB_CPPWR
	str	r0, [r1]
	
	//
	// CPACR Coprocessor Access Control Register
	//
	ldr	r1, =ARMV8M_SCB_CPACR
	ldr	r0, [r1]
	// CP11 [23:22] 0b11 Same as CP10.
	// CP10 [21:20] 0b11 Full access to the Floating-point Extension and MVE.
	// CP0-CP7 [15:0] 0b1111111111111111 Full access.
	ldr	r1, =0xF0FFFF
	orr	r0, r0, r1
	
	ldr	r1, =ARMV8M_SCB_CPACR
	str	r0, [r1]
	
	//
	// NSACR Non-secure Access Control Register
	//
	ldr	r1, =ARMV8M_SCB_NSACR
	ldr	r0, [r1]
	// CP11 [11] 0b1 Same as CP10
	// CP10 [10] 0b1 Non-secure access to the Floating-point Extension or MVE permitted.
	// CP0-CP7 [7:0] 0b11111111 Non-secure access to this coprocessor permitted.
	ldr	r1, =0xCFF
	orr	r0, r0, r1
	
	ldr	r1, =ARMV8M_SCB_NSACR
	str	r0, [r1]
	
	//
	// FPDSCR Floating-Point Default Status Control Register
	//
	mov	r0, #0
	// AHP [26] Alternative half-precision. Default value for FPSCR.AHP.
	// 0b0 IEEE half-precision format selected.
	ldr	r1, =~0x4000000
	and	r0, r0, r1
	// DN [25] Default NaN. Default value for FPSCR.DN.
	// 0b1 Any operation involving one of more NaNs returns the Default NaN.
	ldr	r1, =0x2000000
	orr	r0, r0, r1
	// FZ [24] Flush-to-zero. Default value for FPSCR.FZ.
	// 0b0 Flush-to-zero mode disabled. Behavior of the Floating-point unit is fully compliant with the IEEE754 standard.
	ldr	r1, =~0x1000000
	and	r0, r0, r1
	// RMode [23:22] Rounding mode. Default value for FPSCR.RMode.
	// 0b00 Round to Nearest mode.
	ldr	r1, =~0xC00000
	and	r0, r0, r1
	// FZ16 [19] Flush-to-zero mode control bit on half-precision data-processing instruction. Default value for FPSCR.FZ16.
	// 0b0 Flush-to-zero mode disabled. Behavior of the Floating-point unit is fully compliant with the IEEE754 standard.
	ldr	r1, =~0x80000
	and	r0, r0, r1
	
	ldr	r1, =ARMV8M_FPU_FPDSCR
	str	r0, [r1]
	
	//
	// FPCCR Floating-Point Context Control Register
	//
	mov	r0, #0
	// ASPEN [31] Automatic state preservation enable.
	// 0b1 Executing a floating-point instruction sets CONTROL.FPCA to 1.
	ldr	r1, =0x80000000
	orr	r0, r0, r1
	// LSPEN [30] Lazy Floating-point state preservation enable.
	// 0b1 Enable automatic lazy Floating-point state save.
	// LSPENS [29] Lazy Floating-point state preservation enable Secure.
	// 0b0 LSPEN is readable and writable from both Security states.
	ldr	r1, =0x40000000
	orr	r0, r0, r1
	// CLRONRET [28] Clear on return.
	// 0b0 Disabled.
	// CLRONRETS [27] Clrear on return, Secure only.
	// 0b0 The CLRONRET field is accessibly from both Security states.
	ldr	r1, =~0x18000000
	and	r0, r0, r1
	// TS [26] Treat as Secure.
	// 0b0 Disabled
	ldr	r1, =~0x4000000
	and	r0, r0, r1
	
	ldr	r1, =ARMV8M_FPU_FPCCR
	str	r0, [r1]
	
	//
	// FPSCR Floating-point Status and Control Register
	//
	mov	r0, #0
	// AHP [26] Alternative half-precision control bit.
	// 0b0 IEEE half-precision format selected.
	ldr	r1, =~0x4000000
	and	r0, r0, r1
	// DN [25] Default NaN mode control bit.
	// 0b1 Any operation involving one of more NaNs returns the Default NaN.
	ldr	r1, =0x2000000
	orr	r0, r0, r1
	// FZ [24] Flush-to-zero mode control for single and double precision Floating-point.
	// 0b0 Flush-to-zero mode disabled. Behavior of the Floating-point unit fully compliant with the IEEE754 standard.
	ldr	r1, =~0x1000000
	and	r0, r0, r1
	// RMode [23:22] Rounding mode control field.
	// 0b00 Round to Nearest mode.
	ldr	r1, =~0xC00000
	and	r0, r0, r1
	// FZ16 [19] Flush-to-zero mode control bit on half-precision data-processing instructions.
	// 0b0 Flush-to-zero mode disabled. Behavior of the Floating-point unit fully compliant with the IEEE754 standard.
	ldr	r1, =~0x80000
	and	r0, r0, r1
	
	// NOTE:
	// Write to FPSCR will caused CONTROL.FPCA set to 1, and floating-point context is always included.
	vmsr	FPSCR, r0
	
	//
	// MPU_CTRL MPU Control Register
	//
	// PRIVDEFENA [2] Priviledged default enable.
	// 0b0 Use of the system address map disabled. Any instruction or data access that does not access a defined memory region faults.
	// HFNMIENA [1] HardFault, NMI enable. Controls whether handlers executing with a requested execution priority of less than 0 access memory with the MPU enabled or disabled.
	// 0b0 MPU disabled for these handlers.
	// ENABLE [0] Enables the MPU.
	// 0b0 The MPU is disabled.
	mov	r0, #0
	
	ldr	r1, =ARMV8M_MPU_CTRL
	str	r0, [r1]
	
	//
	// CCR Configuration and Control Register
	//
	mov	r0, #0
	// BP [18] Branch prediction enable.
	// 0b0 Program flow prediction disabled for the selected Security state.
	ldr	r1, =~0x40000
	and	r0, r0, r1
	// IC [17] Instruction cache enable.
	// 0b0 Instruction caches disabled for the selected Security state.
	ldr	r1, =~0x20000
	and	r0, r0, r1
	// DC [16] Data cache enable.
	// 0b0 Data caching disabled.
	ldr	r1, =~0x10000
	and	r0, r0, r1
	// STKOFHFNMIGN [10] Stack overflow HardFault and NMI ignore.
	// 0b0 Stack limit faults not ignored.
	ldr	r1, =~0x400
	and	r0, r0, r1
	// BFHFNMIGN [8] BusFault in HardFault or NMI ignore.
	// 0b0 Precise BusFaults not ignored.
	ldr	r1, =~0x100
	and	r0, r0, r1
	// DIV_0_TRP [4] Divide by zero trap.
	// 0b1 UsageFault generation enabled.
	ldr	r1, =0x10
	orr	r0, r0, r1
	// UNALIGN_TRP [3] Unaligned trap.
	// 0b0 Unaligned accesses permitted from LDR, LDRH, STR, and STRH.
	ldr	r1, =~0x8
	and	r0, r0, r1
	// USERSETMPEND [1] User set main pending.
	// 0b1 Unprivileged accessed to the STIR are permitted.
	ldr	r1, =0x2
	orr	r0, r0, r1
	// RES1 [0]
	ldr	r1, =0x1
	orr	r0, r0, r1
	
	ldr	r1, =ARMV8M_SCB_CCR
	str	r0, [r1]
	
	//
	// AIRCR Application Interrupt and Reset Control Register
	//
	// VECTKEY [31:16] Vector key.
	// 0x05FA Writes to the AIRCR must be accompanied by a write of the value 0x05FA to this field.
	ldr	r0, =0x05FA0000
	// ENDIANNESS [15] Data endianness.
	// 0b0 Little-endian.
	ldr	r1, =0x8000
	orr	r0, r0, r1
	// PRIS [14] Prioritize Secure exceptions.
	// 0b0 Priority ranges of Secure and Non-secure exceptions are identical.
	ldr	r1, =~0x4000
	and	r0, r0, r1
	// BFHFNMINS [13] BusFault, HardFault, and NMI Non-secure enable.
	// 0b1 BusFault and NMI are Non-secure and exceptions can target Non-secure HardFault.
	ldr	r1, =0x2000
	orr	r0, r0, r1
	// PRIGROUP [10:8] Priority grouping
	// 0b000 Group priority[7:1], subpriority[0].
	ldr	r1, =~0x700
	and	r0, r0, r1
	// IESB [5] Implicit ESB Enable.
	// 0b0 No Implicit ESB
	ldr	r1, =~0x20
	and	r0, r0, r1
	// DIT [4] Data Independent Timing.
	// 0b0 The architecture makes no statement about the timing properties of any instructions.
	ldr	r1, =~0x10
	and	r0, r0, r1
	// SYSRESETREQS [3] System reset request Secure only.
	// 0b0 SYSRESETREQ functionality is available to both Security states.
	ldr	r1, =~0x8
	and	r0, r0, r1
	// SYSRESETREQ [2] System reset request.
	// 0b0 Do not request a system reset.
	ldr	r1, =~0x4
	and	r0, r0, r1
	// VECTCLRACTIVE [1] Clear active state.
	// 0b0 Do not clear active state.
	ldr	r1, =~0x2
	and	r0, r0, r1
	
	ldr	r1, =ARMV8M_SCB_AIRCR
	str	r0, [r1]
	
	// Bootstrap
	bl	boot
	cmp	r0, #0
	bne	_halt
	
	// Initialize
	bl	init
	cmp	r0, #0
	bne	_halt
	
	//
	// SHCSR System Handler Control and State Register
	//
	mov	r0, #0
	// SECUREFAULTENA [19] SecureFault exception enable.
	// 0b1 SecureFault exception enabled.
	ldr	r1, =0x80000
	orr	r0, r0, r1
	// USGFAULTENA [18] UsageFault exception enable.
	// 0b1 UsageFault exception enabled for the selected Security state.
	ldr	r1, =0x40000
	orr	r0, r0, r1
	// BUSFAULTENA [17] BusFault exception enable.
	// 0b1 BusFault exception enabled.
	ldr	r1, =0x20000
	orr	r0, r0, r1
	// MEMFAULTENA [16] MemManage exception enable.
	// 0b1 MemManage exception enabled for the selected Security state.
	ldr	r1, =0x10000
	orr	r0, r0, r1
	
	ldr	r1, =ARMV8M_SCB_SHCSR
	str	r0, [r1]
	
	// PRIMASK unmask exception
	mov	r0, #0
	msr	PRIMASK, r0
	
	// System
	bl	onlyx

.global _exit
_exit:
	// PRIMASK mask exception
	mov	r0, #0b1
	msr	PRIMASK, r0

_halt:
	b	.
