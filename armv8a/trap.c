#include <stdbool.h>
#include <string.h>

#include "trap.h"

#include "RK3568.h"
#include "BitCodec.h"


InterruptInfo gInterruptInfo[TRAP_MAX_INTID_COUNT];

SysCallHandler gSysCallHandler;


int registerInterrupt(unsigned INTID, InterruptHandler handler, void* arg, uint8_t priority, InterruptTrig trig)
{
	//
	// Register interrupt for PE2
	//
	
	uint64_t gicd_irouter;
	
	if(INTID >= TRAP_MAX_INTID_COUNT)
		return -1;
	
	// SGI
	if(INTID < 15)
	{
		// GICR_ICENABLER0 Interrupt Clear-Enable Register 0
		REG32_SET(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_ICENABLER0), (uint32_t)0b1 << INTID);
		// GICR_ICPENDR0 Interrupt Clear-Pending Register 0
		REG32_SET(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_ICPENDR0), (uint32_t)0b1 << INTID);
		// GICR_ICACTIVER0 Interrupt Clear-Active Register 0
		REG32_SET(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_ICACTIVER0), (uint32_t)0b1 << INTID);
		
		// GICR_ICFGR0 Interrupt Configuration Register 0
		if(trig != InterruptTrigEdge)
			return -1;
		REG32_SET_RANGE(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_ICFGR0), 1 + INTID * 2, INTID * 2, 0b10);
		
		// GICR_IPRIORITYR<n> Interrupt Priority Registers, n = 0 - 7
		REG32_SET_RANGE(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_IPRIORITYR0 + INTID / 4 * 4), 7 + INTID % 4 * 8, INTID % 4 * 8, priority);
		
		gInterruptInfo[INTID].handler = handler;
		gInterruptInfo[INTID].arg = arg;
		
		// GICR_ISENABLER0 Interrupt Set-Enable Register 0
		REG32_SET(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_ISENABLER0), (uint32_t)0b1 << INTID);
	}
	// PPI
	else if(INTID < 32)
	{
		// GICR_ICENABLER0 Interrupt Clear-Enable Register 0
		REG32_SET(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_ICENABLER0), (uint32_t)0b1 << INTID);
		// GICR_ICPENDR0 Interrupt Clear-Pending Register 0
		REG32_SET(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_ICPENDR0), (uint32_t)0b1 << INTID);
		// GICR_ICACTIVER0 Interrupt Clear-Active Register 0
		REG32_SET(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_ICACTIVER0), (uint32_t)0b1 << INTID);
		
		// GICR_ICFGR1 Interrupt Configuration Register 1
		if(trig == InterruptTrigEdge)
			REG32_SET_RANGE(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_ICFGR1), 1 + INTID % 16 * 2, INTID % 16 * 2, 0b10);
		else if(trig == InterruptTrigLevel)
			REG32_SET_RANGE(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_ICFGR1), 1 + INTID % 16 * 2, INTID % 16 * 2, 0b00);
		else
			return -2;
		
		// GICR_IPRIORITYR<n> Interrupt Priority Registers, n = 0 - 7
		REG32_SET_RANGE(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_IPRIORITYR0 + INTID / 4 * 4), 7 + INTID % 4 * 8, INTID % 4 * 8, priority);
		
		gInterruptInfo[INTID].handler = handler;
		gInterruptInfo[INTID].arg = arg;
		
		// GICR_ISENABLER0 Interrupt Set-Enable Register 0
		REG32_SET(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_ISENABLER0), (uint32_t)0b1 << INTID);
	}
	// SPI
	else
	{
		// GICD_ICENABLER<n> Interrupt Clear-Enable Registers, n = 0 - 31
		REG32_SET(RK3568_GIC600_GICD(GIC600_GICD_ICENABLER0 + INTID / 32 * 4), (uint32_t)0b1 << (INTID % 32));
		// GICD_ICPENDR<n> Interrupt Clear-Pending Registers, n = 0 - 31
		REG32_SET(RK3568_GIC600_GICD(GIC600_GICD_ICPENDR0 + INTID / 32 * 4), (uint32_t)0b1 << (INTID % 32));
		// GICD_ICACTIVER<n> Interrupt Clear-Active Registers, n = 0 - 31
		REG32_SET(RK3568_GIC600_GICD(GIC600_GICD_ICACTIVER0 + INTID / 32 * 4), (uint32_t)0b1 << (INTID % 32));
		
		//
		// GICD_ICFGR<n> Interrupt Configuration Registers, n = 0 - 63
		//
		if(trig == InterruptTrigEdge)
			REG32_SET_RANGE(RK3568_GIC600_GICD(GIC600_GICD_ICFGR0 + INTID / 16 * 4), 1 + INTID % 16 * 2, INTID % 16 * 2, 0b10);
		else if(trig == InterruptTrigLevel)
			REG32_SET_RANGE(RK3568_GIC600_GICD(GIC600_GICD_ICFGR0 + INTID / 16 * 4), 1 + INTID % 16 * 2, INTID % 16 * 2, 0b00);
		else
			return -3;
		
		// GICD_IPRIORITYR<n> Interrupt Priority Registers, n = 0 - 254
		REG32_SET_RANGE(RK3568_GIC600_GICD(GIC600_GICD_IPRIORITYR0 + INTID / 4 * 4), 7 + INTID % 4 * 8, INTID % 4 * 8, priority);
		
		//
		// GICD_IROUTER<n> Interrupt Routing Registers, n = 32 - 1019
		//
		gicd_irouter = 0;
		// Aff3 [39:32] Affinity level 3
		BIT_SET_RANGE(gicd_irouter, 39, 32, 0);
		// Interrupt Routing Mode
		// 0b0 Interrupts routed to the PE specified by a.b.c.d. In this routing, a, b, c, and d are the values of fields Aff3, Aff2, Aff1, and Aff0 respectively.
		BIT_SET(gicd_irouter, 31, 0b0);
		// Aff2 [23:16] Affinity level 2
		BIT_SET_RANGE(gicd_irouter, 23, 16, 0);
		// Aff1 [15:8]
		BIT_SET_RANGE(gicd_irouter, 15, 8, 2);
		// Aff0 [7:0]
		BIT_SET_RANGE(gicd_irouter, 7, 0, 0);
		REG64_SET(RK3568_GIC600_GICD(GIC600_GICD_IROUTER32 + (INTID - 32) * 8), gicd_irouter);
		
		gInterruptInfo[INTID].handler = handler;
		gInterruptInfo[INTID].arg = arg;
		
		// GICD_ISENABLER<n> Interrupt Set-Enable Registers, n = 0 - 31
		REG32_SET(RK3568_GIC600_GICD(GIC600_GICD_ISENABLER0 + INTID / 32 * 4), (uint32_t)0b1 << (INTID % 32));
	}
	
	return 0;
}

