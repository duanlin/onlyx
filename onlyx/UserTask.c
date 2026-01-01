#include <stddef.h>
#include <string.h>

#include "UserTask.h"

#include "trap.h"


#define USERTASK_IRQ_TASK0 78
#define USERTASK_IRQ_TASK1 79
#define USERTASK_IRQ_TASK2 80


UserScheder sUserScheders[UserTaskCount];
UserRoutine sUserRoutines[UserTaskCount];
void* sUserTaskArgs[UserTaskCount];


static int userRoutine(void* arg)
{
	if(sUserRoutines[(UserTask)arg])
		sUserRoutines[(UserTask)arg](sUserTaskArgs[(UserTask)arg]);
	
	return 0;
}

int initUserTask(void)
{
	memset(&sUserScheders, 0, sizeof(sUserScheders));
	memset(&sUserRoutines, 0, sizeof(sUserRoutines));
	memset(&sUserTaskArgs, 0, sizeof(sUserTaskArgs));
	
	if(registerInterrupt(USERTASK_IRQ_TASK0, userRoutine, (void*)UserTask0, USERTASK_INTERRUPT_PRIORITY0))
		return -1;
	if(registerInterrupt(USERTASK_IRQ_TASK1, userRoutine, (void*)UserTask1, USERTASK_INTERRUPT_PRIORITY1))
		return -2;
	if(registerInterrupt(USERTASK_IRQ_TASK2, userRoutine, (void*)UserTask2, USERTASK_INTERRUPT_PRIORITY2))
		return -3;
	
	return 0;
}

int registerUserTask(UserTask task, UserScheder scheder, UserRoutine routine, void* arg)
{
	sUserTaskArgs[task] = arg;
	sUserScheders[task] = scheder;
	sUserRoutines[task] = routine;
	
	return 0;
}

int userTaskSchedule(unsigned mainCounter, bool synchronized, UserTask task)
{
	if(sUserScheders[task])
		sUserScheders[task](mainCounter, synchronized, sUserTaskArgs[task]);
	
	return 0;
}

int postUserRoutine(UserTask task)
{
	switch(task)
	{
	case UserTask0: if(setInterruptPending(USERTASK_IRQ_TASK0)) return -1; break;
	case UserTask1: if(setInterruptPending(USERTASK_IRQ_TASK1)) return -2; break;
	case UserTask2: if(setInterruptPending(USERTASK_IRQ_TASK2)) return -3; break;
	
	default:
		return -4;
	}
	
	return 0;
}
