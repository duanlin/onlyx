#ifndef TTAB_H
#define TTAB_H


#include <stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif


typedef union
{
	uint64_t entry;
	
	// 4KB, 16KB, and 64KB granules, 48-bit OA
	struct {
		uint64_t descType: 2; // [1:0]
		uint64_t: 10; // IGNORED [11:2]
		
		// Next-level table address
		uint64_t address: 36; // [47:12]
		uint64_t: 3; // RES0 [50:48]
		
		// Attributes
		uint64_t: 8; // IGNORED [58:51]
		uint64_t PXNTable: 1; // [59]
		uint64_t UXNTable: 1; // [60]
		uint64_t APTable: 2; // [62:61]
		uint64_t NSTable: 1; // [63]
		
	} tableEntry;
	
	// 4KB, 16KB, and 64KB granules, 48-bit OA
	struct {
		uint64_t descType: 2; // [1:0]
		
		// Lower attributes
		uint64_t attrIndex: 3; // [4:2]
		uint64_t NS: 1; // [5]
		uint64_t AP: 2; // [7:6]
		uint64_t SH: 2; // [9:8]
		uint64_t AF: 1; // [10]
		uint64_t nG: 1; // [11]
		uint64_t: 4; // RES0 [15:12]
		uint64_t nT: 1; // [16]
		
		// Output address
		uint64_t address: 31; // [47:17]
		uint64_t: 2; // RES0 [49:48]
		
		// Upper attributes
		uint64_t GP: 1; // [50]
		uint64_t DBM: 1; // [51]
		uint64_t Contiguous: 1; // [52]
		uint64_t PXN: 1; // [53]
		uint64_t UXN: 1; // [54]
		uint64_t: 4; // IGNORED [58:55]
		uint64_t PBHA: 4; // [62:59]
		uint64_t: 1; // IGNORED [63]
		
	} blockEntry;
	
} TransTableEntry;


extern TransTableEntry gTopLevelTransTables[4];
extern TransTableEntry gMemoryTransEntries[512];
extern TransTableEntry gPeriphTransEntries[512];


#ifdef __cplusplus
}
#endif


#endif
