#include <stdint.h>

#include "MailBox.h"

#include "SysCall.h"


int pushMail(MailBox* mailbox, void* mail, void* arg)
{
	return (intptr_t)sysCall(SysCallSendMail, (void*)mailbox, mail, arg);
}

unsigned mailBoxPending(const MailBox* mailbox)
{
	return (mailbox->writeline + mailbox->size - mailbox->readline) % mailbox->size;
}

int takeMail(MailBox* mailbox, MailHandler handler)
{
	int result;
	
	if(mailbox->readline != mailbox->writeline)
	{
		result = handler(mailbox->mails[mailbox->readline], mailbox->args[mailbox->readline]);
		
		mailbox->readline = (mailbox->readline + 1) % mailbox->size;
		
		return result;
	}
	
	return 0;
}
