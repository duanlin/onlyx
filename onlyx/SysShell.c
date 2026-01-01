#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <math.h>

#include "SysShell.h"

#include "SynClock.h"

#include "BitCodec.h"


typedef struct
{
	unsigned pushline;
	unsigned takeline;
	
	uint8_t buff[SYSSHELL_INPUT_BUFF_SIZE];
	
	char line[SYSSHELL_LINE_MAX_SIZE];
	
} InputBuff;

typedef struct
{
	unsigned pushline;
	unsigned takeline;
	
	uint8_t buff[SYSSHELL_PRINT_BUFF_SIZE];
	
} PrintBuff;


static bool sSysShellReady = false;

static unsigned sSysCommandCount;
static SysCommand sSysCommand[SYSSHELL_COMMAND_MAX_COUNT];

static SysCommand sHelpCommand;
static SysCommand sMdCommand;
static SysCommand sMwCommand;

static InputBuff sInputBuff;
static PrintBuff sPrintBuff;

static SysLoginChecker sSysLoginChecker;
static bool sSysShellLoggedin;

static uint32_t sActiveTime;


static int helpCmdRoutine(int argc, const char* argv[])
{
	unsigned i;
	
	if(argc == 1)
	{
		printk("Command         Introduction\n");
		printk("----------------------------\n");
		
		for(i = 0; i < sSysCommandCount; i ++)
		{
			printk("%s", sSysCommand[i].name);
			if(strlen(sSysCommand[i].name) < 8)
				printk("\t\t");
			else
				printk("\t");
			
			printk("%s\n", sSysCommand[i].info);
		}
	}
	else if(argc == 2)
	{
		for(i = 0; i < sSysCommandCount; i ++)
		{
			if(strcmp(argv[1], sSysCommand[i].name) == 0)
			{
				printk("%s", sSysCommand[i].help);
				
				break;
			}
		}
		if(i >= sSysCommandCount)
			printk("Command [%s] not found\n", argv[1]);
	}
	else
		printk("%s", sHelpCommand.help);
	
	return 0;
}

static int mdCmdRoutine(int argc, const char* argv[])
{
	unsigned i;
	
	uintptr_t address;
	unsigned count;
	
	address = 0; count = 0; 
	
	if(argc == 2)
	{
		if(sscanf(argv[1], "0x%x", &address) == 1)
			count = 1;
		else if(sscanf(argv[1], "%x", &address) == 1)
			count = 1;
		else
			count = 0;
	}
	else if(argc == 3)
	{
		if(sscanf(argv[1], "0x%x", &address) == 1)
		{
			if(sscanf(argv[2], "%u", &count) != 1)
				count = 0;
		}
		else if(sscanf(argv[1], "%x", &address) == 1)
		{
			if(sscanf(argv[2], "%u", &count) != 1)
				count = 0;
		}
		else
			count = 0;
	}
	else
		printk("%s", sMdCommand.help);
	
	if(count)
	{
		address = address / 4 * 4;
		
		for(i = 0; (i < count) && (i < 1000); i ++)
		{
			if((i % 4) == 0)
				printk("0x%08X: ", address + i * 4);
			
			printk(" %08x", REG32_GET(address + i * 4));
			
			if((i % 4) == 3)
				printk("\n");
		}
		if(i % 4)
			printk("\n");
	}
	
	return 0;
}

static int mwCmdRoutine(int argc, const char* argv[])
{
	uintptr_t address;
	unsigned value;
	
	address = 0; value = 0; 
	
	if(argc == 3)
	{
		if(sscanf(argv[1], "0x%x", &address) == 1)
		{
			if(sscanf(argv[2], "0x%x", &value) == 1)
			{
				printk("Writing %s with %s", argv[1], argv[2]);
				*(uint32_t*)address = value;
				printk(" OK\n");
			}
			else if(sscanf(argv[2], "%d", &value) == 1)
			{
				printk("Writing %s with %s", argv[1], argv[2]);
				*(uint32_t*)address = value;
				printk(" OK\n");
			}
		}
		else if(sscanf(argv[1], "%x", &address) == 1)
		{
			if(sscanf(argv[2], "0x%x", &value) == 1)
			{
				printk("Writing %s with %s", argv[1], argv[2]);
				*(uint32_t*)address = value;
				printk(" OK\n");
			}
			else if(sscanf(argv[2], "%d", &value) == 1)
			{
				printk("Writing %s with %s", argv[1], argv[2]);
				*(uint32_t*)address = value;
				printk(" OK\n");
			}
		}
	}
	else
		printk("%s", sMwCommand.help);
	
	return 0;
}

