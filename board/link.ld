FLASH_SIZE = 512K;
SRAM_SIZE = 128K;

STACK_SIZE = 16K;


MEMORY
{
	FLASH (rx) : ORIGIN = 0x00000000, LENGTH = FLASH_SIZE
	SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = SRAM_SIZE
}


ENTRY(_start)


SECTIONS
{
	.text ORIGIN(SRAM):
	{
		PROVIDE(__text_vmpos = .);
		
		KEEP(* (.start))
		
		. = ALIGN(1K);
		KEEP(* (.vector))
		
		* (.text .text.*)
		
		PROVIDE(__text_vmend = .);
		
	} > SRAM
	
	.rodata : ALIGN(1K)
	{
		PROVIDE(__rodata_vmpos = .);
		
		* (.rodata .rodata.*)
		
		. = ALIGN(16);
		PROVIDE(__ctors_vmpos = .);
		KEEP(* (SORT_BY_INIT_PRIORITY(.init_array.*)))
		KEEP(* (.init_array))
		PROVIDE(__ctors_vmend = .);
		
		. = ALIGN(16);
		PROVIDE(__dtors_vmpos = .);
		KEEP(* (SORT_BY_INIT_PRIORITY(.finit_array.*)))
		KEEP(* (.finit_array))
		PROVIDE(__dtors_vmend = .);
		
		PROVIDE(__rodata_vmend = .);
		
	} > SRAM
	
	.rwdata : ALIGN(1K)
	{
		PROVIDE(__rwdata_vmpos = .);
		
		* (.data .data.*)
		
		PROVIDE(__rwdata_vmend = .);
		
	} > SRAM
	
	.bss (NOLOAD): ALIGN(1K)
	{
		PROVIDE(__bss_vmpos = .);
		
		* (.bss .bss.*)
		
		PROVIDE(__bss_vmend = .);
		
	} > SRAM
	
	.stack (NOLOAD): ALIGN(1K)
	{
		PROVIDE(__stack_vmpos = .);
		
		. += STACK_SIZE;
		
		PROVIDE(_stack = .);
		
		PROVIDE(__stack_vmend = .);
		
	} > SRAM
	
	.heap (NOLOAD): ALIGN(1K)
	{
		PROVIDE(__heap_vmpos = .);
		
		PROVIDE(_heap = .);
		
		. = ORIGIN(SRAM) + SRAM_SIZE;
		PROVIDE(__heap_vmend = .);
		
	} > SRAM
}
