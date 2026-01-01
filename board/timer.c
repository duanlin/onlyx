#include <string.h>

#include "timer.h"

#include "GD32E503.h"
#include "BitCodec.h"


static InterruptInfo sInterruptInfo[TimerCount];


int initTimer(void)
{
	memset(&sInterruptInfo, 0, sizeof(sInterruptInfo));
	
	// GD32E503_RCU_APB1RST
	// TIMER5RST [4]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB1RST), 4, 0b1);
	
	// GD32E503_RCU_APB1EN
	// TIMER5EN [4]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB1EN), 4, 0b1);
	
	// GD32E503_RCU_APB1RST
	// TIMER5RST [4]
	REG32_SET_BIT(GD32E503_RCU(GD32E503_RCU_APB1RST), 4, 0b0);
	
	// GD32E503_TIMER_CTL0
	REG32_SET(GD32E503_TIMER5(GD32E503_TIMERB_CTL0), 0);
	
	return 0;
}

int restartTimer(Timer timer, uint64_t interval)
{
	// TIMER_CTL0
	// CEN [0] 0b0
	switch(timer)
	{
	case Timer5: REG32_SET_BIT(GD32E503_TIMER5(GD32E503_TIMERB_CTL0), 0, 0b0); break;
	
	default:
		return -1;
	}
	
	// TIMER_DMAINTEN
	// UPIE [0] 0b0
	switch(timer)
	{
	case Timer5: REG32_SET_BIT(GD32E503_TIMER5(GD32E503_TIMERB_DMAINTEN), 0, 0b0); break;
	
	default:
		return -2;
	}
	
	// TIMER_CNT
	// CNT [15:0] 0
	switch(timer)
	{
	case Timer5: REG32_SET(GD32E503_TIMER5(GD32E503_TIMERB_CNT), 0); break;
	
	default:
		return -3;
	}
	
	// TIMER_CAR
	switch(timer)
	{
	case Timer5: REG32_SET(GD32E503_TIMER5(GD32E503_TIMERB_CAR), interval); break;
	
	default:
		return -4;
	}
	
	// TIMER_CTL0
	// CEN [0] 0b1
	switch(timer)
	{
	case Timer5: REG32_SET_BIT(GD32E503_TIMER5(GD32E503_TIMERB_CTL0), 0, 0b1); break;
	
	default:
		return -5;
	}
	
	// TIMER_DMAINTEN
	// UPIE [0] 0b1
	switch(timer)
	{
	case Timer5: REG32_SET_BIT(GD32E503_TIMER5(GD32E503_TIMERB_DMAINTEN), 0, 0b1); break;
	
	default:
		return -6;
	}
	
	return 0;
}

int setTimerInterval(Timer timer, uint64_t interval)
{
	switch(timer)
	{
	case Timer5: REG32_SET(GD32E503_TIMER5(GD32E503_TIMERB_CAR), interval); break;
	
	default:
		return -1;
	}
	
	return 0;
}

static int timerInterruptHandler(void* arg)
{
	Timer timer;
	
	timer = (Timer)arg;
	
	// TIMER_INTF
	switch(timer)
	{
	case Timer5: REG32_SET(GD32E503_TIMER5(GD32E503_TIMERB_INTF), 0); break;
	
	default:
		return -1;
	}
	
	if(sInterruptInfo[timer].handler)
	{
		if(sInterruptInfo[timer].handler(sInterruptInfo[timer].arg))
			return -2;
	}
	
	return 0;
}

int registerTimerInterrupt(Timer timer, uint64_t interval, InterruptHandler handler, void* arg, uint8_t priority)
{
	// TIMER_CTL0
	// ARSE [7] 0b0
	// SPM [3] 0b0
	// UPS [2] 0b1
	// UPDIS [1] 0b0
	// CEN [0] 0b0
	switch(timer)
	{
	case Timer5: REG32_SET(GD32E503_TIMER5(GD32E503_TIMERB_CTL0), 0b100); break;
	
	default:
		return -1;
	}
	
	// TIMER_DMAINTEN
	// UPDEN [8] 0b0
	// UPIE [0] 0b0
	switch(timer)
	{
	case Timer5: REG32_SET(GD32E503_TIMER5(GD32E503_TIMERB_DMAINTEN), 0); break;
	
	default:
		return -2;
	}
	
	// TIMER_INTF
	switch(timer)
	{
	case Timer5: REG32_SET(GD32E503_TIMER5(GD32E503_TIMERB_INTF), 0); break;
	
	default:
		return -3;
	}
	
	// TIMER_CNT
	// CNT [15:0] 0
	switch(timer)
	{
	case Timer5: REG32_SET(GD32E503_TIMER5(GD32E503_TIMERB_CNT), 0); break;
	
	default:
		return -4;
	}
	
	// TIMER_PSC
	// PSC [15:0] 0
	switch(timer)
	{
	case Timer5: REG32_SET(GD32E503_TIMER5(GD32E503_TIMERB_PSC), 0); break;
	
	default:
		return -5;
	}
	
	// TIMER_CAR
	// CAR [15:0]
	switch(timer)
	{
	case Timer5: REG32_SET(GD32E503_TIMER5(GD32E503_TIMERB_CAR), interval); break;
	
	default:
		return -6;
	}
	
	// Register
	switch(timer)
	{
	case Timer5:
		if(registerInterrupt(IrqTimer5, timerInterruptHandler, (void*)timer, priority))
			return -7;
		
		break;
		
	default:
		return -8;
	}
	
	// Handler and argument
	sInterruptInfo[timer].arg = arg;
	sInterruptInfo[timer].handler = handler;
	
	// TIMER_CTL0
	// CEN [0] 0b1
	switch(timer)
	{
	case Timer5: REG32_SET_BIT(GD32E503_TIMER5(GD32E503_TIMERB_CTL0), 0, 0b1); break;
	
	default:
		return -9;
	}
	
	// TIMER_DMAINTEN
	// UPIE [0] 0b1
	switch(timer)
	{
	case Timer5: REG32_SET_BIT(GD32E503_TIMER5(GD32E503_TIMERB_DMAINTEN), 0, 0b1); break;
	
	default:
		return -10;
	}
	
	return 0;
}

int disableTimerInterrupt(Timer timer)
{
	// TIMER_DMAINTEN
	// UPIE [0] 0b0
	switch(timer)
	{
	case Timer5: REG32_SET_BIT(GD32E503_TIMER5(GD32E503_TIMERB_DMAINTEN), 0, 0b0); break;
	
	default:
		return -1;
	}
	
	// Handler and argument
	sInterruptInfo[timer].handler = NULL;
	sInterruptInfo[timer].arg = NULL;
	
	switch(timer)
	{
	case Timer5:
		if(disableInterrupt(IrqTimer5))
			return -2;
		
		break;
		
	default:
		return -3;
	}
	
	return 0;
}
