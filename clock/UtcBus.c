#include <stddef.h>
#include <string.h>

#include "UtcBus.h"

#include "trap.h"
#include "chip.h"

#include "UniTimeX.h"
#include "UtcClock.h"

#include "GD32E503.h"
#include "BitCodec.h"


UtcBus gUtcBus;


static int utcBusHandler(void* arg)
{
	uint32_t edgeTime;
	
	(void)arg;
	
	// PA3 UTC_RX EXTI3
	// PD3 [3] 0b1 Clear
	REG32_SET(GD32E503_EXTI(GD32E503_EXTI_PD), (uint32_t)0b1 << 3);
	
	// PA3 UTC_RX TIMER1_CH3
	//
	edgeTime = REG32_GET(GD32E503_TIMER1(GD32E503_TIMERL0_CH3CV));
	
	// Rx delay adjustment
	edgeTime = edgeTime - (uint64_t)UNITIMEX_COUNTER_FREQUENCY_HZ * gUtcBus.busRxDelayNs / 1000000000;
	
	// Continued edge
	if(gUtcBus.busEdgeValid)
	{
		// Valid width range
		if((edgeTime - gUtcBus.busEdgeTime > UNITIMEX_COUNTER_FREQUENCY_HZ - UNITIMEX_COUNTER_FREQUENCY_HZ / 1000000 * UTCBUS_EDGE_MAX_DIFF_US)
		&& (edgeTime - gUtcBus.busEdgeTime < UNITIMEX_COUNTER_FREQUENCY_HZ + UNITIMEX_COUNTER_FREQUENCY_HZ / 1000000 * UTCBUS_EDGE_MAX_DIFF_US))
		{
			gUtcBus.busEdgeWidth = edgeTime - gUtcBus.busEdgeTime;
			gUtcBus.busWidthValid = true;
		}
		else
		{
			// Invalidate width
			gUtcBus.busWidthValid = false;
			
			gUtcBus.busEdgeErrorCount += 1;
		}
	}
	
	gUtcBus.busEdgeTime = edgeTime;
	gUtcBus.busEdgeValid = true;
	
	gUtcBus.busEdgeRecvCount += 1;
	
	return 0;
}

int initUtcBus(void)
{
	memset(&gUtcBus, 0, sizeof(gUtcBus));
	
	// RS485 SYN bus
	// 3PEAK TPT75176
	gUtcBus.busRxDelayNs = 60;
	
	// PA5 UTC_TE
	//
	// GD32E503_GPIO_CTL0
	// CTL5 [23:22] 0b00 Push-Pull
	// MD5 [21:20] 0b11 Output 50MHz
	REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL0), 23, 20, 0b0011);
	// GD32E503_GPIO_BC
	REG32_SET_BIT(GD32E503_GPIOA(GD32E503_GPIO_BC), 5, 0b1);
	
	// PA2 UTC_TX
	//
	// GD32E503_GPIO_CTL0
	// CTL2 [11:10] 0b00 Push-Pull
	// MD2 [9:8] 0b11 Output 50MHz
	REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL0), 11, 8, 0b0011);
	// GD32E503_GPIO_BOP
	REG32_SET_BIT(GD32E503_GPIOA(GD32E503_GPIO_BOP), 2, 0b1);
	
	// PA3 UTC_RX TIMER1_CH3
	//
	// GD32E503_GPIO_CTL0
	// CTL3 [15:14] 0b01 Float
	// MD3 [13:12] 0b00 Input
	REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL0), 15, 12, 0b0100);
	// GD32E503_TIMERL0_CHTCL2
	// CH3EN [12] 0b0
	REG32_SET_BIT(GD32E503_TIMER1(GD32E503_TIMERL0_CHCTL2), 12, 0b0);
	// GD32E503_TIMERL0_CHCTL1
	// CH3CAPFLT [15:12] 0b0000
	// CH3CAPPSC [11:10] 0b00
	// CH3MS [9:8] 0b01
	REG32_SET_RANGE(GD32E503_TIMER1(GD32E503_TIMERL0_CHCTL1), 15, 8, 0b00000001);
	// GD32E503_TIMERL0_CHCTL2
	// CH3NP [15] 0b0
	// CH3P [13] 0b0
	// CH3EN [12] 0b1
	REG32_SET_RANGE(GD32E503_TIMER1(GD32E503_TIMERL0_CHCTL2), 15, 12, 0b0001);
	
	// PA3 UTC_RX EXTI3
	//
	// GD32E503_AFIO_EXTISS0
	// EXTI3_SS [15:12] 0b0000 PA3
	REG32_SET_RANGE(GD32E503_AFIO(GD32E503_AFIO_EXTISS0), 15, 12, 0b0000);
	// GD32E503_EXTI_EVEN
	// EVEN3 [3] 0b0
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_EVEN), 3, 0b0);
	// GD32E503_EXTI_RTEN
	// RTEN3 [3] 0b0
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_RTEN), 3, 0b0);
	// GD32E503_EXTI_FTEN
	// FTEN3 [3] 0b0
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_FTEN), 3, 0b0);
	// GD32E503_EXTI_SWIEV
	// SWIEV3 [3] 0b0
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_SWIEV), 3, 0b0);
	// GD32E503_EXTI_INTEN
	// INTEN3 [3] 0b0
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_INTEN), 3, 0b0);
	
	// GD32E503_EXTI_PD
	// PD3 [3] 0b1 Clear
	REG32_SET(GD32E503_EXTI(GD32E503_EXTI_PD), (uint32_t)0b1 << 3);
	
	// Register
	if(registerInterrupt(IrqExti3, utcBusHandler, NULL, UTCBUS_INTERRUPT_PRIORITY))
		return -1;
	
	// GD32E503_EXTI_RTEN
	// RTEN [3] 0b1
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_RTEN), 3, 0b1);
	
	// GD32E503_EXTI_INTEN
	// INTEN [3] 0b1
	REG32_SET_BIT(GD32E503_EXTI(GD32E503_EXTI_INTEN), 3, 0b1);
	
	return 0;
}

int utcBusRoutine(unsigned mainCounter, bool synchronized)
{
	uint32_t now;
	
	now = getUniTimeX();
	
	if(gUtcBus.busEdgeValid)
	{
		if(gUtcBus.busInfoValid)
		{
			// Update UTC clock
			gUtcClock.second = gUtcBus.busUtcSecond;
			gUtcClock.edge = gUtcBus.busEdgeTime;
			gUtcClock.width = gUtcBus.busEdgeWidth;
			gUtcClock.LSP = gUtcBus.busLSP;
			gUtcClock.LS = gUtcBus.busLS;
			gUtcClock.quality = gUtcBus.busQuality;
			
			gUtcBus.busEdgeValid = false;
			gUtcBus.busInfoValid = false;
		}
		// Timeout
		else if(now - gUtcBus.busEdgeTime > UNITIMEX_COUNTER_FREQUENCY_HZ / 1000 * UTCBUS_INFO_TIMEOUT_MS)
		{
			// Invalidate
			gUtcBus.busEdgeValid = false;
			
			gUtcBus.busInfoTimeoutCount += 1;
		}
	}
	else if(gUtcBus.busWidthValid)
	{
		// Missing
		if(now - gUtcBus.busEdgeTime > UNITIMEX_COUNTER_FREQUENCY_HZ * 3)
			// Invalidate
			gUtcBus.busWidthValid = false;
	}
	
	return 0;
}