int disableInterrupt(unsigned INTID)
{
	//
	// Disable interrupt for PE2
	//
	uint64_t mpidr_el1;
	uint64_t gicd_irouter;
	
	if(INTID >= TRAP_MAX_INTID_COUNT)
			return -1;
	
	// SGI PPI
	if(INTID < 32)
	{
		gInterruptInfo[INTID].handler = NULL;
		gInterruptInfo[INTID].arg = NULL;
		
		// GICR_ICENABLER0 Interrupt Clear-Enable Register 0
		REG32_SET(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_ICENABLER0), (uint32_t)0b1 << INTID);
	}
	// SPI
	else
	{
		gInterruptInfo[INTID].handler = NULL;
		gInterruptInfo[INTID].arg = NULL;
		
		// MPIDR_EL1 Multiprocessor Affinity Register
		asm volatile("mrs	%0, MPIDR_EL1\n": "=r"(mpidr_el1) : : );
		// GICD_IROUTER<n> Interrupt Routing Registers, n = 32 - 1019
		gicd_irouter = REG64_GET(RK3568_GIC600_GICD(GIC600_GICD_IROUTER32 + (INTID - 32) * 8));
		
		// GICD_IROUTER<n>
		// Interrupt_Routing_Mode [31]
		// 0b0 Interrupts routed to the PE specified by a.b.c.d. In this routing, a, b, c, and d are the values of fields Aff3, Aff2, Aff1, and Aff0 respectively.
		// Aff3.Aff2.Aff1.Aff0 == Aff3.Aff2.Aff1.Aff0
		if((BIT_GET(gicd_irouter, 31) == 0b0)
		&& (BIT_GET_RANGE(mpidr_el1, 39, 32) == BIT_GET_RANGE(gicd_irouter, 39, 32))
		&& (BIT_GET_RANGE(mpidr_el1, 23, 16) == BIT_GET_RANGE(gicd_irouter, 23, 16))
		&& (BIT_GET_RANGE(mpidr_el1, 15, 8) == BIT_GET_RANGE(gicd_irouter, 15, 8))
		&& (BIT_GET_RANGE(mpidr_el1, 7, 0) == BIT_GET_RANGE(gicd_irouter, 7, 0)))
			// GICD_ICENABLER<n> Interrupt Clear-Enable Registers, n = 0 - 31
			REG32_SET(RK3568_GIC600_GICD(GIC600_GICD_ICENABLER0 + INTID / 32 * 4), (uint32_t)0b1 << (INTID % 32));
	}
	
	return 0;
}

