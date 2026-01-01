#ifndef IDLETASK_H
#define IDLETASK_H


#define IDLETASK_SYSIDLE_MAX_COUNT 3

#define IDLETASK_WORK_MAX_PEND 128


#ifdef __cplusplus
extern "C"
{
#endif


typedef int (*IdleWork)(void* arg);


int initIdleTask(void);

int sysRegisterIdle(IdleWork work, void* arg);

int registerUserIdle(IdleWork work, void* arg);

int pushIdleWork(IdleWork work, void* arg);

int idle(void);


#ifdef __cplusplus
}
#endif


#endif