static int uartRxHandler(uint8_t byte, void* arg)
{
	unsigned nextline;
	
	(void)arg;
	
	if(!sSysShellReady)
		return -1;
	
	nextline = (sInputBuff.pushline + 1) % SYSSHELL_INPUT_BUFF_SIZE;
	
	if(nextline == sInputBuff.takeline)
		return -2;
	
	sInputBuff.buff[sInputBuff.pushline] = byte;
	
	sInputBuff.pushline = nextline;
	
	return 0;
}

static int printc(char c)
{
	while(uartTxSize(SYSSHELL_UART) < 1);
	if(uartTxByte(SYSSHELL_UART, c))
		return -1;
	
	return 0;
}

static int showLogin(void)
{
	printk("User password: ");
	
	return 0;
}

static int showPrompt(void)
{
	printk("> %s", sInputBuff.line);
	
	return 0;
}

int initSysShell(void)
{
	sSysCommandCount = 0;
	
	memset(&sSysCommand, 0, sizeof(sSysCommand));
	memset(&sHelpCommand, 0, sizeof(sHelpCommand));
	memset(&sMdCommand, 0, sizeof(sMdCommand));
	memset(&sMwCommand, 0, sizeof(sMwCommand));
	memset(&sInputBuff, 0, sizeof(sInputBuff));
	memset(&sPrintBuff, 0, sizeof(sPrintBuff));
	
	// Help command
	sHelpCommand.name = "help";
	sHelpCommand.info = "Command help";
	sHelpCommand.help =
		"help - For listing all commands and introductions\n"
		"help [COMMAND] - For showing designated [COMMAND] help\n";
	sHelpCommand.routine = helpCmdRoutine;
	
	sSysCommand[sSysCommandCount] = sHelpCommand;
	sSysCommandCount += 1;
	
	// Md command
	sMdCommand.name = "md";
	sMdCommand.info = "System address read";
	sMdCommand.help =
		"md [0x]{ADDRESS} [COUNT] - For read system address at {ADDRESS} with max 1000 [COUNT]\n";
	sMdCommand.routine = mdCmdRoutine;
	
	sSysCommand[sSysCommandCount] = sMdCommand;
	sSysCommandCount += 1;
	
	// Mw command
	sMwCommand.name = "mw";
	sMwCommand.info = "System address write";
	sMwCommand.help =
		"mw [0x]{ADDRESS} [0x]{VALUE} - For write system address with {VALUE} at {ADDRESS}\n";
	sMwCommand.routine = mwCmdRoutine;
	
	sSysCommand[sSysCommandCount] = sMwCommand;
	sSysCommandCount += 1;
	
	sSysLoginChecker = NULL;
	sSysShellLoggedin = false;
	
	// Register UART
	if(registerUartHandler(SYSSHELL_UART, uartRxHandler, NULL, SYSSHELL_UART_INTERRUPT_PRIORITY))
		return -1;
	
	sActiveTime = msClock();
	
	// Ready
	sSysShellReady = true;
	
	return 0;
}

static int execCommand(char line[SYSSHELL_LINE_MAX_SIZE])
{
	unsigned i;
	
	unsigned length;
	
	int argc;
	const char* argv[SYSSHELL_CMDARGS_MAX_COUNT];
	
	length = strlen(line);
	
	for(argc = 0, i = 0; i < length; i ++)
	{
		if((line[i] == ' ') || (line[i] == '\t'))
		{
			line[i] = '\0';
			
			if(i + 1 < length)
			{
				if((line[i + 1] != ' ') && (line[i + 1] != '\t'))
				{
					argv[argc] = &line[i + 1];
					argc += 1;
					
					if(argc >= SYSSHELL_CMDARGS_MAX_COUNT)
						break;
				}
			}
		}
		else if(!argc)
		{
			argv[argc] = &line[i];
			argc += 1;
		}
	}
	
	if(argc)
	{
		for(i = 0; i < sSysCommandCount; i ++)
		{
			if(strcmp(argv[0], sSysCommand[i].name) == 0)
			{
				sSysCommand[i].routine(argc, argv);
				
				break;
			}
		}
		if(i >= sSysCommandCount)
			printk("Command [%s] not found\n", argv[0]);
	}
	
	return 0;
}