int setInterruptPending(unsigned INTID)
{
	//
	// Trig interrupt for PE2
	//
	
	if(INTID >= TRAP_MAX_INTID_COUNT)
		return -1;
	
	// SGI PPI
	if(INTID < 32)
		REG32_SET(RK3568_GIC600_GICR_SGIPPI2(GIC600_GICR_ISPENDR0), (uint32_t)0b1 << INTID);
	// SPI
	else
		REG32_SET(RK3568_GIC600_GICD(GIC600_GICD_ISPENDR0 + INTID / 32 * 4), (uint32_t)0b1 << (INTID % 32));
	
	return 0;
}

int registerSysCallHandler(SysCallHandler handler)
{
	gSysCallHandler = handler;
	
	return 0;
}

int synExceptionHandler(TrapContext* context, uint64_t ESR, uintptr_t FAR)
{
	unsigned ISS;
	uint8_t IL;
	uint8_t EC;
	
	unsigned ISS2;
	uint8_t Xs;
	
	ISS = BIT_GET_RANGE(ESR, 24, 0);
	IL = BIT_GET(ESR, 25);
	EC = BIT_GET_RANGE(ESR, 31, 26);
	ISS2 = BIT_GET_RANGE(ESR, 55, 32);
	
	switch(EC)
	{
	// Unknown reason
	case 0b000000:
		(void)ISS;
		(void)IL;
		
		while(true);
		
		break;
		
	// Trapped WF* instruction execution.
	// Conditional WF* instructions that fail their condition code check do not cause an exception.
	case 0b000001:
		while(true);
		
		break;
		
	// When AArch32 is supported:
	// Trapped MCR or MRC access with (coproc==0b1111) that is not reported using EC 0b000000.
	case 0b000011:
		while(true);
		
		break;
		
	// When AArch32 is supported:
	// Trapped MCRR or MRRC access with (coproc==0b1111) that is not reported using EC 0b000000.
	case 0b000100:
		while(true);
		
		break;
		
	// When AArch32 is supported:
	// Trapped MCR or MRC access with (coproc==0b1110).
	case 0b000101:
		while(true);
		
		break;
		
	// When AArch32 is supported:
	// Trapped LDC or STC access.
	// The only architected uses of these instruction are:
	// - An STC to write data to memory from DBGDTRRXint.
	// - An LDC to read data from memory to DBGDTRTXint.
	case 0b000110:
		while(true);
		
		break;
		
	// Access to SME, SVE, Advanced SIMD or floating-point functionality trapped by CPACR_EL1.FPEN, CPTR_EL2.FPEN, CPTR_EL2.TFP, or CPTR_EL3.TFP control.
	// Excludes exceptions resulting from CPACR_EL1 when the value of HCR_EL2.TGE is 1, or because SVE or Advanced SIMD and floating-point are not implemented.
	// These are reported with EC value 0b000000.
	case 0b000111:
		while(true);
		
		break;
		
	// When FEAT_LS64 is implemented:
	// Trapped execution of an LD64B or ST64B* instruction.
	case 0b001010:
		while(true);
		
		break;
		
	// When AArch32 is supported:
	// Trapped MRRC access with (coproc==0b1110).
	case 0b001100:
		while(true);
		
		break;
		
	// When FEAT_BTI is implemented:
	// Branch Target Exception.
	case 0b001101:
		while(true);
		
		break;
		
	// Illegal Execution state.
	case 0b001110:
		while(true);
		
		break;
		
	// When AArch32 is supported:
	// SVC instruction execution in AArch32 state.
	case 0b010001:
		while(true);
		
		break;
		
	// When AArch64 is supported:
	// SVC instruction execution in AArch64 state.
	case 0b010101:
		if(gSysCallHandler)
			gSysCallHandler(context);
		
		break;
		
	// When AArch64 is supported:
	// Trapped MSR, MRS or System instruction execution in AArch64 state, that is not reported using EC 0b000000, 0b000001, or 0b000111.
	// This includes all instructions that cause exceptions that are part of the encoding space defined in System instruction class encoding overview on page C5-731, except for those exceptions reported using EC values 0b000000, 0b000001, or 0b000111.
	case 0b011000:
		while(true);
		
		break;
		
	// When FEAT_SVE is implemented:
	// Access to SVE functionality trapped as a result of CPACR_EL1.ZEN, CPTR_EL2.ZEN, CPTR_EL2.TZ, or CPTR_EL3.EZ, that is not reported using EC 0b000000.
	case 0b011001:
		while(true);
		
		break;
		
	// When FEAT_TME is implemented:
	// Exception from an access to a TSTART instruction at EL0 when SCTLR_EL1.TME0 == 0, EL0 when SCTLR_EL2.TME0 == 0, at EL1 when SCTLR_EL1.TME == 0, at EL2 when SCTLR_EL2.TME == 0 or at EL3 when SCTLR_EL3.TME == 0.
	case 0b011011:
		while(true);
		
		break;
		
	// When FEAT_FPAC is implemented:
	// Exception from a Pointer Authentication instruction authentication failure
	case 0b011100:
		while(true);
		
		break;
		
	// When FEAT_SME is implemented:
	// Access to SME functionality trapped as a result of CPACR_EL1.SMEN, CPTR_EL2.SMEN, CPTR_EL2.TSM, CPTR_EL3.ESM, or an attempted execution of an instruction that is illegal because of the value of PSTATE.SM or PSTATE.ZA, that is not reported using EC 0b000000.
	case 0b011101:
		while(true);
		
		break;
		
	// Instruction Abort from a lower Exception level.
	// Used for MMU faults generated by instruction accesses and synchronous External aborts, including synchronous parity or ECC errors.
	// Not used for debug-related exceptions.
	case 0b100000:
		while(true);
		
		break;
		
	// Instruction Abort taken without a change in Exception level.
	// Used for MMU faults generated by instruction accesses and synchronous External aborts, including synchronous parity or ECC errors.
	// Not used for debug-related exceptions.
	case 0b100001:
		while(true);
		
		break;
		
	// PC alignment fault exception.
	case 0b100010:
		while(true);
		
		break;
		
	// Data Abort exception from a lower Exception level.
	// Used for MMU faults generated by data accesses, alignment faults other than those caused by Stack Pointer misalignment, and synchronous External aborts, including synchronous parity or ECC errors.
	// Not used for debug-related exceptions.
	case 0b100100:
		Xs = BIT_GET_RANGE(ISS2, 4, 0);
		(void)Xs;
		
		while(true);
		
		break;
		
	// Data Abort exception taken without a change in Exception level.
	// Used for MMU faults generated by data accesses, alignment faults other than those caused by Stack Pointer misalignment, and synchronous External aborts, including synchronous parity or ECC errors.
	// Not used for debug-related exceptions.
	case 0b100101:
		Xs = BIT_GET_RANGE(ISS2, 4, 0);
		(void)Xs;
		
		while(true);
		
		break;
		
	// SP alignment fault exception.
	case 0b100110:
		while(true);
		
		break;
		
	// When FEAT_MOPS is implemented:
	// Memory Operation Exception.
	case 0b100111:
		while(true);
		
		break;
		
	// When AArch32 is supported:
	// Trapped floating-point exception taken from AArch32 state.
	// This EC value is valid if the implementation supports trapping of floating-point exceptions, otherwise it is reserved.
	// Whether a floating-point implementation supports trapping of floating-point exceptions is IMPLEMENTATION DEFINED.
	case 0b101000:
		while(true);
		
		break;
		
	// When AArch64 is supported:
	// Trapped floating-point exception taken from AArch64 state.
	// This EC value is valid if the implementation supports trapping of floating-point exceptions, otherwise it is reserved.
	// Whether a floating-point implementation supports trapping of floating-point exceptions is IMPLEMENTATION DEFINED.
	case 0b101100:
		while(true);
		
		break;
		
	// Breakpoint exception from a lower Exception level.
	case 0b110000:
		while(true);
		break;
		
	// Breakpoint exception taken without a change in Exception level.
	case 0b110001:
		while(true);
		
		break;
		
	// Software Step exception from a lower Exception level.
	case 0b110010:
		while(true);
		
		break;
		
	// Software Step exception taken without a change in Exception level.
	case 0b110011:
		while(true);
		
		break;
		
	// Watchpoint exception from a lower Exception level.
	case 0b110100:
		while(true);
		
		break;
		
	// Watchpoint exception taken without a change in Exception level.
	case 0b110101:
		while(true);
		
		break;
		
	// When AArch32 is supported:
	// BKPT instruction execution in AArch32 state.
	case 0b111000:
		while(true);
		
		break;
		
	// When AArch64 is supported:
	// BRK instruction execution in AArch64 state.
	case 0b111100:
		while(true);
		
		break;
		
	// Unexpected EC
	default:
		while(true);
	}
	
	return 0;
}

