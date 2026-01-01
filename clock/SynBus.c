#include <stddef.h>
#include <string.h>

#include "SynBus.h"

#include "trap.h"
#include "chip.h"

#include "UniTimeX.h"
#include "SynClock.h"
#include "MainTask.h"

#include "GD32E503.h"
#include "BitCodec.h"


SynBus gSynBus;


static int synBusHandler(void* arg)
{
	uint32_t edgeTime;
	
	(void)arg;
	
	// PA1 SYN_RX EXTI1
	//
	// GD32E503_EXTI_PD
	// PD1 [1] 0b1 Clear
	REG32_SET(GD32E503_EXTI(GD32E503_EXTI_PD), (uint32_t)0b1 << 1);
	
	// PA1 SYN_RX TIMER1_CH1
	//
	// GD32E503_TIMERL0_CH1CV
	edgeTime = REG32_GET(GD32E503_TIMER1(GD32E503_TIMERL0_CH1CV));
	
	// Rx delay adjustment
	edgeTime = edgeTime - (uint64_t)UNITIMEX_COUNTER_FREQUENCY_HZ * gSynBus.busRxDelayNs / 1000000000;
	
	// Continued edge
	if(gSynBus.edgeValid)
	{
		// Valid width range
		if((edgeTime - gSynBus.edgeTime > UNITIMEX_COUNTER_FREQUENCY_HZ - UNITIMEX_COUNTER_FREQUENCY_HZ / 1000000 * SYNBUS_EDGE_MAX_DIFF_US)
		&& (edgeTime - gSynBus.edgeTime < UNITIMEX_COUNTER_FREQUENCY_HZ + UNITIMEX_COUNTER_FREQUENCY_HZ / 1000000 * SYNBUS_EDGE_MAX_DIFF_US))
		{
			gSynBus.edgeWidth = edgeTime - gSynBus.edgeTime;
			gSynBus.widthValid = true;
		}
		else
		{
			// Invalidate width
			gSynBus.widthValid = false;
			
			gSynBus.busEdgeErrorCount += 1;
		}
	}
	
	gSynBus.edgeTime = edgeTime;
	gSynBus.edgeValid = true;
	
	gSynBus.busEdgeRecvCount += 1;
	
	// Post main routine for edge alignment
	postMainRoutine();
	
	return 0;
}

int initSynBus(void)
{
	memset(&gSynBus, 0, sizeof(gSynBus));
	
	// RS485 SYN bus
	// 3PEAK TPT75176
	gSynBus.busRxDelayNs = 60;
	
	// PA4 SYN_TE
	//
	// GD32E503_GPIO_CTL0
	// CTL4 [19:18] 0b00 Push-Pull
	// MD4 [17:16] 0b11 Output 50MHz
	REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL0), 19, 16, 0b0011);
	// GD32E503_GPIO_BC
	REG32_SET_BIT(GD32E503_GPIOA(GD32E503_GPIO_BC), 4, 0b1);
	
	// PA6 SYN_TX
	//
	// GD32E503_GPIO_CTL0
	// CTL6 [27:26] 0b00 Push-Pull
	// MD6 [25:24] 0b11 Output 50MHz
	REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL0), 27, 24, 0b0011);
	// GD32E503_GPIO_BOP
	REG32_SET_BIT(GD32E503_GPIOA(GD32E503_GPIO_BOP), 6, 0b1);
	
	// PA1 SYN_RX TIMER1_CH1
	//
	// GD32E503_GPIO_CTL0
	// CTL1 [7:6] 0b01 Float
	// MD1 [5:4] 0b00 Input
	REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL0), 7, 4, 0b0100);
	// GD32E503_TIMERL0_CHCTL2
	// CH1EN [4] 0b0
	REG32_SET_BIT(GD32E503_TIMER1(GD32E503_TIMERL0_CHCTL2), 4, 0b0);
	// GD32E503_TIMERL0_CHCTL0
	// CH1CAPFLT [15:12] 0b0000
	// CH1CAPPSC [11:10] 0b00
	// CH1MS [9:8] 0b01
	REG32_SET_RANGE(GD32E503_TIMER1(GD32E503_TIMERL0_CHCTL0), 15, 8, 0b00000001);
	// GD32E503_TIMERL0_CHCTL2
	// CH1NP [7] 0b0
	// CH1P [5] 0b0
	// CH1EN [4] 0b1
	REG32_SET_RANGE(GD32E503_TIMER1(GD32E503_TIMERL0_CHCTL2), 7, 4, 0b0001);
	
	// PA1 SYN_RX EXTI1
	//
	// GD32E503_AFIO_EXTISS0
	// EXTI1_SS [7:4] 0b0000 PA1
	REG32_SET_RANGE(GD32E503_AFIO(GD32E503_AFIO_EXTISS0), 7, 4, 0b0000);
	// GD32E503_EXTI_EVEN
	// EVEN1 [1] 0b0
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_EVEN), 1, 0b0);
	// GD32E503_EXTI_RTEN
	// RTEN1 [1] 0b0
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_RTEN), 1, 0b0);
	// GD32E503_EXTI_FTEN
	// FTEN1 [1] 0b0
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_FTEN), 1, 0b0);
	// GD32E503_EXTI_SWIEV
	// SWIEV1 [1] 0b0
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_SWIEV), 1, 0b0);
	// GD32E503_EXTI_INTEN
	// INTEN1 [1] 0b0
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_INTEN), 1, 0b0);
	
	// GD32E503_EXTI_PD
	// PD1 [1] 0b1 Clear
	REG32_SET(GD32E503_EXTI(GD32E503_EXTI_PD), (uint32_t)0b1 << 1);
	
	// Register
	if(registerInterrupt(IrqExti1, synBusHandler, NULL, SYNBUS_INTERRUPT_PRIORITY))
		return -1;
	
	// GD32E503_EXTI_RTEN
	// RTEN [1] 0b1
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_RTEN), 1, 0b1);
	
	// GD32E503_EXTI_INTEN
	// INTEN1 [1] 0b1
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_INTEN), 1, 0b1);
	
	return 0;
}

int synBusRoutine(void)
{
	uint32_t now;
	
	now = getUniTimeX();
	
	if(gSynBus.edgeValid)
	{
		// Timeout
		if(now - gSynBus.edgeTime > UNITIMEX_COUNTER_FREQUENCY_HZ / 1000000 * (1000000 + SYNBUS_EDGE_MAX_DIFF_US))
		{
			// Invalidate
			gSynBus.edgeValid = false;
			
			// Update SYN clock
			gSynClock.quality = SynQualityMiss;
			
			gSynBus.busEdgeTimeoutCount += 1;
		}
		else
		{
			// Update SYN clock
			gSynClock.edge = gSynBus.edgeTime;
			gSynClock.width = gSynBus.edgeWidth;
			gSynClock.quality = SynQualityGood;
		}
	}
	else if(gSynBus.widthValid)
	{
		// Missing
		if(now - gSynBus.edgeTime > UNITIMEX_COUNTER_FREQUENCY_HZ * 3)
		{
			// Invalidate
			gSynBus.widthValid = false;
			
			// Quality reducing
			gSynClock.quality = SynQualityLost;
		}
	}
	
	return 0;
}
