.section .start, "ax"

.global _start
_start:
	// EL1 expected
	mrs	x0, CurrentEL
	lsr	x0, x0, #2
	and	x0, x0, #0b11
	cmp	x0, #1 // EL1
	bne	.
	
	// DAIFSet mask exception
	msr	DAIFSet, #0b1111
	
	// Vector
	ldr	x0, =_vector
	msr	VBAR_EL1, x0
	
	// Stack
	ldr	fp, =_stack
	mov	sp, fp
	
	msr	SP_EL0, fp
	msr	SPSel, #0b1
	
	// PSTATE
	msr	PAN, #0b0
	msr	UAO, #0b1
	
	//
	// CPACR_EL1 Architectural Feature Access Control Register
	//
	mrs	x0, CPACR_EL1
	// TTA [28] Traps EL0 and EL1 System register accesses to all implemented trace registers from both Execution states to EL1, or to EL2 when it is implemented and enabled in the current Security state and HCR_EL2.TGE is 1
	// 0b1 This control causes EL0 and EL1 System register accesses to all implemented trace registers to be trapped.
	orr	x0, x0, #0x10000000
	// FPEN [21:20] Traps execution at EL1 and EL0 of instructions that access the Advanced SIMD and floating-point registers from both Execution states to EL1, reported using ESR_ELx.EC value 0x07, or to EL2 reported using ESR_ELx.EC value 0x00 when EL2 is implemented and enabled in the current Security state and HCR_EL2.TGE is 1
	// 0b11 This control does not cause execution of any instructions to be trapped.
	orr	x0, x0, #0x300000
	msr	CPACR_EL1, x0
	
	//
	// FPCR Floating-point Control Register
	//
	mov	x0, #0
	// AHP [26] Alternative half-precision control bit.
	// 0b0 IEEE half-precision format selected.
	and	x0, x0, #~0x4000000
	// DN [25] Default NaN use for NaN propagation.
	// 0b0 NaN operands propagate through to the output of a floating-point operation.
	and	x0, x0, #~0x2000000
	// FZ [24] Flushing denormalized numbers to zero control bit.
	// 0b0 If FPCR.AH is 0, the flushing to zero of single-precision and double-precision denormalized inputs to, and outputs of, floating-point instructions not enabled by this control, but other factors might cause the input denormalized numbers to be flushed to zero.
	// If FPCR.AH is 1, the flushing to zero of single-precision and double-precision denormalized outputs of floating-point instructions not enabled by this control, but other factors might cause the input denormalized numbers to be flushed to zero.
	and	x0, x0, #~0x1000000
	// RMode [23:22] Rounding Mode control field.
	// 0b00 Round to Nearest (RN) mode.
	and	x0, x0, #~0xC00000
	// FZ16 [19]
	// When FEAT_FP16 is implemented:
	// Flushing denormalized numbers to zero control bit on half-precision data-processing instructions.
	// 0b0 For some instructions, this bit disables flushing to zero of inputs and outputs that are half-precision denormalized numbers.
	and	x0, x0, #~0x80000
	// IDE [15] Input Denormal floating-point exception trap enable.
	// 0b0 Untrapped exception handling selected. If the floating-point exception occurs, the FPSR.IDC bit is set to 1.
	and	x0, x0, #~0x8000
	// IXE [12] Inexact floating-point exception trap enable.
	// 0b0 Untrapped exception handling selected. If the floating-point exception occurs, the FPSR.IXC bit is set to 1.
	and	x0, x0, #~0x1000
	// UFE [11] Underflow floating-point exception trap enable.
	// 0b0 Untrapped exception handling selected. If the floating-point exception occurs, the FPSR.UFC bit is set to 1.
	and	x0, x0, #~0x800
	// OFE [10] Overflow floating-point exception trap enable.
	// 0b0 Untrapped exception handling selected. If the floating-point exception occurs, the FPSR.OFC bit is set to 1.
	and	x0, x0, #~0x400
	// DZE [9] Divide by Zero floating-point exception trap enable.
	// 0b0 Untrapped exception handling selected. If the floating-point exception occurs, the FPSR.DZC bit is set to 1.
	and	x0, x0, #~0x200
	// IOE [8] Invalid Operation floating-point exception trap enable.
	// 0b0 Untrapped exception handling selected. If the floating-point exception occurs, the FPSR.IOC bit is set to 1.
	and	x0, x0, #~0x100
	msr	FPCR, x0
	
	//
	// SCTLR_EL1 System Control Register (EL1)
	//
	mov	x0, #0
	// DSSBS [44] Default PSTATE.SSBS value on Exception Entry.
	// 0b0 PSTATE.SSBS is set to 0 on an exception to EL1.
	and	x0, x0, #~0x100000000000
	orr	x0, x0, #0x20000000 // LSMAOE [29] RES1
	orr	x0, x0, #0x10000000 // nTLSMD [28] RES1
	// UCI [26] Traps EL0 execution of cache maintenance instructions, to EL1, or to EL2 when it is implemented and enabled for the current Security state and HCR_EL2.TGE is 1, from AArch64 state only, reported using an ESR_ELx.EC value of 0x18. This applies to DC CVAU, DC CIVAC, DC CVAC, DC CVAP, and IC IVAU.
	// 0b0 Execution of the specified instructions at EL0 using AArch64 is trapped.
	and	x0, x0, #~0x4000000
	// EE [25] Endianness of data accesses at EL1, and stage 1 translation table walks in the EL1&0 translation regime.
	// 0b0 Explicit data accesses at EL1, and stage 1 translation table walks in the EL1&0 translation regime are little-endian.
	and	x0, x0, #~0x2000000
	// E0E [24] Endianness of data accesses at EL0.
	// 0b0 Explicit data accesses at EL0 are little-endian.
	and	x0, x0, #~0x1000000
	// SPAN [23] Set Privileged Access Never, on taking an exception to EL1.
	// 0b1 The value of PSTATE.PAN is left unchanged on taking an exception to EL1.
	orr	x0, x0, #0x800000
	orr	x0, x0, #0x400000 // EIS [22] RES1
	// IESB [21] When FEAT_IESB is implemented: Implicit Error Synchronization event enable. Possible values are:
	// 0b0 Disabled.
	and	x0, x0, #~0x200000
	orr	x0, x0, #0x100000 // TSCXT [20] RES1
	// WXN [19] Write permission implies XN (Execute-never). For the EL1&0 translation regime, this bit can force all memory regions that are writable to be treated as XN.
	// 0b0 This control has no effect on memory access permissions
	and	x0, x0, #~0x80000
	// nTWE [18] Traps EL0 execution of WFE instructions to EL1, or to EL2 when it is implemented and enabled for the current Security state and HCR_EL2.TGE is 1, from both Execution states, reported using an ESR_ELx.EC value of 0x01.
	// 0b0 Any attempt to execute a WFE instruction at EL0 is trapped, if the instruction would otherwise have caused the PE to enter a low-power state.
	and	x0, x0, #~0x40000
	// nTWI [16] Traps EL0 execution of WFI instructions to EL1, or to EL2 when it is implemented and enabled for the current Security state and HCR_EL2.TGE is 1, from both Execution states, reported using an ESR_ELx.EC value of 0x01.
	// 0b0 Any attempt to execute a WFI instruction at EL0 is trapped, if the instruction would otherwise have caused the PE to enter a low-power state.
	and	x0, x0, #~0x10000
	// UCT [15] Traps EL0 accesses to the CTR_EL0 to EL1, or to EL2 when it is implemented and enabled for the current Security state and HCR_EL2.TGE is 1, from AArch64 state only, reported using an ESR_ELx.EC value of 0x18.
	// 0b0 Accesses to the CTR_EL0 from EL0 using AArch64 are trapped.
	and	x0, x0, #~0x8000
	// DZE [14] Traps EL0 execution of DC ZVA instructions to EL1, or to EL2 when it is implemented and enabled for the current Security state and HCR_EL2.TGE is 1, from AArch64 state only, reported using an ESR_ELx.EC value of 0x18.
	// 0b0 Any attempt to execute an instruction that this trap applies to at EL0 using AArch64 is trapped. Reading DCZID_EL0.DZP from EL0 returns 1, indicating that the instructions this trap applies to are not supported.
	and	x0, x0, #~0x4000
	// I [12] Stage 1 instruction access Cacheability control, for accesses at EL0 and EL1.
	// 0b0 All instruction access to Stage 1 Normal memory from EL0 and EL1 are Stage 1 Non-cacheable. If the value of SCTLR_EL1.M is 0, instruction accesses from stage 1 of the EL1&0 translation regime are to Normal, Outer Shareable, Inner Non-cacheable, Outer Non-cacheable memory.
	and	x0, x0, #~0x1000
	orr	x0, x0, #0x800 // EOS [11] RES1
	// UMA [9] User Mask Access. Traps EL0 execution of MSR and MRS instructions that access the PSTATE.{D, A, I, F} masks to EL1, or to EL2 when it is implemented and enabled for the current Security state and HCR_EL2.TGE is 1, from AArch64 state only, reported using an ESR_ELx.EC value of 0x18.
	// 0b0 Any attempt at EL0 using AArch64 to execute an MRS, MSR(register), or MSR(immediate) instruction that accesses the DAIF is trapped.
	and	x0, x0, #~0x200
	// SED [8] When EL0 is capable of using AArch32: SETEND instruction disable. Disables SETEND instructions at EL0 using AArch32.
	// 0b1 SETEND instructions are UNDEFINED at EL0 using AArch32 and any attempt at EL0 to access a SETEND instruction generates an exception to EL1, or to EL2 when it is implemented and enabled for the current Security state and HCR_EL2.TGE is 1, reported using an ESR_ELx.EC value of 0x00.
	orr	x0, x0, #0x100
	// ITD [7] When EL0 is capable of using AArch32: IT Disable. Disables some uses of IT instructions at EL0 using AArch32.
	// 0b1 Any attempt at EL0 using AArch32 to execute any of the following is UNDEFINED and generates an exception, reported using an ESR_ELx.EC value of 0x00.
	orr	x0, x0, #0x80
	// CP15BEN [5] When EL0 is capable of using AArch32: System instruction memory barrier enable. Enables accesses to the DMB, DSB, and ISB System instructions in the (coproc==0b1111) encoding space from EL0:
	// 0b0 EL0 using AArch32: EL0 execution of the CP15DMB, CP15DSB, and CP15ISB instructions is UNDEFINED and generates an exception to EL1, or to EL2 when it is implemented and enabled for the current Security state and HCR_EL2.TGE is 1. The exception is reported using an ESR_ELx.EC value of 0x00.
	and	x0, x0, #~0x20
	// SA0 [4] SP Alignment check enable for EL0. When set to 1, if a load or store instruction executed at EL0 uses the SP as the base address and the SP is not aligned to a 16-byte boundary, then an SP alignment fault exception is generated.
	orr	x0, x0, #0x10
	// SA [3] SP Alignment check enable. When set to 1, if a load or store instruction executed at EL1 uses the SP as the base address and the SP is not aligned to a 16-byte boundary, then an SP alignment fault exception is generated.
	orr	x0, x0, #0x8
	// C [2] Stage 1 Cacheability control, for data accesses.
	// 0b0 All data access to Stage 1 Normal memory from EL0 and EL1, and all Normal memory accesses from unified cache to the EL1&0 Stage 1 translation tables, are treated as Stage 1 Non-cacheable.
	and	x0, x0, #~0x4
	// A [1] Alignment check enable. This is the enable bit for Alignment fault checking at EL1 and EL0.
	// 0b0 Alignment fault checking disabled when executing at EL1 or EL0. Instructions that load or store one or more registers, other than load/store exclusive and load-acquire/store-release, do not check that the address being accessed is aligned to the size of the data element(s) being accessed.
	and	x0, x0, #~0x2
	// M [0] MMU enable for EL1&0 stage 1 address translation.
	// 0b0 EL1&0 stage 1 address translation disabled. See the SCTLR_EL1.I field for the behavior of instruction accesses to Normal memory.
	and	x0, x0, #~0x1
	msr	SCTLR_EL1, x0
	
	// Initialize
	bl	init
	cmp	w0, #0
	bne	_halt
	
	// DAIFClr enable exception
	msr	DAIFClr, #0b1111
	
	// System
	bl	onlyx
	
.global _exit
_exit:
	// DAIFSet mask exception
	msr	DAIFSet, #0b1111
	
_halt:
	b	.