int intExceptionHandler(const TrapContext* context, unsigned INTID)
{
	// Special INTIDs
	switch(INTID)
	{
	// The GIC returns this value in response to a read of ICC_IAR0_EL1 or ICC_HPPIR0_EL1 at EL3, to indicate that the interrupt being acknowledged is one which is expected to be handled at Secure EL1 or Secure EL2.
	// This INTID is only returned when the PE is executing at EL3 using AArch64 state, or when the PE is executing in AArch32 state in Monitor mode.
	// This value can also be returned by reads of ICC_IAR1_EL1 or ICC_HPPIR1_EL1 at EL3 when ICC_CTLR_EL3.RM == 1
	case 1020:
		return 0;
		
	// The GIC returns this value in response to a read of ICC_IAR0_EL1 or ICC_HPPIR0_EL1 at EL3, to indicate that the interrupt being acknowledged is one which is expected to be handled at Non-secure EL1 or EL2.
	// This INTID is only returned when the PE is executing at EL3 using AArch64 state, or when the PE is executing in AArch32 state in Monitor mode.
	// This value can also be returned by reads of ICC_IAR1_EL1or ICC_HPPIR1_EL1 at EL3 when ICC_CTLR_EL3.RM == 1
	case 1021:
		return 0;
		
	// In GICv3.3, when SCLTR_ELx.NMI is set to 1, the GIC returns this value in response to a read of ICC_IAR1_EL1 to indicate that the interrupt being acknowledged is an NMI.
	// This value also applies in legacy operation.
	case 1022:
		return 0;
		
	// This value is returned in response to an interrupt acknowledge, if there is no pending interrupt with sufficient priority for it to be signaled to the PE, or if the highest priority pending interrupt is not appropriate for the:
	// - Current Security state.
	// - Interrupt group that is associated with the System register.
	case 1023:
		return 0;
		
	default:
		break;
	}
	
	// Normal INTIDs
	if(INTID >= TRAP_MAX_INTID_COUNT)
		return -1;
	
	if(gInterruptInfo[INTID].handler)
	{
		// Interrupt handle
		if(gInterruptInfo[INTID].handler(gInterruptInfo[INTID].arg))
			return -2;
	}
	
	return 0;
}

int errExceptionHandler(const TrapContext* context, uint64_t ESR, uintptr_t FAR)
{
	unsigned ISS;
	uint8_t EC;
	
	ISS = BIT_GET_RANGE(ESR, 24, 0);
	EC = BIT_GET_RANGE(ESR, 31, 26);
	
	switch(EC)
	{
	// SError exception.
	case 0b101111:
		(void)ISS;
		
		while(true);
		
		break;
		
	// Unexpected EC
	default:
		return -1;
	}
	
	return 0;
}
