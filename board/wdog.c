#include <stdint.h>

#include "wdog.h"

#include "GD32E503.h"
#include "BitCodec.h"


int initWdog(void)
{
	// WatchDog
	// PA8 WDI
	//
	// GD32E503_GPIO_CTL1
	// CTL8 [3:2] 0b00 Push-Pull
	// MD8 [1:0] 0b11 Output 50MHz
	REG32_SET_RANGE(GD32E503_GPIOA(GD32E503_GPIO_CTL1), 3, 0, 0b0011);
	
	return 0;
}

int feedHardDog(void)
{
	uint16_t status;
	
	status = REG32_GET(GD32E503_GPIOA(GD32E503_GPIO_ISTAT));
	
	if(status & ((uint16_t)0b1 << 8))
		// Clear to Low
		REG32_SET(GD32E503_GPIOA(GD32E503_GPIO_BOP), (uint32_t)0x00010000 << 8);
	else
		// Set to High
		REG32_SET(GD32E503_GPIOA(GD32E503_GPIO_BOP), (uint32_t)0x0001 << 8);
	
	return 0;
}
