#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "ttab.h"

#include "trap.h"

#include "BitCodec.h"


int init(void)
{
	unsigned i;
	
	uint64_t mair_el1;
	uintptr_t address;
	
	uint64_t ttbr0_el1;
	uint64_t tcr_el1;
	uint64_t sctlr_el1;
	
	uint64_t icc_sre_el1;
	uint64_t icc_bpr1_el1;
	uint64_t icc_pmr_el1;
	uint64_t icc_igrpen1_el1;
	uint64_t icc_ap1r0_el1;
	uint64_t icc_ctlr_el1;
	uint64_t icc_iar1_el1;
	uint64_t icc_eoir1_el1;
	
	//
	// Translation tables initialization
	//
	for(i = 0; i < sizeof(gTopLevelTransTables) / sizeof(TransTableEntry); i ++)
		gTopLevelTransTables[i].entry = 0;
	for(i = 0; i < sizeof(gMemoryTransEntries) / sizeof(TransTableEntry); i ++)
		gMemoryTransEntries[i].entry = 0;
	for(i = 0; i < sizeof(gPeriphTransEntries) / sizeof(TransTableEntry); i ++)
		gPeriphTransEntries[i].entry = 0;
	
	//
	// MAIR_EL1 Memory Attribute Indirection Register (EL1)
	//
	asm volatile("mrs	%0, MAIR_EL1\n": "=r"(mair_el1) : : );
	// AttrIndex 2: Normal memory
	// Outer write-back transient read-allocate write-noallocate
	// Inner write-through transient read-allocate write-noallocate
	BIT_SET_RANGE(mair_el1, 23, 16, 0b01100010);
	// AttrIndex 1: Normal memory
	// Outer non-cacheable
	// Inner non-cacheable
	BIT_SET_RANGE(mair_el1, 15, 8, 0b00100010);
	// AttrIndex 0: Device-nGnRnE
	BIT_SET_RANGE(mair_el1, 7, 0, 0b00000000);
	asm volatile("msr	MAIR_EL1, %0\n": : "r"(mair_el1) : );
	
	//
	// External DDR 0G - 1G
	//
	extern uint8_t __text_vmpos[], __text_vmend[];
	extern uint8_t __exec_vmpos[], __exec_vmend[];
	extern uint8_t __ttab_vmpos[], __ttab_vmend[];
	extern uint8_t __rodata_vmpos[], __rodata_vmend[];
	extern uint8_t __rwdata_vmpos[], __rwdata_vmend[];
	extern uint8_t __bss_vmpos[], __bss_vmend[];
	extern uint8_t __nocache_vmpos[], __nocache_vmend[];
	extern uint8_t __stack_vmpos[], __stack_vmend[];
	extern uint8_t __heap_vmpos[], __heap_vmend[];
	extern uint8_t __shared_vmpos[], __shared_vmend[];
	for(i = 0; i < sizeof(gMemoryTransEntries) / sizeof(TransTableEntry); i ++)
	{
		address = 0x200000 /* 2M */ * i;
		
		// [47:17] Output address
		gMemoryTransEntries[i].blockEntry.address = address >> 17;
		
		// Default attributes
		gMemoryTransEntries[i].blockEntry.attrIndex = 2;
		gMemoryTransEntries[i].blockEntry.AP = 0b11; // read-only
		gMemoryTransEntries[i].blockEntry.AF = 0b1;
		
		if((address >= (uintptr_t)__text_vmpos) && (address < (uintptr_t)__text_vmend))
			gMemoryTransEntries[i].blockEntry.descType = 0b01;
		else if((address >= (uintptr_t)__exec_vmpos) && (address < (uintptr_t)__exec_vmend))
		{
			gMemoryTransEntries[i].blockEntry.descType = 0b01;
			gMemoryTransEntries[i].blockEntry.AP = 0b01; // read/write
		}
		else if((address >= (uintptr_t)__ttab_vmpos) && (address < (uintptr_t)__ttab_vmend))
		{
			gMemoryTransEntries[i].blockEntry.descType = 0b01;
			gMemoryTransEntries[i].blockEntry.AP = 0b01; // read/write
			gMemoryTransEntries[i].blockEntry.PXN = 0b01;
			gMemoryTransEntries[i].blockEntry.UXN = 0b01;
		}
		else if((address >= (uintptr_t)__rodata_vmpos) && (address < (uintptr_t)__rodata_vmend))
		{
			gMemoryTransEntries[i].blockEntry.descType = 0b01;
			gMemoryTransEntries[i].blockEntry.PXN = 0b01;
			gMemoryTransEntries[i].blockEntry.UXN = 0b01;
		}
		else if((address >= (uintptr_t)__rwdata_vmpos) && (address < (uintptr_t)__rwdata_vmend))
		{
			gMemoryTransEntries[i].blockEntry.descType = 0b01;
			gMemoryTransEntries[i].blockEntry.AP = 0b01; // read/write
			gMemoryTransEntries[i].blockEntry.PXN = 0b01;
			gMemoryTransEntries[i].blockEntry.UXN = 0b01;
		}
		else if((address >= (uintptr_t)__bss_vmpos) && (address < (uintptr_t)__bss_vmend))
		{
			gMemoryTransEntries[i].blockEntry.descType = 0b01;
			gMemoryTransEntries[i].blockEntry.AP = 0b01; // read/write
			gMemoryTransEntries[i].blockEntry.PXN = 0b01;
			gMemoryTransEntries[i].blockEntry.UXN = 0b01;
		}
		else if((address >= (uintptr_t)__nocache_vmpos) && (address < (uintptr_t)__nocache_vmend))
		{
			gMemoryTransEntries[i].blockEntry.descType = 0b01;
			gMemoryTransEntries[i].blockEntry.attrIndex = 1; // non-cacheable
			gMemoryTransEntries[i].blockEntry.AP = 0b01; // read/write
			gMemoryTransEntries[i].blockEntry.PXN = 0b01;
			gMemoryTransEntries[i].blockEntry.UXN = 0b01;
		}
		else if((address >= (uintptr_t)__stack_vmpos) && (address < (uintptr_t)__stack_vmend))
		{
			gMemoryTransEntries[i].blockEntry.descType = 0b01;
			gMemoryTransEntries[i].blockEntry.AP = 0b01; // read/write
			gMemoryTransEntries[i].blockEntry.PXN = 0b01;
			gMemoryTransEntries[i].blockEntry.UXN = 0b01;
		}
		else if((address >= (uintptr_t)__heap_vmpos) && (address < (uintptr_t)__heap_vmend))
		{
			gMemoryTransEntries[i].blockEntry.descType = 0b01;
			gMemoryTransEntries[i].blockEntry.AP = 0b01; // read/write
			gMemoryTransEntries[i].blockEntry.PXN = 0b01;
			gMemoryTransEntries[i].blockEntry.UXN = 0b01;
		}
		else if((address >= (uintptr_t)__shared_vmpos) && (address < (uintptr_t)__shared_vmend))
		{
			gMemoryTransEntries[i].blockEntry.descType = 0b01;
			gMemoryTransEntries[i].blockEntry.AP = 0b01; // read/write
			gMemoryTransEntries[i].blockEntry.PXN = 0b01;
			gMemoryTransEntries[i].blockEntry.UXN = 0b01;
			gMemoryTransEntries[i].blockEntry.SH = 0b10; // outer shareable
		}
	}
	gTopLevelTransTables[0].tableEntry.descType = 0b11;
	gTopLevelTransTables[0].tableEntry.address = (uintptr_t)gMemoryTransEntries >> 12;
	
	//
	// Device BUS 3G - 4G
	//
	// 0xF0000000 start
	// 0x10000000 size
	for(i = 0xF0000000 % 0x40000000 / 0x200000; i < sizeof(gPeriphTransEntries) / sizeof(TransTableEntry); i ++)
	{
		gPeriphTransEntries[i].blockEntry.descType = 0b01;
		gPeriphTransEntries[i].blockEntry.address = (0xF0000000 / 0x40000000 * 0x40000000 + 0x200000 * i) >> 17;
		gPeriphTransEntries[i].blockEntry.AF = 0b1;
		gPeriphTransEntries[i].blockEntry.AP = 0b01; // Read/write
		gPeriphTransEntries[i].blockEntry.PXN = 0b1;
		gPeriphTransEntries[i].blockEntry.UXN = 0b1;
	}
	gTopLevelTransTables[3].tableEntry.descType = 0b11;
	gTopLevelTransTables[3].tableEntry.address = (uintptr_t)gPeriphTransEntries >> 12;
	gTopLevelTransTables[3].tableEntry.PXNTable = 0b1;
	gTopLevelTransTables[3].tableEntry.UXNTable = 0b1;
	
	//
	// TTBR0_EL1 Translation Table Base Register 0 (EL1)
	//
	asm volatile("mrs	%0, TTBR0_EL1\n": "=r"(ttbr0_el1) : : );
	// [63:48] ASID
	BIT_SET_RANGE(ttbr0_el1, 63, 48, 0);
	// [47:1] BADDR
	BIT_SET_RANGE(ttbr0_el1, 47, 1, (uintptr_t)gTopLevelTransTables >> 1);
	// [0] CnP
	BIT_SET(ttbr0_el1, 0, 0b0);
	asm volatile("msr	TTBR0_EL1, %0\n": : "r"(ttbr0_el1) : );
	
	//
	// TCR_EL1 Translation Control Register (EL1)
	//
	asm volatile("mrs	%0, TCR_EL1\n": "=r"(tcr_el1) : : );
	// [59] DS = 0
	BIT_SET(tcr_el1, 59, 0b0);
	// [58] TCMA1 [57] TCMA0
	BIT_SET_RANGE(tcr_el1, 58, 57, 0b00);
	// [56] E0PD1 [55] E0PD0
	BIT_SET_RANGE(tcr_el1, 56, 55, 0b00);
	// [54] NFD1 [53] NFD0
	BIT_SET_RANGE(tcr_el1, 54, 53, 0b00);
	// [52] TBID1 [51] TBID0
	BIT_SET_RANGE(tcr_el1, 52, 51, 0b00);
	// [50:47] HWU162-HWU159
	BIT_SET_RANGE(tcr_el1, 50, 47, 0b0000);
	// [46:43] HWU062-HWU059
	BIT_SET_RANGE(tcr_el1, 46, 43, 0b0000);
	// [41] HPD0 = 0
	BIT_SET(tcr_el1, 41, 0b0);
	// [40] HD = 0
	BIT_SET(tcr_el1, 40, 0b0);
	// [39] HA = 0
	BIT_SET(tcr_el1, 39, 0b0);
	// [37] TBI0 = 0
	BIT_SET(tcr_el1, 37, 0b0);
	// [36] AS = 1
	BIT_SET(tcr_el1, 36, 0b1);
	// [34:32] IPS = 0b010
	BIT_SET_RANGE(tcr_el1, 34, 32, 0b010);
	// [23] EPD1 = 1
	BIT_SET(tcr_el1, 23, 0b1);
	// [22] A1 = 0
	BIT_SET(tcr_el1, 22, 0b0);
	// [15:14] TG0 = 4KB = 0b00
	BIT_SET_RANGE(tcr_el1, 15, 14, 0b00);
	// [13:12] SH0 = Non-shareable = 0b00
	BIT_SET_RANGE(tcr_el1, 13, 12, 0b00);
	// [11:10] ORGN0 = Normal outer write-back read-allocate no write-allocate = 0b11
	BIT_SET_RANGE(tcr_el1, 11, 10, 0b11);
	// [9:8] IRGN0 = Normal inner write-through read-allocate = 0b10
	BIT_SET_RANGE(tcr_el1, 9, 8, 0b10);
	// [7] EPD0 = 0
	BIT_SET(tcr_el1, 7, 0b0);
	// [5:0] T0SZ = 64bit - 32bit address = 32
	BIT_SET_RANGE(tcr_el1, 5, 0, 32);
	asm volatile("msr	TCR_EL1, %0\n": : "r"(tcr_el1) : );
	
	//
	// SCTLR_EL1 System Control Register (EL1)
	//
	asm volatile("mrs	%0, SCTLR_EL1\n": "=r"(sctlr_el1) : : );
	// I [12] Stage 1 instruction access cacheablility control, for accessed at EL0 and EL1
	BIT_SET(sctlr_el1, 12, 0b1);
	// C [2] Stage 1 cacheablility control, for data accesses
	BIT_SET(sctlr_el1, 2, 0b1);
	// M [0] MMU enable for EL1&0 stage 1 address translation
	BIT_SET(sctlr_el1, 0, 0b1);
	asm volatile("msr	SCTLR_EL1, %0\n": : "r"(sctlr_el1) : );
	asm volatile("isb	SY\n");
	
	//
	// Interrupt initialization
	//
	for(i = 0; i < TRAP_MAX_INTID_COUNT; i ++)
		gInterruptInfo[i].handler = NULL;
	
	gSysCallHandler = NULL;
	
	//
	// GIC-600
	//
	// ICC_SRE_EL1 Interrupt Controller System Register Enable Register
	// DIB [2] Disable IRQ bypass
	// 0b1 IRQ bypass disabled.
	// DFB [1] Disable FIQ bypass
	// 0b1 FIQ bypass disabled.
	// SRE [0] System Register Enable
	// 0b1 The System register interface for the current Security state is enabled.
	icc_sre_el1 = 0b111;
	asm volatile("msr	S3_0_C12_C12_5, %0\n": : "r"(icc_sre_el1) : );
	
	// ICC_IGRPEN1_EL1 Interrupt Controller Interrupt Group 1 Enable Register
	// Enable [0] 0b0 Disable
	icc_igrpen1_el1 = 0b0;
	asm volatile("msr	S3_0_C12_C12_7, %0\n": : "r"(icc_igrpen1_el1) : );
	
	// ICC_AP1R<n>_EL1 Interrupt Controller Active Priorities Group 1 Registers, n = 0 - 3
	// Write 0 for dropping all priorities
	icc_ap1r0_el1 = 0;
	asm volatile("msr	S3_0_C12_C9_0, %0\n": : "r"(icc_ap1r0_el1) : );
	
	// ICC_BPR1_EL1 Interrupt Controller Binary Point Register 1
	// BinaryPoint [2:0]
	// 0b001 Field with binary point ggggggg.s
	icc_bpr1_el1 = 0b001;
	asm volatile("msr	S3_0_C12_C12_3, %0\n": : "r"(icc_bpr1_el1) : );
	
	// ICC_PMR_EL1 Interrupt Controller Interrupt Priority Mask Register
	// Priority [7:0] The priority mask level for the CPU interface. If the priority of an interrupt is higher than the value indicated by this field, the interface signals the interrupt to the PE.
	icc_pmr_el1 = 0b11111111;
	asm volatile("msr	S3_0_C4_C6_0, %0\n": : "r"(icc_pmr_el1) : );
	
	// ICC_CTLR_EL1 Interrupt Controller Control Register (EL1)
	asm volatile("mrs	%0, S3_0_C12_C12_4\n": "=r"(icc_ctlr_el1) : : );
	// PMHE [6] Priority Mask Hint Enable. Controls whether the priority mask register is used as a hint for interrupt distribution:
	// 0b1 Enables use of ICC_PMR_EL1 as a hint for interrupt distribution.
	BIT_SET(icc_ctlr_el1, 6, 0b1);
	// EOImode [1] EOI mode for the current Security state. Controls whether a write to an End of Interrupt register also deactivates the interrupt:
	// 0b0 ICC_EOIR0_EL1 and ICC_EOIR1_EL1 provide both priority drop and interrupt deactivation functionality. Accesses to ICC_DIR_EL1 are UNPREDICTABLE.
	BIT_SET(icc_ctlr_el1, 1, 0b0);
	// CBPR [0] Common Binary Point Register. Controls whether the same register is used for interrupt preemption of both Group 0 and Group 1 interrupts:
	// 0b0 ICC_BPR0_EL1 determines the preemption group for Group 0 interrupts only. ICC_BPR1_EL1 determines the preemption group for Group 1 interrupts.
	BIT_SET(icc_ctlr_el1, 0, 0b0);
	asm volatile("msr	S3_0_C12_C12_4, %0\n": : "r"(icc_ctlr_el1) : );
	
	// Disable all INTIDs
	for(i = 0; i < TRAP_MAX_INTID_COUNT; i ++)
		disableInterrupt(i);
	
	// Clean pending
	while(true)
	{
		// ICC_IAR1_EL1 Interrupt Controller Interrupt Acknowledge Register 1
		asm volatile("mrs	%0, S3_0_C12_C12_0\n": "=r"(icc_iar1_el1) : : );
		
		// Not special INTIDs
		if((icc_iar1_el1 < 1020) || (icc_iar1_el1 > 1023))
		{
			// ICC_EOIR1_EL1 Interrupt Controller End Of Interrupt Register 1
			icc_eoir1_el1 = icc_iar1_el1;
			asm volatile("msr	S3_0_C12_C12_1, %0\n": : "r"(icc_eoir1_el1) : );
		}
		else
			break;
	}
	
	// ICC_IGRPEN1_EL1 Interrupt Controller Interrupt Group 1 Enable Register
	// Enable [0] 0b1 Enable
	icc_igrpen1_el1 = 0b1;
	asm volatile("msr	S3_0_C12_C12_7, %0\n": : "r"(icc_igrpen1_el1) : );
	
	//
	// bss zero initialization
	//
	memset(__bss_vmpos, 0, __bss_vmend - __bss_vmpos);
	
	return 0;
}
