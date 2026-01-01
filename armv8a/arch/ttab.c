#include "ttab.h"


TransTableEntry __attribute__((section(".ttab"), aligned(4096))) gTopLevelTransTables[4];
TransTableEntry __attribute__((section(".ttab"), aligned(4096))) gMemoryTransEntries[512];
TransTableEntry __attribute__((section(".ttab"), aligned(4096))) gPeriphTransEntries[512];
