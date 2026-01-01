#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>

#include "SysCall.h"
#include "SysShell.h"


void* __wrap_malloc(size_t size)
{
	return sysCall(SysCallHeapAlloc, (void*)size, NULL, NULL);
}

void __wrap_free(void* p)
{
	sysCall(SysCallHeapFree, p, NULL, NULL);
}

int __wrap_putchar(int c)
{
	char line[2];
	
	line[0] = (char)c;
	line[1] = '\0';
	
	if(sysCall(SysCallStdPrint, (void*)line, NULL, NULL))
		return EOF;
	
	return line[0];
}

int __wrap_puts(const char* line)
{
	if(sysCall(SysCallStdPrint, (void*)line, NULL, NULL))
		return -1;
	
	return 0;
}

int __wrap_printf(const char* format, ...)
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
	
	char line[SYSSHELL_LINE_MAX_SIZE];
	
	memset(&line, 0, sizeof(line));
	
#define printc(C) do { \
	if(result + 1 < sizeof(line)) \
		line[result] = (C); \
	\
} while(false)
	
	va_start(argp, format);
	
	i = 0, result = 0; while(format[i] != '\0')
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
	
	if(sysCall(SysCallStdPrint, (void*)line, NULL, NULL))
		return -1;
	
	return result;
}

int __wrap_sscanf(const char* source, const char* format, ...)
{
	unsigned i, j;
	int result;
	
	bool wide;
	
	uintptr_t arg;
	int64_t intarg;
	uint64_t uintarg;
	
	va_list argp;
	
	va_start(argp, format);
	
	i = 0, j = 0, result = 0; while(source[i] != '\0')
	{
		// 普通
		if(format[j] != '%')
		{
			if(source[i] != format[j])
				break;
			
			i ++;
			
			j ++;
			
			if(format[j] == '\0')
				break;
			
			continue;
		}
		
		// 格式
		j ++;
		
		if(format[j] == '\0')
			break;
		
		// wide
		if(format[j] == 'l')
		{
			j ++;
			
			if(format[j] == '\0')
				break;
			
			if(format[j] == 'l')
			{
				j ++;
				
				if(format[j] == '\0')
					break;
				
				wide = true;
			}
			else if(format[j] == 'f')
				wide = true;
			else
				wide = false;
		}
		else
			wide = false;
		
		switch(format[j])
		{
		case 'd':
			arg = va_arg(argp, uintptr_t);
			
			if(source[i] == '-')
			{
				i ++;
				
				if(source[i] == '\0')
					break;
				
				if((source[i] >= '0') && (source[i] <= '9'))
				{
					intarg = -(source[i] - '0');
					
					i ++;
					
					result += 1;
				}
				else
					break;
			}
			else if(source[i] == '+')
			{
				i ++;
				
				if(source[i] == '\0')
					break;
				
				if((source[i] >= '0') && (source[i] <= '9'))
				{
					intarg = source[i] - '0';
					
					i ++;
					
					result += 1;
				}
				else
					break;
			}
			else if((source[i] >= '0') && (source[i] <= '9'))
			{
				intarg = source[i] - '0';
				
				i ++;
				
				result += 1;
			}
			else
				break;
			
			while((source[i] >= '0') && (source[i] <= '9'))
			{
				intarg = intarg * 10 + (source[i] - '0');
				
				i ++;
				
				if(source[i] == '\0')
					break;
			}
			
			if(wide)
				*(int64_t*)arg = intarg;
			else
				*(int32_t*)arg = intarg;
			
			break;
			
		case 'u':
			arg = va_arg(argp, uintptr_t);
			
			if(source[i] == '+')
			{
				i ++;
				
				if(source[i] == '\0')
					break;
				
				if((source[i] >= '0') && (source[i] <= '9'))
				{
					uintarg = source[i] - '0';
					
					i ++;
					
					result += 1;
				}
				else
					break;
			}
			else if((source[i] >= '0') && (source[i] <= '9'))
			{
				uintarg = source[i] - '0';
				
				i ++;
				
				result += 1;
			}
			else
				break;
			
			while((source[i] >= '0') && (source[i] <= '9'))
			{
				uintarg = uintarg * 10 + (source[i] - '0');
				
				i ++;
				
				if(source[i] == '\0')
					break;
			}
			
			if(wide)
				*(uint64_t*)arg = uintarg;
			else
				*(uint32_t*)arg = uintarg;
			
			break;
			
		case 'x': case 'X':
			arg = va_arg(argp, uintptr_t);
			
			uintarg = 0;
			
			if(((source[i] >= '0') && (source[i] <= '9'))
			|| ((source[i] >= 'a') && (source[i] <= 'f'))
			|| ((source[i] >= 'A') && (source[i] <= 'F')))
				result += 1;
			else
				break;
			
			while(((source[i] >= '0') && (source[i] <= '9'))
			|| ((source[i] >= 'a') && (source[i] <= 'f'))
			|| ((source[i] >= 'A') && (source[i] <= 'F')))
			{
				if((source[i] >= '0') && (source[i] <= '9'))
					uintarg = uintarg * 16 + (source[i] - '0');
				else if((source[i] >= 'a') && (source[i] <= 'f'))
					uintarg = uintarg * 16 + 10 + (source[i] - 'a');
				else if((source[i] >= 'A') && (source[i] <= 'F'))
					uintarg = uintarg * 16 + 10 + (source[i] - 'A');
				
				i ++;
				
				if(source[i] == '\0')
					break;
			}
			
			if(wide)
				*(uint64_t*)arg = uintarg;
			else
				*(uint32_t*)arg = uintarg;
			
			break;
			
		default:
			break;
		}
	}
	
	va_end(argp);
	
	return result;
}
