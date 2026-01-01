#ifndef SYSCALL_H
#define SYSCALL_H


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	SysCallNone = 0,
	
	SysCallHeapAlloc,
	SysCallHeapFree,
	
	SysCallSendMail,
	
	SysCallStdPrint
	
} SysCall;


extern void* sysCall(SysCall what, void* arg0, void* arg1, void* arg2);


int initSysCall(void);


#ifdef __cplusplus
}
#endif


#endif