int sysShellRoutine(void)
{
	char c;
	
	if(sInputBuff.takeline != sInputBuff.pushline)
	{
		while(sInputBuff.takeline != sInputBuff.pushline)
		{
			c = sInputBuff.buff[sInputBuff.takeline];
			
			sInputBuff.takeline = (sInputBuff.takeline + 1) % SYSSHELL_INPUT_BUFF_SIZE;
			
			// RETURN
			if(c == '\r')
			{
				printc('\r'), printc('\n');
				
				if(sSysShellLoggedin)
				{
					if(strlen(sInputBuff.line))
					{
						execCommand(sInputBuff.line);
						
						memset(sInputBuff.line, 0, sizeof(sInputBuff.line));
					}
					
					showPrompt();
				}
				else
				{
					// Login check
					if(sSysLoginChecker)
					{
						// Reject
						if(sSysLoginChecker(sInputBuff.line))
						{
							memset(sInputBuff.line, 0, sizeof(sInputBuff.line));
							
							showLogin();
						}
						// Accept
						else
						{
							memset(sInputBuff.line, 0, sizeof(sInputBuff.line));
							sSysShellLoggedin = true;
							
							printk("\n");
							printk("Press [help] for command list\n");
							
							showPrompt();
						}
					}
					// Open login
					else
					{
						memset(sInputBuff.line, 0, sizeof(sInputBuff.line));
						sSysShellLoggedin = true;
						
						printk("\n");
						printk("Press [help] for command list\n");
						
						showPrompt();
					}
				}
			}
			// DEL
			else if(c == '\177')
			{
				if(strlen(sInputBuff.line))
				{
					if(sSysShellLoggedin) printc('\177');
					
					sInputBuff.line[strlen(sInputBuff.line) - 1] = '\0';
				}
			}
			// NORMAL
			else if((c < '\177') && (c > '\037'))
			{
				if(strlen(sInputBuff.line) + 1 < SYSSHELL_LINE_MAX_SIZE)
				{
					if(sSysShellLoggedin) printc(c);
					
					sInputBuff.line[strlen(sInputBuff.line)] = c;
				}
			}
		}
		
		sActiveTime = msClock();
	}
	else if(sSysShellLoggedin)
	{
		// Login timeout
		if(sSysLoginChecker)
		{
			// Logout
			if(msClock() - sActiveTime > SYSSHELL_ACTIVE_TIME_MS)
			{
				printk("\n");
				printk("User login timeout\n");
				
				memset(sInputBuff.line, 0, sizeof(sInputBuff.line));
				sSysShellLoggedin = false;
				
				sysShellLogin();
			}
		}
		// Input timeout
		else if(strlen(sInputBuff.line))
		{
			// Clear input
			if(msClock() - sActiveTime > SYSSHELL_ACTIVE_TIME_MS)
			{
				printk("\n");
				printk("Command input timeout\n");
				
				memset(sInputBuff.line, 0, sizeof(sInputBuff.line));
				
				showPrompt();
			}
		}
	}
	// Passwd timeout
	else if(strlen(sInputBuff.line))
	{
		// Clear passwd
		if(msClock() - sActiveTime > SYSSHELL_ACTIVE_TIME_MS)
		{
			memset(sInputBuff.line, 0, sizeof(sInputBuff.line));
			
			printk("\n");
			
			showLogin();
		}
	}
	
	if(sPrintBuff.takeline != sPrintBuff.pushline)
	{
		printc('\n');
		
		do {
			c = sPrintBuff.buff[sPrintBuff.takeline];
			
			sPrintBuff.takeline = (sPrintBuff.takeline + 1) % SYSSHELL_PRINT_BUFF_SIZE;
			
			if(c)
				printc(c);
			else
				break;
			
			if(c == '\n')
				break;
			
		} while(sPrintBuff.takeline != sPrintBuff.pushline);
		
		if(sPrintBuff.takeline == sPrintBuff.pushline)
		{
			if(c != '\n')
				printc('\r'), printc('\n');
			
			if(sSysShellLoggedin)
				showPrompt();
			else
				showLogin();
		}
	}
	
	return 0;
}

