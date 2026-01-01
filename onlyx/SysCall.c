#include <stddef.h>
#include <string.h>

#include "SysCall.h"

#include "trap.h"

#include "SysHeap.h"
#include "MailBox.h"
#include "SysShell.h"


static int sysCallHandler(TrapContext* context)
{
	SysCall what;
	
	unsigned nextline;
	void* arg;
	
	size_t size;
	void* ptr;
	
	MailBox* mailbox;
	void* mail;
	
	const char* string;
	
	what = (SysCall)context->r0;
	
	switch(what)
	{
	case SysCallNone:
		context->r0 = 0;
		
		break;
		
	case SysCallHeapAlloc:
		size = (size_t)context->r1;
		context->r0 = (uintptr_t)sysHeapAlloc(size);
		
		break;
		
	case SysCallHeapFree:
		ptr = (void*)context->r1;
		sysHeapFree(ptr);
		
		context->r0 = 0;
		
		break;
		
	case SysCallSendMail:
		mailbox = (MailBox*)context->r1;
		mail = (void*)context->r2;
		arg = (void*)context->r3;
		
		nextline = (mailbox->writeline + 1) % mailbox->size;
		if(nextline == mailbox->readline)
		{
			context->r0 = -1;
			
			break;
		}
		
		mailbox->mails[mailbox->writeline] = mail;
		mailbox->args[mailbox->writeline] = arg;
		
		mailbox->writeline = nextline;
		
		context->r0 = 0;
		
		break;
		
	case SysCallStdPrint:
		string = (const char*)context->r1;
		
		if(sysPushStdPrint(string))
			context->r0 = -2;
		else
			context->r0 = 0;
		
		break;
		
	default:
		context->r0 = -3;
		
		break;
	}
	
	return 0;
}

int initSysCall(void)
{
	if(registerSysCallHandler(sysCallHandler))
		return -1;
	
	return 0;
}
