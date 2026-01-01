#ifndef SYSSHELL_H
#define SYSSHELL_H


#include "uart.h"


#define SYSSHELL_UART Uart0
#define SYSSHELL_UART_INTERRUPT_PRIORITY 0xD0

#define SYSSHELL_LINE_MAX_SIZE 256
#define SYSSHELL_INPUT_BUFF_SIZE 16
#define SYSSHELL_PRINT_BUFF_SIZE (1024 * 2)

#define SYSSHELL_COMMAND_MAX_COUNT 32
#define SYSSHELL_CMDARGS_MAX_COUNT 8

#define SYSSHELL_ACTIVE_TIME_MS (1000 * 300)


#ifdef __cplusplus
extern "C"
{
#endif


typedef int (*SysCmdRoutine)(int argc, const char* argv[]);
typedef int (*SysLoginChecker)(const char* passwd);

typedef struct
{
	const char* name;
	const char* info;
	const char* help;
	
	SysCmdRoutine routine;
	
} SysCommand;


int initSysShell(void);

int sysShellRoutine(void);

int sysShellLogin(void);
int sysPushStdPrint(const char* string);

int registerSysCommand(const SysCommand* command);
int registerLoginChecker(SysLoginChecker checker);

int printk(const char* format, ...);


#ifdef __cplusplus
}
#endif


#endif