int sysShellLogin(void)
{
	if(!sSysLoginChecker)
	{
		sSysShellLoggedin = true;
		
		printk("\n");
		printk("Press [help] for command list\n");
		
		showPrompt();
	}
	else
	{
		printk("\n");
		
		showLogin();
	}
	
	return 0;
}

int sysPushStdPrint(const char* string)
{
	unsigned nextline;
	unsigned freeSize;
	unsigned printSize;
	
	if(!sSysShellReady)
		return -1;
	
	nextline = (sPrintBuff.pushline + 1) % SYSSHELL_PRINT_BUFF_SIZE;
	freeSize = (sPrintBuff.takeline + SYSSHELL_PRINT_BUFF_SIZE - nextline) % SYSSHELL_PRINT_BUFF_SIZE;
	
	printSize = strlen(string);
	
	if(printSize > freeSize)
		printSize = freeSize;
	
	if(SYSSHELL_PRINT_BUFF_SIZE - sPrintBuff.pushline >= printSize)
	{
		memcpy(&sPrintBuff.buff[sPrintBuff.pushline], string, printSize);
	
		sPrintBuff.pushline = (sPrintBuff.pushline + printSize) % SYSSHELL_PRINT_BUFF_SIZE;
	}
	else
	{
		memcpy(&sPrintBuff.buff[sPrintBuff.pushline], string, SYSSHELL_PRINT_BUFF_SIZE - sPrintBuff.pushline);
		memcpy(&sPrintBuff.buff[0], &string[SYSSHELL_PRINT_BUFF_SIZE - sPrintBuff.pushline], sPrintBuff.pushline + printSize - SYSSHELL_PRINT_BUFF_SIZE);
		
		sPrintBuff.pushline = (sPrintBuff.pushline + printSize) % SYSSHELL_PRINT_BUFF_SIZE;
	}
	
	return 0;
}

int registerSysCommand(const SysCommand* command)
{
	if(sSysCommandCount >= SYSSHELL_COMMAND_MAX_COUNT)
		return -1;
	
	sSysCommand[sSysCommandCount] = *command;
	
	sSysCommandCount += 1;
	
	return 0;
}

int registerLoginChecker(SysLoginChecker checker)
{
	sSysLoginChecker = checker;
	
	return 0;
}

