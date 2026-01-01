.section .vector, "ax"

.global _vector
_vector:
	//
	// Current exception level with SP_EL0
	//
	
	// Synchronous
	.org	_vector + 0x000
	b	.
	
	// IRQ or vIRQ
	.org	_vector + 0x080
	b	.
	
	// FIQ or vFIQ
	.org	_vector + 0x100
	b	.
	
	// SError or vSError
	.org	_vector + 0x180
	b	.
	
	//
	// Current exception level with SP_ELx
	//
	
	// Synchronous
	.org	_vector + 0x200
	b	_synExceptionELx
	
	// IRQ or vIRQ
	.org	_vector + 0x280
	b	_irqExceptionELx
	
	// FIQ or vFIQ
	.org	_vector + 0x300
	b	_fiqExceptionELx
	
	// SError or vSError
	.org	_vector + 0x380
	b	_seiExceptionELx
	
	//
	// Lower exception level, where the implemented level immediately lower than the target level is using AArch64
	//
	.org	_vector + 0x400
	b	.
	.org	_vector + 0x480
	b	.
	.org	_vector + 0x500
	b	.
	.org	_vector + 0x580
	b	.
	
	//
	// Lower exception level, where the implemented level immediately lower than the target level is using AArch32
	//
	.org	_vector + 0x600
	b	.
	.org	_vector + 0x680
	b	.
	.org	_vector + 0x700
	b	.
	.org	_vector + 0x780
	b	.
	
	// The vector end
	.org	_vector + 0x800

