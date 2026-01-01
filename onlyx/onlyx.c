#include <stdint.h>

#include "onlyx.h"

#include "core.h"
#include "wdog.h"
#include "uart.h"
#include "timer.h"

#include "SynBus.h"
#include "UtcBus.h"


int onlyx(void)
{
	feedHardDog();
	
	if(initSysHeap())
		return -1;
	if(initSysCall())
		return -2;
	
	if(initUart())
		return -3;
	if(initSysShell())
		return -4;
	
	if(initTimer())
		return -5;
	
	if(initSynClock())
		return -5;
	if(initSynBus())
		return -6;
	
	if(initUtcClock())
		return -7;
	if(initUtcBus())
		return -8;
	
	if(initIdleTask())
		return -9;
	if(initUserTask())
		return -10;
	if(initMainTask())
		return -11;
	
	// ctors
	uintptr_t* ctor;
	extern uintptr_t __ctors_vmpos[], __ctors_vmend[];
	for(ctor = __ctors_vmpos; ctor < __ctors_vmend; ctor ++)
		((void (*)(void))(*ctor))();
	
	printk("\n");
	printk("OnlyX Operating System %s V%u.%02u\n", CORE_ARCH, ONLYX_MAJOR_VERSION, ONLYX_MINOR_VERSION);
	
	// Application
	extern int main();
	return main();
}

void exit(int status)
{
	unsigned i;
	
	// Unregister user tasks
	for(i = 0; i < UserTaskCount; i ++)
		registerUserTask(i, NULL, NULL, NULL);
	
	// Unregister user idle
	registerUserIdle(NULL, NULL);
	
	// dtors
	uintptr_t* dtor;
	extern uintptr_t __dtors_vmpos[], __dtors_vmend[];
	for(dtor = __dtors_vmpos; dtor < __dtors_vmend; dtor ++)
		((void (*)(void))(*dtor))();
	
	extern void _exit(int status);
	_exit(status);
	// No return
}
