#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "SysHeap.h"


typedef union HeapBlock
{
	uint8_t header[SYSHEAP_ALIGN_SIZE];
	
	struct
	{
		bool assigned;
		
		size_t size;
		union HeapBlock* next;
	};
	
} HeapBlock;


static HeapBlock* sHeadBlock;
static HeapBlock* sFreeBlock;


int initSysHeap(void)
{
	extern uint8_t __heap_vmpos[], __heap_vmend[];
	
	// Align check
	if((uintptr_t)__heap_vmpos % SYSHEAP_ALIGN_SIZE)
		return -1;
	
	sHeadBlock = (HeapBlock*)__heap_vmpos;
	memset(sHeadBlock, 0, sizeof(HeapBlock));
	
	sHeadBlock->size = __heap_vmend - __heap_vmpos;
	sFreeBlock = sHeadBlock;
	
	return 0;
}

void* sysHeapAlloc(size_t size)
{
	HeapBlock* next;
	HeapBlock* free;
	
	// Block header
	size += sizeof(HeapBlock);
	
	// Alignment
	if(size % SYSHEAP_ALIGN_SIZE)
		size += SYSHEAP_ALIGN_SIZE - size % SYSHEAP_ALIGN_SIZE;
	
	// Search free
	free = sFreeBlock; while(!free->assigned)
	{
		// Merge fragments
		next = free->next; while(next != NULL)
		{
			if(!next->assigned)
			{
				free->next = next->next;
				free->size += next->size;
				
				next = free->next;
			}
			else
				break;
		}
		
		// Enough
		if(free->size > size)
		{
			sFreeBlock = (HeapBlock*)((uint8_t*)free + size);
			memset(sFreeBlock, 0, sizeof(HeapBlock));
			
			sFreeBlock->size = free->size - size;
			sFreeBlock->next = free->next;
			
			free->size = size;
			free->assigned = true;
			free->next = sFreeBlock;
			
			return (uint8_t*)free + sizeof(HeapBlock);
		}
		// Just fit
		else if(free->size == size)
		{
			if(free->next != NULL)
				sFreeBlock = free->next;
			else
				sFreeBlock = sHeadBlock;
			
			free->assigned = true;
			
			return (uint8_t*)free + sizeof(HeapBlock);
		}
		
		// Search next
		if(free->next != NULL)
			free = free->next;
		else
			free = sHeadBlock;
		
		// Not found
		if(free == sFreeBlock)
			break;
	}
	
	return NULL;
}

void sysHeapFree(void* ptr)
{
	HeapBlock* free;
	HeapBlock* next;
	
	free = (HeapBlock*)((uint8_t*)ptr - sizeof(HeapBlock));
	
	free->assigned = false;
	
	// Merge fragments
	next = free->next; while(next != NULL)
	{
		if(!next->assigned)
		{
			free->next = next->next;
			free->size += next->size;
			
			next = free->next;
		}
		else
			break;
	}
}
