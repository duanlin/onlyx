#ifndef USERTASK_H
#define USERTASK_H


#include <stdbool.h>


#define USERTASK_INTERRUPT_PRIORITY0 0xA0
#define USERTASK_INTERRUPT_PRIORITY1 0xB0
#define USERTASK_INTERRUPT_PRIORITY2 0xC0


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
	UserTask0 = 0,
	UserTask1,
	UserTask2,
	
	UserTaskCount
	
} UserTask;

typedef int (*UserScheder)(unsigned mainCounter, bool synchronized, void* arg);
typedef int (*UserRoutine)(void* arg);


int initUserTask(void);

int registerUserTask(UserTask task, UserScheder scheder, UserRoutine routine, void* arg);

int userTaskSchedule(unsigned mainCounter, bool synchronized, UserTask task);

int postUserRoutine(UserTask task);


#ifdef __cplusplus
}
#endif


#endif
