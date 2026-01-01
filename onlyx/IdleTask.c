#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "IdleTask.h"

#include "MailBox.h"
#include "SysShell.h"
#include "SoftDog.h"


MailBox sIdleMailBox;

static IdleWork sSysIdleWork[IDLETASK_SYSIDLE_MAX_COUNT];
static void* sSysIdleArg[IDLETASK_SYSIDLE_MAX_COUNT];

static IdleWork sUserIdleWork;
static void* sUserIdleArg;


int initIdleTask(void)
{
	static IdleWork sIdleWorks[IDLETASK_WORK_MAX_PEND];
	static void* sIdleWorkArgs[IDLETASK_WORK_MAX_PEND];
	
	memset(&sSysIdleWork, 0, sizeof(sSysIdleWork));
	memset(&sSysIdleArg, 0, sizeof(sSysIdleArg));
	
	memset(&sIdleWorks, 0, sizeof(sIdleWorks));
	memset(&sIdleWorkArgs, 0, sizeof(sIdleWorkArgs));
	memset(&sIdleMailBox, 0, sizeof(sIdleMailBox));
	
	sIdleMailBox.mails = (void**)sIdleWorks;
	sIdleMailBox.args = sIdleWorkArgs;
	sIdleMailBox.size = IDLETASK_WORK_MAX_PEND;
	
	sUserIdleWork = NULL;
	
	return 0;
}

int sysRegisterIdle(IdleWork work, void* arg)
{
	unsigned i;
	
	for(i = 0; i < IDLETASK_SYSIDLE_MAX_COUNT; i ++)
	{
		if(sSysIdleWork[i])
			continue;
		
		sSysIdleArg[i] = arg;
		sSysIdleWork[i] = work;
		
		break;
	}
	if(i >= IDLETASK_SYSIDLE_MAX_COUNT)
		return -1;
	
	return 0;
}

int registerUserIdle(IdleWork work, void* arg)
{
	sUserIdleWork = NULL;
	
	sUserIdleArg = arg;
	sUserIdleWork = work;
	
	return 0;
}

int pushIdleWork(IdleWork work, void* arg)
{
	return pushMail(&sIdleMailBox, work, arg);
}

static int mailHandler(void* mail, void* arg)
{
	IdleWork work;
	
	work = (IdleWork)mail;
	
	work(arg);
	
	return 0;
}

int idle(void)
{
	unsigned i;
	
	if(sysShellLogin())
		return -1;
	
	while(true)
	{
		// Pending works
		takeMail(&sIdleMailBox, mailHandler);
		
		// Shell interact
		sysShellRoutine();
		
		// Sys idle
		for(i = 0; i < IDLETASK_SYSIDLE_MAX_COUNT; i ++)
		{
			if(sSysIdleWork[i])
				sSysIdleWork[i](sSysIdleArg[i]);
		}
		
		// User idle
		if(sUserIdleWork)
			sUserIdleWork(sUserIdleArg);
		
		// SoftDog
		feedSoftDog();
	}
	
	return 0;
}
