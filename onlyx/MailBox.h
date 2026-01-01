#ifndef MAILBOX_H
#define MAILBOX_H


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct
{
	unsigned size;
	
	unsigned readline;
	unsigned writeline;
	
	void** mails;
	void** args;
	
} MailBox;

typedef int (*MailHandler)(void* mail, void* arg);


int pushMail(MailBox* mailbox, void* mail, void* arg);

unsigned mailBoxPending(const MailBox* mailbox);

int takeMail(MailBox* mailbox, MailHandler handler);


#ifdef __cplusplus
}
#endif


#endif