.align	6 // Cache line align
_synExceptionELx:
	// Enter critical
	//
	stp	x29, x30, [sp, #-16]!
	// x19-x28 Callee-saved
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!
	
	sub	sp, sp, #16
	str	x0, [sp, #8]
	
	mrs	x0, SPSR_EL1
	str	w0, [sp, #4]
	
	mrs	x0, FPSR
	str	w0, [sp]
	
	stp	q30, q31, [sp, #-32]!
	stp	q28, q29, [sp, #-32]!
	stp	q26, q27, [sp, #-32]!
	stp	q24, q25, [sp, #-32]!
	stp	q22, q23, [sp, #-32]!
	stp	q20, q21, [sp, #-32]!
	stp	q18, q19, [sp, #-32]!
	stp	q16, q17, [sp, #-32]!
	stp	q14, q15, [sp, #-32]!
	stp	q12, q13, [sp, #-32]!
	stp	q10, q11, [sp, #-32]!
	stp	q8, q9, [sp, #-32]!
	stp	q6, q7, [sp, #-32]!
	stp	q4, q5, [sp, #-32]!
	stp	q2, q3, [sp, #-32]!
	stp	q0, q1, [sp, #-32]!
	
	// For stack unwind
	mrs	x30, ELR_EL1
	stp	x29, x30, [sp, #-16]!
	mov	fp, sp
	
	// ICC_PMR_EL1 Interrupt Controller Interrupt Priority Mask Register
	// Priority [7:0] The priority mask level for the CPU interface. If the priority of an interrupt is higher than the value indicated by this field, the interface signals the interrupt to the PE.
	mov	x0, #0x01 // Second highest
	msr	S3_0_C4_C6_0, x0
	
	// Handler parameters
	mov	x0, sp
	mrs	x1, ESR_EL1
	mrs	x2, FAR_EL1
	
	// Preemption allowed
	msr	DAIFClr, #0b1111
	
	//
	// Exit critical
	
	//
	// int synExceptionHandler(const TrapContext* context, uint64_t ESR, uintptr_t FAR)
	//
	bl	synExceptionHandler
	
	// Enter critical
	//
	
	// Preemption forbid
	msr	DAIFSet, #0b1111
	
	cmp	w0, #0
	bne	_halt
	
	// ICC_PMR_EL1 Interrupt Controller Interrupt Priority Mask Register
	// Priority [7:0] The priority mask level for the CPU interface. If the priority of an interrupt is higher than the value indicated by this field, the interface signals the interrupt to the PE.
	mov	x0, #0b11111111
	msr	S3_0_C4_C6_0, x0
	
	// Context recovering
	//
	ldp	x29, x30, [sp], #16
	msr	ELR_EL1, x30
	
	ldp	q0, q1, [sp], #32
	ldp	q2, q3, [sp], #32
	ldp	q4, q5, [sp], #32
	ldp	q6, q7, [sp], #32
	ldp	q8, q9, [sp], #32
	ldp	q10, q11, [sp], #32
	ldp	q12, q13, [sp], #32
	ldp	q14, q15, [sp], #32
	ldp	q16, q17, [sp], #32
	ldp	q18, q19, [sp], #32
	ldp	q20, q21, [sp], #32
	ldp	q22, q23, [sp], #32
	ldp	q24, q25, [sp], #32
	ldp	q26, q27, [sp], #32
	ldp	q28, q29, [sp], #32
	ldp	q30, q31, [sp], #32
	
	ldr	w0, [sp]
	msr	FPSR, x0
	
	ldr	w0, [sp, #4]
	msr	SPSR_EL1, x0
	
	ldr	x0, [sp, #8]
	add	sp, sp, #16
	
	ldp	x1, x2, [sp], #16
	ldp	x3, x4, [sp], #16
	ldp	x5, x6, [sp], #16
	ldp	x7, x8, [sp], #16
	ldp	x9, x10, [sp], #16
	ldp	x11, x12, [sp], #16
	ldp	x13, x14, [sp], #16
	ldp	x15, x16, [sp], #16
	ldp	x17, x18, [sp], #16
	// x19-x28 Callee-saved
	ldp	x29, x30, [sp], #16
	
	eret

.align	6 // Cache line align
_irqExceptionELx:
	// Enter critical
	//
	stp	x29, x30, [sp, #-16]!
	// x19-x28 Callee-saved
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!
	
	sub	sp, sp, #16
	str	x0, [sp, #8]
	
	mrs	x0, SPSR_EL1
	str	w0, [sp, #4]
	
	mrs	x0, FPSR
	str	w0, [sp]
	
	stp	q30, q31, [sp, #-32]!
	stp	q28, q29, [sp, #-32]!
	stp	q26, q27, [sp, #-32]!
	stp	q24, q25, [sp, #-32]!
	stp	q22, q23, [sp, #-32]!
	stp	q20, q21, [sp, #-32]!
	stp	q18, q19, [sp, #-32]!
	stp	q16, q17, [sp, #-32]!
	stp	q14, q15, [sp, #-32]!
	stp	q12, q13, [sp, #-32]!
	stp	q10, q11, [sp, #-32]!
	stp	q8, q9, [sp, #-32]!
	stp	q6, q7, [sp, #-32]!
	stp	q4, q5, [sp, #-32]!
	stp	q2, q3, [sp, #-32]!
	stp	q0, q1, [sp, #-32]!
	
	// For stack unwind
	mrs	x30, ELR_EL1
	stp	x29, x30, [sp, #-16]!
	mov	fp, sp
	
	// Handler parameters
	mov	x0, sp
	mrs	x1, S3_0_C12_C12_0 // ICC_IAR1_EL1: INTID
	
	// Preemption allowed
	msr	DAIFClr, #0b1111
	
	//
	// Exit critical
	
	//
	// int intExceptionHandler(const TrapContext* context, unsigned INTID)
	//
	bl	intExceptionHandler
	
	// Enter critical
	//
	
	// Preemption forbid
	msr	DAIFSet, #0b1111
	
	cmp	w0, #0
	blt	_halt
	
	// ICC_EOIR1_EL1
	msr	S3_0_C12_C12_1, x0
	
	// Context recovering
	//
	ldp	x29, x30, [sp], #16
	msr	ELR_EL1, x30
	
	ldp	q0, q1, [sp], #32
	ldp	q2, q3, [sp], #32
	ldp	q4, q5, [sp], #32
	ldp	q6, q7, [sp], #32
	ldp	q8, q9, [sp], #32
	ldp	q10, q11, [sp], #32
	ldp	q12, q13, [sp], #32
	ldp	q14, q15, [sp], #32
	ldp	q16, q17, [sp], #32
	ldp	q18, q19, [sp], #32
	ldp	q20, q21, [sp], #32
	ldp	q22, q23, [sp], #32
	ldp	q24, q25, [sp], #32
	ldp	q26, q27, [sp], #32
	ldp	q28, q29, [sp], #32
	ldp	q30, q31, [sp], #32
	
	ldr	w0, [sp]
	msr	FPSR, x0
	
	ldr	w0, [sp, #4]
	msr	SPSR_EL1, x0
	
	ldr	x0, [sp, #8]
	add	sp, sp, #16
	
	ldp	x1, x2, [sp], #16
	ldp	x3, x4, [sp], #16
	ldp	x5, x6, [sp], #16
	ldp	x7, x8, [sp], #16
	ldp	x9, x10, [sp], #16
	ldp	x11, x12, [sp], #16
	ldp	x13, x14, [sp], #16
	ldp	x15, x16, [sp], #16
	ldp	x17, x18, [sp], #16
	// x19-x28 Callee-saved
	ldp	x29, x30, [sp], #16
	
	eret

.align	6 // Cache line align
_fiqExceptionELx:
	// Enter critical
	//
	stp	x29, x30, [sp, #-16]!
	// x19-x28 Callee-saved
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!
	
	sub	sp, sp, #16
	str	x0, [sp, #8]
	
	mrs	x0, SPSR_EL1
	str	w0, [sp, #4]
	
	mrs	x0, FPSR
	str	w0, [sp]
	
	stp	q30, q31, [sp, #-32]!
	stp	q28, q29, [sp, #-32]!
	stp	q26, q27, [sp, #-32]!
	stp	q24, q25, [sp, #-32]!
	stp	q22, q23, [sp, #-32]!
	stp	q20, q21, [sp, #-32]!
	stp	q18, q19, [sp, #-32]!
	stp	q16, q17, [sp, #-32]!
	stp	q14, q15, [sp, #-32]!
	stp	q12, q13, [sp, #-32]!
	stp	q10, q11, [sp, #-32]!
	stp	q8, q9, [sp, #-32]!
	stp	q6, q7, [sp, #-32]!
	stp	q4, q5, [sp, #-32]!
	stp	q2, q3, [sp, #-32]!
	stp	q0, q1, [sp, #-32]!
	
	// For stack unwind
	mrs	x30, ELR_EL1
	stp	x29, x30, [sp, #-16]!
	mov	fp, sp
	
	// Handler parameters
	mov	x0, sp
	mrs	x1, S3_0_C12_C12_0 // ICC_IAR1_EL1: INTID
	
	// Preemption allowed
	msr	DAIFClr, #0b1111
	
	//
	// Exit critical
	
	//
	// int intExceptionHandler(const TrapContext* context, unsigned INTID)
	//
	bl	intExceptionHandler
	
	// Enter critical
	//
	
	// Preemption forbid
	msr	DAIFSet, #0b1111
	
	cmp	w0, #0
	blt	_halt
	
	// ICC_EOIR1_EL1
	msr	S3_0_C12_C12_1, x0
	
	// Context recovering
	//
	ldp	x29, x30, [sp], #16
	msr	ELR_EL1, x30
	
	ldp	q0, q1, [sp], #32
	ldp	q2, q3, [sp], #32
	ldp	q4, q5, [sp], #32
	ldp	q6, q7, [sp], #32
	ldp	q8, q9, [sp], #32
	ldp	q10, q11, [sp], #32
	ldp	q12, q13, [sp], #32
	ldp	q14, q15, [sp], #32
	ldp	q16, q17, [sp], #32
	ldp	q18, q19, [sp], #32
	ldp	q20, q21, [sp], #32
	ldp	q22, q23, [sp], #32
	ldp	q24, q25, [sp], #32
	ldp	q26, q27, [sp], #32
	ldp	q28, q29, [sp], #32
	ldp	q30, q31, [sp], #32
	
	ldr	w0, [sp]
	msr	FPSR, x0
	
	ldr	w0, [sp, #4]
	msr	SPSR_EL1, x0
	
	ldr	x0, [sp, #8]
	add	sp, sp, #16
	
	ldp	x1, x2, [sp], #16
	ldp	x3, x4, [sp], #16
	ldp	x5, x6, [sp], #16
	ldp	x7, x8, [sp], #16
	ldp	x9, x10, [sp], #16
	ldp	x11, x12, [sp], #16
	ldp	x13, x14, [sp], #16
	ldp	x15, x16, [sp], #16
	ldp	x17, x18, [sp], #16
	// x19-x28 Callee-saved
	ldp	x29, x30, [sp], #16
	
	eret

.align	6 // Cache line align
_seiExceptionELx:
	// Enter critical
	//
	stp	x29, x30, [sp, #-16]!
	// x19-x28 Callee-saved
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!
	
	sub	sp, sp, #16
	str	x0, [sp, #8]
	
	mrs	x0, SPSR_EL1
	str	w0, [sp, #4]
	
	mrs	x0, FPSR
	str	w0, [sp]
	
	stp	q30, q31, [sp, #-32]!
	stp	q28, q29, [sp, #-32]!
	stp	q26, q27, [sp, #-32]!
	stp	q24, q25, [sp, #-32]!
	stp	q22, q23, [sp, #-32]!
	stp	q20, q21, [sp, #-32]!
	stp	q18, q19, [sp, #-32]!
	stp	q16, q17, [sp, #-32]!
	stp	q14, q15, [sp, #-32]!
	stp	q12, q13, [sp, #-32]!
	stp	q10, q11, [sp, #-32]!
	stp	q8, q9, [sp, #-32]!
	stp	q6, q7, [sp, #-32]!
	stp	q4, q5, [sp, #-32]!
	stp	q2, q3, [sp, #-32]!
	stp	q0, q1, [sp, #-32]!
	
	// For stack unwind
	mrs	x30, ELR_EL1
	stp	x29, x30, [sp, #-16]!
	mov	fp, sp
	
	// Handler parameters
	mov	x0, sp
	mrs	x1, ESR_EL1
	mrs	x2, FAR_EL1
	
	// Preemption allowed
	msr	DAIFClr, #0b1111
	
	//
	// Exit critical
	
	//
	// int errExceptionHandler(const TrapContext* context, uint64_t ESR, uintptr_t FAR)
	//
	bl	errExceptionHandler
	
	// Enter critical
	//
	
	// Preemption forbid
	msr	DAIFSet, #0b1111
	
	cmp	w0, #0
	bne	_halt
	
	// Context recovering
	//
	ldp	x29, x30, [sp], #16
	msr	ELR_EL1, x30
	
	ldp	q0, q1, [sp], #32
	ldp	q2, q3, [sp], #32
	ldp	q4, q5, [sp], #32
	ldp	q6, q7, [sp], #32
	ldp	q8, q9, [sp], #32
	ldp	q10, q11, [sp], #32
	ldp	q12, q13, [sp], #32
	ldp	q14, q15, [sp], #32
	ldp	q16, q17, [sp], #32
	ldp	q18, q19, [sp], #32
	ldp	q20, q21, [sp], #32
	ldp	q22, q23, [sp], #32
	ldp	q24, q25, [sp], #32
	ldp	q26, q27, [sp], #32
	ldp	q28, q29, [sp], #32
	ldp	q30, q31, [sp], #32
	
	ldr	w0, [sp]
	msr	FPSR, x0
	
	ldr	w0, [sp, #4]
	msr	SPSR_EL1, x0
	
	ldr	x0, [sp, #8]
	add	sp, sp, #16
	
	ldp	x1, x2, [sp], #16
	ldp	x3, x4, [sp], #16
	ldp	x5, x6, [sp], #16
	ldp	x7, x8, [sp], #16
	ldp	x9, x10, [sp], #16
	ldp	x11, x12, [sp], #16
	ldp	x13, x14, [sp], #16
	ldp	x15, x16, [sp], #16
	ldp	x17, x18, [sp], #16
	// x19-x28 Callee-saved
	ldp	x29, x30, [sp], #16
	
	eret

_halt:
	b	.


.weak synExceptionHandler
synExceptionHandler:
	b	.

.weak intExceptionHandler
intExceptionHandler:
	b	.

.weak errExceptionHandler
errExceptionHandler:
	b	.

.global sysCall
sysCall:
	svc	#0
	ret
