#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "MainTask.h"

#include "wdog.h"
#include "trap.h"
#include "timer.h"

#include "SynBus.h"
#include "UtcBus.h"

#include "SynClock.h"
#include "UtcClock.h"
#include "UniTimeX.h"

#include "UserTask.h"
#include "SoftDog.h"


static bool sMainTaskReady = false;
static unsigned sMainCounter;

static UserMain sUserMain;
static void* sUserMainArg;


static int mainRoutine(void* arg)
{
	static uint32_t edgeTime;
	static uint32_t lastEdgeTime;
	static bool lastEdgeValid = false;
	
	uint32_t now;
	uint32_t busEdgeTime;
	uint32_t busEdgeWidth;
	
	uint32_t expectedTime;
	bool synchronized;
	
	UserTask task;
	
	now = getUniTimeX();
	
	//
	// Main task scheduling
	//
	if(gSynBus.widthValid)
	{
		// Lock edge
		busEdgeTime = gSynBus.edgeTime;
		busEdgeWidth = gSynBus.edgeWidth;
		
		// New edge valid
		if(!lastEdgeValid)
		{
			sMainCounter = 0;
			
			edgeTime = busEdgeTime;
			lastEdgeTime = busEdgeTime;
		}
		// Predict next edge
		else
		{
			if(sMainCounter == 0)
				edgeTime += busEdgeWidth;
			
			// New edge comming
			if(busEdgeTime != lastEdgeTime)
			{
				if(sMainCounter < MAINTASK_FREQUENCY_HZ / 2)
					edgeTime = busEdgeTime;
				else
					edgeTime = busEdgeTime - busEdgeWidth;
				
				lastEdgeTime = busEdgeTime;
			}
		}
		
		expectedTime = edgeTime + (uint64_t)busEdgeWidth * sMainCounter / MAINTASK_FREQUENCY_HZ;
		
		// Need speed up
		if((signed)(now - expectedTime) > 0)
			setTimerInterval(MAINTASK_TIMER, TIMER_FREQUENCY_HZ / MAINTASK_FREQUENCY_HZ - 1);
		// Need slow down
		else if((signed)(now - expectedTime) < 0)
			setTimerInterval(MAINTASK_TIMER, TIMER_FREQUENCY_HZ / MAINTASK_FREQUENCY_HZ + 1);
		// Just catch up
		else
			setTimerInterval(MAINTASK_TIMER, TIMER_FREQUENCY_HZ / MAINTASK_FREQUENCY_HZ);
		
		lastEdgeValid = true;
		
		synchronized = true;
	}
	else
	{
		synchronized = false;
		
		lastEdgeValid = false;
		
		// Return to normal
		setTimerInterval(MAINTASK_TIMER, TIMER_FREQUENCY_HZ / MAINTASK_FREQUENCY_HZ);
	}
	
	// User main routine
	if(sUserMain)
		sUserMain(sMainCounter, synchronized, sUserMainArg);
	
	// Update clocks
	if(sMainCounter % (MAINTASK_FREQUENCY_HZ / SYNBUS_ROUTINE_FREQUENCY_HZ) == SYNBUS_MAINTASK_MOD_OFFSET)
		synBusRoutine();
	updateSynClock();
	
	if(sMainCounter % (MAINTASK_FREQUENCY_HZ / UTCBUS_ROUTINE_FREQUENCY_HZ) == UTCBUS_MAINTASK_MOD_OFFSET)
		utcBusRoutine(sMainCounter, synchronized);
	updateUtcClock();
	
	// User task schedule
	for(task = UserTask0; task < UserTaskCount; task ++)
		userTaskSchedule(sMainCounter, synchronized, task);
	
	// Check SoftDog
	if(checkSoftDog())
		while(true);
	// Feed HardDog
	feedHardDog();
	
	sMainCounter = (sMainCounter + 1) % MAINTASK_FREQUENCY_HZ;
	
	return 0;
}

int initMainTask(void)
{
	sMainCounter = 0;
	
	sUserMain = NULL;
	sUserMainArg = NULL;
	
	// Highest priority
	if(registerTimerInterrupt(MAINTASK_TIMER, TIMER_FREQUENCY_HZ / MAINTASK_FREQUENCY_HZ, mainRoutine, NULL, 0x00)) // The highest
		return -1;
	
	// Ready
	sMainTaskReady = true;
	
	return 0;
}

int registerUserMain(UserMain routine, void* arg)
{
	sUserMainArg = arg;
	sUserMain = routine;
	
	return 0;
}

int postMainRoutine(void)
{
	if(!sMainTaskReady)
		return -1;
	
	if(sMainCounter > 0)
		// Restart the timer
		restartTimer(MAINTASK_TIMER, TIMER_FREQUENCY_HZ / MAINTASK_FREQUENCY_HZ);
	else
		// Immediate timeout
		setInterruptPending(MAINTASK_IRQ);
	
	return 0;
}
