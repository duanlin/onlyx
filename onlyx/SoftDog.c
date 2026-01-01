#include <stdint.h>

#include "SoftDog.h"

#include "MainTask.h"


static unsigned sSoftDog;

static unsigned sCheckDog;
static unsigned sUnChange;


int initSoftDog(void)
{
	sSoftDog = 0;
	
	sCheckDog = 0;
	sUnChange = 0;
	
	return 0;
}

int feedSoftDog(void)
{
	sSoftDog += 1;
	
	return 0;
}

int checkSoftDog(void)
{
	if(sCheckDog != sSoftDog)
	{
		sCheckDog = sSoftDog;
		sUnChange = 0;
	}
	else
	{
		sUnChange += 1;
		
		if(sUnChange > MAINTASK_FREQUENCY_HZ / 1000 * SOFTDOG_TIMEOUT_MS)
			return -1;
	}
	
	return 0;
}