int printk(const char* format, ...)
{
	unsigned i;
	
	uint64_t j, k;
	
	va_list argp;
	unsigned result;
	
	bool wide;
	char fill;
	unsigned width;
	
	int64_t intarg;
	uint64_t uintarg;
	double floatarg;
	const char* strarg;
	
	uint8_t byte;
	
	va_start(argp, format);
	i = 0; result = 0;
	
	while(format[i] != '\0')
	{
		// 转义
		if(format[i] == '\\')
		{
			i ++;
			
			// 提前结束
			if(format[i] == '\0')
				break;
			
			if(format[i] == '0')
				break;
			
			switch(format[i])
			{
			case 'r': printc('\r'); result ++; i ++; break;
			case 'n': printc('\n'); result ++; i ++; break;
			case 't': printc('\t'); result ++; i ++; break;
			case 'v': printc('\v'); result ++; i ++; break;
			case '?': printc('?'); result ++; i ++; break;
			case '\'': printc('\''); result ++; i ++; break;
			case '\"': printc('\"'); result ++; i ++; break;
			case '\\': printc('\\'); result ++; i ++; break;
			
			case 'x':
				if(((format[i + 1] != '\0') && (format[i + 2] != '\0'))
				&& (((format[i + 1] >= '0') && (format[i + 1] <= '9')) || ((format[i + 1] >= 'a') && (format[i + 1] <= 'f')) || ((format[i + 1] >= 'A') && (format[i + 1] <= 'F')))
				&& (((format[i + 2] >= '0') && (format[i + 2] <= '9')) || ((format[i + 2] >= 'a') && (format[i + 2] <= 'f')) || ((format[i + 2] >= 'A') && (format[i + 2] <= 'F'))))
				{
					if((format[i + 1] >= '0') && (format[i + 1] <= '9'))
						byte = (format[i + 1] - '0') * 16;
					else if((format[i + 1] >= 'a') && (format[i + 1] <= 'f'))
						byte = ((format[i + 1] - 'a') + 10) * 16;
					else if((format[i + 1] >= 'A') && (format[i + 1] <= 'F'))
						byte = ((format[i + 1] - 'a') + 10) * 16;
					else // Never here, avoid warning
						byte = 0;
					
					if((format[i + 2] >= '0') && (format[i + 2] <= '9'))
						byte += format[i + 1] - '0';
					else if((format[i + 2] >= 'a') && (format[i + 2] <= 'f'))
						byte += (format[i + 1] - 'a') + 10;
					else if((format[i + 2] >= 'A') && (format[i + 2] <= 'F'))
						byte += (format[i + 1] - 'A') + 10;
					
					printc(byte);
					result ++;
					i += 3;
				}
				
				break;
				
			default: i ++; break;
			}
			
			continue;
		}
		
		// 普通
		if(format[i] != '%')
		{
			printc(format[i ++]);
			result ++;
			
			continue;
		}
		
		// 格式
		i ++;
		
		if(format[i] == '\0')
			break;
		
		// fill
		if(format[i] == '0')
		{
			fill = '0';
			
			i ++;
			
			if(format[i] == '\0')
				break;
		}
		else
			fill = 0;
		
		// width
		width = 0; while((format[i] >= '1') && (format[i] <= '9'))
		{
			if(!fill) fill = ' ';
			width = width * 10 + (format[i] - '0');
			
			i ++;
			
			if(format[i] == '\0')
				break;
		}
		if(format[i] == '\0')
			break;
		
		// wide
		if(format[i] == 'l')
		{
			i ++;
			
			if(format[i] == '\0')
				break;
			
			if(format[i] == 'l')
			{
				i ++;
				
				if(format[i] == '\0')
					break;
				
				wide = true;
			}
			else if(format[i] == 'f')
				wide = true;
			else
				wide = false;
		}
		else
			wide = false;
		
		switch(format[i])
		{
		case 'o':
			if(wide)
				uintarg = va_arg(argp, uint64_t);
			else
				uintarg = va_arg(argp, uint32_t);
			
			if(uintarg > 0) for(j = 01000000000000000000000; j > 0; j = j / 8)
			{
				// 最高有数值的位开始
				if(uintarg >= j)
				{
					for(k = j; k > 0; k = k / 8)
					{
						printc(uintarg / k + '0');
						result ++;
						
						uintarg = uintarg % k;
					}
					
					break;
				}
				
				if(fill) if(width > log2(j) / 3)
				{
					printc(fill);
					result ++;
				}
			}
			else
			{
				if(fill && width) for(j = 0; j < width - 1; j ++)
				{
					printc(fill);
					result ++;
				}
				
				printc('0');
				result ++;
			}
			
			i ++;
			
			break;
			
		case 'd':
			if(wide)
				intarg = va_arg(argp, int64_t);
			else
				intarg = va_arg(argp, int32_t);
			
			if(intarg < 0)
			{
				printc('-');
				result ++;
				
				uintarg = -intarg;
			}
			else
				uintarg = intarg;
			
			if(uintarg > 0) for(j = 10000000000000000000ull; j > 0; j = j / 10)
			{
				// 最高有数值的位开始
				if(uintarg >= j)
				{
					for(k = j; k > 0; k = k / 10)
					{
						printc(uintarg / k + '0');
						result ++;
						
						uintarg = uintarg % k;
					}
					
					break;
				}
				
				if(fill) if(width > log10(j))
				{
					printc(fill);
					result ++;
				}
			}
			else
			{
				if(fill && width) for(j = 0; j < width - 1; j ++)
				{
					printc(fill);
					result ++;
				}
				
				printc('0');
				result ++;
			}
			
			i ++;
			
			break;
			
		case 'u':
			if(wide)
				uintarg = va_arg(argp, uint64_t);
			else
				uintarg = va_arg(argp, uint32_t);
			
			if(uintarg > 0) for(j = 10000000000000000000ull; j > 0; j = j / 10)
			{
				// 最高有数值的位开始
				if(uintarg >= j)
				{
					for(k = j; k > 0; k = k / 10)
					{
						printc(uintarg / k + '0');
						result ++;
						
						uintarg = uintarg % k;
					}
					
					break;
				}
				
				if(fill) if(width > log10(j))
				{
					printc(fill);
					result ++;
				}
			}
			else
			{
				if(fill && width) for(j = 0; j < width - 1; j ++)
				{
					printc(fill);
					result ++;
				}
				
				printc('0');
				result ++;
			}
			
			i ++;
			
			break;
			
		case 'p':
			width = 8;
			fill = '0';
			
			// Downward
			
		case 'x':
			if(wide)
				uintarg = va_arg(argp, uint64_t);
			else
				uintarg = va_arg(argp, uint32_t);
			
			if(uintarg > 0) for(j = 0x1000000000000000; j > 0; j = j / 16)
			{
				// 最高有数值的位开始
				if(uintarg >= j)
				{
					for(k = j; k > 0; k = k / 16)
					{
						if(uintarg / k < 10)
						{
							printc(uintarg / k + '0');
							result ++;
						}
						else
						{
							printc(uintarg / k - 10 + 'a');
							result ++;
						}
						
						uintarg = uintarg % k;
					}
					
					break;
				}
				
				if(fill) if(width > log2(j) / 4)
				{
					printc(fill);
					result ++;
				}
			}
			else
			{
				if(fill && width) for(j = 0; j < width - 1; j ++)
				{
					printc(fill);
					result ++;
				}
				
				printc('0');
				result ++;
			}
			
			i ++;
			
			break;
			
		case 'X':
			if(wide)
				uintarg = va_arg(argp, uint64_t);
			else
				uintarg = va_arg(argp, uint32_t);
			
			if(uintarg > 0) for(j = 0x1000000000000000; j > 0; j = j / 16)
			{
				// 最高有数值的位开始
				if(uintarg >= j)
				{
					for(k = j; k > 0; k = k / 16)
					{
						if(uintarg / k < 10)
						{
							printc(uintarg / k + '0');
							result ++;
						}
						else
						{
							printc(uintarg / k - 10 + 'A');
							result ++;
						}
						
						uintarg = uintarg % k;
					}
					
					break;
				}
				
				if(fill) if(width > log2(j) / 4)
				{
					printc(fill);
					result ++;
				}
			}
			else
			{
				if(fill && width) for(j = 0; j < width - 1; j ++)
				{
					printc(fill);
					result ++;
				}
				
				printc('0');
				result ++;
			}
			
			i ++;
			
			break;
			
		case 'e': case 'E': // TODO
		case 'g': case 'G': // TODO
		
		case 'f':
			// 浮点数在可变参传递时提升为 double
			floatarg = va_arg(argp, double);
			
			if(!wide)
				floatarg = (float)floatarg;
			
			if(floatarg < 0)
			{
				printc('-');
				result ++;
				
				floatarg = -floatarg;
			}
			
			uintarg = floatarg;
			floatarg -= uintarg;
			
			// TODO 不支持大于 uint64_t 表示的最大值
			if(uintarg > 0) for(j = 10000000000000000000ull; j > 0; j = j / 10)
			{
				// 最高有数值的位开始
				if(uintarg >= j)
				{
					for(k = j; k > 0; k = k / 10)
					{
						printc(uintarg / k + '0');
						result ++;
						
						uintarg = uintarg % k;
					}
					
					break;
				}
			}
			else
			{
				printc('0');
				result ++;
			}
			
			printc('.');
			result ++;
			
			// TODO 不支持 10 位小数以上精度
			uintarg = floatarg * 10000000000ull;
			if(uintarg > 0) for(j = 1000000000ull; j > 0; j = j / 10)
			{
				printc(uintarg / j + '0');
				result ++;
				
				uintarg = uintarg % j;
				
				if(!uintarg)
					break;
			}
			else
			{
				printc('0');
				result ++;
			}
			
			i ++;
			
			break;
			
		case 'c':
			// 字符在可变参传递时提升为 int
			intarg = va_arg(argp, int);
			printc((char)intarg);
			result ++;
			
			i ++;
			
			break;
			
		case 's':
			strarg = va_arg(argp, const char*);
			while(*strarg != 0)
			{
				printc(*strarg);
				strarg ++;
				result ++;
			}
			
			i ++;
			
			break;
			
		default:
			break;
		}
	}
	
	va_end(argp);
	
	return result;
}
